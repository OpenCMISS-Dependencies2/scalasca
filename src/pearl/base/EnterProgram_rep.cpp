/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2018-2019                                                **
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
 *  @brief   Implementation of the class EnterProgram_rep.
 *
 *  This file provides the implementation of the class EnterProgram_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/EnterProgram_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/Utils.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

EnterProgram_rep::EnterProgram_rep(const timestamp_t              timestamp,
                                   const Region&                  region,
                                   uint64_t* const                metrics,
                                   const String&                  name,
                                   const vector< const String* >& arguments)
    : Enter_rep(timestamp, region, metrics),
      mName(name),
      mArguments(arguments)
{
}


EnterProgram_rep::EnterProgram_rep(const GlobalDefs& defs,
                                   Buffer&           buffer)
    : Enter_rep(defs, buffer),
      mName(defs.getString(buffer.get_uint32()))
{
    const uint32_t numArguments = buffer.get_uint32();
    mArguments.reserve(numArguments);
    for (uint32_t i = 0; i < numArguments; ++i)
    {
        const String& arg = defs.getString(buffer.get_uint32());

        mArguments.push_back(&arg);
    }
}


// --- Event type information -----------------------------------------------

event_t
EnterProgram_rep::getType() const
{
    return ENTER_PROGRAM;
}


bool
EnterProgram_rep::isOfType(event_t type) const
{
    return (  (type == ENTER_PROGRAM)
           || (type == GROUP_ENTER)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

const String&
EnterProgram_rep::getName() const
{
    return mName;
}


uint32_t
EnterProgram_rep::numArguments() const
{
    return mArguments.size();
}


const String&
EnterProgram_rep::getArgument(const uint32_t index) const
{
    if (index >= mArguments.size())
    {
        throw RuntimeError("pearl::EnterProgram_rep::"
                           "getArgument(uint32_t) -- "
                           "index out of range: " + toStdString(index));
    }

    return *mArguments[index];
}


// --- Serialize event data (protected) -------------------------------------

void
EnterProgram_rep::pack(Buffer& buffer) const
{
    Enter_rep::pack(buffer);

    buffer.put_uint32(mName.getId());

    const uint32_t numArguments = mArguments.size();
    buffer.put_uint32(numArguments);
    for (uint32_t i = 0; i < numArguments; ++i)
    {
        buffer.put_uint32(mArguments[i]->getId());
    }
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
EnterProgram_rep::output(ostream& stream) const
{
    Enter_rep::output(stream);

    // Print data
    const size_t numArguments = mArguments.size();
    stream << sep
           << "name: " << mName
           << sep
           << "argc: " << numArguments
           << sep
           << "argv: "
           << beginArray;
    size_t index = 0;
    while (index < numArguments)
    {
        stream << *mArguments[index];

        ++index;
        if (index < numArguments)
        {
            stream << sep;
        }
    }
    stream << endArray;

    return stream;
}
