/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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

#include "Callstack.h"

#include <cstddef>

#include <pearl/Enter_rep.h>
#include <pearl/Region.h>
#include <pearl/ThreadTaskComplete_rep.h>
#include <pearl/ThreadTaskSwitch_rep.h>

#include "Threading.h"

using namespace std;
using namespace pearl;
using namespace scout;


Callstack::Callstack()
    : mCurrentTaskBlockId((static_cast< uint64_t >(PEARL_GetThreadNumber()) << 32)),
      mUpdateCallstack(true)
{
}


void
Callstack::update(const Event& event)
{
    // Update callstack if necessary
    if (mUpdateCallstack)
    {
        if (event->isOfType(GROUP_ENTER))
        {
            push(event);
        }
        else if (event->isOfType(GROUP_LEAVE))
        {
            pop();
        }
        else if (event->isOfType(THREAD_TEAM_BEGIN))
        {
            mStartTime[mCurrentTaskBlockId] = event->getTimestamp();

            mTotalOutOfTaskTime[mCurrentTaskBlockId] = 0;
            mSwitchOutTime[mCurrentTaskBlockId]      = 0;
        }
        else if (event->isOfType(THREAD_TASK_SWITCH))
        {
            // task handling
            ThreadTaskSwitch_rep& taskSwitch     = event_cast< ThreadTaskSwitch_rep >(*event);
            uint64_t              switchToTaskId = taskSwitch.getTaskId();

            // only if this is the first time we switch to this new task
            if (mStartTime.count(switchToTaskId) == 0)
            {
                // select the region for the stub node from the begining of the
                // chosen task region
                // ensure the next enter isn't a flush or any other artifical
                // step forward until a matching event is found
                Event next = event.next();
                while (next.is_valid())
                {
                    if (next->isOfType(GROUP_ENTER))
                    {
                        Enter_rep& temp = event_cast< Enter_rep >(*next);
                        if (temp.getRegion().getRole() != Region::ROLE_ARTIFICIAL)
                        {
                            break;
                        }
                    }
                    next = next.next();
                }

                // if the loop didn't find a matching enter, your trace is broken
                assert(next.is_valid());
                Enter_rep&    enter          = event_cast< Enter_rep >(*next);
                const Region& nextTaskRegion = enter.getRegion();

                // stub node handling only on the implicit task 0
                if (mCurrentTaskBlockId == (static_cast< uint64_t >(PEARL_GetThreadNumber()) << 32))
                {
                    Event eOrigin = event.prev();
                    if (!eOrigin->isOfType(THREAD_TASK_COMPLETE))
                    {
                        // before the switch is the normal implicit task region and not another task
                        // update the origin node for the stub nodes
                        mOrigin = eOrigin.get_cnode();
                    }
                }

                // find the matching child from the origin node
                for (uint32_t i = 0; i < mOrigin->numChildren(); i++)
                {
                    if (mOrigin->getChild(i).getRegion() == nextTaskRegion)
                    {
                        // store the ID for use in the patterns to generate the time and visits
                        mStubNodeCallpathIds[switchToTaskId] = mOrigin->getChild(i).getId();
                        break;
                    }
                }
            }

            // store the time you left the current task
            mSwitchOutTime[mCurrentTaskBlockId] = event->getTimestamp();

            // switch to new task id
            mCurrentTaskBlockId = switchToTaskId;

            if (mStacks[mCurrentTaskBlockId].empty())
            {
                // first time on this task
                // first task switch time, needed later for total task time calculation
                mStartTime[mCurrentTaskBlockId] = event->getTimestamp();

                mTotalOutOfTaskTime[mCurrentTaskBlockId] = 0;
                mSwitchOutTime[mCurrentTaskBlockId]      = 0;
            }
            else    // aka been there before and there is a usable reference time stored
            {
                // calc last switch out time
                double currentOutTime = event->getTimestamp() - mSwitchOutTime[mCurrentTaskBlockId];

                // add this to every element on this stack
                for (std::vector< double >::iterator it = mOutOfTaskTimes[mCurrentTaskBlockId].begin();
                     it != mOutOfTaskTimes[mCurrentTaskBlockId].end();
                     ++it)
                {
                    *it += currentOutTime;
                }

                // for stubnode correction also the total sum
                mTotalOutOfTaskTime[mCurrentTaskBlockId] += currentOutTime;
            }
        }
        else if (event->isOfType(THREAD_TASK_COMPLETE))
        {
            // switch to the implicit task 0 and clean up
            mCurrentTaskBlockId = (static_cast< uint64_t >(PEARL_GetThreadNumber()) << 32);

            // remove the unneeded slots connected to this task
            ThreadTaskComplete_rep& taskComplete = event_cast< ThreadTaskComplete_rep >(*event);
            uint64_t                eventId      = taskComplete.getTaskId();
            mStacks.erase(eventId);
            mOutOfTaskTimes.erase(eventId);
            mTotalOutOfTaskTime.erase(eventId);
            mStartTime.erase(eventId);
            mSwitchOutTime.erase(eventId);
        }
    }
}


Event
Callstack::top()
{
    assert(mStacks[mCurrentTaskBlockId].top()->isOfType(GROUP_ENTER));

    // return the top ENTER element from the current stack
    return mStacks[mCurrentTaskBlockId].top();
}


void
Callstack::push(const pearl::Event& event)
{
    // push event on current stack
    mStacks[mCurrentTaskBlockId].push(event);

    // initialize the offset with 0
    mOutOfTaskTimes[mCurrentTaskBlockId].push_back(0);
}


void
Callstack::pop()
{
    assert(mStacks[mCurrentTaskBlockId].top()->isOfType(GROUP_ENTER));

    // remove the top element from the current stack
    mStacks[mCurrentTaskBlockId].pop();
    mOutOfTaskTimes[mCurrentTaskBlockId].pop_back();
}


uint32_t
Callstack::getStubNodeId()
{
    return mStubNodeCallpathIds[mCurrentTaskBlockId];
}


void
Callstack::setUpdate(bool update)
{
    mUpdateCallstack = update;
}


double
Callstack::getCurrentTimeOffset()
{
    if (mOutOfTaskTimes[mCurrentTaskBlockId].size() == 0)
    {
        return 0;
    }

    return mOutOfTaskTimes[mCurrentTaskBlockId].back();
}


double
Callstack::getTotalTimeOffset()
{
    return mTotalOutOfTaskTime[mCurrentTaskBlockId];
}


double
Callstack::getStartTime()
{
    return mStartTime[mCurrentTaskBlockId];
}
