/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2018                                                **
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
 *  @brief   Implementation of the class pearl::detail::ChunkAllocator.
 *
 *  This file provides the implementation of the internal helper class
 *  pearl::detail::ChunkAllocator.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "ChunkAllocator.h"

#include <UTILS_Error.h>

#include "MemoryChunk.h"
#include "MemoryChunkTree.h"

using namespace std;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

/// @todo [C++11] Initialize `mFreeList` and `mChunkTree` with `nullptr`
ChunkAllocator::ChunkAllocator(const size_t blockSize)
    : mFreeList(0),
      mChunkTree(0),
      mBlockSize(blockSize)
{
    UTILS_ASSERT(mBlockSize > 0);
    UTILS_ASSERT(mBlockSize <= 1024);

    // Determine chunk size based on block size
    if (mBlockSize < 4)
    {
        mChunkSize = 256;
    }
    else if (mBlockSize < 16)
    {
        mChunkSize = 1024;
    }
    else
    {
        mChunkSize = 4096;
    }
}


// --- Memory allocation functions ------------------------------------------

/// @todo [C++11]
///     - Compare `mFreeList` against `nullptr` (3x)
///     - Assign `nullptr` to `mPrev` (2x) and `mNext` (1x)
void*
ChunkAllocator::allocate()
{
    // Empty free list => allocate and track new chunk
    if (mFreeList == 0)
    {
        mFreeList        = MemoryChunk::create(mChunkSize, mBlockSize);
        mFreeList->mPrev = 0;
        mFreeList->mNext = 0;

        mChunkTree = insert(mChunkTree, mFreeList, mChunkSize);
    }

    UTILS_ASSERT(mFreeList != 0);
    UTILS_ASSERT(mFreeList->availableBlocks() > 0);

    // Allocate block from chunk at head of free list
    void* const block = mFreeList->allocate(mBlockSize);

    // All blocks allocated => remove chunk from free list
    if (mFreeList->availableBlocks() == 0)
    {
        mFreeList = mFreeList->mNext;
        if (mFreeList != 0)
        {
            mFreeList->mPrev = 0;
        }
    }

    return block;
}


/// @todo [C++11]
///     - Assertions: Compare `ptr` and `mChunkTree` with `nullptr`
///     - Compare `mFreeList`, `mNext`, and `mPrev` with `nullptr` (1x each)
///     - Assign `nullptr` to `mPrev`
void
ChunkAllocator::deallocate(void* const ptr)
{
    UTILS_ASSERT(ptr != 0);
    UTILS_ASSERT(mChunkTree != 0);

    // Move chunk containing ptr to root of search tree
    mChunkTree = splay(mChunkTree, ptr, mChunkSize);

    UTILS_ASSERT(!(  mChunkTree->isLessThan(ptr, mChunkSize)
                  || mChunkTree->isGreaterThan(ptr, mChunkSize)));

    // Deallocate block within chunk at tree root
    mChunkTree->deallocate(ptr, mBlockSize);

    // Deallocation from full chunk => prepend chunk to free list
    const size_t avail = mChunkTree->availableBlocks();
    if (avail == 1)
    {
        if (mFreeList != 0)
        {
            mFreeList->mPrev = mChunkTree;
        }
        mChunkTree->mPrev = 0;
        mChunkTree->mNext = mFreeList;
        mFreeList         = mChunkTree;
    }

    // Chunk empty => remove from free list & search tree, then destroy
    if (avail == MemoryChunk::capacity(mChunkSize, mBlockSize))
    {
        if (mChunkTree->mPrev != 0)
        {
            mChunkTree->mPrev->mNext = mChunkTree->mNext;
        }
        if (mChunkTree->mNext != 0)
        {
            mChunkTree->mNext->mPrev = mChunkTree->mPrev;
        }
        if (mFreeList == mChunkTree)
        {
            mFreeList = mFreeList->mNext;
        }

        MemoryChunk* const chunk = mChunkTree;
        mChunkTree = remove(mChunkTree, mChunkSize);

        MemoryChunk::destroy(chunk);
    }
}
