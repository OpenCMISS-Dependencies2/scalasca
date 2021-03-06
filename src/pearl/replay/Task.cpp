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

#include <pearl/Task.h>

using namespace pearl;


// --- Constructors & destructor --------------------------------------------

/**
 *  Destructor. Destroys the instance.
 **/
Task::~Task()
{
}


// --- Accessing task properties --------------------------------------------

/**
 *  Returns a pointer to the parent CompoundTask, if any.
 *
 *  @return Pointer to parent CompoundTask, or 0 if there is none.
 **/
CompoundTask*
Task::get_parent() const
{
    return m_parent;
}


// --- Constructors & destructor (protected) --------------------------------

/**
 *  Creates a new task instance and initializes its data.
 **/
Task::Task()
    : m_parent(0)
{
}


// --- Setting task properties (private) ------------------------------------

/**
 *  Sets the parent pointer. This method can be overwritten in derived
 *  classes. It is recommended to include a call to Task::set_parent()
 *  in a reimplementation.
 *
 *  @param  parent  Pointer to parent CompoundTask, or 0 if there is none.
 **/
void
Task::set_parent(CompoundTask* parent)
{
    m_parent = parent;
}
