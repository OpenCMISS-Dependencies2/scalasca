/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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

#include "Threading.h"

#ifdef _OPENMP
    #include <omp.h>
#endif    // _OPENMP

using namespace pearl;


/*-------------------------------------------------------------------------*/
/**
 *  @file    Threading.cpp
 *  @ingroup PEARL_thread
 *  @brief   Implementation of the threading-related free functions.
 *
 *  This file provides the implementation of the threading-related free
 *  functions of the PEARL thread library.
 **/
/*-------------------------------------------------------------------------*/


// --- Threading-related functions ------------------------------------------

namespace pearl
{
int
PEARL_GetThreadNumber()
{
    #ifdef _OPENMP
        return omp_get_thread_num();
    #else
        return 0;
    #endif    // _OPENMP
}


int
PEARL_GetNumThreads()
{
    #ifdef _OPENMP
        return omp_get_num_threads();
    #else
        return 1;
    #endif
}


void
PEARL_Barrier()
{
    #ifdef _OPENMP
        #pragma omp barrier
    #endif    // _OPENMP
}
}    // namespace pearl
