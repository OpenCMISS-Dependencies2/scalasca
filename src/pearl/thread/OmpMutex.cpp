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

#include "Threading.h"

#include <omp.h>

using namespace pearl;


/*-------------------------------------------------------------------------*/
/**
 *  @file    OmpMutex.cpp
 *  @ingroup PEARL_thread
 *  @brief   OpenMP implementation of the class Mutex.
 *
 *  This file provides the OpenMP implementation of the class Mutex.
 **/
/*-------------------------------------------------------------------------*/


// --- Private data structures ----------------------------------------------

struct Mutex::mutex_t
{
    /// Underlying OpenMP lock
    omp_lock_t mLock;
};


// --- Constructors & destructor --------------------------------------------

Mutex::Mutex()
    : mMutex(new mutex_t)
{
    omp_init_lock(&mMutex->mLock);
}


Mutex::~Mutex()
{
    omp_destroy_lock(&mMutex->mLock);
    delete mMutex;
}


// --- Mutex functionality --------------------------------------------------

void
Mutex::lock()
{
    omp_set_lock(&mMutex->mLock);
}


void
Mutex::unlock()
{
    omp_unset_lock(&mMutex->mLock);
}
