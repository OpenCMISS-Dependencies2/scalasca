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
 *  @brief   Implementation of the class MpiRmaLock_rep.
 *
 *  This file provides the implementation of the class MpiRmaLock_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiRmaLock_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiWindow.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MpiRmaLock_rep::MpiRmaLock_rep(timestamp_t timestamp,
                               uint32_t    location,
                               MpiWindow*  window,
                               bool        exclusive)
    : Event_rep(timestamp),
      m_window(window),
      m_location(location),
      m_exclusive(exclusive)
{
}


MpiRmaLock_rep::MpiRmaLock_rep(const GlobalDefs& defs,
                               Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    m_window    = dynamic_cast< MpiWindow* >(defs.get_window(buffer.get_uint32()));
    m_location  = buffer.get_uint32();
    m_exclusive = buffer.get_uint8();
}


// --- Event type information -----------------------------------------------

event_t
MpiRmaLock_rep::getType() const
{
    return MPI_RMA_LOCK;
}


bool
MpiRmaLock_rep::isOfType(event_t type) const
{
    return (  (type == MPI_RMA_LOCK)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

MpiWindow*
MpiRmaLock_rep::get_window() const
{
    return m_window;
}


uint32_t
MpiRmaLock_rep::get_remote() const
{
    return m_location;
}


bool
MpiRmaLock_rep::is_exclusive() const
{
    return m_exclusive;
}


// --- Serialize event data (protected) -------------------------------------

void
MpiRmaLock_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(m_window->getId());
    buffer.put_uint32(m_location);
    buffer.put_uint8(m_exclusive ? 1 : 0);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiRmaLock_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    const ios_base::fmtflags fmtflags = stream.setf(ios_base::boolalpha);
    stream << sep
           << "location: " << m_location
           << sep
           << "window: " << *m_window
           << sep
           << "exclusive: " << m_exclusive;

    stream.flags(fmtflags);

    return stream;
}
