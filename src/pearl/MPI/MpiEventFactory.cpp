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

#include "MpiEventFactory.h"

#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiCancelled_rep.h>
#include <pearl/MpiCollBegin_rep.h>
#include <pearl/MpiCollEnd_rep.h>
#include <pearl/MpiComm.h>
#include <pearl/MpiGroup.h>
#include <pearl/MpiReceiveComplete_rep.h>
#include <pearl/MpiReceiveRequest_rep.h>
#include <pearl/MpiReceive_rep.h>
#include <pearl/MpiRequestTested_rep.h>
#include <pearl/MpiRmaCollBegin_rep.h>
#include <pearl/MpiRmaCollEnd_rep.h>
#include <pearl/MpiRmaGats_rep.h>
#include <pearl/MpiRmaGetEnd_rep.h>
#include <pearl/MpiRmaGetStart_rep.h>
#include <pearl/MpiRmaLock_rep.h>
#include <pearl/MpiRmaPutEnd_rep.h>
#include <pearl/MpiRmaPutStart_rep.h>
#include <pearl/MpiRmaUnlock_rep.h>
#include <pearl/MpiSendComplete_rep.h>
#include <pearl/MpiSendRequest_rep.h>
#include <pearl/MpiSend_rep.h>
#include <pearl/MpiWindow.h>

using namespace pearl;


// --- MPI-1 specific factory methods ---------------------------------------

Event_rep*
MpiEventFactory::createMpiSend(const GlobalDefs& defs,
                               timestamp_t       time,
                               ident_t           comm,
                               uint32_t          dest,
                               uint32_t          tag,
                               uint64_t          bytesSent) const
{
    // Determine communicator
    MpiComm* communicator = dynamic_cast< MpiComm* >(defs.get_comm(comm));
    if (!communicator)
    {
        throw RuntimeError("MpiEventFactory::createMpiSend() -- "
                           "Invalid MPI communicator ID.");
    }

    return new MpiSend_rep(time,
                           communicator,
                           dest,
                           tag,
                           bytesSent);
}


Event_rep*
MpiEventFactory::createMpiRecv(const GlobalDefs& defs,
                               timestamp_t       time,
                               ident_t           comm,
                               uint32_t          source,
                               uint32_t          tag,
                               uint64_t          bytesReceived) const
{
    // Determine communicator
    MpiComm* communicator = dynamic_cast< MpiComm* >(defs.get_comm(comm));
    if (!communicator)
    {
        throw RuntimeError("MpiEventFactory::createMpiRecv() -- "
                           "Invalid MPI communicator ID.");
    }

    return new MpiReceive_rep(time,
                              communicator,
                              source,
                              tag,
                              bytesReceived);
}


Event_rep*
MpiEventFactory::createMpiCollBegin(const GlobalDefs& defs,
                                    timestamp_t       time) const
{
    return new MpiCollBegin_rep(time);
}


Event_rep*
MpiEventFactory::createMpiCollEnd(const GlobalDefs&         defs,
                                  timestamp_t               time,
                                  MpiCollEnd_rep::coll_type type,
                                  ident_t                   comm,
                                  uint32_t                  root,
                                  uint64_t                  bytesSent,
                                  uint64_t                  bytesReceived) const
{
    // Determine communicator
    MpiComm* communicator = dynamic_cast< MpiComm* >(defs.get_comm(comm));
    if (!communicator)
    {
        throw RuntimeError("MpiEventFactory::createMpiCollEnd() -- "
                           "Invalid MPI communicator ID.");
    }

    return new MpiCollEnd_rep(time,
                              type,
                              communicator,
                              root,
                              bytesSent,
                              bytesReceived);
}


// --- MPI-1 Non-blocking specific factory methods --------------------------

Event_rep*
MpiEventFactory::createMpiCancelled(const GlobalDefs& defs,
                                    timestamp_t       time,
                                    uint64_t          requestId) const
{
    return new MpiCancelled_rep(time, requestId);
}


Event_rep*
MpiEventFactory::createMpiSendRequest(const GlobalDefs& defs,
                                      timestamp_t       time,
                                      ident_t           comm,
                                      uint32_t          dest,
                                      uint32_t          tag,
                                      uint64_t          bytesSent,
                                      uint64_t          requestId) const
{
    // Determine communicator
    MpiComm* communicator = dynamic_cast< MpiComm* >(defs.get_comm(comm));
    if (!communicator)
    {
        throw RuntimeError("MpiEventFactory::createMpiSend() -- "
                           "Invalid MPI communicator ID.");
    }

    return new MpiSendRequest_rep(time,
                                  communicator,
                                  dest,
                                  tag,
                                  bytesSent,
                                  requestId);
}


Event_rep*
MpiEventFactory::createMpiSendComplete(const GlobalDefs& defs,
                                       timestamp_t       time,
                                       uint64_t          requestId) const
{
    return new MpiSendComplete_rep(time, requestId);
}


Event_rep*
MpiEventFactory::createMpiRecvComplete(const GlobalDefs& defs,
                                       timestamp_t       time,
                                       ident_t           comm,
                                       uint32_t          source,
                                       uint32_t          tag,
                                       uint64_t          bytesReceived,
                                       uint64_t          requestId) const
{
    // Determine communicator
    MpiComm* communicator = dynamic_cast< MpiComm* >(defs.get_comm(comm));
    if (!communicator)
    {
        throw RuntimeError("MpiEventFactory::createMpiRecv() -- "
                           "Invalid MPI communicator ID.");
    }

    return new MpiReceiveComplete_rep(time,
                                      communicator,
                                      source,
                                      tag,
                                      bytesReceived,
                                      requestId);
}


Event_rep*
MpiEventFactory::createMpiRecvRequest(const GlobalDefs& defs,
                                      timestamp_t       time,
                                      uint64_t          requestId) const
{
    return new MpiReceiveRequest_rep(time, requestId);
}


Event_rep*
MpiEventFactory::createMpiRequestTested(const GlobalDefs& defs,
                                        timestamp_t       time,
                                        uint64_t          requestId) const
{
    return new MpiRequestTested_rep(time, requestId);
}


// --- MPI-2 RMA specific factory methods -----------------------------------

Event_rep*
MpiEventFactory::createMpiRmaPutStart(const GlobalDefs& defs,
                                      timestamp_t       time,
                                      uint32_t          rma_id,
                                      uint32_t          target,
                                      uint64_t          bytesSent,
                                      ident_t           window) const
{
    MpiWindow* win = dynamic_cast< MpiWindow* >(defs.get_window(window));
    if (!win)
    {
        throw RuntimeError("MpiEventFactory::createMpiRmaPutStart() -- "
                           "Invalid MPI window ID.");
    }

    return new MpiRmaPutStart_rep(time,
                                  rma_id,
                                  target,
                                  bytesSent,
                                  win);
}


Event_rep*
MpiEventFactory::createMpiRmaPutEnd(const GlobalDefs& defs,
                                    timestamp_t       time,
                                    uint32_t          rma_id) const
{
    return new MpiRmaPutEnd_rep(time, rma_id);
}


Event_rep*
MpiEventFactory::createMpiRmaGetStart(const GlobalDefs& defs,
                                      timestamp_t       time,
                                      uint32_t          rma_id,
                                      uint32_t          origin,
                                      uint64_t          bytesReceived,
                                      ident_t           window) const
{
    MpiWindow* win = dynamic_cast< MpiWindow* >(defs.get_window(window));
    if (!win)
    {
        throw RuntimeError("MpiEventFactory::createMpiRmaGetStart() -- "
                           "Invalid MPI window ID.");
    }

    return new MpiRmaGetStart_rep(time,
                                  rma_id,
                                  origin,
                                  bytesReceived,
                                  win);
}


Event_rep*
MpiEventFactory::createMpiRmaGetEnd(const GlobalDefs& defs,
                                    timestamp_t       time,
                                    uint32_t          rma_id) const
{
    return new MpiRmaGetEnd_rep(time, rma_id);
}


Event_rep*
MpiEventFactory::createMpiRmaGats(const GlobalDefs& defs,
                                  timestamp_t       time,
                                  ident_t           window,
                                  ident_t           group,
                                  bool              sync) const
{
    MpiWindow* win = dynamic_cast< MpiWindow* >(defs.get_window(window));
    if (!win)
    {
        throw RuntimeError("MpiEventFactory::createMpiRmaGats() -- "
                           "Invalid MPI window ID.");
    }
    MpiGroup* grp = dynamic_cast< MpiGroup* >(defs.get_group(group));
    if (!grp)
    {
        throw RuntimeError("MpiEventFactory::createMpiRmaGats() -- "
                           "Invalid MPI group ID.");
    }

    return new MpiRmaGats_rep(time,
                              win,
                              grp,
                              sync);
}


Event_rep*
MpiEventFactory::createMpiRmaCollBegin(const GlobalDefs& defs,
                                       timestamp_t       time) const
{
    return new MpiRmaCollBegin_rep(time);
}


Event_rep*
MpiEventFactory::createMpiRmaCollEnd(const GlobalDefs& defs,
                                     timestamp_t       time,
                                     ident_t           window) const
{
    MpiWindow* win = dynamic_cast< MpiWindow* >(defs.get_window(window));
    if (!win)
    {
        throw RuntimeError("MpiEventFactory::createMpiRmaCollEnd() -- "
                           "Invalid MPI window ID.");
    }

    return new MpiRmaCollEnd_rep(time, win);
}


Event_rep*
MpiEventFactory::createMpiRmaLock(const GlobalDefs& defs,
                                  timestamp_t       time,
                                  uint32_t          location,
                                  ident_t           window,
                                  bool              exclusive) const
{
    MpiWindow* win = dynamic_cast< MpiWindow* >(defs.get_window(window));
    if (!win)
    {
        throw RuntimeError("MpiEventFactory::createMpiRmaLock() -- "
                           "Invalid MPI window ID.");
    }

    return new MpiRmaLock_rep(time, location, win, exclusive);
}


Event_rep*
MpiEventFactory::createMpiRmaUnlock(const GlobalDefs& defs,
                                    timestamp_t       time,
                                    uint32_t          location,
                                    ident_t           window) const
{
    MpiWindow* win = dynamic_cast< MpiWindow* >(defs.get_window(window));
    if (!win)
    {
        throw RuntimeError("MpiEventFactory::createMpiRmaUnlock() -- "
                           "Invalid MPI window ID.");
    }

    return new MpiRmaUnlock_rep(time, location, win);
}


// --- Buffer-based factory methods -----------------------------------------

Event_rep*
MpiEventFactory::createEvent(event_t           type,
                             const GlobalDefs& defs,
                             Buffer&           buffer) const
{
    Event_rep* result = NULL;

    switch (type)
    {
        case MPI_SEND:
            result = new MpiSend_rep(defs, buffer);
            break;

        case MPI_SEND_REQUEST:
            result = new MpiSendRequest_rep(defs, buffer);
            break;

        case MPI_SEND_COMPLETE:
            result = new MpiSendComplete_rep(defs, buffer);
            break;

        case MPI_RECV:
            result = new MpiReceive_rep(defs, buffer);
            break;

        case MPI_RECV_REQUEST:
            result = new MpiReceiveRequest_rep(defs, buffer);
            break;

        case MPI_RECV_COMPLETE:
            result = new MpiReceiveComplete_rep(defs, buffer);
            break;

        case MPI_REQUEST_TESTED:
            result = new MpiRequestTested_rep(defs, buffer);
            break;

        case MPI_CANCELLED:
            result = new MpiCancelled_rep(defs, buffer);
            break;

        case MPI_COLLECTIVE_BEGIN:
            result = new MpiCollBegin_rep(defs, buffer);
            break;

        case MPI_COLLECTIVE_END:
            result = new MpiCollEnd_rep(defs, buffer);
            break;

        case MPI_RMA_PUT_START:
            result = new MpiRmaPutStart_rep(defs, buffer);
            break;

        case MPI_RMA_PUT_END:
            result = new MpiRmaPutEnd_rep(defs, buffer);
            break;

        case MPI_RMA_GET_START:
            result = new MpiRmaGetStart_rep(defs, buffer);
            break;

        case MPI_RMA_GET_END:
            result = new MpiRmaGetEnd_rep(defs, buffer);
            break;

        case MPI_RMA_GATS:
            result = new MpiRmaGats_rep(defs, buffer);
            break;

        case MPI_RMA_COLLECTIVE_BEGIN:
            result = new MpiRmaCollBegin_rep(defs, buffer);
            break;

        case MPI_RMA_COLLECTIVE_END:
            result = new MpiRmaCollEnd_rep(defs, buffer);
            break;

        case MPI_RMA_LOCK:
            result = new MpiRmaLock_rep(defs, buffer);
            break;

        case MPI_RMA_UNLOCK:
            result = new MpiRmaUnlock_rep(defs, buffer);
            break;

        default:
            result = EventFactory::createEvent(type, defs, buffer);
            break;
    }

    return result;
}
