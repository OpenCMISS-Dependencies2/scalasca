/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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


#ifndef CLC_AMORTDATA_H
#define CLC_AMORTDATA_H


#include <pearl/pearl_types.h>

#include "clc_types.h"


struct amortization_data
{
    pearl::ident_t     machine_id;
    pearl::ident_t     node_id;
    pearl::ident_t     process_id;
    pearl::ident_t     thread_id;
    pearl::timestamp_t timestamp;
};


amortization_data
pack_data(clc_location       loc,
          pearl::timestamp_t m_time);

pearl::timestamp_t
unpack_data(clc_location&     loc,
            amortization_data data);


#endif    // !CLC_AMORTDATA_H
