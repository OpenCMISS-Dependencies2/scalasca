/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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


#ifndef PEARL_THREADING_H
#define PEARL_THREADING_H


/*-------------------------------------------------------------------------*/
/**
 *  @file    Threading.h
 *  @ingroup PEARL_thread
 *  @brief   Declaration of the threading-related classes and functions.
 *
 *  This header file provides the declaration of the class Mutex as well as
 *  other threading-related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   Mutex
 *  @ingroup PEARL_thread
 *  @brief   Mutex abstraction.
 *
 *  This class provides an abstraction of a programming-model dependent
 *  Mutex implementation. It provides the typical lock/unlock functionality.
 **/
/*-------------------------------------------------------------------------*/

class Mutex
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Default constructor
        ///
        /// Creates a new mutex.
        ///
        Mutex();

        /// @brief Destructor
        ///
        /// Destroys the instance. The mutex must be unlocked for the
        /// destructor to be successful.
        ///
        ~Mutex();

        /// @}
        /// @name Mutex functionality
        /// @{

        /// @brief Lock the mutex.
        ///
        /// Provides means to lock the mutex. The calling thread blocks until
        /// the mutex was acquired.
        ///
        void
        lock();

        /// @brief Unlock the mutex.
        ///
        /// Provides means to unlock the mutex. If one or more threads are
        /// waiting for the mutex, one thread is chosen and given ownership.
        ///
        void
        unlock();

        /// @}


    private:
        // --- Forward declarations ------------------------

        struct mutex_t;


        // --- Data members --------------------------------

        /// Programming-model specific mutex implementation
        mutex_t* mMutex;
};


// --- Function prototypes --------------------------------------------------

/// @name Threading-related functions
/// @{

/// @brief Get thread number.
///
/// Returns the ID of the calling thread.
///
/// @return Thread number
///
int
PEARL_GetThreadNumber();

/// @brief Get the number of threads.
///
/// Returns the number of threads.
///
/// @return Number of threads
///
int
PEARL_GetNumThreads();

/// @brief Synchronize threads at a barrier.
///
/// Synchronizes all PEARL threads at the point where the function call
/// appears.
///
void
PEARL_Barrier();

/// @}
}    // namespace pearl


#endif    // !PEARL_THREADING_H
