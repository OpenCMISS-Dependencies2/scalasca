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
 *  @brief   Implementation of the class MpiReceive_rep.
 *
 *  This file provides the implementation of the class MpiReceive_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiReceive_rep.h>

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

MpiReceive_rep::MpiReceive_rep(timestamp_t timestamp,
                               MpiComm*    communicator,
                               uint32_t    source,
                               uint32_t    tag,
                               uint64_t    bytesReceived)
    : Event_rep(timestamp),
      mCommunicator(communicator),
      mSource(source),
      mTag(tag),
      mBytesReceived(bytesReceived)
{
}


MpiReceive_rep::MpiReceive_rep(const GlobalDefs& defs,
                               Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    mCommunicator  = dynamic_cast< MpiComm* >(defs.get_comm(buffer.get_uint32()));
    mSource        = buffer.get_uint32();
    mTag           = buffer.get_uint32();
    mBytesReceived = buffer.get_uint64();
}


// --- Event type information -----------------------------------------------

event_t
MpiReceive_rep::getType() const
{
    return MPI_RECV;
}


bool
MpiReceive_rep::isOfType(event_t type) const
{
    return (  (type == MPI_RECV)
           || (type == GROUP_RECV)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

MpiComm*
MpiReceive_rep::getComm() const
{
    return mCommunicator;
}


uint64_t
MpiReceive_rep::getBytesReceived() const
{
    return mBytesReceived;
}


uint32_t
MpiReceive_rep::getSource() const
{
    return mSource;
}


uint32_t
MpiReceive_rep::getTag() const
{
    return mTag;
}


uint64_t
MpiReceive_rep::getRequestId() const
{
    return PEARL_NO_REQUEST;
}


// --- Serialize event data (protected) -------------------------------------

void
MpiReceive_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(mCommunicator->getId());
    buffer.put_uint32(mSource);
    buffer.put_uint32(mTag);
    buffer.put_uint64(mBytesReceived);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiReceive_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "comm: " << *mCommunicator
           << sep
           << "source: " << mSource
           << " (g: " << mCommunicator->getCommSet().getGlobalRank(mSource) << ')'
           << sep
           << "tag: " << mTag
           << sep
           << "rcvd: " << mBytesReceived;

    return stream;
}


// --- Find previous / next communication request entries (protected) -------

uint32_t
MpiReceive_rep::get_prev_reqoffs() const
{
    return 0;
}


uint32_t
MpiReceive_rep::get_next_reqoffs() const
{
    return 0;
}


void
MpiReceive_rep::set_prev_reqoffs(uint32_t ptr)
{
}


void
MpiReceive_rep::set_next_reqoffs(uint32_t ptr)
{
}
