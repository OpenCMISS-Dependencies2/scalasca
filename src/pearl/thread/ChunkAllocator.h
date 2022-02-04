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
 *  @brief   Declaration of the class pearl::detail::ChunkAllocator.
 *
 *  This header file provides the declaration of the internal helper class
 *  pearl::detail::ChunkAllocator.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_CHUNKALLOCATOR_H
#define PEARL_CHUNKALLOCATOR_H


#include <cstddef>

#include <pearl/Uncopyable.h>


namespace pearl
{
namespace detail
{
// --- Forward declarations -------------------------------------------------

class MemoryChunk;


/*-------------------------------------------------------------------------*/
/**
 *  @class   ChunkAllocator
 *  @ingroup PEARL_thread
 *  @brief   Allocator for fixed-sized memory blocks, allocating memory in
 *           larger chunks.
 *
 *  The class `%ChunkAllocator` is part of PEARL's @ref PEARL_memory
 *  "custom memory management for small objects".  Each `%ChunkAllocator`
 *  instance manages a (theoretically) unbounded number of fixed-sized
 *  memory blocks on the free store.
 *
 *  A `%ChunkAllocator` instance uses pearl::detail::MemoryChunk as the
 *  underlying basic block to manage the allocation and deallocation of
 *  fixed-sized memory blocks.  Since `%MemoryChunk` instances can only
 *  handle a fixed number of blocks, `%ChunkAllocator` keeps track of a
 *  set of `%MemoryChunk` objects in a self-adjusting binary search tree
 *  (bottom-up splay tree, see @cite SleatorTarjan:1985).  This allows to
 *  efficiently retrieve the memory chunk containing a particular pointer
 *  during deallocation, as this information is not directly available
 *  from an allocated memory block.  In addition, all memory chunks with
 *  at least one unallocated block are organized in a double-linked free
 *  list for efficient block allocations.
 **/
/*-------------------------------------------------------------------------*/

class ChunkAllocator
    : private pearl::Uncopyable
{
    public:
        // --- Public member functions ---------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new chunk allocator which is capable of managing
        /// allocations of fixed-sized memory blocks of @p blockSize bytes
        /// on the free store.
        ///
        /// @param blockSize
        ///     Size of the managed memory blocks in bytes
        ///
        /// @pre
        ///     0 < @p blockSize &le; 1024
        ///
        /// @exception_nothrow
        ///
        explicit
        ChunkAllocator(std::size_t blockSize);

        /// @}
        /// @name Memory allocation functions
        /// @{

        /// @brief Allocate a single memory block.
        ///
        /// Allocates a single, uninitialized memory block and returns a
        /// pointer to it.
        ///
        /// @returns
        ///     Pointer to the allocated memory block
        ///
        /// @exception_strong
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate a new memory chunk
        ///
        void*
        allocate();

        /// @brief Deallocate a single memory block.
        ///
        /// Deallocates the memory block pointed to by @p ptr, which must
        /// have been returned by a previous call to allocate() on the same
        /// `%ChunkAllocator` instance.  Otherwise, or if `deallocate(ptr)`
        /// has been called before, the behavior is undefined.
        ///
        /// @param ptr
        ///     Pointer to the allocated memory block
        ///
        /// @exception_nothrow
        ///
        void
        deallocate(void* ptr);

        /// @}


    private:
        // --- Data members --------------------------------

        /// Head of the free list storing memory chunks with at least one
        /// unallocated memory block
        MemoryChunk* mFreeList;

        /// Root node of the memory chunk search tree
        MemoryChunk* mChunkTree;

        /// Size of the managed memory chunks (in bytes)
        std::size_t mChunkSize;

        /// Size of the managed fixed-size memory blocks (in bytes)
        std::size_t mBlockSize;
};
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_CHUNKALLOCATOR_H
