/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2014                                                     **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include <pearl/MpiAmRequest.h>

#include <cassert>

#include <pearl/AmHandler.h>
#include <pearl/MpiComm.h>
#include <pearl/MpiMessage.h>

using namespace std;
using namespace pearl;


/// Factory method.
///
/// @param [in] remote rank of communiction partner in @a comm
/// @param [in] comm %MpiComm object to use for communication
/// @param [in] ensureSynchronous Ensure synchronous delivery of request
///
AmRequest::Ptr
MpiAmRequest::create(uint32_t            remote,
                     const Communicator& comm,
                     bool                ensureSynchronous)
{
    const MpiComm* mpicomm = dynamic_cast< const MpiComm* >(&comm);
    assert(mpicomm);

    return AmRequest::Ptr(new MpiAmRequest(remote, *mpicomm, ensureSynchronous));
}


/// Creates an instance of an MPI enabled active message request.
///
/// @param [in] remote Id of remote process
/// @param [in] comm MPI communicator to use for communication
/// @param [in] ensureSynchronous Ensure synchronous delivery of request
///
MpiAmRequest::MpiAmRequest(uint32_t       remote,
                           const MpiComm& comm,
                           bool           ensureSynchronous)
    : AmRequest(new MpiMessage(comm), remote, comm, ensureSynchronous)
{
}


/// Destructor. Frees potentially held ressources.
///
MpiAmRequest::~MpiAmRequest()
{
    // we know our buffer is an MpiMessage as we created it
    MpiMessage* message = static_cast< MpiMessage* >(getBuffer());

    // if the request is still active, cancel it to avoid memory corruption
    if (!message->test())
    {
        message->cancel();
    }
}


/// Start active message request.
///
void
MpiAmRequest::start()
{
    // we know our buffer is an MpiMessage as we created it
    MpiMessage* message = static_cast< MpiMessage* >(getBuffer());

    /// @FIXME hard-coded message tag in MpiAmRequest
    if (ensureSynchronous())
    {
        message->issend(getRemote(), 170275);
    }
    else
    {
        message->isend(getRemote(), 170275);
    }
}


/// Test for completion.
///
/// @return true if the request is completed, false otherwise.
///
bool
MpiAmRequest::test()
{
    // we know our buffer is an MpiMessage as we created it
    MpiMessage* message = static_cast< MpiMessage* >(getBuffer());

    return message->test();
}


/// Cancel request.
///
void
MpiAmRequest::cancel()
{
    // we know our buffer is an MpiMessage as we created it
    MpiMessage* message = static_cast< MpiMessage* >(getBuffer());

    message->cancel();
}
