/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2017                                                **
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
 *  @brief   Implementation of helper functions related to calling context
 *           trees.
 *
 *  This file provides the implementation of helper functions which are
 *  related to calling context trees.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "ContextTree.h"

#include <iostream>

#include <pearl/CallingContext.h>
#include <pearl/Region.h>
#include <pearl/SourceLocation.h>
#include <pearl/String.h>

using namespace std;
using namespace pearl;


namespace pearl
{
namespace detail
{
// --- Stream I/O functions -------------------------------------------------

ostream&
print(ostream&              stream,
      const CallingContext& item,
      const bool            isLastChild,
      const string&         prefix)
{
    // Special case: undefined calling context
    if (item == CallingContext::UNDEFINED)
    {
        return stream << prefix
                      << " + (undefined)"
                      << endl;
    }

    const Region&         region = item.getRegion();
    const SourceLocation& source = item.getSourceLocation();

    // Print current calling context
    stream << prefix
           << " + "
           << region.getDisplayName().getString();
    if (source != SourceLocation::UNDEFINED)
    {
        stream << " @ "
               << source.getFilename().getString()
               << ":"
               << source.getLine();
    }
    else
    {
        // Fallback: use filename stored with the region (if available)
        const String& filename = region.getFilename();
        if (filename != String::UNDEFINED)
        {
            stream << " @ "
                   << filename.getString();
        }
    }
    stream << "  <"
           << item.getId()
           << ">"
           << endl;

    // Recursively print children
    const string childPrefix(prefix + (isLastChild ? "   " : " | "));
    const size_t numChildren = item.numChildren();
    for (size_t i = 0; i < numChildren; ++i)
    {
        print(stream,
              item.getChild(i),
              i == (numChildren - 1),
              childPrefix);
    }

    return stream;
}
}    // namespace pearl::detail
}    // namespace pearl
