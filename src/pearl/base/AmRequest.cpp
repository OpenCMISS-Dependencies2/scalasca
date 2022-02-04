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

#include <pearl/AmRequest.h>

#include <cassert>
#include <iostream>

#include <pearl/Buffer.h>

using namespace std;
using namespace pearl;


/// Base class constructor initializing buffer pointer.
///
/// @param [in] buffer            Message buffer of request
/// @param [in] remote            Remote target process
/// @param [in] comm              Communicator used for active message
/// @param [in] ensureSynchronous Flag indicating whether the runtime must ensure synchronous send
///
AmRequest::AmRequest(Buffer*             buffer,
                     uint32_t            remote,
                     const Communicator& comm,
                     bool                ensureSynchronous)
    : mBuffer(buffer),
      mRemote(remote),
      mComm(comm),
      mEnsureSynchronous(ensureSynchronous)
{
}


/// Base class destructor cleaning up the message buffer.
///
AmRequest::~AmRequest()
{
    delete mBuffer;
}


/// Returns whether the underlying communication library
/// should enforce a synchronous send.
///
/// @return Flag whether synchronous communication should be enforced.
///
bool
AmRequest::ensureSynchronous() const
{
    return mEnsureSynchronous;
}


/// External function call to test for request completion.
/// This function is used for checking and clearing up request
/// in the runtime.
///
/// @param request Pointer to request object.
///
/// @return True if request is finished, false otherwise.
///
bool
pearl::isFinished(AmRequest* request)
{
    return request->test();
}


/// Returns a pointer to the internal buffer.
/// This call is used to obtain a pointer to the internal buffer
/// needed for a handler to pack the message.
///
/// @return Pointer to internal buffer object.
///
Buffer*
AmRequest::getBuffer()
{
    return mBuffer;
}


/// Returns a reference to the communicator used for this
/// request.
///
/// @returns Reference to communicator
///
const Communicator&
AmRequest::getComm() const
{
    return mComm;
}


/// Returns the process rank for the remote process in the
/// corresponding communicator.
///
/// @return Process rank.
///
uint32_t
AmRequest::getRemote() const
{
    return mRemote;
}
