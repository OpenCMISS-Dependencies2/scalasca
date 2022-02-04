/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2018-2019                                                **
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
 *  @brief   Implementation of the class LeaveProgram_rep.
 *
 *  This file provides the implementation of the class LeaveProgram_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/LeaveProgram_rep.h>

#include <climits>
#include <iostream>

#include <pearl/Buffer.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

LeaveProgram_rep::LeaveProgram_rep(const timestamp_t timestamp,
                                   const Region&     region,
                                   uint64_t* const   metrics,
                                   int64_t           exitStatus)
    : Leave_rep(timestamp, region, metrics),
      mExitStatus(exitStatus)
{
}


LeaveProgram_rep::LeaveProgram_rep(const GlobalDefs& defs,
                                   Buffer&           buffer)
    : Leave_rep(defs, buffer),
      mExitStatus(buffer.get_int64())
{
}


// --- Event type information -----------------------------------------------

event_t
LeaveProgram_rep::getType() const
{
    return LEAVE_PROGRAM;
}


bool
LeaveProgram_rep::isOfType(event_t type) const
{
    return (  (type == LEAVE_PROGRAM)
           || (type == GROUP_LEAVE)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

int64_t
LeaveProgram_rep::getExitStatus() const
{
    return mExitStatus;
}


// --- Serialize event data (protected) -------------------------------------

void
LeaveProgram_rep::pack(Buffer& buffer) const
{
    Leave_rep::pack(buffer);

    buffer.put_int64(mExitStatus);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
LeaveProgram_rep::output(ostream& stream) const
{
    Leave_rep::output(stream);

    // Print data
    stream << sep
           << "status: ";
    if (mExitStatus == INT64_MIN)
    {
        stream << "<undef>";
    }
    else
    {
        stream << mExitStatus;
    }

    return stream;
}
