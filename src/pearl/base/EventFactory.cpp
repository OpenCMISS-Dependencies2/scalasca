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

#include "EventFactory.h"

#include <cstddef>
#include <vector>

#include <pearl/EnterCS_rep.h>
#include <pearl/EnterProgram_rep.h>
#include <pearl/Enter_rep.h>
#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LeaveProgram_rep.h>
#include <pearl/Leave_rep.h>
#include <pearl/RmaGetEnd_rep.h>
#include <pearl/RmaGetStart_rep.h>
#include <pearl/RmaPutEnd_rep.h>
#include <pearl/RmaPutStart_rep.h>
#include <pearl/ThreadAcquireLock_rep.h>
#include <pearl/ThreadBegin_rep.h>
#include <pearl/ThreadCreate_rep.h>
#include <pearl/ThreadEnd_rep.h>
#include <pearl/ThreadFork_rep.h>
#include <pearl/ThreadJoin_rep.h>
#include <pearl/ThreadReleaseLock_rep.h>
#include <pearl/ThreadTaskComplete_rep.h>
#include <pearl/ThreadTaskCreate_rep.h>
#include <pearl/ThreadTaskSwitch_rep.h>
#include <pearl/ThreadTeamBegin_rep.h>
#include <pearl/ThreadTeamEnd_rep.h>
#include <pearl/ThreadWait_rep.h>

#ifdef PEARL_ENABLE_METRICS
    #include <cstring>
#endif    // PEARL_ENABLE_METRICS

using namespace std;
using namespace pearl;


// --- Static member variables ----------------------------------------------

const EventFactory* EventFactory::m_instance = NULL;


// --- Constructors & destructor --------------------------------------------

EventFactory::~EventFactory()
{
}


// --- Generic factory methods ----------------------------------------------

Event_rep*
EventFactory::createEnter(const GlobalDefs& defs,
                          timestamp_t       time,
                          uint32_t          regionId,
                          uint64_t*         metrics) const
{
    return new Enter_rep(time,
                         defs.getRegion(regionId),
                         copyMetrics(defs, metrics));
}


Event_rep*
EventFactory::createEnterCS(const GlobalDefs& defs,
                            timestamp_t       time,
                            uint32_t          callsiteId,
                            uint64_t*         metrics) const
{
    return new EnterCS_rep(time,
                           defs.getCallsite(callsiteId),
                           copyMetrics(defs, metrics));
}


Event_rep*
EventFactory::createEnterProgram(const GlobalDefs& defs,
                                 timestamp_t       time,
                                 uint32_t          regionId,
                                 uint64_t*         metrics,
                                 uint32_t          nameId,
                                 uint32_t          numArguments,
                                 const uint32_t*   argumentIds) const
{
    vector< const String* > arguments;
    arguments.reserve(numArguments);
    for (uint32_t i = 0; i < numArguments; ++i)
    {
        const String& arg = defs.getString(argumentIds[i]);

        arguments.push_back(&arg);
    }

    return new EnterProgram_rep(time,
                                defs.getRegion(regionId),
                                copyMetrics(defs, metrics),
                                defs.getString(nameId),
                                arguments);
}


Event_rep*
EventFactory::createLeave(const GlobalDefs& defs,
                          timestamp_t       time,
                          uint32_t          regionId,
                          uint64_t*         metrics) const
{
    return new Leave_rep(time,
                         defs.getRegion(regionId),
                         copyMetrics(defs, metrics));
}


Event_rep*
EventFactory::createLeaveProgram(const GlobalDefs& defs,
                                 timestamp_t       time,
                                 uint32_t          regionId,
                                 uint64_t*         metrics,
                                 int64_t           exitStatus) const
{
    return new LeaveProgram_rep(time,
                                defs.getRegion(regionId),
                                copyMetrics(defs, metrics),
                                exitStatus);
}


// --- MPI-1 specific factory methods ---------------------------------------

Event_rep*
EventFactory::createMpiSend(const GlobalDefs& defs,
                            timestamp_t       time,
                            ident_t           comm,
                            uint32_t          dest,
                            uint32_t          tag,
                            uint64_t          bytesSent) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRecv(const GlobalDefs& defs,
                            timestamp_t       time,
                            ident_t           comm,
                            uint32_t          source,
                            uint32_t          tag,
                            uint64_t          bytesReceived) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiCollBegin(const GlobalDefs& defs,
                                 timestamp_t       time) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiCollEnd(const GlobalDefs&         defs,
                               timestamp_t               time,
                               MpiCollEnd_rep::coll_type type,
                               ident_t                   comm,
                               uint32_t                  root,
                               uint64_t                  bytesSent,
                               uint64_t                  bytesReceived) const
{
    return NULL;
}


// --- MPI-1 non-blocking specific methods ----------------------------------

Event_rep*
EventFactory::createMpiCancelled(const GlobalDefs& defs,
                                 timestamp_t       time,
                                 uint64_t          requestId) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiSendRequest(const GlobalDefs& defs,
                                   timestamp_t       time,
                                   ident_t           comm,
                                   uint32_t          dest,
                                   uint32_t          tag,
                                   uint64_t          bytesSent,
                                   uint64_t          requestId) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiSendComplete(const GlobalDefs& defs,
                                    timestamp_t       time,
                                    uint64_t          requestId) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRecvComplete(const GlobalDefs& defs,
                                    timestamp_t       time,
                                    ident_t           comm,
                                    uint32_t          source,
                                    uint32_t          tag,
                                    uint64_t          bytesReceived,
                                    uint64_t          requestId) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRecvRequest(const GlobalDefs& defs,
                                   timestamp_t       time,
                                   uint64_t          requestId) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRequestTested(const GlobalDefs& defs,
                                     timestamp_t       time,
                                     uint64_t          requestId) const
{
    return NULL;
}


// --- Thread-specific factory methods --------------------------------------

Event_rep*
EventFactory::createThreadBegin(const GlobalDefs& defs,
                                timestamp_t       time,
                                uint32_t          threadContingent,
                                uint64_t          sequenceCount) const
{
    return new ThreadBegin_rep(time,
                               defs.get_comm(threadContingent),
                               sequenceCount);
}


Event_rep*
EventFactory::createThreadEnd(const GlobalDefs& defs,
                              timestamp_t       time,
                              uint32_t          threadContingent,
                              uint64_t          sequenceCount) const
{
    return new ThreadEnd_rep(time,
                             defs.get_comm(threadContingent),
                             sequenceCount);
}


Event_rep*
EventFactory::createThreadCreate(const GlobalDefs& defs,
                                 timestamp_t       time,
                                 uint32_t          threadContingent,
                                 uint64_t          sequenceCount) const
{
    return new ThreadCreate_rep(time,
                                defs.get_comm(threadContingent),
                                sequenceCount);
}


Event_rep*
EventFactory::createThreadWait(const GlobalDefs& defs,
                               timestamp_t       time,
                               uint32_t          threadContingent,
                               uint64_t          sequenceCount) const
{
    return new ThreadWait_rep(time,
                              defs.get_comm(threadContingent),
                              sequenceCount);
}


Event_rep*
EventFactory::createThreadFork(const GlobalDefs& defs,
                               timestamp_t       time,
                               uint32_t          teamSize,
                               Paradigm          paradigm) const
{
    return new ThreadFork_rep(time, teamSize, paradigm);
}


Event_rep*
EventFactory::createThreadJoin(const GlobalDefs& defs,
                               timestamp_t       time,
                               Paradigm          paradigm) const
{
    return new ThreadJoin_rep(time, paradigm);
}


Event_rep*
EventFactory::createThreadAcquireLock(const GlobalDefs& defs,
                                      timestamp_t       time,
                                      uint32_t          lockId,
                                      uint32_t          order,
                                      Paradigm          paradigm) const
{
    return new ThreadAcquireLock_rep(time, lockId, order, paradigm);
}


Event_rep*
EventFactory::createThreadReleaseLock(const GlobalDefs& defs,
                                      timestamp_t       time,
                                      uint32_t          lockId,
                                      uint32_t          order,
                                      Paradigm          paradigm) const
{
    return new ThreadReleaseLock_rep(time, lockId, order, paradigm);
}


Event_rep*
EventFactory::createThreadTaskCreate(const GlobalDefs& defs,
                                     timestamp_t       time,
                                     uint32_t          threadTeam,
                                     uint64_t          taskId) const
{
    return new ThreadTaskCreate_rep(time, defs.get_comm(threadTeam), taskId);
}


Event_rep*
EventFactory::createThreadTaskComplete(const GlobalDefs& defs,
                                       timestamp_t       time,
                                       uint32_t          threadTeam,
                                       uint64_t          taskId) const
{
    return new ThreadTaskComplete_rep(time, defs.get_comm(threadTeam), taskId);
}


Event_rep*
EventFactory::createThreadTaskSwitch(const GlobalDefs& defs,
                                     timestamp_t       time,
                                     uint32_t          threadTeam,
                                     uint64_t          taskId) const
{
    return new ThreadTaskSwitch_rep(time, defs.get_comm(threadTeam), taskId);
}


Event_rep*
EventFactory::createThreadTeamBegin(const GlobalDefs& defs,
                                    timestamp_t       time,
                                    uint32_t          threadTeam) const
{
    return new ThreadTeamBegin_rep(time, defs.get_comm(threadTeam));
}


Event_rep*
EventFactory::createThreadTeamEnd(const GlobalDefs& defs,
                                  timestamp_t       time,
                                  uint32_t          threadTeam) const
{
    return new ThreadTeamEnd_rep(time, defs.get_comm(threadTeam));
}


// --- RMA factory methods --------------------------------------------------

Event_rep*
EventFactory::createRmaPutStart(const GlobalDefs& defs,
                                timestamp_t       time,
                                uint32_t          rma_id,
                                uint32_t          target,
                                uint64_t          bytesSent) const
{
    return new RmaPutStart_rep(time, rma_id, target, bytesSent);
}


Event_rep*
EventFactory::createRmaPutEnd(const GlobalDefs& defs,
                              timestamp_t       time,
                              uint32_t          rma_id) const
{
    return new RmaPutEnd_rep(time, rma_id);
}


Event_rep*
EventFactory::createRmaGetStart(const GlobalDefs& defs,
                                timestamp_t       time,
                                uint32_t          rma_id,
                                uint32_t          origin,
                                uint64_t          bytesReceived) const
{
    return new RmaGetStart_rep(time, rma_id, origin, bytesReceived);
}


Event_rep*
EventFactory::createRmaGetEnd(const GlobalDefs& defs,
                              timestamp_t       time,
                              uint32_t          rma_id) const
{
    return new RmaGetEnd_rep(time, rma_id);
}


// --- MPI-2 RMA specific factory methods -----------------------------------

Event_rep*
EventFactory::createMpiRmaPutStart(const GlobalDefs& defs,
                                   timestamp_t       time,
                                   uint32_t          rma_id,
                                   uint32_t          target,
                                   uint64_t          bytesSent,
                                   ident_t           window) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRmaPutEnd(const GlobalDefs& defs,
                                 timestamp_t       time,
                                 uint32_t          rma_id) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRmaGetStart(const GlobalDefs& defs,
                                   timestamp_t       time,
                                   uint32_t          rma_id,
                                   uint32_t          origin,
                                   uint64_t          bytesReceived,
                                   ident_t           window) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRmaGetEnd(const GlobalDefs& defs,
                                 timestamp_t       time,
                                 uint32_t          rma_id) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRmaLock(const GlobalDefs& defs,
                               timestamp_t       time,
                               uint32_t          location,
                               ident_t           window,
                               bool              sync) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRmaUnlock(const GlobalDefs& defs,
                                 timestamp_t       time,
                                 uint32_t          location,
                                 ident_t           window) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRmaGats(const GlobalDefs& defs,
                               timestamp_t       time,
                               ident_t           window,
                               ident_t           group,
                               bool              sync) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRmaCollBegin(const GlobalDefs& defs,
                                    timestamp_t       time) const
{
    return NULL;
}


Event_rep*
EventFactory::createMpiRmaCollEnd(const GlobalDefs& defs,
                                  timestamp_t       time,
                                  ident_t           window) const
{
    return NULL;
}


// --- Buffer-based factory methods -----------------------------------------

Event_rep*
EventFactory::createEvent(event_t           type,
                          const GlobalDefs& defs,
                          Buffer&           buffer) const
{
    Event_rep* result = NULL;

    switch (type)
    {
        case ENTER:
            result = new Enter_rep(defs, buffer);
            break;

        case ENTER_CS:
            result = new EnterCS_rep(defs, buffer);
            break;

        case ENTER_PROGRAM:
            result = new EnterProgram_rep(defs, buffer);
            break;

        case LEAVE:
            result = new Leave_rep(defs, buffer);
            break;

        case LEAVE_PROGRAM:
            result = new LeaveProgram_rep(defs, buffer);
            break;

        case RMA_PUT_START:
            result = new RmaPutStart_rep(defs, buffer);
            break;

        case RMA_PUT_END:
            result = new RmaPutEnd_rep(defs, buffer);
            break;

        case RMA_GET_START:
            result = new RmaGetStart_rep(defs, buffer);
            break;

        case RMA_GET_END:
            result = new RmaGetEnd_rep(defs, buffer);
            break;

        case THREAD_CREATE:
            result = new ThreadCreate_rep(defs, buffer);
            break;

        case THREAD_WAIT:
            result = new ThreadWait_rep(defs, buffer);
            break;

        case THREAD_BEGIN:
            result = new ThreadBegin_rep(defs, buffer);
            break;

        case THREAD_END:
            result = new ThreadEnd_rep(defs, buffer);
            break;

        case THREAD_FORK:
            result = new ThreadFork_rep(defs, buffer);
            break;

        case THREAD_JOIN:
            result = new ThreadJoin_rep(defs, buffer);
            break;

        case THREAD_TEAM_BEGIN:
            result = new ThreadTeamBegin_rep(defs, buffer);
            break;

        case THREAD_TEAM_END:
            result = new ThreadTeamEnd_rep(defs, buffer);
            break;

        case THREAD_TASK_CREATE:
            result = new ThreadTaskCreate_rep(defs, buffer);
            break;

        case THREAD_TASK_COMPLETE:
            result = new ThreadTaskComplete_rep(defs, buffer);
            break;

        case THREAD_TASK_SWITCH:
            result = new ThreadTaskSwitch_rep(defs, buffer);
            break;

        case THREAD_ACQUIRE_LOCK:
            result = new ThreadAcquireLock_rep(defs, buffer);
            break;

        case THREAD_RELEASE_LOCK:
            result = new ThreadReleaseLock_rep(defs, buffer);
            break;

        default:
            throw RuntimeError("EventFactory::createEvent() -- Unknown event type.");
    }

    return result;
}


// --- Singleton interface --------------------------------------------------

void
EventFactory::registerFactory(const EventFactory* factory)
{
    assert(factory);

    if (m_instance)
    {
        throw FatalError("EventFactory::registerFactory(const EventFactory*) -- "
                         "Factory already registered.");
    }

    m_instance = factory;
}


void
EventFactory::deregisterFactory()
{
    if (!m_instance)
    {
        throw FatalError("EventFactory::deregisterFactory() -- "
                         "No factory registered.");
    }

    delete m_instance;
    m_instance = 0;
}


// --- Data copy methods ----------------------------------------------------

uint64_t*
EventFactory::copyMetrics(const GlobalDefs& defs,
                          uint64_t*         metrics) const
{
    uint64_t* result = NULL;

    #ifdef PEARL_ENABLE_METRICS
        // Copy hardware counter metric values
        uint32_t count = defs.num_metrics();
        if (count > 0)
        {
            result = new uint64_t[count];
            memcpy(result, metrics, count * sizeof(uint64_t));
        }
    #endif

    return result;
}
