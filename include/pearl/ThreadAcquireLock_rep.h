/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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


#ifndef PEARL_THREADACQUIRELOCK_REP_H
#define PEARL_THREADACQUIRELOCK_REP_H


#include <pearl/Event_rep.h>
#include <pearl/Paradigm.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    ThreadAcquireLock_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class ThreadAcquireLock_rep.
 *
 *  This header file provides the declaration of the class
 *  ThreadAcquireLock_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   ThreadAcquireLock_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for THREAD_ACQUIRE_LOCK events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR ThreadAcquireLock_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        ThreadAcquireLock_rep(timestamp_t timestamp,
                              uint32_t    lockId,
                              uint32_t    order,
                              Paradigm    paradigm);

        ThreadAcquireLock_rep(const GlobalDefs& defs,
                              Buffer&           buffer);

        /// @}
        /// @name Event type information
        /// @{

        virtual event_t
        getType() const;

        virtual bool
        isOfType(event_t type) const;

        /// @}
        /// @name Access event data
        /// @{

        virtual uint32_t
        getLockId() const;

        uint32_t
        getAcquisitionOrder() const;

        Paradigm
        getParadigm() const;

        /// @}


    protected:
        /// @name Serialize event data
        /// @{

        virtual void
        pack(Buffer& buffer) const;

        /// @}
        /// @name Generate human-readable output of event data
        /// @{

        virtual std::ostream&
        output(std::ostream& stream) const;

        /// @}


    private:
        /// Identifier of the lock being acquired
        uint32_t mLockId;

        /// Monotonically increasing ID to determine the order of lock acquisitions
        uint32_t mAcquisitionOrder;

        /// Threading paradigm
        Paradigm mParadigm;
};
}    // namespace pearl


#endif    // !PEARL_THREADACQUIRELOCK_REP_H
