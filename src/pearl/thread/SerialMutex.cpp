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

#include <cstddef>

using namespace std;
using namespace pearl;


/*-------------------------------------------------------------------------*/
/**
 *  @file    SerialMutex.cpp
 *  @ingroup PEARL_thread
 *  @brief   Serial implementation of the class Mutex.
 *
 *  This file provides the serial implementation of the class Mutex. All
 *  operations are basically no-ops, as no thread synchroniozation is
 *  necessary in the serial case.
 **/
/*-------------------------------------------------------------------------*/


// --- Constructors & destructor --------------------------------------------

Mutex::Mutex()
    : mMutex(NULL)
{
}


Mutex::~Mutex()
{
}


// --- Mutex functionality --------------------------------------------------

void
Mutex::lock()
{
}


void
Mutex::unlock()
{
}
