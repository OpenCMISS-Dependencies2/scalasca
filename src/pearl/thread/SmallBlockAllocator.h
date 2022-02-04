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
 *  @brief   Declaration of the class pearl::detail::SmallBlockAllocator.
 *
 *  This header file provides the declaration of the internal helper class
 *  SmallBlockAllocator.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_SMALLBLOCKALLOCATOR_H
#define PEARL_SMALLBLOCKALLOCATOR_H


#include <cstddef>
#include <vector>

#include <pearl/Uncopyable.h>


namespace pearl
{
namespace detail
{
// --- Forward declarations -------------------------------------------------

class ChunkAllocator;


/*-------------------------------------------------------------------------*/
/**
 *  @class   SmallBlockAllocator
 *  @ingroup PEARL_thread
 *  @brief   Allocator for variable-sized small memory blocks.
 *
 *  The class `%SmallBlockAllocator` is part of PEARL's @ref PEARL_memory
 *  "custom memory management for small objects".  Each `%SmallBlockAllocator`
 *  instance manages a (theoretically) unbounded number of variable-sized
 *  memory blocks on the free store.
 *
 *  A `%SmallBlockAllocator` instance handles allocation requests of up to
 *  64 bytes using a set of pearl::detail::ChunkAllocator instances, while
 *  larger allocation requests are transparently forwarded to the default
 *  memory allocator `operator new`.  To reduce the number of required
 *  `%ChunkAllocator` instances, a minimum alignment of 4 bytes is enforced.
 **/
/*-------------------------------------------------------------------------*/

class SmallBlockAllocator
    : private pearl::Uncopyable
{
    public:
        // --- Public member functions ---------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new small block allocator which is capable of managing
        /// allocations of variable-sized memory blocks.
        ///
        /// @exception_strong
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate memory for internal data structures
        ///
        SmallBlockAllocator();

        /// @brief Destructor.
        ///
        /// Deletes all internal data structures and destroys the small block
        /// allocator.
        ///
        /// @exception_nothrow
        ///
        ~SmallBlockAllocator();

        /// @}
        /// @name Memory allocation functions
        /// @{

        /// @brief Allocate a single memory block.
        ///
        /// Allocates a single, uninitialized memory block of @p blockSize
        /// bytes and returns a pointer to it.
        ///
        /// @param blockSize
        ///     Size of the requested memory block (in bytes)
        /// @returns
        ///     Pointer to the allocated memory block
        ///
        /// @exception_safety
        ///     If a chunk allocator for @p blockSize already exists, the
        ///     allocator remains unchanged in case of exceptions (strong
        ///     guarantee).  Otherwise the allocator is guaranteed to end
        ///     in a valid state (basic guarantee).
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate a new memory block or any of the
        ///     required internal data structures
        ///
        void*
        allocate(std::size_t blockSize);

        /// @brief Deallocate a single memory block.
        ///
        /// Deallocates the memory block of @p blockSize bytes pointed to
        /// by @p ptr, which must have been returned by a previous call to
        /// allocate() on the same `%SmallBlockAllocator` instance.
        /// Otherwise, or if `deallocate(ptr, blockSize)` has been called
        /// before, the behavior is undefined.
        ///
        /// @param ptr
        ///     Pointer to the allocated memory block
        /// @param blockSize
        ///     Size of the allocated memory block (in bytes)
        ///
        /// @exception_nothrow
        ///
        void
        deallocate(void*       ptr,
                   std::size_t blockSize);

        /// @}


    private:
        // --- Data members --------------------------------

        /// Underlying fixed-sized memory allocators
        std::vector< ChunkAllocator* > mAllocators;
};
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_SMALLBLOCKALLOCATOR_H
