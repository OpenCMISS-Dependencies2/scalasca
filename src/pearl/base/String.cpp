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
 *  @brief   Implementation of the class String.
 *
 *  This file provides the implementation of the class String and related
 *  functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/String.h>

#include <iostream>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

String::String(const IdType  id,
               const string& str)
    : mIdentifier(id),
      mString(str)
{
}


// --- Access definition data -----------------------------------------------

String::IdType
String::getId() const
{
    return mIdentifier;
}


const string
String::getString() const
{
    return mString;
}


const char*
String::getCString() const
{
    return mString.c_str();
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&      stream,
                  const String& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: undefined string
    if (item == String::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << '\"' << item.getString() << '\"';
    }

    // Print data
    stream << "String"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "text: \"" << item.getString() << '\"'
           << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const String& lhs,
                  const String& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getString() == rhs.getString()));
}


bool
pearl::operator!=(const String& lhs,
                  const String& rhs)
{
    return !(lhs == rhs);
}
