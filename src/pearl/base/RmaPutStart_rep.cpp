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
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class RmaPutStart_rep.
 *
 *  This file provides the implementation of the class RmaPutStart_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/RmaPutStart_rep.h>

#include <iostream>

#include <pearl/Buffer.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

RmaPutStart_rep::RmaPutStart_rep(timestamp_t timestamp,
                                 uint32_t    rma_id,
                                 uint32_t    remote,
                                 uint64_t    bytesSent)
    : Event_rep(timestamp),
      m_remote(remote),
      mBytesSent(bytesSent),
      m_rma_id(rma_id)
{
}


RmaPutStart_rep::RmaPutStart_rep(const GlobalDefs& defs,
                                 Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    m_remote   = buffer.get_uint32();
    mBytesSent = buffer.get_uint64();
    m_rma_id   = buffer.get_uint32();
}


// --- Event type information -----------------------------------------------

event_t
RmaPutStart_rep::getType() const
{
    return RMA_PUT_START;
}


bool
RmaPutStart_rep::isOfType(event_t type) const
{
    return (  (type == RMA_PUT_START)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint32_t
RmaPutStart_rep::get_remote() const
{
    return m_remote;
}


uint64_t
RmaPutStart_rep::getBytesSent() const
{
    return mBytesSent;
}


uint32_t
RmaPutStart_rep::get_rma_id() const
{
    return m_rma_id;
}


// --- Serialize event data (protected) -------------------------------------

void
RmaPutStart_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(m_remote);
    buffer.put_uint64(mBytesSent);
    buffer.put_uint32(m_rma_id);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
RmaPutStart_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "remote: " << m_remote
           << sep
           << "sent: " << mBytesSent
           << sep
           << "RMA id: " << m_rma_id;

    return stream;
}
