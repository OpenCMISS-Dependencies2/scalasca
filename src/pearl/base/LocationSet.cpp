/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017-2019                                                **
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
 *  @brief   Implementation of the class LocationSet.
 *
 *  This file provides the implementation of the class LocationSet.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/LocationSet.h>

#include <iostream>

#include <pearl/Error.h>
#include <pearl/Location.h>
#include <pearl/String.h>
#include <pearl/Utils.h>
#include <pearl/iomanip.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

LocationSet::LocationSet(const IdType                     id,
                         const String&                    name,
                         const vector< const Location* >& members)
    : GroupingSet(id, name),
      mLocations(members)
{
}


// --- Query functions ------------------------------------------------------

GroupingSet::SetType
LocationSet::getType() const
{
    return GroupingSet::LOCATION_SET;
}


size_t
LocationSet::numLocations() const
{
    return mLocations.size();
}


const Location&
LocationSet::getLocation(const size_t rank) const
{
    if (rank >= mLocations.size())
    {
        throw RuntimeError("pearl::LocationSet::"
                           "getLocation(std::size_t) -- "
                           "invalid rank: " + toStdString(rank));
    }

    return *mLocations[rank];
}


// --- Stream I/O functions (private) ---------------------------------------

ostream&
LocationSet::output(ostream& stream) const
{
    const long detail = getDetail(stream);

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << getId();
    }

    // Print data
    const size_t numLocations = this->numLocations();
    stream << "LocationSet"
           << beginObject
           << "id: " << getId()
           << sep
           << "name: " << getName()
           << sep
           << "size: " << numLocations
           << sep
           << "members: "
           << beginArray;
    size_t index = 0;
    while (index < numLocations)
    {
        stream << getLocation(index);

        ++index;
        if (index < numLocations)
        {
            stream << sep;
        }
    }
    stream << endArray
           << endObject;

    return stream;
}
