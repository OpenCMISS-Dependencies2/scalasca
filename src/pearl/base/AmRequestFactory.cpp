/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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

#include <pearl/AmRequestFactory.h>

#include <cstdlib>
#include <iostream>

#include <pearl/AmHandler.h>
#include <pearl/AmRequest.h>
#include <pearl/Communicator.h>

using namespace std;
using namespace pearl;


/// Creates instances of %AmRequest::Ptr that point to
/// a correct object of a request type corresponding to
/// the dynamic type of @a comm.
///
/// @param remote            Id of target process in @a comm
/// @param comm              Comm object used for communication.
/// @param ensureSynchronous Flag to indicate whether the runtime system needs to ensure synchronous send
///
/// @return Pointer to created request object
AmRequest::Ptr
AmRequestFactory::create(uint32_t            remote,
                         const Communicator& comm,
                         bool                ensureSynchronous)
{
    switch (native_value(comm.getParadigm()))
    {
        case Paradigm::MPI:
            return (*getCallbacks()[comm.getParadigm()])(remote, comm, ensureSynchronous);

        default:
            return AmRequest::Ptr(NULL);
    }
}


/// Get static callback container
///
/// @return Reference to static callback container
///
AmRequestFactory::CbContainer&
AmRequestFactory::getCallbacks()
{
    static AmRequestFactory::CbContainer mCallbacks;

    return mCallbacks;
}


/// Register a request factory callback
///
/// @param [in] paradigm Paradigm the callback should be used for
/// @param [in] callback Factory callback for request creation
///
void
AmRequestFactory::registerCallback(Paradigm                     paradigm,
                                   AmRequestFactory::CbFunction callback)
{
    getCallbacks()[paradigm] = callback;
}
