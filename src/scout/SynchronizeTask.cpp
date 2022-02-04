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


#include <config.h>

#include "SynchronizeTask.h"

#include <cstddef>

#include <pearl/Location.h>

#include "CheckedTask.h"
#include "Synchronizer.h"
#include "TaskData.h"
#include "TimedPhase.h"

using namespace std;
using namespace pearl;
using namespace scout;


// --- Local helper macros --------------------------------------------------

#define SUBTASK(task, message)                                       \
    add_task(TimedPhase::make_timed(CheckedTask::make_checked(task), \
                                    message, 1, true))


// --- Helper classes -------------------------------------------------------

namespace
{
/*-------------------------------------------------------------------------*/
/**
 *  @class ForwardAmortTask
 *  @brief Task performing forward timestamp amortization.
 *
 *  The ForwardAmortTask class implements a sub-task of SynchronizerTask
 *  which performs the forward timestamp amortization phase of the controlled
 *  logical clock algorithm.
 **/
/*-------------------------------------------------------------------------*/

class ForwardAmortTask
    : public pearl::Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        ForwardAmortTask(const TaskDataPrivate& privateData);

        /// @}
        /// @name Execution control
        /// @{

        virtual bool
        execute();

        /// @}


    private:
        /// Private task data object
        const TaskDataPrivate& mPrivateData;
};


/*-------------------------------------------------------------------------*/
/**
 *  @class BackwardAmortTask
 *  @brief Task performing backward timestamp amortization.
 *
 *  The BackwardAmortTask class implements a sub-task of SynchronizerTask
 *  which performs the backward timestamp amortization phase of the controlled
 *  logical clock algorithm.
 **/
/*-------------------------------------------------------------------------*/

class BackwardAmortTask
    : public pearl::Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        BackwardAmortTask(const TaskDataPrivate& privateData);

        /// @}
        /// @name Execution control
        /// @{

        virtual bool
        execute();

        /// @}


    private:
        /// Private task data object
        const TaskDataPrivate& mPrivateData;
};
}    // unnamed namespace


// --------------------------------------------------------------------------
//
//  class SynchronizeTask
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new synchronizer task.
 *
 *  This constructor creates a new timestamp synchronization task.
 *
 *  @param  privateData  Private task data object
 **/
SynchronizeTask::SynchronizeTask(TaskDataPrivate& privateData)
    : CompoundTask(),
      mPrivateData(privateData)
{
    SUBTASK(new ForwardAmortTask(privateData),
            "\n"
            "  Forward amortization           ");
    SUBTASK(new BackwardAmortTask(privateData),
            "  Backward amortization          ");
}


/**
 *  @brief Destructor.
 *
 *  Destructor. Destroys the instance and releases all occupied resources.
 **/
SynchronizeTask::~SynchronizeTask()
{
    delete mPrivateData.mSynchronizer;
}


// --- Printing statistics --------------------------------------------------

/**
 *  @brief Prints some synchronizer-related statistics.
 *
 *  This method prints some additional statistics about the timestamp
 *  synchronization process. In case the synchronizer task has not been
 *  called, it defaults to a no-op.
 **/
void
SynchronizeTask::print_statistics() const
{
    if (mPrivateData.mSynchronizer)
    {
        mPrivateData.mSynchronizer->print_statistics();
    }
}


// --- Execution control (protected) ----------------------------------------

/**
 *  @brief Subtask execution setup.
 *
 *  Creates a timestamp synchronization object before the subtasks are
 *  executed.
 *
 *  @return Always @em true
 **/
bool
SynchronizeTask::prepare()
{
    const LocalTrace& trace = *mPrivateData.mTrace;

    int rank = trace.get_location().getRank();
    mPrivateData.mSynchronizer = new Synchronizer(rank, trace);

    return true;
}


// --------------------------------------------------------------------------
//
//  class ForwardAmortTask
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new ForwardAmortTask task instance.
 *
 *  This constructor creates a new task instance for performing the
 *  forward amortization phase of the controlled logical clock algorithm.
 *
 *  @param  privateData  Private task data object
 **/
ForwardAmortTask::ForwardAmortTask(const TaskDataPrivate& privateData)
    : Task(),
      mPrivateData(privateData)
{
}


// --- Execution control ----------------------------------------------------

/**
 *  @brief Executes the task.
 *
 *  Executes the forward amortization phase of the controlled logical clock
 *  algorithm.
 *
 *  @return Always @em true.
 **/
bool
ForwardAmortTask::execute()
{
    mPrivateData.mSynchronizer->forward_amortization();

    return true;
}


// --------------------------------------------------------------------------
//
//  class BackwardAmortTask
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new BackwardAmortTask task instance.
 *
 *  This constructor creates a new task instance for performing the
 *  backward amortization phase of the controlled logical clock algorithm.
 *
 *  @param  privateData  Private task data object
 **/
BackwardAmortTask::BackwardAmortTask(const TaskDataPrivate& privateData)
    : Task(),
      mPrivateData(privateData)
{
}


// --- Execution control ----------------------------------------------------

/**
 *  @brief Executes the task.
 *
 *  Executes the backward amortization phase of the controlled logical clock
 *  algorithm.
 *
 *  @return Always @em true.
 **/
bool
BackwardAmortTask::execute()
{
    mPrivateData.mSynchronizer->backward_amortization();

    return true;
}
