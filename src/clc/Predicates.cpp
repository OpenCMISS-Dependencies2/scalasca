/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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


#include <config.h>

#include "Predicates.h"

#include <UTILS_Error.h>

#include <pearl/CommSet.h>
#include <pearl/MpiCollEnd_rep.h>
#include <pearl/MpiComm.h>
#include <pearl/Region.h>

using namespace pearl;


/**
 *  Returns TRUE if the MPI_COLLECTIVE_END event is on the
 *  root process of a collective operation.
 **/
bool
isOnRoot(const Event& event,
         uint32_t     globalRank)
{
    const CommSet& commSet        = event->getComm()->getCommSet();
    const uint32_t rootLocalRank  = event->getRoot();
    const uint32_t rootGlobalRank = commSet.getGlobalRank(rootLocalRank);

    return (globalRank == rootGlobalRank);
}


bool
isLogicalSend(const Event& event,
              int          rank)
{
    // Obviously, (non-)blocking SEND events are also logical sends
    if (event->isOfType(GROUP_SEND))
    {
        return true;
    }

    event_t type = event->getType();
    if (type == MPI_COLLECTIVE_BEGIN)
    {
        const Event& end = event.endptr();

        // No synchronization on self-like communicators, thus no logical send
        if (end->getComm()->getSize() == 1)
        {
            return false;
        }

        MpiCollEnd_rep& endRep = event_cast< MpiCollEnd_rep >(*end);
        switch (endRep.getCollType())
        {
            // CASE 1: Barrier, N-to-N communication, and prefix reduction
            case MpiCollEnd_rep::BARRIER:
            case MpiCollEnd_rep::ALLGATHER:
            case MpiCollEnd_rep::ALLGATHERV:
            case MpiCollEnd_rep::ALLTOALL:
            case MpiCollEnd_rep::ALLREDUCE:
            case MpiCollEnd_rep::REDUCE_SCATTER:
            case MpiCollEnd_rep::REDUCE_SCATTER_BLOCK:
            case MpiCollEnd_rep::SCAN:
            case MpiCollEnd_rep::EXSCAN:
                return true;

            // CASE 2: 1-to-N communication
            case MpiCollEnd_rep::BCAST:
            case MpiCollEnd_rep::SCATTER:
            case MpiCollEnd_rep::SCATTERV:
                return isOnRoot(end, rank);

            // CASE 3: N-to-1 communication
            case MpiCollEnd_rep::GATHER:
            case MpiCollEnd_rep::GATHERV:
            case MpiCollEnd_rep::REDUCE:
                return !isOnRoot(end, rank);

            // SPECIAL CASE:
            // MPI_Alltoallv and MPI_Alltoallw can be used to implement arbitrary
            // communication patterns (e.g., pair-wise message exchange).  However,
            // the trace data currently does not provide enough information for a
            // proper reconstruction, thus, ignore those calls and handle their
            // MPI_COLLECTIVE_END as "internal events".
            case MpiCollEnd_rep::ALLTOALLV:
            case MpiCollEnd_rep::ALLTOALLW:
                return false;

            // We should never get to this point...
            default:
                UTILS_BUG("Unknown MPI collective operation encountered");
                break;
        }

        return false;
    }

    if (event->isOfType(GROUP_ENTER))
    {
        const Region& region = event->getRegion();
        if (  is_mpi_init(region)
           || is_mpi_finalize(region))
        {
            return true;
        }

        #if defined(_OPENMP)
            else if (is_omp_barrier(region))
            {
                return true;
            }
        #endif    // _OPENMP

        return false;
    }

    #if defined(_OPENMP)
        if (type == THREAD_FORK)
        {
            return true;
        }

        if (type == THREAD_TEAM_END)
        {
            return true;
        }
    #endif    // _OPENMP

    return false;
}
