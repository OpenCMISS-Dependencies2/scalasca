/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2019                                                     **
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
 *  @brief   Implementation of the class CartTopology.
 *
 *  This file provides the implementation of the class CartTopology and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/CartTopology.h>

#include <iostream>

#include <pearl/CartDimension.h>
#include <pearl/Communicator.h>
#include <pearl/Error.h>
#include <pearl/String.h>
#include <pearl/Utils.h>
#include <pearl/iomanip.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

CartTopology::CartTopology(const IdType              id,
                           const String&             name,
                           const Communicator&       communicator,
                           const DimensionContainer& dimensions)
    : mCommunicator(&communicator),
      mDimensions(dimensions),
      mName(name),
      mIdentifier(id)
{
}


CartTopology::~CartTopology()
{
}


// --- Access definition data -----------------------------------------------

CartTopology::IdType
CartTopology::getId() const
{
    return mIdentifier;
}


const String&
CartTopology::getName() const
{
    return mName;
}


const Communicator&
CartTopology::getCommunicator() const
{
    return *mCommunicator;
}


uint8_t
CartTopology::numDimensions() const
{
    return mDimensions.size();
}


const CartDimension&
CartTopology::getDimension(const uint8_t index) const
{
    if (index >= mDimensions.size())
    {
        throw RuntimeError("pearl::CartTopology::"
                           "getDimension(uint8_t) -- "
                           "invalid index: " + toStdString(index));
    }

    return *mDimensions[index];
}


const CartCoordList
CartTopology::getRankCoordinates(const uint32_t rank) const
{
    if (rank >= mCommunicator->getSize())
    {
        throw RuntimeError("pearl::CartTopology::"
                           "getRankCoordinates(uint32_t) -- "
                           "invalid rank: " + toStdString(rank));
    }

    CoordinateContainer::const_iterator it = mCoordinates.find(rank);
    if (it == mCoordinates.end())
    {
        return CartCoordList();
    }

    return it->second;
}


// --- Stream I/O functions -------------------------------------------------

ostream&
pearl::operator<<(ostream&            stream,
                  const CartTopology& item)
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
        return stream << item.getId();
    }

    // Print data
    const uint16_t numDimensions = item.numDimensions();
    stream << "CartTopology"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "name: " << item.getName()
           << sep
           << "comm: " << item.getCommunicator()
           << sep
           << "#dims: " << numDimensions
           << sep
           << "dims: "
           << beginArray;
    uint16_t index = 0;
    while (index < numDimensions)
    {
        stream << item.getDimension(index);

        ++index;
        if (index < numDimensions)
        {
            stream << sep;
        }
    }
    stream << endArray
           << sep
           << "coords: "
           << beginArray;
    if (getDetail(stream) > DETAIL_SKIP)
    {
        const uint32_t commSize = item.getCommunicator().getSize();
        uint32_t       rank     = 0;
        while (rank < commSize)
        {
            const CartCoordList coordList = item.getRankCoordinates(rank);

            CartCoordList::const_iterator it = coordList.begin();
            while (it != coordList.end())
            {
                stream << rank << " -> (";
                CartCoordinate::const_iterator cit = it->begin();
                while (cit != it->end())
                {
                    stream << *cit;
                    ++cit;
                    if (cit != it->end())
                    {
                        stream << ", ";
                    }
                }
                stream << ")";

                ++it;
                if (it != coordList.end())
                {
                    stream << sep;
                }
            }

            ++rank;
            if (rank < commSize)
            {
                stream << sep;
            }
        }
    }
    stream << endArray
           << endObject;

    return stream;
}
