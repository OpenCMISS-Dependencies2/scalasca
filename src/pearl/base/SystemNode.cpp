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
 *  @brief   Implementation of the class SystemNode.
 *
 *  This file provides the implementation of the class SystemNode and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/SystemNode.h>

#include <cassert>
#include <cstddef>
#include <iostream>

#include <pearl/String.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

SystemNode::SystemNode(const IdType      id,
                       const String&     name,
                       const String&     className,
                       SystemNode* const parent)
    : mIdentifier(id),
      mName(name),
      mClassName(className),
      mParent(parent)
{
    if (parent)
    {
        parent->mChildren.push_back(this);
    }
}


// --- Access definition data -----------------------------------------------

SystemNode::IdType
SystemNode::getId() const
{
    return mIdentifier;
}


const String&
SystemNode::getName() const
{
    return mName;
}


const String&
SystemNode::getClassName() const
{
    return mClassName;
}


SystemNode*
SystemNode::getParent() const
{
    return mParent;
}


uint32_t
SystemNode::numChildren() const
{
    return mChildren.size();
}


const SystemNode&
SystemNode::getChild(const uint32_t index) const
{
    return *(mChildren.at(index));
}


uint32_t
SystemNode::numLocationGroups() const
{
    return mLocationGroups.size();
}


const LocationGroup&
SystemNode::getLocationGroup(uint32_t index) const
{
    return *(mLocationGroups.at(index));
}


// --- Private methods ------------------------------------------------------

/// @brief Add a location group.
///
/// Attaches the given location group to the system node instance.
///
/// @param group
///     %Location group to attach (non-NULL)
///
void
SystemNode::addLocationGroup(LocationGroup* const group)
{
    assert(group);
    mLocationGroups.push_back(group);
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&          stream,
                  const SystemNode& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: undefined system node
    if (item == SystemNode::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << item.getId();
    }

    // Print data
    const SystemNode* const parent = item.getParent();
    stream << "SystemNode"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "name: " << item.getName()
           << sep
           << "class: " << item.getClassName()
           << sep
           << "parent: " << (parent ? *parent : SystemNode::UNDEFINED)
           << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const SystemNode& lhs,
                  const SystemNode& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getName() == rhs.getName())
           && (lhs.getClassName() == rhs.getClassName())
           && (lhs.getParent() == rhs.getParent()));
}


bool
pearl::operator!=(const SystemNode& lhs,
                  const SystemNode& rhs)
{
    return !(lhs == rhs);
}
