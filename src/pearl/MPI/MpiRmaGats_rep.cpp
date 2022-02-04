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
 *  @brief   Implementation of the class MpiRmaGats_rep.
 *
 *  This file provides the implementation of the class MpiRmaGats_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiRmaGats_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiGroup.h>
#include <pearl/MpiWindow.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MpiRmaGats_rep::MpiRmaGats_rep(timestamp_t timestamp,
                               MpiWindow*  window,
                               MpiGroup*   group,
                               bool        sync)
    : Event_rep(timestamp),
      m_window(window),
      m_group(group),
      m_sync(sync)
{
}


MpiRmaGats_rep::MpiRmaGats_rep(const GlobalDefs& defs,
                               Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    m_window = dynamic_cast< MpiWindow* >(defs.get_window(buffer.get_uint32()));
    m_group  = dynamic_cast< MpiGroup* >(defs.get_group(buffer.get_uint32()));
    m_sync   = (bool)buffer.get_uint8();
}


// --- Event type information -----------------------------------------------

event_t
MpiRmaGats_rep::getType() const
{
    return MPI_RMA_GATS;
}


bool
MpiRmaGats_rep::isOfType(event_t type) const
{
    return (  (type == MPI_RMA_GATS)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

MpiWindow*
MpiRmaGats_rep::get_window() const
{
    return m_window;
}


MpiGroup*
MpiRmaGats_rep::get_group() const
{
    return m_group;
}


bool
MpiRmaGats_rep::is_sync() const
{
    return m_sync;
}


// --- Serialize event data (protected) -------------------------------------

void
MpiRmaGats_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(m_window->getId());
    buffer.put_uint32(m_group->getId());
    buffer.put_uint8((uint8_t)(m_sync ? 1 : 0));
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiRmaGats_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    const ios_base::fmtflags fmtflags = stream.setf(ios_base::boolalpha);
    stream << sep
           << "window: " << *m_window
           << sep
           << "group: " << *m_group
           << sep
           << "sync: " << m_sync;
    stream.flags(fmtflags);

    return stream;
}
