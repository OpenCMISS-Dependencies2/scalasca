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
 *  @brief   Implementation of the class MpiSendRequest_rep.
 *
 *  This file provides the implementation of the class MpiSendRequest_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiSendRequest_rep.h>

#include <iostream>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MpiSendRequest_rep::MpiSendRequest_rep(timestamp_t timestamp,
                                       MpiComm*    communicator,
                                       uint32_t    destination,
                                       uint32_t    tag,
                                       uint64_t    bytesSent,
                                       uint64_t    requestId)
    : MpiSend_rep(timestamp, communicator, destination, tag, bytesSent),
      mRequestId(requestId),
      m_next_reqoffs(0)
{
}


MpiSendRequest_rep::MpiSendRequest_rep(const GlobalDefs& defs,
                                       Buffer&           buffer)
    : MpiSend_rep(defs, buffer),
      mRequestId(PEARL_NO_REQUEST),
      m_next_reqoffs(0)
{
}


// --- Event type information -----------------------------------------------

event_t
MpiSendRequest_rep::getType() const
{
    return MPI_SEND_REQUEST;
}


bool
MpiSendRequest_rep::isOfType(event_t type) const
{
    return (  (type == MPI_SEND_REQUEST)
           || (type == GROUP_SEND)
           || (type == GROUP_NONBLOCK)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint64_t
MpiSendRequest_rep::getRequestId() const
{
    return mRequestId;
}


// --- Modify event data ----------------------------------------------------

void
MpiSendRequest_rep::setRequestId(uint64_t requestId)
{
    mRequestId = requestId;
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiSendRequest_rep::output(ostream& stream) const
{
    MpiSend_rep::output(stream);

    // Print data
    stream << sep
           << "request: " << mRequestId;

    return stream;
}


// --- Find next/prev request entries (protected) ---------------------------

uint32_t
MpiSendRequest_rep::get_next_reqoffs() const
{
    return m_next_reqoffs;
}


void
MpiSendRequest_rep::set_next_reqoffs(uint32_t offs)
{
    m_next_reqoffs = offs;
}
