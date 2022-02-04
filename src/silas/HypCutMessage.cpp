/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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

#include "HypCutMessage.h"

#include <mpi.h>

#include <pearl/Event.h>
#include <pearl/MpiComm.h>

#include "silas_events.h"
#include "silas_util.h"

using namespace silas;
using namespace std;
using namespace pearl;


HypCutMessage::HypCutMessage()
{
}


HypCutMessage::~HypCutMessage()
{
}


void
HypCutMessage::register_callbacks(const int               run,
                                  pearl::CallbackManager* cbmanager)
{
    if (  (run == 0)
       && !(m_reg_msg_size.empty() && m_reg_msg_tag.empty()))
    {
        log("Registering message surgeon for SEND_EXIT\n");
        cbmanager->register_callback(SEND_EXIT,
                                     PEARL_create_callback(this,
                                                           &HypCutMessage::cb_check_send));
        log("Registering message surgeon for RECV_EXIT\n");
        cbmanager->register_callback(RECV_EXIT,
                                     PEARL_create_callback(this,
                                                           &HypCutMessage::cb_check_recv));
    }
}


void
HypCutMessage::remove_message(const pearl::Event&  exit,
                              silas::CallbackData* data)
{
    pearl::Event enter    = exit.prev().prev();
    pearl::Event transfer = exit.prev();

    pearl::timestamp_t time_to_next_event = exit.next()->getTimestamp()
                                            + data->get_delta_t()
                                            - exit->getTimestamp();

    if (time_to_next_event > 1.5e-6)
    {
        time_to_next_event = 0;
    }

    data->set_delta_t(data->get_delta_t()
                      - time_to_next_event
                      - (exit->getTimestamp() - enter->getTimestamp()));

    // mark events as deleted
    data->set_action(silas::DELETE, enter);
    enter->setTimestamp(-1);
    data->set_action(silas::DELETE, transfer);
    transfer->setTimestamp(-1);
    data->set_action(silas::DELETE, exit);
    exit->setTimestamp(-1);

    // for nonblocking requests, parse through trace and check
    // if test/wait calls can be deleted

    uint64_t requestId = transfer->getRequestId();

    if (requestId == PEARL_NO_REQUEST)
    {
        return;
    }

    pearl::Event ev_it = exit.next();
    event_t      creqt = transfer->isOfType(MPI_RECV_REQUEST)
                         ? MPI_RECV_COMPLETE
                         : MPI_SEND_COMPLETE;

    while (ev_it.is_valid())
    {
        if (  ev_it->isOfType(MPI_REQUEST_TESTED)
           && (ev_it->getRequestId() == requestId))
        {
            check_call_removal(ev_it, data);
        }

        if (  ev_it->isOfType(creqt)
           && (ev_it->getRequestId() == requestId))
        {
            check_call_removal(ev_it, data);
            break;
        }
    }
}


void
HypCutMessage::check_call_removal(const pearl::Event&  completion,
                                  silas::CallbackData* data)
{
    pearl::Event enter = completion.enterptr();
    pearl::Event ev_it = enter.next();

    while (!ev_it->isOfType(GROUP_LEAVE))
    {
        if (data->get_action(ev_it) != silas::DELETE)
        {
            return;
        }

        ++ev_it;
    }

    // mark enter and exit event as deleted
    data->set_action(silas::DELETE, enter);
    enter->setTimestamp(-1);
    data->set_action(silas::DELETE, ev_it);
    ev_it->setTimestamp(-1);
}


void
HypCutMessage::cb_noop(const pearl::CallbackManager& cbmanager,
                       int                           user_event,
                       const pearl::Event&           event,
                       pearl::CallbackData*          cdata)
{
}


void
HypCutMessage::cb_check_send(const pearl::CallbackManager& cbmanager,
                             int                           user_event,
                             const pearl::Event&           event,
                             pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    /*
     * decide about deletion on sender side and notify the receiver
     */
    pearl::Event    send       = event.prev();
    pearl::MpiComm* comm       = send->getComm();
    int             delete_msg = is_registered(send) ? 1 : 0;

    MPI_Send(&delete_msg, 1, MPI_INT,
             send->getDestination(),
             send->getTag(), comm->getHandle());

    if (delete_msg)
    {
        remove_message(event, data);
    }
}


void
HypCutMessage::cb_check_recv(const pearl::CallbackManager& cbmanager,
                             int                           user_event,
                             const pearl::Event&           event,
                             pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    /*
     * Receive deletion flag from sender to enable this check on receiver
     * side.
     */
    pearl::Event recv = event.prev();

    if (recv->getRequestId() != PEARL_NO_REQUEST)
    {
        recv = recv.completion();
    }

    pearl::MpiComm* comm       = recv->getComm();
    int             delete_msg = 1;

    MPI_Recv(&delete_msg, 1, MPI_INT,
             recv->getSource(),
             recv->getTag(), comm->getHandle(), MPI_STATUS_IGNORE);

    if (delete_msg)
    {
        remove_message(event, data);
    }
}


void
HypCutMessage::register_message_size(const std::string& rel,
                                     int                rval)
{
    log("Registering message surgeon for size %s %i\n", rel.c_str(), rval);
    m_reg_msg_size[rel] = rval;
}


void
HypCutMessage::register_message_tag(const std::string& rel,
                                    int                rval)
{
    log("Registering message surgeon for tag %s %i\n", rel.c_str(), rval);
    m_reg_msg_tag[rel] = rval;
}


/// @todo Calls cmp() with uint64_t argument
bool
HypCutMessage::is_registered(const pearl::Event& event)
{
    bool ret = false;

    for (registration_t::iterator it = m_reg_msg_size.begin();
         it != m_reg_msg_size.end();
         ++it)
    {
        if (cmp(it->first, event->getBytesSent(), it->second))
        {
            ret = true;
        }
    }

    for (registration_t::iterator it = m_reg_msg_tag.begin();
         it != m_reg_msg_tag.end();
         ++it)
    {
        if (cmp(it->first, event->getTag(), it->second))
        {
            ret = true;
        }
    }

    return ret;
}


/// @todo Used in is_registered() with uint64_t argument
bool
HypCutMessage::cmp(const std::string& rel,
                   int                a,
                   int                b)
{
    if (rel == "!=")
    {
        return ne(a, b);
    }
    if (rel == "==")
    {
        return eq(a, b);
    }
    if (rel == ">=")
    {
        return ge(a, b);
    }
    if (rel == ">")
    {
        return gt(a, b);
    }
    if (rel == "<=")
    {
        return le(a, b);
    }
    if (rel == "<")
    {
        return lt(a, b);
    }

    return false;
}
