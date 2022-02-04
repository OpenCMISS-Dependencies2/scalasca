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
 *  @brief   Implementation of the class ThreadFork_rep.
 *
 *  This file provides the implementation of the class ThreadFork_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/ThreadFork_rep.h>

#include <iostream>

#include <pearl/Buffer.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

ThreadFork_rep::ThreadFork_rep(timestamp_t timestamp,
                               uint32_t    teamSize,
                               Paradigm    paradigm)
    : Event_rep(timestamp),
      mTeamSize(teamSize),
      mParadigm(paradigm)
{
}


ThreadFork_rep::ThreadFork_rep(const GlobalDefs& defs,
                               Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    mTeamSize = buffer.get_uint32();
    mParadigm = Paradigm(buffer.get_uint32());
}


// --- Event type information -----------------------------------------------

event_t
ThreadFork_rep::getType() const
{
    return THREAD_FORK;
}


bool
ThreadFork_rep::isOfType(event_t type) const
{
    return (  (type == THREAD_FORK)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint32_t
ThreadFork_rep::getTeamSize() const
{
    return mTeamSize;
}


Paradigm
ThreadFork_rep::getParadigm() const
{
    return mParadigm;
}


// --- Serialize event data (protected) -------------------------------------

void
ThreadFork_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(mTeamSize);
    buffer.put_uint32(native_value(mParadigm));
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
ThreadFork_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "team size: " << mTeamSize
           << sep
           << "paradigm: " << mParadigm;

    return stream;
}
