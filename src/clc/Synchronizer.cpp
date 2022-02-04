/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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

#include "Synchronizer.h"

#include <algorithm>
#include <cfloat>
#include <cstdio>

#include <UTILS_Error.h>

#include <pearl/CommSet.h>
#include <pearl/Location.h>
#include <pearl/MpiCollEnd_rep.h>
#include <pearl/MpiComm.h>
#include <pearl/Region.h>
#include <pearl/String.h>
#include <pearl/pearl_replay.h>

#include "AmortData.h"
#include "Clock.h"
#include "FwdAmortData.h"
#include "IntervalStat.h"
#include "Latency.h"
#include "Network.h"
#include "Operator.h"
#include "Predicates.h"
#include "RuntimeStat.h"
#include "TimeStat.h"

#ifdef _OPENMP
    #include "OmpCommunication.h"
    #include "OmpData.h"
#endif    // _OPENMP

using namespace std;
using namespace pearl;


// --- Constructors & destructor --------------------------------------------

Synchronizer::Synchronizer(const int           rank,
                           const LocalTrace&   trace,
                           TimeStat* const     timeStat,
                           IntervalStat* const intervalStat)
    : trace(trace),
      firstEvtT(-DBL_MAX),
      max_error(0.01),
      m_max_slope(0.01),
      threshold(0.05),
      m_rank(rank),
      num_sends(0),
      t_stat(timeStat),
      i_stat(intervalStat),
      tsa(0)
{
    // Get location
    const Location& loc = trace.get_location();
    m_loc.machine = loc.getMachineId();
    m_loc.node    = loc.getNodeId();
    m_loc.process = loc.getRank();
    m_loc.thread  = loc.getThreadId();

    // Create Latency
    Latency* latency = new Latency(0.0, 1.0e-6, 1.0e-6, 1.0e-6, 1.0e-6, 0.01e-6);

    // Create Network with Latency
    m_network = new Network(m_loc, latency);

    // Create clock
    m_clc = new Clock;

    // Create runtime statistics
    r_stat = new RuntimeStat(1);

    // Initialize operator and datatypes
    #pragma omp master
    {
        init_operators();
    }
}


Synchronizer::~Synchronizer()
{
    delete m_clc;
    delete m_network;
    delete r_stat;

    delete[] tsa;

    delete_requests();
    m_violations.clear();
    #pragma omp master
    {
        free_operators();
    }
}


// --- Timestamp correction -------------------------------------------------

/**
 *  Used to synchronize event stream
 **/
void
Synchronizer::synchronize()
{
    forward_amortization();
    backward_amortization();
}


void
Synchronizer::forward_amortization()
{
    timestamp_t gl_error = -DBL_MAX;

    if ((t_stat != 0) && (i_stat != 0))
    {
        t_stat->get_org_timestamps();
        i_stat->get_org_timestamps();
    }

    // Set up callback manager
    CallbackManager* cbmanager = new CallbackManager;
    cbmanager->register_callback(GROUP_LEAVE, PEARL_create_callback(this, &Synchronizer::amortize_fwd_leave));
    cbmanager->register_callback(GROUP_SEND, PEARL_create_callback(this, &Synchronizer::amortize_fwd_send));
    cbmanager->register_callback(GROUP_RECV, PEARL_create_callback(this, &Synchronizer::amortize_fwd_recv));
    cbmanager->register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &Synchronizer::amortize_fwd_coll));
    #ifdef _OPENMP
        cbmanager->register_callback(THREAD_FORK, PEARL_create_callback(this, &Synchronizer::amortize_fwd_fork));
        cbmanager->register_callback(THREAD_TEAM_BEGIN, PEARL_create_callback(this, &Synchronizer::amortize_fwd_ttb));
        cbmanager->register_callback(THREAD_TEAM_END, PEARL_create_callback(this, &Synchronizer::amortize_fwd_tte));
        cbmanager->register_callback(THREAD_JOIN, PEARL_create_callback(this, &Synchronizer::amortize_fwd_join));
    #endif    // _OPENMP

    // Perform the replay
    do
    {
        // Prepare Synchronizer for subsequent replay
        prepare_replay();

        // Replay the target application and synchronize timestamps...
        {
            // FCCpx requires block
            #pragma omp barrier
        }
        FwdAmortData data(this);
        PEARL_forward_replay(trace, *cbmanager, &data);
        #pragma omp barrier

        // Until accuracy is sufficient
        gl_error = r_stat->get_clock_error();
    }
    while (  (gl_error > threshold)
          && r_stat->next_pass());

    // Release callback manager
    delete cbmanager;
}


void
Synchronizer::backward_amortization()
{
    // If clock condition violations occured -> apply backward amortization
    timestamp_t gl_violations = 0.0;
    timestamp_t violations    = (timestamp_t)m_clc->get_num_viol();
    #ifdef _OPENMP
        OMP_Allreduce_sum(violations, omp_global_sum_timestamp);
        violations = omp_global_sum_timestamp;
    #endif    // _OPENMP

    #pragma omp master
    {
        MPI_Allreduce(&violations, &gl_violations, 1, MPI_DOUBLE,
                      MPI_SUM,
                      MPI_COMM_WORLD);
    }

    #ifdef _OPENMP
        OMP_Bcast(gl_violations, omp_global_sum_timestamp);
        gl_violations = omp_global_sum_timestamp;
    #endif    // _OPENMP

    if ((t_stat != 0) && (i_stat != 0))
    {
        t_stat->get_fa_timestamps();
        i_stat->get_fa_timestamps();
    }

    if (gl_violations > 0.0)
    {
        #pragma omp barrier
        ba_amortize();
        #pragma omp barrier
    }

    if ((t_stat != 0) && (i_stat != 0))
    {
        t_stat->get_ba_timestamps();
        i_stat->get_ba_timestamps();
    }

    // Release timestamp array
    delete[] tsa;
    tsa = 0;
}


// --- Statistics -----------------------------------------------------------

/**
 *  Print some fancy runtime statistics :-).
 */
void
Synchronizer::print_statistics()
{
    timestamp_t gl_corr       = r_stat->get_num_clock_corr();
    timestamp_t gl_max_error  = r_stat->get_max_clock_error();
    timestamp_t gl_error      = r_stat->get_clock_error();
    timestamp_t gl_num        = 0.0;
    timestamp_t num           = (timestamp_t)trace.num_events();
    timestamp_t gl_violations = 0.0;
    timestamp_t violations    = (timestamp_t)m_clc->get_num_viol();
    timestamp_t gl_max_slope  = 0.0;

    int sum_p2p_violation  = 0;
    int p2p_violation      = m_clc->get_p2p_viol();
    int sum_coll_violation = 0;
    int coll_violation     = m_clc->get_coll_viol();
    int sum_omp_violation  = 0;
    int omp_violation      = m_clc->get_omp_viol();

    #ifdef _OPENMP
        OMP_Allreduce_sum(num, omp_global_sum_timestamp);
        num = omp_global_sum_timestamp;
        OMP_Allreduce_sum(violations, omp_global_sum_timestamp);
        violations = omp_global_sum_timestamp;
        OMP_Allreduce_max(m_max_slope, omp_global_max_timestamp);
        m_max_slope = omp_global_max_timestamp;
        OMP_Allreduce_sum(p2p_violation, omp_global_sum_count);
        p2p_violation = omp_global_sum_count;
        OMP_Allreduce_sum(coll_violation, omp_global_sum_count);
        coll_violation = omp_global_sum_count;
        OMP_Allreduce_sum(omp_violation, omp_global_sum_count);
        omp_violation = omp_global_sum_count;
    #endif    // _OPENMP

    #pragma omp master
    {
        MPI_Allreduce(&num, &gl_num, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&violations, &gl_violations, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&m_max_slope, &gl_max_slope, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        MPI_Allreduce(&p2p_violation, &sum_p2p_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&coll_violation, &sum_coll_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&omp_violation, &sum_omp_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        // Print statistics only on master rank
        if (m_rank == 0)
        {
            printf("\n\t# passes        : %d\n", r_stat->get_pass());
            printf("\t# violated      : %.0f\n", gl_violations);
            if (gl_violations > 0.0)
            {
                printf("\t# corrected     : %.0f\n", gl_corr);
                printf("\t# reversed-p2p  : %d\n", sum_p2p_violation);
                printf("\t# reversed-coll : %d\n", sum_coll_violation);
                printf("\t# reversed-omp  : %d\n", sum_omp_violation);
                printf("\t# events        : %.0f\n", gl_num);
                printf("\tmax. error      : %.6f [s]\n", gl_max_error);
                printf("\terror at final. : %.6f [%%] \n", gl_error * 100);
                printf("\tMax slope       : %.9f  \n", gl_max_slope);
            }
        }
    }
}


// --- Callback functions for forward amortization --------------------------

void
Synchronizer::amortize_fwd_leave(const CallbackManager& cbManager,
                                 const int              userEvent,
                                 const Event&           event,
                                 CallbackData* const    cbData)
{
    FwdAmortData* data = static_cast< FwdAmortData* >(cbData);

    const Region& region = event->getRegion();
    #ifdef _OPENMP
        // OpenMP barriers are much more likely to occur than MPI_Init(_thread)
        // and MPI_Finalize, thus handle them first
        if (is_omp_barrier(region))
        {
            amortize_fwd_omp_barrier(event, data);

            return;
        }
    #endif    // _OPENMP

    if (  is_mpi_init(region)
       || is_mpi_finalize(region))
    {
        amortize_fwd_mpi_init_finalize(event, data);

        return;
    }
}


void
Synchronizer::amortize_fwd_send(const CallbackManager& cbManager,
                                const int              userEvent,
                                const Event&           event,
                                CallbackData* const    cbData)
{
    FwdAmortData* data = static_cast< FwdAmortData* >(cbData);

    // Apply controlled logical clock (internal amortization)
    fa_amortize_intern(event, data);

    // Send amortized SEND event timestamp
    amortization_data* local = new amortization_data;
    *local = pack_data(m_loc, event->getTimestamp());

    MPI_Request request;
    MPI_Isend(local, 1, AMORT_DATA,
              event->getDestination(),
              event->getTag(),
              event->getComm()->getHandle(),
              &request);

    m_requests.push_back(request);
    m_data.push_back(local);
    update_requests();
}


void
Synchronizer::amortize_fwd_recv(const CallbackManager& cbManager,
                                const int              userEvent,
                                const Event&           event,
                                CallbackData* const    cbData)
{
    FwdAmortData* data = static_cast< FwdAmortData* >(cbData);

    // Receive amortized SEND event timestamp
    amortization_data ex_object;
    MPI_Status        status;
    MPI_Recv(&ex_object, 1, AMORT_DATA,
             event->getSource(),
             event->getTag(),
             event->getComm()->getHandle(),
             &status);

    clc_location      ex_loc;
    const timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);

    // Apply controlled logical clock (receive amortization)
    fa_amortize_recv(event, sendEvtT, ex_loc, CLC_P2P, data);
}


void
Synchronizer::amortize_fwd_coll(const CallbackManager& cbManager,
                                const int              userEvent,
                                const Event&           event,
                                CallbackData* const    cbData)
{
    FwdAmortData* data = static_cast< FwdAmortData* >(cbData);

    // Handle MPI_COLLECTIVE_END events on self-like communicators as
    // "internal event"
    if (event->getComm()->getSize() == 1)
    {
        return;
    }

    const MpiCollEnd_rep& endRep = event_cast< MpiCollEnd_rep >(*event);
    switch (endRep.getCollType())
    {
        // CASE 1: Barrier
        case MpiCollEnd_rep::BARRIER:
            amortize_fwd_mpi_barrier(event, data);
            break;

        // CASE 2: 1-to-N communication
        case MpiCollEnd_rep::BCAST:
        case MpiCollEnd_rep::SCATTER:
        case MpiCollEnd_rep::SCATTERV:
            amortize_fwd_mpi_12n(event, data);
            break;

        // CASE 3: N-to-N communication
        case MpiCollEnd_rep::ALLGATHER:
        case MpiCollEnd_rep::ALLGATHERV:
        case MpiCollEnd_rep::ALLTOALL:
        case MpiCollEnd_rep::ALLREDUCE:
        case MpiCollEnd_rep::REDUCE_SCATTER:
        case MpiCollEnd_rep::REDUCE_SCATTER_BLOCK:
            amortize_fwd_mpi_n2n(event, data);
            break;

        // CASE 4: N-to-1 communication
        case MpiCollEnd_rep::GATHER:
        case MpiCollEnd_rep::GATHERV:
        case MpiCollEnd_rep::REDUCE:
            amortize_fwd_mpi_n21(event, data);
            break;

        // CASE 5: Prefix reduction
        case MpiCollEnd_rep::SCAN:
        case MpiCollEnd_rep::EXSCAN:
            amortize_fwd_mpi_scan(event, data);
            break;

        // SPECIAL CASE:
        // MPI_Alltoallv and MPI_Alltoallw can be used to implement arbitrary
        // communication patterns (e.g., pair-wise message exchange).  However,
        // the trace data currently does not provide enough information for a
        // proper reconstruction, thus, ignore those calls and handle their
        // MPI_COLLECTIVE_END events as "internal event".
        case MpiCollEnd_rep::ALLTOALLV:
        case MpiCollEnd_rep::ALLTOALLW:
            return;

        default:
            UTILS_BUG("Unknown MPI collective operation encountered");
            break;
    }
}


void
Synchronizer::amortize_fwd_fork(const CallbackManager& cbManager,
                                const int              userEvent,
                                const Event&           event,
                                CallbackData* const    cbData)
{
    #ifdef _OPENMP
        FwdAmortData* data = static_cast< FwdAmortData* >(cbData);

        // Apply controlled logical clock (internal amortization)
        fa_amortize_intern(event, data);

        // Send amortized THREAD_FORK event timestamp via shared variable
        omp_global_max_timestamp = event->getTimestamp();

        // Increment OpenMP parallel region nesting level
        omp_global_nest_level++;
    #endif    // _OPENMP
}


void
Synchronizer::amortize_fwd_ttb(const CallbackManager& cbManager,
                               const int              userEvent,
                               const Event&           event,
                               CallbackData* const    cbData)
{
    #ifdef _OPENMP
        FwdAmortData* data = static_cast< FwdAmortData* >(cbData);

        // Receive amortized THREAD_FORK event timestamp via shared variable
        #pragma omp barrier
        const timestamp_t sendEvtT = omp_global_max_timestamp;

        // Apply controlled logical clock (receive amortization)
        fa_amortize_recv(event, sendEvtT, m_loc, CLC_OMP, data);
    #endif    // _OPENMP
}


void
Synchronizer::amortize_fwd_tte(const CallbackManager& cbManager,
                               const int              userEvent,
                               const Event&           event,
                               CallbackData* const    cbData)
{
    #ifdef _OPENMP
        FwdAmortData* data = static_cast< FwdAmortData* >(cbData);

        // Apply controlled logical clock (internal amortization)
        fa_amortize_intern(event, data);

        // Send max( amortized THREAD_TEAM_END event timestamps )
        // via shared variable
        OMP_Allreduce_max(event->getTimestamp(), omp_global_max_timestamp);
    #endif    // _OPENMP
}


void
Synchronizer::amortize_fwd_join(const CallbackManager& cbManager,
                                const int              userEvent,
                                const Event&           event,
                                CallbackData* const    cbData)
{
    #ifdef _OPENMP
        FwdAmortData* data = static_cast< FwdAmortData* >(cbData);

        // Receive max( amortized THREAD_TEAM_END event timestamps )
        // via shared variable
        const timestamp_t sendEvtT = omp_global_max_timestamp;

        // Apply controlled logical clock (receive amortization)
        fa_amortize_recv(event, sendEvtT, m_loc, CLC_OMP, data);

        // Decrement OpenMP parallel region nesting level
        omp_global_nest_level--;
    #endif    // _OPENMP
}


// --- Callback functions for backward amortization -------------------------

void
Synchronizer::amortize_bkwd_leave(const CallbackManager& cbManager,
                                  const int              userEvent,
                                  const Event&           event,
                                  CallbackData* const    cbData)
{
    const Region& region = event->getRegion();
    #ifdef _OPENMP
        // OpenMP barriers are much more likely to occur than MPI_Init(_thread)
        // and MPI_Finalize, thus handle them first
        if (is_omp_barrier(region))
        {
            amortize_bkwd_omp_barrier(event);

            return;
        }
    #endif    // _OPENMP

    if (  is_mpi_init(region)
       || is_mpi_finalize(region))
    {
        amortize_bkwd_mpi_init_finalize(event);

        return;
    }
}


void
Synchronizer::amortize_bkwd_send(const CallbackManager& cbManager,
                                 const int              userEvent,
                                 const Event&           event,
                                 CallbackData* const    cbData)
{
    // Receive RECV event timestamp
    amortization_data ex_object;
    MPI_Status        status;
    MPI_Recv(&ex_object, 1, AMORT_DATA,
             event->getDestination(),
             event->getTag(),
             event->getComm()->getHandle(),
             &status);

    clc_location      ex_loc;
    const timestamp_t recvEvtT = unpack_data(ex_loc, ex_object);

    // Calculate maximum allowed SEND event timestamp based on received
    // timestamp and latency
    tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_P2P);
    num_sends++;
}


void
Synchronizer::amortize_bkwd_recv(const CallbackManager& cbManager,
                                 const int              userEvent,
                                 const Event&           event,
                                 CallbackData* const    cbData)
{
    // Send RECV event timestamp
    amortization_data* local = new amortization_data;
    *local = pack_data(m_loc, event->getTimestamp());

    MPI_Request request;
    MPI_Isend(local, 1, AMORT_DATA,
              event->getSource(),
              event->getTag(),
              event->getComm()->getHandle(),
              &request);

    m_requests.push_back(request);
    m_data.push_back(local);
    update_requests();
}


void
Synchronizer::amortize_bkwd_coll(const CallbackManager& cbManager,
                                 const int              userEvent,
                                 const Event&           event,
                                 CallbackData* const    cbData)
{
    // Handle MPI_COLLECTIVE_END events on self-like communicators as
    // "internal event"
    if (event->getComm()->getSize() == 1)
    {
        return;
    }

    const MpiCollEnd_rep& endRep = event_cast< MpiCollEnd_rep >(*event);
    switch (endRep.getCollType())
    {
        // CASE 1: Barrier
        case MpiCollEnd_rep::BARRIER:
            amortize_bkwd_mpi_barrier(event);
            break;

        // CASE 2: 1-to-N communication
        case MpiCollEnd_rep::BCAST:
        case MpiCollEnd_rep::SCATTER:
        case MpiCollEnd_rep::SCATTERV:
            amortize_bkwd_mpi_12n(event);
            break;

        // CASE 3: N-to-N communication
        case MpiCollEnd_rep::ALLGATHER:
        case MpiCollEnd_rep::ALLGATHERV:
        case MpiCollEnd_rep::ALLTOALL:
        case MpiCollEnd_rep::ALLREDUCE:
        case MpiCollEnd_rep::REDUCE_SCATTER:
        case MpiCollEnd_rep::REDUCE_SCATTER_BLOCK:
            amortize_bkwd_mpi_n2n(event);
            break;

        // CASE 4: N-to-1 communication
        case MpiCollEnd_rep::GATHER:
        case MpiCollEnd_rep::GATHERV:
        case MpiCollEnd_rep::REDUCE:
            amortize_bkwd_mpi_n21(event);
            break;

        // CASE 5: Prefix reduction
        case MpiCollEnd_rep::SCAN:
        case MpiCollEnd_rep::EXSCAN:
            amortize_bkwd_mpi_scan(event);
            break;

        // SPECIAL CASE:
        // MPI_Alltoallv and MPI_Alltoallw can be used to implement arbitrary
        // communication patterns (e.g., pair-wise message exchange).  However,
        // the trace data currently does not provide enough information for a
        // proper reconstruction, thus, ignore those calls and handle their
        // MPI_COLLECTIVE_END events as "internal event".
        case MpiCollEnd_rep::ALLTOALLV:
        case MpiCollEnd_rep::ALLTOALLW:
            return;

        default:
            UTILS_BUG("Unknown MPI collective operation encountered");
            break;
    }
}


void
Synchronizer::amortize_bkwd_fork(const CallbackManager& cbManager,
                                 const int              userEvent,
                                 const Event&           event,
                                 CallbackData* const    cbData)
{
    #ifdef _OPENMP
        // Receive min( THREAD_TEAM_BEGIN event timestamps ) via shared variable
        const timestamp_t recvEvtT = omp_global_min_timestamp;

        // Calculate maximum allowed THREAD_FORK event timestamp based on
        // received timestamp and latency
        clc_location ex_loc;
        tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_OMP);
        num_sends++;

        // Decrement OpenMP parallel region nesting level
        omp_global_nest_level--;
    #endif    // _OPENMP
}


void
Synchronizer::amortize_bkwd_ttb(const CallbackManager& cbManager,
                                const int              userEvent,
                                const Event&           event,
                                CallbackData* const    cbData)
{
    #ifdef _OPENMP
        // Send min( THREAD_TEAM_BEGIN event timestamps ) via shared variable
        OMP_Allreduce_min(event->getTimestamp(), omp_global_min_timestamp);
    #endif    // _OPENMP
}


void
Synchronizer::amortize_bkwd_tte(const CallbackManager& cbManager,
                                const int              userEvent,
                                const Event&           event,
                                CallbackData* const    cbData)
{
    #ifdef _OPENMP
        // Receive THREAD_JOIN event timestamp via shared variable
        #pragma omp barrier
        const timestamp_t recvEvtT = omp_global_min_timestamp;

        // Calculate maximum allowed THREAD_TEAM_END event timestamp based on
        // received timestamp and latency
        clc_location ex_loc;
        tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_OMP);
        num_sends++;
    #endif    // _OPENMP
}


void
Synchronizer::amortize_bkwd_join(const CallbackManager& cbManager,
                                 const int              userEvent,
                                 const Event&           event,
                                 CallbackData* const    cbData)
{
    #ifdef _OPENMP
        // Send THREAD_JOIN event timestamp via shared variable
        omp_global_min_timestamp = event->getTimestamp();

        // Increment OpenMP parallel region nesting level
        omp_global_nest_level++;
    #endif    // _OPENMP
}


// --- Forward amortization functions ---------------------------------------

void
Synchronizer::amortize_fwd_omp_barrier(const Event&        event,
                                       FwdAmortData* const data)
{
    #ifdef _OPENMP
        // Handle LEAVE events of OpenMP barriers in serial context as
        // "internal event" -- otherwise the algorithm will deadlock
        if (omp_global_nest_level == 0)
        {
            return;
        }

        // Receive max( amortized ENTER event timestamps ) via shared variable
        const Event enter = event.enterptr();

        OMP_Allreduce_max(enter->getTimestamp(), omp_global_max_timestamp);

        const timestamp_t sendEvtT = omp_global_max_timestamp;

        // Apply controlled logical clock (receive amortization)
        fa_amortize_recv(event, sendEvtT, m_loc, CLC_OMP, data);
    #endif    // _OPENMP
}


void
Synchronizer::amortize_fwd_mpi_init_finalize(const Event&        event,
                                             FwdAmortData* const data)
{
    // Receive max( amortized ENTER event timestamps )
    const Event       enter        = event.enterptr();
    amortization_data local_object = pack_data(m_loc, enter->getTimestamp());

    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MAX,
                  MPI_COMM_WORLD);

    clc_location      ex_loc;
    const timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);

    // Apply controlled logical clock (receive amortization)
    fa_amortize_recv(event, sendEvtT, ex_loc, CLC_COLL, data);
}


void
Synchronizer::amortize_fwd_mpi_barrier(const Event&        event,
                                       FwdAmortData* const data)
{
    // Receive max( amortized MPI_COLLECTIVE_BEGIN event timestamps )
    const Event       begin        = event.beginptr();
    amortization_data local_object = pack_data(m_loc, begin->getTimestamp());

    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MAX,
                  event->getComm()->getHandle());

    clc_location      ex_loc;
    const timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);

    // Apply controlled logical clock (receive amortization)
    fa_amortize_recv(event, sendEvtT, ex_loc, CLC_COLL, data);
}


void
Synchronizer::amortize_fwd_mpi_12n(const Event&        event,
                                   FwdAmortData* const data)
{
    const bool onRootRank = isOnRoot(event, m_rank);

    // Receive amortized MPI_COLLECTIVE_BEGIN event timestamp of root rank.
    // If no payload is transferred (i.e., the call is not synchronizing), use
    // a timestamp that is guaranteed to not trigger a receive amortization.
    timestamp_t       sendEvtT;
    amortization_data local_object;
    if (onRootRank)
    {
        const Event begin = event.beginptr();
        sendEvtT = begin->getTimestamp();
        if (event->getBytesSent() == 0)
        {
            sendEvtT = -DBL_MAX;
        }

        local_object = pack_data(m_loc, sendEvtT);
    }

    MPI_Bcast(&local_object, 1, AMORT_DATA,
              event->getRoot(),
              event->getComm()->getHandle());

    // Apply controlled logical clock (receive amortization) on non-root ranks
    // that receive payload
    if (  !onRootRank
       && (event->getBytesReceived() > 0))
    {
        clc_location ex_loc;
        sendEvtT = unpack_data(ex_loc, local_object);

        fa_amortize_recv(event, sendEvtT, ex_loc, CLC_COLL, data);
    }
}


void
Synchronizer::amortize_fwd_mpi_n2n(const Event&        event,
                                   FwdAmortData* const data)
{
    // Receive max( amortized MPI_COLLECTIVE_BEGIN event timestamps ).
    // If no payload is transferred (i.e., the call is not synchronizing), use
    // a timestamp that is guaranteed to not trigger a receive amortization.
    const Event begin    = event.beginptr();
    timestamp_t sendEvtT = begin->getTimestamp();
    if (event->getBytesSent() == 0)
    {
        sendEvtT = -DBL_MAX;
    }

    amortization_data local_object = pack_data(m_loc, sendEvtT);

    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MAX,
                  event->getComm()->getHandle());

    clc_location ex_loc;
    sendEvtT = unpack_data(ex_loc, ex_object);
    if (event->getBytesReceived() == 0)
    {
        sendEvtT = -DBL_MAX;
    }

    // Apply controlled logical clock (receive amortization)
    fa_amortize_recv(event, sendEvtT, ex_loc, CLC_COLL, data);
}


void
Synchronizer::amortize_fwd_mpi_n21(const Event&        event,
                                   FwdAmortData* const data)
{
    // Receive max( amortized MPI_COLLECTIVE_BEGIN event timestamps ) on root
    // rank.
    // If no payload is transferred (i.e., the call is not synchronizing), use
    // a timestamp that is guaranteed to not trigger a receive amortization.
    const Event begin    = event.beginptr();
    timestamp_t sendEvtT = begin->getTimestamp();
    if (event->getBytesSent() == 0)
    {
        sendEvtT = -DBL_MAX;
    }

    amortization_data local_object = pack_data(m_loc, sendEvtT);

    amortization_data ex_object;
    MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA,
               CLC_MAX,
               event->getRoot(),
               event->getComm()->getHandle());

    // Apply controlled logical clock (receive amortization) on root rank
    // if it is receiving any payload
    if (  isOnRoot(event, m_rank)
       && (event->getBytesReceived() > 0))
    {
        clc_location ex_loc;
        sendEvtT = unpack_data(ex_loc, ex_object);

        fa_amortize_recv(event, sendEvtT, ex_loc, CLC_COLL, data);
    }
}


void
Synchronizer::amortize_fwd_mpi_scan(const Event&        event,
                                    FwdAmortData* const data)
{
    // Receive
    // max( amortized MPI_COLLECTIVE_BEGIN event timestamps of lower ranks ).
    // If no payload is transferred (i.e., the call is not synchronizing), use
    // a timestamp that is guaranteed to not trigger a receive amortization.
    const Event begin    = event.beginptr();
    timestamp_t sendEvtT = begin->getTimestamp();
    if (event->getBytesSent() == 0)
    {
        sendEvtT = -DBL_MAX;
    }

    amortization_data local_object = pack_data(m_loc, sendEvtT);

    amortization_data ex_object;
    MPI_Scan(&local_object, &ex_object, 1, AMORT_DATA,
             CLC_MAX,
             event->getComm()->getHandle());

    clc_location ex_loc;
    sendEvtT = unpack_data(ex_loc, ex_object);
    if (event->getBytesReceived() == 0)
    {
        sendEvtT = -DBL_MAX;
    }

    // Apply controlled logical clock (receive amortization)
    fa_amortize_recv(event, sendEvtT, ex_loc, CLC_COLL, data);
}


/**
 *  Amortization for receive events. Including both replays.
 */
void
Synchronizer::fa_amortize_recv(const Event&        event,
                               const timestamp_t   sendEvtT,
                               clc_location&       ex_loc,
                               const clc_event_t   event_type,
                               FwdAmortData* const data)
{
    // Timestamp before any amortization
    timestamp_t curT = event->getTimestamp();

    // Amortize forward
    if (firstEvtT == -DBL_MAX)
    {
        // Store first event timestamp for use in backward amortization
        firstEvtT = curT;

        // Set initial clock value
        m_clc->set_value(firstEvtT - m_clc->get_delta());

        // Set initial previous event
        m_clc->set_prev(firstEvtT - m_clc->get_delta());
    }

    // Timestamp after internal forward amortization
    timestamp_t latency = m_network->get_latency(ex_loc, event_type);
    timestamp_t internT = m_clc->amortize_forward_recv(curT, sendEvtT, latency, event_type);

    // Set new timestamp
    event->setTimestamp(m_clc->get_value());

    // Store event in case of clock condition violation
    if (internT < m_clc->get_value())
    {
        m_violations[event] = internT;
    }

    // Adjust statistics
    r_stat->set_statistics(curT, m_clc->get_value());

    // Suppress implicit internal amortization
    data->setAmortized();
}


/**
 *  Amortization for internal events. Only Controlled logical clock is
 *  locally applied.
 */
void
Synchronizer::fa_amortize_intern(const Event&        event,
                                 FwdAmortData* const data)
{
    // Timestamp before any amortization
    timestamp_t curT = event->getTimestamp();

    if (firstEvtT == -DBL_MAX)
    {
        // Store first event timestamp for use in backward amortization
        firstEvtT = curT;

        // Set initial clock value
        m_clc->set_value(curT);

        // Set initial previous event
        m_clc->set_prev(curT);
    }
    else
    {
        // Amortize timestamp internally
        m_clc->amortize_forward_intern(curT);
    }

    // Set new timestamp
    event->setTimestamp(m_clc->get_value());

    // Adjust statistics and timestamp
    r_stat->set_statistics(curT, m_clc->get_value());

    // Suppress implicit internal amortization
    data->setAmortized();
}


// --- Backward amortization functions --------------------------------------

void
Synchronizer::amortize_bkwd_omp_barrier(const Event& event)
{
    #ifdef _OPENMP
        // Handle LEAVE events of OpenMP barriers in serial context as
        // "internal event" -- otherwise the algorithm will deadlock
        if (omp_global_nest_level == 0)
        {
            return;
        }

        // Receive min( LEAVE event timestamps ) via shared variable
        OMP_Allreduce_min(event->getTimestamp(), omp_global_min_timestamp);

        const timestamp_t recvEvtT = omp_global_min_timestamp;

        // Calculate maximum allowed ENTER event timestamp based on
        // received timestamp and latency
        clc_location ex_loc;
        tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_OMP);
        num_sends++;
    #endif    // _OPENMP
}


void
Synchronizer::amortize_bkwd_mpi_init_finalize(const Event& event)
{
    // Receive min( LEAVE event timestamps )
    amortization_data local_object = pack_data(m_loc, event->getTimestamp());

    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MIN,
                  MPI_COMM_WORLD);

    clc_location      ex_loc;
    const timestamp_t recvEvtT = unpack_data(ex_loc, ex_object);

    // Calculate maximum allowed ENTER event timestamp based on
    // received timestamp and latency
    tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_COLL);
    num_sends++;
}


void
Synchronizer::amortize_bkwd_mpi_barrier(const Event& event)
{
    // Receive min( MPI_COLLECTIVE_END event timestamps )
    amortization_data local_object = pack_data(m_loc, event->getTimestamp());

    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MIN,
                  event->getComm()->getHandle());

    clc_location      ex_loc;
    const timestamp_t recvEvtT = unpack_data(ex_loc, ex_object);

    // Calculate maximum allowed MPI_COLLECTIVE_BEGIN event timestamp based on
    // received timestamp and latency
    tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_COLL);
    num_sends++;
}


void
Synchronizer::amortize_bkwd_mpi_12n(const Event& event)
{
    // Receive min( MPI_COLLECTIVE_END event timestamps ) on root rank.
    // If no payload is transferred (i.e., the call is not synchronizing),
    // use a timestamp that is guaranteed to not trigger the piecewise
    // linear interpolation.
    timestamp_t recvEvtT = event->getTimestamp();
    if (event->getBytesReceived() == 0)
    {
        recvEvtT = DBL_MAX;
    }

    amortization_data local_object = pack_data(m_loc, recvEvtT);

    amortization_data ex_object;
    MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA,
               CLC_MIN,
               event->getRoot(),
               event->getComm()->getHandle());

    // Calculate maximum allowed MPI_COLLECTIVE_BEGIN event timestamp on root
    // rank based on received timestamp and latency
    if (isOnRoot(event, m_rank))
    {
        clc_location ex_loc;
        recvEvtT = unpack_data(ex_loc, ex_object);
        if (event->getBytesSent() == 0)
        {
            recvEvtT = DBL_MAX;
        }

        tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_COLL);
        num_sends++;
    }
}


void
Synchronizer::amortize_bkwd_mpi_n2n(const Event& event)
{
    // Receive min( MPI_COLLECTIVE_END event timestamps ).
    // If no payload is transferred (i.e., the call is not synchronizing),
    // use a timestamp that is guaranteed to not trigger the piecewise
    // linear interpolation.
    timestamp_t recvEvtT = event->getTimestamp();
    if (event->getBytesReceived() == 0)
    {
        recvEvtT = DBL_MAX;
    }

    amortization_data local_object = pack_data(m_loc, recvEvtT);

    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MIN,
                  event->getComm()->getHandle());

    clc_location ex_loc;
    recvEvtT = unpack_data(ex_loc, ex_object);
    if (event->getBytesSent() == 0)
    {
        recvEvtT = DBL_MAX;
    }

    // Calculate maximum allowed MPI_COLLECTIVE_BEGIN event timestamp
    // based on received timestamp and latency
    tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_COLL);
    num_sends++;
}


void
Synchronizer::amortize_bkwd_mpi_n21(const Event& event)
{
    // Receive MPI_COLLECTIVE_END event timestamp of root rank.
    // If no payload is transferred (i.e., the call is not synchronizing),
    // use a timestamp that is guaranteed to not trigger the piecewise
    // linear interpolation.
    timestamp_t recvEvtT = event->getTimestamp();
    if (event->getBytesReceived() == 0)
    {
        recvEvtT = DBL_MAX;
    }

    amortization_data local_object = pack_data(m_loc, recvEvtT);

    MPI_Bcast(&local_object, 1, AMORT_DATA,
              event->getRoot(),
              event->getComm()->getHandle());

    // Calculate maximum allowed MPI_COLLECTIVE_BEGIN event timestamp on
    // non-root ranks based on received timestamp and latency
    if (!isOnRoot(event, m_rank))
    {
        clc_location ex_loc;
        recvEvtT = unpack_data(ex_loc, local_object);
        if (event->getBytesSent() == 0)
        {
            recvEvtT = DBL_MAX;
        }

        tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_COLL);
        num_sends++;
    }
}


void
Synchronizer::amortize_bkwd_mpi_scan(const Event& event)
{
    // Receive min( MPI_COLLECTIVE_END event timestamps of higher ranks )
    // If no payload is transferred (i.e., the call is not synchronizing),
    // use a timestamp that is guaranteed to not trigger the piecewise
    // linear interpolation.
    timestamp_t recvEvtT = event->getTimestamp();
    if (event->getBytesReceived() == 0)
    {
        recvEvtT = DBL_MAX;
    }

    amortization_data local_object = pack_data(m_loc, recvEvtT);

    amortization_data ex_object;
    MPI_Comm          reversed_comm = getReverseComm(event->getComm());
    MPI_Scan(&local_object, &ex_object, 1, AMORT_DATA,
             CLC_MIN,
             reversed_comm);

    clc_location ex_loc;
    recvEvtT = unpack_data(ex_loc, ex_object);
    if (event->getBytesSent() == 0)
    {
        recvEvtT = DBL_MAX;
    }

    // Calculate maximum allowed MPI_COLLECTIVE_BEGIN event timestamp based
    // on received timestamp and latency
    tsa[num_sends] = recvEvtT - m_network->get_latency(ex_loc, CLC_COLL);
    num_sends++;
}


/**
 *  Applies backward amortization
 *  First : Replay
 *  Second: Local Correction
 **/
void
Synchronizer::ba_amortize()
{
    map< Event, timestamp_t, EventCompare >::iterator iter;

    // Set up callback manager
    CallbackManager* cbmanager = new CallbackManager;
    cbmanager->register_callback(GROUP_LEAVE, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_leave));
    cbmanager->register_callback(GROUP_SEND, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_send));
    cbmanager->register_callback(GROUP_RECV, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_recv));
    cbmanager->register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_coll));
    #ifdef _OPENMP
        cbmanager->register_callback(THREAD_FORK, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_fork));
        cbmanager->register_callback(THREAD_TEAM_BEGIN, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_ttb));
        cbmanager->register_callback(THREAD_TEAM_END, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_tte));
        cbmanager->register_callback(THREAD_JOIN, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_join));
    #endif    // _OPENMP

    // Perform backward replay to obtain (earliest) receive timestamps
    PEARL_backward_replay(trace, *cbmanager, 0);

    // Release callback manager
    delete cbmanager;

    // Free cached reverse communicators
    freeReverseComms();

    // Apply local correction
    // Index of next send event
    int mySend = num_sends - 1;

    // Traverse the trace from begin to end.
    LocalTrace::iterator it = trace.begin();
    while (it != trace.end())
    {
        // For every receive event R encountered along the way with d_r > 0
        iter = m_violations.find(it);
        if (iter != m_violations.end())
        {
            // Calculate delta_r
            timestamp_t recv_s  = iter->first->getTimestamp();
            timestamp_t recv_o  = iter->second;
            timestamp_t delta_r = recv_s - recv_o;

            // Start the piece-wise linear amortization
            // with Er := R = iter->first
            //      tr := r* = r - d_r = recv_o
            //      delta_er := delta_r
            //      slope m := const = max_error = 0.01
            ba_piecewise_interpolation(iter->first, recv_o, delta_r, max_error, mySend);
        }

        if (isLogicalSend(it, m_rank))
        {
            mySend--;
        }
        ++it;
    }
}


/**
 *  Backward amortization for receive events.
 *  Computes an interval, L_A, and amortizes events on this interval
 *  using a piecewise linear interpolation function
 **/
void
Synchronizer::ba_piecewise_interpolation(const Event&      event_rhs,
                                         timestamp_t       time_rhs,
                                         const timestamp_t delta_rhs,
                                         const timestamp_t slope_g1,
                                         const long        send_idx)
{
    // Index of the preceding send event
    // Used to obtain correcponding receive event timestamp in timestamp array
    int prec_send_idx = send_idx + 1;

    // Left corner tl := tr - (delta_rhs/m).
    // Calculate left corner of the amortization interval
    timestamp_t m_left = time_rhs - (delta_rhs / slope_g1);

    // If tl < begin of the trace e_b then tl := e_b and m := delta_rhs/(tr - e_b).
    // with tr := r* = r - d_r = time_rhs
    // Set or (if left_t < firstEvtT) calculate m_slope_g1
    timestamp_t m_slope = slope_g1;

    if (m_left < firstEvtT)
    {
        m_left  = firstEvtT;
        m_slope = delta_rhs / (time_rhs - m_left);
    }

    // Linear function g1(t) := m * t - (m * tl) = m_slope * t + m_const
    // Calculate offset of linear equation
    timestamp_t m_const = -1.0 * m_slope * m_left;

    // Search for the send event Si in [tl,Er)
    // with slope mi := (delta_rhs - delta_i)/(tr - si) maximal
    // and mi > m.

    // Up until now we have not found such a send event
    bool found_send = false;

    // Declare individual offsets and slopes at distinct send events
    timestamp_t delta_i, slope_i;

    // Temporary variables to store max. values during the search phase
    Event       event_send = event_rhs;
    timestamp_t slope_send = 0.0;
    timestamp_t delta_send = 0.0;
    timestamp_t const_send = 0.0;
    int         index_send = 0;

    for (Event it = event_rhs.prev();
         it.is_valid() && (m_left < it->getTimestamp());
         --it)
    {
        // Send event found?
        if (isLogicalSend(it, m_rank))
        {
            // Determine offset between send and receive (-l_min)
            delta_i = tsa[prec_send_idx] - it->getTimestamp();

            // Calculate individual slope
            slope_i = (delta_rhs - delta_i) / (time_rhs - it->getTimestamp());

            // If individual slope is larger than initial slope
            if (slope_i > m_slope)
            {
                // Found conflicting send event
                found_send = true;

                // Store new maximum slope
                m_slope = slope_i;

                // Store event data of send event
                event_send = it;
                delta_send = delta_i;
                slope_send = slope_i;
                const_send = delta_send - (slope_send * event_send->getTimestamp());
                index_send = prec_send_idx;
            }

            // Increment pointer to preceding send
            prec_send_idx++;
        }
    }

    // If there is no such Si, apply the linear amortization to the interval [tl,Er)
    // and continue the forward traversal
    if (!found_send)
    {
        ba_linear_amortization_time_to_event(m_left, event_rhs, m_slope, m_const);
    }
    else
    {
        // Apply the linear amortization to the interval [si,Er) using gi
        ba_linear_amortization_event_to_event(event_send, event_rhs, slope_send, const_send);

        // If delta_i > 0 start the piece-wise linear amortization again
        // with Er := Si = event_send
        // tr := si
        // delta_rhs := delat_s
        // m = delta_i/(si - tl).
        time_rhs   = event_send->getTimestamp() - delta_send;
        slope_send = delta_send / (time_rhs - m_left);
        if (delta_send > 0.0)
        {
            ba_piecewise_interpolation(event_send, time_rhs, delta_send, slope_send, index_send);
        }
    }
}


void
Synchronizer::ba_linear_amortization_event_to_event(const Event&      event_lhs,
                                                    const Event&      event_rhs,
                                                    const timestamp_t slope_g,
                                                    const timestamp_t c_g)
{
    for (Event it = event_rhs.prev();
         it.is_valid() && (event_lhs.prev() != it);
         --it)
    {
        it->setTimestamp(it->getTimestamp() + (slope_g * it->getTimestamp()) + c_g);
    }
    if (m_max_slope < slope_g)
    {
        m_max_slope = slope_g;
    }
}


void
Synchronizer::ba_linear_amortization_time_to_event(const timestamp_t time_lhs,
                                                   const Event&      event_rhs,
                                                   const timestamp_t slope_g,
                                                   const timestamp_t c_g)
{
    for (Event it = event_rhs.prev();
         it.is_valid() && (time_lhs < it->getTimestamp());
         --it)
    {
        it->setTimestamp(it->getTimestamp() + (slope_g * it->getTimestamp()) + c_g);
    }
}


// --- Asynchronous message handling ----------------------------------------

void
Synchronizer::update_requests()
{
    // No pending requests? ==> continue
    if (m_requests.empty())
    {
        return;
    }

    // Check for completed messages
    int completed;
    int count = m_requests.size();
    m_indices.resize(count);
    m_statuses.resize(count);

    MPI_Testsome(count, &m_requests[0], &completed, &m_indices[0], &m_statuses[0]);

    m_requests.erase(remove(m_requests.begin(), m_requests.end(),
                            static_cast< MPI_Request >(MPI_REQUEST_NULL)),
                     m_requests.end());

    // Update array of messages
    for (int i = 0; i < completed; ++i)
    {
        int index = m_indices[i];
        delete m_data[index];
        m_data[index] = 0;
    }
    m_data.erase(remove(m_data.begin(), m_data.end(),
                        static_cast< amortization_data* >(0)),
                 m_data.end());
}


void
Synchronizer::delete_requests()
{
    update_requests();

    // Handle remaining messages
    if (!m_requests.empty())
    {
        int count = m_requests.size();

        // Print warning
        UTILS_WARNING("Encountered %d unreceived send operations!", count);

        // Cancel pending messages
        for (int i = 0; i < count; ++i)
        {
            MPI_Cancel(&m_requests[i]);
            MPI_Status status;
            MPI_Wait(&m_requests[i], &status);
            delete m_data[i];
        }

        m_requests.clear();
        m_data.clear();
    }
}


// --- Reverse communicator handling ----------------------------------------

MPI_Comm
Synchronizer::getReverseComm(MpiComm* const comm)
{
    const uint32_t commId = comm->getId();

    // See S. Meyers, "Effective STL", item 24
    map< uint32_t, MPI_Comm >::iterator lb = m_reverse_comms.lower_bound(commId);
    if (  (lb != m_reverse_comms.end())
       && !(m_reverse_comms.key_comp()(commId, lb->first)))
    {
        return lb->second;
    }

    MPI_Comm reversed;
    MPI_Comm_split(comm->getHandle(), 42,
                   (comm->getSize() - comm->getCommSet().getLocalRank(m_rank) - 1),
                   &reversed);

    m_reverse_comms.insert(lb, map< uint32_t, MPI_Comm >::value_type(commId, reversed));

    return reversed;
}


void
Synchronizer::freeReverseComms()
{
    map< uint32_t, MPI_Comm >::iterator it = m_reverse_comms.begin();
    while (it != m_reverse_comms.end())
    {
        MPI_Comm_free(&it->second);
        ++it;
    }
}


// --- Helper functions -----------------------------------------------------

void
Synchronizer::prepare_replay()
{
    // Next pass
    r_stat->set_pass();

    // Reset clock
    m_clc->reset();

    // Apply controller to obtain control variable for next pass
    m_clc->apply_controller(r_stat->get_pass());

    // Clear clock violation list
    m_violations.clear();

    // Reset number of send events for subsequent replay
    num_sends = 0;

    // Copy/restore timestamps for subsequent replay
    if (r_stat->get_pass() == 1)
    {
        get_ts();
    }
    else
    {
        set_ts();
    }
}


/**
 *  Store original timestamps in buffer.
 */
void
Synchronizer::get_ts()
{
    const uint32_t num = trace.num_events();
    tsa = new timestamp_t[num];

    long                 i  = 0;
    LocalTrace::iterator it = trace.begin();
    while (it != trace.end())
    {
        tsa[i] = it->getTimestamp();
        ++i;
        ++it;
    }
}


/**
 *  Restore original timestamps stored by get_ts() before.
 */
void
Synchronizer::set_ts()
{
    long                 i  = 0;
    LocalTrace::iterator it = trace.begin();
    while (it != trace.end())
    {
        it->setTimestamp(tsa[i]);
        ++i;
        ++it;
    }
}
