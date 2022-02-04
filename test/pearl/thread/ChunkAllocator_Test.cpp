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


#include <config.h>

#include "ChunkAllocator.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <vector>

#include <gtest/gtest.h>

using namespace std;
using namespace testing;
using namespace pearl::detail;


namespace
{
// --- Constants ------------------------------------------------------------

// Number of blocks to allocate; this fills at least 8 chunks
static const size_t MAX_BLOCKS = 2048;


// --- Helpers --------------------------------------------------------------

// Test fixture used for chunk allocator tests
class ChunkAllocatorT
    : public TestWithParam< size_t >
{
    public:
        ChunkAllocatorT()
        {
            const size_t& blockSize = GetParam();

            mAllocator = new ChunkAllocator(blockSize);
            mBlocks.reserve(MAX_BLOCKS);
        }

        ~ChunkAllocatorT()
        {
            // Deallocate in LIFO order
            while (!mBlocks.empty())
            {
                mAllocator->deallocate(mBlocks.back());
                mBlocks.pop_back();
            }
            delete mAllocator;
        }


    protected:
        ChunkAllocator* mAllocator;
        vector< void* > mBlocks;
};
}    // unnamed namespace


// --- ChunkAllocator tests -------------------------------------------------

/// @todo [C++11] Compare `ptr` with `nullptr`
TEST_P(ChunkAllocatorT,
       allocate_allocator_returnsNonNullptr)
{
    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        void* const ptr = mAllocator->allocate();
        mBlocks.push_back(ptr);

        EXPECT_TRUE(ptr != 0);
    }

    // Implicitly tests deallocation in LIFO order via fixture destructor
}


TEST_P(ChunkAllocatorT,
       allocate_allocator_returnsUniquePointers)
{
    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        void* const ptr = mAllocator->allocate();
        mBlocks.push_back(ptr);
    }

    // Create vector with sorted pointers & remove duplicates
    vector< void* > sorted(mBlocks);
    sort(sorted.begin(), sorted.end());
    sorted.erase(unique(sorted.begin(), sorted.end()), sorted.end());

    EXPECT_EQ(mBlocks.size(), sorted.size());

    // Implicitly tests deallocation in LIFO order via fixture destructor
}


TEST_P(ChunkAllocatorT,
       allocate_afterDeallocate_returnsSamePointer)
{
    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        void* const ptr = mAllocator->allocate();
        mBlocks.push_back(ptr);
    }

    // Deallocate each block in FIFO order and allocate a new one;
    // pointers should match => efficient replacement
    const size_t count = mBlocks.size();
    for (size_t i = 0; i < count; ++i)
    {
        void* const oldPtr = mBlocks[i];
        mAllocator->deallocate(oldPtr);
        void* const newPtr = mAllocator->allocate();

        EXPECT_EQ(oldPtr, newPtr);

        // Update entry in case we got a different pointer
        mBlocks[i] = newPtr;
    }
}


TEST_P(ChunkAllocatorT,
       deallocate_fifoOrder_works)
{
    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        void* const ptr = mAllocator->allocate();
        mBlocks.push_back(ptr);
    }

    // Tests deallocation in FIFO order
    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        mAllocator->deallocate(mBlocks[i]);
    }
    mBlocks.clear();
}


TEST_P(ChunkAllocatorT,
       deallocate_randomOrder_works)
{
    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        void* const ptr = mAllocator->allocate();
        mBlocks.push_back(ptr);
    }

    // Tests deallocation in (deterministic) random order
    srand(42);
    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        const size_t index = rand() % mBlocks.size();

        mAllocator->deallocate(mBlocks[index]);
        mBlocks.erase(mBlocks.begin() + index);
    }
}


// Instantiate tests with various block sizes, especially with the boundary
// values triggering different chunk sizes
INSTANTIATE_TEST_CASE_P(ChunkAllocator,
                        ChunkAllocatorT,
                        Values(1, 3, 4, 15, 16, 32));
