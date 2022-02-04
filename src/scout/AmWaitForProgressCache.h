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


#ifndef SCOUT_AMWAITFORPROGRESSCACHE_H
#define SCOUT_AMWAITFORPROGRESSCACHE_H


#include <inttypes.h>

#include <map>
#include <memory>
#include <set>

#include <pearl/RemoteTimeSegment.h>
#include <pearl/RmaEpoch.h>
#include <pearl/TimeSegment.h>
#include <pearl/Uncopyable.h>


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class AmWaitForProgressCache
 * @brief Storage to exchange information between Requests and Responses for the Wait-for-progress pattern.
 *
 * Singleton class to provide intermediate storage capabilities for the
 * two active message handlers for the wait-for-progress pattern.
 */
/*-------------------------------------------------------------------------*/

class AmWaitForProgressCache
    : private pearl::Uncopyable
{
    public:
        // forward declaration
        class Element;

        /// Smart-pointed element type
        typedef std::auto_ptr< Element > ElementPtr;


        /*-----------------------------------------------------------------*/
        /**
         * @class Element
         * @brief Cache element for intermediate storage of information related to the Wait-for-progress pattern.
         *
         * Cache element for the Wait-for-progress pattern. Both, requests and
         * responses alike can access this singlton to store and retrieve pattern
         * related information.
         */
        /*-----------------------------------------------------------------*/

        class Element
        {
            public:
                /// @name Factory methods
                /// @{

                static ElementPtr
                create(const std::set< int >& locations,
                       const pearl::Event&    enter,
                       const pearl::Event&    leave);

                /// @}
                /// @name Local information interface
                /// @{

                uint32_t
                numPending() const;

                const pearl::Event&
                getLocalEnter() const;

                const pearl::Event&
                getLocalLeave() const;

                pearl::TimeSegment
                getLocalSegment() const;

                /// @}
                /// @name Remote information interface
                /// @{

                void
                addRemoteSegment(pearl::RemoteTimeSegment segment);

                const std::set< pearl::RemoteTimeSegment >&
                getRemoteSegments() const;

                const pearl::RemoteTimeSegment&
                getLastRemoteSegment() const;

                /// @}


            private:
                /// Number of pending responses from remote processes
                uint32_t mPendingResponses;

                /// Local enter event
                pearl::Event mLocalEnter;

                /// Local leave event
                pearl::Event mLocalLeave;

                /// Set of remote time segments
                std::set< pearl::RemoteTimeSegment > mRemoteSegments;

                /// @name Constructors
                /// @{

                Element(const std::set< int >& locations,
                        const pearl::Event&    enter,
                        const pearl::Event&    leave);

                /// @}
        };


        /// @name Singleton interface
        /// @{

        static AmWaitForProgressCache&
        getInstance();

        /// @}
        /// @name Storage interface
        /// @{

        void
        storeElement(uint32_t   key,
                     ElementPtr element);

        AmWaitForProgressCache::Element&
        getElement(uint32_t key);

        void
        purgeElement(uint32_t key);

        /// @}


    private:
        /// Container type for cache elements
        typedef std::map< uint32_t, AmWaitForProgressCache::Element* > ContainerType;

        /// Storage for elements
        ContainerType mElements;

        /// @name Construction & destruction
        /// @{

        AmWaitForProgressCache();

        ~AmWaitForProgressCache();

        /// @}
};
}    // namespace scout


#endif    // !SCOUT_AMWAITFORPROGRESSCACHE_H
