/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017-2018                                                **
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
 *  @brief   Declaration of helper functions to manage a search tree of
 *           pearl::detail::MemoryChunk instances.
 *
 *  This header file provides the declaration of internal helper functions
 *  used to manage an efficient search tree (top-down splay tree) for
 *  instances of class pearl::detail::MemoryChunk.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_MEMORYCHUNKTREE_H
#define PEARL_MEMORYCHUNKTREE_H


#include <cstddef>


namespace pearl
{
namespace detail
{
// --- Forward declarations -------------------------------------------------

class MemoryChunk;


// --- Function prototypes --------------------------------------------------

/// @name Memory management: MemoryChunk search tree
/// @{

/// @brief Insert memory chunk into chunk search tree.
///
/// Inserts a new memory @p chunk of size @p chunkSize bytes into the chunk
/// search tree (top-down splay tree) with the given @p root node, and returns
/// the new root node after the implicit tree restructuring.
/// (Algorithm as described in @cite SleatorTarjan:1985, p. 662)
///
/// @param root
///     Root node of memory chunk search tree
/// @param chunk
///     New memory chunk to be inserted
/// @param chunkSize
///     Size of the managed memory chunks (in bytes)
/// @returns
///     New root node of the chunk search tree
///
/// @exception_nothrow
///
MemoryChunk*
insert(MemoryChunk* root,
       MemoryChunk* chunk,
       std::size_t  chunkSize);

/// @brief Remove root node from chunk search tree.
///
/// Removes the @p root node from the chunk search tree (top-down splay tree)
/// storing memory chunks of @p chunkSize bytes, and returns the new root node.
/// (Algorithm as described in @cite SleatorTarjan:1985, p. 662)
///
/// @param root
///     Root node of the memory chunk search tree
/// @param chunkSize
///     Size of the managed memory chunks (in bytes)
/// @returns
///     New root node of the chunk search tree
///
/// @exception_nothrow
///
MemoryChunk*
remove(MemoryChunk* root,
       std::size_t  chunkSize);

/// @}
/// @name Memory management: MemoryChunck search tree helpers
/// @{

/// @brief Splaying operation on chunk search tree for a pointer.
///
/// Performs splaying on the chunk search tree (top-down splay tree) with the
/// given @p root node storing memory chunks of @p chunkSize bytes at the chunk
/// containing @p ptr, or the closest leaf node if @p ptr is not in the tree.
/// As a result, the function returns the new root node of the tree after
/// restructuring.
/// (Algorithm as described in @cite SleatorTarjan:1985, p. 655f., implemented
/// using a lookahead of two tree levels and recursion)
///
/// @param root
///     Root node of the memory chunk search tree
/// @param ptr
///     Pointer within chunk to splay at
/// @param chunkSize
///     Size of the managed memory chunks in bytes
/// @returns
///     New root node of the chunk search tree
///
/// @exception_nothrow
///
MemoryChunk*
splay(MemoryChunk* root,
      void*        ptr,
      std::size_t  chunkSize);

/// @brief Single left rotation of a (sub-)tree.
///
/// Performs a single left rotation of the (sub-)tree with the given @p root
/// node and returns the new root node.
///
/// @param root
///     Root node of the memory chunk search (sub-)tree
/// @returns
///     New root node of the (sub-)tree after the rotation
///
/// @pre
///     @p root != `nullptr`
/// @pre
///     @p root->mRight != `nullptr`
///
/// @exception_nothrow
///
MemoryChunk*
rotateLeft(MemoryChunk* root);

/// @brief Single right rotation of a (sub-)tree.
///
/// Performs a single right rotation of the (sub-)tree with the given @p root
/// node and returns the new root node.
///
/// @param root
///     Root node of the memory chunk search (sub-)tree
/// @returns
///     New root node of the (sub-)tree after the rotation
///
/// @pre
///     @p root != `nullptr`
/// @pre
///     @p root->mLeft != `nullptr`
///
/// @exception_nothrow
///
MemoryChunk*
rotateRight(MemoryChunk* root);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


// --- Inline member functions ----------------------------------------------

#include "MemoryChunkTree-inl.h"


#endif    // !PEARL_MEMORYCHUNKTREE_H
