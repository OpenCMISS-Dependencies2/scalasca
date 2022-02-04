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


#ifndef CLC_LATENCY_H
#define CLC_LATENCY_H


#include <pearl/pearl_types.h>


class Latency
{
    public:
        // Constructors and Destructor
        Latency();

        Latency(pearl::timestamp_t lat_machine,
                pearl::timestamp_t lat_p2p_intra,
                pearl::timestamp_t lat_p2p_inter,
                pearl::timestamp_t lat_coll_intra,
                pearl::timestamp_t lat_coll_inter,
                pearl::timestamp_t lat_shared_mem);

        pearl::timestamp_t
        get_lat_machine()
        {
            return lat_machine;
        }

        pearl::timestamp_t
        get_lat_p2p_intra()
        {
            return lat_p2p_intra;
        }

        pearl::timestamp_t
        get_lat_p2p_inter()
        {
            return lat_p2p_inter;
        }

        pearl::timestamp_t
        get_lat_coll_intra()
        {
            return lat_coll_intra;
        }

        pearl::timestamp_t
        get_lat_coll_inter()
        {
            return lat_coll_inter;
        }

        pearl::timestamp_t
        get_lat_shared_mem()
        {
            return lat_shared_mem;
        }


    private:
        pearl::timestamp_t lat_machine;
        pearl::timestamp_t lat_p2p_intra;
        pearl::timestamp_t lat_p2p_inter;
        pearl::timestamp_t lat_coll_intra;
        pearl::timestamp_t lat_coll_inter;
        pearl::timestamp_t lat_shared_mem;
};


#endif    // !CLC_LATENCY_H
