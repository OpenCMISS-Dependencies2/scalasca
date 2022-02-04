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

#include <pearl/AmListener.h>

#include <cassert>

#include <pearl/Buffer.h>
#include <pearl/Communicator.h>

using namespace std;
using namespace pearl;


/// Initializes the internal Buffer pointer.
///
/// @param [in] buffer Pointer to a Buffer (or derived) object.
///
AmListener::AmListener(Buffer* buffer)
    : mBuffer(buffer)
{
}


/// Destructor. Destroys the instance and deletes the internal Buffer object
///
AmListener::~AmListener()
{
    delete mBuffer;
}


/// Returns a reference to the internal Buffer object
///
/// @return Reference to internal buffer
///
Buffer&
AmListener::getBuffer()
{
    assert(mBuffer);

    return *mBuffer;
}


/// Increases the number of references for this listener
///
void
AmListener::addReference()
{
    ++mReferences;
}


/// Descreses the number of references for this listener
///
void
AmListener::removeReference()
{
    --mReferences;
}


/// Return the number of references for this listener
///
/// @return Number of references for this listener
///
size_t
AmListener::numReferences() const
{
    return mReferences;
}
