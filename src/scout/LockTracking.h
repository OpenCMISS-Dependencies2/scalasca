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


#ifndef SCOUT_LOCKTRACKING_H
#define SCOUT_LOCKTRACKING_H


#include <map>

#include <pearl/Event.h>
#include <pearl/Paradigm.h>
#include <pearl/ScopedEnum.h>

#include "../pearl/thread/Threading.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Declaration of the LockTracking class.
 *
 *  This header file provides the declaration of the LockTracking class and
 *  helper functions for lock type/pardigm differentiation.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Forward declarations -------------------------------------------------

class ThreadCountedLock;


// --- Type definitions -----------------------------------------------------

/// @brief Lock type definition.
///
/// Defines possible lock types as a ScopedEnum.
///
struct Lock_enum
{
    enum type
    {
        UNKNOWN,    ///< Unknown lock type

        OMP_CRITICAL,           ///< Named and unnamed OpenMP critical regions
        OMP_LOCK_API,           ///< OpenMP lock API
        PTHREAD_MUTEX_LOCK,     ///< Pthread lock API: mutex locks
        PTHREAD_MUTEX_TRYLOCK,  ///< Pthread lock API: mutex try locks
        PTHREAD_CONDITION_WAIT, ///< Pthread conditional wait
        NUMBER_OF_LOCK_TYPES    ///< For internal use only, not a lock type
    };
};


typedef pearl::ScopedEnum< Lock_enum > LockType;


/*-------------------------------------------------------------------------*/
/**
 *  @class LockTracking
 *  @brief Class for keeping track of locks.
 *
 *  This class keeps track of locks per paradigm and and associated release
 *  timestamps, for example, for use in the Lock Contention Pattern.
 **/
/*-------------------------------------------------------------------------*/

class LockTracking
{
    public:
        /// @brief Acquires a lock.
        ///
        /// Depending on the acquisition order of the event, it creates a new
        /// lock data structure for its respective paradigm or waits for it to
        /// be accessible. Then the associated timestamp of the last release of
        /// the lock returned for further calculation.
        ///
        /// @param event
        ///     Event, should be an acquire lock event
        /// @returns
        ///     Release timestamp for lock contention calculation
        ///
        pearl::timestamp_t
        acquireLock(const pearl::Event& event);

        /// @brief Releases a lock.
        ///
        /// Updates the respective timestamp based on the event and releases
        /// the lock.
        ///
        /// @param event
        ///     Event, should be a release lock event
        ///
        void
        releaseLock(const pearl::Event& event);


    private:
        ///  Map of locks: lock ID |-@> acquisition counter
        static std::map< uint32_t, scout::ThreadCountedLock* > mLockMap;

        /// Local mutex to assure thread safety
        static pearl::Mutex mLock;
};


// --- Related functions ----------------------------------------------------

/// @brief Get lock type for an event.
///
/// Returns the specific lock type based on the generic thread locking event.
///
/// @param event
///     Event, should be an acquire or release lock event
/// @returns
///     LockType if recognized, LockType::UNKNOWN otherwise
///
LockType
getLockType(const pearl::Event& event);


/// @brief Get paradigm for a lock event.
///
/// Returns the Paradigm of an acquire or release lock event.
///
/// @param event
///     Event, should be an acquire or release lock event
/// @returns
///     Paradigm if recognized, Paradigm::UNKNOWN otherwise
///
pearl::Paradigm
getLockParadigm(const pearl::Event& event);
}    // namespace scout


#endif    // !SCOUT_LOCKTRACKING_H
