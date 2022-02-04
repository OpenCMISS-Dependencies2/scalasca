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
 *  @ingroup PEARL_mpi
 *  @brief   Implementation of the class MpiRmaPutEnd_rep.
 *
 *  This file provides the implementation of the class MpiRmaPutEnd_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiRmaPutEnd_rep.h>

using namespace pearl;


// --- Constructors & destructor --------------------------------------------

MpiRmaPutEnd_rep::MpiRmaPutEnd_rep(timestamp_t timestamp,
                                   uint32_t    rma_id)
    : RmaPutEnd_rep(timestamp, rma_id)
{
}


MpiRmaPutEnd_rep::MpiRmaPutEnd_rep(const GlobalDefs& defs,
                                   Buffer&           buffer)
    : RmaPutEnd_rep(defs, buffer)
{
}


// --- Event type information -----------------------------------------------

event_t
MpiRmaPutEnd_rep::getType() const
{
    return MPI_RMA_PUT_END;
}


bool
MpiRmaPutEnd_rep::isOfType(event_t type) const
{
    return (  (type == MPI_RMA_PUT_END)
           || (type == RMA_PUT_END)
           || (type == GROUP_ALL));
}
