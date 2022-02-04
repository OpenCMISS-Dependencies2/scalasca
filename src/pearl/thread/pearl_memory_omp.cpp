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
 *  @brief   OpenMP-specific mplementation of custom memory management
 *           functions.
 *
 *  This header file provides the OpenMP-specific implementation of the
 *  PEARL-internal custom memory management functions.  See pearl_memory.h
 *  for a detailed description of the API.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "pearl_memory.h"

#include <algorithm>
#include <vector>

#include <UTILS_Error.h>

#include <pearl/ScopedPtr.h>

#include "Functors.h"
#include "SmallBlockAllocator.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Helpers --------------------------------------------------------------

namespace
{
// --- Global variables ------------------------------------

/// Thread-local allocator used by the memory management
///
/// @todo [C++11] Initialize with `nullptr`
#ifdef __FUJITSU
    // Due to issues with threadprivate OpenMP variables on K/FX10, we use the
    // non-standard '__thread' storage class keyword (GNU extension, requires
    // '-Xg' compiler flag) on this platform.
    __thread SmallBlockAllocator* tls_allocator = 0;
#else    // !__FUJITSU
    SmallBlockAllocator* tls_allocator = 0;
    #pragma omp threadprivate(tls_allocator)
#endif    // !__FUJITSU

/// Vector keeping track of all thread-local small block allocators
vector< SmallBlockAllocator* > smallBlockAllocators;
}    // unnamed namespace


// --- Memory management: Environmental management --------------------------

/// @todo [C++11] Use `std::default_delete` from `<memory>` instead of
///     `delete_ptr` (allows to remove the "Functors.h" include and the
///     corresponding include path from `Makefile.inc.am`)
void
pearl::detail::memoryFinalize()
{
    #pragma omp critical
    {
        for_each(smallBlockAllocators.begin(), smallBlockAllocators.end(),
                 delete_ptr< SmallBlockAllocator >());
        smallBlockAllocators.clear();
    }
}


// --- Memory management: Allocation functions ------------------------------

/// @todo [C++11]
///     - Compare `tls_allocator` with `nullptr`
///     - Use `std::unique_ptr` from `<memory>` instead of `ScopedPtr`
///       (allows to remove the `<pearl/ScopedPtr.h>` include and using the
///       `pearl` namespace)
void*
pearl::detail::memoryAllocate(const std::size_t blockSize)
{
    // Create small block allocator if necessary
    if (tls_allocator == 0)
    {
        ScopedPtr< SmallBlockAllocator > instance(new SmallBlockAllocator);
        #pragma omp critical
        {
            smallBlockAllocators.push_back(instance.get());
        }
        tls_allocator = instance.release();
    }

    return tls_allocator->allocate(blockSize);
}


/// @todo [C++11] Assertion: compare `tls_allocator` with `nullptr`
void
pearl::detail::memoryDeallocate(void* const       ptr,
                                const std::size_t blockSize)
{
    UTILS_ASSERT(tls_allocator != 0);

    tls_allocator->deallocate(ptr, blockSize);
}
