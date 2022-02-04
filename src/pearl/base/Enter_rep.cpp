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
 *  @brief   Implementation of the class Enter_rep.
 *
 *  This file provides the implementation of the class Enter_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/Enter_rep.h>

#include <cstddef>
#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/Callpath.h>
#include <pearl/Callsite.h>
#include <pearl/GlobalDefs.h>
#include <pearl/Region.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

Enter_rep::Enter_rep(timestamp_t   timestamp,
                     const Region& region,
                     uint64_t*     metrics)
    : Event_rep(timestamp),
      mRegion(const_cast< Region* >(&region))
{
    #ifdef PEARL_ENABLE_METRICS
        mMetrics = metrics;
    #endif
}


Enter_rep::Enter_rep(const GlobalDefs& defs,
                     Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    mCallpath = defs.get_cnode(buffer.get_id());
    #ifdef PEARL_ENABLE_METRICS
        mMetrics = NULL;
    #endif
}


Enter_rep::~Enter_rep()
{
    #ifdef PEARL_ENABLE_METRICS
        delete[] mMetrics;
    #endif
}


// --- Event type information -----------------------------------------------

event_t
Enter_rep::getType() const
{
    return ENTER;
}


bool
Enter_rep::isOfType(event_t type) const
{
    return (  (type == ENTER)
           || (type == GROUP_ENTER)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

const Region&
Enter_rep::getRegion() const
{
    return mCallpath->getRegion();
}


const Callsite&
Enter_rep::getCallsite() const
{
    return Callsite::UNDEFINED;
}


#ifdef PEARL_ENABLE_METRICS
    uint64_t
    Enter_rep::getMetric(int index) const
    {
        // FIXME: Validation of arguments missing.
        return mMetrics[index];
    }
#endif


// --- Serialize event data (protected) -------------------------------------

void
Enter_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(mCallpath->getId());
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
Enter_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    // WARNING: This assumes that the trace has already been preprocessed!
    stream << sep
           << "region: " << mCallpath->getRegion();

    return stream;
}


// --- Private methods ------------------------------------------------------

const Region&
Enter_rep::getRegionEntered() const
{
    return *mRegion;
}


Callpath*
Enter_rep::getCallpath() const
{
    return mCallpath;
}


void
Enter_rep::setCallpath(Callpath* callpath)
{
    mCallpath = callpath;
}
