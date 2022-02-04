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
 *  @brief   Implementation of the class ThreadTaskComplete_rep.
 *
 *  This file provides the implementation of the class ThreadTaskComplete_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/ThreadTaskComplete_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

ThreadTaskComplete_rep::ThreadTaskComplete_rep(const timestamp_t   timestamp,
                                               Communicator* const threadTeam,
                                               const uint64_t      taskId)
    : Event_rep(timestamp),
      mTaskId(taskId),
      mThreadTeam(threadTeam)
{
}


ThreadTaskComplete_rep::ThreadTaskComplete_rep(const GlobalDefs& defs,
                                               Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    mThreadTeam = defs.get_comm(buffer.get_uint32());
    mTaskId     = buffer.get_uint64();
}


// --- Event type information -----------------------------------------------

event_t
ThreadTaskComplete_rep::getType() const
{
    return THREAD_TASK_COMPLETE;
}


bool
ThreadTaskComplete_rep::isOfType(event_t type) const
{
    return (  (type == THREAD_TASK_COMPLETE)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint64_t
ThreadTaskComplete_rep::getTaskId() const
{
    return mTaskId;
}


Communicator*
ThreadTaskComplete_rep::getThreadTeam() const
{
    return mThreadTeam;
}


// --- Serialize event data (protected) -------------------------------------

void
ThreadTaskComplete_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(mThreadTeam->getId());
    buffer.put_uint64(mTaskId);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
ThreadTaskComplete_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    stream << sep
           << "task: " << mTaskId
           << sep
           << "team: " << *mThreadTeam;

    return stream;
}
