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
 *  @brief   Implementation of the class RmaGetStart_rep.
 *
 *  This file provides the implementation of the class RmaGetStart_rep.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/RmaGetStart_rep.h>

#include <iostream>

#include <pearl/Buffer.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

RmaGetStart_rep::RmaGetStart_rep(timestamp_t timestamp,
                                 uint32_t    rma_id,
                                 uint32_t    remote,
                                 uint64_t    bytesReceived)
    : Event_rep(timestamp),
      m_remote(remote),
      mBytesReceived(bytesReceived),
      m_rma_id(rma_id)
{
}


RmaGetStart_rep::RmaGetStart_rep(const GlobalDefs& defs,
                                 Buffer&           buffer)
    : Event_rep(defs, buffer)
{
    m_remote       = buffer.get_uint32();
    mBytesReceived = buffer.get_uint64();
    m_rma_id       = buffer.get_uint32();
}


// --- Event type information -----------------------------------------------

event_t
RmaGetStart_rep::getType() const
{
    return RMA_GET_START;
}


bool
RmaGetStart_rep::isOfType(event_t type) const
{
    return (  (type == RMA_GET_START)
           || (type == GROUP_ALL));
}


// --- Access event data ----------------------------------------------------

uint32_t
RmaGetStart_rep::get_remote() const
{
    return m_remote;
}


uint64_t
RmaGetStart_rep::getBytesReceived() const
{
    return mBytesReceived;
}


uint32_t
RmaGetStart_rep::get_rma_id() const
{
    return m_rma_id;
}


// --- Serialize event data (protected) -------------------------------------

void
RmaGetStart_rep::pack(Buffer& buffer) const
{
    Event_rep::pack(buffer);

    buffer.put_uint32(m_remote);
    buffer.put_uint64(mBytesReceived);
    buffer.put_uint32(m_rma_id);
}


// --- Generate human-readable output of event data (protected) -------------

ostream&
RmaGetStart_rep::output(ostream& stream) const
{
    Event_rep::output(stream);

    // Print data
    stream << sep
           << "remote: " << m_remote
           << sep
           << "rcvd: " << mBytesReceived
           << sep
           << "RMA id: " << m_rma_id;

    return stream;
}
