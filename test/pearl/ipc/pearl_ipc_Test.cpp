/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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


#include <config.h>

#include "pearl_ipc.h"

#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

using namespace std;
using namespace testing;
using namespace pearl::detail;


namespace
{
// --- Constants ------------------------------------------------------------

// Number of data elements per send/receive chunk
const size_t CHUNK_SIZE = 8;

// Number of "guard" elements to use in buffers before/after payload data
const size_t NUM_GUARDS = 4;

// Maximum value stored in any buffer (used as guard and modulus)
const size_t MAX_VALUE = 127;


// --- Datatype handling ----------------------------------------------------

// Overloaded helper functions returning the PEARL IPC datatype for a vector
// of values of a particular type.
#define GET_DATATYPE_FUNC(type, dtype) \
    inline IpcDatatype                 \
    getDatatype(const vector< type >&) \
    {                                  \
        return PEARL_ ## dtype;        \
    }

GET_DATATYPE_FUNC(int8_t, INT8_T)
GET_DATATYPE_FUNC(uint8_t, UINT8_T)
GET_DATATYPE_FUNC(int16_t, INT16_T)
GET_DATATYPE_FUNC(uint16_t, UINT16_T)
GET_DATATYPE_FUNC(int32_t, INT32_T)
GET_DATATYPE_FUNC(uint32_t, UINT32_T)
GET_DATATYPE_FUNC(int64_t, INT64_T)
GET_DATATYPE_FUNC(uint64_t, UINT64_T)
GET_DATATYPE_FUNC(float, FLOAT)
GET_DATATYPE_FUNC(double, DOUBLE)

#undef GET_DATATYPE_FUNC


// --- Send/receive buffer management ---------------------------------------

// Sets up a buffer with <numElements> entries of the guard value (plus the
// guards themselves). If <numElements> is zero, the buffer will be empty,
// i.e., also no guards will be generated.
template< typename T >
inline void
clearBuffer(vector< T >& array,
            const size_t numElements)
{
    array.clear();
    if (numElements > 0)
    {
        const size_t totalElements = (numElements + (2 * NUM_GUARDS));

        array.reserve(totalElements);
        for (size_t i = 0; i < totalElements; ++i)
        {
            array.push_back(MAX_VALUE);
        }
    }
}


// Sets up a buffer with <numElements> entries and appropriate guard entries.
// The value of each entry is (<startValue> + <index>) mod MAX_VALUE. If
// <numElements> is zero, the buffer will be empty, i.e., also no guards will
// be generated.
template< typename T >
inline void
fillBuffer(vector< T >& array,
           const size_t numElements,
           const size_t startValue)
{
    array.clear();
    if (numElements > 0)
    {
        const size_t totalElements = (numElements + (2 * NUM_GUARDS));

        array.reserve(totalElements);
        for (size_t i = 0; i < NUM_GUARDS; ++i)
        {
            array.push_back(MAX_VALUE);
        }
        for (size_t i = 0; i < numElements; ++i)
        {
            array.push_back((startValue + i) % MAX_VALUE);
        }
        for (size_t i = 0; i < NUM_GUARDS; ++i)
        {
            array.push_back(MAX_VALUE);
        }
    }
}


// Returns a pointer to the first payload entry of a buffer, or NULL if there
// are no payload entries. The number of guard entries can be specified using
// the <guards> parameter (default: NUM_GUARDS).
template< typename T >
inline T*
getDataPtr(vector< T >& array,
           const size_t guards = NUM_GUARDS)
{
    if (array.empty())
    {
        return 0;
    }

    return &array[guards];
}


// --- Test environment customization ---------------------------------------

// Helper class to set up/tear down IPC environment
class IpcEnvironment
    : public Environment
{
    public:
        virtual void
        SetUp()
        {
            ipcInit();
        }

        virtual void
        TearDown()
        {
            ipcFinalize();
        }
};


// Instantiate IPC environment
Environment* const environment = AddGlobalTestEnvironment(new IpcEnvironment);
}    // unnamed namespace


// --- IPC communication context tests --------------------------------------

// Communication context test:
// Instantiates three different communication contexts from the predefined
// "world" context: splitting into upper/lower half, splitting into odd/even
// ranks, and reversing the rank order.  Validation is based on rank numbers
// in the resulting communication context, which is freed afterwards.
TEST(IpcCommContext,
     testCommSplit)
{
    const uint32_t rank = ipcGetRank(ipcCommWorld);
    const uint32_t size = ipcGetSize(ipcCommWorld);

    // Upper/lower
    IpcComm* comm;
    ipcCommSplit(ipcCommWorld, rank < (size / 2), rank, &comm);
    EXPECT_EQ(rank < (size / 2) ? rank : rank - (size / 2), ipcGetRank(comm));
    ipcCommFree(&comm);
    EXPECT_TRUE(comm == 0);

    // Odd/even
    ipcCommSplit(ipcCommWorld, rank % 2, rank, &comm);
    EXPECT_EQ(rank / 2, ipcGetRank(comm));
    ipcCommFree(&comm);
    EXPECT_TRUE(comm == 0);

    // Reverse
    ipcCommSplit(ipcCommWorld, 42, size - 1 - rank, &comm);
    EXPECT_EQ(size - 1 - rank, ipcGetRank(comm));
    ipcCommFree(&comm);
    EXPECT_TRUE(comm == 0);
}


// --- IPC collective communication tests -----------------------------------

// Broadcast test:
// Sends a message with payload [0, CHUNK_SIZE-1] (mod MAX_VALUE) from the
// root rank to all other ranks. The test is carried out for each rank as
// root.
template< typename T >
void
testBroadcast(vector< T >&         sendBuf,
              vector< T >&         reference,
              const IpcComm* const comm)
{
    const uint32_t numRanks = ipcGetSize(comm);
    const uint32_t rank     = ipcGetRank(comm);

    // Setup reference
    fillBuffer(reference, CHUNK_SIZE, 0);

    // Repeat test for each rank as root
    for (uint32_t root = 0; root < numRanks; ++root)
    {
        // Setup sender payload
        if (rank == root)
        {
            fillBuffer(sendBuf, CHUNK_SIZE, 0);
        }
        else
        {
            clearBuffer(sendBuf, CHUNK_SIZE);
        }

        // Do the broadcast & verify result
        ipcBroadcast(getDataPtr(sendBuf), CHUNK_SIZE,
                     getDatatype(sendBuf), root, comm);
        EXPECT_EQ(reference, sendBuf);
    }
}


// Gather test:
// Sends a message of size CHUNK_SIZE from each rank to the root. The payload
// is constructed such that the resulting message contains consecutive values
// [0, (numRanks * CHUNK_SIZE) - 1] (mod MAX_VALUE). The test is carried out
// for each rank as root.
template< typename T >
void
testGather(vector< T >&         sendBuf,
           vector< T >&         recvBuf,
           vector< T >&         reference,
           const IpcComm* const comm)
{
    const uint32_t numRanks = ipcGetSize(comm);
    const uint32_t rank     = ipcGetRank(comm);

    // Setup sender payload
    fillBuffer(sendBuf, CHUNK_SIZE, rank * CHUNK_SIZE);

    // Repeat test for each rank as root
    for (uint32_t root = 0; root < numRanks; ++root)
    {
        // Setup receive buffer & reference
        if (rank == root)
        {
            clearBuffer(recvBuf, numRanks * CHUNK_SIZE);
            fillBuffer(reference, numRanks * CHUNK_SIZE, 0);
        }
        else
        {
            clearBuffer(recvBuf, 0);
            clearBuffer(reference, 0);
        }

        // Do the gather & verify result
        ipcGather(getDataPtr(sendBuf),
                  getDataPtr(recvBuf), CHUNK_SIZE,
                  getDatatype(sendBuf), root, comm);
        EXPECT_EQ(reference, recvBuf);
    }
}


// Gatherv test:
// Sends a message of size ((rank+1) * CHUNK_SIZE) from each rank to root. The
// individual payload is constructed such that the resulting message contains
// consecutive values [0, #total_elements_received] (mod MAX_VALUE). The test
// is carried out for each rank as root.
template< typename T >
void
testGatherv(vector< T >&         sendBuf,
            vector< T >&         recvBuf,
            vector< T >&         reference,
            const IpcComm* const comm)
{
    const uint32_t numRanks = ipcGetSize(comm);
    const uint32_t rank     = ipcGetRank(comm);
    const uint32_t sendSize = (rank + 1) * CHUNK_SIZE;

    // Setup sender payload
    size_t startValue = 0;
    for (uint32_t i = 0; i < rank; ++i)
    {
        startValue += (i + 1) * CHUNK_SIZE;
    }
    fillBuffer(sendBuf, sendSize, startValue);

    // Repeat test for each rank as root
    for (uint32_t root = 0; root < numRanks; ++root)
    {
        vector< uint32_t > recvCounts;
        if (rank == root)
        {
            size_t total = 0;
            for (uint32_t i = 0; i < numRanks; ++i)
            {
                recvCounts.push_back((i + 1) * CHUNK_SIZE);
                total += recvCounts.back();
            }
            clearBuffer(recvBuf, total);
            fillBuffer(reference, total, 0);
        }
        else
        {
            clearBuffer(recvBuf, 0);
            clearBuffer(reference, 0);
        }

        // Do the gatherv & verify result
        ipcGatherv(getDataPtr(sendBuf), sendSize,
                   getDataPtr(recvBuf), getDataPtr(recvCounts, 0),
                   getDatatype(sendBuf), root, comm);
        EXPECT_EQ(reference, recvBuf);
    }
}


// Scatter test:
// Sends a message from root to each rank. The payload consists of consecutive
// values [0, #total_elements_sent] (mod MAX_VALUE), which is split into equal
// segments of size CHUNK_SIZE. The test is carried out for each rank as root.
template< typename T >
void
testScatter(vector< T >&         sendBuf,
            vector< T >&         recvBuf,
            vector< T >&         reference,
            const IpcComm* const comm)
{
    const uint32_t numRanks = ipcGetSize(comm);
    const uint32_t rank     = ipcGetRank(comm);

    // Setup reference
    fillBuffer(reference, CHUNK_SIZE, rank * CHUNK_SIZE);

    // Repeat test for each rank as root
    for (uint32_t root = 0; root < numRanks; ++root)
    {
        // Setup send & receive buffer
        if (rank == root)
        {
            fillBuffer(sendBuf, numRanks * CHUNK_SIZE, 0);
        }
        else
        {
            clearBuffer(sendBuf, 0);
        }
        clearBuffer(recvBuf, CHUNK_SIZE);

        // Do the scatter & verify result
        ipcScatter(getDataPtr(sendBuf),
                   getDataPtr(recvBuf), CHUNK_SIZE,
                   getDatatype(sendBuf), root, comm);
        EXPECT_EQ(reference, recvBuf);
    }
}


// Scatterv test:
// Sends a message from root to each rank. The payload consists of consecutive
// values [0, #total_elements_sent] (mod MAX_VALUE), which is split into
// segments of size ((dest_rank+1) * CHUNKSIZE) (mod MAX_VALUE). The test is
// carried out for each rank as root.
template< typename T >
void
testScatterv(vector< T >&         sendBuf,
             vector< T >&         recvBuf,
             vector< T >&         reference,
             const IpcComm* const comm)
{
    const uint32_t numRanks = ipcGetSize(comm);
    const uint32_t rank     = ipcGetRank(comm);
    const uint32_t recvSize = (rank + 1) * CHUNK_SIZE;

    // Setup reference
    size_t startValue = 0;
    for (uint32_t i = 0; i < rank; ++i)
    {
        startValue += (i + 1) * CHUNK_SIZE;
    }
    fillBuffer(reference, recvSize, startValue);

    // Repeat test for each rank as root
    for (uint32_t root = 0; root < numRanks; ++root)
    {
        vector< uint32_t > sendCounts;
        if (rank == root)
        {
            size_t total = 0;
            for (uint32_t i = 0; i < numRanks; ++i)
            {
                sendCounts.push_back((i + 1) * CHUNK_SIZE);
                total += sendCounts.back();
            }
            fillBuffer(sendBuf, total, 0);
        }
        else
        {
            clearBuffer(sendBuf, 0);
        }
        clearBuffer(recvBuf, recvSize);

        // Do the scatterv & verify result
        ipcScatterv(getDataPtr(sendBuf), getDataPtr(sendCounts, 0),
                    getDataPtr(recvBuf), recvSize,
                    getDatatype(sendBuf), root, comm);
        EXPECT_EQ(reference, recvBuf);
    }
}


// Test fixture template used for collective tests
template< typename T >
class IpcCollectiveFixture
    : public Test
{
    public:
        virtual void
        SetUp()
        {
            const uint32_t rank = ipcGetRank(ipcCommWorld);
            const uint32_t size = ipcGetSize(ipcCommWorld);

            ipcCommSplit(ipcCommWorld, rank < (size / 2), rank, &mUpLowComm);
            ipcCommSplit(ipcCommWorld, rank % 2, rank, &mOddEvenComm);
            ipcCommSplit(ipcCommWorld, 42, size - 1 - rank, &mReverseComm);
        }

        virtual void
        TearDown()
        {
            ipcCommFree(&mReverseComm);
            ipcCommFree(&mOddEvenComm);
            ipcCommFree(&mUpLowComm);
        }


    protected:
        vector< T > mSendBuf;
        vector< T > mRecvBuf;
        vector< T > mReference;
        IpcComm*    mUpLowComm;
        IpcComm*    mOddEvenComm;
        IpcComm*    mReverseComm;
};


#define COLLECTIVE_TESTS(type)                                         \
    typedef IpcCollectiveFixture< type > IpcCollective_ ## type;       \
                                                                       \
                                                                       \
    TEST_F(IpcCollective_ ## type,                                     \
           testBroadcast)                                              \
    {                                                                  \
        testBroadcast(this->mSendBuf, this->mReference,                \
                      ipcCommWorld);                                   \
        testBroadcast(this->mSendBuf, this->mReference,                \
                      mUpLowComm);                                     \
        testBroadcast(this->mSendBuf, this->mReference,                \
                      mOddEvenComm);                                   \
        testBroadcast(this->mSendBuf, this->mReference,                \
                      mReverseComm);                                   \
    }                                                                  \
                                                                       \
                                                                       \
    TEST_F(IpcCollective_ ## type,                                     \
           testGather)                                                 \
    {                                                                  \
        testGather(this->mSendBuf, this->mRecvBuf, this->mReference,   \
                   ipcCommWorld);                                      \
        testGather(this->mSendBuf, this->mRecvBuf, this->mReference,   \
                   mUpLowComm);                                        \
        testGather(this->mSendBuf, this->mRecvBuf, this->mReference,   \
                   mOddEvenComm);                                      \
        testGather(this->mSendBuf, this->mRecvBuf, this->mReference,   \
                   mReverseComm);                                      \
    }                                                                  \
                                                                       \
                                                                       \
    TEST_F(IpcCollective_ ## type,                                     \
           testGatherv)                                                \
    {                                                                  \
        testGatherv(this->mSendBuf, this->mRecvBuf, this->mReference,  \
                    ipcCommWorld);                                     \
        testGatherv(this->mSendBuf, this->mRecvBuf, this->mReference,  \
                    mUpLowComm);                                       \
        testGatherv(this->mSendBuf, this->mRecvBuf, this->mReference,  \
                    mOddEvenComm);                                     \
        testGatherv(this->mSendBuf, this->mRecvBuf, this->mReference,  \
                    mReverseComm);                                     \
    }                                                                  \
                                                                       \
                                                                       \
    TEST_F(IpcCollective_ ## type,                                     \
           testScatter)                                                \
    {                                                                  \
        testScatter(this->mSendBuf, this->mRecvBuf, this->mReference,  \
                    ipcCommWorld);                                     \
        testScatter(this->mSendBuf, this->mRecvBuf, this->mReference,  \
                    mUpLowComm);                                       \
        testScatter(this->mSendBuf, this->mRecvBuf, this->mReference,  \
                    mOddEvenComm);                                     \
        testScatter(this->mSendBuf, this->mRecvBuf, this->mReference,  \
                    mReverseComm);                                     \
    }                                                                  \
                                                                       \
                                                                       \
    TEST_F(IpcCollective_ ## type,                                     \
           testScatterv)                                               \
    {                                                                  \
        testScatterv(this->mSendBuf, this->mRecvBuf, this->mReference, \
                     ipcCommWorld);                                    \
        testScatterv(this->mSendBuf, this->mRecvBuf, this->mReference, \
                     mUpLowComm);                                      \
        testScatterv(this->mSendBuf, this->mRecvBuf, this->mReference, \
                     mOddEvenComm);                                    \
        testScatterv(this->mSendBuf, this->mRecvBuf, this->mReference, \
                     mReverseComm);                                    \
    }


// Instantiate collective tests for all supported datatypes
COLLECTIVE_TESTS(int8_t)
COLLECTIVE_TESTS(int16_t)
COLLECTIVE_TESTS(int32_t)
COLLECTIVE_TESTS(int64_t)
COLLECTIVE_TESTS(uint8_t)
COLLECTIVE_TESTS(uint16_t)
COLLECTIVE_TESTS(uint32_t)
COLLECTIVE_TESTS(uint64_t)
COLLECTIVE_TESTS(float)
COLLECTIVE_TESTS(double)


#undef COLLECTIVE_TESTS
