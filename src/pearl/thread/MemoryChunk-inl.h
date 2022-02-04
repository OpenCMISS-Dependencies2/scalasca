/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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


#ifndef PEARL_MEMORYCHUNK_H
    #error "Do not include this file!  Use 'MemoryChunk.h' instead."
#endif


#include <functional>
#include <new>

#include <UTILS_Error.h>

#include "wrap-memcheck.h"


namespace pearl
{
namespace detail
{
// --- Memory allocation routines (inline) ----------------------------------

inline void*
MemoryChunk::allocate(const std::size_t blockSize)
{
    UTILS_ASSERT(blockSize > 0);
    UTILS_ASSERT(mAvailableBlocks > 0);

    // Determine block pointer
    void* const    chunkPtr = static_cast< void* >(this);
    uint8_t* const block    = static_cast< uint8_t* >(chunkPtr)
                              + MemoryChunk::payloadOffset()
                              + (mFirstAvailableBlock * blockSize);

    // Valgrind: record block allocation and mark its memory as UNDEFINED,
    // except for the first byte to avoid "use of uninitialized value" below
    VALGRIND_MALLOCLIKE_BLOCK(block, blockSize, 0, 0);
    VALGRIND_MAKE_MEM_DEFINED(block, sizeof(uint8_t));

    // Update chunk metadata
    mFirstAvailableBlock = *block;
    --mAvailableBlocks;

    // Valgrind: mark first byte of block also as UNDEFINED
    VALGRIND_MAKE_MEM_UNDEFINED(block, sizeof(uint8_t));

    return block;
}


inline void
MemoryChunk::deallocate(void* const       ptr,
                        const std::size_t blockSize)
{
    UTILS_ASSERT(blockSize > 0);

    // Determine block and payload base pointer
    uint8_t* const block    = static_cast< uint8_t* >(ptr);
    void* const    chunkPtr = static_cast< void* >(this);
    uint8_t* const base     = static_cast< uint8_t* >(chunkPtr)
                              + MemoryChunk::payloadOffset();

    UTILS_ASSERT(std::less_equal< uint8_t* >()(base, block));

    // Calculate block index within chunk
    const std::ptrdiff_t offset = block - base;
    const std::size_t    index  = offset / blockSize;

    UTILS_ASSERT((offset % blockSize) == 0);
    UTILS_ASSERT(index < 256);

    // Update chunk metadata
    *block               = mFirstAvailableBlock;
    mFirstAvailableBlock = index;
    ++mAvailableBlocks;

    // Valgrind: record block deallocation and mark its memory as NOACCESS
    VALGRIND_FREELIKE_BLOCK(block, 0);
}


// --- Query functions (inline) ---------------------------------------------

inline std::size_t
MemoryChunk::availableBlocks() const
{
    return mAvailableBlocks;
}


// --- Comparison functions (inline) ----------------------------------------

inline bool
MemoryChunk::isLessThan(const void* const ptr,
                        const std::size_t chunkSize) const
{
    UTILS_ASSERT(chunkSize > MemoryChunk::payloadOffset());

    // Determine payload end pointer (points to address right *after* chunk)
    const void* const    chunkPtr = static_cast< const void* >(this);
    const uint8_t* const chunkEnd = static_cast< const uint8_t* >(chunkPtr)
                                    + chunkSize;

    return std::less_equal< const void* >()(chunkEnd, ptr);
}


inline bool
MemoryChunk::isGreaterThan(const void* const ptr,
                           const std::size_t) const
{
    return std::greater< const void* >()(this, ptr);
}


// --- Creation & destruction (static, inline) ------------------------------

inline MemoryChunk*
MemoryChunk::create(const std::size_t chunkSize,
                    const std::size_t blockSize)
{
    UTILS_ASSERT(chunkSize > MemoryChunk::payloadOffset());

    // Allocate raw memory
    void* ptr = operator new(chunkSize);

    // Construct memory chunk using "placement new"
    return new (ptr)MemoryChunk(chunkSize, blockSize);
}


/// @todo [C++11] Assertion: Compare `chunk` with `nullptr`
inline void
MemoryChunk::destroy(MemoryChunk* const chunk)
{
    UTILS_ASSERT(chunk != 0);

    // Destroy memory chunk
    chunk->~MemoryChunk();

    // Deallocate raw memory
    void* const ptr = static_cast< void* >(chunk);
    operator delete(ptr);
}


// --- Query functions (static, inline) -------------------------------------

inline std::size_t
MemoryChunk::capacity(const std::size_t chunkSize,
                      const std::size_t blockSize)
{
    UTILS_ASSERT(chunkSize > MemoryChunk::payloadOffset());
    UTILS_ASSERT(blockSize > 0);
    UTILS_ASSERT(blockSize <= (chunkSize - MemoryChunk::payloadOffset()));

    const size_t capacity = (chunkSize - MemoryChunk::payloadOffset())
                            / blockSize;

    UTILS_ASSERT(capacity <= 256);

    return capacity;
}


// --- Constructors & destructor (private, inline) --------------------------

inline
MemoryChunk::MemoryChunk(const std::size_t chunkSize,
                         const std::size_t blockSize)
    : mAvailableBlocks(MemoryChunk::capacity(chunkSize, blockSize)),
      mFirstAvailableBlock(0)
{
    // Determine base pointer
    void* const    chunkPtr = static_cast< void* >(this);
    uint8_t* const base     = static_cast< uint8_t* >(chunkPtr)
                              + MemoryChunk::payloadOffset();

    // Initialize chunk-internal free list
    // ("next"-index of last block is left uninitialized to avoid overflow)
    for (std::size_t index = 0; index < (mAvailableBlocks - 1); ++index)
    {
        base[index * blockSize] = index + 1;
    }

    // Valgrind: mark payload memory as NOACCESS
    VALGRIND_MAKE_MEM_NOACCESS(base, chunkSize - MemoryChunk::payloadOffset());
}


inline
MemoryChunk::~MemoryChunk()
{
}


// --- Query functions (static, private, inline) ----------------------------

inline std::size_t
MemoryChunk::payloadOffset()
{
    const std::size_t ALIGNMENT = 8;

    return (((sizeof(MemoryChunk) + (ALIGNMENT - 1)) / ALIGNMENT) * ALIGNMENT);
}
}    // namespace pearl::detail
}    // namespace pearl
