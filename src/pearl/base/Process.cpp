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
 *  @brief   Implementation of the class Process.
 *
 *  This file provides the implementation of the class Process and related
 *  functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "Process.h"

#include <cassert>
#include <iostream>

#include <pearl/String.h>
#include <pearl/SystemNode.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

Process::Process(const IdType      id,
                 const String&     name,
                 SystemNode* const parent)
    : LocationGroup(id, name, parent),
      mRank(-1)
{
}


// --- Access definition data -----------------------------------------------

LocationGroup::Type
Process::getType() const
{
    return LocationGroup::TYPE_PROCESS;
}


int
Process::getRank() const
{
    return mRank;
}


// --- Set definition data --------------------------------------------------

void
Process::setRank(int rank)
{
    // Assigned rank numbers need to be >= 0
    assert(rank >= 0);

    mRank = rank;
}


// --- Private methods ------------------------------------------------------

/// @brief Stream output helper function.
///
/// Prints the location group's attributes to the given @a stream. This
/// member function is triggered by the stream output operator for objects
/// of type LocationGroup.
///
/// @param stream
///     Output stream
/// @returns
///     Output stream
///
ostream&
Process::output(ostream& stream) const
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << getId();
    }

    // Print data (NOTE: dangling location groups w/o parent are prohibited)
    stream << "Process"
           << beginObject
           << "id: " << getId()
           << sep
           << "name: " << getName()
           << sep
           << "rank: " << getRank()
           << sep
           << "parent: " << *getParent()
           << endObject;

    return stream;
}
