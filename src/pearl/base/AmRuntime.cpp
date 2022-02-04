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

#include <pearl/AmRuntime.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include <pearl/AmHandler.h>
#include <pearl/AmListener.h>
#include <pearl/AmListenerFactory.h>
#include <pearl/AmRequest.h>
#include <pearl/AmRequestFactory.h>
#include <pearl/Buffer.h>
#include <pearl/Communicator.h>

#include "AmNull.h"

using namespace std;
using namespace pearl;


// Forward declarations
class CallbackManager;


// --- Constructors & destructor --------------------------------------------

/**
 * Private default constructor needed for getInstance().
 */
AmRuntime::AmRuntime()
    : mCbdata(NULL),
      mCbmanager(NULL)
{
}


/**
 * Destructor.
 */
AmRuntime::~AmRuntime()
{
}


// --- Public interface -----------------------------------------------------

/**
 * Returns a reference to the runtime object
 */
AmRuntime&
AmRuntime::getInstance()
{
    static AmRuntime mInstance;

    return mInstance;
}


/**
 * Shuts down the runtime in a controlled manner
 */
void
AmRuntime::shutdown()
{
    if (getRequests().size() > 0)
    {
        cerr << "WARNING: Premature release of AmRuntime left "
             << getRequests().size() << " requests unhandled!" << endl;
    }
    for (RequestContainer::iterator request = getRequests().begin();
         request != getRequests().end();
         ++request)
    {
        (*request)->cancel();
    }
    getListeners().clear();

    /// @todo print summary of AM statistics
}


/**
 * Tries to progress any pending requests and executes any incoming requests.
 *
 * @return Number of requests advanced during this call.
 */
size_t
AmRuntime::advance()
{
    size_t numAdvanced = 0;

    // receive and execute pending messages
    for (ListenerContainer::iterator it = getListeners().begin();
         it != getListeners().end();
         ++it)
    {
        if (it->second->probe())
        {
            while (it->second->getBuffer().position() < it->second->getBuffer().size())
            {
                numAdvanced++;
                uint32_t handlerId = it->second->getBuffer().get_uint32();

                if (  (handlerId < getHandlers().size())
                   && (getHandlers()[handlerId] != NULL))
                {
                    getHandlers()[handlerId]->execute(it->second->getBuffer());

                    // keep track of number of executions
                    getStatistics()[handlerId] += 1;
                }
            }
        }
    }

    // test pending requests
    for (RequestContainer::iterator it = getRequests().begin();
         it != getRequests().end();
         ++it)
    {
        if ((*it)->test())
        {
            numAdvanced++;
        }
    }

    // erase completed requests
    RequestContainer::iterator newEnd =
        remove_if(getRequests().begin(),
                  getRequests().end(),
                  isFinished);
    getRequests().erase(newEnd, getRequests().end());

    return numAdvanced;
}


/**
 * Blocks until all local requests have finished
 */
void
AmRuntime::wait()
{
    while (getRequests().size() > 0)
    {
        advance();
    }
}


/**
 * Blocks until all local requests on @a comm have finished
 */
void
AmRuntime::wait(const Communicator& comm)
{
    size_t count = 0;

    do
    {
        count = getRequests().size();

        RequestContainer::iterator it = getRequests().begin();
        while (it != getRequests().end())
        {
            if ((*it)->getComm().getId() != comm.getId())
            {
                --count;
            }
            ++it;
        }
        advance();
    }
    while (count > 0);
}


/**
 * Sends a strictly synchronous request to all request targets
 * since the last fence.
 */
void
AmRuntime::allfence()
{
    TargetSetContainer targetSets = getTargetsets();

    for (TargetSetContainer::iterator tset = targetSets.begin();
         tset != targetSets.end();
         ++tset)
    {
        for (TargetSet::iterator target = tset->second.begin();
             target != tset->second.end();
             ++target)
        {
            AmRequest::Ptr request = AmRequestFactory::create(*target, *tset->first, true);

            AmNull::pack(request.get());

            AmRuntime::getInstance().enqueue(request);
        }
        tset->second.clear();
    }
    targetSets.clear();

    wait();
}


/**
 * Sends a strictly synchronous request to all request targets
 * since the last fence on a given communicator.
 *
 * @param [in] comm Communicator to fence.
 */
void
AmRuntime::fence(const Communicator& comm)
{
    TargetSet& targetSet = getTargetset(comm);

    for (TargetSet::iterator target = targetSet.begin();
         target != targetSet.end();
         ++target)
    {
        AmRequest::Ptr request = AmRequestFactory::create(*target, comm);

        AmNull::pack(request.get());

        AmRuntime::getInstance().enqueue(request);
    }
    targetSet.clear();

    wait(comm);
}


/**
 * Sets the internal pointer to the callback data object.
 *
 * @param data Pointer to callback data object.
 */
void
AmRuntime::setCbdata(CallbackData* data)
{
    mCbdata = data;
}


/**
 * Get the value of internal pointer to the callback data.
 *
 * @return Pointer to callback data object
 */
CallbackData*
AmRuntime::getCbdata() const
{
    return mCbdata;
}


/**
 * Sets the internal pointer to the callback manager in use
 *
 * @param cbmanager Pointer to callback manager object.
 */
void
AmRuntime::setCbmanager(const CallbackManager& cbmanager)
{
    mCbmanager = &cbmanager;
}


/**
 * Gets the internal pointer to the callback manager in use
 *
 * @return Pointer to the callback manager object
 */
const pearl::CallbackManager&
AmRuntime::getCbmanager() const
{
    return *mCbmanager;
}


// --- Message handling -----------------------------------------------------

/**
 * Attach a handler object to runtime system.
 * Only attached handlers are able to communicate through
 * the active message runtime system. During this process
 * the handler get its handler id from the runtime system.
 *
 * @param handler Pointer to AmHandler object
 */
void
AmRuntime::attach(AmHandler& handler)
{
    // only attach handler if it has not been attached already
    if (handler.getId() == AmHandler::UNREGISTERED)
    {
        // set handler id to index in internal handler vector
        handler.setId(getHandlers().size());

        // save pointer to handler instance
        getHandlers().push_back(&handler);
    }
}


/**
 * Insert request into the runtime system.
 *
 * @param request Smart pointer to request object.
 */
void
AmRuntime::enqueue(AmRequest::Ptr request)
{
    // start sending
    request->start();

    // add target to targetset of this request's communicator
    getTargetsets()[&request->getComm()].insert(request->getRemote());

    // save pointer to request object
    getRequests().push_back(request.release());
}


/**
 * Start listening on a certain communicator
 */
void
AmRuntime::startListen(const Communicator& comm)
{
    // check whether runtime listens on this communicator already
    ListenerContainer::iterator it = getListeners().find(comm.getId());
    if (it == getListeners().end())
    {
        getListeners().insert(make_pair(comm.getId(), AmListenerFactory::create(comm)));
    }
    else
    {
        it->second->addReference();
    }
}


/**
 * Stop listening on a certain communicator
 */
void
AmRuntime::stopListen(const Communicator& comm)
{
    // check whether we listen to comm
    ListenerContainer::iterator it = getListeners().find(comm.getId());
    if (it != getListeners().end())
    {
        // decrease number of references for listener
        it->second->removeReference();

        // erase from container if no references present
        if (it->second->numReferences() == 0)
        {
            getListeners().erase(it);
        }
    }
}


// --- Access to static storage ---------------------------------------------

/**
 * Return reference to static request storage
 *
 * @return Reference to static request storage
 */
AmRuntime::RequestContainer&
AmRuntime::getRequests()
{
    static RequestContainer mRequests;

    return mRequests;
}


/**
 * Return reference to static listener storage
 *
 * @return Reference to static listener storage
 */
AmRuntime::ListenerContainer&
AmRuntime::getListeners()
{
    static ListenerContainer mListeners;

    return mListeners;
}


/**
 * Return reference to static handler storage
 *
 * @return Reference to static handler storage
 */
AmRuntime::HandlerContainer&
AmRuntime::getHandlers()
{
    static HandlerContainer mHandlers;

    return mHandlers;
}


/**
 * Return reference to static statistics storage
 *
 * @return Reference to static statistics storage
 */
AmRuntime::StatisticsContainer&
AmRuntime::getStatistics()
{
    static StatisticsContainer mStatistics;

    return mStatistics;
}


/**
 * Returns reference to targetset container.
 * TargetSets contain the communication ranks of all active
 * message targets of a given communicator.
 *
 * @return Reference to targetset container.
 */
AmRuntime::TargetSetContainer&
AmRuntime::getTargetsets()
{
    static TargetSetContainer mTargetsets;

    return mTargetsets;
}


/**
 * Returns reference to the targetset of a given communicator.
 *
 * @param [in] comm Communicator the targetset is requested for.
 *
 * @return Reference to targetset.
 */
AmRuntime::TargetSet&
AmRuntime::getTargetset(const Communicator& comm)
{
    return getTargetsets()[&comm];
}


/**
 * Print runtime summary.
 */
void
AmRuntime::printSummary()
{
    stringstream message;

    /// @todo Gather global data and have only one process print
    /// the summary;

    message << "SCOUT: Active Message Summary:\n"
            << "==============================\n\n";

    for (HandlerContainer::const_iterator it = getHandlers().begin();
         it != getHandlers().end();
         ++it)
    {
        message << (*it)->getName() << ": " << getStatistics()[(*it)->getId()] << "\n";
    }
    cerr << message.str() << "\n" << endl;
}
