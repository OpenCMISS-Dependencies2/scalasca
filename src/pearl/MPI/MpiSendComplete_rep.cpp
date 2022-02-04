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
 *  @brief   Implementation of the class MpiSendComplete_rep.
 *
 *  This file provides the implementation of the class MpiSendComplete_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiSendComplete_rep.h>

#include <iostream>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MpiSendComplete_rep::MpiSendComplete_rep(timestamp_t timestamp,
                                         uint64_t    requestId)
    : Event_rep(timestamp),
      mRequestId(requestId),
      m_prev_reqoffs(0)
{
}


MpiSendComplete_rep::MpiSendComplete_rep(const GlobalDefs& defs,
                                         Buffer&           buffer)
    : Event_rep(defs, buffer),
      mRequestId(PEARL_NO_REQUEST),
      m_prev_reqoffs(0)
{
}


// --- Event type information -----------------------------------------------

event_t
MpiSendComplete_rep::getType() const
{
    return MPI_SEND_COMPLETE;
}


bool
MpiSendComplete_rep::isOfType(event_t type) const
{
    return (  (type == MPI_SEND_COMPLETE)
           || (type == GROUP_NONBLOCK)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint64_t
MpiSendComplete_rep::getRequestId() const
{
    return mRequestId;
}


// --- Modify event data ----------------------------------------------------

void
MpiSendComplete_rep::setRequestId(uint64_t requestId)
{
    mRequestId = requestId;
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiSendComplete_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "request: " << mRequestId;

    return stream;
}


// --- Find next/prev request entries (protected) ---------------------------

uint32_t
MpiSendComplete_rep::get_prev_reqoffs() const
{
    return m_prev_reqoffs;
}


uint32_t
MpiSendComplete_rep::get_next_reqoffs() const
{
    return 0;
}


void
MpiSendComplete_rep::set_prev_reqoffs(uint32_t offs)
{
    m_prev_reqoffs = offs;
}


void
MpiSendComplete_rep::set_next_reqoffs(uint32_t ptr)
{
}
