/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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

#include "DelayCostMap.h"

using namespace pearl;


namespace scout
{
DelayCostMap::DelayCostMap()
{
}


double
DelayCostMap::getCosts(const Event& e) const
{
    costmap_t::const_iterator it = mCosts.find(e);

    return (it == mCosts.end() ? 0.0 : it->second);
}


void
DelayCostMap::addCosts(const SynchpointInfoMap& sps,
                       double                   scale)
{
    if (!(scale > 0))
    {
        return;
    }

    for (SynchpointInfoMap::const_iterator it = sps.begin();
         it != sps.end();
         ++it)
    {
        if (it->second.waitTime > 0)
        {
            mCosts[it->first] += scale * it->second.waitTime;
        }
    }
}


void
DelayCostMap::addCosts(const SynchpointInfoMap& sps,
                       const ScaleMap&          scales)
{
    if (sps.empty() || scales.empty())
    {
        return;
    }

    ScaleMap::const_iterator scalemapit = scales.begin();
    double                   scale      = scalemapit->second;

    for (SynchpointInfoMap::const_iterator it = sps.begin();
         it != sps.end();
         ++it)
    {
        scalemapit = scales.find(it->first);

        if (scalemapit != scales.end())
        {
            scale = scalemapit->second;
        }

        if ((scale > 0) && (it->second.waitTime > 0))
        {
            mCosts[it->first] += scale * it->second.waitTime;
        }
    }
}
}    // namespace scout
