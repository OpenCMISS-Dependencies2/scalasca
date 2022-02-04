/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class pearl::SmallObject.
 *
 *  This file provides the implementation of the class pearl::SmallObject.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/SmallObject.h>

#include "pearl_memory.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor (protected) --------------------------------

/// Prevents polymorphic deletion (see @cite Sutter:2001), thus saving the
/// vtable pointer space if it is not needed by derived classes.
SmallObject::~SmallObject()
{
}


// --- Memory allocation operators ------------------------------------------

void*
SmallObject::operator new(const size_t objectSize)
{
    return memoryAllocate(objectSize);
}


void
SmallObject::operator delete(void* const  deadObject,
                             const size_t objectSize)
{
    memoryDeallocate(deadObject, objectSize);
}
