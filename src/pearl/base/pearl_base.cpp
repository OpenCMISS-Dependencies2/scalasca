/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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

#include <pearl/pearl.h>

#include <cfloat>
#include <map>

#define SCALASCA_DEBUG_MODULE_NAME    PEARL
#include <UTILS_Debug.h>

#include <pearl/Callpath.h>
#include <pearl/Callsite.h>
#include <pearl/Enter_rep.h>
#include <pearl/Error.h>
#include <pearl/Event.h>
#include <pearl/Leave_rep.h>
#include <pearl/LocalTrace.h>
#include <pearl/Region.h>
#include <pearl/ThreadBegin_rep.h>
#include <pearl/ThreadCreate_rep.h>
#include <pearl/ThreadTaskComplete_rep.h>
#include <pearl/ThreadTaskSwitch_rep.h>

#include "CallTree.h"
#include "DefsFactory.h"
#include "EventFactory.h"
#include "MutableGlobalDefs.h"
#include "Threading.h"
#include "pearl_ipc.h"
#include "pearl_memory.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Local variables ------------------------------------------------------

namespace
{
/// Mutex variable protecting call tree updates during trace preprocessing
Mutex calltreeMutex;

void
determineTimeOffset(MutableGlobalDefs& defs,
                    const LocalTrace&  trace);

Callpath*
mapCallpath(CallTree*       calltree,
            const Callpath* callpath);
}    // unnamed namespace


// --- Local defines --------------------------------------------------------

#define BALANCED           0
#define TOO_MANY_ENTERS    1
#define TOO_MANY_LEAVES    2


// --- Library setup & teardown ---------------------------------------------

/**
 *  Initializes the PEARL library and installs a custom out-of-memory handler.
 *  It is required to call one of the PEARL initialization functions before
 *  calling any other PEARL function or instantiating any PEARL class.
 *
 *  @attention Make sure to initialize PEARL before installing any exit
 *             handlers using atexit(). Otherwise, you might get error
 *             messages about memory leaks.
 *
 *  @note This function is intended to be used in non-MPI PEARL programs.
 *
 *  @see PEARL_mpi_init()
 **/
void
pearl::PEARL_init()
{
    UTILS_DLOG << "Initializing PEARL";

    // Register factories & allocators
    DefsFactory::registerFactory(new DefsFactory());
    EventFactory::registerFactory(new EventFactory());

    // Initialize internal layers
    memoryInit();
    ipcInit();
}


/**
 *  This function finalizes the PEARL library. Each PEARL program must call
 *  PEARL_finalize() in a single-threaded context before it exits. In a
 *  multi-process setup (e.g., when using MPI), this function has to be
 *  called before the corresponding communication library is finalized.
 **/
void
pearl::PEARL_finalize()
{
    // Finalize internal layers
    ipcFinalize();
    memoryFinalize();

    // Deregister factories
    EventFactory::deregisterFactory();
    DefsFactory::deregisterFactory();

    UTILS_DLOG << "Finalized PEARL";
}


// --- Call tree verification -----------------------------------------------

/**
 *  Verifies whether the global call tree provided by the trace definition
 *  data @a defs is complete with respect to the local @a trace data. If not,
 *  the process-local call tree is extended accordingly. This has to be done
 *  before PEARL_preprocess_trace() is called.
 *
 *  @param defs  Global definitions object
 *  @param trace Local trace data object
 *
 *  @see PEARL_mpi_unify_calltree(), PEARL_preprocess_trace()
 *
 *  @todo Add trace consistency checks (matching ENTER/LEAVE)
 **/
void
pearl::PEARL_verify_calltree(GlobalDefs& defs,
                             LocalTrace& trace)
{
    MutableGlobalDefs& mutableDefs = dynamic_cast< MutableGlobalDefs& >(defs);

    // Determine process-local timestamp offset
    determineTimeOffset(mutableDefs, trace);

    // Rectify event order of buffer flush events
    LocalTrace::reverse_iterator rEvent  = trace.rbegin();
    bool                         isFlush = false;
    while (rEvent != trace.rend())
    {
        if (rEvent->isOfType(GROUP_ENTER))
        {
            Enter_rep&    enterRep = static_cast< Enter_rep& >(*rEvent);
            const Region& region   = enterRep.getRegionEntered();

            if (region == defs.getFlushingRegion())
            {
                isFlush = true;
            }
        }
        else if (rEvent->isOfType(GROUP_LEAVE))
        {
            Leave_rep&    leaveRep = static_cast< Leave_rep& >(*rEvent);
            const Region& region   = leaveRep.getRegionLeft();

            if (region == defs.getFlushingRegion())
            {
                isFlush = true;
            }
        }

        // "Bubble" the FLUSHING event to the correct position in the event stream
        if (isFlush)
        {
            timestamp_t eventTime = rEvent->getTimestamp();
            Event       next      = rEvent.base();
            Event       current   = next.prev();
            while (  (next != trace.end())
                  && (next->getTimestamp() <= eventTime))
            {
                trace.swap(current, next);
                ++current;
                ++next;
            }
            isFlush = false;
        }
        ++rEvent;
    }

    // These variables are shared!
    // Used to communicate callpaths from master thread to worker threads
    static Callpath* sharedCallpath = NULL;

    // Call tree verification
    const int threadId = PEARL_GetThreadNumber();
    CallTree* ctree    = (threadId == 0)
                         ? mutableDefs.getCallTree()
                         : new CallTree;
    Callpath* current = NULL;
    long      depth   = 0;
    int       status  = BALANCED;

    // Tasking related
    uint64_t                        currentTaskId  = trace.get_location().getId();
    uint64_t                        previousTaskId = 0;
    std::map< uint64_t, Callpath* > activeTasks;
    Callpath*                       lastStubNodeParent = NULL;

    // Create program callpath if first event on master thread is of type
    // EnterProgram; used as parent by the artificial TASKS/THREADS callpaths
    Event event = trace.begin();
    if (  (threadId == 0)
       && event.is_valid()
       && event->isOfType(ENTER_PROGRAM))
    {
        Enter_rep& enter = event_cast< Enter_rep >(*event);

        sharedCallpath = ctree->getCallpath(enter.getRegionEntered(),
                                            enter.getCallsite(),
                                            NULL);
    }
    PEARL_Barrier();
    Callpath* programCallpath = mapCallpath(ctree, sharedCallpath);
    PEARL_Barrier();

    while (event != trace.end())
    {
        // ENTER:
        if (event->isOfType(GROUP_ENTER))
        {
            Enter_rep& enter = event_cast< Enter_rep >(*event);

            // Update thread-local call tree and current call path
            current = ctree->getCallpath(enter.getRegionEntered(),
                                         enter.getCallsite(),
                                         current);

            // Increase call stack depth
            depth++;
        }

        // LEAVE:
        else if (event->isOfType(GROUP_LEAVE))
        {
            // Decrease call stack depth
            depth--;

            // Verify correct nesting of ENTER/LEAVE events
            // Call stack empty ==> too many LEAVE events
            if ((depth < 0) && (status == BALANCED))
            {
                status = TOO_MANY_LEAVES;
            }

            // Update current call path
            current = current->getParent();
        }

        // THREAD_FORK:
        //  - occurs only on master thread
        //  - immediately before THREAD_TEAM_BEGIN
        else if (event->isOfType(THREAD_FORK))
        {
            // Provide current callpath to worker threads
            sharedCallpath = current;
        }

        // THREAD_TEAM_BEGIN:
        //  - first event on each fork/join thread
        else if (event->isOfType(THREAD_TEAM_BEGIN))
        {
            // Synchronize team to ensure that the master thread has stored the
            // parent callpath while processing the preceding THREAD_FORK event
            PEARL_Barrier();

            if (threadId != 0)
            {
                // Verify correct nesting of ENTER/LEAVE events on worker threads
                // for *previous* parallel region
                // Something left on the callstack ==> too many ENTER events
                if ((depth > 0) && (status == BALANCED))
                {
                    status = TOO_MANY_ENTERS;
                }

                // Get parent callpath from master thread and map it to the
                // corresponding local callpath
                current = mapCallpath(ctree, sharedCallpath);
            }

            // Synchronize team again to avoid data races
            // (As there is no synchronization at the end of a thread team, the
            // master could potentially already process the next THREAD_FORK
            // event and overwrite the parent callpath variable before worker
            // threads had a chance read it.)
            PEARL_Barrier();
        }

        // THREAD_BEGIN:
        //  - first event on each create/wait thread
        else if (event->isOfType(THREAD_BEGIN))
        {
            // Set current call path to artificial THREADS call path, so that the
            // thread function is created as a child
            current = ctree->getCallpath(defs.getThreadRootRegion(),
                                         Callsite::UNDEFINED,
                                         programCallpath);
        }

        // THREAD_TASK_SWITCH:
        else if (event->isOfType(THREAD_TASK_SWITCH))
        {
            // keep track of previous task
            previousTaskId = currentTaskId;

            // get id from event
            ThreadTaskSwitch_rep& taskSwitch = event_cast< ThreadTaskSwitch_rep >(*event);
            currentTaskId = taskSwitch.getTaskId();

            // store current callpath progress
            activeTasks[previousTaskId] = current;

            if (activeTasks.count(currentTaskId) == 0)
            {
                // get the region for the stubnode from the following task
                // advancing in the trace until the required event is found
                // checking if it is an enter and if its role is not
                // ROLE_ARTIFICIAL (e.g. flushing, pausing)
                Event next = event.next();
                while (next.is_valid())
                {
                    if (next->isOfType(GROUP_ENTER))
                    {
                        Enter_rep& test = event_cast< Enter_rep >(*next);
                        if (test.getRegionEntered().getRole() != Region::ROLE_ARTIFICIAL)
                        {
                            break;
                        }
                    }
                    next = next.next();
                }

                // if the loop didn't find a matching enter, your trace is broken
                assert(next.is_valid());
                Enter_rep&    enter          = event_cast< Enter_rep >(*next);
                const Region& nextTaskRegion = enter.getRegionEntered();

                // if on the implicit task - create the task node
                if (previousTaskId == trace.get_location().getId())
                {
                    lastStubNodeParent = current;
                }
                ctree->getCallpath(nextTaskRegion,
                                   Callsite::UNDEFINED,
                                   lastStubNodeParent);

                // store task_root node as parent for this task
                current = ctree->getCallpath(defs.getTaskRootRegion(),
                                             Callsite::UNDEFINED,
                                             programCallpath);
            }
            else
            {
                // continue the callpath stored for this taskId
                current = activeTasks[currentTaskId];
            }
        }

        // THREAD_TASK_COMPLETE:
        else if (event->isOfType(THREAD_TASK_COMPLETE))
        {
            // fall back to the implicit task
            currentTaskId = trace.get_location().getId();    // until the next task switch
            current       = activeTasks[currentTaskId];

            // task complete remove data to minimize resources
            ThreadTaskComplete_rep& taskComplete = event_cast< ThreadTaskComplete_rep >(*event);
            activeTasks.erase(taskComplete.getTaskId());
        }

        // Go to next event
        ++event;
    }

    // Verify correct nesting of Enter/Leave events
    // Something left on the call stack ==> too many Enter events
    // Call stack empty ==> too many Leave events
    if (  (depth < 0)
       || (status == TOO_MANY_LEAVES))
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many LEAVEs).");
    }
    else if (  (depth > 0)
            || (status == TOO_MANY_ENTERS))
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many ENTERs).");
    }

    // Track thread-local call trees in a shared array
    const int         numThreads = PEARL_GetNumThreads();
    static CallTree** localTrees = 0;
    if (threadId == 0)
    {
        localTrees = new CallTree*[numThreads];
    }
    PEARL_Barrier();
    localTrees[threadId] = ctree;

    // Hierarchically merge thread-local call trees
    int distance = 1;
    while (distance < numThreads)
    {
        // Ensure readiness of source call trees
        PEARL_Barrier();

        // Merge and delete source call tree
        const int  source = threadId + distance;
        const bool active = threadId % (2 * distance) == 0;
        if (active && (source < numThreads))
        {
            ctree->merge(*localTrees[source]);
            delete localTrees[source];
        }

        // Double the distance
        distance *= 2;
    }

    // Avoid data race w.r.t. process-local call tree
    PEARL_Barrier();

    // Clean up shared call tree array
    if (threadId == 0)
    {
        delete[] localTrees;
        localTrees = 0;
    }
}


// --- Trace preprocessing --------------------------------------------------

/**
 *  Performs some local preprocessing of the given @a trace which is required
 *  to provide the full trace-access functionality. This has to be done as the
 *  last step in setting up the data structures, i.e., after calling
 *  PEARL_verify_calltree() and PEARL_mpi_unify_calltree().
 *
 *  @param defs  Global definitions object
 *  @param trace Local trace data object
 *
 *  @see PEARL_verify_calltree(), PEARL_mpi_unify_calltree()
 **/
void
pearl::PEARL_preprocess_trace(const GlobalDefs& defs,
                              const LocalTrace& trace)
{
    // No explicit thread synchronization is required here. The threads will be
    // synchronized when examining the Enter event of the first parallel region
    // of the trace.

    const MutableGlobalDefs& mutableDefs =
        dynamic_cast< const MutableGlobalDefs& >(defs);

    // This variable is shared!
    // Used to communicate callpaths from master thread to worker threads
    static Callpath* sharedCallpath = NULL;

    // Precompute callpath pointers and apply global time offset shifting
    timestamp_t offset  = mutableDefs.getGlobalOffset();
    CallTree*   ctree   = mutableDefs.getCallTree();
    Callpath*   current = NULL;

    // Tasking related
    uint64_t                        currentTaskId  = trace.get_location().getId();
    uint64_t                        previousTaskId = 0;
    std::map< uint64_t, Callpath* > activeTasks;
    std::map< uint64_t, Event >     requestMap;

    // Determine program callpath if first event on master thread is of type
    // EnterProgram; used as parent by the artificial TASKS/THREADS callpaths
    static Callpath* programCallpath = NULL;
    Event            event           = trace.begin();
    if (  (PEARL_GetThreadNumber() == 0)
       && event.is_valid()
       && event->isOfType(ENTER_PROGRAM))
    {
        Enter_rep& enter = event_cast< Enter_rep >(*event);

        programCallpath = ctree->getCallpath(enter.getRegionEntered(),
                                             enter.getCallsite(),
                                             NULL);
    }
    PEARL_Barrier();

    while (event != trace.end())
    {
        // Perform global time offset shift
        event->setTimestamp(event->getTimestamp() - offset);

        // ENTER:
        if (event->isOfType(GROUP_ENTER))
        {
            Enter_rep& enter = event_cast< Enter_rep >(*event);

            // Update current callpath
            // Only read access, so no synchronization necessary
            current = ctree->getCallpath(enter.getRegionEntered(),
                                         enter.getCallsite(),
                                         current);

            // Set callpath pointer
            enter.setCallpath(current);
        }

        // LEAVE:
        else if (event->isOfType(GROUP_LEAVE))
        {
            Leave_rep& leave = event_cast< Leave_rep >(*event);
            leave.setCallpath(current);

            // Update current callpath
            current = current->getParent();
        }

        // THREAD_FORK:
        //  - occurs only on master thread
        //  - immediately before THREAD_TEAM_BEGIN
        else if (event->isOfType(THREAD_FORK))
        {
            // Provide current callpath to worker threads
            sharedCallpath = current;
        }

        // THREAD_TEAM_BEGIN:
        //  - first event on each thread of the team
        else if (event->isOfType(THREAD_TEAM_BEGIN))
        {
            // Synchronize team to ensure that the master thread has stored the
            // parent callpath while processing the preceding THREAD_FORK event
            PEARL_Barrier();

            // Get parent callpath from master thread
            current = sharedCallpath;

            // Synchronize team again to avoid data races
            // (As there is no synchronization at the end of a thread team, the
            // master could potentially already process the next THREAD_FORK
            // event and overwrite the parent callpath variable before worker
            // threads had a chance read it.)
            PEARL_Barrier();
        }

        else if (event->isOfType(THREAD_BEGIN))
        {
            // store THREADS node as parent for this thread function
            current = ctree->getCallpath(defs.getThreadRootRegion(),
                                         Callsite::UNDEFINED,
                                         programCallpath);
        }

        else if (event->isOfType(THREAD_TASK_SWITCH))
        {
            // keep track of previous task
            previousTaskId = currentTaskId;
            ThreadTaskSwitch_rep& taskSwitch = event_cast< ThreadTaskSwitch_rep >(*event);
            currentTaskId = taskSwitch.getTaskId();

            // store current callpath progress
            activeTasks[previousTaskId] = current;

            // unvisited task
            if (activeTasks.count(currentTaskId) == 0)
            {
                // store task_root node as parent for this task
                current = ctree->getCallpath(defs.getTaskRootRegion(),
                                             Callsite::UNDEFINED,
                                             programCallpath);
            }
            else
            {
                // continue the callpath stored for this taskId
                current = activeTasks[currentTaskId];
            }
        }

        else if (event->isOfType(THREAD_TASK_COMPLETE))
        {
            // fall back to the implicit task
            currentTaskId = trace.get_location().getId();    // until the next task switch
            current       = activeTasks[currentTaskId];

            // task complete remove data to minimize resources
            ThreadTaskComplete_rep& taskComplete = event_cast< ThreadTaskComplete_rep >(*event);
            activeTasks.erase(taskComplete.getTaskId());
        }

        else if (event->isOfType(GROUP_NONBLOCK))
        {
            // Precompute next/previous request offset
            uint64_t requestId = event->getRequestId();

            std::map< uint64_t, Event >::iterator rmapit = requestMap.find(requestId);
            if (rmapit != requestMap.end())
            {
                rmapit->second->set_next_reqoffs(event.get_id() - rmapit->second.get_id());
                event->set_prev_reqoffs(event.get_id() - rmapit->second.get_id());

                if (event->isOfType(MPI_REQUEST_TESTED))
                {
                    rmapit->second = event;
                }
                else
                {
                    requestMap.erase(rmapit);
                }
            }
            else
            {
                requestMap.insert(std::make_pair(requestId, event));
            }
        }
        ++event;
    }

    // Synchronize threads
    PEARL_Barrier();
}


// --- Local helper functions -----------------------------------------------

namespace
{
/// @brief Determine process-local timestamp offset
///
/// Determines the earliest event timestamp across all thread-local event
/// trace streams of a process (passed as @a trace) and updates the
/// process-local timestamp offset stored in the global definitions object
/// @a defs accordingly.  In a multi-process setup, the global timestamp
/// offset will be calculated during PEARL_mpi_unify_calltree().
///
/// @param defs
///     Global definitions object
/// @param trace
///     Local trace data object
///
void
determineTimeOffset(MutableGlobalDefs& defs,
                    const LocalTrace&  trace)
{
    // Get first event in stream
    Event event = trace.begin();

    // Determine its timestamp
    timestamp_t localOffset = DBL_MAX;
    if (event != trace.end())
    {
        localOffset = event->getTimestamp();
    }

    // Update process-local timestamp offset if necessary
    calltreeMutex.lock();
    if (localOffset < defs.getGlobalOffset())
    {
        defs.setGlobalOffset(localOffset);
    }
    calltreeMutex.unlock();
}


/// @brief Determine the corresponding call path object in another call tree
///
/// Determines the call path object in @a calltree which corresponds to the
/// given input @a callpath.  It also ensures that all ancestor call paths
/// exist (if not, they are implicitly created).
///
/// @param calltree
///     Pointer to the target call tree object
/// @param callpath
///     Pointer to the input call path
/// @returns
///     Pointer to the corresponding call path in @a calltree
///
Callpath*
mapCallpath(CallTree* const       calltree,
            const Callpath* const callpath)
{
    // Stop recursion when reaching a top-level node
    if (callpath == NULL)
    {
        return NULL;
    }

    // Recursively ensure that ancestor call paths exist
    Callpath* parent = mapCallpath(calltree, callpath->getParent());

    return calltree->getCallpath(callpath->getRegion(),
                                 callpath->getCallsite(),
                                 parent);
}
}    // unnamed namespace
