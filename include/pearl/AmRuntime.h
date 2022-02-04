/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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


#ifndef PEARL_AMRUNTIME_H
#define PEARL_AMRUNTIME_H


#include <map>
#include <set>
#include <vector>

#include <pearl/AmListener.h>
#include <pearl/AmRequest.h>
#include <pearl/Uncopyable.h>
#include <pearl/pearl_types.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    AmRuntime.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class AmRuntime.
 *
 * This header files provides the declaration of the class
 * AmRuntime.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{
// Forward declarartions
class AmHandler;
class CallbackData;
class CallbackManager;
class Communicator;


/*-------------------------------------------------------------------------*/
/**
 * @class   AmRuntime
 * @ingroup PEARL_am
 * @brief   Active message runtime class.
 *
 * This class implements the core runtime environment for the
 * active messaging framework. It is designed as a singleton
 * so an application can have only one runtime object at any
 * time. The runtime uses three different classes to implement
 * its active message scheme. An AmRequest object is used to send
 * data to a remote location. An AmListener object is used to
 * probe for and receive messages on the remote location. Handler
 * classes are used to pack the initial message buffers on the
 * origin side and to unpack and execute their business logic on
 * the target side.
 *
 * @note It is the users responsibility to register Listener objects
 * on the target location that are capable of receiving requests that
 * injected into the system on any of the origin locations.
 */
/*-------------------------------------------------------------------------*/

class AmRuntime
    : private pearl::Uncopyable
{
    public:
        /// @name AmRuntime creation & destruction
        /// @{

        static AmRuntime&
        getInstance();

        void
        shutdown();

        /// @}
        /// @name AmRuntime progress
        /// @{

        size_t
        advance();

        void
        wait();

        void
        wait(const Communicator& comm);

        void
        allfence();

        void
        fence(const Communicator& comm);

        /// @}
        /// @name Accessing runtime parameters
        /// @{

        void
        setCbdata(CallbackData* data);

        CallbackData*
        getCbdata() const;

        void
        setCbmanager(const CallbackManager& cbmanager);

        const CallbackManager&
        getCbmanager() const;

        /// @}
        /// @name Message handling
        /// @{

        void
        attach(AmHandler& handler);

        void
        enqueue(AmRequest::Ptr request);

        void
        startListen(const Communicator& comm);

        void
        stopListen(const Communicator& comm);

        /// @}
        /// @name Runtime information
        /// @{

        void
        printSummary();

        /// @}


    private:
        /// @name Access to static storage
        /// @{

        /// Container type for requests
        typedef std::vector< AmRequest* > RequestContainer;

        /// Container type for listeners
        typedef std::map< pearl::ident_t, AmListener* > ListenerContainer;

        /// Container type for active message handlers
        typedef std::vector< AmHandler* > HandlerContainer;

        /// Container type for runtime statistics
        typedef std::map< uint32_t, size_t > StatisticsContainer;

        /// Container type for a single target set (list of active
        /// message target since last fence)
        typedef std::set< uint32_t > TargetSet;

        /// Container type for storing all active target sets
        typedef std::map< const pearl::Communicator*, TargetSet > TargetSetContainer;


        RequestContainer&
        getRequests();

        ListenerContainer&
        getListeners();

        HandlerContainer&
        getHandlers();

        StatisticsContainer&
        getStatistics();

        TargetSetContainer&
        getTargetsets();

        TargetSet&
        getTargetset(const Communicator& comm);

        /// @}
        /// @name Constructors & destructor
        /// @{

        AmRuntime();

        ~AmRuntime();

        /// @}


        /// Pointer to callback data structure in use
        CallbackData* mCbdata;

        /// Pointer to callback manager in use
        const CallbackManager* mCbmanager;
};
}    // namespace pearl


#endif    // !PEARL_AMRUNTIME_H
