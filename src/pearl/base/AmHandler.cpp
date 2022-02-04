/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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


#include <config.h>

#include <pearl/AmHandler.h>

#include <cassert>
#include <limits>

#include <pearl/AmRuntime.h>
#include <pearl/Buffer.h>

using namespace std;
using namespace pearl;


/// Implementation of private constructor. It should only be invoked by the
/// runtime system through AmRuntime.
///
AmHandler::AmHandler()
    : mId(UNREGISTERED)
{
}


/// Implementation of private virtual destructor. It is needed to get the
/// compiler to create a vtable for the AmHandler-part of derived objects.
///
AmHandler::~AmHandler()
{
}


/// Returns the id this handler is registered with at the %AmRuntime.
///
/// @returns Handler Id
///
uint32_t
AmHandler::getId()
{
    return mId;
}


/// Sets the handler id of a certain handler class
///
/// @param id Id for handler class
///
void
AmHandler::setId(uint32_t id)
{
    mId = id;
}


/// Initial value for handler objects until they are registered with at an
/// %AmRuntime object.
///
const uint32_t AmHandler::UNREGISTERED = numeric_limits< uint32_t >::max();
