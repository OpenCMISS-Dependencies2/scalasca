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


#ifndef SCOUT_DELAYCOSTMAP_H
#define SCOUT_DELAYCOSTMAP_H


#include <pearl/Event.h>

#include "scout_types.h"


/**
 *  @file  DelayCostMap.h
 *  @brief Declaration of class DelayCostMap
 **/


namespace scout
{
class DelayCostMap
{
    typedef std::map< pearl::Event, double, pearl::EventKeyCompare >
        costmap_t;

    costmap_t mCosts;


    public:
        DelayCostMap();

        double
        getCosts(const pearl::Event& e) const;

        void
        addCosts(const SynchpointInfoMap& sps,
                 double                   scale);

        void
        addCosts(const SynchpointInfoMap& sps,
                 const ScaleMap&          scales);
};
}    // namespace scout


#endif    // !SCOUT_DELAYCOSTMAP_H
