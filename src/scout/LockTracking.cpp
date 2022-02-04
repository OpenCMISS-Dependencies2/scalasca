/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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

#include "LockTracking.h"

#include <cassert>

#include <pearl/Callpath.h>
#include <pearl/ThreadAcquireLock_rep.h>
#include <pearl/ThreadReleaseLock_rep.h>

#include "ThreadCountedLock.h"

using namespace std;
using namespace pearl;
using namespace scout;


// --- Implementation Class LockTracking ------------------------------------

std::map< uint32_t, scout::ThreadCountedLock* > LockTracking::mLockMap;
pearl::Mutex                                    LockTracking::mLock;


// --- Public member functions ----------------------------------------------

pearl::timestamp_t
LockTracking::acquireLock(const Event& event)
{
    ThreadAcquireLock_rep& acquire          = event_cast< ThreadAcquireLock_rep >(*event);
    uint32_t               lockId           = acquire.getLockId();
    uint32_t               acquisitionOrder = acquire.getAcquisitionOrder();

    std::map< uint32_t, scout::ThreadCountedLock* >::iterator lock_it;
    if (acquisitionOrder == 1)
    {
        // first one into the lock - create data structures
        scout::ThreadCountedLock* lock = new scout::ThreadCountedLock();
        mLock.lock();
        lock_it = (mLockMap.insert(std::make_pair(lockId, lock))).first;
        mLock.unlock();
    }
    else
    {
        // not the first one into the lock - wait until data structures are there
        bool lock_exists = false;
        while (!lock_exists)
        {
            mLock.lock();
            lock_it     = mLockMap.find(lockId);
            lock_exists = (lock_it != mLockMap.end());
            mLock.unlock();
        }
    }

    scout::ThreadCountedLock* lock = lock_it->second;
    lock->acquire(acquisitionOrder);

    return lock->getTime(acquisitionOrder);
}


void
LockTracking::releaseLock(const Event& event)
{
    ThreadReleaseLock_rep& release          = event_cast< ThreadReleaseLock_rep >(*event);
    uint32_t               lockId           = release.getLockId();
    uint32_t               acquisitionOrder = release.getAcquisitionOrder();

    std::map< uint32_t, scout::ThreadCountedLock* >::iterator lock_it;

    mLock.lock();
    lock_it = mLockMap.find(lockId);
    assert(lock_it != mLockMap.end());
    mLock.unlock();

    lock_it->second->release(acquisitionOrder, event->getTimestamp());
}


// --- Related functions ----------------------------------------------------

LockType
scout::getLockType(const pearl::Event& event)
{
    Region   region   = event.get_cnode()->getRegion();
    Paradigm paradigm = region.getParadigm();

    if (paradigm == Paradigm::OPENMP)
    {
        Region::Role role = region.getRole();
        if (  (role == Region::ROLE_CRITICAL)
           || (role == Region::ROLE_CRITICAL_SBLOCK))
        {
            return LockType::OMP_CRITICAL;
        }
        else    // the only other case for OMP
        {
            return LockType::OMP_LOCK_API;
        }
    }
    else if (paradigm == Paradigm::PTHREAD)
    {
        if (is_pthread_mutex_lock(region))
        {
            return LockType::PTHREAD_MUTEX_LOCK;
        }
        if (is_pthread_mutex_trylock(region))
        {
            return LockType::PTHREAD_MUTEX_TRYLOCK;
        }
        if (is_pthread_condition(region))
        {
            return LockType::PTHREAD_CONDITION_WAIT;
        }
    }

    // unrecognized paradigm or lock
    return LockType::UNKNOWN;
}


Paradigm
scout::getLockParadigm(const pearl::Event& event)
{
    if (event->isOfType(THREAD_ACQUIRE_LOCK))
    {
        ThreadAcquireLock_rep& acquire = event_cast< ThreadAcquireLock_rep >(*event);

        return acquire.getParadigm();
    }
    else if (event->isOfType(THREAD_RELEASE_LOCK))
    {
        ThreadReleaseLock_rep& release = event_cast< ThreadReleaseLock_rep >(*event);

        return release.getParadigm();
    }

    // probably used in the wrong place if we return here
    return Paradigm::UNKNOWN;
}
