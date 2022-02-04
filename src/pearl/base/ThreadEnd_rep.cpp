/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2014-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class ThreadEnd_rep.
 *
 *  This file provides the implementation of the class ThreadEnd_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/ThreadEnd_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

ThreadEnd_rep::ThreadEnd_rep(const timestamp_t   timestamp,
                             Communicator* const threadContingent,
                             const uint64_t      sequenceCount)
    : Event_rep(timestamp),
      mSequenceCount(sequenceCount),
      mThreadContingent(threadContingent)
{
}


ThreadEnd_rep::ThreadEnd_rep(const GlobalDefs& defs,
                             Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    mThreadContingent = defs.get_comm(buffer.get_uint32());
    mSequenceCount    = buffer.get_uint64();
}


// --- Event type information -----------------------------------------------

event_t
ThreadEnd_rep::getType() const
{
    return THREAD_END;
}


bool
ThreadEnd_rep::isOfType(event_t type) const
{
    return (  (type == THREAD_END)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint64_t
ThreadEnd_rep::getSequenceCount() const
{
    return mSequenceCount;
}


Communicator*
ThreadEnd_rep::getThreadContingent() const
{
    return mThreadContingent;
}


// --- Serialize event data (protected) -------------------------------------

void
ThreadEnd_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(mThreadContingent->getId());
    buffer.put_uint64(mSequenceCount);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
ThreadEnd_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "sequence: " << mSequenceCount
           << sep
           << "contingent: " << *mThreadContingent;

    return stream;
}
