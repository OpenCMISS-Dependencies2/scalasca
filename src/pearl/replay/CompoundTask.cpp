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

#include <pearl/CompoundTask.h>

#include <algorithm>
#include <cassert>

using namespace std;
using namespace pearl;


// --- Constructors & destructor --------------------------------------------

/**
 *  Default constructor. Creates a new compound task without subtasks.
 **/
CompoundTask::CompoundTask()
    : Task()
{
}


/**
 *  Destructor. Deletes all subtasks associated with this compound task and
 *  destroys the instance.
 **/
CompoundTask::~CompoundTask()
{
    // Ignore duplicates to prevent double deletion.
    // std::unique only removes consecutive duplicates, so sort vector first
    sort(m_tasklist.begin(), m_tasklist.end());
    TaskList::iterator uend = unique(m_tasklist.begin(), m_tasklist.end());

    // Release resources
    for (TaskList::iterator it = m_tasklist.begin(); it != uend; ++it)
    {
        delete *it;
    }
}


// --- Executing the subtasks -----------------------------------------------

/**
 *  Executes the prepare() method to perform necessary initialitions, followed
 *  by all associated subtasks in the order they were added, and finally the
 *  finish() method to clean up. If any of these steps fail (indicated by
 *  returning @em false), execution is aborted immediately.
 *
 *  @return @em True if all subtasks (including prepare() and finish() were
 *          successful, @em false otherwise.
 **/
bool
CompoundTask::execute()
{
    // Preparation
    if (!prepare())
    {
        return false;
    }

    // Execute subtasks
    m_currentStep = 1;
    TaskList::iterator it = m_tasklist.begin();
    while (it != m_tasklist.end())
    {
        if (!(*it)->execute())
        {
            return false;
        }

        ++m_currentStep;
        ++it;
    }

    // Clean up
    return finish();
}


// --- Task handling --------------------------------------------------------

/**
 *  Add a subtask. The compound task will take ownership of the given @a task
 *  and release it on destruction.
 *
 *  Note that a single task can @em not be added to multiple CompoundTasks.
 *  However, it is save to append a task object to a single CompoundTask
 *  multiple times.
 *
 *  @param  task  %Task object to be added
 **/
void
CompoundTask::add_task(Task* task)
{
    // Validate arguments
    assert(task != 0);
    assert((task->m_parent == 0) || (task->m_parent == this));

    // Add task
    m_tasklist.push_back(task);

    // Update parent pointer
    task->set_parent(this);
}


/**
 *  Return total number of execution steps.
 *
 *  @return Number of steps
 **/
uint32_t
CompoundTask::num_steps() const
{
    return m_tasklist.size();
}


/**
 *  Return current step number.
 *
 *  @return Step number
 **/
uint32_t
CompoundTask::current_step() const
{
    return m_currentStep;
}


// --- Execution control (protected) ----------------------------------------

/**
 *  The prepare() callback method is called before execution of the subtasks
 *  starts. It can be overwritten in derived classes to perform any necessary
 *  initialization steps before executing the subtasks.
 *
 *  The default implementation does nothing and returns successfully.
 *
 *  @return Always @em true
 *
 *  @see execute()
 **/
bool
CompoundTask::prepare()
{
    return true;
}


/**
 *  The finish() callback method is called after the execution of the subtasks
 *  finishes. It can be overwritten in derived classes to perform necessary
 *  postprocessing after execution. It will only be called if all subtasks
 *  executed successfully.
 *
 *  The default implementation does nothing and returns successfully.
 *
 *  @return Always @em true
 *
 *  @see execute()
 **/
bool
CompoundTask::finish()
{
    return true;
}
