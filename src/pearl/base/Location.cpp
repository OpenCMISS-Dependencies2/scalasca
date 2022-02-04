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
 *  @brief   Implementation of the class Location.
 *
 *  This file provides the implementation of the class Location and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/Location.h>

#include <cassert>
#include <cstddef>
#include <iostream>

#include <pearl/Error.h>
#include <pearl/LocationGroup.h>
#include <pearl/String.h>

#include "Process.h"
#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Local helper functions -----------------------------------------------

namespace
{
const string
typeToString(const Location::Type type);
}    // unnamed namespace


// --- Constructors & destructor --------------------------------------------

Location::Location(const IdType         id,
                   const String&        name,
                   const Type           type,
                   const uint64_t       numEvents,
                   LocationGroup* const parent)
    : mIdentifier(id),
      mName(name),
      mType(type),
      mNumEvents(numEvents),
      mParent(parent)
{
    // Arbitrary int's can be assigned to an enum, i.e., we need a range check
    assert(  (type >= Location::TYPE_UNKNOWN)
          && (type < Location::NUMBER_OF_TYPES));

    // Dangling locations are prohibited, except for UNDEFINED location
    assert(  parent
          || (  (id == Location::NO_ID)
             && (name == String::UNDEFINED)
             && (type == Location::TYPE_UNKNOWN)
             && (numEvents == 0)));

    if (parent)
    {
        parent->addLocation(this);
    }
}


// --- Access definition data -----------------------------------------------

Location::IdType
Location::getId() const
{
    return mIdentifier;
}


const String&
Location::getName() const
{
    return mName;
}


Location::Type
Location::getType() const
{
    return mType;
}


uint64_t
Location::getNumEvents() const
{
    return mNumEvents;
}


LocationGroup*
Location::getParent() const
{
    return mParent;
}


// --- Access location properties -------------------------------------------

SystemNode::IdType
Location::getMachineId() const
{
    // Search machine node
    SystemNode* node = mParent->getParent();
    while (  node
          && (node->getClassName().getString() != "machine"))
    {
        node = node->getParent();
    }

    // Sanity check
    if (node == NULL)
    {
        throw FatalError("Unable to find machine in system tree");
    }

    return node->getId();
}


SystemNode::IdType
Location::getNodeId() const
{
    // Search node
    SystemNode* node = mParent->getParent();
    while (  node
          && (node->getClassName().getString() != "node"))
    {
        node = node->getParent();
    }

    // Sanity check
    if (node == NULL)
    {
        throw FatalError("Unable to find machine node in system tree");
    }

    return node->getId();
}


int
Location::getRank() const
{
    if (mType != Location::TYPE_CPU_THREAD)
    {
        return -1;
    }

    Process* process = static_cast< Process* >(mParent);

    return process->getRank();
}


/// @todo Do not rely on Score-P specific knowledge!
uint32_t
Location::getThreadId() const
{
    return (mIdentifier >> 32);
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&        stream,
                  const Location& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: undefined location
    if (item == Location::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << item.getId();
    }


    // Print data
    stream << "Location"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "name: " << item.getName()
           << sep
           << "type: " << typeToString(item.getType())
           << sep
           << "#events: " << item.getNumEvents()
           << sep
           << "parent: ";
    const LocationGroup* const parent = item.getParent();
    if (parent == NULL)
    {
        stream << "<undef>";
    }
    else
    {
        stream << *parent;
    }
    stream << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const Location& lhs,
                  const Location& rhs)
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
pearl::operator!=(const Location& lhs,
                  const Location& rhs)
{
    return !(lhs == rhs);
}


// --- Local helper functions -----------------------------------------------

namespace
{
/// @brief Get string representation of a location type.
///
/// Returns a human-readable string representation of the given location
/// @a type, used by the stream I/O functionality.
///
/// @param type
///     %Location type
/// @returns
///     Corresponding string representation
///
const string
typeToString(const Location::Type type)
{
    switch (type)
    {
        case Location::TYPE_UNKNOWN:
            return "UNKNOWN";

        case Location::TYPE_CPU_THREAD:
            return "CPU thread";

        case Location::TYPE_GPU:
            return "GPU";

        case Location::TYPE_METRIC:
            return "Metric";

        // For "NUMBER_OF_TYPES" -- to make the compiler happy...
        default:
            break;
    }

    // Since all possible location types should be handled in the switch
    // statement above, something went wrong if we reach this point...
    assert(false);

    return "";
}
}    // unnamed namespace
