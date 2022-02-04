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
 *  @brief   Implementation of the class CommLocationSet.
 *
 *  This file provides the implementation of the class CommLocationSet.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/CommLocationSet.h>

#include <iostream>

#include <pearl/Location.h>
#include <pearl/String.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

CommLocationSet::CommLocationSet(const IdType                     id,
                                 const String&                    name,
                                 const Paradigm                   paradigm,
                                 const vector< const Location* >& members)
    : LocationSet(id, name, members),
      mParadigm(paradigm)
{
}


// --- Query functions ------------------------------------------------------

GroupingSet::SetType
CommLocationSet::getType() const
{
    return GroupingSet::COMM_LOCATION_SET;
}


Paradigm
CommLocationSet::getParadigm() const
{
    return mParadigm;
}


// --- Stream I/O functions (private) ---------------------------------------

ostream&
CommLocationSet::output(ostream& stream) const
{
    // Special case: undefined comm location set
    if (this == &CommLocationSet::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    const long detail = getDetail(stream);
    if (detail == DETAIL_ABBRV)
    {
        return stream << getId();
    }

    // Print data
    const size_t numLocations = this->numLocations();
    stream << "CommLocationSet"
           << beginObject
           << "id: " << getId()
           << sep
           << "name: " << getName()
           << sep
           << "paradigm: " << getParadigm()
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
