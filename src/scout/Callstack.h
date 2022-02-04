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


#ifndef SCOUT_CALLSTACK_H
#define SCOUT_CALLSTACK_H


#include <map>
#include <stack>
#include <vector>

#include <pearl/Event.h>

#include "fixed_set.hh"
#include "scout_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @file  Callstack.h
 * @brief Declaration of the Callstack class.
 *
 * This header file provides the declaration of the Callstack class which
 * manages task-dependent callstacks.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class  Callstack
 * @brief  Class for keeping a callstack with consideration of tasks
 *
 * When encountering tasks this class keeps separate call stacks for
 * each active tasks and manages additional data to attribute correct
 * time spans to each task. For non-task programs the additional
 * overhead should be minimal since most task-specific actions are
 * only done on task events.
 */
/*-------------------------------------------------------------------------*/

class Callstack
{
    public:
        Callstack();

        ~Callstack()
        {
        }

        /// @brief Updates the callstack
        ///
        /// Method to update the callstack according to the event given and
        /// managing the times for the time spent inside and outside of tasks
        ///
        /// @param event
        ///     The current event, special considerations for ENTER/LEAVE
        ///     and TASK_SWITCH/TASK_COMPLETE
        ///
        void
        update(const pearl::Event& event);

        /// @name Implementation of standard stack methods (push, pop, and top)
        ///       for events
        /// @{
        pearl::Event
        top();

        void
        push(const pearl::Event& event);

        void
        pop();

        /// @}

        /// @brief Changing status of the callstack bookkeeping
        void
        setUpdate(bool update);

        /// @brief Checking status of the callstack bookkeeping
        bool
        getUpdate()
        {
            return mUpdateCallstack;
        }

        /// @name Retrival methods for the calculations in the time/visit pattern
        /// @{
        /// @brief Get the current time spent outside this task
        ///
        /// Returns the current stored time for on-the-fly task time calculation.
        ///
        /// @return Stored timestamp as double value
        ///
        double
        getCurrentTimeOffset();

        /// @brief Get the time spent outside this task
        ///
        /// Returns the complete time outside this task for stub node time
        /// calculation.
        ///
        /// @return Stored timestmap as double value
        ///
        double
        getTotalTimeOffset();

        /// @brief Get the start time of the current task
        ///
        /// Returns start time of the current task stored in the first switch
        /// of this task.
        ///
        /// @return Stored timestmap as double value
        ///
        double
        getStartTime();

        /// @brief Get the index of the task stub node
        ///
        /// Returns the callpath ID of the task stub node, stored on the first
        /// task switch, for visit/time calculation.
        ///
        /// @return Stored stub node id as integer value
        ///
        uint32_t
        getStubNodeId();

        /// @}


    private:
        /// task tracking, time correction
        std::map< uint64_t, std::stack< pearl::Event > > mStacks;
        std::map< uint64_t, std::vector< double > >      mOutOfTaskTimes;
        std::map< uint64_t, double >                     mStartTime;
        std::map< uint64_t, double >                     mSwitchOutTime;

        /// stub nodes
        std::map< uint64_t, double >   mTotalOutOfTaskTime;
        std::map< uint64_t, uint32_t > mStubNodeCallpathIds;
        uint64_t                       mCurrentTaskBlockId;
        pearl::Callpath*               mOrigin;

        /// enable/disable callstack updates
        bool mUpdateCallstack;
};
}    // namespace scout


#endif    // !SCOUT_CALLSTACK_H
