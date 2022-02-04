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
 *  @brief   Implementation of the class CartDimension.
 *
 *  This file provides the implementation of the class CartDimension and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/CartDimension.h>

#include <iostream>

#include <pearl/String.h>
#include <pearl/iomanip.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

CartDimension::CartDimension(const IdType      id,
                             const String&     name,
                             const uint32_t    size,
                             const Periodicity periodicity)
    : mName(name),
      mIdentifier(id),
      mSize(size),
      mPeriodicity(periodicity)
{
}


// --- Access definition data -----------------------------------------------


CartDimension::IdType
CartDimension::getId() const
{
    return mIdentifier;
}


const String&
CartDimension::getName() const
{
    return mName;
}


uint32_t
CartDimension::getSize() const
{
    return mSize;
}


bool
CartDimension::isPeriodic() const
{
    return (mPeriodicity == CartDimension::PERIODIC);
}


// --- Stream I/O functions -------------------------------------------------

ostream&
pearl::operator<<(ostream&             stream,
                  const CartDimension& item)
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
    const ios_base::fmtflags fmtflags = stream.setf(ios_base::boolalpha);
    stream << "CartDimension"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "name: " << item.getName()
           << sep
           << "size: " << item.getSize()
           << sep
           << "periodic: " << item.isPeriodic()
           << endObject;
    stream.flags(fmtflags);

    return stream;
}
