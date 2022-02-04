/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_DEFINITIONCONTAINER_H
    #error "Do not include this file!  Use 'DefinitionContainer.h' instead."
#endif


#include <algorithm>
#include <functional>
#include <stdexcept>

#include <pearl/Utils.h>

#include "Functors.h"


namespace pearl
{
namespace detail
{
// --- Constructors & destructor --------------------------------------------

template< typename DefClassT >
DefinitionContainer< DefClassT >::DefinitionContainer()
    : mIsSorted(true),
      mIsCompact(true)
{
}


template< typename DefClassT >
DefinitionContainer< DefClassT >::~DefinitionContainer()
{
    std::for_each(mItems.begin(), mItems.end(), delete_ptr< DefClassT >());
}


// --- Query functions ------------------------------------------------------

template< typename DefClassT >
bool
DefinitionContainer< DefClassT >::isEmpty() const
{
    return mItems.empty();
}


template< typename DefClassT >
std::size_t
DefinitionContainer< DefClassT >::numItems() const
{
    return mItems.size();
}


template< typename DefClassT >
DefClassT*
DefinitionContainer< DefClassT >::getItemById(const typename DefClassT::IdType id) const
{
    // Compact IDs: direct lookup
    if (mIsCompact)
    {
        if (id >= mItems.size())
        {
            throw std::invalid_argument("pearl::detail::DefinitionContainer<T>::"
                                        "getItemById(T::IdType) -- "
                                        "invalid ID: " + toStdString(id));
        }

        return mItems[id];
    }

    // Sorted IDs: binary search
    if (mIsSorted)
    {
        typename ContainerType::const_iterator it =
            std::lower_bound(mItems.begin(), mItems.end(), id,
                             std::ptr_fun(lessThanId));
        if (  (it == mItems.end())
           || ((*it)->getId() != id))
        {
            throw std::invalid_argument("pearl::detail::DefinitionContainer<T>::"
                                        "getItemById(T::IdType) -- "
                                        "invalid ID: " + toStdString(id));
        }

        return *it;
    }

    // Fallback: linear search
    typename ContainerType::const_iterator it =
        std::find_if(mItems.begin(), mItems.end(),
                     std::bind2nd(std::ptr_fun(equalToId), id));
    if (it == mItems.end())
    {
        throw std::invalid_argument("pearl::detail::DefinitionContainer<T>::"
                                    "getItemById(T::IdType) -- "
                                    "invalid ID: " + toStdString(id));
    }

    return *it;
}


template< typename DefClassT >
DefClassT*
DefinitionContainer< DefClassT >::getItemByIndex(const std::size_t index) const
{
    if (index >= mItems.size())
    {
        throw std::out_of_range("pearl::detail::DefinitionContainer<T>::"
                                "getItemByIndex(size_t) -- "
                                "index out of range: " + toStdString(index));
    }

    return mItems[index];
}


// --- Insertion functions --------------------------------------------------

template< typename DefClassT >
void
DefinitionContainer< DefClassT >::addItem(DefClassT* const item)
{
    if (item == 0)
    {
        throw std::invalid_argument("pearl::detail::DefinitionContainer<T>::"
                                    "addItem(T*) -- "
                                    "invalid argument: `nullptr`");
    }

    const typename DefClassT::IdType nextCompactId = mItems.size();
    const DefClassT* const           lastItem      = mItems.empty() ? 0 : mItems.back();

    mItems.push_back(item);

    if (mIsCompact && (item->getId() != nextCompactId))
    {
        mIsCompact = false;
    }
    if (  !mIsCompact && mIsSorted
       && lastItem && (item->getId() < lastItem->getId()))
    {
        mIsSorted = false;
    }
}


// --- Miscellaneous functions ----------------------------------------------

template< typename DefClassT >
void
DefinitionContainer< DefClassT >::optimizeLookups()
{
    if (!mIsSorted)
    {
        std::sort(mItems.begin(), mItems.end(), std::ptr_fun(lessThanItem));
        mIsSorted = true;
    }

    // As a second step, one could check whether the IDs are compact.
    // However, such a check would have linear complexity, i.e., one would
    // need a sufficiently large number of lookups to amortize this cost
    // via the faster lookups.  For typical global definition data such as
    // the one generated by Score-P, this is unlikely to pay off.  Thus,
    // this additional check is intentionally not carried out.
}


// --- Private methods ------------------------------------------------------

/// @brief Less-than inequality comparison between definition object and
///        global identifier.
///
/// Static member function whose call returns whether the global identifier
/// of @p lhs compares "less than" the given identifier @p rhs (as returned
/// by `operator<`).
///
/// @param lhs
///     Left-hand operand: global definition object
/// @param rhs
///     Right-hand operand: global identifier
/// @returns
///     True if @p lhs' global identifier compares "less than" @p rhs, false
///     otherwise
///
/// @exception_nothrow
///     Depends on the requirement that the `getId() const` member function
///     for objects of type @p DefClassT does not throw exceptions.
///
template< typename DefClassT >
bool
DefinitionContainer< DefClassT >::lessThanId(const DefClassT* const           lhs,
                                             const typename DefClassT::IdType rhs)
{
    return (lhs->getId() < rhs);
}


/// @brief Equality comparison between definition object and global identifier.
///
/// Static member function whose call returns whether the global identifier
/// of @p lhs compares "equal to" the given identifier @p rhs (as returned
/// by `operator==`).
///
/// @param lhs
///     Left-hand operand: global definition object
/// @param rhs
///     Right-hand operand: global identifier
/// @returns
///     True if @p lhs' global identifier compares "equal to" @p rhs, false
///     otherwise
///
/// @exception_nothrow
///     Depends on the requirement that the `getId() const` member function
///     for objects of type @p DefClassT does not throw exceptions.
///
template< typename DefClassT >
bool
DefinitionContainer< DefClassT >::equalToId(const DefClassT* const           lhs,
                                            const typename DefClassT::IdType rhs)
{
    return (lhs->getId() == rhs);
}


/// @brief Less-than inequality comparison between two definition objects.
///
/// Static member function whose call returns whether the global identifier
/// of @p lhs compares "less than" the global identifier of @p rhs (as returned
/// by `operator<`).
///
/// @param lhs
///     Left-hand operand: global definition object
/// @param rhs
///     Right-hand operand: global definition object
/// @returns
///     True if @p lhs compares "less than" @p rhs with respect to their
///     global identifiers, false otherwise
///
/// @exception_nothrow
///     Depends on the requirement that the `getId() const` member function
///     for objects of type @p DefClassT does not throw exceptions.
///
template< typename DefClassT >
bool
DefinitionContainer< DefClassT >::lessThanItem(const DefClassT* const lhs,
                                               const DefClassT* const rhs)
{
    return (lhs->getId() < rhs->getId());
}
}    // namespace pearl::detail
}    // namespace pearl
