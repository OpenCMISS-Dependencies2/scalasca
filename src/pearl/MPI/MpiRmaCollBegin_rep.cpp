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
 *  @brief   Implementation of the class MpiRmaCollBegin_rep.
 *
 *  This file provides the implementation of the class MpiRmaCollBegin_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiRmaCollBegin_rep.h>

using namespace pearl;


// --- Constructors & destructor --------------------------------------------

MpiRmaCollBegin_rep::MpiRmaCollBegin_rep(timestamp_t timestamp)
    : Event_rep(timestamp)
{
}


MpiRmaCollBegin_rep::MpiRmaCollBegin_rep(const GlobalDefs& defs,
                                         Buffer&           buffer)
    : Event_rep(defs, buffer)
{
}


// --- Event type information -----------------------------------------------

event_t
MpiRmaCollBegin_rep::getType() const
{
    return MPI_RMA_COLLECTIVE_BEGIN;
}


bool
MpiRmaCollBegin_rep::isOfType(event_t type) const
{
    return (  (type == MPI_RMA_COLLECTIVE_BEGIN)
           || (type == GROUP_BEGIN)
           || (type == GROUP_ALL));
}
