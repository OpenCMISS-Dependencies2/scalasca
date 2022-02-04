/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2017                                                **
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
 *           DefinitionContainer.
 *
 *  This header file provides declaration and implementation of the class
 *  template DefinitionContainer.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_DEFINITIONCONTAINER_H
#define PEARL_DEFINITIONCONTAINER_H


#include <cstddef>
#include <vector>

#include <pearl/Uncopyable.h>


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   DefinitionContainer
 *  @ingroup PEARL_base
 *  @brief   Class template providing a specialized container for global
 *           definition objects.
 *
 *  The class template DefinitionContainer implements an efficient storage
 *  container for global definition objects.  It is based on the assumption
 *  that insertions and lookups happen in distinct phases (i.e., first setting
 *  up the container by adding items, followed by many lookups), and that in
 *  many cases definition objects are added in either sorted or even in
 *  consecutive order with respect to their global identifiers.  Item lookups
 *  by identifier use the best strategy available for the current set of
 *  definitions (either direct lookup, binary search, or linear search),
 *  however, the search strategy can be optimized by an explicit call to the
 *  optimizeLookups() member function (e.g., at the end of an insertion phase).
 *
 *  A DefinitionContainer instance may be constructed from an incomplete type
 *  @p DefClassT.  However, @p DefClassT must be complete at the point where
 *  its destructor or any of the required member functions is invoked.
 *
 *  @tparam DefClassT
 *      @parblock
 *      Global definition class which defines a public typedef `IdType` of
 *      unsigned numeric type and provides a `IdType getId() const` member
 *      function returning the global identifier of an instance.
 *
 *      The behavior of this class is undefined if either the destructor, the
 *      `delete` operator, or the `IdType getId() const` member function for
 *      objects of type @p DefClassT throws exceptions.
 *      @endparblock
 *
 *  @thread_safety
 *      The DefinitionContainer class template is not thread-safe by itself.
 *      While queries/lookups can be carried out concurrently, they have to be
 *      properly protected from insertions and calls optimizing the lookup
 *      strategy.
 */
/*-------------------------------------------------------------------------*/

template< typename DefClassT >
class DefinitionContainer
    : private pearl::Uncopyable
{
    public:
        // --- Type definitions ----------------------------

        /// Base type of the managed definition objects.
        typedef DefClassT ElementType;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Default constructor.
        ///
        /// Creates a new empty global definition container instance.
        ///
        /// @exception_strong
        ///
        DefinitionContainer();

        /// @brief Destructor.
        ///
        /// Destroys the container instance.  This implicitly deletes the
        /// global definition objects stored in the container.
        ///
        /// @exception_nothrow
        ///     Depends on the requirement that the destructor and the `delete`
        ///     operator for objects of type @p DefClassT do not throw
        ///     exceptions.
        ///
        ~DefinitionContainer();

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Check whether container is empty.
        ///
        /// Checks if the container stores no elements.
        ///
        /// @returns
        ///     True if the container is empty, false otherwise
        ///
        /// @exception_nothrow
        ///
        bool
        isEmpty() const;

        /// @brief Get number of items.
        ///
        /// Returns the actual number of global definition objects stored in
        /// the container.
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

        /// @}
        /// @name Insertion functions
        /// @{

        /// @brief Add item.
        ///
        /// Adds the given global definition object @p item to the container,
        /// with transfer of ownership from the caller to the container.
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
        /// If needed, reorganizes the contents of the container to optimize
        /// the complexity of lookups by identifier.
        ///
        /// @exception_nothrow
        ///     Depends on the requirement that the `IdType getId() const`
        ///     member function for objects of type @p DefClassT does not throw
        ///     exceptions.
        ///
        void
        optimizeLookups();

        /// @}


    private:
        // --- Type definitions ----------------------------

        /// Internal container type
        typedef std::vector< DefClassT* > ContainerType;


        // --- Data members --------------------------------

        /// Container items (global definition objects)
        ContainerType mItems;

        /// True if items are sorted with respect to their global IDs
        bool mIsSorted;

        /// True if items have consecutive global IDs starting from zero
        bool mIsCompact;


        // --- Private methods -----------------------------

        /// @name Comparison functions
        /// @{

        static bool
        lessThanId(const DefClassT*           lhs,
                   typename DefClassT::IdType rhs);

        static bool
        equalToId(const DefClassT*           lhs,
                  typename DefClassT::IdType rhs);

        static bool
        lessThanItem(const DefClassT* lhs,
                     const DefClassT* rhs);

        /// @}
};
}    // namespace pearl::detail
}    // namespace pearl


// --- Class template implementation ----------------------------------------

#include "DefinitionContainer-inl.h"


#endif    // !PEARL_DEFINITIONCONTAINER_H
