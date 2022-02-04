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
 *  @brief   Implementation of the class MpiCollBegin_rep.
 *
 *  This file provides the implementation of the class MpiCollBegin_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiCollBegin_rep.h>

using namespace pearl;


// --- Constructors & destructor --------------------------------------------

MpiCollBegin_rep::MpiCollBegin_rep(timestamp_t timestamp)
    : Event_rep(timestamp)
{
}


MpiCollBegin_rep::MpiCollBegin_rep(const GlobalDefs& defs,
                                   Buffer&           buffer)
    : Event_rep(defs, buffer)
{
}


// --- Event type information -----------------------------------------------

event_t
MpiCollBegin_rep::getType() const
{
    return MPI_COLLECTIVE_BEGIN;
}


bool
MpiCollBegin_rep::isOfType(event_t type) const
{
    return (  (type == MPI_COLLECTIVE_BEGIN)
           || (type == GROUP_BEGIN)
           || (type == GROUP_ALL));
}
