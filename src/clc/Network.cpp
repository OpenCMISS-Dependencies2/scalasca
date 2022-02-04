/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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

#include "Network.h"

#include "Latency.h"

using namespace pearl;


Network::Network(clc_location loc,
                 Latency*     lat)
    : location(loc),
      latency(lat)
{
}


Network::~Network()
{
    delete latency;
}


timestamp_t
Network::get_latency(clc_location& loc,
                     clc_event_t   com_type)
{
    if (com_type == CLC_OMP)
    {
        return latency->get_lat_shared_mem();
    }

    double lat = 0.0;
    if (location.machine != loc.machine)
    {
        lat = latency->get_lat_machine();
    }

    if (com_type == CLC_P2P)
    {
        if (location.node != loc.node)
        {
            lat += latency->get_lat_p2p_inter();
        }
        else
        {
            lat += latency->get_lat_p2p_intra();
        }
    }
    else if (com_type == CLC_COLL)
    {
        if (location.node != loc.node)
        {
            lat += latency->get_lat_coll_inter();
        }
        else
        {
            lat += latency->get_lat_coll_intra();
        }
    }

    return lat;
}
