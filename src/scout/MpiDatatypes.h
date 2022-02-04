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


#ifndef SCOUT_MPIDATATYPES_H
#define SCOUT_MPIDATATYPES_H


#include <mpi.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file  MpiDatatypes.h
 *  @brief Declaration of MPI datatypes and related functions.
 *
 *  This header file provides the declaration of SCOUT-specific MPI datatypes
 *  and related functions.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Global variables -----------------------------------------------------

extern MPI_Datatype TIMEVEC2;
extern MPI_Datatype TOPSEVERE;
extern MPI_Datatype TOPSEVEREARRAY;
extern MPI_Datatype TOPSEVERECOLL;


// --- Related functions ----------------------------------------------------

void
InitDatatypes();

void
FreeDatatypes();
}    // namespace scout


#endif    // !SCOUT_MPIDATATYPES_H
