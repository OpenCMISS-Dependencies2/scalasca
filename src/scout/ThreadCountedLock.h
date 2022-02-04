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


#ifndef SCOUT_THREADCOUNTEDLOCK_H
#define SCOUT_THREADCOUNTEDLOCK_H


#include <cassert>
#include <map>

#include <pearl/Enter_rep.h>
#include <pearl/Event.h>
#include <pearl/Paradigm.h>
#include <pearl/Region.h>
#include <pearl/ScopedEnum.h>

#include "../pearl/thread/Threading.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Declaration of the ThreadCountedLock class.
 *
 *  This header file provides the declaration of the ThreadCountedLock class.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 *  @class ThreadCountedLock
 *  @brief Class for synchronizing the execution based on the acquisition
 *         order of locks.
 *
 *  Lock class that counts the number of acquisitions and compares it to the
 *  event information, ensuring correct order of execution.
 **/
/*-------------------------------------------------------------------------*/

class ThreadCountedLock
{
    public:
        ThreadCountedLock()
            : mCount(1),
              mNestingLevel(0),
              mTime(0)
        {
        }

        ~ThreadCountedLock()
        {
        }

        /// @brief Store timestamp of releasing the lock.
        ///
        /// If acquisition order matches the internal count, the count
        /// increases and the time is stored
        ///
        /// @param count
        ///     Acquisition order
        /// @param time
        ///     Timestamp of release
        ///
        void
        release(uint32_t           count,
                pearl::timestamp_t time)
        {
            mLock.lock();
            if (count == mCount)
            {
                mNestingLevel--;
                if (mNestingLevel == 0)
                {
                    ++mCount;
                }
                mTime = time;
            }
            else
            {
                assert(0);
            }
            mLock.unlock();
        }

        /// @brief Wait until acqusition counts match on acquire lock.
        ///
        /// Checks acquisition order against internal count and waits until
        /// they are the same.
        ///
        /// @param count
        ///     Acquisition order on acquire
        ///
        void
        acquire(uint32_t count)
        {
            while (!countsMatch(count))
            {
                // busy wait until counts match
            }
            mNestingLevel++;
        }

        /// @brief Get last release timestamp of the lock.
        ///
        /// If the acquisition order matches the internal count, it returns
        /// the stored release timestamp for the lock.
        ///
        /// @param count
        ///     Acquisition order on acquire
        /// @returns
        ///     Timestamp of the lock
        ///
        pearl::timestamp_t
        getTime(uint32_t count)
        {
            mLock.lock();
            assert(count == mCount);
            pearl::timestamp_t time = mTime;
            mLock.unlock();

            return time;
        }


    private:
        /// @brief Comparison of internal count and acquisition order.
        ///
        /// Inside of a lock it checks if the internal count and the
        /// acquisition order match.
        ///
        /// @param count
        ///     Acquisition order on acquire
        /// @returns
        ///     True if counts match, false otherwise
        ///
        inline bool
        countsMatch(uint32_t count)
        {
            mLock.lock();
            bool match = (count == mCount);
            mLock.unlock();

            return match;
        }

        uint32_t           mCount;
        uint32_t           mNestingLevel;
        pearl::timestamp_t mTime;
        pearl::Mutex       mLock;
};
}    // namespace scout


#endif    // !SCOUT_THREADCOUNTEDLOCK_H
