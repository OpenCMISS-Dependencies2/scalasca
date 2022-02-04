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
 *  @brief   Implementation of PEARL-specific stream I/O functionality.
 *
 *  This header file provides the implementation of PEARL-specific (both
 *  public and internal) stream I/O manipulators, format flag handlers, and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/iomanip.h>

#include "iomanip_detail.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Local constants/variables --------------------------------------------

namespace
{
/// Number of spaces per indentation level
const long INDENT_WIDTH = 4;

/// Format flag index of the current indentation level
static const int indentIndex = ios_base::xalloc();

/// Format flag index of the current level of detail for PEARL objects
static const int detailIndex = ios_base::xalloc();
}    // unnamed namespace


// --- Stream I/O functions -------------------------------------------------

ostream&
pearl::operator<<(ostream&            stream,
                  const IomSetDetail& item)
{
    assert(item.mLevel >= DETAIL_SKIP);
    stream.iword(detailIndex) = item.mLevel;

    return stream;
}


ostream&
pearl::detail::operator<<(ostream&            stream,
                          const IomSetIndent& item)
{
    assert(item.mLevel >= 0);
    stream.iword(indentIndex) = item.mLevel;

    return stream;
}


// --- I/O manipulators -----------------------------------------------------

ostream&
pearl::detail::beginObject(ostream& stream)
{
    const long detail = getDetail(stream);
    assert(detail >= DETAIL_COMPACT);

    if (detail == DETAIL_COMPACT)
    {
        stream << " { ";
    }
    else
    {
        const long newIndent = getIndent(stream) + 1;
        stream << " {\n"
               << string(newIndent * INDENT_WIDTH, ' ')
               << setIndent(newIndent);
    }

    return stream << setDetail(detail - 1);
}


ostream&
pearl::detail::endObject(ostream& stream)
{
    const long detail = getDetail(stream);
    assert(detail >= DETAIL_ABBRV);

    if (detail == DETAIL_ABBRV)
    {
        stream << " }";
    }
    else
    {
        const long newIndent = getIndent(stream) - 1;
        stream << '\n'
               << string(newIndent * INDENT_WIDTH, ' ')
               << '}'
               << setIndent(newIndent);
    }

    return stream << setDetail(detail + 1);
}


ostream&
pearl::detail::beginArray(ostream& stream)
{
    const long detail = getDetail(stream);
    assert(detail >= DETAIL_ABBRV);

    switch (detail)
    {
        case DETAIL_ABBRV:
            stream << "[...";
            break;

        case DETAIL_COMPACT:
            stream << "[ ";
            break;

        default:
            {
                const long newIndent = getIndent(stream) + 1;
                stream << "[\n"
                       << string(newIndent * INDENT_WIDTH, ' ')
                       << setIndent(newIndent);
            }
            break;
    }

    return stream << setDetail(detail - 1);
}


ostream&
pearl::detail::endArray(ostream& stream)
{
    const long detail = getDetail(stream);
    assert(detail >= DETAIL_SKIP);

    switch (detail)
    {
        case DETAIL_SKIP:
            stream << "]";
            break;

        case DETAIL_ABBRV:
            stream << " ]";
            break;

        default:
            {
                const long newIndent = getIndent(stream) - 1;
                stream << '\n'
                       << string(newIndent * INDENT_WIDTH, ' ')
                       << ']'
                       << setIndent(newIndent);
            }
            break;
    }

    return stream << setDetail(detail + 1);
}


ostream&
pearl::detail::sep(ostream& stream)
{
    const long detail = getDetail(stream);
    assert(detail >= DETAIL_SKIP);

    switch (detail)
    {
        case DETAIL_SKIP:
            break;

        case DETAIL_ABBRV:
            stream << ", ";
            break;

        default:
            {
                const long indent = getIndent(stream);
                stream << '\n'
                       << string(indent * INDENT_WIDTH, ' ');
            }
            break;
    }

    return stream;
}


// --- Query stream format flags --------------------------------------------

long
pearl::detail::getIndent(ostream& stream)
{
    return stream.iword(indentIndex);
}


long
pearl::detail::getDetail(ostream& stream)
{
    return stream.iword(detailIndex);
}
