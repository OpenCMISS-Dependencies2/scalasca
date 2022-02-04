/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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

#include "OmpDelayOps.h"

#include <algorithm>
#include <map>
#include <vector>

#include <pearl/GlobalDefs.h>

#include "CbData.h"
#include "DelayOps.h"
#include "Roles.h"
#include "SynchpointHandler.h"
#include "TmapCacheHandler.h"

using namespace pearl;
using namespace scout;

using std::vector;
using std::map;


namespace
{
// Initialization of function-local static non-POD objects is not thread-safe,
// therefore using variables at file scope.
static timemap_t  latestMap;
static TimeMapSum delayMap;
}    // unnamed namespace


namespace scout
{
/// @brief Calculate delay costs and scaling factors for OpenMP barrier
void
CalculateOmpBarrierDelay(const TimeRank& my,
                         const TimeRank& latest,
                         const Event&    event,
                         CbData*         data)
{
    // shortcuts
    const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
    const SynchpointHandler* SpH = data->mSynchpointHandler;

    data->mPrevSp = SpH->findPreviousOmpSynchpoint(event);

    data->mPropSpMap = SpH->getSynchpointsBetween(data->mPrevSp, event);
    data->mWaitInfo  = CalculateWaitMap(data->mPropSpMap);

    TimeMapSum myTmap = PruneDelayTimemap(TmH->getRuntimes(data->mPrevSp, event),
                                          data->mWaitInfo.mMap,
                                          data->mPrevSp,
                                          SpH->getSynchpointInfo(data->mPrevSp));

    static double propWait;
    static double sumScale;
    static double maxScale;

    data->mCollDelayInfo.clear();
    data->mMaxScales.clear();
    data->mSumScales.clear();

    data->mInputScales.insert(std::make_pair(KEY_WAITTIME, static_cast< double >(latest.mTime - my.mTime)));

    for (map< uint32_t, double >::iterator inpIt = data->mInputScales.begin();
         inpIt != data->mInputScales.end();
         ++inpIt)
    {
        #pragma omp barrier
        { }
        if (my.mRank == latest.mRank)
        {
            latestMap     = myTmap.mMap;
            propWait      = data->mWaitInfo.mSum;
            sumScale      = 0;
            maxScale      = 0;
            delayMap.mSum = 0.0;
            delayMap.mMap.clear();
        }
        #pragma omp barrier
        { }

        TimeMapSum diffMap = CalculateDiff(latestMap, myTmap.mMap);
        double     scale   = 0.0;

        if ((diffMap.mSum + propWait) > 0)
        {
            scale = inpIt->second / (diffMap.mSum + propWait);
        }

        #pragma omp barrier
        { }
        #pragma omp critical
        {
            AddScaledTimemap(diffMap, scale, delayMap);

            sumScale += scale;
            maxScale  = std::max(scale, maxScale);
        }
        #pragma omp barrier
        { }

        if (my.mRank == latest.mRank)
        {
            data->mCollDelayInfo.insert(std::make_pair(inpIt->first, delayMap));

            ScaleMap sumscales, maxscales;

            sumscales.insert(std::make_pair(data->mPrevSp, sumScale));
            maxscales.insert(std::make_pair(data->mPrevSp, maxScale));

            data->mSumScales.insert(std::make_pair(inpIt->first, sumscales));
            data->mMaxScales.insert(std::make_pair(inpIt->first, maxscales));
        }
        #pragma omp barrier
        { }
    }
}
}    // namespace scout
