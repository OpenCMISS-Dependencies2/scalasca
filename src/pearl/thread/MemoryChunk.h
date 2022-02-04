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
 *  @ingroup PEARL_thread
 *  @brief   Declaration of the class pearl::detail::MemoryChunk.
 *
 *  This header file provides the declaration of the internal helper class
 *  pearl::detail::MemoryChunk.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_MEMORYCHUNK_H
#define PEARL_MEMORYCHUNK_H


#include <stdint.h>

#include <cstddef>

#include <pearl/Uncopyable.h>


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MemoryChunk
 *  @ingroup PEARL_thread
 *  @brief   Manages a chunk of memory consisting of an integral number of
 *           fixed-sized memory blocks.
 *
 *  Instances of the class `%MemoryChunk` form the lowest layer of PEARL's
 *  @ref PEARL_memory "custom memory management for small objects".  Each
 *  `%MemoryChunk` instance manages a contiguous chunk of memory on the free
 *  store, in which an integral number of fixed-sized memory blocks can be
 *  allocated and deallocated with low overhead.
 *
 *  A memory chunk can be considered an array of fixed-sized memory blocks
 *  which is preceded by a header storing related metadata.  Unallocated
 *  blocks within a chunk are managed using a free list, where the first
 *  byte of an unused block stores the array index of the next free block.
 *  Using indices rather than pointers to build up the single-linked list
 *  of free blocks allows for block sizes smaller than `sizeof(void*)` and
 *  also avoids alignment issues (as bytes are always aligned).  As a
 *  downside, it limits the maximum number of blocks that can be managed
 *  by a single chunk to 256 blocks.  Therefore, each `%MemoryChunk` also
 *  provides a set of publicly accessible pointer variables to allow
 *  higher-level allocators such as pearl::detail::ChunkAllocator to manage
 *  a (theoretically) unbounded number of memory chunks, and thus also
 *  memory blocks.
 *
 *  @attention
 *      Basic parameters such as chunk and block sizes are not redundantly
 *      stored in each `%MemoryChunk` instance, but have to be passed down
 *      by a higher-level allocator where required.  While this reduces the
 *      per-chunk memory overhead, it also limits the amount of sanity
 *      checking that can be done within the `%MemoryChunk` member functions
 *      via assertions.  That is, **the caller is responsible for ensuring
 *      that values passed to member functions are valid and consistent.**
 *
 *  @note
 *      Defining the preprocessor macro `HAVE_VALGRIND_MEMCHECK_H` to `1`
 *      during compilation will enable instrumentation for Valgrind's
 *      memcheck tool.
 **/
/*-------------------------------------------------------------------------*/

class MemoryChunk
    : private pearl::Uncopyable
{
    public:
        // --- Public data members -------------------------

        /// Pointer to previous memory chunk in free list
        MemoryChunk* mPrev;

        /// Pointer to next memory chunk in free list
        MemoryChunk* mNext;

        /// Pointer to left child in pointer search tree
        MemoryChunk* mLeft;

        /// Pointer to right child in pointer search tree
        MemoryChunk* mRight;


        // --- Public member functions ---------------------

        /// @name Memory allocation functions
        /// @{

        /// @brief Allocate a single memory block.
        ///
        /// Allocates a single, uninitialized memory block of @p blockSize
        /// bytes from the chunk and returns a pointer to it.
        ///
        /// @param blockSize
        ///     Size of the requested memory block in bytes
        /// @returns
        ///     Pointer to the allocated memory block
        ///
        /// @pre
        ///     Number of available blocks in chunk > 0
        /// @pre
        ///     @p blockSize = block size specified during create()
        ///
        /// @exception_nothrow
        ///
        void*
        allocate(std::size_t blockSize);

        /// @brief Deallocate a single memory block.
        ///
        /// Deallocates the memory block of @p blockSize bytes pointed to
        /// by @p ptr, which must have been returned by a previous call to
        /// allocate() on the same `%MemoryChunk` instance.  Otherwise, or
        /// if `deallocate(ptr)` has been called before, the behavior is
        /// undefined.
        ///
        /// @param ptr
        ///     Pointer to the allocated memory block
        /// @param blockSize
        ///     Size of the memory block in bytes
        ///
        /// @pre
        ///     @p ptr &isin; chunk
        /// @pre
        ///     @p blockSize = block size specified during create()
        ///
        /// @exception_nothrow
        ///
        void
        deallocate(void*       ptr,
                   std::size_t blockSize);

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Get number of available blocks.
        ///
        /// Returns the total number of unallocated memory blocks in the
        /// chunk.
        ///
        /// @returns
        ///     Number of available blocks
        ///
        /// @exception_nothrow
        ///
        std::size_t
        availableBlocks() const;

        /// @}
        /// @name Comparison functions
        /// @{

        /// @brief Less-than comparison with pointer.
        ///
        /// Returns @e true if the entire memory chunk of size @p chunkSize
        /// compares "less than" the pointer @p ptr and @e false otherwise.
        ///
        /// @param ptr
        ///     Pointer to compare with
        /// @param chunkSize
        ///     Size of the memory chunk in bytes
        /// @returns
        ///     @e true if the chunk is "less than" @p ptr, @e false otherwise
        ///
        /// @pre
        ///     @p chunkSize = chunk size specified during create()
        ///
        /// @exception_nothrow
        ///
        bool
        isLessThan(const void* ptr,
                   std::size_t chunkSize) const;

        /// @brief Greater-than comparison with pointer.
        ///
        /// Returns @e true if the entire memory chunk of size @p chunkSize
        /// compares "greater than" the pointer @p ptr and @e false otherwise.
        ///
        /// @param ptr
        ///     Pointer to compare with
        /// @param chunkSize
        ///     Size of the memory chunk in bytes
        /// @returns
        ///     @e true if the chunk is "greater than" @p ptr, @e false
        ///     otherwise
        ///
        /// @pre
        ///     @p chunkSize = chunk size specified during create()
        ///
        /// @exception_nothrow
        ///
        bool
        isGreaterThan(const void* ptr,
                      std::size_t chunkSize) const;

        /// @}


        // --- Static public member functions --------------

        /// @name Creation & destruction functions
        /// @{

        /// @brief Creates a new memory chunk.
        ///
        /// Allocates a contiguous chunk of memory of @p chunkSize bytes
        /// on the free store and initializes it such that it can handle
        /// allocations of fixed-sized memory blocks of @p blockSize bytes.
        ///
        /// @param chunkSize
        ///     Size of the entire memory chunk in bytes
        /// @param blockSize
        ///     Size of each individual memory block in bytes
        ///
        /// @pre
        ///     @p chunkSize &gt; payloadOffset()
        /// @pre
        ///     0 &lt; @p blockSize &le; @p chunkSize - payloadOffset()
        /// @pre
        ///     @p chunkSize - payloadOffset() / @p blockSize &le; 256
        ///
        /// @exception_strong
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate the memory chunk
        ///
        static MemoryChunk*
        create(std::size_t chunkSize,
               std::size_t blockSize);

        /// @brief Destroys a memory chunk.
        ///
        /// Destroys the memory chunk pointed to by @p chunk, which must have
        /// been returned by a previous call to create().
        ///
        /// @param chunk
        ///     Memory chunk to be destroyed
        ///
        /// @exception_nothrow
        ///
        static void
        destroy(MemoryChunk* chunk);

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Get capacity of a memory chunk.
        ///
        /// Returns the maximum number of memory blocks of size @p blockSize
        /// bytes a memory chunk of @p chunkSize bytes can store.
        ///
        /// @param chunkSize
        ///     Size of the entire memory chunk in bytes
        /// @param blockSize
        ///     Size of each individual memory block in bytes
        /// @returns
        ///     Number of memory blocks the chunk can hold
        ///
        /// @pre
        ///     @p chunkSize &gt; payloadOffset()
        /// @pre
        ///     0 &lt; @p blockSize &le; @p chunkSize - payloadOffset()
        /// @pre
        ///     @p chunkSize - payloadOffset() / @p blockSize &le; 256
        ///
        /// @exception_nothrow
        ///
        static std::size_t
        capacity(std::size_t chunkSize,
                 std::size_t blockSize);

        /// @}


    private:
        // --- Data members --------------------------------

        /// Number of unallocated blocks
        uint16_t mAvailableBlocks;

        /// Index of first unallocated block
        uint8_t mFirstAvailableBlock;


        // --- Private member functions --------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Initializes a contiguous chunk of memory of @p chunkSize bytes
        /// starting at the base address of the instance such that it can
        /// handle allocations of fixed-size memory blocks of @p blockSize
        /// bytes.  This constructor has to be used in conjunction with
        /// "placement new".
        ///
        /// @param chunkSize
        ///     Size of the entire memory chunk in bytes
        /// @param blockSize
        ///     Size of each individual memory block in bytes
        ///
        /// @pre
        ///     0 &lt; @p blockSize &le; @p chunkSize - payloadOffset()
        ///
        /// @exception_nothrow
        ///
        MemoryChunk(std::size_t chunkSize,
                    std::size_t blockSize);

        /// @brief Destructor.
        ///
        /// Destroys the memory chunk starting at the base address of the
        /// instance.
        ///
        /// @exception_nothrow
        ///
        ~MemoryChunk();

        /// @}


        // --- Static private member functions -------------

        /// @name Query functions
        /// @{

        /// @brief Get memory offset of payload data.
        ///
        /// Returns the memory offset of the chunk's payload data relative
        /// to the chunk start address in bytes.
        ///
        /// @returns
        ///     Payload data offset in bytes
        ///
        /// @exception_nothrow
        ///
        /// @todo [C++11] Make function `constexpr`
        ///
        static std::size_t
        payloadOffset();

        /// @}
};
}    // namespace pearl::detail
}    // namespace pearl


// --- Inline member functions ----------------------------------------------

#include "MemoryChunk-inl.h"


#endif    // !PEARL_MEMORYCHUNK_H
