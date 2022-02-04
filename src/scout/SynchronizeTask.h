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


#ifndef SCOUT_SYNCHRONIZETASK_H
#define SCOUT_SYNCHRONIZETASK_H


#include <pearl/CompoundTask.h>


// --- Forward declarations -------------------------------------------------

class Synchronizer;


/*-------------------------------------------------------------------------*/
/**
 *  @file  SynchronizeTask.h
 *  @brief Declaration of the SynchronizeTask class.
 *
 *  This header file provides the declaration of the SynchronizeTask class
 *  which performs a post-mortem timestamp synchronization based on the
 *  controlled logical clock algorithm.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Forward declarations -------------------------------------------------

struct TaskDataPrivate;


/*-------------------------------------------------------------------------*/
/**
 *  @class SynchronizeTask
 *  @brief Task performing a post-mortem timestamp synchronization.
 *
 *  The SynchronizeTask class performs a post-mortem timestamp syhcnronization
 *  of event streams based on the controlled logical clock algorithm.
 **/
/*-------------------------------------------------------------------------*/

class SynchronizeTask
    : public pearl::CompoundTask
{
    public:
        /// @name Constructors & destructor
        /// @{

        SynchronizeTask(TaskDataPrivate& privateData);

        virtual
        ~SynchronizeTask();

        /// @}
        /// @name Printing statistics
        /// @{

        void
        print_statistics() const;

        /// @}


    protected:
        /// @name Execution control
        /// @{

        virtual bool
        prepare();

        /// @}


    private:
        /// Private task data object
        TaskDataPrivate& mPrivateData;
};
}    // namespace scout


#endif    // !SCOUT_SYNCHRONIZETASK_H
