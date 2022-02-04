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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class ThreadReleaseLock_rep.
 *
 *  This file provides the implementation of the class ThreadReleaseLock_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/ThreadReleaseLock_rep.h>

#include <iostream>

#include <pearl/Buffer.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

ThreadReleaseLock_rep::ThreadReleaseLock_rep(timestamp_t timestamp,
                                             uint32_t    lockId,
                                             uint32_t    order,
                                             Paradigm    paradigm)
    : Event_rep(timestamp),
      mLockId(lockId),
      mAcquisitionOrder(order),
      mParadigm(paradigm)
{
}


ThreadReleaseLock_rep::ThreadReleaseLock_rep(const GlobalDefs& defs,
                                             Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    mLockId           = buffer.get_uint32();
    mAcquisitionOrder = buffer.get_uint32();
    mParadigm         = Paradigm(buffer.get_uint32());
}


// --- Event type information -----------------------------------------------

event_t
ThreadReleaseLock_rep::getType() const
{
    return THREAD_RELEASE_LOCK;
}


bool
ThreadReleaseLock_rep::isOfType(event_t type) const
{
    return (  (type == THREAD_RELEASE_LOCK)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint32_t
ThreadReleaseLock_rep::getLockId() const
{
    return mLockId;
}


uint32_t
ThreadReleaseLock_rep::getAcquisitionOrder() const
{
    return mAcquisitionOrder;
}


Paradigm
ThreadReleaseLock_rep::getParadigm() const
{
    return mParadigm;
}


// --- Serialize event data (protected) -------------------------------------

void
ThreadReleaseLock_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(mLockId);
    buffer.put_uint32(mAcquisitionOrder);
    buffer.put_uint32(native_value(mParadigm));
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
ThreadReleaseLock_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "lock: " << mLockId
           << sep
           << "paradigm: " << mParadigm
           << sep
           << "order: " << mAcquisitionOrder;

    return stream;
}
