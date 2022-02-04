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
 *  @brief   Implementation of the class Communicator.
 *
 *  This file provides the implementation of the class Communicator and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/Communicator.h>

#include <cstddef>
#include <iostream>

#include <pearl/CommSet.h>
#include <pearl/String.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

Communicator::Communicator(const IdType              id,
                           const String&             name,
                           const CommSet&            commSet,
                           const Communicator* const parent)
    : mIdentifier(id),
      mName(name),
      mCommSet(commSet),
      mParent(parent)
{
}


Communicator::~Communicator()
{
}


// --- Query functions ------------------------------------------------------

Communicator::IdType
Communicator::getId() const
{
    return mIdentifier;
}


const String&
Communicator::getName() const
{
    return mName;
}


const CommSet&
Communicator::getCommSet() const
{
    return mCommSet;
}


const Communicator*
Communicator::getParent() const
{
    return mParent;
}


uint32_t
Communicator::getSize() const
{
    return mCommSet.numRanks();
}


Paradigm
Communicator::getParadigm() const
{
    return mCommSet.getParadigm();
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&            stream,
                  const Communicator& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: undefined communicator
    if (item == Communicator::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << item.getId();
    }

    // Print data
    const Communicator* const parent = item.getParent();
    stream << "Communicator"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "name: " << item.getName()
           << sep
           << "comm set: " << item.getCommSet()
           << sep
           << "parent: " << (parent ? *parent : Communicator::UNDEFINED)
           << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const Communicator& lhs,
                  const Communicator& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getName() == rhs.getName())
           && (lhs.getCommSet() == rhs.getCommSet())
           && (lhs.getParent() == rhs.getParent()));
}


bool
pearl::operator!=(const Communicator& lhs,
                  const Communicator& rhs)
{
    return !(lhs == rhs);
}
