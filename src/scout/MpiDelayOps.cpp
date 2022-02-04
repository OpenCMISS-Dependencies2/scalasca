/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "MpiDelayOps.h"

#include <algorithm>
#include <map>
#include <vector>

#include <mpi.h>

#include <pearl/CommSet.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiComm.h>

#include "CbData.h"
#include "DelayOps.h"
#include "Roles.h"
#include "SynchpointHandler.h"
#include "TmapCacheHandler.h"

using namespace std;
using namespace pearl;
using namespace scout;


namespace
{
/// @brief Get synchpoint computation times & total propagating wait time

TimeMapSum
GetSynchpointTimes(const Event&             from,
                   const Event&             to,
                   const SynchpointHandler* SpH,
                   const TmapCacheHandler*  TmH)
{
    TimeMapSum     w   = CalculateWaitMap(SpH->getSynchpointsBetween(from, to));
    SynchpointInfo spi = SpH->getSynchpointInfo(from);
    TimeMapSum     m;

    m      = PruneDelayTimemap(TmH->getRuntimes(from, to), w.mMap, from, spi);
    m.mSum = w.mSum;

    return m;
}


/// @brief Transfer data for collective delay calculation
// *INDENT-OFF*    Uncrustify issue #2757
void
TransferData(const TimeRank&               my,
             const TimeRank&               root,
             const TimeMapSum&             groupTmap,
             const map< int, TimeMapSum >& prevSpTmaps,
             vector< int >&                cpIdVec,
             vector< double >&             timeVec,
             int&                          prevSp,
             double&                       propWait,
             MpiComm*                      comm)
// *INDENT-ON*
{
    // --- setup

    vector< int > prevSpVec(comm->getSize(), 0);
    int           cpVecSize = 0;

    if (my.mRank == root.mRank)
    {
        for (map< int, TimeMapSum >::const_iterator it = prevSpTmaps.begin();
             it != prevSpTmaps.end();
             ++it)
        {
            prevSpVec[it->first] = 1;
        }

        cpIdVec.resize(groupTmap.mMap.size());
        timeVec.resize(groupTmap.mMap.size());

        int i = 0;
        for (timemap_t::const_iterator it = groupTmap.mMap.begin();
             it != groupTmap.mMap.end();
             ++it)
        {
            cpIdVec[i] = it->first;
            timeVec[i] = it->second;
            ++i;
        }

        propWait  = groupTmap.mSum;
        cpVecSize = groupTmap.mMap.size();
    }

    // --- transfer

    // who was synched after group synchpoint?
    MPI_Scatter(&prevSpVec[0], 1, MPI_INT,
                &prevSp, 1, MPI_INT, root.mRank, comm->getHandle());

    // propagating wait time
    MPI_Bcast(&propWait, 1, MPI_DOUBLE, root.mRank, comm->getHandle());

    // group callpath ID map and times
    MPI_Bcast(&cpVecSize, 1, MPI_INT, root.mRank, comm->getHandle());

    if (cpIdVec.size() == 0)
    {
        cpIdVec.resize(cpVecSize);
    }
    if (timeVec.size() == 0)
    {
        timeVec.resize(cpVecSize);
    }

    MPI_Bcast(&cpIdVec[0], cpVecSize, MPI_INT, root.mRank, comm->getHandle());
    MPI_Bcast(&timeVec[0], cpVecSize, MPI_DOUBLE, root.mRank, comm->getHandle());

    // point-to-point exchange for ranks after group synchpoint
    if (my.mRank == root.mRank)
    {
        vector< double > vec(cpVecSize);

        for (map< int, TimeMapSum >::const_iterator it = prevSpTmaps.begin();
             it != prevSpTmaps.end();
             ++it)
        {
            MPI_Send(const_cast< double* >(&(it->second.mSum)), 1, MPI_DOUBLE,
                     it->first, 0, comm->getHandle());

            int              i = 0;
            const timemap_t& gtmap(groupTmap.mMap);
            for (timemap_t::const_iterator git = gtmap.begin();
                 git != gtmap.end();
                 ++git, ++i)
            {
                timemap_t::const_iterator pspit = it->second.mMap.find(git->first);
                vec[i] = (pspit == it->second.mMap.end() ? 0.0 : pspit->second);
            }

            MPI_Send(&vec[0], cpVecSize, MPI_DOUBLE, it->first, 0, comm->getHandle());
        }
    }
    else if (prevSp > 0)
    {
        MPI_Recv(&propWait, 1, MPI_DOUBLE, root.mRank, 0,
                 comm->getHandle(), MPI_STATUS_IGNORE);
        MPI_Recv(&timeVec[0], cpVecSize, MPI_DOUBLE, root.mRank, 0,
                 comm->getHandle(), MPI_STATUS_IGNORE);
    }
}


/// @brief Calculate sumscales and maxscales
// *INDENT-OFF*    Uncrustify issue #2757
void
CalculateScales(const TimeRank&          my,
                const TimeRank&          root,
                const map< int, Event >& prevSps,
                bool                     prevSync,
                MpiComm*                 comm,
                CbData*                  data)
// *INDENT-ON*
{
    // mSumScales are long-term delays
    // mMaxScales are (somehow) used for terminal/propagating classification
    data->mSumScales.clear();
    data->mMaxScales.clear();

    const vector< double >::size_type N = data->mInputScales.size();

    vector< double > sum_in(N);
    vector< double > max_in(N);

    map< uint32_t, double >::const_iterator inputIt;
    vector< double >::size_type             i = 0;

    // FIXME: Implicit assumption!
    //
    // This relies on the PRE_*_DELAY user events to create an input scale
    // entry (KEY_RCOST_*) for every handled wait-state type.  Otherwise, the
    // MPI_Reduce() calls below would actually aggregate scales for different
    // wait state types.
    //
    // NOTE: When keeping that assumption, `data->mInputScales` could actually
    // be a fixed-sized vector instead of a map.
    for (inputIt = data->mInputScales.begin(), i = 0;
         inputIt != data->mInputScales.end();
         ++inputIt, ++i)
    {
        sum_in[i] = inputIt->second;
        max_in[i] = prevSync ? 0.0 : inputIt->second;
    }

    vector< double > sum_out(N);
    vector< double > max_out(N);

    MPI_Reduce(&sum_in[0], &sum_out[0], N, MPI_DOUBLE, MPI_SUM, root.mRank, comm->getHandle());
    MPI_Reduce(&max_in[0], &max_out[0], N, MPI_DOUBLE, MPI_MAX, root.mRank, comm->getHandle());

    if (my.mRank == root.mRank)
    {
        vector< ScaleMap > scincmap(N);

        for (inputIt = data->mInputScales.begin(), i = 0;
             inputIt != data->mInputScales.end();
             ++inputIt, ++i)
        {
            ScaleMap maxscales;
            maxscales.insert(make_pair(data->mPrevSp, max_out[i]));
            data->mMaxScales.insert(make_pair(inputIt->first, maxscales));
        }

        // FIXME: Improve documentation for this special handling of delay
        //        costs for intermediate synchronization intervals
        for (map< int, Event >::const_iterator it = prevSps.begin();
             it != prevSps.end();
             ++it)
        {
            vector< double > s(N);

            MPI_Recv(&s[0], N, MPI_DOUBLE, it->first, 0, comm->getHandle(), MPI_STATUS_IGNORE);

            for (inputIt = data->mInputScales.begin(), i = 0;
                 inputIt != data->mInputScales.end();
                 ++inputIt, ++i)
            {
                data->mMaxScales[inputIt->first].insert(make_pair(it->second, max_out[i]));

                max_out[i]               = max(max_out[i], s[i]);
                sum_out[i]              -= s[i];
                scincmap[i][it->second] += s[i];
            }
        }

        for (inputIt = data->mInputScales.begin(), i = 0;
             inputIt != data->mInputScales.end();
             ++inputIt, ++i)
        {
            ScaleMap sumscales;
            sumscales.insert(make_pair(data->mPrevSp, sum_out[i]));

            for (ScaleMap::const_iterator it = scincmap[i].begin();
                 it != scincmap[i].end();
                 ++it)
            {
                sumscales.insert(make_pair(it->first, sum_out[i]));
                sum_out[i] += it->second;
            }

            data->mSumScales.insert(make_pair(inputIt->first, sumscales));
        }
    }
    else if (prevSync > 0)
    {
        MPI_Send(&sum_in[0], N, MPI_DOUBLE, root.mRank, 0, comm->getHandle());
    }
}
}    // unnamed namespace


namespace scout
{
/// @brief Get delay analysis timemap for MPI synchronization interval

timemap_t
GetMpiDelayTimemap(const Event&             event,
                   int                      worldrank,
                   const SynchpointHandler* SpH,
                   const TmapCacheHandler*  TmH)
{
    Event prevSp = SpH->findPreviousMpiSynchpoint(event, worldrank);

    // FIXME: Everything below looks pretty much like GetSynchpointTimes()...
    SynchpointInfoMap propSpMap = SpH->getSynchpointsBetween(prevSp, event);
    TimeMapSum        waitMap   = CalculateWaitMap(propSpMap);

    TimeMapSum m =
        PruneDelayTimemap(TmH->getRuntimes(prevSp, event),
                          waitMap.mMap,
                          prevSp,
                          SpH->getSynchpointInfo(prevSp));

    return m.mMap;
}


/// @brief Calculate delay costs and scaling factors for latearrival collectives

void
CalculateCollectiveDelay(const TimeRank& my,
                         const TimeRank& root,
                         const Event&    event,
                         CbData*         data)
{
    MpiComm*       comm    = event->getComm();
    const CommSet& commSet = comm->getCommSet();

    // shortcuts
    const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
    const SynchpointHandler* SpH = data->mSynchpointHandler;

    // synchpoints since last group synchpoint
    map< int, Event > prevSps;

    // group synchpoint computation times and total prop. wait time
    TimeMapSum groupTmap;

    // computation times and prop. wait time for synchpoints after group synchpoint
    map< int, TimeMapSum > prevSpTmaps;

    // local timemap of non-root processes
    timemap_t localTmap;

    bool isSynchpoint = SpH->isSynchpoint(event);

    // --- setup

    if (isSynchpoint)
    {
        if (my.mRank == root.mRank)
        {
            // mPrevSp defines the begin of the synchronization interval (i.e.,
            // last preceding event at which root has in between synchronized
            // with all other ranks in group)
            // prevSps contains intermediate synchronizing events (per rank), if any
            data->mPrevSp =
                SpH->findPreviousMpiGroupSynchpoints(event,
                                                     commSet,
                                                     commSet.getGlobalRank(root.mRank),
                                                     data->mDefs->numLocationGroups(),
                                                     prevSps);
            data->mPropSpMap =
                SpH->getSynchpointsBetween(data->mPrevSp, event);

            // Time map w/o wait time and previous sync point time, but including completion time
            // groupTmap for whole interval, prevSpTmaps for each sync interval with other ranks
            groupTmap = GetSynchpointTimes(data->mPrevSp, event, SpH, TmH);

            for (map< int, Event >::const_iterator it = prevSps.begin();
                 it != prevSps.end();
                 ++it)
            {
                prevSpTmaps.insert(make_pair(it->first,
                                             GetSynchpointTimes(it->second, event, SpH, TmH)));
            }
        }
        else
        {
            // Time map w/o wait time and previous sync point time, but including completion time
            localTmap = GetMpiDelayTimemap(event, commSet.getGlobalRank(root.mRank), SpH, TmH);
        }
    }

    // --- data exchange

    vector< int >    cpIdVec;     // vector of call-path ids
    vector< double > diffTimeVec; // vector of call-path times

    int    isPrevSync = 0;
    double propWait   = 0.0;

    // In contrast to algorithm 1 on page 10 in Boehme et al., "Identifying
    // the root causes of wait states in large-scale parallel applications",
    // TOPC 3(2), article 11, July 2016, the code below is reversed.  That is,
    // the rank causing the wait state sends its data to the ranks exhibiting
    // wait states, they then calculate their contribution (what's in the `for`
    // loop, line 15-30) locally in parallel, and finally aggregate these
    // contributions on the causing rank via MPI_Reduce() calls.

    TransferData(my, root, groupTmap, prevSpTmaps, cpIdVec, diffTimeVec, isPrevSync, propWait, comm);

    // --- local delay calculation

    vector< int >::size_type cpVecSize = cpIdVec.size();
    double                   dsum      = 0.0;

    if (isSynchpoint)
    {
        for (vector< int >::size_type i = 0; i < cpVecSize; ++i)
        {
            timemap_t::const_iterator fit = localTmap.find(cpIdVec[i]);

            double lt = (fit == localTmap.end() ? 0.0 : fit->second);
            double d  = max(diffTimeVec[i] - lt, 0.0);

            diffTimeVec[i] = d;
            dsum          += d;
        }
    }
    else
    {
        fill(diffTimeVec.begin(), diffTimeVec.end(), 0.0);
    }

    // --- sum up delays

    data->mCollDelayInfo.clear();

    // FIXME: Once all syncpoints are adjusted, `spi.waitTime` should be >= 0
    //        (i.e., `max` may not be necessary)
    const SynchpointInfo spi = SpH->getSynchpointInfo(event);
    data->mInputScales.insert(make_pair(KEY_WAITTIME, max(spi.waitTime, 0.0)));

    for (map< uint32_t, double >::iterator inpIt = data->mInputScales.begin();
         inpIt != data->mInputScales.end();
         ++inpIt)
    {
        // FIXME: This creates a new instance on each iteration and copies
        //        diffTimeVec.  The `for` loop below the scales the values.
        //        Resizing the vector before the loop and assigning the
        //        product of `diffTimeVec[i]` and `scale` (e.g., using
        //        `std::transform`) should be slightly more efficient.
        vector< double > timeVec(diffTimeVec);
        double           scale = dsum;

        // calculate scaling factor and local delays
        if ((scale + propWait) > 0)
        {
            scale = inpIt->second / (scale + propWait);
        }
        for (size_t i = 0; i < cpVecSize; ++i)
        {
            timeVec[i] *= scale;
        }

        MPI_Reduce((my.mRank == root.mRank ? MPI_IN_PLACE : &timeVec[0]),
                   &timeVec[0], cpVecSize, MPI_DOUBLE, MPI_SUM, root.mRank, comm->getHandle());

        TimeMapSum delayInfo;

        if (  (my.mRank == root.mRank)
           && isSynchpoint)
        {
            for (vector< int >::size_type i = 0; i < cpVecSize; ++i)
            {
                delayInfo.mMap[cpIdVec[i]] = timeVec[i];
                delayInfo.mSum            += timeVec[i];
            }
        }

        // mCollDelayInfo now contains "gamma * s * delta_p[c]" (lines 24/25)
        // with KEY_WAITTIME for gamma_short and RCOST keys for gamma_long
        data->mCollDelayInfo.insert(make_pair(inpIt->first, delayInfo));
        inpIt->second = scale;
    }

    // --- get scaling factors for long-term cost and propagating wait-state calculation

    CalculateScales(my, root, prevSps, isPrevSync, comm, data);
}
}    // namespace scout
