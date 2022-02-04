/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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

#include "AmWaitForProgressResponse.h"

#include <inttypes.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include <pearl/AmRequestFactory.h>
#include <pearl/AmRuntime.h>
#include <pearl/Buffer.h>
#include <pearl/CallbackManager.h>
#include <pearl/Callpath.h>
#include <pearl/CommSet.h>
#include <pearl/Event.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiComm.h>
#include <pearl/RemoteEvent.h>
#include <pearl/RemoteTimeSegment.h>
#include <pearl/RmaEpoch.h>
#include <pearl/TimeSegment.h>

#include "AmSyncpointExchange.h"
#include "AmWaitForProgressCache.h"
#include "CbData.h"
#include "Predicates.h"
#include "user_events.h"

using namespace std;
using namespace scout;
using namespace pearl;


/**
 * Standard constructor.
 */
AmWaitForProgressResponse::AmWaitForProgressResponse()
{
}


/**
 * Standard destructor.
 */
AmWaitForProgressResponse::~AmWaitForProgressResponse()
{
}


/**
 * Returns a reference to the handler instance
 *
 * @return Handler instance
 */
AmWaitForProgressResponse&
AmWaitForProgressResponse::getInstance()
{
    static AmWaitForProgressResponse mInstance;

    return mInstance;
}


/**
 * Packs handler relevant information to provided buffer.
 *
 * @param [in] key        Id to match response to initial request
 * @param [in] local_rank rank local to comm
 * @param [in] event_id   id of first event in epoch
 * @param [in] comm_id    id of communicator
 * @param [in] epoch      List of relevant events
 * @param [in,out] buffer Buffer to pack information in
 */
void
AmWaitForProgressResponse::pack(uint32_t        key,
                                uint32_t        local_rank,
                                uint32_t        event_id,
                                uint32_t        comm_id,
                                const RmaEpoch& epoch,
                                Buffer&         buffer)
{
    buffer.put_id(getInstance().getId());
    buffer.put_uint32(key);
    buffer.put_uint32(local_rank);
    buffer.put_uint32(event_id);
    buffer.put_uint32(comm_id);
    buffer.put_uint32((uint32_t)epoch.size());
    for (RmaEpoch::const_iterator it = epoch.begin(); it != epoch.end(); ++it)
    {
        buffer.put_event(*it);
    }
}


/**
 * Packs handler relevant information to provided buffer.
 *
 * @param [in] key         Id to match response to initial request
 * @param [in] local_rank  rank local to comm
 * @param [in] event_id    id of first event in epoch
 * @param [in] comm_id     id of communicator
 * @param [in] epoch       List of relevant events
 * @param [in,out] request Active message request
 */
void
AmWaitForProgressResponse::pack(uint32_t        key,
                                uint32_t        local_rank,
                                uint32_t        event_id,
                                uint32_t        comm_id,
                                const RmaEpoch& epoch,
                                AmRequest*      request)
{
    Buffer* buffer = request->getBuffer();
    pack(key, local_rank, event_id, comm_id, epoch, *buffer);
}


/**
 * Unpacks remote time segments and, if all responses have been received,
 * computes the severity of the pattern.
 *
 * @param [in,out] buffer Buffer holding handler information
 */
void
AmWaitForProgressResponse::execute(Buffer& buffer)
{
    CbData* cbdata = static_cast< CbData* >(AmRuntime::getInstance().getCbdata());
    assert(cbdata);

    const CallbackManager& cbmanager = AmRuntime::getInstance().getCbmanager();

    uint32_t      key         = buffer.get_uint32();
    uint32_t      remote_rank = buffer.get_uint32();
    uint32_t      event_id    = buffer.get_uint32();
    uint32_t      comm_id     = buffer.get_uint32();
    Communicator* comm        = cbdata->mDefs->get_comm(comm_id);
    uint32_t      size        = buffer.get_uint32();

    AmWaitForProgressCache::Element& element = AmWaitForProgressCache::getInstance().getElement(key);

    // epoch must be a list of event pairs (enter-leave)
    assert((size % 2) == 0);

    for (uint32_t i = 0; i < size; i += 2)
    {
        RemoteEvent remote_enter = buffer.get_event(*(cbdata->mDefs));
        RemoteEvent remote_exit  = buffer.get_event(*(cbdata->mDefs));

        // create pair from remote enter/exit and insert into map
        element.addRemoteSegment(RemoteTimeSegment(remote_enter, remote_exit));
    }

    if (element.numPending() == 0)
    {
        #if defined(WFP_LATEST_ENTER)
            const RemoteTimeSegment& lastRemoteSegment = element.getLastRemoteSegment();

            if (  (lastRemoteSegment->getStart() < element.getLocalLeave()->getTimestamp())
               && (lastRemoteSegment->getStart() > element.getLocalEnter()->getTimestamp()))
            {
                cbdata->mIdle = lastRemoteSegment->getStart() - element.getLocalEnter()->getTimestamp();
            }
        #else
            set< RemoteTimeSegment >::const_iterator remote_segment = element.getRemoteSegments().begin();
            assert(remote_segment != element.getRemoteSegments().end());

            cbdata->mIdle = 0.;
            timestamp_t current_local_start = element.getLocalEnter()->getTimestamp();

            while (remote_segment != element.getRemoteSegments().end())
            {
                if (remote_segment->getStart() > current_local_start)
                {
                    cbdata->mIdle += remote_segment->getStart() - current_local_start;
                }
                if (remote_segment->getEnd() > current_local_start)
                {
                    current_local_start = remote_segment->getEnd();
                }
                ++remote_segment;
            }
            assert(cbdata->mIdle <= element.getLocalSegment().getDuration());
        #endif

        if (cbdata->mIdle > 0)
        {
            cbdata->mCallpathId = element.getLocalEnter().get_cnode()->getId();

            AmRequest::Ptr request = AmRequestFactory::create(remote_rank, *comm);

            ContentionInfo info = { event_id, 0, comm->getCommSet().getGlobalRank(remote_rank) };

            AmSyncpointExchange::pack(WFP_SYNC, comm_id, info, request.get());

            AmRuntime::getInstance().enqueue(request);

            // notify pattern callback
            cbmanager.notify(RMA_WAIT_FOR_PROGRESS, element.getLocalEnter(), cbdata);
        }
    }
}


/**
 * Returns the name of the handler
 *
 * @return Handler name
 */
const string&
AmWaitForProgressResponse::getName() const
{
    static const string mName = "scout::AmWaitForProgressResponse";

    return mName;
}
