/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2014                                                     **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "AmSyncpointExchange.h"

#include <inttypes.h>

#include <pearl/AmRuntime.h>
#include <pearl/Buffer.h>
#include <pearl/CallbackManager.h>
#include <pearl/Callpath.h>
#include <pearl/Event.h>
#include <pearl/LocalTrace.h>

#include "CbData.h"
#include "user_events.h"

#ifdef DEBUG_SPX
    #include <iostream>
#endif    // DEBUG_SPX

using namespace std;
using namespace scout;
using namespace pearl;


/**
 * Standard constructor.
 */
AmSyncpointExchange::AmSyncpointExchange()
{
}


/**
 * Standard destructor
 */
AmSyncpointExchange::~AmSyncpointExchange()
{
}


/**
 * Returns a pointer to singleton object.
 *
 * @return Reference to handler instance
 */
AmSyncpointExchange&
AmSyncpointExchange::getInstance()
{
    static AmSyncpointExchange mInstance;

    return mInstance;
}


/**
 * Returns the name of the handler.
 *
 * @return Name of the handler
 */
const string&
AmSyncpointExchange::getName() const
{
    static const string mName = "scout::AmSyncpointExchange";

    return mName;
}


/**
 * Packs all relevant information into the provided buffer.
 *
 * @param [in]     userEvent user event
 * @param [in]     commId id of communicator
 * @param [in]     info eventId, idleTime, syncRank
 * @param [in,out] buffer Buffer to pack the information
 */
void
AmSyncpointExchange::pack(uint32_t        userEvent,
                          uint32_t        commId,
                          ContentionInfo& info,
                          Buffer&         buffer)
{
    buffer.put_id(getInstance().getId());
    buffer.put_uint32(userEvent);
    buffer.put_uint32(commId);
    buffer.put_id(info.eventIndex);
    buffer.putTimestamp(info.waitTime);
    buffer.put_uint32(info.syncRank);

    #ifdef DEBUG_SPX
        cerr << "AmSyncpointExchange::pack:" << info.waittime << "  " << info.sync_rank << endl;
    #endif
}


/**
 * Packs all relevant information into the provided buffer.
 *
 * @param [in]     userEvent user event
 * @param [in]     commId id of communicator
 * @param [in]     info eventIndex, waittime, syncRank
 * @param [in,out] request Active message request
 */
void
AmSyncpointExchange::pack(uint32_t        userEvent,
                          uint32_t        commId,
                          ContentionInfo& info,
                          AmRequest*      request)
{
    Buffer* buffer = request->getBuffer();
    pack(userEvent, commId, info, *buffer);
}


/**
 * Executes handler on target side
 *
 * @param [in,out] buffer Buffer with handler information
 */
void
AmSyncpointExchange::execute(Buffer& buffer)
{
    const pearl::CallbackManager& cbmanager = AmRuntime::getInstance().getCbmanager();

    CbData* cbdata = static_cast< CbData* >(AmRuntime::getInstance().getCbdata());
    assert(cbdata);

    uint32_t userEvent = buffer.get_uint32();
    cbdata->mCommId = buffer.get_uint32();
    pearl::Event ev = cbdata->mTrace->at(buffer.get_id());
    cbdata->mCallpathId = ev.enterptr().get_cnode()->getId();
    cbdata->mIdle       = buffer.getTimestamp();
    cbdata->mSyncRank   = buffer.get_uint32();

    #ifdef DEBUG_SPX
        cerr << "AmSyncpointExchange::execute: " << " " << cbdata->m_idle << " " << cbdata->m_sync_rank << endl;
    #endif

    cbmanager.notify(userEvent, ev, cbdata);
}
