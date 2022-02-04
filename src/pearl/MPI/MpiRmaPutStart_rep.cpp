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
 *  @brief   Implementation of the class MpiRmaPutStart_rep.
 *
 *  This file provides the implementation of the class MpiRmaPutStart_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiRmaPutStart_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiWindow.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MpiRmaPutStart_rep::MpiRmaPutStart_rep(timestamp_t timestamp,
                                       uint32_t    rma_id,
                                       uint32_t    remote,
                                       uint64_t    bytesSent,
                                       MpiWindow*  window)
    : RmaPutStart_rep(timestamp, rma_id, remote, bytesSent),
      m_window(window)
{
}


MpiRmaPutStart_rep::MpiRmaPutStart_rep(const GlobalDefs& defs,
                                       Buffer&           buffer)
    : RmaPutStart_rep(defs, buffer)
{
    m_window = dynamic_cast< MpiWindow* >(defs.get_window(buffer.get_uint32()));
}


// --- Event type information -----------------------------------------------

event_t
MpiRmaPutStart_rep::getType() const
{
    return MPI_RMA_PUT_START;
}


bool
MpiRmaPutStart_rep::isOfType(event_t type) const
{
    return (  (type == MPI_RMA_PUT_START)
           || (type == RMA_PUT_START)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

MpiWindow*
MpiRmaPutStart_rep::get_window() const
{
    return m_window;
}


// --- Serialize event data (protected) -------------------------------------

void
MpiRmaPutStart_rep::pack(Buffer& buffer) const
{
    RmaPutStart_rep::pack(buffer);

    buffer.put_uint32(m_window->getId());
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiRmaPutStart_rep::output(ostream& stream) const
{
    RmaPutStart_rep::output(stream);

    // Print data
    stream << sep
           << "window: " << *m_window;

    return stream;
}
