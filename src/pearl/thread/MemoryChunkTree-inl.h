/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_MEMORYCHUNKTREE_H
    #error "Do not include this file!  Use 'MemoryChunkTree.h' instead."
#endif


#include <UTILS_Error.h>

#include "MemoryChunk.h"


namespace pearl
{
namespace detail
{
/// @todo [C++11]
///     - Assertion: compare `chunk` with `nullptr`
///     - Special case: compare `root` with `nullptr` and assign `nullptr`
///       to child pointers of `chunk`
///     - Insertion: assign `nullptr` to child pointers of `root`
inline MemoryChunk*
insert(MemoryChunk*       root,
       MemoryChunk* const chunk,
       const size_t       chunkSize)
{
    UTILS_ASSERT(chunk != 0);

    // Special case: Empty tree => chunk is new root
    if (root == 0)
    {
        chunk->mLeft  = 0;
        chunk->mRight = 0;

        return chunk;
    }

    // Make closest leaf node the new (temporary) root
    root = splay(root, chunk, chunkSize);

    // Chunk is in left subtree
    //   => split on left link of root and join subtrees using chunk
    if (chunk->isLessThan(root, chunkSize))
    {
        chunk->mLeft  = root->mLeft;
        chunk->mRight = root;
        root->mLeft   = 0;
    }

    // Chunk is in right subtree
    //   => split on right link of root and join subtrees using chunk
    else
    {
        chunk->mLeft  = root;
        chunk->mRight = root->mRight;
        root->mRight  = 0;
    }

    // Chunk is new root
    return chunk;
}


/// @todo [C++11]
///     - Assertion: Compare `root` with `nullptr`
///     - Compare `mLeft` with `nullptr`
inline MemoryChunk*
remove(MemoryChunk* root,
       const size_t chunkSize)
{
    UTILS_ASSERT(root != 0);

    // No left subtree => root of right subtree is new root
    if (root->mLeft == 0)
    {
        root = root->mRight;
    }

    // Otherwise => join left and right subtrees
    else
    {
        MemoryChunk* const left  = root->mLeft;
        MemoryChunk* const right = root->mRight;

        // Make "largest" node in left subtree the new root (with no right
        // child) and add right subtree of previous root as its right subtree
        root         = splay(left, root, chunkSize);
        root->mRight = right;
    }

    return root;
}


/// @todo [C++11] Compare against `nullptr` (7x)
inline MemoryChunk*
splay(MemoryChunk* root,
      void* const  ptr,
      const size_t chunkSize)
{
    UTILS_ASSERT(root != 0);

    // ptr is in left subtree
    if (  root->isGreaterThan(ptr, chunkSize)
       && (root->mLeft != 0))
    {
        // Case 2: zig-zig (left/left)
        if (  root->mLeft->isGreaterThan(ptr, chunkSize)
           && (root->mLeft->mLeft != 0))
        {
            // Recursively make chunk containing ptr (or the closest leaf)
            // the root of left-left
            root->mLeft->mLeft = splay(root->mLeft->mLeft, ptr, chunkSize);

            // Two single rotations
            //   => first rotation brings left to root,
            //      second rotation (outside the `if`) left-left to root
            root = rotateRight(root);
        }

        // Case 3: zig-zag (left/right)
        else if (  root->mLeft->isLessThan(ptr, chunkSize)
                && (root->mLeft->mRight != 0))
        {
            // Recursively make chunk containing ptr (or the closest leaf)
            // the root of left-right
            root->mLeft->mRight = splay(root->mLeft->mRight, ptr, chunkSize);

            // Double rotation
            //   => first rotation brings left-right to left,
            //      second rotation (outside the `if`) left-right to root
            root->mLeft = rotateLeft(root->mLeft);
        }

        // Case 1: zig (left)
        // Terminating single rotation => brings left to root
        root = rotateRight(root);
    }

    // ptr is in right subtree
    else if (  root->isLessThan(ptr, chunkSize)
            && (root->mRight != 0))
    {
        // Case 2: zig-zig (right/right)
        if (  root->mRight->isLessThan(ptr, chunkSize)
           && (root->mRight->mRight != 0))
        {
            // Recursively make chunk containing ptr (or the closest leaf)
            // the root of right-right
            root->mRight->mRight = splay(root->mRight->mRight, ptr, chunkSize);

            // Two single rotations
            //   => first rotation brings right to root,
            //      second rotation (outside the `if`) right-right to root
            root = rotateLeft(root);
        }

        // Case 3: zig-zag (right/left)
        else if (  root->mRight->isGreaterThan(ptr, chunkSize)
                && (root->mRight->mLeft != 0))
        {
            // Recursively make chunk containing ptr (or the closest leaf)
            // the root of right-left
            root->mRight->mLeft = splay(root->mRight->mLeft, ptr, chunkSize);

            // Double rotation
            //   => first rotation brings right-left to right,
            //      second rotation (outside the `if`) right-left to root
            root->mRight = rotateRight(root->mRight);
        }

        // Case 1: zig (right)
        // Terminating single rotation => brings right to root
        root = rotateLeft(root);
    }

    // root now contains ptr or is closest leaf if ptr is not in tree
    return root;
}


/// @todo [C++11] Assertion: compare `root` and `root->mRight` with `nullptr`
inline MemoryChunk*
rotateLeft(MemoryChunk* const root)
{
    UTILS_ASSERT(root != 0);
    UTILS_ASSERT(root->mRight != 0);

    MemoryChunk* const temp = root->mRight;

    root->mRight = temp->mLeft;
    temp->mLeft  = root;

    return temp;
}


/// @todo [C++11] Assertion: compare `root` and `root->mLeft` with `nullptr`
inline MemoryChunk*
rotateRight(MemoryChunk* const root)
{
    UTILS_ASSERT(root != 0);
    UTILS_ASSERT(root->mLeft != 0);

    MemoryChunk* const temp = root->mLeft;

    root->mLeft  = temp->mRight;
    temp->mRight = root;

    return temp;
}
}    // namespace pearl::detail
}    // namespace pearl
