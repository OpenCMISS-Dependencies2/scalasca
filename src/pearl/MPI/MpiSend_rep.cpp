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
 *  @brief   Implementation of the class MpiSend_rep.
 *
 *  This file provides the implementation of the class MpiSend_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiSend_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/CommSet.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiComm.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MpiSend_rep::MpiSend_rep(timestamp_t timestamp,
                         MpiComm*    communicator,
                         uint32_t    destination,
                         uint32_t    tag,
                         uint64_t    bytesSent)
    : Event_rep(timestamp),
      mCommunicator(communicator),
      mDestination(destination),
      mTag(tag),
      mBytesSent(bytesSent)
{
}


MpiSend_rep::MpiSend_rep(const GlobalDefs& defs,
                         Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    mCommunicator = dynamic_cast< MpiComm* >(defs.get_comm(buffer.get_uint32()));
    mDestination  = buffer.get_uint32();
    mTag          = buffer.get_uint32();
    mBytesSent    = buffer.get_uint64();
}


// --- Event type information -----------------------------------------------

event_t
MpiSend_rep::getType() const
{
    return MPI_SEND;
}


bool
MpiSend_rep::isOfType(event_t type) const
{
    return (  (type == MPI_SEND)
           || (type == GROUP_SEND)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

MpiComm*
MpiSend_rep::getComm() const
{
    return mCommunicator;
}


uint64_t
MpiSend_rep::getBytesSent() const
{
    return mBytesSent;
}


uint32_t
MpiSend_rep::getDestination() const
{
    return mDestination;
}


uint32_t
MpiSend_rep::getTag() const
{
    return mTag;
}


uint64_t
MpiSend_rep::getRequestId() const
{
    return PEARL_NO_REQUEST;
}


// --- Serialize event data (protected) -------------------------------------

void
MpiSend_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(mCommunicator->getId());
    buffer.put_uint32(mDestination);
    buffer.put_uint32(mTag);
    buffer.put_uint64(mBytesSent);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiSend_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "comm: " << *mCommunicator
           << sep
           << "dest: " << mDestination
           << " (g: " << mCommunicator->getCommSet().getGlobalRank(mDestination) << ')'
           << sep
           << "tag: " << mTag
           << sep
           << "sent: " << mBytesSent;

    return stream;
}


// --- Find previous / next communication request entries (protected) -------

uint32_t
MpiSend_rep::get_prev_reqoffs() const
{
    return 0;
}


uint32_t
MpiSend_rep::get_next_reqoffs() const
{
    return 0;
}


void
MpiSend_rep::set_prev_reqoffs(uint32_t ptr)
{
}


void
MpiSend_rep::set_next_reqoffs(uint32_t ptr)
{
}
