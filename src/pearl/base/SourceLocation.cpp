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
 *  @brief   Implementation of the class SourceLocation.
 *
 *  This file provides the implementation of the class SourceLocation and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/SourceLocation.h>

#include <iostream>

#include <pearl/String.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

SourceLocation::SourceLocation(const IdType   id,
                               const String&  filename,
                               const uint32_t line)
    : mFilename(filename),
      mIdentifier(id),
      mLine(line)
{
}


// --- Access definition data -----------------------------------------------

SourceLocation::IdType
SourceLocation::getId() const
{
    return mIdentifier;
}


const String&
SourceLocation::getFilename() const
{
    return mFilename;
}


uint32_t
SourceLocation::getLine() const
{
    return mLine;
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&              stream,
                  const SourceLocation& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: undefined source-code location
    if (item == SourceLocation::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << item.getId();
    }

    // Print data
    stream << "SourceLocation"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "file: " << item.getFilename()
           << sep
           << "line: " << item.getLine()
           << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const SourceLocation& lhs,
                  const SourceLocation& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getFilename() == rhs.getFilename())
           && (lhs.getLine() == rhs.getLine()));
}


bool
pearl::operator!=(const SourceLocation& lhs,
                  const SourceLocation& rhs)
{
    return !(lhs == rhs);
}
