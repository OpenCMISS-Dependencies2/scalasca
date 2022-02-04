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


#include <config.h>

#include "MemoryChunk.h"

#include <stdint.h>

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <vector>

#include <gtest/gtest.h>

using namespace std;
using namespace testing;
using namespace pearl::detail;


namespace
{
// --- Helpers --------------------------------------------------------------

// Helper class storing test parameters
struct TestConfig
{
    TestConfig(const size_t chunkSize,
               const size_t blockSize)
        : mChunkSize(chunkSize),
          mBlockSize(blockSize),
          mCapacity(MemoryChunk::capacity(chunkSize, blockSize))
    {
    }

    const size_t mChunkSize;
    const size_t mBlockSize;
    const size_t mCapacity;
};


ostream&
operator<<(ostream&          stream,
           const TestConfig& cfg)
{
    return stream << "{chunkSize=" << cfg.mChunkSize
                  << ",blockSize=" << cfg.mBlockSize
                  << "}";
}


// Test fixture used for parameterized memory chunk tests
class MemoryChunkT
    : public TestWithParam< TestConfig >
{
    public:
        MemoryChunkT()
        {
            const TestConfig& cfg = GetParam();

            mChunk = MemoryChunk::create(cfg.mChunkSize,
                                         cfg.mBlockSize);
            mBlocks.reserve(cfg.mCapacity);
        }

        ~MemoryChunkT()
        {
            const TestConfig& cfg = GetParam();

            while (!mBlocks.empty())
            {
                mChunk->deallocate(mBlocks.back(), cfg.mBlockSize);
                mBlocks.pop_back();
            }
            MemoryChunk::destroy(mChunk);
        }


    protected:
        MemoryChunk*    mChunk;
        vector< void* > mBlocks;
};
}    // unnamed namespace


// --- MemoryChunk tests ----------------------------------------------------

TEST_P(MemoryChunkT,
       capacity_testConfig_returnsCapacity)
{
    const TestConfig& cfg = GetParam();

    // Calculate size of header with alignment on 8-byte boundary
    const size_t alignment  = 8;
    size_t       headerSize = sizeof(MemoryChunk);
    if ((headerSize % alignment) != 0)
    {
        headerSize += alignment - (headerSize % alignment);
    }

    // Calculate capacity
    const size_t expected = (cfg.mChunkSize - headerSize) / cfg.mBlockSize;

    // All following tests rely on a correct capacity,
    // thus abort immediately if it is bogus
    ASSERT_EQ(expected, MemoryChunk::capacity(cfg.mChunkSize, cfg.mBlockSize));
}


TEST_P(MemoryChunkT,
       availableBlocks_emptyChunk_returnsCapacity)
{
    const TestConfig& cfg = GetParam();

    EXPECT_EQ(cfg.mCapacity, mChunk->availableBlocks());
}


TEST_P(MemoryChunkT,
       allocate_chunk_decrementsAvailableBlocks)
{
    const TestConfig& cfg = GetParam();

    for (size_t i = 0; i < cfg.mCapacity; ++i)
    {
        void* const ptr = mChunk->allocate(cfg.mBlockSize);
        mBlocks.push_back(ptr);

        EXPECT_EQ(cfg.mCapacity - i - 1, mChunk->availableBlocks());
    }

    // Implicitly tests deallocation in LIFO order
}


TEST_P(MemoryChunkT,
       allocate_chunk_returnsPointerWithinChunk)
{
    const TestConfig& cfg = GetParam();

    for (size_t i = 0; i < cfg.mCapacity; ++i)
    {
        void* const ptr = mChunk->allocate(cfg.mBlockSize);
        mBlocks.push_back(ptr);

        void* const    chunkPtr   = static_cast< void* >(mChunk);
        uint8_t* const chunkBegin = static_cast< uint8_t* >(chunkPtr);
        uint8_t* const chunkEnd   = chunkBegin + cfg.mChunkSize;

        EXPECT_TRUE(std::greater_equal< void* >()(ptr, chunkBegin));
        EXPECT_TRUE(std::less< void* >()(ptr, chunkEnd));
    }

    // Implicitly tests deallocation in LIFO order
}


TEST_P(MemoryChunkT,
       allocate_newChunk_returnsConsecutiveBlocks)
{
    const TestConfig& cfg = GetParam();

    for (size_t i = 0; i < cfg.mCapacity; ++i)
    {
        void* const ptr = mChunk->allocate(cfg.mBlockSize);

        if (!mBlocks.empty())
        {
            uint8_t* const block = static_cast< uint8_t* >(ptr);
            uint8_t* const last  = static_cast< uint8_t* >(mBlocks.back());

            const ptrdiff_t offset = block - last;

            EXPECT_EQ(cfg.mBlockSize, offset);
        }

        mBlocks.push_back(ptr);
    }

    // Implicitly tests deallocation in LIFO order
}


TEST_P(MemoryChunkT,
       allocate_usedChunk_returnsBlocksInReverseDeallocOrder)
{
    const TestConfig& cfg = GetParam();

    for (size_t i = 0; i < cfg.mCapacity; ++i)
    {
        void* const ptr = mChunk->allocate(cfg.mBlockSize);
        mBlocks.push_back(ptr);
    }

    // Tests deallocation in (deterministic) random order
    vector< void* > dealloc;
    dealloc.reserve(cfg.mCapacity);
    srand(42);
    for (size_t i = 0; i < cfg.mCapacity; ++i)
    {
        const size_t index = rand() % mBlocks.size();
        void* const  ptr   = mBlocks[index];

        mChunk->deallocate(ptr, cfg.mBlockSize);
        dealloc.push_back(ptr);
        mBlocks.erase(mBlocks.begin() + index);
    }

    for (size_t i = 0; i < cfg.mCapacity; ++i)
    {
        void* const ptr = mChunk->allocate(cfg.mBlockSize);
        mBlocks.push_back(ptr);

        EXPECT_EQ(dealloc.back(), ptr);

        dealloc.pop_back();
    }
}


TEST_P(MemoryChunkT,
       deallocate_chunk_incrementsAvailableBlocks)
{
    const TestConfig& cfg = GetParam();

    for (size_t i = 0; i < cfg.mCapacity; ++i)
    {
        void* const ptr = mChunk->allocate(cfg.mBlockSize);
        mBlocks.push_back(ptr);
    }

    // Tests deallocation in FIFO order
    for (size_t i = 0; i < cfg.mCapacity; ++i)
    {
        mChunk->deallocate(mBlocks[i], cfg.mBlockSize);

        EXPECT_EQ(i + 1, mChunk->availableBlocks());
    }
    mBlocks.clear();
}


TEST_P(MemoryChunkT,
       isLessThan_chunkAndLowerAddress_returnsFalse)
{
    const TestConfig& cfg = GetParam();

    void* const chunkPtr = static_cast< void* >(mChunk);
    uint8_t*    ptr      = static_cast< uint8_t* >(chunkPtr)
                           + cfg.mChunkSize - 1;

    // All addresses lower or equal than the last byte of the chunk should
    // not compare "less than"
    for (size_t i = 0; i < (2 * cfg.mChunkSize); ++i)
    {
        EXPECT_FALSE(mChunk->isLessThan(ptr, cfg.mChunkSize));
        --ptr;
    }
}


TEST_P(MemoryChunkT,
       isLessThan_chunkAndHigherAddress_returnsTrue)
{
    const TestConfig& cfg = GetParam();

    void* const chunkPtr = static_cast< void* >(mChunk);
    uint8_t*    ptr      = static_cast< uint8_t* >(chunkPtr)
                           + cfg.mChunkSize;

    // All addresses greater than the last byte of the chunk should
    // compare "less than"
    for (size_t i = 0; i < cfg.mChunkSize; ++i)
    {
        EXPECT_TRUE(mChunk->isLessThan(ptr, cfg.mChunkSize));
        ++ptr;
    }
}


TEST_P(MemoryChunkT,
       isGreaterThan_chunkAndLowerAddress_returnsTrue)
{
    const TestConfig& cfg = GetParam();

    void* const chunkPtr = static_cast< void* >(mChunk);
    uint8_t*    ptr      = static_cast< uint8_t* >(chunkPtr) - 1;

    // All addresses lower than the first byte of the chunk should
    // compare "greater than"
    for (size_t i = 0; i < cfg.mChunkSize; ++i)
    {
        EXPECT_TRUE(mChunk->isGreaterThan(ptr, cfg.mChunkSize));
        --ptr;
    }
}


TEST_P(MemoryChunkT,
       isGreaterThan_chunkAndHigherAddress_returnsFalse)
{
    const TestConfig& cfg = GetParam();

    void* const chunkPtr = static_cast< void* >(mChunk);
    uint8_t*    ptr      = static_cast< uint8_t* >(chunkPtr);

    // All addresses greater or equal than the first byte of the chunk should
    // not compare "greater than"
    for (size_t i = 0; i < (2 * cfg.mChunkSize); ++i)
    {
        EXPECT_FALSE(mChunk->isGreaterThan(ptr, cfg.mChunkSize));
        ++ptr;
    }
}


// Instantiate tests with various configurations
INSTANTIATE_TEST_CASE_P(MemoryChunk,
                        MemoryChunkT,
                        Values(

                            // Minimal block size
                            TestConfig(256, 1),

                            // Small blocks
                            TestConfig(1024, 4),

                            // Larger blocks
                            TestConfig(4096, 16),

                            // Weird chunk/block sizes
                            TestConfig(4023, 17),

                            // Only a single chunk per block
                            TestConfig(1024, 900)
                        ));
