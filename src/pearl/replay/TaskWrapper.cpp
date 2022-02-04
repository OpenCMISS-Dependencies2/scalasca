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

#include <pearl/TaskWrapper.h>

#include <cassert>

using namespace pearl;


// --- Constructors & destructor --------------------------------------------

/**
 *  Destructor. Deletes the wrapped task and destroys the instance.
 **/
TaskWrapper::~TaskWrapper()
{
    delete m_task;
}


// --- Constructors & destructor (protected) --------------------------------

/**
 *  Creates a new wrapper for the given @a task.
 *
 *  @param  task  %Task to be wrapped
 **/
TaskWrapper::TaskWrapper(Task* task)
    : m_task(task)
{
    assert(m_task != 0);
}


// --- Setting task properties ----------------------------------------------

/**
 *  Sets the @a parent pointer of both the wrapper task and the wrapped task
 *  to the given CompoundTask object. This ensures that the wrapped task
 *  always refers to the correct CompoundTask as its parent.
 *
 *  If this method needs to be overwritten in derived classes, it is
 *  recommended to include a call to Task::set_parent().
 *
 *  @param  parent  Pointer to parent CompoundTask, or 0 if there is none.
 **/
void
TaskWrapper::set_parent(CompoundTask* parent)
{
    Task::set_parent(parent);
    m_task->set_parent(parent);
}
