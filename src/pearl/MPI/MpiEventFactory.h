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


#ifndef PEARL_MPIEVENTFACTORY_H
#define PEARL_MPIEVENTFACTORY_H


#include "EventFactory.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiEventFactory.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiEventFactory.
 *
 *  This header file provides the declaration of the class MpiEventFactory.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiEventFactory
 *  @ingroup PEARL_mpi
 *  @brief   Abstract factory class for MPI-specific event representations.
 **/
/*-------------------------------------------------------------------------*/


class MpiEventFactory
    : public EventFactory
{
    public:
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
        /// @name MPI-1 Non-blocking specific factory methods
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
};
}    // namespace pearl


#endif    // !PEARL_MPIEVENTFACTORY_H
