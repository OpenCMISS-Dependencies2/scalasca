/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "AnalyzeTask.h"

#include <algorithm>
#include <cassert>
#include <cstddef>

#define SCALASCA_DEBUG_MODULE_NAME    TRIGGER
#include <UTILS_Debug.h>
#include <cubew_cube.h>

#include <pearl/ReplayTask.h>
#include <pearl/pearl_replay.h>

#include "MpiCommunicationHandler.h"
#include "OmpEventHandler.h"
#include "Pattern.h"
#include "Patterns_gen.h"
#include "ReplayControlHandler.h"
#include "ReportData.h"
#include "StatisticsTask.h"
#include "SynchpointHandler.h"
#include "TaskData.h"
#include "TimedPhase.h"
#include "TmapCacheHandler.h"
#include "user_events.h"

using namespace std;
using namespace pearl;
using namespace scout;

extern bool enableCriticalPath;
extern bool enableDelayAnalysis;
extern bool enableSinglePass;


// --------------------------------------------------------------------------
//
//  class AnalysisReplayTask
//
// --------------------------------------------------------------------------

namespace scout
{
class AnalysisReplayTask
    : public pearl::ReplayTask
{
    public:
        typedef enum
        {
            forward, backward
        } direction_t;


    private:
        direction_t mDirection;
        int         mPrepareEvent;
        int         mFinishEvent;

        bool
        prepare()
        {
            UTILS_DLOG << "Prepare replay";

            m_cbmanager->notify(mPrepareEvent, m_trace.end(), m_cbdata);

            return true;
        }

        bool
        finish()
        {
            UTILS_DLOG << "Finished replay";

            m_cbmanager->notify(mFinishEvent, m_trace.end(), m_cbdata);

            return true;
        }

        bool
        replay()
        {
            if (mDirection == forward)
            {
                PEARL_forward_replay(m_trace, *m_cbmanager, m_cbdata);

                return true;
            }
            else if (mDirection == backward)
            {
                PEARL_backward_replay(m_trace, *m_cbmanager, m_cbdata);

                return true;
            }

            return false;
        }


    public:
        AnalysisReplayTask(AnalyzeTask*     parent,
                           CallbackManager* cbmgr,
                           direction_t      direction,
                           int              prepevt,
                           int              finevt)
            : ReplayTask(*(parent->mSharedData.mDefinitions),
                         *(parent->mPrivateData.mTrace),
                         &(parent->mCallbackData),
                         cbmgr),
              mDirection(direction),
              mPrepareEvent(prepevt),
              mFinishEvent(finevt)
        {
        }
};
}    // namespace scout


// --------------------------------------------------------------------------
//
//  class AnalyzeTask
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new analysis task.
 *
 *  This constructor creates a new analysis task instance using the given
 *  parameters.
 *
 *  @param  sharedData   Shared task data object
 *  @param  privateData  Private task data object
 **/
AnalyzeTask::AnalyzeTask(const TaskDataShared&  sharedData,
                         const TaskDataPrivate& privateData)
    : CompoundTask(),
      mSharedData(sharedData),
      mPrivateData(privateData),
      mStatistics(NULL)
{
}


/**
 *  @brief Destructor. Cleanup.
 *
 **/
AnalyzeTask::~AnalyzeTask()
{
    for (PatternList::iterator it = mPatterns.begin();
         it != mPatterns.end();
         ++it)
    {
        delete *it;
    }
}


// --- Adding performance patterns ------------------------------------------

/**
 *  @brief Adds a performance pattern object to the task.
 *
 *  This method adds the given performance @a pattern object to the task and
 *  implicitely registers it's callbacks. Adding a pattern to the task also
 *  leads to it's inclusion in the final analysis report.
 **/
void
AnalyzeTask::addPattern(Pattern* pattern)
{
    // Add pattern to list and register callbacks
    mPatterns.push_back(pattern);
    pattern->reg_cb(mCallbackManagers);
}


// --- Register statistics task ---------------------------------------------

/**
 *  @brief Registers a statistics task with this analysis task.
 *
 *  This method registers the given statistics task @p sttask with this
 *  analysis task. Before starting the analysis phase, the callbacks of
 *  this statistics task will be registered, so that the individual analysis
 *  tasks can trigger those to centrally collect various statistics.
 *
 *  @param  sttask  Pointer to statistics task to register
 **/
void
AnalyzeTask::register_statistics_cbs(StatisticsTask* sttask)
{
    assert(sttask);

    mStatistics = sttask;
}


// --- Retrieve report data -------------------------------------------------

/**
 *  @brief Generates the performance metric information.
 *
 *  This method defines the CUBE performance metric hierarchy in the CUBE
 *  object passed via @a data based on the information provided by the
 *  individual pattern classes.
 *
 *  @param  data  Report data structure
 **/
void
AnalyzeTask::genCubeMetrics(ReportData& data) const
{
    // Create performance metrics
    for (PatternList::const_iterator it = mPatterns.begin();
         it != mPatterns.end();
         ++it)
    {
        cube_metric* cmetric = 0;

        if (!(*it)->is_hidden())
        {
            // Determine parent
            long         parent_id = (*it)->get_parent();
            cube_metric* cparent   = 0;

            if (parent_id != PAT_NONE)
            {
                cparent = data.metrics[parent_id];
            }

            // Create metric
            cmetric = cube_def_met(data.cb,
                                   (*it)->get_name().c_str(),
                                   (*it)->get_unique_name().c_str(),
                                   (*it)->get_unit() == "sec"
                                   ? "FLOAT"
                                   : "INTEGER",
                                   (*it)->get_unit().c_str(),
                                   "",
                                   (*it)->get_url().c_str(),
                                   (*it)->get_descr().c_str(),
                                   cparent,
                                   (*it)->get_mode());
        }

        data.metrics[(*it)->get_id()] = cmetric;
    }
}


/**
 *  @brief Collates all pattern severities and writes the corresponding
 *         section in the CUBE report.
 *
 *  Generates the severity section of the CUBE report for all patterns. The
 *  actual collation and writing is done by the individual pattern classes.
 *
 *  @param  data   Temporary data used for report writing
 *  @param  rank   Global process rank (MPI rank or 0)
 **/
void
AnalyzeTask::genCubeSeverities(ReportData& data,
                               int         rank) const
{
    for (PatternList::const_iterator it = mPatterns.begin();
         it != mPatterns.end();
         ++it)
    {
        if (  !(*it)->is_hidden()
           || ((*it)->get_name() == "STATISTICS"))
        {
            (*it)->gen_severities(data, rank, *mPrivateData.mTrace);
        }
    }
}


// --- Execution control (protected) ----------------------------------------

/**
 *  @brief Task preparation.
 *
 *  The prepare() callback method is called before execution of the subtasks
 *  starts. In case of an analyze task, the sub-tasks are created, initialized
 *  and added to the analysis task.
 *
 *  @return @em true if all sub-tasks could be initialized, @em false otherwise
 **/
bool
AnalyzeTask::prepare()
{
    mCallbackData.mDefs  = mSharedData.mDefinitions;
    mCallbackData.mTrace = mPrivateData.mTrace;

    // *INDENT-OFF*
    const struct replay
    {
        const char*                     name;
        const char*                     descr;
        AnalysisReplayTask::direction_t direction;
    }
    full_replays[] = {
        { "",    "\n  Wait-state detection (fwd)     ", AnalysisReplayTask::forward  },
        { "bws",   "  Wait-state detection (bwd)     ", AnalysisReplayTask::backward },
        { "fws",   "  Synchpoint exchange            ", AnalysisReplayTask::forward  },
        { "bwc",   "  Critical-path & delay analysis ", AnalysisReplayTask::backward },
        { "fwc",   "  Propagating wait-state exchange", AnalysisReplayTask::forward  },

        { 0,     0,                                     AnalysisReplayTask::forward  }
    },
    basic_replays[] = {
        { "",    "\n  Wait-state detection (fwd)     ", AnalysisReplayTask::forward  },
        { "bws",   "  Wait-state detection (bwd)     ", AnalysisReplayTask::backward },

        { 0,     0,                                     AnalysisReplayTask::forward  }
    },
    single_pass_replays[] = {
        { "",    "\n  Wait-state detection (fwd)     ", AnalysisReplayTask::forward  },

        { 0,     0,                                     AnalysisReplayTask::forward  }
    };
    // *INDENT-ON*


    // --- Setup callback manager map

    for (const struct replay* r = full_replays; r->name; ++r)
    {
        mCallbackManagers[r->name] = new CallbackManager;
    }

    // --- Create pattern-orthogonal callback handlers. Note: the order is important!

    mHandlers.push_back(new ReplayControlHandler);

    #ifdef _MPI
        #pragma omp master
        {
            // MPI communication is only handled on the master thread
            mHandlers.push_back(new MpiCommunicationHandler);
        }
    #endif
    #ifdef _OPENMP
        mCallbackData.mOmpEventHandler = new OmpEventHandler;
        mHandlers.push_back(mCallbackData.mOmpEventHandler);
    #endif

    if (enableCriticalPath || enableDelayAnalysis)
    {
        // Synchpoint detection and timemap cache are only needed
        // for critical-path and delay analysis
        mCallbackData.mSynchpointHandler = new SynchpointHandler;
        mCallbackData.mTmapCacheHandler  = new TmapCacheHandler;

        mHandlers.push_back(mCallbackData.mSynchpointHandler);
        mHandlers.push_back(mCallbackData.mTmapCacheHandler);
    }

    // --- Register patterns and handlers

    // Register pre-pattern callbacks
    for (HandlerList::iterator it = mHandlers.begin();
         it != mHandlers.end();
         ++it)
    {
        (*it)->register_pre_pattern_callbacks(mCallbackManagers);
    }

    // Register pattern callbacks
    create_patterns(this);

    mStatistics->registerAnalysisCallbacks(mCallbackManagers);

    // Register post-pattern callbacks
    for (HandlerList::iterator it = mHandlers.begin();
         it != mHandlers.end();
         ++it)
    {
        (*it)->register_post_pattern_callbacks(mCallbackManagers);
    }

    // --- Setup replay tasks

    const struct replay* stages = basic_replays;

    if (enableCriticalPath || enableDelayAnalysis)
    {
        stages = full_replays;
    }
    if (enableSinglePass)
    {
        stages = single_pass_replays;
    }

    for (const struct replay* r = stages; r->name; ++r)
    {
        CallbackManagerMap::iterator cbmgrIt = mCallbackManagers.find(r->name);
        assert(cbmgrIt != mCallbackManagers.end());

        AnalysisReplayTask* rpltask =
            new AnalysisReplayTask(this, cbmgrIt->second, r->direction, PREPARE, FINISHED);

        assert(rpltask != 0);

        add_task(TimedPhase::make_timed(rpltask, r->descr, 1, true));
    }

    return true;
}


/**
 *  @brief Task cleanup.
 *
 *  The finish() callback method is called when the execution of the subtasks
 *  has finished. Do some cleanup here.
 *
 *  @return @em true (always)
 **/
bool
AnalyzeTask::finish()
{
    // callback managers will be deleted by sub-replay task destructors
    mCallbackManagers.clear();

    mCallbackData.mSynchpointHandler = 0;
    mCallbackData.mTmapCacheHandler  = 0;
    mCallbackData.mOmpEventHandler   = 0;

    for (HandlerList::iterator it = mHandlers.begin();
         it != mHandlers.end();
         ++it)
    {
        delete *it;
    }

    mHandlers.clear();

    return true;
}
