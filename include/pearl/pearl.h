/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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


#ifndef PEARL_PEARL_H
#define PEARL_PEARL_H


/*-------------------------------------------------------------------------*/
/**
 *  @file    pearl.h
 *  @ingroup PEARL_base
 *  @ingroup PEARL_mpi
 *  @brief   Declarations of global library functions.
 *
 *  This header file provides declarations of global library functions and
 *  support routines. It includes declarations for both, generic as well as
 *  programming-model-specific routines.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class GlobalDefs;
class LocalTrace;


// --- Function prototypes --------------------------------------------------

/// @name Library setup & teardown
/// @{

void
PEARL_init();

void
PEARL_mpi_init();

void
PEARL_finalize();

/// @}
/// @name Call tree verification
/// @{

void
PEARL_verify_calltree(GlobalDefs& defs,
                      LocalTrace& trace);

/// @}
/// @name Call tree unification
/// @{

void
PEARL_mpi_unify_calltree(GlobalDefs& defs);

/// @}
/// @name Trace preprocessing
/// @{

void
PEARL_preprocess_trace(const GlobalDefs& defs,
                       const LocalTrace& trace);

/// @}
}    // namespace pearl


#endif    // !PEARL_PEARL_H
