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


#ifndef PEARL_COMPOUNDTASK_H
#define PEARL_COMPOUNDTASK_H


#include <stdint.h>

#include <vector>

#include <pearl/Task.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    CompoundTask.h
 *  @ingroup PEARL_replay
 *  @brief   Declaration of the class CompoundTask.
 *
 *  This header file provides the declaration of the class CompoundTask.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   CompoundTask
 *  @ingroup PEARL_replay
 *  @brief   Maintains a set of tasks and executes them in sequence.
 *
 *  A CompoundTask maintains a list of subtasks and executes them in the
 *  order they were added. Subtasks can be added with
 *  CompoundTask::add_task().
 *
 *  CompoundTask takes ownership of the task objects associated with it and
 *  will automatically free them on destruction.
 **/
/*-------------------------------------------------------------------------*/

class CompoundTask
    : public Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        CompoundTask();

        virtual
        ~CompoundTask();

        /// @}
        /// @name Executing the subtasks
        /// @{

        virtual bool
        execute();

        /// @}
        /// @name Task handling
        /// @{

        void
        add_task(Task* task);

        uint32_t
        num_steps() const;

        uint32_t
        current_step() const;

        /// @}


    protected:
        /// @name Execution control
        /// @{

        virtual bool
        prepare();

        virtual bool
        finish();

        /// @}


    private:
        /// Container type for tasks
        typedef std::vector< Task* > TaskList;


        /// Stores the individual subtasks
        TaskList m_tasklist;

        /// Current step number during execution
        uint32_t m_currentStep;
};
}    // namespace pearl


#endif    // !PEARL_COMPOUNDTASK_H
