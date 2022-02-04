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
 *  @brief   Implementation of the class LocationGroup.
 *
 *  This file provides the implementation of the class LocationGroup and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/LocationGroup.h>

#include <cassert>

#include <pearl/String.h>
#include <pearl/SystemNode.h>

using namespace std;
using namespace pearl;


// --- Static class data ----------------------------------------------------

const LocationGroup::IdType LocationGroup::NO_ID;


// --- Constructors & destructor --------------------------------------------

LocationGroup::LocationGroup(const IdType      id,
                             const String&     name,
                             SystemNode* const parent)
    : mIdentifier(id),
      mName(name),
      mParent(parent)
{
    // Dangling location groups are prohibited
    assert(parent);

    parent->addLocationGroup(this);
}


LocationGroup::~LocationGroup()
{
}


// --- Access definition data -----------------------------------------------

LocationGroup::IdType
LocationGroup::getId() const
{
    return mIdentifier;
}


const String&
LocationGroup::getName() const
{
    return mName;
}


SystemNode*
LocationGroup::getParent() const
{
    return mParent;
}


uint32_t
LocationGroup::numLocations() const
{
    return mLocations.size();
}


const Location&
LocationGroup::getLocation(const uint32_t index) const
{
    return *(mLocations.at(index));
}


// --- Private methods ------------------------------------------------------

/// @brief Add a location.
///
/// Attaches the given @a location to the location group instance.
///
/// @param location
///     %Location to attach (non-NULL)
///
void
LocationGroup::addLocation(Location* const location)
{
    assert(location);
    mLocations.push_back(location);
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&             stream,
                  const LocationGroup& item)
{
    return item.output(stream);
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const LocationGroup& lhs,
                  const LocationGroup& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getName() == rhs.getName())
           && (lhs.getType() == rhs.getType())
           && (lhs.getParent() == rhs.getParent()));
}


bool
pearl::operator!=(const LocationGroup& lhs,
                  const LocationGroup& rhs)
{
    return !(lhs == rhs);
}
