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
 *  @brief   Implementation of the class MutableCartTopology.
 *
 *  This file provides the implementation of the class MutableCartTopology.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "MutableCartTopology.h"

#include <pearl/CartDimension.h>
#include <pearl/Communicator.h>
#include <pearl/Error.h>
#include <pearl/Utils.h>

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MutableCartTopology::MutableCartTopology(const IdType              id,
                                         const String&             name,
                                         const Communicator&       communicator,
                                         const DimensionContainer& dimensions)
    : CartTopology(id, name, communicator, dimensions)
{
}


// --- Set definition data --------------------------------------------------

void
MutableCartTopology::setCommunicator(const Communicator& communicator)
{
    mCommunicator = &communicator;
}


void
MutableCartTopology::addRankCoordinate(const uint32_t        rank,
                                       const CartCoordinate& coordinate)
{
    if (  (*mCommunicator != Communicator::UNDEFINED)
       && (rank >= mCommunicator->getSize()))
    {
        throw RuntimeError("pearl::detail::MutableCartTopology::"
                           "addRankCoordinate(uint32_t, CartCoordinate&) -- "
                           "invalid rank: " + toStdString(rank));
    }

    const uint8_t numDimensions = this->numDimensions();
    if (coordinate.size() != numDimensions)
    {
        throw RuntimeError("pearl::detail::MutableCartTopology::"
                           "addRankCoordinate(uint32_t, CartCoordinate&) -- "
                           "dimension mismatch: "
                           + toStdString(coordinate.size())
                           + " != "
                           + toStdString(numDimensions));
    }

    for (uint8_t dimIndex = 0; dimIndex < numDimensions; ++dimIndex)
    {
        const CartDimension& dimension = getDimension(dimIndex);
        if (coordinate[dimIndex] >= dimension.getSize())
        {
            throw RuntimeError("pearl::detail::MutableCartTopology::"
                               "addRankCoordinate(uint32_t, CartCoordinate&) -- "
                               "coordinate "
                               + toStdString(dimIndex)
                               + "out of range: "
                               + toStdString(coordinate[dimIndex]));
        }
    }

    // See S. Meyers, "Effective STL", item 24
    CoordinateContainer::iterator it = mCoordinates.lower_bound(rank);
    if (  (it != mCoordinates.end())
       && !(mCoordinates.key_comp()(rank, it->first)))
    {
        it->second.push_back(coordinate);
    }
    else
    {
        typedef CoordinateContainer::value_type valueType;

        mCoordinates.insert(it, valueType(rank, CartCoordList(1, coordinate)));
    }
}
