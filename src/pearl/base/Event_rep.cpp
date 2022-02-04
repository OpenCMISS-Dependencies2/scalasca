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
 *  @brief   Implementation of the class Event_rep.
 *
 *  This file provides the implementation of the class Event_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/Event_rep.h>

#include <cstddef>
#include <iomanip>
#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/Error.h>

#include "iomanip_detail.h"

#if defined(__SUNPRO_CC)
    #include <pearl/Callsite.h>
    #include <pearl/Region.h>
#endif    // __SUNPRO_CC

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// Some compilers (e.g., Oracle/Sun) issue bogus warnings if a non-void
// function returns via an unconditional throw statement. This macro is
// used to make those buggy compilers happy.
#if defined(__SUNPRO_CC)
    #define DUMMY_RETURN(return_type)           return (return_type)0
    #define UNDEFINED_REFERENCE(return_type)    return (return_type::UNDEFINED)
#else
    #define DUMMY_RETURN(return_type)
    #define UNDEFINED_REFERENCE(return_type)
#endif


// --- Constructors & destructor --------------------------------------------

Event_rep::~Event_rep()
{
}


// --- Access event data ----------------------------------------------------

const Region&
Event_rep::getRegion() const
{
    throw FatalError("EventRep::getRegion() -- "
                     "Oops! This method should not be called.");

    UNDEFINED_REFERENCE(Region);
}


const Callsite&
Event_rep::getCallsite() const
{
    throw FatalError("EventRep::getCallsite() -- "
                     "Oops! This method should not be called.");

    UNDEFINED_REFERENCE(Callsite);
}


MpiComm*
Event_rep::getComm() const
{
    throw FatalError("EventRep::getComm() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(MpiComm*);
}


uint32_t
Event_rep::getRoot() const
{
    throw FatalError("EventRep::getRoot() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint32_t);
}


uint64_t
Event_rep::getBytesSent() const
{
    throw FatalError("EventRep::getBytesSent() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint64_t);
}


uint64_t
Event_rep::getBytesReceived() const
{
    throw FatalError("EventRep::getBytesReceived() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint64_t);
}


uint32_t
Event_rep::getSource() const
{
    throw FatalError("EventRep::getSource() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint32_t);
}


uint32_t
Event_rep::getDestination() const
{
    throw FatalError("EventRep::getDestination() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint32_t);
}


uint32_t
Event_rep::getTag() const
{
    throw FatalError("EventRep::getTag() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint32_t);
}


uint64_t
Event_rep::getRequestId() const
{
    throw FatalError("EventRep::getRequestId() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint64_t);
}


uint32_t
Event_rep::getLockId() const
{
    throw FatalError("EventRep::getLockId() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint32_t);
}


MpiWindow*
Event_rep::get_window() const
{
    throw FatalError("EventRep::get_window() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(MpiWindow*);
}


uint32_t
Event_rep::get_remote() const
{
    throw FatalError("EventRep::get_remote() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint32_t);
}


uint32_t
Event_rep::get_rma_id() const
{
    throw FatalError("EventRep::get_rma_id() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint32_t);
}


MpiGroup*
Event_rep::get_group() const
{
    throw FatalError("EventRep::get_group() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(MpiGroup*);
}


bool
Event_rep::is_sync() const
{
    throw FatalError("EventRep::is_sync() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(bool);
}


bool
Event_rep::is_exclusive() const
{
    throw FatalError("EventRep::is_exclusive() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(bool);
}


#ifdef PEARL_ENABLE_METRICS
    uint64_t
    Event_rep::getMetric(int index) const
    {
        throw FatalError("EventRep::getMetric(int) -- "
                         "Oops! This method should not be called.");

        DUMMY_RETURN(uint64_t);
    }
#endif


// --- Modify event data ----------------------------------------------------

void
Event_rep::setRequestId(uint64_t)
{
    throw FatalError("EventRep::setRequestId(uint64_t) -- "
                     "Oops! This method should not be called.");
}


// --- Constructors & destructor (protected) --------------------------------

Event_rep::Event_rep(pearl::timestamp_t timestamp)
    : mTimestamp(timestamp)
{
}


Event_rep::Event_rep(const GlobalDefs& defs,
                     Buffer&           buffer)
{
    mTimestamp = buffer.getTimestamp();
}


// --- Serialize event data (protected) -------------------------------------

void
Event_rep::pack(Buffer& buffer) const
{
    buffer.putTimestamp(mTimestamp);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
Event_rep::output(ostream& stream) const
{
    // Print data
    stream << sep
           << "time: " << fixed << mTimestamp;

    return stream;
}


// --- Find previous / next request event in trace (protected) --------------

uint32_t
Event_rep::get_next_reqoffs() const
{
    throw FatalError("EventRep::get_next_reqoffs() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint32_t);
}


uint32_t
Event_rep::get_prev_reqoffs() const
{
    throw FatalError("EventRep::get_prev_reqoffs() -- "
                     "Oops! This method should not be called.");

    DUMMY_RETURN(uint32_t);
}


void
Event_rep::set_next_reqoffs(uint32_t)
{
    throw FatalError("EventRep::set_next_reqoffs(uint32_t) -- "
                     "Oops! This method should not be called.");
}


void
Event_rep::set_prev_reqoffs(uint32_t)
{
    throw FatalError("EventRep::set_prev_reqoffs(uint32_t) -- "
                     "Oops! This method should not be called.");
}
