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


#ifndef SCOUT_MPIOPERATORS_H
#define SCOUT_MPIOPERATORS_H


#include <mpi.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file  MpiOperators.h
 *  @brief Declaration of MPI reduction operators and related functions.
 *
 *  This header file provides the declaration of SCOUT-specific MPI reduction
 *  operators and related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Global variables -----------------------------------------------------

extern MPI_Op MINMAX_TIMEVEC2;
extern MPI_Op MAX_TOPSEVEREARRAY;
extern MPI_Op MAXSUM_TOPSEVERECOLL;


// --- Related functions ----------------------------------------------------

void
InitOperators();

void
FreeOperators();
}    // namespace scout


#endif    // !SCOUT_MPIOPERATORS_H
