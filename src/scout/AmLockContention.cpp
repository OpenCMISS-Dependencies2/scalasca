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

#include "AmLockContention.h"

#include <inttypes.h>

#include <pearl/AmRuntime.h>
#include <pearl/Buffer.h>
#include <pearl/Event.h>

#include "CbData.h"

using namespace std;
using namespace scout;
using namespace pearl;


/// Standard constructor.
///
AmLockContention::AmLockContention()
{
}


/// Standard destructor
///
AmLockContention::~AmLockContention()
{
}


/// Returns a pointer to singleton object.
///
/// @return Reference to handler instance
///
AmLockContention&
AmLockContention::getInstance()
{
    static AmLockContention mInstance;

    return mInstance;
}


/// Registers a remote eventlist queue using a unique key
///
/// @param [in] key  Key to identify the queue (e.g., lock id)
/// @param [in] queue Pointer to remote event list priority queue
///
void
AmLockContention::registerQueue(ident_t         key,
                                LockEpochQueue* queue)
{
    mQueues[key] = queue;
}


/// Erases internal references to the queue
///
/// @param [in] key Key to identify the queue
///
void
AmLockContention::releaseQueue(ident_t key)
{
    map< ident_t, LockEpochQueue* >::iterator it = mQueues.find(key);

    if (it != mQueues.end())
    {
        mQueues.erase(it);
    }
}


/// Returns the name of the handler.
///
/// @return Name of the handler
///
const string&
AmLockContention::getName() const
{
    static const string mName = "scout::AmLockContention";

    return mName;
}


///  Packs all relevant information into the provided buffer.
///
/// @param [in] key        Id for the lock
/// @param [in] eventList All events of the lock epoch
/// @param [in,out] buffer Buffer to pack the information
///
void
AmLockContention::pack(ident_t   key,
                       RmaEpoch& eventList,
                       Buffer&   buffer)
{
    buffer.put_id(getInstance().getId());
    buffer.put_id(key);
    buffer.put_uint32((uint32_t)eventList.size());
    for (RmaEpoch::iterator it = eventList.begin(); it != eventList.end(); ++it)
    {
        buffer.put_event(*it);
    }
}


/// Packs all relevant information into the provided buffer.
///
/// @param [in] key         Id for the lock
/// @param [in] eventList  All events of the lock epoch
/// @param [in,out] request Active message request
///
void
AmLockContention::pack(ident_t    key,
                       RmaEpoch&  eventList,
                       AmRequest* request)
{
    Buffer* buffer = request->getBuffer();
    pack(key, eventList, *buffer);
}


/// Executes handler on target side
///
/// @param [in,out] buffer Buffer with handler information
///
void
AmLockContention::execute(Buffer& buffer)
{
    ident_t  key    = buffer.get_id();
    uint32_t size   = buffer.get_uint32();
    CbData*  cbdata = static_cast< CbData* >(AmRuntime::getInstance().getCbdata());

    RemoteRmaEpochPtr remoteRmaEpoch(new RemoteRmaEpoch());

    for (uint32_t i = 0; i < size; ++i)
    {
        remoteRmaEpoch->push_back(buffer.get_event(*(cbdata->mDefs)));
    }
    mQueues[key]->push(remoteRmaEpoch);
}
