/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2019                                                **
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
 *  @brief   Implementation of the class CallingContext.
 *
 *  This file provides the implementation of the class CallingContext and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/CallingContext.h>

#include <iostream>

#include <pearl/Region.h>
#include <pearl/SourceLocation.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

CallingContext::CallingContext(const IdType          id,
                               const Region&         region,
                               const SourceLocation& scl,
                               CallingContext* const parent)
    : mRegion(region),
      mSourceLocation(scl),
      mParent(parent),
      mIdentifier(id)
{
    if (parent)
    {
        parent->mChildren.push_back(this);
    }
}


// --- Access definition data -----------------------------------------------

CallingContext::IdType
CallingContext::getId() const
{
    return mIdentifier;
}


const Region&
CallingContext::getRegion() const
{
    return mRegion;
}


const SourceLocation&
CallingContext::getSourceLocation() const
{
    return mSourceLocation;
}


CallingContext*
CallingContext::getParent() const
{
    return mParent;
}


uint32_t
CallingContext::numChildren() const
{
    return mChildren.size();
}


const CallingContext&
CallingContext::getChild(const uint32_t index) const
{
    return *(mChildren.at(index));
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&              stream,
                  const CallingContext& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: undefined calling context
    if (item == CallingContext::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << item.getId();
    }

    // Print data
    const CallingContext* const parent = item.getParent();
    stream << "CallingContext"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "region: " << item.getRegion()
           << sep
           << "scl: " << item.getSourceLocation()
           << sep
           << "parent: " << (parent ? *parent : CallingContext::UNDEFINED)
           << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const CallingContext& lhs,
                  const CallingContext& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getRegion() == rhs.getRegion())
           && (lhs.getSourceLocation() == rhs.getSourceLocation())
           && (lhs.getParent() == rhs.getParent()));
}


bool
pearl::operator!=(const CallingContext& lhs,
                  const CallingContext& rhs)
{
    return !(lhs == rhs);
}
