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

#include <pearl/AmListenerFactory.h>

#include <pearl/AmListener.h>
#include <pearl/Communicator.h>

using namespace std;
using namespace pearl;


/// Creates instances of AmListener::Ptr that point to a correct object
/// of a listener type corresponding to the dynamic type of @a comm.
///
/// @param [in] comm Communicator object.
///
/// @returns Listener object corresponding to @a comm
///
AmListener::Ptr
AmListenerFactory::create(const Communicator& comm)
{
    switch (native_value(comm.getParadigm()))
    {
        case Paradigm::MPI:
            return (*getCallbacks()[comm.getParadigm()])(comm);

        default:
            return AmListener::Ptr(NULL);
    }
}


/// Gets the static container of factory method callbacks
///
/// @return Container of factory method callbacks.
///
AmListenerFactory::CbContainer&
AmListenerFactory::getCallbacks()
{
    static AmListenerFactory::CbContainer mCallbacks;

    return mCallbacks;
}


/// Registers a factory method callback for a given communication paradigm
///
/// @param [in] paradigm Communication paradigm
/// @param [in] callback Factory method callback
///
void
AmListenerFactory::registerCallback(Paradigm                      paradigm,
                                    AmListenerFactory::CbFunction callback)
{
    getCallbacks()[paradigm] = callback;
}
