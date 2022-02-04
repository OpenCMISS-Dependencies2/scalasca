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
 *  @brief   Implementation of the class MpiRequestTested_rep.
 *
 *  This file provides the implementation of the class MpiRequestTested_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiRequestTested_rep.h>

#include <iostream>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MpiRequestTested_rep::MpiRequestTested_rep(timestamp_t timestamp,
                                           uint64_t    requestId)
    : Event_rep(timestamp),
      mRequestId(requestId),
      m_next_reqoffs(0),
      m_prev_reqoffs(0)
{
}


MpiRequestTested_rep::MpiRequestTested_rep(const GlobalDefs& defs,
                                           Buffer&           buffer)
    : Event_rep(defs, buffer),
      mRequestId(PEARL_NO_REQUEST),
      m_next_reqoffs(0),
      m_prev_reqoffs(0)
{
}


// --- Event type information -----------------------------------------------

event_t
MpiRequestTested_rep::getType() const
{
    return MPI_REQUEST_TESTED;
}


bool
MpiRequestTested_rep::isOfType(event_t type) const
{
    return (  (type == MPI_REQUEST_TESTED)
           || (type == GROUP_NONBLOCK)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint64_t
MpiRequestTested_rep::getRequestId() const
{
    return mRequestId;
}


// --- Modify event data ----------------------------------------------------

void
MpiRequestTested_rep::setRequestId(uint64_t requestId)
{
    mRequestId = requestId;
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiRequestTested_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "request: " << mRequestId;

    return stream;
}


// --- Find next/prev request entries (protected) ---------------------------

uint32_t
MpiRequestTested_rep::get_prev_reqoffs() const
{
    return m_prev_reqoffs;
}


uint32_t
MpiRequestTested_rep::get_next_reqoffs() const
{
    return m_next_reqoffs;
}


void
MpiRequestTested_rep::set_prev_reqoffs(uint32_t offs)
{
    m_prev_reqoffs = offs;
}


void
MpiRequestTested_rep::set_next_reqoffs(uint32_t offs)
{
    m_next_reqoffs = offs;
}
