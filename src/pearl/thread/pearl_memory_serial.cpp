/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_thread
 *  @brief   Implementation of custom memory management functions for
 *           single-threaded codes.
 *
 *  This header file provides the implementation of the PEARL-internal
 *  custom memory management functions for single-threaded codes.  See
 *  pearl_memory.h for a detailed description of the API.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "pearl_memory.h"

#include <UTILS_Error.h>

#include "SmallBlockAllocator.h"

using namespace std;
using namespace pearl::detail;


// --- Helpers --------------------------------------------------------------

namespace
{
// --- Global variables ------------------------------------

/// Single allocator used by the memory management
///
/// @todo [C++11] Initialize with `nullptr`
SmallBlockAllocator* smallBlockAllocator = 0;
}    // unnamed namespace


// --- Memory management: Environmental management --------------------------

/// @todo [C++11] Reset `smallBlockAllocator` to `nullptr`
void
pearl::detail::memoryFinalize()
{
    delete smallBlockAllocator;
    smallBlockAllocator = 0;
}


// --- Memory management: Allocation functions ------------------------------

/// @todo [C++11] Compare `smallBlockAllocator` with `nullptr`
void*
pearl::detail::memoryAllocate(const std::size_t blockSize)
{
    if (smallBlockAllocator == 0)
    {
        smallBlockAllocator = new SmallBlockAllocator;
    }

    return smallBlockAllocator->allocate(blockSize);
}


/// @todo [C++11] Assertion: compare `smallBlockAllocator` with `nullptr`
void
pearl::detail::memoryDeallocate(void* const       ptr,
                                const std::size_t blockSize)
{
    UTILS_ASSERT(smallBlockAllocator != 0);

    smallBlockAllocator->deallocate(ptr, blockSize);
}
