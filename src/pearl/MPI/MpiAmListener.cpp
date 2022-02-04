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

#include <pearl/MpiAmListener.h>

#include <cassert>

#include <mpi.h>

#include <pearl/Buffer.h>
#include <pearl/MpiComm.h>
#include <pearl/MpiMessage.h>
#include <pearl/MpiWindow.h>

using namespace std;
using namespace pearl;


/// Creates a new instance of an active message listener on a given communicator.
///
/// @param comm Communicator to listen on for requests
///
MpiAmListener::MpiAmListener(const MpiComm& comm)
    : AmListener(new MpiMessage(comm))
{
}


/// Destructor. Destroys the instance.
///
MpiAmListener::~MpiAmListener()
{
}


/// Factory method.
///
/// @param [in] comm Communicator to listen for active messages
///
/// @return Pointer to object.
///
AmListener::Ptr
MpiAmListener::create(const Communicator& comm)
{
    const MpiComm* mpicomm = dynamic_cast< const MpiComm* >(&comm);
    assert(mpicomm);

    return AmListener::Ptr(new MpiAmListener(*mpicomm));
}


/// Probing on the provided communicator for incomming messages.
///
bool
MpiAmListener::probe()
{
    MpiMessage& message = static_cast< MpiMessage& >(getBuffer());

    /// FIXME hard-coded message tag in Listener
    return message.probe(MPI_ANY_SOURCE, 170275);
}
