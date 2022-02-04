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
 *  @brief   Implementation of the class MpiCollEnd_rep.
 *
 *  This file provides the implementation of the class MpiCollEnd_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiCollEnd_rep.h>

#include <cassert>
#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/CommSet.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiComm.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Local helper functions -----------------------------------------------

namespace
{
const string
typeToString(const MpiCollEnd_rep::coll_type type);
}    // unnamed namespace


// --- Constructors & destructor --------------------------------------------

MpiCollEnd_rep::MpiCollEnd_rep(timestamp_t timestamp,
                               coll_type   type,
                               MpiComm*    communicator,
                               uint32_t    root,
                               uint64_t    bytesSent,
                               uint64_t    bytesReceived)
    : Event_rep(timestamp),
      mType(type),
      mCommunicator(communicator),
      mRoot(root),
      mBytesSent(bytesSent),
      mBytesReceived(bytesReceived)
{
    // Arbitrary int's can be assigned to an enum, i.e., we need a range check
    assert(  (type >= 0)
          && (type < MpiCollEnd_rep::NUMBER_OF_COLL_TYPES));
}


MpiCollEnd_rep::MpiCollEnd_rep(const GlobalDefs& defs,
                               Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    mType          = static_cast< coll_type >(buffer.get_uint32());
    mCommunicator  = dynamic_cast< MpiComm* >(defs.get_comm(buffer.get_uint32()));
    mRoot          = buffer.get_uint32();
    mBytesSent     = buffer.get_uint64();
    mBytesReceived = buffer.get_uint64();
}


// --- Event type information -----------------------------------------------

event_t
MpiCollEnd_rep::getType() const
{
    return MPI_COLLECTIVE_END;
}


bool
MpiCollEnd_rep::isOfType(event_t type) const
{
    return (  (type == MPI_COLLECTIVE_END)
           || (type == GROUP_END)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

MpiComm*
MpiCollEnd_rep::getComm() const
{
    return mCommunicator;
}


uint32_t
MpiCollEnd_rep::getRoot() const
{
    return mRoot;
}


uint64_t
MpiCollEnd_rep::getBytesSent() const
{
    return mBytesSent;
}


uint64_t
MpiCollEnd_rep::getBytesReceived() const
{
    return mBytesReceived;
}


MpiCollEnd_rep::coll_type
MpiCollEnd_rep::getCollType() const
{
    return mType;
}


// --- Serialize event data (protected) -------------------------------------

void
MpiCollEnd_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(static_cast< uint32_t >(mType));
    buffer.put_uint32(mCommunicator->getId());
    buffer.put_uint32(mRoot);
    buffer.put_uint64(mBytesSent);
    buffer.put_uint64(mBytesReceived);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiCollEnd_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "type: " << typeToString(mType)
           << sep
           << "comm: " << *mCommunicator
           << sep
           << "root: ";
    if (mRoot == PEARL_NO_ID)
    {
        stream << "n/a";
    }
    else
    {
        stream << mRoot
               << " (g: " << mCommunicator->getCommSet().getGlobalRank(mRoot) << ')';
    }
    stream << sep
           << "sent: " << mBytesSent
           << sep
           << "rcvd: " << mBytesReceived;

    return stream;
}


// --- Local helper functions -----------------------------------------------

namespace
{
/// @brief Get string representation of a collective type.
///
/// Returns a human-readable string representation of the given MPI collective
/// @p type, used by the stream I/O functionality.
///
/// @param type
///     MPI collective type
/// @returns
///     Corresponding string representation
///
const string
typeToString(const MpiCollEnd_rep::coll_type type)
{
    switch (type)
    {
        case MpiCollEnd_rep::BARRIER:
            return "Barrier";

        case MpiCollEnd_rep::BCAST:
            return "Bcast";

        case MpiCollEnd_rep::GATHER:
            return "Gather";

        case MpiCollEnd_rep::GATHERV:
            return "Gatherv";

        case MpiCollEnd_rep::SCATTER:
            return "Scatter";

        case MpiCollEnd_rep::SCATTERV:
            return "Scatterv";

        case MpiCollEnd_rep::ALLGATHER:
            return "Allgather";

        case MpiCollEnd_rep::ALLGATHERV:
            return "Allgatherv";

        case MpiCollEnd_rep::ALLTOALL:
            return "Alltoall";

        case MpiCollEnd_rep::ALLTOALLV:
            return "Alltoallv";

        case MpiCollEnd_rep::ALLTOALLW:
            return "Alltoallw";

        case MpiCollEnd_rep::ALLREDUCE:
            return "Allreduce";

        case MpiCollEnd_rep::REDUCE:
            return "Reduce";

        case MpiCollEnd_rep::REDUCE_SCATTER:
            return "Reduce_scatter";

        case MpiCollEnd_rep::SCAN:
            return "Scan";

        case MpiCollEnd_rep::EXSCAN:
            return "Exscan";

        case MpiCollEnd_rep::REDUCE_SCATTER_BLOCK:
            return "Reduce_scatter_block";

        // For "NUMBER_OF_COLL_TYPES" -- to make the compiler happy...
        default:
            break;
    }

    // Since all possible collective types should be handled in the switch
    // statement above, something went wrong if we reach this point...
    assert(false);

    return "";
}
}    // unnamed namespace
