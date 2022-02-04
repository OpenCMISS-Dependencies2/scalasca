/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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


#include <config.h>

#include "AmSeverityExchange.h"

#include <cassert>
#include <iostream>

#include <pearl/AmRequest.h>
#include <pearl/AmRuntime.h>
#include <pearl/Buffer.h>
#include <pearl/CallbackManager.h>
#include <pearl/LocalTrace.h>

#include "CbData.h"
#include "user_events.h"

using namespace std;
using namespace pearl;
using namespace scout;


/// Standard constructor
///
AmSeverityExchange::AmSeverityExchange()
{
}


/// Standard destructor
///
AmSeverityExchange::~AmSeverityExchange()
{
}


/// Returns a reference to handler instance
///
/// @return Handler instance
///
AmSeverityExchange&
AmSeverityExchange::getInstance()
{
    static AmSeverityExchange mInstance;

    return mInstance;
}


/// Pack handler specific information to provided buffer
///
/// @param [in] userEvent Scout user event to trigger on remote side
/// @param [in] eventId   Event index
/// @param [in] cnodeId   Callpath node id of the occurence of the pattern
/// @param [in] rank       MPI rank
/// @param [in] severity   Severity of this pattern instance
/// @param [in,out] buffer Buffer to pack information to
///
void
AmSeverityExchange::pack(uint32_t    userEvent,
                         ident_t     eventId,
                         ident_t     cnodeId,
                         uint32_t    rank,
                         timestamp_t severity,
                         Buffer&     buffer)
{
    buffer.put_id(getInstance().getId());
    buffer.put_uint32(userEvent);
    buffer.put_id(eventId);
    buffer.put_id(cnodeId);
    buffer.put_uint32(rank);
    buffer.putTimestamp(severity);
}


/// Pack handler specific information to buffer of provided request
///
/// @param [in] userEvent Scout user event to trigger on remote side
/// @param [in] eventId   Event index
/// @param [in] cnodeId   Callpath node id of the occurence of the pattern
/// @param [in] rank       MPI rank
/// @param [in] severity   Severity of this pattern instance
/// @param [in,out] request Active message request
///
void
AmSeverityExchange::pack(uint32_t    userEvent,
                         ident_t     eventId,
                         ident_t     cnodeId,
                         uint32_t    rank,
                         timestamp_t severity,
                         AmRequest*  request)
{
    Buffer* buffer = request->getBuffer();
    pack(userEvent, eventId, cnodeId, rank, severity, *buffer);
}


/// Unpacks severity information and triggers the callback system.
///
/// @param [in,out] buffer Buffer containing handler information
///
void
AmSeverityExchange::execute(Buffer& buffer)
{
    CbData*                cbdata    = static_cast< CbData* >(AmRuntime::getInstance().getCbdata());
    const CallbackManager& cbmanager = AmRuntime::getInstance().getCbmanager();

    // ensure callback data structure is present
    assert(cbdata);

    uint32_t userEvent = buffer.get_uint32();
    Event    event     = cbdata->mTrace->at(buffer.get_id());
    cbdata->mCallpathId = buffer.get_id();
    cbdata->mSyncRank   = buffer.get_uint32();
    cbdata->mIdle       = buffer.getTimestamp();

    cbmanager.notify(userEvent, event, cbdata);
}


/// Return handler name
///
/// @return Handler name
///
const string&
AmSeverityExchange::getName() const
{
    static const string& mName("scout::AmSeverityExchange");

    return mName;
}
