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
 *  @brief   Implementation of the class MpiRmaGetStart_rep.
 *
 *  This file provides the implementation of the class MpiRmaGetStart_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiRmaGetStart_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiWindow.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

MpiRmaGetStart_rep::MpiRmaGetStart_rep(timestamp_t timestamp,
                                       uint32_t    rma_id,
                                       uint32_t    remote,
                                       uint64_t    bytesReceived,
                                       MpiWindow*  window)
    : RmaGetStart_rep(timestamp, rma_id, remote, bytesReceived),
      m_window(window)
{
}


MpiRmaGetStart_rep::MpiRmaGetStart_rep(const GlobalDefs& defs,
                                       Buffer&           buffer)
    : RmaGetStart_rep(defs, buffer)
{
    m_window = dynamic_cast< MpiWindow* >(defs.get_window(buffer.get_uint32()));
}


// --- Event type information -----------------------------------------------

event_t
MpiRmaGetStart_rep::getType() const
{
    return MPI_RMA_GET_START;
}


bool
MpiRmaGetStart_rep::isOfType(event_t type) const
{
    return (  (type == MPI_RMA_GET_START)
           || (type == RMA_GET_START)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

MpiWindow*
MpiRmaGetStart_rep::get_window() const
{
    return m_window;
}


// --- Serialize event data (protected) -------------------------------------

void
MpiRmaGetStart_rep::pack(Buffer& buffer) const
{
    RmaGetStart_rep::pack(buffer);

    buffer.put_uint32(m_window->getId());
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
MpiRmaGetStart_rep::output(ostream& stream) const
{
    RmaGetStart_rep::output(stream);

    // Print data
    stream << sep
           << "window: " << *m_window;

    return stream;
}
