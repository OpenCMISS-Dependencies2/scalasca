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


#ifndef PEARL_DEFINITIONTREE_H
    #error "Do not include this file!  Use 'DefinitionTree.h' instead."
#endif


#include <stdexcept>

#include <pearl/Utils.h>


namespace pearl
{
namespace detail
{
// --- Query functions ------------------------------------------------------

template< typename DefClassT >
bool
DefinitionTree< DefClassT >::isEmpty() const
{
    return mItems.isEmpty();
}


template< typename DefClassT >
std::size_t
DefinitionTree< DefClassT >::numItems() const
{
    return mItems.numItems();
}


template< typename DefClassT >
DefClassT*
DefinitionTree< DefClassT >::getItemById(const typename DefClassT::IdType id) const
{
    return mItems.getItemById(id);
}


template< typename DefClassT >
DefClassT*
DefinitionTree< DefClassT >::getItemByIndex(const std::size_t index) const
{
    return mItems.getItemByIndex(index);
}


template< typename DefClassT >
std::size_t
DefinitionTree< DefClassT >::numRootItems() const
{
    return mRootItems.size();
}


template< typename DefClassT >
DefClassT*
DefinitionTree< DefClassT >::getRootItemByIndex(const std::size_t index) const
{
    if (index >= mRootItems.size())
    {
        throw std::out_of_range("pearl::detail::DefinitionTree<T>::"
                                "getRootItemByIndex(size_t) -- "
                                "index out of range: " + toStdString(index));
    }

    return mRootItems[index];
}


// --- Insertion functions --------------------------------------------------

template< typename DefClassT >
void
DefinitionTree< DefClassT >::addItem(DefClassT* const item)
{
    if (item == 0)
    {
        throw std::invalid_argument("pearl::detail::DefinitionTree<T>::"
                                    "addItem(T*) -- "
                                    "invalid argument: `nullptr`");
    }

    if (item->getParent() != 0)
    {
        mItems.addItem(item);
    }
    else
    {
        mRootItems.push_back(item);
        try
        {
            mItems.addItem(item);
        }
        catch (std::exception& ex)
        {
            // Roll back to guarantee basic exception safety
            mRootItems.pop_back();
            throw;
        }
    }
}


// --- Miscellaneous functions ----------------------------------------------

template< typename DefClassT >
void
DefinitionTree< DefClassT >::optimizeLookups()
{
    mItems.optimizeLookups();
}


// --- Related functions ----------------------------------------------------

template< typename DefClassT >
std::ostream&
operator<<(std::ostream&                      stream,
           const DefinitionTree< DefClassT >& item)
{
    // Print root items, which recursively print their children
    const std::size_t numRoots = item.numRootItems();
    for (std::size_t i = 0; i < numRoots; ++i)
    {
        print(stream, *item.getRootItemByIndex(i), i == (numRoots - 1));
    }

    return stream;
}
}    // namespace pearl::detail
}    // namespace pearl
