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


#ifndef CLC_OPERATOR_H
#define CLC_OPERATOR_H


#include <mpi.h>


extern "C"  MPI_Datatype AMORT_DATA;
extern "C"  MPI_Datatype STAT_DATA;

extern "C"  MPI_Op CLC_MAX;
extern "C"  MPI_Op CLC_MIN;
extern "C"  MPI_Op STAT_REL_MAX;
extern "C"  MPI_Op STAT_ABS_MAX;
extern "C"  MPI_Op STAT_REL_MIN;
extern "C"  MPI_Op STAT_ABS_MIN;


void
init_operators();

void
free_operators();


#endif    // !CLC_OPERATOR_H
