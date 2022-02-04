/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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

#include "AmWaitForProgressRequest.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include <pearl/AmRequestFactory.h>
#include <pearl/AmRuntime.h>
#include <pearl/Buffer.h>
#include <pearl/CallbackManager.h>
#include <pearl/CommSet.h>
#include <pearl/Communicator.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/MpiComm.h>

#include "AmSeverityExchange.h"
#include "AmWaitForProgressResponse.h"
#include "CbData.h"
#include "Predicates.h"
#include "user_events.h"

using namespace std;
using namespace scout;
using namespace pearl;


/// Standard constructor (protected)
///
AmWaitForProgressRequest::AmWaitForProgressRequest()
{
}


/// Standard destructor
///
AmWaitForProgressRequest::~AmWaitForProgressRequest()
{
}


/// Returns a reference to the handler instance
///
/// @return Handler instance
///
AmWaitForProgressRequest&
AmWaitForProgressRequest::getInstance()
{
    static AmWaitForProgressRequest mInstance;

    return mInstance;
}


/// Generates a new key to be used in communication.
///
/// The key uniquely identifies a pattern communication. With this
/// key, the possible responses to this request can be matched.
///
/// @return locally unique key for request.
///
uint32_t
AmWaitForProgressRequest::generateKey()
{
    return ++mKey;
}


/// Pack handler specific information to provided buffer
///
/// @param [in] key        unique key identifying this request instance
/// @param [in] comm       communicator used for possible response
/// @param [in] dependency dependency type
/// @param [in] rmaEpoch   List of events in the epoch
/// @param [in,out] buffer Buffer to pack information in
///
void
AmWaitForProgressRequest::pack(uint32_t            key,
                               const Communicator& comm,
                               dependency_t        dependency,
                               const RmaEpoch&     rmaEpoch,
                               Buffer&             buffer)
{
    CbData* cbdata = static_cast< CbData* >(AmRuntime::getInstance().getCbdata());
    assert(cbdata);

    // pack handler id
    buffer.put_id(getInstance().getId());

    // pack transaction key
    buffer.put_uint32(key);

    // pack rank and communicator
    buffer.put_id(comm.getId());
    int globalRank = cbdata->mTrace->get_location().getRank();
    buffer.put_uint32(comm.getCommSet().getLocalRank(globalRank));

    // pack dependency type
    buffer.put_uint32(dependency);

    // pack epoch
    buffer.put_uint32((uint32_t)rmaEpoch.size());
    for (RmaEpoch::const_iterator it = rmaEpoch.begin(); it != rmaEpoch.end(); ++it)
    {
        buffer.put_event(*it);
    }
}


/// Pack handler specific information to internal buffer of provided request
///
/// @param [in] key         unique key identifying this request instance
/// @param [in] comm        communicator used for possible response
/// @param [in] dependency  dependency type
/// @param [in] rmaEpoch    List of events in the epoch
/// @param [in,out] request Active message request
///
void
AmWaitForProgressRequest::pack(uint32_t            key,
                               const Communicator& comm,
                               dependency_t        dependency,
                               const RmaEpoch&     rmaEpoch,
                               AmRequest*          request)
{
    Buffer* buffer = request->getBuffer();
    pack(key, comm, dependency, rmaEpoch, *buffer);
}


/// Helper struct for PGI compiler
struct timestamp_comp
{
    bool
    operator()(const pearl::Event_rep&  e,
               const pearl::timestamp_t t)
    {
        return (e < t);
    }
};


/// Unpacks remote event list and checks for local overlap.  Depending on
/// whether the waiting time can be computed locally, it does so or sends the
/// relevant information back to the origin.
///
/// @param [in,out] buffer Buffer holding handler information
///
void
AmWaitForProgressRequest::execute(Buffer& buffer)
{
    CbData* cbdata = static_cast< CbData* >(AmRuntime::getInstance().getCbdata());

    const pearl::CallbackManager& cbmanager = AmRuntime::getInstance().getCbmanager();

    // ensure callback data structure is present
    assert(cbdata);

    uint32_t        requestKey     = buffer.get_uint32();
    uint32_t        remote         = buffer.get_uint32();
    uint32_t        commId         = buffer.get_id();
    Communicator*   comm           = cbdata->mDefs->get_comm(commId);
    uint32_t        dependency     = buffer.get_uint32();
    uint32_t        size           = buffer.get_uint32();
    RemoteRmaEpoch* remoteRmaEpoch = new RemoteRmaEpoch();

    for (uint32_t i = 0; i < size; ++i)
    {
        remoteRmaEpoch->push_back(buffer.get_event(*(cbdata->mDefs)));
    }

    // Get remote event iterator
    RemoteRmaEpoch::iterator remote_it = remoteRmaEpoch->begin();

    // get remote enter
    while (  (remote_it != remoteRmaEpoch->end())
          && !(*remote_it)->isOfType(GROUP_ENTER))
    {
        ++remote_it;
    }
    if (remote_it == remoteRmaEpoch->end())
    {
        // TODO: Fix memory leak
        return;
    }
    RemoteEvent& remote_enter = *remote_it;
    assert((*remote_it)->isOfType(GROUP_ENTER));

    // get rma event
    while (  (remote_it != remoteRmaEpoch->end())
          && !(  (*remote_it)->isOfType(RMA_PUT_START)
              || (*remote_it)->isOfType(RMA_GET_START)))
    {
        ++remote_it;
    }
    RemoteEvent& remote_start = *remote_it;
    assert(  (*remote_it)->isOfType(RMA_PUT_START)
          || (*remote_it)->isOfType(RMA_GET_START));

    // get remote exit
    while (  (remote_it != remoteRmaEpoch->end())
          && !(*remote_it)->isOfType(GROUP_LEAVE))
    {
        ++remote_it;
    }
    if (remote_it == remoteRmaEpoch->end())
    {
        // TODO: Fix memory leak
        return;
    }
    RemoteEvent& remote_exit = *remote_it;
    assert((*remote_it)->isOfType(GROUP_LEAVE));

    // get first event older than remote enter

    Event local_exit = std::lower_bound(cbdata->mTrace->begin(),
                                        cbdata->mTrace->end(),
                                        remote_enter->getTimestamp(),
                                        timestamp_comp());

    // adjust local event iterator
    while (  (local_exit != cbdata->mTrace->end())
          && !(  local_exit->isOfType(GROUP_LEAVE)
              && is_progress(&(local_exit.get_cnode()->getRegion()))))
    {
        ++local_exit;
    }

    // if no progress function is found the function is assumed to
    // finish without remote progress
    if (local_exit == cbdata->mTrace->end())
    {
        // TODO: Fix memory leak
        return;
    }

    // set local enter event for convenience
    Event local_enter = local_exit.enterptr();

    // check for overlap
    timestamp_t idle_time = 0.;

    if (  (local_enter->getTimestamp() > remote_enter->getTimestamp())
       && (local_enter->getTimestamp() <= remote_exit->getTimestamp()))
    {
        idle_time = local_enter->getTimestamp() - remote_enter->getTimestamp();
    }

    // sanity check
    assert(idle_time <= remote_exit->getTimestamp() - remote_enter->getTimestamp());
    assert(idle_time >= 0);


    AmRequest::Ptr request = AmRequestFactory::create(remote, *comm);

    int globalRank = cbdata->mTrace->get_location().getRank();

    if (  (dependency == SINGLE_DEPENDENCY)
       && (idle_time > 0))
    {
        // pack pattern severity
        AmSeverityExchange::pack(RMA_WAIT_FOR_PROGRESS,
                                 remote_start.get_remote_index(),
                                 remote_start.get_cnode()->getId(),
                                 globalRank,
                                 idle_time, request.get());

        AmRuntime::getInstance().enqueue(request);

        // Prepare information for synchpoint
        cbdata->mSyncRank = comm->getCommSet().getGlobalRank(remote);
        cbdata->mCommId   = commId;
        cbdata->mIdle     = 0;

        cbmanager.notify(WFP_SYNC, local_enter, cbdata);
    }
    else if (dependency == MULTI_DEPENDENCY)
    {
        // Pack enter and exit times
        RmaEpoch epoch;
        epoch.push_back(local_enter);
        epoch.push_back(local_exit);

        AmWaitForProgressResponse::pack(requestKey,
                                        remote,
                                        local_enter.get_id(),
                                        commId,
                                        epoch,
                                        request.get());

        AmRuntime::getInstance().enqueue(request);
    }

    delete remoteRmaEpoch;
}


/// Returns the name of the handler
///
const string&
AmWaitForProgressRequest::getName() const
{
    static const string m_name = "scout::AmWaitForProgressRequest";

    return m_name;
}


/// Static variable containg the last key used
uint32_t AmWaitForProgressRequest::mKey = 1;
