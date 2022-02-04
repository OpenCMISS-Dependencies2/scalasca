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

#include "AmWaitForProgressCache.h"

#include <cassert>

#include <pearl/RemoteTimeSegment.h>
#include <pearl/TimeSegment.h>

using namespace std;
using namespace pearl;
using namespace scout;


/// Constructs a cache element using the expected number or responses and the
/// event list to create the request.
///
AmWaitForProgressCache::Element::Element(const std::set< int >& locations,
                                         const Event&           enter,
                                         const Event&           leave)
    : mPendingResponses(locations.size())
{
    mLocalEnter = enter;
    mLocalLeave = leave;
}


/// Factory method for cache elements
///
/// @return Smartpointer to cache element
///
AmWaitForProgressCache::ElementPtr
AmWaitForProgressCache::Element::create(const set< int >& locations,
                                        const Event&      enter,
                                        const Event&      leave)
{
    return AmWaitForProgressCache::ElementPtr(new AmWaitForProgressCache::Element(locations, enter, leave));
}


/// @brief Returns the number of pending responses for this element
///
/// @return Number of pending responses.
///
uint32_t
AmWaitForProgressCache::Element::numPending() const
{
    return mPendingResponses;
}


/// @brief Returns a reference the local enter event
///
/// @return Reference of the local enter event
///
const pearl::Event&
AmWaitForProgressCache::Element::getLocalEnter() const
{
    return mLocalEnter;
}


/// @brief Returns the local leave event
///
/// @return The local leave event
///
const pearl::Event&
AmWaitForProgressCache::Element::getLocalLeave() const
{
    return mLocalLeave;
}


/// @brief Adds a remote time segment to internal storage
///
/// @param [in] segment Remote time segment
///
void
AmWaitForProgressCache::Element::addRemoteSegment(pearl::RemoteTimeSegment segment)
{
    mRemoteSegments.insert(segment);
}


pearl::TimeSegment
AmWaitForProgressCache::Element::getLocalSegment() const
{
    return TimeSegment(mLocalEnter, mLocalLeave);
}


/// @brief Returns a reference to the remote segments container
///
/// @return Remote segments container
///
const std::set< pearl::RemoteTimeSegment >&
AmWaitForProgressCache::Element::getRemoteSegments() const
{
    return mRemoteSegments;
}


/// @brief Returns the time between the local enter and the last enter
///
/// @return Time between local and last remote enter
///
const pearl::RemoteTimeSegment&
AmWaitForProgressCache::Element::getLastRemoteSegment() const
{
    set< RemoteTimeSegment >::const_reverse_iterator segment      = mRemoteSegments.rbegin();
    set< RemoteTimeSegment >::const_reverse_iterator last_segment = segment;
    while (segment != mRemoteSegments.rend())
    {
        if (segment->getStart() > last_segment->getStart())
        {
            last_segment = segment;
        }
        ++segment;
    }

    return *last_segment;
}


/// Private constructor
///
AmWaitForProgressCache::AmWaitForProgressCache()
{
}


/// Private destructor
///
AmWaitForProgressCache::~AmWaitForProgressCache()
{
}


/// Returns a reference to the Cache object
///
/// @return Cache instance
///
AmWaitForProgressCache&
AmWaitForProgressCache::getInstance()
{
    static AmWaitForProgressCache mInstance;

    return mInstance;
}


/// Stores a cache element using a certain key
///
/// @param [in] key Unique id for the element
/// @param [in] element Cache element
///
void
AmWaitForProgressCache::storeElement(uint32_t                           key,
                                     AmWaitForProgressCache::ElementPtr element)
{
    mElements[key] = element.release();
}


/// Returns a reference to the cache element stored using @a key.
///
/// @param [in] key Unique id for the element
///
/// @return Cache element
///
AmWaitForProgressCache::Element&
AmWaitForProgressCache::getElement(uint32_t key)
{
    ContainerType::iterator element = mElements.find(key);
    assert(element != mElements.end());

    return *(element->second);
}


/// Frees all resources associated with element identified by @a key
///
/// @param [in] key Unique id for the element
///
void
AmWaitForProgressCache::purgeElement(uint32_t key)
{
    assert(mElements[key]->numPending() == 0);

    delete mElements[key];
    mElements.erase(key);
}
