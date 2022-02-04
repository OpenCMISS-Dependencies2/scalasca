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
 *  @brief   Implementation of the class ThreadTaskCreate_rep.
 *
 *  This file provides the implementation of the class ThreadTaskCreate_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/ThreadTaskCreate_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

ThreadTaskCreate_rep::ThreadTaskCreate_rep(const timestamp_t   timestamp,
                                           Communicator* const threadTeam,
                                           const uint64_t      taskId)
    : Event_rep(timestamp),
      mTaskId(taskId),
      mThreadTeam(threadTeam)
{
}


ThreadTaskCreate_rep::ThreadTaskCreate_rep(const GlobalDefs& defs,
                                           Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    mThreadTeam = defs.get_comm(buffer.get_uint32());
    mTaskId     = buffer.get_uint64();
}


// --- Event type information -----------------------------------------------

event_t
ThreadTaskCreate_rep::getType() const
{
    return THREAD_TASK_CREATE;
}


bool
ThreadTaskCreate_rep::isOfType(event_t type) const
{
    return (  (type == THREAD_TASK_CREATE)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint64_t
ThreadTaskCreate_rep::getTaskId() const
{
    return mTaskId;
}


Communicator*
ThreadTaskCreate_rep::getThreadTeam() const
{
    return mThreadTeam;
}


// --- Serialize event data (protected) -------------------------------------

void
ThreadTaskCreate_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(mThreadTeam->getId());
    buffer.put_uint64(mTaskId);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
ThreadTaskCreate_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "task: " << mTaskId
           << sep
           << "team: " << *mThreadTeam;

    return stream;
}
