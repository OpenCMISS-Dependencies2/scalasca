/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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
 *  @ingroup PEARL_mpi
 *  @brief   Implementation of the class MpiComm.
 *
 *  This file provides the implementation of the class MpiComm.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiComm.h>

#include <pearl/MpiGroup.h>

using namespace std;
using namespace pearl;


// --- Constructors & destructor --------------------------------------------

MpiComm::MpiComm(const IdType         id,
                 const String&        name,
                 const MpiGroup&      group,
                 const MpiComm* const parent)
    : Communicator(id, name, group, parent),
      mHandle(MPI_COMM_NULL)
{
    MPI_Group groupHandle = group.createHandle();
    MPI_Comm_create(MPI_COMM_WORLD, groupHandle, &mHandle);
    MPI_Group_free(&groupHandle);
}


MpiComm::MpiComm(const MpiComm& comm)
    : Communicator(comm),
      mHandle(MPI_COMM_NULL)
{
    MPI_Comm_dup(comm.mHandle, &mHandle);
}


MpiComm::~MpiComm()
{
    if (mHandle != MPI_COMM_NULL)
    {
        MPI_Comm_free(&mHandle);
    }
}


// --- Query functions ------------------------------------------------------

MPI_Comm
MpiComm::getHandle() const
{
    return mHandle;
}


// --- Communicator duplication ---------------------------------------------

MpiComm*
MpiComm::duplicate() const
{
    return new MpiComm(*this);
}
