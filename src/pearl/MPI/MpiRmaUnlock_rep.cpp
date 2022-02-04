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
 *  @brief   Implementation of the class MpiRmaUnlock_rep.
 *
 *  This file provides the implementation of the class MpiRmaUnlock_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiRmaUnlock_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiWindow.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MpiRmaUnlock_rep::MpiRmaUnlock_rep(timestamp_t timestamp,
                                   uint32_t    location,
                                   MpiWindow*  window)
    : Event_rep(timestamp),
      m_window(window),
      m_location(location)
{
}


MpiRmaUnlock_rep::MpiRmaUnlock_rep(const GlobalDefs& defs,
                                   Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    m_window   = dynamic_cast< MpiWindow* >(defs.get_window(buffer.get_uint32()));
    m_location = buffer.get_uint32();
}


// --- Event type information -----------------------------------------------

event_t
MpiRmaUnlock_rep::getType() const
{
    return MPI_RMA_UNLOCK;
}


bool
MpiRmaUnlock_rep::isOfType(event_t type) const
{
    return (  (type == MPI_RMA_UNLOCK)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

MpiWindow*
MpiRmaUnlock_rep::get_window() const
{
    return m_window;
}


uint32_t
MpiRmaUnlock_rep::get_remote() const
{
    return m_location;
}


// --- Serialize event data (protected) -------------------------------------

void
MpiRmaUnlock_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(m_window->getId());
    buffer.put_uint32(m_location);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiRmaUnlock_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "location: " << m_location
           << sep
           << "window: " << *m_window;

    return stream;
}
