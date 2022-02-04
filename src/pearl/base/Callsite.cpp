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
 *  @brief   Implementation of the class Callsite.
 *
 *  This file provides the implementation of the class Callsite and related
 *  functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/Callsite.h>

#include <iostream>

#include <pearl/Region.h>
#include <pearl/String.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

Callsite::Callsite(const IdType   id,
                   const String&  filename,
                   const uint32_t line,
                   const Region&  callee)
    : mIdentifier(id),
      mFilename(filename),
      mLine(line),
      mCallee(callee)
{
}


// --- Access definition data -----------------------------------------------

Callsite::IdType
Callsite::getId() const
{
    return mIdentifier;
}


const String&
Callsite::getFilename() const
{
    return mFilename;
}


uint32_t
Callsite::getLine() const
{
    return mLine;
}


const Region&
Callsite::getCallee() const
{
    return mCallee;
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&        stream,
                  const Callsite& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: undefined callsite
    if (item == Callsite::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << item.getId();
    }

    // Print data
    stream << "Callsite"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "file: " << item.getFilename()
           << sep
           << "line: " << item.getLine()
           << sep
           << "callee: " << item.getCallee()
           << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const Callsite& lhs,
                  const Callsite& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getFilename() == rhs.getFilename())
           && (lhs.getLine() == rhs.getLine())
           && (lhs.getCallee() == rhs.getCallee()));
}


bool
pearl::operator!=(const Callsite& lhs,
                  const Callsite& rhs)
{
    return !(lhs == rhs);
}
