/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "ReenactCallbackData.h"

#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include <mpi.h>

#include <pearl/Callpath.h>
#include <pearl/LocalTrace.h>
#include <pearl/Region.h>
#include <pearl/String.h>

#include "Buffer.h"
#include "Model.h"
#include "silas_events.h"
#include "silas_util.h"

using namespace std;
using namespace silas;


/// @todo[C++11] Replace by use of std::default_delete
struct DeleteObject
{
    template< typename T >
    void
    operator()(T ptr)
    {
        delete ptr;
    }
};


// --- Constructors & destructor --------------------------------------------

CallbackData::CallbackData(const pearl::GlobalDefs& defs,
                           const pearl::LocalTrace& trace,
                           const silas::Model&      model)
    : m_global_defs(defs),
      m_local_trace(const_cast< pearl::LocalTrace& >(trace)),
      m_model(model),
      m_send_buffer(0),
      m_recv_buffer(0),
      m_max_msg_size(std::numeric_limits< int >::max()),
      m_action_list(trace.size(), silas::INIT)
{
    m_delta_t            = 0.;
    m_max_count          = 0;
    m_max_pt2pt_count    = 0;
    m_max_buffered_count = 0;

    m_idle_count      = 0;
    m_aggregated_time = 0.;

    m_action_list_active = false;
    m_event_count        = 0;

    m_cur_nb_recv_count = 0;
    m_max_nb_recv_count = 0;
    m_max_nb_recv_size  = 0;

    m_max_mult_comp_count = 0;

    m_request_array         = 0;
    m_status_array          = 0;
    m_requestmap_iter_array = 0;

    m_noop_threshold = 0.0;
    m_skip_threshold = 0.0;

    reset_action_idx();

    // Set model options
    std::string ntstr = m_model.get_option("noop threshold");
    if (!ntstr.empty())
    {
        std::istringstream is(ntstr);
        is >> m_noop_threshold;
    }
    std::string ststr = m_model.get_option("skip threshold");
    if (!ststr.empty())
    {
        std::istringstream is(ststr);
        is >> m_skip_threshold;
    }
    std::string mmstr = m_model.get_option("max messagesize");
    if (!mmstr.empty())
    {
        std::istringstream is(mmstr);
        is >> m_max_msg_size;
    }
    std::string msstr = m_model.get_option("max sendbufs");
    if (!msstr.empty())
    {
        std::istringstream is(msstr);
        is >> m_max_sendbufs;
    }

    m_max_sendbufs = std::max(m_max_sendbufs, 1);
}


CallbackData::~CallbackData()
{
    delete m_send_buffer;
    delete m_recv_buffer;

    delete[] m_request_array;
    delete[] m_requestmap_iter_array;

    std::for_each(m_nb_recv_buffers.begin(), m_nb_recv_buffers.end(),
                  DeleteObject());
    std::for_each(m_p2p_send_buffers.begin(), m_p2p_send_buffers.end(),
                  DeleteObject());
}


// --- Pre- and postprocessing ----------------------------------------------

void
CallbackData::preprocess(const pearl::Event& event)
{
    set_original_timestamp(event->getTimestamp());
}


void
CallbackData::postprocess(const pearl::Event& event)
{
    set_preceeding_timestamp(m_original_timestamp);
}


const pearl::GlobalDefs&
CallbackData::get_global_defs() const
{
    return m_global_defs;
}


pearl::timestamp_t
CallbackData::get_delta_t() const
{
    return m_delta_t;
}


void
CallbackData::set_delta_t(pearl::timestamp_t timespan)
{
    m_delta_t = timespan;
}


void
CallbackData::mod_delta_t(pearl::timestamp_t timespan)
{
    m_delta_t += timespan;
}


// --- Dummy-buffer handling ------------------------------------------------

void*
CallbackData::get_send_buffer(std::size_t capacity)
{
    return m_send_buffer->get_buffer(capacity);
}


void*
CallbackData::get_recv_buffer(std::size_t capacity)
{
    return m_recv_buffer->get_buffer(capacity);
}


void*
CallbackData::get_p2p_send_buffer(std::size_t capacity)
{
    return get_send_buffer(capacity);
}


void
CallbackData::init_buffers()
{
    size_t count = std::max(m_max_count, m_max_pt2pt_count);

    log("Allocating send buffer of size %d\n", count);

    m_send_buffer = new silas::Buffer(count);

    log("Allocating recv buffer of size %d\n", count);

    m_recv_buffer = new silas::Buffer(count);

    log("Allocating %d nonblocking recv buffers of size %d\n",
        m_max_nb_recv_count, m_max_pt2pt_count);

    m_nb_recv_buffers.reserve(m_max_nb_recv_count);

    for (int i = 0; i < m_max_nb_recv_count; ++i)
    {
        m_nb_recv_buffers.push_back(new silas::Buffer(m_max_pt2pt_count));
    }
}


silas::Buffer*
CallbackData::pop_nb_recv_buffer()
{
    silas::Buffer* buf = m_nb_recv_buffers.back();
    m_nb_recv_buffers.pop_back();

    return buf;
}


void
CallbackData::push_nb_recv_buffer(silas::Buffer* buf)
{
    if (buf)
    {
        m_nb_recv_buffers.push_back(buf);
    }
}


// --- Tracking of send/recv information ------------------------------------

void
CallbackData::track_max_count(int count)
{
    if (count > m_max_count)
    {
        m_max_count = count;
    }
}


void
CallbackData::track_max_pt2pt_count(int count)
{
    if (count > m_max_pt2pt_count)
    {
        m_max_pt2pt_count = count;

        if (count > m_max_count)
        {
            m_max_count = count;
        }
    }
}


int
CallbackData::get_max_count()
{
    return m_max_count;
}


int
CallbackData::get_max_pt2pt_count()
{
    return m_max_pt2pt_count;
}


void
CallbackData::set_max_count(int count)
{
    if (count > m_max_msg_size)
    {
        log("Warning: message size %d exceeds maximum message size (%d).\n"
            "Too large messages will be cut.", count, m_max_msg_size);
        count = m_max_msg_size;
    }

    m_max_count = count;
}


void
CallbackData::set_max_pt2pt_count(int count)
{
    if (count > m_max_msg_size)
    {
        log("Warning: p2p message size %d exceeds maximum message size (%d).\n"
            "Too large will be cut.", count, m_max_msg_size);
        count = m_max_msg_size;
    }

    m_max_pt2pt_count = count;
}


int
CallbackData::get_max_buffered_count()
{
    return m_max_buffered_count;
}


void
CallbackData::track_max_buffered_count(int count)
{
    if (count > m_max_buffered_count)
    {
        m_max_buffered_count = count;
    }
}


void
CallbackData::track_max_mult_comp_count(int count)
{
    if (count > m_max_mult_comp_count)
    {
        m_max_mult_comp_count = count;
    }
}


void
CallbackData::inc_nb_recv_count()
{
    if (++m_cur_nb_recv_count > m_max_nb_recv_count)
    {
        m_max_nb_recv_count = m_cur_nb_recv_count;
    }
}


void
CallbackData::dec_nb_recv_count()
{
    --m_cur_nb_recv_count;
}


// --- Tracking nonblocking communication requests --------------------------

MPI_Request*
CallbackData::get_request_array()
{
    if (!m_request_array && m_max_mult_comp_count)
    {
        m_request_array = new MPI_Request[m_max_mult_comp_count];
    }

    return m_request_array;
}


MPI_Status*
CallbackData::get_status_array()
{
    if (!m_status_array && m_max_mult_comp_count)
    {
        m_status_array = new MPI_Status[m_max_mult_comp_count];
    }

    return m_status_array;
}


CallbackData::requestmap_t::iterator*
CallbackData::get_requestmap_iter_array()
{
    if (!m_requestmap_iter_array && m_max_mult_comp_count)
    {
        m_requestmap_iter_array = new requestmap_t::iterator[m_max_mult_comp_count];
    }

    return m_requestmap_iter_array;
}


// --- Debug routines -------------------------------------------------------

void
CallbackData::clear_diff_vector()
{
    ts_diff.clear();
}


int
CallbackData::get_diff_vector_size()
{
    return ts_diff.size();
}


void
CallbackData::write_ts()
{
    ofstream                    outfile;
    char                        filename[256];
    int                         rank;
    double                      global_abs_err = 0;
    pearl::LocalTrace::iterator event          = m_local_trace.begin();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    sprintf(filename, "diff_%i.txt", rank);
    outfile.open(filename);
    outfile << "# Event Action Type Orig.Diff Sim.Diff Abs.Err Rel.Err\n";
    for (std::vector< pearl::timestamp_t >::size_type i = 0;
         i < orig_ts.size();
         i++)
    {
        double orig_diff = ((i > 0) ? orig_ts[i] - orig_ts[i - 1] : 0);
        double sim_diff  = ((i > 0) ? sim_ts[i] - sim_ts[i - 1] : 0);
        double abs_err   = sim_diff - orig_diff;
        double rel_err   = ((orig_diff != 0) ? (abs_err / orig_diff) : 0);
        global_abs_err += abs_err;

        // *INDENT-OFF*    Uncrustify issues #2279 & #2788
        outfile << i << "\t"
                << m_action_list.at(i) << "\t"
                << event->getType() << "\t"
                << scientific << orig_diff << "\t"
                << scientific << sim_diff << "\t"
                << scientific << abs_err << "\t"
                << scientific << rel_err << "\t"
                << event.get_cnode()->getRegion().getDisplayName().getString() << " in "
                << ((event.get_cnode()->getParent() != NULL)
                    ? event.get_cnode()->getParent()->getRegion().getDisplayName().getCString()
                    : "")
                << "\n";
        ++event;
        // *INDENT-ON*
    }
    outfile << "# Sum Abs.Err.: " << scientific << global_abs_err << "\n";
    outfile.close();
}


void
CallbackData::write_diff_vector()
{
    ofstream outfile;
    char     filename[256];
    int      rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    sprintf(filename, "delta_t_%i.txt", rank);
    outfile.open(filename);
    for (std::vector< pearl::timestamp_t >::size_type i = 0;
         i < ts_diff.size();
         i++)
    {
        outfile << i << "\t" << scientific << ts_diff[i] << "\t" << ((i > 0) ? ts_diff[i] - ts_diff[i - 1] : 0) << "\n";
    }
    outfile.close();
}


void
CallbackData::reset_action_idx()
{
    m_action_idx = m_action_list.begin();
}


void
CallbackData::init_action_list()
{
    m_action_list = action_list_t(m_local_trace.size(), silas::IDLE);
    reset_action_idx();
}


inline bool
CallbackData::is_idle(int action) const
{
    return (  (action == silas::IDLE)
           || (action == silas::NOOP));
}


bool
CallbackData::is_aggregatable(const pearl::Event& event) const
{
    int action = get_action(event);

    return (  (action == silas::IDLE)
           || (action == silas::IDLE_WRITE)
           || (action == silas::NOOP)
           || (action == silas::SKIP)
           || (action == silas::SKIP_END)
           || (action == silas::DELETE));
}


void
CallbackData::track_idle_time(const pearl::Event& event)
{
    /*
     * Only count timespan if event has not been deleted.
     */
    if (get_action(event) != silas::DELETE)
    {
        pearl::Event prev = get_prev_event(event);

        // aggregate time
        m_aggregated_time += event->getTimestamp()
                             - prev->getTimestamp();

        // count aggregated events
        m_idle_count += 1;
    }
}


void
CallbackData::aggregate_idle_time(const pearl::Event& event)
{
    if (!m_model.is_set("aggregate idle"))
    {
        return;
    }

    assert(event.is_valid());

    pearl::Event evt = get_prev_event(event);

    if (m_idle_count > 1)
    {
        interval_t idle_epoch;

        // the last event of the idle epoch is the event preceding the
        // tiggering event.
        idle_epoch.end = evt->getTimestamp();

        while (is_aggregatable(evt))
        {
            if (get_action(evt) != silas::DELETE)
            {
                // redefine action to SKIP
                set_action(silas::SKIP, evt);
            }

            // move to preceeding event
            evt = get_prev_event(evt);
        }

        // save the start of the epoch
        idle_epoch.start = evt->getTimestamp();

        // save complete epoch data for post-processing
        m_idle_epochs.push_back(idle_epoch);

        // get previous event to non-aggragatable event
        evt = get_prev_event(event);

        // rewrite event action
        if (m_aggregated_time > m_noop_threshold)
        {
            set_action(silas::IDLE_WRITE, evt);
        }
        else if (m_aggregated_time > m_skip_threshold)
        {
            set_action(silas::NOOP, evt);
        }
        else
        {
            set_action(silas::SKIP_END, evt);
        }
    }
    else
    {
        if (m_aggregated_time <= m_skip_threshold)
        {
            set_action(silas::SKIP, evt);
        }
        else
        {
            set_action(silas::IDLE_WRITE, evt);
        }
    }

    // reset aggregation variables
    m_idle_count      = 0;
    m_aggregated_time = 0.;
}


void
CallbackData::set_action(const int           action,
                         const pearl::Event& event)
{
    // if event has been deleted, ignore all new settings
    if (get_action(event) == silas::DELETE)
    {
        printf("Event %i has already been deleted. set_action() was ignored.\n",
               event.get_id());

        return;
    }

    m_action_list[event.get_id()] = action;
}


void
CallbackData::reverse_epoch_list()
{
    reverse(m_idle_epochs.begin(), m_idle_epochs.end());
}


void
CallbackData::reverse_ratio_list()
{
    reverse(m_event_ratios.begin(), m_event_ratios.end());
}


unsigned int
CallbackData::get_aggregation_count(const pearl::Event& event) const
{
    pearl::Event evt   = event;
    unsigned int count = 0;

    while (is_aggregatable(evt))
    {
        if (get_action(evt) != silas::DELETE)
        {
            count++;
        }
        --evt;
    }

    return count;
}


pearl::Event
CallbackData::get_prev_event(const pearl::Event& event)
{
    pearl::Event evt = event.prev();
    while (  evt.is_valid()
          && (m_action_list.at(evt.get_id()) == silas::DELETE))
    {
        --evt;
    }

    return evt;
}


void
CallbackData::calculate_timespans(const pearl::Event& event,
                                  pearl::timestamp_t  start,
                                  pearl::timestamp_t  end)
{
    // return, if aggregation not enabled
    if (!m_model.is_set("aggregate idle"))
    {
        return;
    }

    // return, if epoch is too small for aggregation
    if (get_aggregation_count(event) < 2)
    {
        return;
    }

    // retrieve epoch entry
    interval_t         epoch         = m_idle_epochs.back();
    pearl::timestamp_t old_timespan  = epoch.end - epoch.start;
    pearl::timestamp_t new_timespan  = end - start;
    pearl::timestamp_t new_timestamp = end;

    pearl::Event evt  = event;
    pearl::Event prev = get_prev_event(evt);

    pearl::timestamp_t this_old_timestamp = epoch.end;
    pearl::timestamp_t prev_old_timestamp = prev->getTimestamp();

    while (is_aggregatable(prev))
    {
        // calculate new timestamp as offset to current timestamp
        new_timestamp -= (((this_old_timestamp - prev_old_timestamp)
                           / old_timespan) * new_timespan);

        // set the time on event
        prev->setTimestamp(new_timestamp);

        // increment variables
        evt  = prev;
        prev = get_prev_event(evt);

        this_old_timestamp = prev_old_timestamp;
        prev_old_timestamp = prev->getTimestamp();
    }

    // discard processed entry
    m_idle_epochs.pop_back();
}


void
CallbackData::activate_action_list()
{
    m_action_list_active = true;
    m_event_count        = 0;
    reset_action_idx();
}


void
CallbackData::deactivate_action_list()
{
    m_action_list_active = false;
}


bool
TimeIsMinusOne(const pearl::Event_rep* event)
{
    if (event->getTimestamp() == -1)
    {
        delete event;

        return true;
    }

    return false;
}


bool
IsDeleteAction(int action)
{
    return (action == silas::DELETE);
}


void
CallbackData::purge_deleted_events()
{
    std::cout << "m_local_trace.size() = " << m_local_trace.size()
              << "m_action_list.size() = " << m_action_list.size()
              << std::endl;

    pearl::Event del_start =
        m_local_trace.remove_if(TimeIsMinusOne);
    m_local_trace.erase(del_start, m_local_trace.end());

    action_list_t::iterator del_action =
        remove_if(m_action_list.begin(), m_action_list.end(),
                  IsDeleteAction);
    m_action_list.erase(del_action, m_action_list.end());

    std::cout << "m_local_trace.size() = " << m_local_trace.size()
              << "m_action_list.size() = " << m_action_list.size()
              << std::endl;
}
