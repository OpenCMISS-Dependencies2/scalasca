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


#include <config.h>

#include "StatData.h"

using namespace pearl;


static stat_t sort_att = STAT_REL;


void
set_sort_att(stat_t att)
{
    sort_att = att;
}


bool
operator<(const statistic_data& a,
          const statistic_data& b)
{
    if (sort_att == STAT_REL)
    {
        return (a.rel < b.rel);
    }
    if (sort_att == STAT_ABS)
    {
        return (a.abs < b.abs);
    }

    return false;
}
