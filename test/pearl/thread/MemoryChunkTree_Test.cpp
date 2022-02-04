/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2018                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "MemoryChunkTree.h"

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

#include "MemoryChunk.h"

using namespace std;
using namespace testing;
using namespace pearl::detail;


namespace
{
// --- Constants ------------------------------------------------------------

// Memory chunk size used tests
static const size_t CHUNKSIZE = 4096;

// Memory block size used for tests
static const size_t BLOCKSIZE = 16;


// --- Helpers --------------------------------------------------------------

// Test fixture for memory chunk tree tests
class MemoryChunkTreeT
    : public Test
{
    public:
        /// @todo [C++11] Initialize `mLeft` and `mRight` with `nullptr`
        MemoryChunkTreeT()
        {
            // Create a bunch of memory chunks with initialized child pointers
            for (int i = 0; i < 11; ++i)
            {
                MemoryChunk* chunk = MemoryChunk::create(CHUNKSIZE, BLOCKSIZE);

                chunk->mLeft  = 0;
                chunk->mRight = 0;

                mChunks.push_back(chunk);
            }

            // Sort chunks by start address
            sort(mChunks.begin(), mChunks.end());
        }

        ~MemoryChunkTreeT()
        {
            // Deallocate chunks
            while (!mChunks.empty())
            {
                MemoryChunk::destroy(mChunks.back());
                mChunks.pop_back();
            }
        }


    protected:
        vector< MemoryChunk* > mChunks;
};
}    // unnamed namespace


// --- MemoryChunkTree tests ------------------------------------------------

// Inserts a series of chunks.  The resulting trees have been verified with
// Sleator's top-down splay tree implementation from
// http://www.link.cs.cmu.edu/link/ftp-site/splaying/top-down-splay.c
//
/// @todo [C++11] Initialize `root` with `nullptr`
TEST_F(MemoryChunkTreeT,
       insert_chunks_buildsExpectedSplayTree)
{
    MemoryChunk* root = 0;

    //  nil
    root = insert(root, mChunks[7], CHUNKSIZE);

    //  7
    ASSERT_EQ(mChunks[7], root);

    root = insert(root, mChunks[5], CHUNKSIZE);

    //  5
    //   \
    //    7
    ASSERT_EQ(mChunks[5], root);
    ASSERT_EQ(mChunks[7], root->mRight);

    root = insert(root, mChunks[2], CHUNKSIZE);

    //  2
    //   \
    //    5
    //     \
    //      7
    ASSERT_EQ(mChunks[2], root);
    ASSERT_EQ(mChunks[5], root->mRight);
    ASSERT_EQ(mChunks[7], root->mRight->mRight);

    root = insert(root, mChunks[9], CHUNKSIZE);

    //        9
    //       /
    //      7
    //     /
    //    5
    //   /
    //  2
    ASSERT_EQ(mChunks[9], root);
    ASSERT_EQ(mChunks[7], root->mLeft);
    ASSERT_EQ(mChunks[5], root->mLeft->mLeft);
    ASSERT_EQ(mChunks[2], root->mLeft->mLeft->mLeft);

    root = insert(root, mChunks[1], CHUNKSIZE);

    //  1
    //   \
    //    2
    //     \
    //      7
    //     / \
    //    5   9
    ASSERT_EQ(mChunks[1], root);
    ASSERT_EQ(mChunks[2], root->mRight);
    ASSERT_EQ(mChunks[7], root->mRight->mRight);
    ASSERT_EQ(mChunks[5], root->mRight->mRight->mLeft);
    ASSERT_EQ(mChunks[9], root->mRight->mRight->mRight);

    root = insert(root, mChunks[8], CHUNKSIZE);

    //      8
    //     / \
    //    2   9
    //   / \
    //  1  7
    //    /
    //   5
    ASSERT_EQ(mChunks[8], root);
    ASSERT_EQ(mChunks[2], root->mLeft);
    ASSERT_EQ(mChunks[1], root->mLeft->mLeft);
    ASSERT_EQ(mChunks[7], root->mLeft->mRight);
    ASSERT_EQ(mChunks[5], root->mLeft->mRight->mLeft);
    ASSERT_EQ(mChunks[9], root->mRight);

    root = insert(root, mChunks[4], CHUNKSIZE);

    //      4
    //     / \
    //    2   5
    //   /     \
    //  1       8
    //         / \
    //        7   9
    ASSERT_EQ(mChunks[4], root);
    ASSERT_EQ(mChunks[2], root->mLeft);
    ASSERT_EQ(mChunks[1], root->mLeft->mLeft);
    ASSERT_EQ(mChunks[5], root->mRight);
    ASSERT_EQ(mChunks[8], root->mRight->mRight);
    ASSERT_EQ(mChunks[7], root->mRight->mRight->mLeft);
    ASSERT_EQ(mChunks[9], root->mRight->mRight->mRight);
}


// Repeatedly removes the root node of the tree.  The resulting trees have
// been verified with Sleator's top-down splay tree implementation from
// http://www.link.cs.cmu.edu/link/ftp-site/splaying/top-down-splay.c
//
/// @todo [C++11]
///   - Initialize `root` with `nullptr`
///   - Last assert: compare `root` with `nullptr`
TEST_F(MemoryChunkTreeT,
       remove_rootNodes_buildsExpectedTree)
{
    MemoryChunk* root = 0;

    //  nil
    root = insert(root, mChunks[7], CHUNKSIZE);
    root = insert(root, mChunks[5], CHUNKSIZE);
    root = insert(root, mChunks[2], CHUNKSIZE);
    root = insert(root, mChunks[9], CHUNKSIZE);
    root = insert(root, mChunks[1], CHUNKSIZE);
    root = insert(root, mChunks[8], CHUNKSIZE);
    root = insert(root, mChunks[4], CHUNKSIZE);

    //      4
    //     / \
    //    2   5
    //   /     \
    //  1       8
    //         / \
    //        7   9
    root = remove(root, CHUNKSIZE);

    //    2
    //   / \
    //  1   5
    //       \
    //        8
    //       / \
    //      7   9
    ASSERT_EQ(mChunks[2], root);
    ASSERT_EQ(mChunks[1], root->mLeft);
    ASSERT_EQ(mChunks[5], root->mRight);
    ASSERT_EQ(mChunks[8], root->mRight->mRight);
    ASSERT_EQ(mChunks[7], root->mRight->mRight->mLeft);
    ASSERT_EQ(mChunks[9], root->mRight->mRight->mRight);

    root = remove(root, CHUNKSIZE);

    //  1
    //   \
    //    5
    //     \
    //      8
    //     / \
    //    7   9
    ASSERT_EQ(mChunks[1], root);
    ASSERT_EQ(mChunks[5], root->mRight);
    ASSERT_EQ(mChunks[8], root->mRight->mRight);
    ASSERT_EQ(mChunks[7], root->mRight->mRight->mLeft);
    ASSERT_EQ(mChunks[9], root->mRight->mRight->mRight);

    root = remove(root, CHUNKSIZE);

    //  5
    //   \
    //    8
    //   / \
    //  7   9
    ASSERT_EQ(mChunks[5], root);
    ASSERT_EQ(mChunks[8], root->mRight);
    ASSERT_EQ(mChunks[7], root->mRight->mLeft);
    ASSERT_EQ(mChunks[9], root->mRight->mRight);

    root = remove(root, CHUNKSIZE);

    //    8
    //   / \
    //  7   9
    ASSERT_EQ(mChunks[8], root);
    ASSERT_EQ(mChunks[7], root->mLeft);
    ASSERT_EQ(mChunks[9], root->mRight);

    root = remove(root, CHUNKSIZE);

    //  7
    //   \
    //    9
    ASSERT_EQ(mChunks[7], root);
    ASSERT_EQ(mChunks[9], root->mRight);

    root = remove(root, CHUNKSIZE);

    //  9
    ASSERT_EQ(mChunks[9], root);

    root = remove(root, CHUNKSIZE);

    //  nil
    ASSERT_TRUE(root == 0);
}


// See SleatorTarjan:1985, p. 656, Figure 3(a)
//
//      y                 x
//     / \               / \
//    x   C     -->     A   y
//   / \                   / \
//  A   B                 B   C
//
TEST_F(MemoryChunkTreeT,
       splay_zigCaseLeft_performsSingleRightRotation)
{
    MemoryChunk* A = mChunks[0];
    MemoryChunk* x = mChunks[1];
    MemoryChunk* B = mChunks[2];
    MemoryChunk* y = mChunks[3];
    MemoryChunk* C = mChunks[4];

    y->mLeft  = x;
    y->mRight = C;
    x->mLeft  = A;
    x->mRight = B;

    MemoryChunk* root = splay(y, x, CHUNKSIZE);

    // Assert on inner nodes as follow-up checks may fail if `nullptr`
    ASSERT_EQ(x, root);
    EXPECT_EQ(A, root->mLeft);
    ASSERT_EQ(y, root->mRight);
    EXPECT_EQ(B, root->mRight->mLeft);
    EXPECT_EQ(C, root->mRight->mRight);
}


// See SleatorTarjan:1985, p. 656, Figure 3(a) mirrored
//
//    y                     x
//   / \                   / \
//  C   x       -->       y   A
//     / \               / \
//    B   A             C   B
//
TEST_F(MemoryChunkTreeT,
       splay_zigCaseRight_performsSingleLeftRotation)
{
    MemoryChunk* C = mChunks[0];
    MemoryChunk* y = mChunks[1];
    MemoryChunk* B = mChunks[2];
    MemoryChunk* x = mChunks[3];
    MemoryChunk* A = mChunks[4];

    y->mLeft  = C;
    y->mRight = x;
    x->mLeft  = B;
    x->mRight = A;

    MemoryChunk* root = splay(y, x, CHUNKSIZE);

    // Assert on inner nodes as follow-up checks may fail if `nullptr`
    ASSERT_EQ(x, root);
    ASSERT_EQ(y, root->mLeft);
    EXPECT_EQ(C, root->mLeft->mLeft);
    EXPECT_EQ(B, root->mLeft->mRight);
    EXPECT_EQ(A, root->mRight);
}


// See SleatorTarjan:1985, p. 656, Figure 3(b)
//
//        z                 x
//       / \               / \
//      y   D             A   y
//     / \        -->        / \
//    x   C                 B   z
//   / \                       / \
//  A   B                     C   D
//
TEST_F(MemoryChunkTreeT,
       splay_zigZigCaseLeft_performsTwoSingleRightRotations)
{
    MemoryChunk* A = mChunks[0];
    MemoryChunk* x = mChunks[1];
    MemoryChunk* B = mChunks[2];
    MemoryChunk* y = mChunks[3];
    MemoryChunk* C = mChunks[4];
    MemoryChunk* z = mChunks[5];
    MemoryChunk* D = mChunks[6];

    z->mLeft  = y;
    z->mRight = D;
    y->mLeft  = x;
    y->mRight = C;
    x->mLeft  = A;
    x->mRight = B;

    MemoryChunk* root = splay(z, x, CHUNKSIZE);

    // Assert on inner nodes as follow-up checks may fail if `nullptr`
    ASSERT_EQ(x, root);
    EXPECT_EQ(A, root->mLeft);
    ASSERT_EQ(y, root->mRight);
    EXPECT_EQ(B, root->mRight->mLeft);
    ASSERT_EQ(z, root->mRight->mRight);
    EXPECT_EQ(C, root->mRight->mRight->mLeft);
    EXPECT_EQ(D, root->mRight->mRight->mRight);
}


// See SleatorTarjan:1985, p. 656, Figure 3(b) mirrored
//
//    z                         x
//   / \                       / \
//  D   y                     y   A
//     / \        -->        / \
//    C   x                 z   B
//       / \               / \
//      B   A             D   C
//
TEST_F(MemoryChunkTreeT,
       splay_zigZigCaseRight_performsTwoSingleLeftRotations)
{
    MemoryChunk* D = mChunks[0];
    MemoryChunk* z = mChunks[1];
    MemoryChunk* C = mChunks[2];
    MemoryChunk* y = mChunks[3];
    MemoryChunk* B = mChunks[4];
    MemoryChunk* x = mChunks[5];
    MemoryChunk* A = mChunks[6];

    z->mLeft  = D;
    z->mRight = y;
    y->mLeft  = C;
    y->mRight = x;
    x->mLeft  = B;
    x->mRight = A;

    MemoryChunk* root = splay(z, x, CHUNKSIZE);

    // Assert on inner nodes as follow-up checks may fail if `nullptr`
    ASSERT_EQ(x, root);
    ASSERT_EQ(y, root->mLeft);
    ASSERT_EQ(z, root->mLeft->mLeft);
    EXPECT_EQ(D, root->mLeft->mLeft->mLeft);
    EXPECT_EQ(C, root->mLeft->mLeft->mRight);
    EXPECT_EQ(B, root->mLeft->mRight);
    EXPECT_EQ(A, root->mRight);
}


// See SleatorTarjan:1985, p. 656, Figure 3(c)
//
//      z                    x
//     / \                  / \
//    y   D                /   \
//   / \        -->       y     z
//  A   x                / \   / \
//     / \              A   B C   D
//    B   C
//
TEST_F(MemoryChunkTreeT,
       splay_zigZagCaseLeft_performsLeftRightRotation)
{
    MemoryChunk* A = mChunks[0];
    MemoryChunk* y = mChunks[1];
    MemoryChunk* B = mChunks[2];
    MemoryChunk* x = mChunks[3];
    MemoryChunk* C = mChunks[4];
    MemoryChunk* z = mChunks[5];
    MemoryChunk* D = mChunks[6];

    z->mLeft  = y;
    z->mRight = D;
    y->mLeft  = A;
    y->mRight = x;
    x->mLeft  = B;
    x->mRight = C;

    MemoryChunk* root = splay(z, x, CHUNKSIZE);

    // Assert on inner nodes as follow-up checks may fail if `nullptr`
    ASSERT_EQ(x, root);
    ASSERT_EQ(y, root->mLeft);
    EXPECT_EQ(A, root->mLeft->mLeft);
    EXPECT_EQ(B, root->mLeft->mRight);
    ASSERT_EQ(z, root->mRight);
    EXPECT_EQ(C, root->mRight->mLeft);
    EXPECT_EQ(D, root->mRight->mRight);
}


// See SleatorTarjan:1985, p. 656, Figure 3(c) mirrored
//
//    z                      x
//   / \                    / \
//  D   y                  /   \
//     / \      -->       z     y
//    x   A              / \   / \
//   / \                D   C B   A
//  C   B
//
TEST_F(MemoryChunkTreeT,
       splay_zigZagCaseRight_performsRightLeftRotation)
{
    MemoryChunk* D = mChunks[0];
    MemoryChunk* z = mChunks[1];
    MemoryChunk* C = mChunks[2];
    MemoryChunk* x = mChunks[3];
    MemoryChunk* B = mChunks[4];
    MemoryChunk* y = mChunks[5];
    MemoryChunk* A = mChunks[6];

    z->mLeft  = D;
    z->mRight = y;
    y->mLeft  = x;
    y->mRight = A;
    x->mLeft  = C;
    x->mRight = B;

    MemoryChunk* root = splay(z, x, CHUNKSIZE);

    // Assert on inner nodes as follow-up checks may fail if `nullptr`
    ASSERT_EQ(x, root);
    ASSERT_EQ(z, root->mLeft);
    EXPECT_EQ(D, root->mLeft->mLeft);
    EXPECT_EQ(C, root->mLeft->mRight);
    ASSERT_EQ(y, root->mRight);
    EXPECT_EQ(B, root->mRight->mLeft);
    EXPECT_EQ(A, root->mRight->mRight);
}


// Example from SleatorTarjan:1985, p. 661, Figure 6, though top-down splaying
// gives a slightly different result (verified with Sleator's implementation
// from http://www.link.cs.cmu.edu/link/ftp-site/splaying/top-down-splay.c).
//
//       f                            i
//      / \                          / \
//     /   \                        g   h
//    d     g                      /     \
//   / \     \                    f       j
//  c   e     h       -->        /
//   \       / \                d
//    b     i   j              / \
//   /       \                c   e
//  a        (n)               \
//                              b
//                             /
//                            a
//
TEST_F(MemoryChunkTreeT,
       splay_ptrNotInTree_movesNearestNodeToRoot)
{
    MemoryChunk* c = mChunks[0];
    MemoryChunk* a = mChunks[1];
    MemoryChunk* b = mChunks[2];
    MemoryChunk* d = mChunks[3];
    MemoryChunk* e = mChunks[4];
    MemoryChunk* f = mChunks[5];
    MemoryChunk* g = mChunks[6];
    MemoryChunk* i = mChunks[7];
    MemoryChunk* n = mChunks[8];
    MemoryChunk* h = mChunks[9];
    MemoryChunk* j = mChunks[10];

    f->mLeft  = d;
    f->mRight = g;
    d->mLeft  = c;
    d->mRight = e;
    c->mRight = b;
    b->mLeft  = a;
    g->mRight = h;
    h->mLeft  = i;
    h->mRight = j;

    MemoryChunk* root = splay(f, n, CHUNKSIZE);

    // Assert on inner nodes as follow-up checks may fail if `nullptr`
    ASSERT_EQ(i, root);
    ASSERT_EQ(g, root->mLeft);
    ASSERT_EQ(f, root->mLeft->mLeft);
    ASSERT_EQ(d, root->mLeft->mLeft->mLeft);
    ASSERT_EQ(c, root->mLeft->mLeft->mLeft->mLeft);
    ASSERT_EQ(b, root->mLeft->mLeft->mLeft->mLeft->mRight);
    EXPECT_EQ(a, root->mLeft->mLeft->mLeft->mLeft->mRight->mLeft);
    EXPECT_EQ(e, root->mLeft->mLeft->mLeft->mRight);
    ASSERT_EQ(h, root->mRight);
    EXPECT_EQ(j, root->mRight->mRight);
}


// See SleatorTarjan:1985, p. 655, Figure 1
//
//      y                 x
//     / \               / \
//    x   C     <--     A   y
//   / \                   / \
//  A   B                 B   C
//
TEST_F(MemoryChunkTreeT,
       rotateLeft_tree_performsSingleLeftRotation)
{
    MemoryChunk* A = mChunks[0];
    MemoryChunk* x = mChunks[1];
    MemoryChunk* B = mChunks[2];
    MemoryChunk* y = mChunks[3];
    MemoryChunk* C = mChunks[4];

    x->mLeft  = A;
    x->mRight = y;
    y->mLeft  = B;
    y->mRight = C;

    MemoryChunk* root = rotateLeft(x);

    // Assert on inner nodes as follow-up checks may fail if `nullptr`
    ASSERT_EQ(y, root);
    ASSERT_EQ(x, root->mLeft);
    EXPECT_EQ(A, root->mLeft->mLeft);
    EXPECT_EQ(B, root->mLeft->mRight);
    EXPECT_EQ(C, root->mRight);
}


// See SleatorTarjan:1985, p. 655, Figure 1
//
//      y                 x
//     / \               / \
//    x   C     -->     A   y
//   / \                   / \
//  A   B                 B   C
//
TEST_F(MemoryChunkTreeT,
       rotateRight_tree_performsSingleRightRotation)
{
    MemoryChunk* A = mChunks[0];
    MemoryChunk* x = mChunks[1];
    MemoryChunk* B = mChunks[2];
    MemoryChunk* y = mChunks[3];
    MemoryChunk* C = mChunks[4];

    y->mLeft  = x;
    y->mRight = C;
    x->mLeft  = A;
    x->mRight = B;

    MemoryChunk* root = rotateRight(y);

    // Assert on inner nodes as follow-up checks may fail if `nullptr`
    ASSERT_EQ(x, root);
    EXPECT_EQ(A, root->mLeft);
    ASSERT_EQ(y, root->mRight);
    EXPECT_EQ(B, root->mRight->mLeft);
    EXPECT_EQ(C, root->mRight->mRight);
}
