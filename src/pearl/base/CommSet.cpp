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
 *  @brief   Implementation of the class CommSet.
 *
 *  This file provides the implementation of the class CommSet and related
 *  functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/CommSet.h>

#include <algorithm>
#include <iostream>

#include <pearl/CommLocationSet.h>
#include <pearl/Error.h>
#include <pearl/Location.h>
#include <pearl/String.h>
#include <pearl/Utils.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

CommSet::CommSet(const IdType              id,
                 const String&             name,
                 const Properties&         properties,
                 const CommLocationSet&    commLocationSet,
                 const vector< uint32_t >& ranks)
    : GroupingSet(id, name),
      mRanks(ranks),
      mCommLocationSet(commLocationSet),
      mProperties(properties)
{
}


// --- Query functions ------------------------------------------------------

GroupingSet::SetType
CommSet::getType() const
{
    return GroupingSet::COMM_SET;
}


GroupingSet::Properties
CommSet::getProperties() const
{
    return mProperties;
}


const CommLocationSet&
CommSet::getCommLocationSet() const
{
    return mCommLocationSet;
}


Paradigm
CommSet::getParadigm() const
{
    return mCommLocationSet.getParadigm();
}


uint32_t
CommSet::numRanks() const
{
    // Special case: world-like group
    if (mProperties.test(GroupingSet::PROPERTY_WORLD))
    {
        return mCommLocationSet.numLocations();
    }

    return mRanks.size();
}


uint32_t
CommSet::getLocalRank(const uint32_t globalRank) const
{
    // Special case: world-like groups have a 1:1 rank mapping
    if (mProperties.test(GroupingSet::PROPERTY_WORLD))
    {
        if (globalRank >= mCommLocationSet.numLocations())
        {
            throw RuntimeError("pearl::CommSet::getLocalRank(uint32_t)"
                               " -- Invalid global rank: "
                               + toStdString(globalRank));
        }

        return globalRank;
    }

    // Linear search for global rank
    RankContainer::const_iterator it;
    it = find(mRanks.begin(), mRanks.end(), globalRank);
    if (it == mRanks.end())
    {
        throw RuntimeError("pearl::CommSet::getLocalRank(uint32_t)"
                           " -- Invalid global rank: "
                           + toStdString(globalRank));
    }

    return (it - mRanks.begin());
}


uint32_t
CommSet::getGlobalRank(const uint32_t localRank) const
{
    // Sanity check
    const uint32_t size = numRanks();
    if (localRank >= size)
    {
        throw RuntimeError("pearl::CommSet::getGlobalRank(uint32_t)"
                           " -- Invalid local rank: "
                           + toStdString(localRank));
    }

    // Special case: world-like groups have a 1:1 rank mapping
    if (mProperties.test(GroupingSet::PROPERTY_WORLD))
    {
        return localRank;
    }

    // Direct lookup of global rank
    return mRanks[localRank];
}


const Location&
CommSet::getLocation(const uint32_t localRank) const
{
    return mCommLocationSet.getLocation(getGlobalRank(localRank));
}


// --- Stream I/O functions (private) ---------------------------------------

ostream&
CommSet::output(ostream& stream) const
{
    // Special case: undefined communication set
    if (this == &CommSet::UNDEFINED)
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
    const size_t numRanks = this->numRanks();
    stream << "CommSet"
           << beginObject
           << "id: " << getId()
           << sep
           << "name: " << getName()
           << sep
           << "props: " << toStdString(getProperties())
           << sep
           << "paradigm: " << getParadigm()
           << sep
           << "size: " << numRanks
           << sep
           << "members: "
           << beginArray;
    size_t rank = 0;
    while (rank < numRanks)
    {
        stream << getLocation(rank);

        ++rank;
        if (rank < numRanks)
        {
            stream << sep;
        }
    }
    stream << endArray
           << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const CommSet& lhs,
                  const CommSet& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getName() == rhs.getName())
           && (lhs.getProperties() == rhs.getProperties())
           && (lhs.numRanks() == rhs.numRanks()));
}


bool
pearl::operator!=(const CommSet& lhs,
                  const CommSet& rhs)
{
    return !(lhs == rhs);
}
