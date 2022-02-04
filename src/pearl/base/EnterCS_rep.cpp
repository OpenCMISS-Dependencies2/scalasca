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
 *  @ingroup PEARL_mpi
 *  @brief   Implementation of the class EnterCS_rep.
 *
 *  This file provides the implementation of the class EnterCS_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/EnterCS_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/Callsite.h>
#include <pearl/GlobalDefs.h>
#include <pearl/Region.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

EnterCS_rep::EnterCS_rep(timestamp_t     timestamp,
                         const Callsite& callsite,
                         uint64_t*       metrics)
    : Enter_rep(timestamp, callsite.getCallee(), metrics),
      mCallsite(callsite)
{
}


EnterCS_rep::EnterCS_rep(const GlobalDefs& defs,
                         Buffer&           buffer)
    : Enter_rep(defs, buffer),
      mCallsite(defs.getCallsite(buffer.get_uint32()))
{
}


// --- Event type information -----------------------------------------------

event_t
EnterCS_rep::getType() const
{
    return ENTER_CS;
}


bool
EnterCS_rep::isOfType(event_t type) const
{
    return (  (type == ENTER_CS)
           || (type == GROUP_ENTER)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

const Callsite&
EnterCS_rep::getCallsite() const
{
    return mCallsite;
}


// --- Serialize event data (protected) -------------------------------------

void
EnterCS_rep::pack(Buffer& buffer) const
{
    Enter_rep::pack(buffer);

    buffer.put_uint32(mCallsite.getId());
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
EnterCS_rep::output(ostream& stream) const
{
    Enter_rep::output(stream);

    // Print data
    stream << sep
           << "callsite: " << mCallsite;

    return stream;
}
