/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class Callpath.
 *
 *  This file provides the implementation of the class Callpath and related
 *  functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/Callpath.h>

#include <algorithm>
#include <cstddef>
#include <iostream>

#include <pearl/Callsite.h>
#include <pearl/Region.h>
#include <pearl/String.h>

#include "CallpathMatcher.h"
#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

Callpath::Callpath(const IdType    id,
                   const Region&   region,
                   const Callsite& callsite,
                   Callpath* const parent)
    : mIdentifier(id),
      mRegion(region),
      mCallsite(callsite),
      mParent(parent)
{
    if (parent)
    {
        parent->mChildren.push_back(this);
    }
}


// --- Access definition data -----------------------------------------------

Callpath::IdType
Callpath::getId() const
{
    return mIdentifier;
}


const Region&
Callpath::getRegion() const
{
    return mRegion;
}


const Callsite&
Callpath::getCallsite() const
{
    return mCallsite;
}


Callpath*
Callpath::getParent() const
{
    return mParent;
}


uint32_t
Callpath::numChildren() const
{
    return mChildren.size();
}


const Callpath&
Callpath::getChild(const uint32_t index) const
{
    return *(mChildren.at(index));
}


// --- Private methods ------------------------------------------------------

/// @brief Stream output helper function.
///
/// Prints the referenced region name and the callpath's identifier to the
/// given @p stream before recursively processing all child callpaths.  This
/// member function is triggered by CallTree::print() to print a nicely
/// formatted calltree.
///
/// @param stream
///     Output stream
/// @param prefix
///     Prefix printed before the callpath data
/// @param isLast
///     True if the callpath is the last entry in the child list of its
///     parent, false otherwise.
/// @returns
///     Output stream
///
ostream&
Callpath::print(ostream&      stream,
                const string& prefix,
                const bool    isLast) const
{
    // Print current callpath
    stream << prefix
           << " + " << mRegion.getDisplayName().getString()
           << "  <" << mIdentifier << ">"
           << endl;

    // Process children
    size_t count = mChildren.size();
    for (size_t index = 0; index < count; ++index)
    {
        mChildren[index]->print(stream,
                                prefix + (isLast ? "   " : " | "),
                                (count == (index + 1)));
    }

    return stream;
}


/// @brief Search a particular child callpath.
///
/// Returns a pointer to the child callpath referencing the given @p region
/// and @p callsite, or NULL if no such child exists.
///
/// @param region
///     Called region
/// @param callsite
///     Specific callsite (Callsite::UNDEFINED if not applicable)
/// @returns
///     Pointer to child callpath if existent, NULL otherwise
///
Callpath*
Callpath::findChild(const Region&   region,
                    const Callsite& callsite) const
{
    ChildContainer::const_iterator it = find_if(mChildren.begin(),
                                                mChildren.end(),
                                                CallpathMatcher(region,
                                                                callsite));
    if (it == mChildren.end())
    {
        return NULL;
    }

    return *it;
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&        stream,
                  const Callpath& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: undefined callpath
    if (item == Callpath::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << item.getId();
    }

    // Print data
    const Callpath* const parent = item.getParent();
    stream << "Callpath"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "region: " << item.getRegion()
           << sep
           << "callsite: " << item.getCallsite()
           << sep
           << "parent: " << (parent ? *parent : Callpath::UNDEFINED)
           << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const Callpath& lhs,
                  const Callpath& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getRegion() == rhs.getRegion())
           && (lhs.getCallsite() == rhs.getCallsite())
           && (lhs.getParent() == rhs.getParent()));
}


bool
pearl::operator!=(const Callpath& lhs,
                  const Callpath& rhs)
{
    return !(lhs == rhs);
}
