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
 *  @brief   Declarations of custom memory management functions.
 *
 *  This header file provides the declarations of custom memory management
 *  functions used internally by the PEARL library.  This layer abstracts
 *  from the different implementations for the supported multi-threaded
 *  programming paradigms and allows for client code to be written in a
 *  generic way.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_PEARL_MEMORY_H
#define PEARL_PEARL_MEMORY_H


#include <cstddef>


namespace pearl
{
namespace detail
{
// --- Function prototypes --------------------------------------------------

/// @name Memory management: Environmental management
/// @{

/// @brief Initialize PEARL's custom memory management layer.
///
/// Initializes the PEARL-internal custom memory management and installs a
/// custom out-of-memory handler.  This routine must be called in a
/// single-threaded context before any PEARL memory management routine is
/// called.
///
void
memoryInit();

/// @brief Finalize PEARL's custom memory management layer.
///
/// Cleans up all PEARL-internal custom memory management state.  It must be
/// called in a single-threaded context.  Afterwards, no further PEARL memory
/// management routines may be called.
///
/// @exception_nothrow
///
void
memoryFinalize();

/// @}
/// @name Memory management: Allocation functions
/// @{

/// @brief Allocate a single memory block.
///
/// Allocates a single, uninitialized memory block of @p blockSize bytes and
/// returns a pointer to it.
///
/// @param blockSize
///     Size of the requested memory block (in bytes)
/// @returns
///     Pointer to the allocated memory block
///
/// @exception_safety
///     If no (re-)allocations happen, the global state remains unchanged in
///     case of exceptions (strong guarantee).  Otherwise the global state is
///     guaranteed to end in a valid state (basic guarantee).
///
/// @throws std::bad_alloc
///     on failure to allocate a new memory block or any of the required
///     internal data structures
///
void*
memoryAllocate(std::size_t blockSize);

/// @brief Deallocate a single memory block.
///
/// Deallocates the memory block of @p blockSize bytes pointed to by @p ptr,
/// which must have been returned by a previous call to memoryAllocate() on
/// the same thread.  Otherwise, or if `memoryDeallocate(ptr)` has been
/// called before, the behavior is undefined.
///
/// @param ptr
///     Pointer to the allocated memory block
/// @param blockSize
///     Size of the allocated memory block (in bytes)
///
/// @exception_nothrow
///
void
memoryDeallocate(void*       ptr,
                 std::size_t blockSize);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_PEARL_MEMORY_H
