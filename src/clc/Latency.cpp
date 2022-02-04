/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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

#include "Latency.h"

using namespace pearl;


Latency::Latency()
    : lat_machine(0.0),
      lat_p2p_intra(1.0e-6),
      lat_p2p_inter(3.0e-6),
      lat_coll_intra(1.0e-6),
      lat_coll_inter(3.0e-6),
      lat_shared_mem(1.0e-7)
{
}


Latency::Latency(timestamp_t machine,
                 timestamp_t p2p_intra,
                 timestamp_t p2p_inter,
                 timestamp_t coll_intra,
                 timestamp_t coll_inter,
                 timestamp_t shared_mem)
    : lat_machine(machine),
      lat_p2p_intra(p2p_intra),
      lat_p2p_inter(p2p_inter),
      lat_coll_intra(coll_intra),
      lat_coll_inter(coll_inter),
      lat_shared_mem(shared_mem)
{
}
