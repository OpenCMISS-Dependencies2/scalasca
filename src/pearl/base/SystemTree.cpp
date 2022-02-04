/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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
 *  @brief   Implementation of helper functions related to system trees.
 *
 *  This file provides the implementation of helper functions which are
 *  related to system trees.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "SystemTree.h"

#include <iostream>

#include <pearl/Location.h>
#include <pearl/LocationGroup.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>

using namespace std;
using namespace pearl;


namespace pearl
{
namespace detail
{
// --- Stream I/O functions -------------------------------------------------

ostream&
print(ostream&        stream,
      const Location& item,
      const string&   prefix)
{
    // Special case: undefined location
    if (item == Location::UNDEFINED)
    {
        return stream << prefix
                      << " + (undefined)"
                      << endl;
    }

    // Print current location
    return stream << prefix
                  << " + "
                  << item.getName().getString()
                  << "  <l:"
                  << item.getId()
                  << ">"
                  << endl;
}


ostream&
print(ostream&             stream,
      const LocationGroup& item,
      const bool           isLastChild,
      const string&        prefix)
{
    // Print current location group
    stream << prefix
           << " + "
           << item.getName().getString()
           << "  <lg:"
           << item.getId()
           << ">"
           << endl;

    // Recursively print child locations
    const string childPrefix(prefix + (isLastChild ? "   " : " | "));
    const size_t numLocations = item.numLocations();
    for (size_t i = 0; i < numLocations; ++i)
    {
        print(stream,
              item.getLocation(i),
              childPrefix);
    }

    return stream;
}


ostream&
print(ostream&          stream,
      const SystemNode& item,
      const bool        isLastChild,
      const string&     prefix)
{
    // Special case: undefined system tree node
    if (item == SystemNode::UNDEFINED)
    {
        return stream << prefix
                      << " + (undefined)"
                      << endl;
    }

    // Print current system tree node
    stream << prefix
           << " + "
           << item.getName().getString()
           << "  <sn:"
           << item.getId()
           << ">"
           << endl;

    // Recursively print child system tree nodes
    const string childPrefix(prefix + (isLastChild ? "   " : " | "));
    const size_t numChildren = item.numChildren();
    for (size_t i = 0; i < numChildren; ++i)
    {
        print(stream,
              item.getChild(i),
              (i == (numChildren - 1)) && (item.numLocationGroups() == 0),
              childPrefix);
    }

    // Recursively print location groups
    const size_t numLocGroups = item.numLocationGroups();
    for (size_t i = 0; i < numLocGroups; ++i)
    {
        print(stream,
              item.getLocationGroup(i),
              i == (numLocGroups - 1),
              childPrefix);
    }

    return stream;
}
}    // namespace pearl::detail
}    // namespace pearl
