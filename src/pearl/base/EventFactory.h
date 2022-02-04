/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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


#ifndef PEARL_EVENTFACTORY_H
#define PEARL_EVENTFACTORY_H


#include <cassert>

#include <pearl/MpiCollEnd_rep.h>
#include <pearl/Paradigm.h>
#include <pearl/pearl_types.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    EventFactory.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class EventFactory.
 *
 *  This header file provides the declaration of the class EventFactory.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Buffer;
class Event_rep;
class GlobalDefs;


/*-------------------------------------------------------------------------*/
/**
 *  @class   EventFactory
 *  @ingroup PEARL_base
 *  @brief   Abstract factory class for event representations.
 **/
/*-------------------------------------------------------------------------*/

class EventFactory
{
    public:
        /// @name Constructors & destructor
        /// @{

        virtual
        ~EventFactory();

        /// @}
        /// @name Generic factory methods
        /// @{

        Event_rep*
        createEnter(const GlobalDefs& defs,
                    timestamp_t       time,
                    uint32_t          regionId,
                    uint64_t*         metrics) const;

        Event_rep*
        createEnterCS(const GlobalDefs& defs,
                      timestamp_t       time,
                      uint32_t          callsiteId,
                      uint64_t*         metrics) const;

        Event_rep*
        createEnterProgram(const GlobalDefs& defs,
                           timestamp_t       time,
                           uint32_t          regionId,
                           uint64_t*         metrics,
                           uint32_t          nameId,
                           uint32_t          numArguments,
                           const uint32_t*   argumentIds) const;

        Event_rep*
        createLeave(const GlobalDefs& defs,
                    timestamp_t       time,
                    uint32_t          regionId,
                    uint64_t*         metrics) const;

        Event_rep*
        createLeaveProgram(const GlobalDefs& defs,
                           timestamp_t       time,
                           uint32_t          regionId,
                           uint64_t*         metrics,
                           int64_t           exitStatus) const;

        /// @}
        /// @name MPI-1 specific factory methods
        /// @{

        virtual Event_rep*
        createMpiSend(const GlobalDefs& defs,
                      timestamp_t       time,
                      ident_t           comm,
                      uint32_t          dest,
                      uint32_t          tag,
                      uint64_t          bytesSent) const;

        virtual Event_rep*
        createMpiRecv(const GlobalDefs& defs,
                      timestamp_t       time,
                      ident_t           comm,
                      uint32_t          source,
                      uint32_t          tag,
                      uint64_t          bytesReceived) const;

        virtual Event_rep*
        createMpiCollBegin(const GlobalDefs& defs,
                           timestamp_t       time) const;

        virtual Event_rep*
        createMpiCollEnd(const GlobalDefs&         defs,
                         timestamp_t               time,
                         MpiCollEnd_rep::coll_type type,
                         ident_t                   comm,
                         uint32_t                  root,
                         uint64_t                  bytesSent,
                         uint64_t                  bytesReceived) const;

        /// @}
        /// @name MPI-1 non-blocking specific factory methods
        /// @{

        virtual Event_rep*
        createMpiCancelled(const GlobalDefs& defs,
                           timestamp_t       time,
                           uint64_t          requestId) const;

        virtual Event_rep*
        createMpiSendRequest(const GlobalDefs& defs,
                             timestamp_t       time,
                             ident_t           comm,
                             uint32_t          dest,
                             uint32_t          tag,
                             uint64_t          bytesSent,
                             uint64_t          requestId) const;

        virtual Event_rep*
        createMpiSendComplete(const GlobalDefs& defs,
                              timestamp_t       time,
                              uint64_t          requestId) const;

        virtual Event_rep*
        createMpiRecvComplete(const GlobalDefs& defs,
                              timestamp_t       time,
                              ident_t           comm,
                              uint32_t          source,
                              uint32_t          tag,
                              uint64_t          bytesReceived,
                              uint64_t          requestId) const;

        virtual Event_rep*
        createMpiRecvRequest(const GlobalDefs& defs,
                             timestamp_t       time,
                             uint64_t          requestId) const;

        virtual Event_rep*
        createMpiRequestTested(const GlobalDefs& defs,
                               timestamp_t       time,
                               uint64_t          requestId) const;

        /// @}
        /// @name Thread-specific factory methods
        /// @{

        Event_rep*
        createThreadBegin(const GlobalDefs& defs,
                          timestamp_t       time,
                          uint32_t          threadContingent,
                          uint64_t          sequenceCount) const;

        Event_rep*
        createThreadEnd(const GlobalDefs& defs,
                        timestamp_t       time,
                        uint32_t          threadContingent,
                        uint64_t          sequenceCount) const;

        Event_rep*
        createThreadCreate(const GlobalDefs& defs,
                           timestamp_t       time,
                           uint32_t          threadContingent,
                           uint64_t          sequenceCount) const;

        Event_rep*
        createThreadWait(const GlobalDefs& defs,
                         timestamp_t       time,
                         uint32_t          threadContingent,
                         uint64_t          sequenceCount) const;

        Event_rep*
        createThreadFork(const GlobalDefs& defs,
                         timestamp_t       time,
                         uint32_t          teamSize,
                         Paradigm          paradigm) const;

        Event_rep*
        createThreadJoin(const GlobalDefs& defs,
                         timestamp_t       time,
                         Paradigm          paradigm) const;

        Event_rep*
        createThreadAcquireLock(const GlobalDefs& defs,
                                timestamp_t       time,
                                uint32_t          lockId,
                                uint32_t          order,
                                Paradigm          paradigm) const;

        Event_rep*
        createThreadReleaseLock(const GlobalDefs& defs,
                                timestamp_t       time,
                                uint32_t          lockId,
                                uint32_t          order,
                                Paradigm          paradigm) const;

        Event_rep*
        createThreadTaskCreate(const GlobalDefs& defs,
                               timestamp_t       time,
                               uint32_t          threadTeam,
                               uint64_t          taskId) const;

        Event_rep*
        createThreadTaskComplete(const GlobalDefs& defs,
                                 timestamp_t       time,
                                 uint32_t          threadTeam,
                                 uint64_t          taskId) const;

        Event_rep*
        createThreadTaskSwitch(const GlobalDefs& defs,
                               timestamp_t       time,
                               uint32_t          threadTeam,
                               uint64_t          taskId) const;

        Event_rep*
        createThreadTeamBegin(const GlobalDefs& defs,
                              timestamp_t       time,
                              uint32_t          threadTeam) const;

        Event_rep*
        createThreadTeamEnd(const GlobalDefs& defs,
                            timestamp_t       time,
                            uint32_t          threadTeam) const;

        /// @}
        /// @name RMA specific factory methods
        /// @{

        Event_rep*
        createRmaPutStart(const GlobalDefs& defs,
                          timestamp_t       time,
                          uint32_t          rma_id,
                          uint32_t          target,
                          uint64_t          bytesSent) const;

        Event_rep*
        createRmaPutEnd(const GlobalDefs& defs,
                        timestamp_t       time,
                        uint32_t          rma_id) const;

        Event_rep*
        createRmaGetStart(const GlobalDefs& defs,
                          timestamp_t       time,
                          uint32_t          rma_id,
                          uint32_t          origin,
                          uint64_t          bytesReceived) const;

        Event_rep*
        createRmaGetEnd(const GlobalDefs& defs,
                        timestamp_t       time,
                        uint32_t          rma_id) const;

        /// @}
        /// @name MPI-2 RMA specific factory methods
        /// @{

        virtual Event_rep*
        createMpiRmaPutStart(const GlobalDefs& defs,
                             timestamp_t       time,
                             uint32_t          rma_id,
                             uint32_t          target,
                             uint64_t          bytesSent,
                             ident_t           window) const;

        virtual Event_rep*
        createMpiRmaPutEnd(const GlobalDefs& defs,
                           timestamp_t       time,
                           uint32_t          rma_id) const;

        virtual Event_rep*
        createMpiRmaGetStart(const GlobalDefs& defs,
                             timestamp_t       time,
                             uint32_t          rma_id,
                             uint32_t          origin,
                             uint64_t          bytesReceived,
                             ident_t           window) const;

        virtual Event_rep*
        createMpiRmaGetEnd(const GlobalDefs& defs,
                           timestamp_t       time,
                           uint32_t          rma_id) const;

        virtual Event_rep*
        createMpiRmaGats(const GlobalDefs& defs,
                         timestamp_t       time,
                         ident_t           window,
                         ident_t           group,
                         bool              sync) const;

        virtual Event_rep*
        createMpiRmaCollBegin(const GlobalDefs& defs,
                              timestamp_t       time) const;

        virtual Event_rep*
        createMpiRmaCollEnd(const GlobalDefs& defs,
                            timestamp_t       time,
                            ident_t           window) const;

        virtual Event_rep*
        createMpiRmaLock(const GlobalDefs& defs,
                         timestamp_t       time,
                         uint32_t          location,
                         ident_t           window,
                         bool              exclusive) const;

        virtual Event_rep*
        createMpiRmaUnlock(const GlobalDefs& defs,
                           timestamp_t       time,
                           uint32_t          location,
                           ident_t           window) const;

        /// @}
        /// @name Buffer-based factory methods
        /// @{

        virtual Event_rep*
        createEvent(event_t           type,
                    const GlobalDefs& defs,
                    Buffer&           buffer) const;

        /// @}
        /// @name Singleton interface
        /// @{

        static const EventFactory*
        instance();

        static void
        registerFactory(const EventFactory* factory);

        static void
        deregisterFactory();

        /// @}


    protected:
        /// @name Data copy methods
        /// @{

        uint64_t*
        copyMetrics(const GlobalDefs& defs,
                    uint64_t*         metrics) const;

        /// @}


    private:
        /// Single factory instance
        static const EventFactory* m_instance;
};


// --- Inline methods -------------------------------------------------------

inline const EventFactory*
EventFactory::instance()
{
    assert(m_instance);

    return m_instance;
}
}    // namespace pearl


#endif    // !PEARL_EVENTFACTORY_H
