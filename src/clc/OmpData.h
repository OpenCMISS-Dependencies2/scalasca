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


#ifndef CLC_OMPDATA_H
#define CLC_OMPDATA_H


extern double omp_global_max_timestamp;
extern double omp_global_min_timestamp;
extern double omp_global_sum_timestamp;

extern int omp_global_max_count;
extern int omp_global_min_count;
extern int omp_global_sum_count;

extern int omp_global_nest_level;


#endif    // !CLC_OMPDATA_H
