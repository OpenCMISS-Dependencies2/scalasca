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


#ifndef CLC_STATDATA_H
#define CLC_STATDATA_H


#include <pearl/pearl_types.h>

#include "clc_types.h"


struct statistic_data
{
    pearl::timestamp_t rel;
    pearl::timestamp_t abs;
    pearl::timestamp_t org;
    pearl::timestamp_t syn;
};


bool
operator<(const statistic_data& a,
          const statistic_data& b);

void
set_sort_att(stat_t att);


#endif    // !CLC_STATDATA_H
