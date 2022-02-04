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
 *  @brief   Implementation of the class pearl::detail::SmallBlockAllocator.
 *
 *  This file provides the implementation of the internal helper class
 *  pearl::detail::SmallBlockAllocator.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "SmallBlockAllocator.h"

#include <algorithm>
#include <new>

#include <UTILS_Error.h>

#include "ChunkAllocator.h"
#include "Functors.h"

using namespace std;
using namespace pearl::detail;


// --- Helpers --------------------------------------------------------------

namespace
{
// --- Constants -------------------------------------------

/// Maximum memory block size handled by `%SmallBlockAllocator`
///
/// @todo [C++11] Use `constexpr` instead of `const`
const size_t MAX_BLOCK_SIZE = 64;

/// Minimum enforced alignment used by `%SmallBlockAllocator`
///
/// @todo [C++11] Use `constexpr` instead of `const`
const size_t MIN_ALIGNMENT = 4;

/// Number of required `%ChunkAllocator` instances
///
/// @todo [C++11] Use `constexpr` instead of `const`
const size_t NUM_ALLOCATORS = MAX_BLOCK_SIZE / MIN_ALIGNMENT;
}    // unnamed namespace


// --- Constructors & destructor --------------------------------------------

/// @todo [C++11] Pass `nullptr` to constructor of `mAllocators`
SmallBlockAllocator::SmallBlockAllocator()
    : mAllocators(NUM_ALLOCATORS, 0)
{
}


/// @todo [C++11] Use `std::default_delete` from `<memory>` instead of
///     `delete_ptr` (allows to remove the "Functors.h" include and the
///     corresponding include path from `Makefile.inc.am`)
SmallBlockAllocator::~SmallBlockAllocator()
{
    for_each(mAllocators.begin(), mAllocators.end(),
             delete_ptr< ChunkAllocator >());
}


// --- Memory allocation functions ------------------------------------------

/// @todo [C++11] Compare `mAllocators[index]` with `nullptr`
void*
SmallBlockAllocator::allocate(const size_t blockSize)
{
    // Let the default allocator handle "extreme" cases
    if (  (blockSize == 0)
       || (blockSize > MAX_BLOCK_SIZE))
    {
        return ::operator new(blockSize);
    }

    // Create chunk allocator if necessary
    const size_t index = (blockSize + MIN_ALIGNMENT - 1) / MIN_ALIGNMENT;
    if (mAllocators[index] == 0)
    {
        mAllocators[index] = new ChunkAllocator(index * MIN_ALIGNMENT);
    }

    // Allocate block from chunk allocator
    return mAllocators[index]->allocate();
}


/// @todo [C++11]
///     - Compare `ptr` with `nullptr`
///     - Assertion: compare `mAllocators[index]` with `nullptr`
void
SmallBlockAllocator::deallocate(void* const       ptr,
                                const std::size_t blockSize)
{
    // Let the default allocator handle "extreme" cases
    if (  (ptr == 0)
       || (blockSize == 0)
       || (blockSize > MAX_BLOCK_SIZE))
    {
        ::operator delete(ptr);

        return;
    }

    // Deallocate block to chunk allocator for block size
    const size_t index = (blockSize + MIN_ALIGNMENT - 1) / MIN_ALIGNMENT;
    UTILS_ASSERT(mAllocators[index] != 0);
    mAllocators[index]->deallocate(ptr);
}
