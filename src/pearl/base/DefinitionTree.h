/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration and implementation of the class template
 *           DefinitionTree.
 *
 *  This header file provides declaration and implementation of the class
 *  template DefinitionTree.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_DEFINITIONTREE_H
#define PEARL_DEFINITIONTREE_H


#include <cstddef>
#include <iosfwd>
#include <vector>

#include <pearl/Uncopyable.h>

#include "DefinitionContainer.h"


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   DefinitionTree
 *  @ingroup PEARL_base
 *  @brief   Class template providing a specialized container for global
 *           definition objects that form a tree structure.
 *
 *  The class template DefinitionTree implements an efficient storage
 *  container for global definition objects that form a tree structure (e.g.,
 *  call tree or system tree) by keeping track of parent-child relationships.
 *  Like the DefinitionContainer class template, the DefinitionTree is based
 *  on the assumption that insertions and lookups happen in distinct phases
 *  (i.e., first setting up the container by adding items, followed by many
 *  lookups), and that in many cases definition objects are added in either
 *  sorted or even in consecutive order with respect to their global
 *  identifiers.  Item lookups by identifier use the best strategy available
 *  for the current set of definitions (either direct lookup, binary search,
 *  or linear search), however, the search strategy can be optimized by an
 *  explicit call to the optimizeLookups() member function (e.g., at the end
 *  of an insertion phase).
 *
 *  A DefinitionTree instance may be constructed from an incomplete type
 *  @p DefClassT.  However, @p DefClassT must be complete at the point where
 *  its destructor or any of the required member functions is invoked.
 *
 *  @tparam DefClassT
 *      @parblock
 *      Global definition class which defines a public typedef `IdType` of
 *      unsigned numeric type and provides a `IdType getId() const` as well
 *      as a `DefClassT* getParent() const` member function returning the
 *      global identifier and a pointer to the parent item of an instance,
 *      respectively.
 *
 *      The behavior of this class is undefined if either the destructor,
 *      the `delete` operator, or the `IdType getId() const` member function
 *      for objects of type @p DefClassT throws exceptions.
 *      @endparblock
 *
 *  @thread_safety
 *      The DefinitionTree class template is not thread-safe by itself.
 *      While queries/lookups can be carried out concurrently, they have
 *      to be properly protected from insertions and calls optimizing the
 *      lookup strategy.
 */
/*-------------------------------------------------------------------------*/

template< typename DefClassT >
class DefinitionTree
    : private pearl::Uncopyable
{
    public:
        // --- Type definitions ----------------------------

        /// Base type of the managed definition objects.
        typedef DefClassT ElementType;


        // --- Public methods ------------------------------

        /// @name Query functions
        /// @{

        /// @brief Check whether tree container is empty.
        ///
        /// Checks if the tree container stores no elements.
        ///
        /// @returns
        ///     True if the tree container is empty, false otherwise
        ///
        /// @exception_nothrow
        ///
        bool
        isEmpty() const;

        /// @brief Get number of items.
        ///
        /// Returns the actual number of global definition objects stored in
        /// the tree container.
        ///
        /// @returns
        ///     Number of items
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numItems() const;

        /// @brief Lookup by identifier.
        ///
        /// Returns a pointer to the definition object with the given global
        /// identifier @p id, while retaining ownership of the definition
        /// returned to the caller.
        ///
        /// @param id
        ///     Global identifier of the definition object to be retrieved
        /// @returns
        ///     Pointer to corresponding definition object
        ///
        /// @exception_strong
        ///
        /// @throws std::invalid_argument
        ///     on failure to find a definition object with the given @p id
        ///
        DefClassT*
        getItemById(typename DefClassT::IdType id) const;

        /// @brief Lookup by index.
        ///
        /// Returns a pointer to the definition object stored at the given
        /// @p index, while retaining ownership of the definition returned to
        /// the caller.
        ///
        /// @param index
        ///     Index in the range [0,@#items-1]
        /// @returns
        ///     Pointer to corresponding definition object
        ///
        /// @exception_strong
        ///
        /// @throws std::out_of_range
        ///     on @p index &notin; [0,@#items-1]
        ///
        DefClassT*
        getItemByIndex(std::size_t index) const;

        /// @brief Get number of root items.
        ///
        /// Returns the actual number of root definitions (i.e., definitions
        /// without parent) in this tree container.
        ///
        /// @returns
        ///     Number of root items
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numRootItems() const;

        /// @brief Root item lookup by index.
        ///
        /// Returns a pointer to the root definition (i.e., definition without
        /// parent) with the given @p index, while retaining ownership of the
        /// definition returned to the caller.
        ///
        /// @param index
        ///     Index in the range [0,@#root_items-1]
        /// @returns
        ///     Pointer to the corresponding definition object
        ///
        /// @exception_strong
        ///
        /// @throws std::out_of_range
        ///     on @p index &notin; [0,@#root_items-1]
        ///
        DefClassT*
        getRootItemByIndex(std::size_t index) const;

        /// @}
        /// @name Insertion functions
        /// @{

        /// @brief Add item.
        ///
        /// Adds the given global definition object @p item to the tree
        /// container, with transfer of ownership from the caller to the
        /// container.
        ///
        /// @param item
        ///     Pointer to definition object to be added
        ///
        /// @exception_safety
        ///     @parblock
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        ///     Depends on the requirement that the `IdType getId() const`
        ///     member function for objects of type @p DefClassT does not throw
        ///     exceptions.
        ///     @endparblock
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addItem(DefClassT* item);

        /// @}
        /// @name Miscellaneous functions
        /// @{

        /// @brief Optimize item lookups.
        ///
        /// If needed, reorganizes the contents of the tree container to
        /// optimize the complexity of lookups by identifier.
        ///
        /// @exception_nothrow
        ///     Depends on the requirement that the `IdType getId() const`
        ///     member function for objects of type @p DefClassT does not throw
        ///     exceptions.
        ///
        void
        optimizeLookups();

        /// @}


    protected:
        // --- Type definitions ----------------------------

        /// Internal container type for all tree items
        typedef DefinitionContainer< DefClassT > ContainerType;

        /// Internal container type for root items
        typedef std::vector< DefClassT* > RootContainerType;


        // --- Data members --------------------------------

        /// Tree items (global definition objects)
        ContainerType mItems;

        /// Root items (global definition objects) of the tree
        RootContainerType mRootItems;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates DefinitionTree
///
/// Prints the contents of the given definition tree container @p item to an
/// output @p stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     Definition tree object
/// @returns
///     Output stream
///
/// @tparam DefClassT
///     Global definition class for which a free function
///     @code
///         std::ostream&
///         print(std::ostream&    stream,
///               const DefClassT& item,
///               bool             isLastChild);
///     @endcode
///     exists, which recursively prints a human-readable representation of
///     @p item and its children to the output @p stream.  The @p isLastChild
///     parameter is set to true for the last root item of the tree and to
///     false otherwise.
///
template< typename DefClassT >
std::ostream&
operator<<(std::ostream&                      stream,
           const DefinitionTree< DefClassT >& item);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


// --- Class template implementation ----------------------------------------

#include "DefinitionTree-inl.h"


#endif    // !PEARL_DEFINITIONTREE_H
