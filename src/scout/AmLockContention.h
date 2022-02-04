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


#ifndef SCOUT_AMLOCKCONTENTION_H
#define SCOUT_AMLOCKCONTENTION_H


#include <map>
#include <queue>
#include <vector>

#include <pearl/AmHandler.h>
#include <pearl/AmRequest.h>
#include <pearl/Event.h>
#include <pearl/RemoteEvent.h>
#include <pearl/RmaEpoch.h>
#include <pearl/pearl_types.h>

#include "LockEpochQueue.h"


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class AmLockContention
 * @brief Active message handler for lock competition pattern.
 *
 * This handler packs a lock epoch on the origin process and collects
 * them on the target. The actual analysis needs to be done outside of
 * the handler.
 */
/*-------------------------------------------------------------------------*/

class AmLockContention
    : public pearl::AmHandler
{
    public:
        /// @name Singleton interface
        /// @{

        static AmLockContention&
        getInstance();

        /// @}
        /// @name Handler specific interface
        /// @{

        void
        registerQueue(pearl::ident_t  key,
                      LockEpochQueue* queue);

        void
        releaseQueue(pearl::ident_t key);

        /// @}
        /// @name Packing interface
        /// @{

        static void
        pack(pearl::ident_t   key,
             pearl::RmaEpoch& eventList,
             pearl::Buffer&   buffer);

        static void
        pack(pearl::ident_t    key,
             pearl::RmaEpoch&  eventList,
             pearl::AmRequest* request);

        /// @}
        /// @name Remote execution
        /// @{

        virtual void
        execute(pearl::Buffer& buffer);

        /// @}


    protected:
        /// @name Constructors & destructors
        /// @{

        AmLockContention();

        virtual
        ~AmLockContention();

        /// @}
        /// @name Debugging information
        /// @{

        virtual const std::string&
        getName() const;

        /// @}


    private:
        /// Map to store pointers to %LockEpochQueue objects used for a specific key
        typedef std::map< pearl::ident_t, LockEpochQueue* > QueueMap;


        QueueMap mQueues;
};
}    // namespace scout


#endif    // !SCOUT_AMLOCKCONTENTION_H
