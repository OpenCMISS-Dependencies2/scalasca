/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2019-2021                                                **
**  RWTH Aachen University, IT Center                                      **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "Patterns_gen.h"

#include <inttypes.h>

#include <cassert>

#include <pearl/CallbackManager.h>
#include <pearl/Callpath.h>
#include <pearl/GlobalDefs.h>

#include "AnalyzeTask.h"
#include "CbData.h"
#include "MpiPattern.h"

#ifdef _OPENMP
    #include "OmpPattern.h"
    #include "PthreadPattern.h"
#endif    // _OPENMP

using namespace std;
using namespace pearl;
using namespace scout;


#line 19 "Generic.pattern"

#include <pearl/Leave_rep.h>
#include <pearl/ThreadEnd_rep.h>

#include "Callstack.h"

#line 17 "Async.pattern"

#include <iostream>

#include <pearl/AmListenerFactory.h>
#include <pearl/AmRequestFactory.h>
#include <pearl/Communicator.h>

#if defined(_MPI)
    #include <pearl/AmRuntime.h>
    #include <pearl/MpiAmListener.h>
    #include <pearl/MpiAmRequest.h>

    #include "AmLockContention.h"
    #include "AmSeverityExchange.h"
    #include "AmSyncpointExchange.h"
    #include "AmWaitForProgressRequest.h"
    #include "AmWaitForProgressResponse.h"
#endif    // _MPI


extern bool enableAsynchronous;

#line 22 "MPI.pattern"

#include <cfloat>
#include <list>

#define SCALASCA_DEBUG_MODULE_NAME    SCOUT
#include <UTILS_Debug.h>

#include <pearl/CallbackManager.h>
#include <pearl/Region.h>
#include <pearl/String.h>

#include "Roles.h"
#include "scout_types.h"
#include "user_events.h"

#if defined(_MPI)
    #include <pearl/EventSet.h>
    #include <pearl/LocalData.h>
    #include <pearl/MpiCollEnd_rep.h>
    #include <pearl/MpiComm.h>
    #include <pearl/MpiMessage.h>
    #include <pearl/RemoteData.h>
    #include <pearl/RemoteEventSet.h>

    #include "MpiDatatypes.h"
    #include "MpiOperators.h"
#endif    // _MPI

#line 19 "OMP.pattern"

#if defined(_OPENMP)
    namespace
    {
    int omp_nest_level = 0;
    }    // unnamed namespace
#endif   // _OPENMP

#line 19 "Thread.pattern"

#include "LockTracking.h"

#line 20 "MPI_RMA.pattern"

#include <algorithm>
#include <cfloat>
#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>

#if defined(_MPI)
    #include <pearl/AmRequestFactory.h>
    #include <pearl/MpiGroup.h>
    #include <pearl/MpiWindow.h>
    #include <pearl/RmaEpoch.h>

    #include "AmWaitForProgressRequest.h"
    #include "LockEpochQueue.h"
    #include "MpiDatatypes.h"
    #include "MpiOperators.h"
    #include "Predicates.h"
#endif    // _MPI

#line 17 "Statistics.pattern"

#include <sys/stat.h>

#include <cstdio>
#include <cstring>

#include <UTILS_Error.h>

#include <pearl/pearl_replay.h>

#include "LockTracking.h"
#include "Quantile.h"
#include "ReportData.h"

#if defined(_MPI)
    #include "MpiDatatypes.h"
    #include "MpiOperators.h"
#endif    // _MPI
#if defined(_OPENMP)
    #include <omp.h>
#endif    // _OPENMP


/// Maximum number of coefficients used for quantile approximation
#define NUMBER_COEFF    60

extern string archiveDirectory;
extern bool   enableStatistics;

/* For debugging: #define WRITE_CONTROL_VALUES */

#line 17 "CriticalPath.pattern"

// #define CPADEBUG

#include <algorithm>
#include <numeric>

#include "DelayOps.h"
#include "SynchpointHandler.h"
#include "TmapCacheHandler.h"

#ifdef _OPENMP
    #include <omp.h>

    #include "OmpEventHandler.h"
#endif    // _OPENMP
#ifdef CPADEBUG
    #include <fstream>
    #include <sstream>
#endif    // CPADEBUG


extern bool enableCriticalPath;

#line 17 "Delay.MPI.pattern"

// #define DELAYDEBUG

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/Location.h>

#include "DelayCostMap.h"
#include "DelayOps.h"
#include "user_events.h"

#ifdef _MPI
    #include <pearl/LocalData.h>
    #include <pearl/RemoteData.h>

    #include "MpiDelayOps.h"
#endif    // _MPI
#ifdef DELAYDEBUG
    #include <fstream>
    #include <sstream>
#endif    // DELAYDEBUG


extern bool enableDelayAnalysis;

#line 17 "Delay.OMP.pattern"

// #define OMPDELAYDEBUG

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/Location.h>

#include "DelayCostMap.h"
#include "DelayOps.h"
#include "user_events.h"

#ifdef _OPENMP
    #include "OmpDelayOps.h"
#endif    // _OPENMP
#ifdef OMPDELAYDEBUG
    #include <fstream>
    #include <sstream>
#endif    // OMPDELAYDEBUG


extern bool enableDelayAnalysis;

#line 17 "Waitstates.MPI.pattern"

extern bool enableDelayAnalysis;


/*-------------------------------------------------------------------------*/
/**
 *  @class PatternTime
 *  @brief Time metric.
 *
 *  Implements the "Time" metric.
 */
/*-------------------------------------------------------------------------*/

class PatternTime
    : public Pattern
{
    public:
        /// @name Registering callbacks
        /// @{

        virtual void
        reg_cb(CallbackManagerMap& cbmanagers);

        /// @}
        /// @name Retrieving pattern information
        /// @{

        virtual long
        get_id() const
        {
            return PAT_TIME;
        }

        virtual long
        get_parent() const
        {
            return PAT_NONE;
        }

        virtual string
        get_name() const
        {
            return "Time";
        }

        virtual string
        get_unique_name() const
        {
            return "time";
        }

        virtual string
        get_descr() const
        {
            return "Total CPU allocation time (includes time allocated"
                   " for idle threads)";
        }

        virtual string
        get_unit() const
        {
            return "sec";
        }

        virtual CubeMetricType
        get_mode() const
        {
            return CUBE_METRIC_INCLUSIVE;
        }

        /// @}
        /// @name Callback methods
        /// @{

        void
        finished__cb(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          cdata);

        void
        group_leave__cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        omp_mgmt_fork__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

        void
        omp_mgmt_join__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

        void
        thread_task_complete__cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

        /// @}
};


// --- Registering callbacks ------------------------------------------------

void
PatternTime::reg_cb(CallbackManagerMap& cbmanagers)
{
    init();

    CallbackManagerMap::iterator    it;
    CallbackManagerMap::mapped_type cbmanager;

    // Register main callbacks
    it = cbmanagers.find("");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        GROUP_LEAVE,
        PEARL_create_callback(
            this,
            &PatternTime::group_leave__cb));
    cbmanager->register_callback(
        THREAD_TASK_COMPLETE,
        PEARL_create_callback(
            this,
            &PatternTime::thread_task_complete__cb));
    cbmanager->register_callback(
        OMP_MGMT_FORK,
        PEARL_create_callback(
            this,
            &PatternTime::omp_mgmt_fork__cb));
    cbmanager->register_callback(
        OMP_MGMT_JOIN,
        PEARL_create_callback(
            this,
            &PatternTime::omp_mgmt_join__cb));
    cbmanager->register_callback(
        FINISHED,
        PEARL_create_callback(
            this,
            &PatternTime::finished__cb));
}


// --- Callback methods -----------------------------------------------------

void
PatternTime::finished__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 78 "Generic.pattern"

    // Combined loop over all callpaths to:
    // (1) Inclusify profile of non-master threads outside of parallel regions
    // (2) Fix the task root value by attributing its children

    uint32_t thread_id = data->mTrace->get_location().getThreadId();

    uint32_t count = data->mDefs->numCallpaths();
    for (uint32_t id = 0; id < count; ++id)
    {
        Callpath* callpath = data->mDefs->get_cnode(id);

        // Fix severity of artificial TASKS/THREADS callpath
        if (  (callpath->getRegion() == data->mDefs->getTaskRootRegion())
           || (callpath->getRegion() == data->mDefs->getThreadRootRegion()))
        {
            for (uint32_t i = 0; i < callpath->numChildren(); ++i)
            {
                uint32_t childId = callpath->getChild(i).getId();
                m_severity[callpath] += m_severity[data->mDefs->get_cnode(childId)];
            }

            // Propagate severity of artificial THREADS callpath to
            // program callpath if existent
            if (callpath->getRegion() == data->mDefs->getThreadRootRegion())
            {
                Callpath* parent = callpath->getParent();
                if (parent)
                {
                    m_severity[parent] += m_severity[callpath];
                }
            }
        }

        // skip in case of thread 0
        if (thread_id != 0)
        {
            // Does the callpath refer to an OpenMP parallel region?
            if (!is_omp_parallel(callpath->getRegion()))
            {
                continue;
            }

            // Propagate severity value "upwards"
            double    value  = m_severity[callpath];
            Callpath* parent = callpath->getParent();
            while (parent)
            {
                m_severity[parent] += value;
                parent              = parent->getParent();
            }
        }
    }
}


void
PatternTime::group_leave__cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 55 "Generic.pattern"

    Event     enter = data->mCallstack->top();
    Callpath* cnode = enter.get_cnode();

    m_severity[cnode] += event->getTimestamp() - enter->getTimestamp() - data->mCallstack->getCurrentTimeOffset();
}


void
PatternTime::omp_mgmt_fork__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 70 "Generic.pattern"

    m_severity[event.get_cnode()] += data->mIdle;
}


void
PatternTime::omp_mgmt_join__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 74 "Generic.pattern"

    m_severity[event.get_cnode()] += data->mIdle;
}


void
PatternTime::thread_task_complete__cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 62 "Generic.pattern"

    // Updating stub nodes in calling tree (only for the implicit task)
    data->mIdle                                                            = event->getTimestamp() - data->mCallstack->getStartTime() - data->mCallstack->getTotalTimeOffset();
    m_severity[data->mDefs->get_cnode(data->mCallstack->getStubNodeId())] += data->mIdle;

    cbmanager.notify(TASK_CORRECTION, event, data);
}


/*-------------------------------------------------------------------------*/
/**
 *  @class PatternVisits
 *  @brief Visits metric.
 *
 *  Implements the "Visits" metric.
 */
/*-------------------------------------------------------------------------*/

class PatternVisits
    : public Pattern
{
    public:
        /// @name Registering callbacks
        /// @{

        virtual void
        reg_cb(CallbackManagerMap& cbmanagers);

        /// @}
        /// @name Retrieving pattern information
        /// @{

        virtual long
        get_id() const
        {
            return PAT_VISITS;
        }

        virtual long
        get_parent() const
        {
            return PAT_NONE;
        }

        virtual string
        get_name() const
        {
            return "Visits";
        }

        virtual string
        get_unique_name() const
        {
            return "visits";
        }

        virtual string
        get_descr() const
        {
            return "Number of visits";
        }

        virtual string
        get_unit() const
        {
            return "occ";
        }

        virtual CubeMetricType
        get_mode() const
        {
            return CUBE_METRIC_EXCLUSIVE;
        }

        /// @}
        /// @name Callback methods
        /// @{

        void
        group_enter__cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        thread_task_complete__cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

        /// @}
};


// --- Registering callbacks ------------------------------------------------

void
PatternVisits::reg_cb(CallbackManagerMap& cbmanagers)
{
    init();

    CallbackManagerMap::iterator    it;
    CallbackManagerMap::mapped_type cbmanager;

    // Register main callbacks
    it = cbmanagers.find("");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        GROUP_ENTER,
        PEARL_create_callback(
            this,
            &PatternVisits::group_enter__cb));
    cbmanager->register_callback(
        THREAD_TASK_COMPLETE,
        PEARL_create_callback(
            this,
            &PatternVisits::thread_task_complete__cb));
}


// --- Callback methods -----------------------------------------------------

void
PatternVisits::group_enter__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata)
{
    #line 173 "Generic.pattern"

    ++m_severity[event.get_cnode()];
}


void
PatternVisits::thread_task_complete__cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 177 "Generic.pattern"

    // updating visits for the task stub nodes on the implicit task
    // current mode: one stub node per instance and one visit per instance
    ++m_severity[data->mDefs->get_cnode(data->mCallstack->getStubNodeId())];
}


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternAsync
     *  @brief Asyncronous Driver metric.
     *
     *  Implements the "Asyncronous Driver" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternAsync
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_ASYNC;
            }

            virtual long
            get_parent() const
            {
                return PAT_NONE;
            }

            virtual string
            get_name() const
            {
                return "ASYNC";
            }

            virtual string
            get_unique_name() const
            {
                return "async";
            }

            virtual string
            get_descr() const
            {
                return "Driver for asynchronous pattern detection";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            virtual bool
            is_hidden() const
            {
                return true;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            finished__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            void
            prepare__cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternAsync::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PREPARE,
            PEARL_create_callback(
                this,
                &PatternAsync::prepare__cb));
        cbmanager->register_callback(
            FINISHED,
            PEARL_create_callback(
                this,
                &PatternAsync::finished__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternAsync::finished__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata)
    {
        #line 87 "Async.pattern"

        AmRuntime::getInstance().shutdown();
    }


    void
    PatternAsync::prepare__cb(const CallbackManager& cbmanager,
                              int                    user_event,
                              const Event&           event,
                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 59 "Async.pattern"

        pearl::AmRuntime::getInstance().setCbdata(data);
        pearl::AmRuntime::getInstance().setCbmanager(cbmanager);

        pearl::AmRuntime::getInstance().attach(AmLockContention::getInstance());
        pearl::AmRuntime::getInstance().attach(AmSeverityExchange::getInstance());
        pearl::AmRuntime::getInstance().attach(AmSyncpointExchange::getInstance());
        pearl::AmRuntime::getInstance().attach(AmWaitForProgressRequest::getInstance());
        pearl::AmRuntime::getInstance().attach(AmWaitForProgressResponse::getInstance());

        #if defined(_MPI)
            // Register MPI specific factory callbacks
            pearl::AmRequestFactory::registerCallback(Paradigm::MPI,
                                                      &MpiAmRequest::create);
            pearl::AmListenerFactory::registerCallback(Paradigm::MPI,
                                                       &MpiAmListener::create);
        #endif

        #if defined(_ARMCI)
            // Register ARMCI specific factory callbacks
            pearl::AmRequestFactory::registerCallback(Paradigm::ARMCI,
                                                      &MpiAmRequest::create);
            pearl::AmListenerFactory::registerCallback(Paradigm::ARMCI,
                                                       &MpiAmListener::create);
        #endif
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_InitCompletion
     *  @brief MPI Initialization Completion Time metric.
     *
     *  Implements the "MPI Initialization Completion Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_InitCompletion
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_INIT_COMPLETION;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_INIT_EXIT;
            }

            virtual string
            get_name() const
            {
                return "MPI Initialization Completion";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_init_completion";
            }

            virtual string
            get_descr() const
            {
                return "Time needed to finish MPI initialization";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            init__cb(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          cdata);

            void
            pre_init__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_InitCompletion::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_INIT,
            PEARL_create_callback(
                this,
                &PatternMPI_InitCompletion::pre_init__cb));
        cbmanager->register_callback(
            INIT,
            PEARL_create_callback(
                this,
                &PatternMPI_InitCompletion::init__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_InitCompletion::init__cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 176 "MPI.pattern"

        if (data->mCompletion > 0)
        {
            Callpath* const callpath = event.get_cnode();

            m_severity[callpath] += data->mCompletion;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << get_name() << " @ " << *callpath
                << ": " << data->mCompletion
                << " [total: " << m_severity[callpath] << ']';
        }

        // There will always be completion time at MPI_Init; notify all
        // processes
        cbmanager.notify(INIT_COMPL, event, data);
    }


    void
    PatternMPI_InitCompletion::pre_init__cb(const CallbackManager& cbmanager,
                                            int                    user_event,
                                            const Event&           event,
                                            CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 152 "MPI.pattern"

        const CollectiveInfo& ci = data->mCollinfo;

        // Validate clock condition
        const timestamp_t localEndT = event->getTimestamp();
        if (ci.latest.mTime > localEndT)
        {
            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CLOCK_VIOLATION)
                << "Clock violation @ " << *event.get_cnode()
                << ": " << (ci.latest.mTime - localEndT);

            cbmanager.notify(CCV_COLL, event, data);
        }

        // Restrict wait & completion time to time spent in operation
        const timestamp_t latestBeginT = min(ci.latest.mTime, localEndT);
        const timestamp_t earliestEndT = max(ci.earliest_end.mTime, latestBeginT);

        // Calculate completion time, but no wait time.  Otherwise, delay
        // costs may be assigned to MPI_Init(_thread) to propagate further,
        // which are then never assigned to any call path.
        data->mCompletion = max(0.0, localEndT - earliestEndT);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_FinalizeWait
     *  @brief Wait at MPI Finalize Time metric.
     *
     *  Implements the "Wait at MPI Finalize Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_FinalizeWait
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_FINALIZE_WAIT;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_INIT_EXIT;
            }

            virtual string
            get_name() const
            {
                return "MPI Wait at Finalize";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_finalize_wait";
            }

            virtual string
            get_descr() const
            {
                return "Waiting time in front of MPI_Finalize";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            finalize__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            void
            pre_finalize__cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_FinalizeWait::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_FINALIZE,
            PEARL_create_callback(
                this,
                &PatternMPI_FinalizeWait::pre_finalize__cb));
        cbmanager->register_callback(
            FINALIZE,
            PEARL_create_callback(
                this,
                &PatternMPI_FinalizeWait::finalize__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_FinalizeWait::finalize__cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 243 "MPI.pattern"

        if (data->mIdle > 0)
        {
            Callpath* const callpath = event.get_cnode();

            m_severity[callpath] += data->mIdle;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << get_name() << " @ " << *callpath
                << ": " << data->mIdle
                << " [total: " << m_severity[callpath] << ']';
        }

        // There will always be waiting time at MPI_Finalize; notify all
        // processes
        cbmanager.notify(WAIT_FINALIZE, event, data);
    }


    void
    PatternMPI_FinalizeWait::pre_finalize__cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 220 "MPI.pattern"

        const CollectiveInfo& ci = data->mCollinfo;

        // Validate clock condition
        const timestamp_t localEndT = event->getTimestamp();
        if (ci.latest.mTime > localEndT)
        {
            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CLOCK_VIOLATION)
                << "Clock violation @ " << *event.get_cnode()
                << ": " << (ci.latest.mTime - localEndT);

            cbmanager.notify(CCV_COLL, event, data);
        }

        // Restrict wait & completion time to time spent in operation
        const timestamp_t latestBeginT = min(ci.latest.mTime, localEndT);
        const timestamp_t earliestEndT = max(ci.earliest_end.mTime, latestBeginT);

        // Calculate wait & completion time
        data->mIdle       = max(0.0, latestBeginT - ci.my.mTime);
        data->mCompletion = max(0.0, localEndT - earliestEndT);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_SyncCollective
     *  @brief MPI Collective Synchronization Time metric.
     *
     *  Implements the "MPI Collective Synchronization Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_SyncCollective
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_SYNC_COLLECTIVE;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_SYNCHRONIZATION;
            }

            virtual string
            get_name() const
            {
                return "MPI Collective Synchronization";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_sync_collective";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in MPI barriers";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            virtual bool
            is_hidden() const
            {
                return true;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            pre_sync_coll__cb(const CallbackManager& cbmanager,
                              int                    user_event,
                              const Event&           event,
                              CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_SyncCollective::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_SYNC_COLL,
            PEARL_create_callback(
                this,
                &PatternMPI_SyncCollective::pre_sync_coll__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_SyncCollective::pre_sync_coll__cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 387 "MPI.pattern"

        const CollectiveInfo& ci = data->mCollinfo;

        // Validate clock condition
        const timestamp_t localEndT = event->getTimestamp();
        if (ci.latest.mTime > localEndT)
        {
            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CLOCK_VIOLATION)
                << "Clock violation @ " << *event.get_cnode()
                << ": " << (ci.latest.mTime - localEndT);

            cbmanager.notify(CCV_COLL, event, data);
        }

        // Restrict wait & completion time to time spent in operation
        const timestamp_t latestBeginT = min(ci.latest.mTime, localEndT);
        const timestamp_t earliestEndT = max(ci.earliest_end.mTime, latestBeginT);

        // Calculate wait & completion time
        data->mIdle       = max(0.0, latestBeginT - ci.my.mTime);
        data->mCompletion = max(0.0, localEndT - earliestEndT);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_BarrierWait
     *  @brief Wait at MPI Barrier Time metric.
     *
     *  Implements the "Wait at MPI Barrier Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_BarrierWait
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_BARRIER_WAIT;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_SYNC_COLLECTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Wait at Barrier";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_barrier_wait";
            }

            virtual string
            get_descr() const
            {
                return "Waiting time in front of MPI barriers";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            sync_coll__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_BarrierWait::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            SYNC_COLL,
            PEARL_create_callback(
                this,
                &PatternMPI_BarrierWait::sync_coll__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_BarrierWait::sync_coll__cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 438 "MPI.pattern"

        if (data->mIdle > 0)
        {
            Callpath* const callpath = event.get_cnode();

            m_severity[callpath] += data->mIdle;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << get_name() << " @ " << *callpath
                << ": " << data->mIdle
                << " [total: " << m_severity[callpath] << ']';
        }

        // There will always be waiting time at barriers; notify all processes
        cbmanager.notify(WAIT_BARRIER, event, data);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_BarrierCompletion
     *  @brief MPI Barrier Completion Time metric.
     *
     *  Implements the "MPI Barrier Completion Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_BarrierCompletion
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_BARRIER_COMPLETION;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_SYNC_COLLECTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Barrier Completion";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_barrier_completion";
            }

            virtual string
            get_descr() const
            {
                return "Time needed to finish MPI barriers";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            sync_coll__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_BarrierCompletion::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            SYNC_COLL,
            PEARL_create_callback(
                this,
                &PatternMPI_BarrierCompletion::sync_coll__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_BarrierCompletion::sync_coll__cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 482 "MPI.pattern"

        if (data->mCompletion > 0)
        {
            Callpath* const callpath = event.get_cnode();

            m_severity[event.get_cnode()] += data->mCompletion;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << get_name() << " @ " << *callpath
                << ": " << data->mCompletion
                << " [total: " << m_severity[callpath] << ']';
        }

        // There will always be completion time at barriers; notify all
        // processes
        cbmanager.notify(BARRIER_COMPL, event, data);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_P2P
     *  @brief MPI Point-to-point Communication Time metric.
     *
     *  Implements the "MPI Point-to-point Communication Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_P2P
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_POINT2POINT;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_COMMUNICATION;
            }

            virtual string
            get_name() const
            {
                return "MPI point-to-point communication";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_point2point";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in MPI point-to-point communication";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            virtual bool
            is_hidden() const
            {
                return true;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            post_recv__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

            void
            pre_recv__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            void
            pre_send__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_P2P::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_SEND,
            PEARL_create_callback(
                this,
                &PatternMPI_P2P::pre_send__cb));
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_P2P::pre_recv__cb));
        cbmanager->register_callback(
            POST_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_P2P::post_recv__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_P2P::post_recv__cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 566 "MPI.pattern"

        // Validate clock condition
        RemoteEvent send = data->mRemote->get_event(ROLE_SEND);
        if (send->getTimestamp() > event->getTimestamp())
        {
            UTILS_DEBUG_PRINTF(SCALASCA_DEBUG_CLOCK_VIOLATION,
                               "Unsynchronized clocks (loc: %" PRIu64 ", reg: %s, diff: %es)!\n",
                               event.get_location().getId(),
                               event.get_cnode()->getRegion().getDisplayName().getCString(),
                               send->getTimestamp() - event->getTimestamp());
            cbmanager.notify(CCV_P2P, event, data);
        }
    }


    void
    PatternMPI_P2P::pre_recv__cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 560 "MPI.pattern"

        data->mLocal->add_event(event, ROLE_RECV);
        data->mLocal->add_event(event.enterptr(), ROLE_ENTER_RECV);
        data->mLocal->add_event(event.leaveptr(), ROLE_LEAVE_RECV);
    }


    void
    PatternMPI_P2P::pre_send__cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 555 "MPI.pattern"

        data->mLocal->add_event(event, ROLE_SEND);
        data->mLocal->add_event(event.enterptr(), ROLE_ENTER_SEND);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_LateSender
     *  @brief MPI Late Sender Time metric.
     *
     *  Implements the "MPI Late Sender Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_LateSender
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_LATESENDER;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_POINT2POINT;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Sender";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_latesender";
            }

            virtual string
            get_descr() const
            {
                return "Time in MPI point-to-point receive operation waiting"
                       " for a message";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            group_leave__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            post_recv__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

            void
            pre_recv__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            void
            pre_send__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            /// @}


        protected:
            /// @name Pattern management
            /// @{

            virtual void
            init()
            {
                #line 620 "MPI.pattern"

                m_max_idle = 0.0;
            }

            /// @}


        private:
            #line 613 "MPI.pattern"

            // *INDENT-OFF*    Uncrustify issue #2789
            double         m_max_idle;
            EventSet       m_receive;
            RemoteEventSet m_send;
            // *INDENT-ON*
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_LateSender::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_SEND,
            PEARL_create_callback(
                this,
                &PatternMPI_LateSender::pre_send__cb));
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_LateSender::pre_recv__cb));
        cbmanager->register_callback(
            POST_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_LateSender::post_recv__cb));
        cbmanager->register_callback(
            GROUP_LEAVE,
            PEARL_create_callback(
                this,
                &PatternMPI_LateSender::group_leave__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_LateSender::group_leave__cb(const CallbackManager& cbmanager,
                                           int                    user_event,
                                           const Event&           event,
                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 693 "MPI.pattern"

        if (  (m_max_idle > 0.0)
           && is_mpi_wait_multi(event.enterptr()->getRegion()))
        {
            m_severity[event.get_cnode()] += m_max_idle;

            Event       recv       = m_receive.get_event(ROLE_RECV);
            RemoteEvent send       = m_send.get_event(ROLE_SEND);
            RemoteEvent leave_send = m_send.get_event(ROLE_LEAVE_SEND_LS);

            data->mIdle = m_max_idle;

            data->mCompletion = 0;
            if (event->getTimestamp() > leave_send->getTimestamp())
            {
                data->mCompletion = event->getTimestamp() - leave_send->getTimestamp();
            }

            data->mLocal->add_event(recv, ROLE_RECV_LS);
            data->mRemote->add_event(send, ROLE_SEND_LS);

            cbmanager.notify(LATE_SENDER, recv, data);
        }

        m_max_idle = 0.0;
    }


    void
    PatternMPI_LateSender::post_recv__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 636 "MPI.pattern"

        RemoteEvent enter_send = data->mRemote->get_event(ROLE_ENTER_SEND_LS);
        RemoteEvent leave_send = data->mRemote->get_event(ROLE_LEAVE_SEND_LS);
        Event       enter_recv = data->mLocal->get_event(ROLE_ENTER_RECV_LS);
        Event       leave_recv = data->mLocal->get_event(ROLE_LEAVE_RECV_LS);

        const Region& region = enter_recv->getRegion();

        if (is_mpi_testx(region))
        {
            return;
        }

        // Validate clock condition
        pearl::timestamp_t max_time = enter_send->getTimestamp();
        if (max_time > leave_recv->getTimestamp())
        {
            // Do not report violation again -- already done in generic P2P code
            max_time = leave_recv->getTimestamp();
        }

        // Calculate waiting time
        data->mIdle = max_time - enter_recv->getTimestamp();
        if (data->mIdle > 0)
        {
            if (is_mpi_wait_multi(region))
            {
                if (data->mIdle > m_max_idle)
                {
                    RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LS);

                    m_receive.clear();
                    m_send.clear();

                    m_max_idle = data->mIdle;
                    m_receive.add_event(event, ROLE_RECV);
                    m_send.add_event(send, ROLE_SEND);
                    m_send.add_event(leave_send, ROLE_LEAVE_SEND_LS);

                    cbmanager.notify(LATE_SENDER_CANDIDATE, event, data);
                }
            }
            else
            {
                m_severity[event.get_cnode()] += data->mIdle;

                data->mCompletion = 0;
                if (leave_recv->getTimestamp() > leave_send->getTimestamp())
                {
                    data->mCompletion = leave_recv->getTimestamp() - leave_send->getTimestamp();
                }

                cbmanager.notify(LATE_SENDER, event, data);
            }
        }
    }


    void
    PatternMPI_LateSender::pre_recv__cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 630 "MPI.pattern"

        data->mLocal->add_event(event, ROLE_RECV_LS);
        data->mLocal->add_event(event.enterptr(), ROLE_ENTER_RECV_LS);
        data->mLocal->add_event(event.leaveptr(), ROLE_LEAVE_RECV_LS);
    }


    void
    PatternMPI_LateSender::pre_send__cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 624 "MPI.pattern"

        data->mLocal->add_event(event, ROLE_SEND_LS);
        data->mLocal->add_event(event.enterptr(), ROLE_ENTER_SEND_LS);
        data->mLocal->add_event(event.leaveptr(), ROLE_LEAVE_SEND_LS);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_LateSenderWO
     *  @brief MPI Late Sender, Wrong Order Time metric.
     *
     *  Implements the "MPI Late Sender, Wrong Order Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_LateSenderWO
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_LATESENDER_WO;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_LATESENDER;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Sender, wrong order";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_latesender_wo";
            }

            virtual string
            get_descr() const
            {
                return "Late Sender situation due to MPI messages received"
                       " in the wrong order";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_sender__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            post_recv__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

            /// @}


        private:
            #line 751 "MPI.pattern"

            static const uint32_t BUFFERSIZE = 100;

            struct LateSender
            {
                LateSender(RemoteEvent send,
                           Event       recv,
                           timestamp_t idle)
                    : m_send(send),
                      m_recv(recv),
                      mIdle(idle)
                {
                }

                RemoteEvent m_send;
                Event       m_recv;
                timestamp_t mIdle;
            };


            typedef std::list< LateSender > LsBuffer;

            LsBuffer m_buffer;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_LateSenderWO::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternMPI_LateSenderWO::late_sender__cb));
        cbmanager->register_callback(
            POST_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_LateSenderWO::post_recv__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_LateSenderWO::late_sender__cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 776 "MPI.pattern"

        // Construct entry
        LateSender item(data->mRemote->get_event(ROLE_SEND_LS),
                        event, data->mIdle);

        // Store entry in buffer
        if (m_buffer.size() == BUFFERSIZE)
        {
            m_buffer.pop_front();
        }
        m_buffer.push_back(item);
    }


    void
    PatternMPI_LateSenderWO::post_recv__cb(const CallbackManager& cbmanager,
                                           int                    user_event,
                                           const Event&           event,
                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 789 "MPI.pattern"

        RemoteEvent send = data->mRemote->get_event(ROLE_SEND);

        // Search for "wrong order" situations
        LsBuffer::iterator it = m_buffer.begin();
        while (it != m_buffer.end())
        {
            if (it->m_send->getTimestamp() > send->getTimestamp())
            {
                double tmp = data->mIdle;

                data->mIdle                         = it->mIdle;
                m_severity[it->m_recv.get_cnode()] += data->mIdle;

                // Store data and notify specializations
                data->mRemote->add_event(it->m_send, ROLE_SEND_LSWO);
                data->mLocal->add_event(it->m_recv, ROLE_RECV_LSWO);
                cbmanager.notify(LATE_SENDER_WO, event, data);

                it = m_buffer.erase(it);

                data->mIdle = tmp;
            }
            else
            {
                ++it;
            }
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_LswoDifferent
     *  @brief MPI Late Sender, Wrong Order Time / Different Sources metric.
     *
     *  Implements the "MPI Late Sender, Wrong Order Time / Different Sources" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_LswoDifferent
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_LSWO_DIFFERENT;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_LATESENDER_WO;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Sender, wrong order (different source)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_lswo_different";
            }

            virtual string
            get_descr() const
            {
                return "Late Sender, Wrong Order situation due to MPI"
                       " messages received from different sources";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_sender_wo__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_LswoDifferent::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_SENDER_WO,
            PEARL_create_callback(
                this,
                &PatternMPI_LswoDifferent::late_sender_wo__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_LswoDifferent::late_sender_wo__cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 846 "MPI.pattern"

        Event recv = data->mLocal->get_event(ROLE_RECV_LSWO);

        if (recv->getSource() != event->getSource())
        {
            m_severity[recv.get_cnode()] += data->mIdle;
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_LswoSame
     *  @brief MPI Late Sender, Wrong Order Time / Same Source metric.
     *
     *  Implements the "MPI Late Sender, Wrong Order Time / Same Source" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_LswoSame
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_LSWO_SAME;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_LATESENDER_WO;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Sender, wrong order (same source)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_lswo_same";
            }

            virtual string
            get_descr() const
            {
                return "Late Sender, Wrong Order situation due to MPI"
                       " messages received from the same source";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_sender_wo__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_LswoSame::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_SENDER_WO,
            PEARL_create_callback(
                this,
                &PatternMPI_LswoSame::late_sender_wo__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_LswoSame::late_sender_wo__cb(const CallbackManager& cbmanager,
                                            int                    user_event,
                                            const Event&           event,
                                            CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 881 "MPI.pattern"

        Event recv = data->mLocal->get_event(ROLE_RECV_LSWO);

        if (recv->getSource() == event->getSource())
        {
            m_severity[recv.get_cnode()] += data->mIdle;
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_LateReceiver
     *  @brief MPI Late Receiver Time metric.
     *
     *  Implements the "MPI Late Receiver Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_LateReceiver
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_LATERECEIVER;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_POINT2POINT;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Receiver";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_latereceiver";
            }

            virtual string
            get_descr() const
            {
                return "Time in MPI point-to-point send operation waiting"
                       " for the receiver to become ready";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_sender_candidate__cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata);

            void
            mpi_send_complete_bws_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            post_send_bws_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            pre_recv_bws_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            pre_send_bws_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        protected:
            /// @name Pattern management
            /// @{

            virtual void
            init()
            {
                #line 945 "MPI.pattern"

                #ifdef DEBUG_MULTI_LR
                    std::ostringstream fnamestr;
                    fnamestr << epk_archive_get_name() << "/dbg_lr." << rank << ".dat";

                    m_dbg_out.open(fnamestr.str().c_str(), std::fstream::app);
                #endif
            }

            /// @}


        private:
            #line 923 "MPI.pattern"

            struct multiwait_info_t
            {
                int    sendcount;
                Event  maxevent;
                double maxidle;
                double maxcmpl;
            };


            typedef std::set< pearl::Event, const pearl::EventKeyCompare >
                event_set_t;
            typedef std::map< Event, multiwait_info_t, pearl::EventKeyCompare >
                multiwait_map_t;

            event_set_t     m_ls_set;
            multiwait_map_t m_multiwait_map;

            #ifdef DEBUG_MULTI_LR
                std::ofstream m_dbg_out;
            #endif
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_LateReceiver::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_SENDER_CANDIDATE,
            PEARL_create_callback(
                this,
                &PatternMPI_LateReceiver::late_sender_candidate__cb));

        // Register bws callbacks
        it = cbmanagers.find("bws");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_SEND,
            PEARL_create_callback(
                this,
                &PatternMPI_LateReceiver::pre_send_bws_cb));
        cbmanager->register_callback(
            POST_SEND,
            PEARL_create_callback(
                this,
                &PatternMPI_LateReceiver::post_send_bws_cb));
        cbmanager->register_callback(
            MPI_SEND_COMPLETE,
            PEARL_create_callback(
                this,
                &PatternMPI_LateReceiver::mpi_send_complete_bws_cb));
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_LateReceiver::pre_recv_bws_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_LateReceiver::late_sender_candidate__cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        #line 954 "MPI.pattern"

        m_ls_set.insert(event.enterptr());
    }


    void
    PatternMPI_LateReceiver::mpi_send_complete_bws_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        #line 1037 "MPI.pattern"

        Event enter = event.enterptr();

        if (  is_mpi_wait_multi(enter->getRegion())
           && !m_ls_set.count(enter))
        {
            multiwait_map_t::iterator it = m_multiwait_map.find(enter);

            if (it == m_multiwait_map.end())
            {
                multiwait_info_t info;

                info.sendcount = 1;
                info.maxidle   = 0.0;

                m_multiwait_map.insert(std::make_pair(enter, info));
            }
            else
            {
                it->second.sendcount++;
            }
        }
    }


    void
    PatternMPI_LateReceiver::post_send_bws_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 969 "MPI.pattern"

        // backward replay: msg received from destination here

        Event       enter_sendcmp = data->mLocal->get_event(ROLE_ENTER_SEND_LR);
        Event       leave_sendcmp = data->mLocal->get_event(ROLE_LEAVE_SEND_LR);
        RemoteEvent enter_recvreq = data->mRemote->get_event(ROLE_ENTER_RECV_LR);
        RemoteEvent leave_recvreq = data->mRemote->get_event(ROLE_LEAVE_RECV_LR);

        // Calculate waiting time
        data->mIdle =
            leave_sendcmp->getTimestamp() <= enter_recvreq->getTimestamp()
            ? 0
            : enter_recvreq->getTimestamp() - enter_sendcmp->getTimestamp();

        if (data->mIdle > 0)
        {
            data->mCompletion =
                leave_sendcmp->getTimestamp() > leave_recvreq->getTimestamp()
                ? 0
                : leave_sendcmp->getTimestamp() - leave_recvreq->getTimestamp();
        }

        Region region = enter_sendcmp->getRegion();

        if (  is_mpi_block_send(region)
           || is_mpi_wait_single(region))
        {
            if (data->mIdle > 0)
            {
                m_severity[enter_sendcmp.get_cnode()] += data->mIdle;

                cbmanager.notify(LATE_RECEIVER, event, data);
            }
        }
        else if (is_mpi_wait_multi(region))
        {
            multiwait_map_t::iterator it = m_multiwait_map.find(enter_sendcmp);

            if (it == m_multiwait_map.end())
            {
                return;
            }

            if (data->mIdle > it->second.maxidle)
            {
                it->second.maxidle  = data->mIdle;
                it->second.maxcmpl  = data->mCompletion;
                it->second.maxevent = event;
            }

            --it->second.sendcount;

            if (it->second.sendcount == 0)
            {
                if (it->second.maxidle > 0.0)
                {
                    m_severity[it->first.get_cnode()] += it->second.maxidle;
                    data->mIdle                        = it->second.maxidle;
                    data->mCompletion                  = it->second.maxcmpl;

                    cbmanager.notify(LATE_RECEIVER, it->second.maxevent, data);
                }

                m_multiwait_map.erase(it);
            }
        }
    }


    void
    PatternMPI_LateReceiver::pre_recv_bws_cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1061 "MPI.pattern"

        data->mLocal->add_event(event, ROLE_RECV_LR);

        Event request = event.request();

        data->mLocal->add_event(request.enterptr(), ROLE_ENTER_RECV_LR);
        data->mLocal->add_event(request.leaveptr(), ROLE_LEAVE_RECV_LR);
    }


    void
    PatternMPI_LateReceiver::pre_send_bws_cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 960 "MPI.pattern"

        data->mLocal->add_event(event, ROLE_SEND_LR);

        Event completion = event.completion();

        data->mLocal->add_event(completion.enterptr(), ROLE_ENTER_SEND_LR);
        data->mLocal->add_event(completion.leaveptr(), ROLE_LEAVE_SEND_LR);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_Collective
     *  @brief MPI Collective Communication Time metric.
     *
     *  Implements the "MPI Collective Communication Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_Collective
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_COLLECTIVE;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_COMMUNICATION;
            }

            virtual string
            get_name() const
            {
                return "MPI collective communication";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_collective";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in MPI collective communication";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            virtual bool
            is_hidden() const
            {
                return true;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            pre_coll_12n__cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            pre_coll_n21__cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            pre_coll_n2n__cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            pre_coll_scan__cb(const CallbackManager& cbmanager,
                              int                    user_event,
                              const Event&           event,
                              CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_Collective::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_COLL_12N,
            PEARL_create_callback(
                this,
                &PatternMPI_Collective::pre_coll_12n__cb));
        cbmanager->register_callback(
            PRE_COLL_N2N,
            PEARL_create_callback(
                this,
                &PatternMPI_Collective::pre_coll_n2n__cb));
        cbmanager->register_callback(
            PRE_COLL_N21,
            PEARL_create_callback(
                this,
                &PatternMPI_Collective::pre_coll_n21__cb));
        cbmanager->register_callback(
            PRE_COLL_SCAN,
            PEARL_create_callback(
                this,
                &PatternMPI_Collective::pre_coll_scan__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_Collective::pre_coll_12n__cb(const CallbackManager& cbmanager,
                                            int                    user_event,
                                            const Event&           event,
                                            CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1117 "MPI.pattern"

        if (data->mIsSyncpoint)
        {
            const CollectiveInfo& ci = data->mCollinfo;

            // Validate clock condition
            const timestamp_t localEndT = event->getTimestamp();
            if (ci.root.mTime > localEndT)
            {
                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CLOCK_VIOLATION)
                    << "Clock violation @ " << *event.get_cnode()
                    << ": " << (ci.latest.mTime - localEndT);

                cbmanager.notify(CCV_COLL, event, data);
            }

            // Restrict wait & completion time to time spent in operation
            const timestamp_t rootBeginT   = min(ci.root.mTime, localEndT);
            const timestamp_t earliestEndT = max(ci.earliest_end.mTime, rootBeginT);

            // Calculate wait & completion time
            data->mIdle       = max(0.0, rootBeginT - ci.my.mTime);
            data->mCompletion = max(0.0, localEndT - earliestEndT);
        }
    }


    void
    PatternMPI_Collective::pre_coll_n21__cb(const CallbackManager& cbmanager,
                                            int                    user_event,
                                            const Event&           event,
                                            CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1169 "MPI.pattern"

        if (data->mIsSyncpoint)
        {
            const CollectiveInfo& ci = data->mCollinfo;

            // Validate clock condition
            const timestamp_t localEndT = event->getTimestamp();
            if (ci.latest.mTime > localEndT)
            {
                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CLOCK_VIOLATION)
                    << "Clock violation @ " << *event.get_cnode()
                    << ": " << (ci.latest.mTime - localEndT);

                cbmanager.notify(CCV_COLL, event, data);
            }

            // Restrict wait & completion time to time spent in operation
            const timestamp_t latestBeginT = min(ci.latest.mTime, localEndT);
            const timestamp_t earliestEndT = max(ci.earliest_end.mTime, latestBeginT);

            // Calculate wait & completion time
            data->mIdle       = max(0.0, latestBeginT - ci.my.mTime);
            data->mCompletion = max(0.0, localEndT - earliestEndT);
        }
    }


    void
    PatternMPI_Collective::pre_coll_n2n__cb(const CallbackManager& cbmanager,
                                            int                    user_event,
                                            const Event&           event,
                                            CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1143 "MPI.pattern"

        if (data->mIsSyncpoint)
        {
            const CollectiveInfo& ci = data->mCollinfo;

            // Validate clock condition
            const timestamp_t localEndT = event->getTimestamp();
            if (ci.latest.mTime > localEndT)
            {
                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CLOCK_VIOLATION)
                    << "Clock violation @ " << *event.get_cnode()
                    << ": " << (ci.latest.mTime - localEndT);

                cbmanager.notify(CCV_COLL, event, data);
            }

            // Restrict wait & completion time to time spent in operation
            const timestamp_t latestBeginT = min(ci.latest.mTime, localEndT);
            const timestamp_t earliestEndT = max(ci.earliest_end.mTime, latestBeginT);

            // Calculate wait & completion time
            data->mIdle       = max(0.0, latestBeginT - ci.my.mTime);
            data->mCompletion = max(0.0, localEndT - earliestEndT);
        }
    }


    void
    PatternMPI_Collective::pre_coll_scan__cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1195 "MPI.pattern"

        if (data->mIsSyncpoint)
        {
            const CollectiveInfo& ci = data->mCollinfo;

            // Validate clock condition
            const timestamp_t localEndT = event->getTimestamp();
            if (ci.latest.mTime > localEndT)
            {
                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CLOCK_VIOLATION)
                    << "Clock violation @ " << *event.get_cnode()
                    << ": " << (ci.latest.mTime - localEndT);

                cbmanager.notify(CCV_COLL, event, data);
            }

            // Restrict wait time to time spent in operation
            const timestamp_t latestBeginT = min(ci.latest.mTime, localEndT);

            // Calculate wait time
            data->mIdle = max(0.0, latestBeginT - ci.my.mTime);
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_EarlyReduce
     *  @brief MPI Early Reduce Time metric.
     *
     *  Implements the "MPI Early Reduce Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_EarlyReduce
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_EARLYREDUCE;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_COLLECTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Early Reduce";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_earlyreduce";
            }

            virtual string
            get_descr() const
            {
                return "Waiting time due to an early receiver in MPI n-to-1"
                       " operations";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            coll_n21__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_EarlyReduce::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            COLL_N21,
            PEARL_create_callback(
                this,
                &PatternMPI_EarlyReduce::coll_n21__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_EarlyReduce::coll_n21__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1246 "MPI.pattern"

        if (data->mIdle > 0)
        {
            Callpath* const callpath = event.get_cnode();

            m_severity[callpath] += data->mIdle;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << get_name() << " @ " << *callpath
                << ": " << data->mIdle
                << " [total: " << m_severity[callpath] << ']';

            // Early Reduce time only occurs on the root process, i.e.,
            // most-severe instance tracking can be performed locally
            cbmanager.notify(EARLY_REDUCE, event, data);
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_EarlyScan
     *  @brief MPI Early Scan Time metric.
     *
     *  Implements the "MPI Early Scan Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_EarlyScan
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_EARLYSCAN;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_COLLECTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Early Scan";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_earlyscan";
            }

            virtual string
            get_descr() const
            {
                return "Waiting time due to an early receiver in an MPI scan"
                       " operation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            coll_scan__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_EarlyScan::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            COLL_SCAN,
            PEARL_create_callback(
                this,
                &PatternMPI_EarlyScan::coll_scan__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_EarlyScan::coll_scan__cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1288 "MPI.pattern"

        if (data->mIdle > 0)
        {
            Callpath* const callpath = event.get_cnode();

            m_severity[callpath] += data->mIdle;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << get_name() << " @ " << *callpath
                << ": " << data->mIdle
                << " [total: " << m_severity[callpath] << ']';
        }

        // All processes need to take part in the most-severe instance tracking
        cbmanager.notify(EARLY_SCAN, event, data);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_LateBroadcast
     *  @brief MPI Late Broadcast Time metric.
     *
     *  Implements the "MPI Late Broadcast Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_LateBroadcast
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_LATEBROADCAST;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_COLLECTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Broadcast";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_latebroadcast";
            }

            virtual string
            get_descr() const
            {
                return "Waiting time due to a late sender in MPI 1-to-n"
                       " operations";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            coll_12n__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_LateBroadcast::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            COLL_12N,
            PEARL_create_callback(
                this,
                &PatternMPI_LateBroadcast::coll_12n__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_LateBroadcast::coll_12n__cb(const CallbackManager& cbmanager,
                                           int                    user_event,
                                           const Event&           event,
                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1332 "MPI.pattern"

        if (data->mIdle > 0)
        {
            Callpath* const callpath = event.get_cnode();

            m_severity[callpath] += data->mIdle;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << get_name() << " @ " << *callpath
                << ": " << data->mIdle
                << " [total: " << m_severity[callpath] << ']';
        }

        // Notify all processes to allow for collective communication (e.g.,
        // during most-severe instance tracking)
        cbmanager.notify(LATE_BCAST, event, data);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_WaitNxN
     *  @brief MPI Wait at N x N Time metric.
     *
     *  Implements the "MPI Wait at N x N Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_WaitNxN
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_WAIT_NXN;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_COLLECTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Wait at N x N";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_wait_nxn";
            }

            virtual string
            get_descr() const
            {
                return "Time due to inherent synchronization in MPI n-to-n"
                       " operations";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            coll_n2n__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_WaitNxN::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            COLL_N2N,
            PEARL_create_callback(
                this,
                &PatternMPI_WaitNxN::coll_n2n__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_WaitNxN::coll_n2n__cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1383 "MPI.pattern"

        if (data->mIdle > 0)
        {
            Callpath* const callpath = event.get_cnode();

            m_severity[callpath] += data->mIdle;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << get_name() << " @ " << *callpath
                << ": " << data->mIdle
                << " [total: " << m_severity[callpath] << ']';
        }

        // Notify all processes to allow for collective communication (e.g.,
        // during most-severe instance tracking)
        cbmanager.notify(WAIT_NXN, event, data);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_NxNCompletion
     *  @brief MPI N x N Completion Time metric.
     *
     *  Implements the "MPI N x N Completion Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_NxNCompletion
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_NXN_COMPLETION;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_COLLECTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI N x N Completion";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_nxn_completion";
            }

            virtual string
            get_descr() const
            {
                return "Time needed to finish an MPI n-to-n collective"
                       " operation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            coll_n2n__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_NxNCompletion::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            COLL_N2N,
            PEARL_create_callback(
                this,
                &PatternMPI_NxNCompletion::coll_n2n__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_NxNCompletion::coll_n2n__cb(const CallbackManager& cbmanager,
                                           int                    user_event,
                                           const Event&           event,
                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1428 "MPI.pattern"

        if (data->mCompletion > 0)
        {
            Callpath* const callpath = event.get_cnode();

            m_severity[event.get_cnode()] += data->mCompletion;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << get_name() << " @ " << *callpath
                << ": " << data->mCompletion
                << " [total: " << m_severity[callpath] << ']';
        }

        // Notify all processes to allow for collective communication (e.g.,
        // during most-severe instance tracking)
        cbmanager.notify(NXN_COMPL, event, data);
    }
#endif    // _MPI


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternOMP_Mgmt
     *  @brief OpenMP Thread Management Time metric.
     *
     *  Implements the "OpenMP Thread Management Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternOMP_Mgmt
        : public OmpPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_OMP_MANAGEMENT;
            }

            virtual long
            get_parent() const
            {
                return PAT_OMP_TIME;
            }

            virtual string
            get_name() const
            {
                return "OMP Management";
            }

            virtual string
            get_unique_name() const
            {
                return "omp_management";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in OpenMP thread management";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            group_enter__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            group_leave__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            thread_fork__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            thread_join__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternOMP_Mgmt::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            THREAD_FORK,
            PEARL_create_callback(
                this,
                &PatternOMP_Mgmt::thread_fork__cb));
        cbmanager->register_callback(
            THREAD_JOIN,
            PEARL_create_callback(
                this,
                &PatternOMP_Mgmt::thread_join__cb));
        cbmanager->register_callback(
            GROUP_ENTER,
            PEARL_create_callback(
                this,
                &PatternOMP_Mgmt::group_enter__cb));
        cbmanager->register_callback(
            GROUP_LEAVE,
            PEARL_create_callback(
                this,
                &PatternOMP_Mgmt::group_leave__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternOMP_Mgmt::group_enter__cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 163 "OMP.pattern"

        // Are we entering an OpenMP parallel region?
        const Region& region = event->getRegion();
        if (!is_omp_parallel(region))
        {
            return;
        }

        // Determine timestamp of FORK event and the earliest ENTER into
        // the parallel region
        timestamp_t enterT = event->getTimestamp();

        static timestamp_t forkT;
        static timestamp_t earliestEnterT;
        #pragma omp master
        {
            Event fork = event.prev();
            while (fork->getType() != THREAD_FORK)
            {
                --fork;
            }
            forkT          = fork->getTimestamp();
            earliestEnterT = enterT;
        }
        #pragma omp barrier
        #pragma omp critical
        {
            earliestEnterT = std::min(earliestEnterT, enterT);
        }

        // Calculate thread fork and completion time
        #pragma omp barrier
        data->mIdle       = std::max(0.0, enterT - forkT);
        data->mCompletion = std::max(0.0, enterT - earliestEnterT);

        m_severity[event.get_cnode()] += data->mIdle;
        cbmanager.notify(OMP_MGMT_FORK, event, data);
        #pragma omp barrier
    }


    void
    PatternOMP_Mgmt::group_leave__cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 203 "OMP.pattern"

        // Are we leaving an OpenMP parallel region?
        Event         enter  = data->mCallstack->top();
        const Region& region = enter->getRegion();
        if (!is_omp_parallel(region))
        {
            return;
        }

        // Determine timestamp of JOIN event
        static timestamp_t joinTimestamp;
        #pragma omp master
        {
            Event join = event.next();
            while (join->getType() != THREAD_JOIN)
            {
                ++join;
            }

            joinTimestamp = join->getTimestamp();
        }

        // Calculate thread join time
        #pragma omp barrier
        data->mIdle                    = std::max(0.0, joinTimestamp - event->getTimestamp());
        m_severity[enter.get_cnode()] += data->mIdle;
        cbmanager.notify(OMP_MGMT_JOIN, event, data);
        #pragma omp barrier
    }


    void
    PatternOMP_Mgmt::thread_fork__cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
    {
        #line 153 "OMP.pattern"

        // Increment nesting level
        omp_nest_level++;
    }


    void
    PatternOMP_Mgmt::thread_join__cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
    {
        #line 158 "OMP.pattern"

        // Decrement nesting level
        omp_nest_level--;
    }
#endif    // _OPENMP


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternOMP_Mgmt_Fork
     *  @brief OpenMP Thread Team Fork Time metric.
     *
     *  Implements the "OpenMP Thread Team Fork Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternOMP_Mgmt_Fork
        : public OmpPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_OMP_FORK;
            }

            virtual long
            get_parent() const
            {
                return PAT_OMP_MANAGEMENT;
            }

            virtual string
            get_name() const
            {
                return "OMP Fork";
            }

            virtual string
            get_unique_name() const
            {
                return "omp_fork";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in forking OpenMP thread teams";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            omp_mgmt_fork__cb(const CallbackManager& cbmanager,
                              int                    user_event,
                              const Event&           event,
                              CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternOMP_Mgmt_Fork::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            OMP_MGMT_FORK,
            PEARL_create_callback(
                this,
                &PatternOMP_Mgmt_Fork::omp_mgmt_fork__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternOMP_Mgmt_Fork::omp_mgmt_fork__cb(const CallbackManager& cbmanager,
                                            int                    user_event,
                                            const Event&           event,
                                            CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 251 "OMP.pattern"

        m_severity[event.get_cnode()] += data->mIdle;
    }
#endif    // _OPENMP


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternOMP_Sync_Barrier
     *  @brief OpenMP Barrier Synchronization Time metric.
     *
     *  Implements the "OpenMP Barrier Synchronization Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternOMP_Sync_Barrier
        : public OmpPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_OMP_BARRIER;
            }

            virtual long
            get_parent() const
            {
                return PAT_OMP_SYNCHRONIZATION;
            }

            virtual string
            get_name() const
            {
                return "OMP Barrier";
            }

            virtual string
            get_unique_name() const
            {
                return "omp_barrier";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in OpenMP barrier synchronization";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            virtual bool
            is_hidden() const
            {
                return true;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            group_enter__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            group_leave__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            task_correction__cb(const CallbackManager& cbmanager,
                                int                    user_event,
                                const Event&           event,
                                CallbackData*          cdata);

            /// @}


        private:
            #line 297 "OMP.pattern"

            double mTaskExecutionTime;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternOMP_Sync_Barrier::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            TASK_CORRECTION,
            PEARL_create_callback(
                this,
                &PatternOMP_Sync_Barrier::task_correction__cb));
        cbmanager->register_callback(
            GROUP_ENTER,
            PEARL_create_callback(
                this,
                &PatternOMP_Sync_Barrier::group_enter__cb));
        cbmanager->register_callback(
            GROUP_LEAVE,
            PEARL_create_callback(
                this,
                &PatternOMP_Sync_Barrier::group_leave__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternOMP_Sync_Barrier::group_enter__cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
    {
        #line 305 "OMP.pattern"

        const Region& region = event.get_cnode()->getRegion();
        if (!is_omp_barrier(region))
        {
            return;
        }

        // Reset task execution time when entering the barrier
        mTaskExecutionTime = 0;
    }


    void
    PatternOMP_Sync_Barrier::group_leave__cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 316 "OMP.pattern"

        // Are we leaving an (implicit or explicit) OpenMP barrier?
        Event         enter  = data->mCallstack->top();
        const Region& region = enter->getRegion();
        if (!is_omp_barrier(region))
        {
            return;
        }

        // Skip OpenMP barrier waiting time calculation when running serially
        // (the barriers called in the algorithm cause ill side effects...)
        if (omp_nest_level == 0)
        {
            return;
        }

        // Determine latest enter time
        timestamp_t enterTime = enter->getTimestamp();

        // Implicitly shared timestamp for reduction
        static timestamp_t latestEnter;
        #pragma omp barrier
        #pragma omp master
        latestEnter = 0;
        #pragma omp barrier
        #pragma omp critical
        {
            if (latestEnter < enterTime)
            {
                latestEnter = enterTime;
            }
        }
        #pragma omp barrier

        // Calculate waiting time
        data->mIdle = latestEnter - enterTime - mTaskExecutionTime;

        // Notify sub-patterns
        if (is_omp_ebarrier(region))
        {
            cbmanager.notify(OMP_EBARRIER, event, data);
        }
        else if (is_omp_ibarrier(region))
        {
            cbmanager.notify(OMP_IBARRIER, event, data);
        }
    }


    void
    PatternOMP_Sync_Barrier::task_correction__cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 301 "OMP.pattern"

        mTaskExecutionTime += data->mIdle;
    }
#endif    // _OPENMP


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternOMP_Sync_Ebarrier_Wait
     *  @brief Wait at Explicit OpenMP Barrier Time metric.
     *
     *  Implements the "Wait at Explicit OpenMP Barrier Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternOMP_Sync_Ebarrier_Wait
        : public OmpPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_OMP_EBARRIER_WAIT;
            }

            virtual long
            get_parent() const
            {
                return PAT_OMP_EBARRIER;
            }

            virtual string
            get_name() const
            {
                return "OMP Wait at Explicit Barrier";
            }

            virtual string
            get_unique_name() const
            {
                return "omp_ebarrier_wait";
            }

            virtual string
            get_descr() const
            {
                return "Waiting time in front of explicit OpenMP barriers";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            omp_ebarrier__cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternOMP_Sync_Ebarrier_Wait::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            OMP_EBARRIER,
            PEARL_create_callback(
                this,
                &PatternOMP_Sync_Ebarrier_Wait::omp_ebarrier__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternOMP_Sync_Ebarrier_Wait::omp_ebarrier__cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 414 "OMP.pattern"

        Event enter = data->mCallstack->top();

        if (data->mIdle > 0)
        {
            m_severity[enter.get_cnode()] += data->mIdle;
        }

        // There will always be waiting time at barriers; all threads need
        // to take part in most-severe instance tracking
        cbmanager.notify(OMP_EBARRIER_WAIT, event, data);
    }
#endif    // _OPENMP


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternOMP_Sync_Ibarrier_Wait
     *  @brief Wait at Implicit OpenMP Barrier Time metric.
     *
     *  Implements the "Wait at Implicit OpenMP Barrier Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternOMP_Sync_Ibarrier_Wait
        : public OmpPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_OMP_IBARRIER_WAIT;
            }

            virtual long
            get_parent() const
            {
                return PAT_OMP_IBARRIER;
            }

            virtual string
            get_name() const
            {
                return "OMP Wait at Implicit Barrier";
            }

            virtual string
            get_unique_name() const
            {
                return "omp_ibarrier_wait";
            }

            virtual string
            get_descr() const
            {
                return "Waiting time in front of implicit OpenMP barriers";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            omp_ibarrier__cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternOMP_Sync_Ibarrier_Wait::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            OMP_IBARRIER,
            PEARL_create_callback(
                this,
                &PatternOMP_Sync_Ibarrier_Wait::omp_ibarrier__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternOMP_Sync_Ibarrier_Wait::omp_ibarrier__cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 477 "OMP.pattern"

        Event enter = data->mCallstack->top();

        if (data->mIdle > 0)
        {
            m_severity[enter.get_cnode()] += data->mIdle;
        }

        // There will always be waiting time at barriers; all threads need
        // to take part in most-severe instance tracking
        cbmanager.notify(OMP_IBARRIER_WAIT, event, data);
    }
#endif    // _OPENMP


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternOMP_Lock_Contention_Critical
     *  @brief OpenMP Critical Contention Time metric.
     *
     *  Implements the "OpenMP Critical Contention Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternOMP_Lock_Contention_Critical
        : public OmpPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_OMP_LOCK_CONTENTION_CRITICAL;
            }

            virtual long
            get_parent() const
            {
                return PAT_OMP_CRITICAL;
            }

            virtual string
            get_name() const
            {
                return "OMP Critical Contention";
            }

            virtual string
            get_unique_name() const
            {
                return "omp_lock_contention_critical";
            }

            virtual string
            get_descr() const
            {
                return "Time lost waiting for an OpenMP critical section to"
                       " become available";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            omp_contention_critical__cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternOMP_Lock_Contention_Critical::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            OMP_CONTENTION_CRITICAL,
            PEARL_create_callback(
                this,
                &PatternOMP_Lock_Contention_Critical::omp_contention_critical__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternOMP_Lock_Contention_Critical::omp_contention_critical__cb(const CallbackManager& cbmanager,
                                                                     int                    user_event,
                                                                     const Event&           event,
                                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 538 "OMP.pattern"

        if (data->mIdle > 0)
        {
            m_severity[event.get_cnode()] += data->mIdle;

            // instance tracking
            cbmanager.notify(THREAD_LOCK_CONTENTION_OMPCRITICAL_WAIT, event, data);
        }
    }
#endif    // _OPENMP


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternOMP_Lock_Contention_API
     *  @brief OpenMP Lock API Contention Time metric.
     *
     *  Implements the "OpenMP Lock API Contention Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternOMP_Lock_Contention_API
        : public OmpPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_OMP_LOCK_CONTENTION_API;
            }

            virtual long
            get_parent() const
            {
                return PAT_OMP_LOCK_API;
            }

            virtual string
            get_name() const
            {
                return "OMP Lock API Contention";
            }

            virtual string
            get_unique_name() const
            {
                return "omp_lock_contention_api";
            }

            virtual string
            get_descr() const
            {
                return "Time lost waiting for an OpenMP lock to be acquired";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            omp_contention_api__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternOMP_Lock_Contention_API::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            OMP_CONTENTION_API,
            PEARL_create_callback(
                this,
                &PatternOMP_Lock_Contention_API::omp_contention_api__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternOMP_Lock_Contention_API::omp_contention_api__cb(const CallbackManager& cbmanager,
                                                           int                    user_event,
                                                           const Event&           event,
                                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 595 "OMP.pattern"

        if (data->mIdle > 0)
        {
            m_severity[event.get_cnode()] += data->mIdle;

            // instance tracking
            cbmanager.notify(THREAD_LOCK_CONTENTION_OMPLOCK_WAIT, event, data);
        }
    }
#endif    // _OPENMP


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternPthread_Lock_Contention_Mutex_Lock
     *  @brief POSIX Threads Mutex Contention Time metric.
     *
     *  Implements the "POSIX Threads Mutex Contention Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternPthread_Lock_Contention_Mutex_Lock
        : public PthreadPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_PTHREAD_LOCK_CONTENTION_MUTEX_LOCK;
            }

            virtual long
            get_parent() const
            {
                return PAT_PTHREAD_LOCK_API;
            }

            virtual string
            get_name() const
            {
                return "Pthread Mutex Lock Contention";
            }

            virtual string
            get_unique_name() const
            {
                return "pthread_lock_contention_mutex_lock";
            }

            virtual string
            get_descr() const
            {
                return "Time lost waiting for a POSIX threads mutex to be"
                       " acquired";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            pthread_contention_mutex_lock__cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternPthread_Lock_Contention_Mutex_Lock::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PTHREAD_CONTENTION_MUTEX_LOCK,
            PEARL_create_callback(
                this,
                &PatternPthread_Lock_Contention_Mutex_Lock::pthread_contention_mutex_lock__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternPthread_Lock_Contention_Mutex_Lock::pthread_contention_mutex_lock__cb(const CallbackManager& cbmanager,
                                                                                 int                    user_event,
                                                                                 const Event&           event,
                                                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 134 "Pthread.pattern"

        if (data->mIdle > 0)
        {
            m_severity[event.get_cnode()] += data->mIdle;

            // instance tracking
            cbmanager.notify(THREAD_LOCK_CONTENTION_PTHREADMUTEX_WAIT, event, data);
        }
    }
#endif    // _OPENMP


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternPthread_Lock_Contention_Conditional
     *  @brief POSIX Threads Condition Contention Time metric.
     *
     *  Implements the "POSIX Threads Condition Contention Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternPthread_Lock_Contention_Conditional
        : public PthreadPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_PTHREAD_LOCK_CONTENTION_CONDITIONAL;
            }

            virtual long
            get_parent() const
            {
                return PAT_PTHREAD_CONDITIONAL;
            }

            virtual string
            get_name() const
            {
                return "Pthread Condition Lock Contention";
            }

            virtual string
            get_unique_name() const
            {
                return "pthread_lock_contention_conditional";
            }

            virtual string
            get_descr() const
            {
                return "Time lost waiting for a POSIX threads mutex to be"
                       " acquired in a condition operation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            pthread_contention_conditional__cb(const CallbackManager& cbmanager,
                                               int                    user_event,
                                               const Event&           event,
                                               CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternPthread_Lock_Contention_Conditional::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PTHREAD_CONTENTION_CONDITIONAL,
            PEARL_create_callback(
                this,
                &PatternPthread_Lock_Contention_Conditional::pthread_contention_conditional__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternPthread_Lock_Contention_Conditional::pthread_contention_conditional__cb(const CallbackManager& cbmanager,
                                                                                   int                    user_event,
                                                                                   const Event&           event,
                                                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 190 "Pthread.pattern"

        if (data->mIdle > 0)
        {
            m_severity[event.get_cnode()] += data->mIdle;

            // instance tracking
            cbmanager.notify(THREAD_LOCK_CONTENTION_PTHREADCONDITION_WAIT, event, data);
        }
    }
#endif    // _OPENMP


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternThread_LockContention
     *  @brief Thread Lock Contention Time metric.
     *
     *  Implements the "Thread Lock Contention Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternThread_LockContention
        : public OmpPattern
    {
        public:
            /// @name Constructors & destructor
            /// @{

            virtual
            ~PatternThread_LockContention()
            {
                #line 67 "Thread.pattern"

                PEARL_Barrier();
                if (PEARL_GetThreadNumber() == 0)
                {
                    delete mOpenmpLockTracking;
                    delete mPthreadLockTracking;
                }
            }

            /// @}
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_THREAD_LOCK_CONTENTION;
            }

            virtual long
            get_parent() const
            {
                return PAT_NONE;
            }

            virtual string
            get_name() const
            {
                return "Thread Lock Contention";
            }

            virtual string
            get_unique_name() const
            {
                return "thread_lock_contention";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in thread lock contention";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            virtual bool
            is_hidden() const
            {
                return true;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            thread_acquire_lock__cb(const CallbackManager& cbmanager,
                                    int                    user_event,
                                    const Event&           event,
                                    CallbackData*          cdata);

            void
            thread_release_lock__cb(const CallbackManager& cbmanager,
                                    int                    user_event,
                                    const Event&           event,
                                    CallbackData*          cdata);

            /// @}


        protected:
            /// @name Pattern management
            /// @{

            virtual void
            init()
            {
                #line 59 "Thread.pattern"

                if (PEARL_GetThreadNumber() == 0)
                {
                    mOpenmpLockTracking  = new scout::LockTracking();
                    mPthreadLockTracking = new scout::LockTracking();
                }
                PEARL_Barrier();
            }

            /// @}


        private:
            #line 35 "Thread.pattern"

            LockTracking*
            getLockTrackingVariant(Paradigm paradigm)
            {
                switch (native_value(paradigm))
                {
                    case Paradigm::OPENMP:
                        return mOpenmpLockTracking;

                    case Paradigm::PTHREAD:
                        return mPthreadLockTracking;

                    default:
                        return NULL;
                }
            }

            static LockTracking* mOpenmpLockTracking;
            static LockTracking* mPthreadLockTracking;
    };


    // --- Static class data ------------------------------------------------

    #line 55 "Thread.pattern"

    LockTracking* PatternThread_LockContention::mOpenmpLockTracking;
    LockTracking* PatternThread_LockContention::mPthreadLockTracking;


    // --- Registering callbacks --------------------------------------------

    void
    PatternThread_LockContention::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            THREAD_ACQUIRE_LOCK,
            PEARL_create_callback(
                this,
                &PatternThread_LockContention::thread_acquire_lock__cb));
        cbmanager->register_callback(
            THREAD_RELEASE_LOCK,
            PEARL_create_callback(
                this,
                &PatternThread_LockContention::thread_release_lock__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternThread_LockContention::thread_acquire_lock__cb(const CallbackManager& cbmanager,
                                                          int                    user_event,
                                                          const Event&           event,
                                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 76 "Thread.pattern"

        Paradigm paradigm = getLockParadigm(event);

        LockTracking* lockMap = getLockTrackingVariant(paradigm);
        if (lockMap == NULL)
        {
            // unsupported paradigm
            return;
        }

        // wait time=last release time  - respective Enter timestamp
        data->mIdle = lockMap->acquireLock(event)
                      - event.enterptr()->getTimestamp();

        LockType lockType = getLockType(event);

        // distribute time dependent on lock type
        if (lockType == LockType::OMP_LOCK_API)
        {
            cbmanager.notify(OMP_CONTENTION_API, event, data);
        }
        else if (lockType == LockType::OMP_CRITICAL)
        {
            cbmanager.notify(OMP_CONTENTION_CRITICAL, event, data);
        }
        else if (lockType == LockType::PTHREAD_MUTEX_LOCK)
        {
            cbmanager.notify(PTHREAD_CONTENTION_MUTEX_LOCK, event, data);
        }
        else if (lockType == LockType::PTHREAD_MUTEX_TRYLOCK)
        {
            cbmanager.notify(PTHREAD_CONTENTION_MUTEX_LOCK, event, data);
        }
        else if (lockType == LockType::PTHREAD_CONDITION_WAIT)
        {
            cbmanager.notify(PTHREAD_CONTENTION_CONDITIONAL, event, data);
        }
    }


    void
    PatternThread_LockContention::thread_release_lock__cb(const CallbackManager& cbmanager,
                                                          int                    user_event,
                                                          const Event&           event,
                                                          CallbackData*          cdata)
    {
        #line 115 "Thread.pattern"

        Paradigm paradigm = getLockParadigm(event);

        LockTracking* lockMap = getLockTrackingVariant(paradigm);
        if (lockMap == NULL)
        {
            // unsupported paradigm
            return;
        }
        lockMap->releaseLock(event);
    }
#endif    // _OPENMP


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_SyncsSend
     *  @brief MPI Point-to-point Send Synchronization Operations metric.
     *
     *  Implements the "MPI Point-to-point Send Synchronization Operations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_SyncsSend
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_SYNCS_SEND;
            }

            virtual long
            get_parent() const
            {
                return PAT_SYNCS_P2P;
            }

            virtual string
            get_name() const
            {
                return "MPI P2P send synchronizations";
            }

            virtual string
            get_unique_name() const
            {
                return "syncs_send";
            }

            virtual string
            get_descr() const
            {
                return "Number of MPI point-to-point send synchronization"
                       " operations";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            group_send__cb(const CallbackManager& cbmanager,
                           int                    user_event,
                           const Event&           event,
                           CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_SyncsSend::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            GROUP_SEND,
            PEARL_create_callback(
                this,
                &PatternMPI_SyncsSend::group_send__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_SyncsSend::group_send__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        #line 73 "Message.pattern"

        if (event->getBytesSent() == 0)
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_SyncsRecv
     *  @brief MPI Point-to-point Receive Synchronization Operations metric.
     *
     *  Implements the "MPI Point-to-point Receive Synchronization Operations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_SyncsRecv
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_SYNCS_RECV;
            }

            virtual long
            get_parent() const
            {
                return PAT_SYNCS_P2P;
            }

            virtual string
            get_name() const
            {
                return "MPI P2P recv synchronizations";
            }

            virtual string
            get_unique_name() const
            {
                return "syncs_recv";
            }

            virtual string
            get_descr() const
            {
                return "Number of MPI point-to-point receive synchronization"
                       " operations";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            post_recv__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_SyncsRecv::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            POST_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_SyncsRecv::post_recv__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_SyncsRecv::post_recv__cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 97 "Message.pattern"

        RemoteEvent send = data->mRemote->get_event(ROLE_SEND);

        if (send->getBytesSent() == 0)
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_SyncsColl
     *  @brief MPI Collective Synchronizations metric.
     *
     *  Implements the "MPI Collective Synchronizations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_SyncsColl
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_SYNCS_COLL;
            }

            virtual long
            get_parent() const
            {
                return PAT_SYNCS;
            }

            virtual string
            get_name() const
            {
                return "MPI collective synchronizations";
            }

            virtual string
            get_unique_name() const
            {
                return "syncs_coll";
            }

            virtual string
            get_descr() const
            {
                return "Number of MPI collective synchronization operations";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_collective_end__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_SyncsColl::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_COLLECTIVE_END,
            PEARL_create_callback(
                this,
                &PatternMPI_SyncsColl::mpi_collective_end__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_SyncsColl::mpi_collective_end__cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        #line 134 "Message.pattern"

        if (  (event->getBytesSent() == 0)
           && (event->getBytesReceived() == 0))
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_CommsSend
     *  @brief MPI Point-to-point Send Communication Operations metric.
     *
     *  Implements the "MPI Point-to-point Send Communication Operations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_CommsSend
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_COMMS_SEND;
            }

            virtual long
            get_parent() const
            {
                return PAT_COMMS_P2P;
            }

            virtual string
            get_name() const
            {
                return "MPI P2P send communications";
            }

            virtual string
            get_unique_name() const
            {
                return "comms_send";
            }

            virtual string
            get_descr() const
            {
                return "Number of MPI point-to-point send communication"
                       " operations";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            group_send__cb(const CallbackManager& cbmanager,
                           int                    user_event,
                           const Event&           event,
                           CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_CommsSend::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            GROUP_SEND,
            PEARL_create_callback(
                this,
                &PatternMPI_CommsSend::group_send__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_CommsSend::group_send__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        #line 240 "Message.pattern"

        if (event->getBytesSent() != 0)
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_CommsRecv
     *  @brief MPI Point-to-point Receive Communication Operations metric.
     *
     *  Implements the "MPI Point-to-point Receive Communication Operations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_CommsRecv
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_COMMS_RECV;
            }

            virtual long
            get_parent() const
            {
                return PAT_COMMS_P2P;
            }

            virtual string
            get_name() const
            {
                return "MPI P2P recv communications";
            }

            virtual string
            get_unique_name() const
            {
                return "comms_recv";
            }

            virtual string
            get_descr() const
            {
                return "Number of MPI point-to-point receive communication"
                       " operations";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            post_recv__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_CommsRecv::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            POST_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_CommsRecv::post_recv__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_CommsRecv::post_recv__cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 264 "Message.pattern"

        RemoteEvent send = data->mRemote->get_event(ROLE_SEND);

        if (send->getBytesSent() != 0)
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_CommsExch
     *  @brief MPI Collective Exchange Communications metric.
     *
     *  Implements the "MPI Collective Exchange Communications" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_CommsExch
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_COMMS_CXCH;
            }

            virtual long
            get_parent() const
            {
                return PAT_COMMS_COLL;
            }

            virtual string
            get_name() const
            {
                return "MPI collective exchange communications";
            }

            virtual string
            get_unique_name() const
            {
                return "comms_cxch";
            }

            virtual string
            get_descr() const
            {
                return "Number of MPI collective communications as source"
                       " and destination";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_collective_end__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_CommsExch::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_COLLECTIVE_END,
            PEARL_create_callback(
                this,
                &PatternMPI_CommsExch::mpi_collective_end__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_CommsExch::mpi_collective_end__cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        #line 316 "Message.pattern"

        if (  (event->getBytesSent() != 0)
           && (event->getBytesReceived() != 0))
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_CommsSrc
     *  @brief MPI Collective Communications as Source metric.
     *
     *  Implements the "MPI Collective Communications as Source" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_CommsSrc
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_COMMS_CSRC;
            }

            virtual long
            get_parent() const
            {
                return PAT_COMMS_COLL;
            }

            virtual string
            get_name() const
            {
                return "MPI collective communications as source";
            }

            virtual string
            get_unique_name() const
            {
                return "comms_csrc";
            }

            virtual string
            get_descr() const
            {
                return "Number of MPI collective communications as source";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_collective_end__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_CommsSrc::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_COLLECTIVE_END,
            PEARL_create_callback(
                this,
                &PatternMPI_CommsSrc::mpi_collective_end__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_CommsSrc::mpi_collective_end__cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        #line 342 "Message.pattern"

        if (  (event->getBytesSent() != 0)
           && (event->getBytesReceived() == 0))
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_CommsDst
     *  @brief MPI Collective Communications as Destination metric.
     *
     *  Implements the "MPI Collective Communications as Destination" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_CommsDst
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_COMMS_CDST;
            }

            virtual long
            get_parent() const
            {
                return PAT_COMMS_COLL;
            }

            virtual string
            get_name() const
            {
                return "MPI collective communications as destination";
            }

            virtual string
            get_unique_name() const
            {
                return "comms_cdst";
            }

            virtual string
            get_descr() const
            {
                return "Number of MPI collective communications as"
                       " destination";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_collective_end__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_CommsDst::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_COLLECTIVE_END,
            PEARL_create_callback(
                this,
                &PatternMPI_CommsDst::mpi_collective_end__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_CommsDst::mpi_collective_end__cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        #line 368 "Message.pattern"

        if (  (event->getBytesSent() == 0)
           && (event->getBytesReceived() != 0))
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_BytesSent
     *  @brief MPI Point-to-point Bytes Sent metric.
     *
     *  Implements the "MPI Point-to-point Bytes Sent" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_BytesSent
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_BYTES_SENT;
            }

            virtual long
            get_parent() const
            {
                return PAT_BYTES_P2P;
            }

            virtual string
            get_name() const
            {
                return "MPI P2P bytes sent";
            }

            virtual string
            get_unique_name() const
            {
                return "bytes_sent";
            }

            virtual string
            get_descr() const
            {
                return "Number of bytes sent in MPI point-to-point"
                       " communication operations";
            }

            virtual string
            get_unit() const
            {
                return "bytes";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            group_send__cb(const CallbackManager& cbmanager,
                           int                    user_event,
                           const Event&           event,
                           CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_BytesSent::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            GROUP_SEND,
            PEARL_create_callback(
                this,
                &PatternMPI_BytesSent::group_send__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_BytesSent::group_send__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        #line 537 "Message.pattern"

        if (event->getBytesSent() > 0)
        {
            m_severity[event.get_cnode()] += event->getBytesSent();
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_BytesRcvd
     *  @brief MPI Point-to-point Bytes Received metric.
     *
     *  Implements the "MPI Point-to-point Bytes Received" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_BytesRcvd
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_BYTES_RCVD;
            }

            virtual long
            get_parent() const
            {
                return PAT_BYTES_P2P;
            }

            virtual string
            get_name() const
            {
                return "MPI P2P bytes received";
            }

            virtual string
            get_unique_name() const
            {
                return "bytes_rcvd";
            }

            virtual string
            get_descr() const
            {
                return "Number of bytes received in MPI point-to-point"
                       " communication operations";
            }

            virtual string
            get_unit() const
            {
                return "bytes";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            post_recv__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_BytesRcvd::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            POST_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_BytesRcvd::post_recv__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_BytesRcvd::post_recv__cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 579 "Message.pattern"

        RemoteEvent send = data->mRemote->get_event(ROLE_SEND);

        if (send->getBytesSent() > 0)
        {
            m_severity[event.get_cnode()] += send->getBytesSent();
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_BytesCout
     *  @brief MPI Collective Bytes Outgoing metric.
     *
     *  Implements the "MPI Collective Bytes Outgoing" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_BytesCout
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_BYTES_COUT;
            }

            virtual long
            get_parent() const
            {
                return PAT_BYTES_COLL;
            }

            virtual string
            get_name() const
            {
                return "MPI collective bytes outgoing";
            }

            virtual string
            get_unique_name() const
            {
                return "bytes_cout";
            }

            virtual string
            get_descr() const
            {
                return "Number of bytes outgoing in MPI collective"
                       " communication operations";
            }

            virtual string
            get_unit() const
            {
                return "bytes";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_collective_end__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_BytesCout::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_COLLECTIVE_END,
            PEARL_create_callback(
                this,
                &PatternMPI_BytesCout::mpi_collective_end__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_BytesCout::mpi_collective_end__cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        #line 639 "Message.pattern"

        uint64_t bytesSent = event->getBytesSent();
        if (bytesSent > 0)
        {
            m_severity[event.get_cnode()] += bytesSent;
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_BytesCin
     *  @brief MPI Collective Bytes Incoming metric.
     *
     *  Implements the "MPI Collective Bytes Incoming" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_BytesCin
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_BYTES_CIN;
            }

            virtual long
            get_parent() const
            {
                return PAT_BYTES_COLL;
            }

            virtual string
            get_name() const
            {
                return "MPI collective bytes incoming";
            }

            virtual string
            get_unique_name() const
            {
                return "bytes_cin";
            }

            virtual string
            get_descr() const
            {
                return "Number of bytes incoming in MPI collective"
                       " communication operations";
            }

            virtual string
            get_unit() const
            {
                return "bytes";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_collective_end__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_BytesCin::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_COLLECTIVE_END,
            PEARL_create_callback(
                this,
                &PatternMPI_BytesCin::mpi_collective_end__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_BytesCin::mpi_collective_end__cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        #line 669 "Message.pattern"

        uint64_t bytesReceived = event->getBytesReceived();
        if (bytesReceived > 0)
        {
            m_severity[event.get_cnode()] += bytesReceived;
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_BytesPut
     *  @brief MPI One-sided Bytes Sent metric.
     *
     *  Implements the "MPI One-sided Bytes Sent" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_BytesPut
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_BYTES_PUT;
            }

            virtual long
            get_parent() const
            {
                return PAT_BYTES_RMA;
            }

            virtual string
            get_name() const
            {
                return "MPI one-sided bytes sent";
            }

            virtual string
            get_unique_name() const
            {
                return "bytes_put";
            }

            virtual string
            get_descr() const
            {
                return "Number of bytes sent in MPI one-sided communication"
                       " operations";
            }

            virtual string
            get_unit() const
            {
                return "bytes";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_put_start__cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_BytesPut::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_PUT_START,
            PEARL_create_callback(
                this,
                &PatternMPI_BytesPut::mpi_rma_put_start__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_BytesPut::mpi_rma_put_start__cb(const CallbackManager& cbmanager,
                                               int                    user_event,
                                               const Event&           event,
                                               CallbackData*          cdata)
    {
        #line 722 "Message.pattern"

        m_severity[event.get_cnode()] += event->getBytesSent();
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_BytesGet
     *  @brief MPI One-sided Bytes Received metric.
     *
     *  Implements the "MPI One-sided Bytes Received" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_BytesGet
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_BYTES_GET;
            }

            virtual long
            get_parent() const
            {
                return PAT_BYTES_RMA;
            }

            virtual string
            get_name() const
            {
                return "MPI one-sided bytes received";
            }

            virtual string
            get_unique_name() const
            {
                return "bytes_get";
            }

            virtual string
            get_descr() const
            {
                return "Number of bytes received in MPI one-sided"
                       " communication operations";
            }

            virtual string
            get_unit() const
            {
                return "bytes";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_get_start__cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_BytesGet::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_GET_START,
            PEARL_create_callback(
                this,
                &PatternMPI_BytesGet::mpi_rma_get_start__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_BytesGet::mpi_rma_get_start__cb(const CallbackManager& cbmanager,
                                               int                    user_event,
                                               const Event&           event,
                                               CallbackData*          cdata)
    {
        #line 748 "Message.pattern"

        m_severity[event.get_cnode()] += event->getBytesReceived();
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_SLR_Count
     *  @brief MPI Late Receiver Instances (Synchronizations) metric.
     *
     *  Implements the "MPI Late Receiver Instances (Synchronizations)" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_SLR_Count
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_SLR_COUNT;
            }

            virtual long
            get_parent() const
            {
                return PAT_SYNCS_SEND;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Receiver Instances (Synchronizations)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_slr_count";
            }

            virtual string
            get_descr() const
            {
                return "Number of Late Receiver instances in MPI"
                       " synchronizations";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_receiver_bws_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_SLR_Count::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bws callbacks
        it = cbmanagers.find("bws");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternMPI_SLR_Count::late_receiver_bws_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_SLR_Count::late_receiver_bws_cb(const CallbackManager& cbmanager,
                                               int                    user_event,
                                               const Event&           event,
                                               CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 31 "Counts.pattern"

        Event completion = data->mLocal->get_event(ROLE_ENTER_SEND_LR);

        if (event->getBytesSent() == 0)
        {
            ++m_severity[completion.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_SLS_Count
     *  @brief MPI Late Sender Instances (Synchronizations) metric.
     *
     *  Implements the "MPI Late Sender Instances (Synchronizations)" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_SLS_Count
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_SLS_COUNT;
            }

            virtual long
            get_parent() const
            {
                return PAT_SYNCS_RECV;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Sender Instances (Synchronizations)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_sls_count";
            }

            virtual string
            get_descr() const
            {
                return "Number of Late Sender instances in MPI"
                       " synchronizations";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_sender__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_SLS_Count::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternMPI_SLS_Count::late_sender__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_SLS_Count::late_sender__cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 57 "Counts.pattern"

        RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LS);

        if (send->getBytesSent() == 0)
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_SLSWO_Count
     *  @brief MPI Late Sender, Wrong Order Instances (Synchronizations) metric.
     *
     *  Implements the "MPI Late Sender, Wrong Order Instances (Synchronizations)" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_SLSWO_Count
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_SLSWO_COUNT;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_SLS_COUNT;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Sender, Wrong Order Instances"
                       " (Synchronizations)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_slswo_count";
            }

            virtual string
            get_descr() const
            {
                return "Number of Late Sender instances in MPI"
                       " synchronizations where messages are received in"
                       " wrong order";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_sender_wo__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_SLSWO_Count::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_SENDER_WO,
            PEARL_create_callback(
                this,
                &PatternMPI_SLSWO_Count::late_sender_wo__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_SLSWO_Count::late_sender_wo__cb(const CallbackManager& cbmanager,
                                               int                    user_event,
                                               const Event&           event,
                                               CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 84 "Counts.pattern"

        RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LSWO);
        Event       recv = data->mLocal->get_event(ROLE_RECV_LSWO);

        if (send->getBytesSent() == 0)
        {
            ++m_severity[recv.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_CLR_Count
     *  @brief MPI Late Receiver Instances (Communications) metric.
     *
     *  Implements the "MPI Late Receiver Instances (Communications)" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_CLR_Count
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_CLR_COUNT;
            }

            virtual long
            get_parent() const
            {
                return PAT_COMMS_SEND;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Receiver Instances (Communications)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_clr_count";
            }

            virtual string
            get_descr() const
            {
                return "Number of Late Receiver instances in MPI"
                       " communications";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_receiver_bws_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_CLR_Count::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bws callbacks
        it = cbmanagers.find("bws");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternMPI_CLR_Count::late_receiver_bws_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_CLR_Count::late_receiver_bws_cb(const CallbackManager& cbmanager,
                                               int                    user_event,
                                               const Event&           event,
                                               CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 111 "Counts.pattern"

        Event completion = data->mLocal->get_event(ROLE_ENTER_SEND_LR);

        if (event->getBytesSent() > 0)
        {
            ++m_severity[completion.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_CLS_Count
     *  @brief MPI Late Sender Instances (Communications) metric.
     *
     *  Implements the "MPI Late Sender Instances (Communications)" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_CLS_Count
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_CLS_COUNT;
            }

            virtual long
            get_parent() const
            {
                return PAT_COMMS_RECV;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Sender Instances (Communications)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_cls_count";
            }

            virtual string
            get_descr() const
            {
                return "Number of Late Sender instances in MPI"
                       " communications";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_sender__cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_CLS_Count::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternMPI_CLS_Count::late_sender__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_CLS_Count::late_sender__cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 137 "Counts.pattern"

        RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LS);

        if (send->getBytesSent() != 0)
        {
            ++m_severity[event.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_CLSWO_Count
     *  @brief MPI Late Sender, Wrong Order Instances (Communications) metric.
     *
     *  Implements the "MPI Late Sender, Wrong Order Instances (Communications)" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_CLSWO_Count
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_CLSWO_COUNT;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_CLS_COUNT;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Sender, Wrong Order Instances"
                       " (Communications)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_clswo_count";
            }

            virtual string
            get_descr() const
            {
                return "Number of Late Sender instances in MPI"
                       " communications where messages are received in wrong"
                       " order";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            late_sender_wo__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_CLSWO_Count::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            LATE_SENDER_WO,
            PEARL_create_callback(
                this,
                &PatternMPI_CLSWO_Count::late_sender_wo__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_CLSWO_Count::late_sender_wo__cb(const CallbackManager& cbmanager,
                                               int                    user_event,
                                               const Event&           event,
                                               CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 163 "Counts.pattern"

        RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LSWO);
        Event       recv = data->mLocal->get_event(ROLE_RECV_LSWO);

        if (send->getBytesSent() != 0)
        {
            ++m_severity[recv.get_cnode()];
        }
    }
#endif    // _MPI


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaWaitAtCreate
     *  @brief MPI Wait at Window Create Time metric.
     *
     *  Implements the "MPI Wait at Window Create Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaWaitAtCreate
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_WAIT_AT_CREATE;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_MGMT_WIN;
            }

            virtual string
            get_name() const
            {
                return "MPI Wait at Window Create";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_wait_at_create";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in MPI_Win_create, waiting for the last"
                       " process to enter the call";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_wait_at_create__cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaWaitAtCreate::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_WAIT_AT_CREATE,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaWaitAtCreate::mpi_rma_wait_at_create__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaWaitAtCreate::mpi_rma_wait_at_create__cb(const CallbackManager& cbmanager,
                                                           int                    user_event,
                                                           const Event&           event,
                                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 58 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mIdle;
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaWaitAtFree
     *  @brief MPI Wait at Window Free Time metric.
     *
     *  Implements the "MPI Wait at Window Free Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaWaitAtFree
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_WAIT_AT_FREE;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_MGMT_WIN;
            }

            virtual string
            get_name() const
            {
                return "MPI Wait at Window Free";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_wait_at_free";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in MPI_Win_free, waiting for the last"
                       " process to enter the call";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_wait_at_free__cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaWaitAtFree::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_WAIT_AT_FREE,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaWaitAtFree::mpi_rma_wait_at_free__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaWaitAtFree::mpi_rma_wait_at_free__cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 79 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mIdle;
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaSync
     *  @brief MPI One-sided Synchronization Time metric.
     *
     *  Implements the "MPI One-sided Synchronization Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaSync
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_SYNCHRONIZATION;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_SYNCHRONIZATION;
            }

            virtual string
            get_name() const
            {
                return "MPI One-sided Synchronization";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_synchronization";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in MPI one-sided synchronization calls";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            virtual bool
            is_hidden() const
            {
                return true;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_collective_end__cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            mpi_rma_gats__cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            mpi_rma_lc_compute__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            mpi_rma_lock__cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            mpi_rma_post_complete__cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata);

            void
            mpi_rma_unlock__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

            /// @}


        private:
            #line 99 "MPI_RMA.pattern"

            typedef struct
            {
                LocalData                 events;              ///< Local event set for caching GATS start events
                map< uint32_t, RmaEpoch > lockEpoch;           ///< Map to store different
                LockEpochQueue            epochQueue;          ///< Queue to save epoch events of remote processes
                ContentionInfoStackMap    syncPointInfoStacks; ///< Map to store syncpoint exchange data
            } win_t;

            map< MpiWindow*, win_t > m_windows;    ///< Map of window-related cache data
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaSync::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_LOCK,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaSync::mpi_rma_lock__cb));
        cbmanager->register_callback(
            MPI_RMA_UNLOCK,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaSync::mpi_rma_unlock__cb));
        cbmanager->register_callback(
            MPI_RMA_GATS,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaSync::mpi_rma_gats__cb));
        cbmanager->register_callback(
            MPI_RMA_COLLECTIVE_END,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaSync::mpi_rma_collective_end__cb));
        cbmanager->register_callback(
            MPI_RMA_LC_COMPUTE,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaSync::mpi_rma_lc_compute__cb));
        cbmanager->register_callback(
            MPI_RMA_POST_COMPLETE,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaSync::mpi_rma_post_complete__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaSync::mpi_rma_collective_end__cb(const CallbackManager& cbmanager,
                                                   int                    user_event,
                                                   const Event&           event,
                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 196 "MPI_RMA.pattern"

        // NOTE: The heuristic for collective calls in RMA is to check
        //       whether all the recorded timespans of all participating
        //       processes overlap. If not all timespans overlap, no
        //       collective synchronization is assumed.
        Event         enter  = event.enterptr();
        const Region* region = &enter->getRegion();

        MpiWindow* win = dynamic_cast< MpiWindow* >(event->get_window());
        assert(win);

        MPI_Comm comm = dynamic_cast< MpiComm* >(win->get_comm())->getHandle();
        assert(comm);

        // Check valid overlap & possible idle time
        const CollectiveInfo& coll(data->mCollinfo);
        if (  (coll.earliest_end.mTime > coll.latest.mTime)
           && (coll.my.mTime < coll.latest.mTime))
        {
            data->mIdle = coll.latest.mTime - coll.my.mTime;
        }
        else
        {
            data->mIdle = 0;
        }

        if (is_mpi_rma_fence(region))
        {
            if (data->mIdle > 0)
            {
                cbmanager.notify(MPI_RMA_WAIT_AT_FENCE, event, data);

                if (  (data->mLastRmaOp.mTime > enter->getTimestamp())
                   && (data->mLastRmaOp.mTime <= event->getTimestamp()))
                {
                    data->mIdle = data->mLastRmaOp.mTime - enter->getTimestamp();
                    cbmanager.notify(MPI_RMA_EARLY_FENCE, event, data);
                }
            }

            // analyze pairwise sync pattern
            int comm_size = 0;
            MPI_Comm_size(comm, &comm_size);

            // count of remote partners is stored in data->mCount
            data->mCount = comm_size - data->mCount;
            cbmanager.notify(MPI_RMA_PWS_UNNEEDED_COUNT, event, data);

            data->mCount = comm_size;
            cbmanager.notify(MPI_RMA_PWS_COUNT, event, data);

            // compute lock competition
            data->mWindow = win;
            cbmanager.notify(MPI_RMA_LC_COMPUTE, event, data);
        }
        else if (is_mpi_rma_create(region))
        {
            MpiWindow* win   = event->get_window();
            win_t&     entry = m_windows[win];

            if (data->mIdle > 0)
            {
                cbmanager.notify(MPI_RMA_WAIT_AT_CREATE, event, data);
            }

            // register lock epoch queue for new window
            AmLockContention::getInstance().registerQueue(win->getId(), &entry.epochQueue);
        }
        else if (is_mpi_rma_free(region))
        {
            if (data->mIdle > 0)
            {
                cbmanager.notify(MPI_RMA_WAIT_AT_FREE, event, data);

                if (data->mLastRmaOp.mTime > enter->getTimestamp())
                {
                    data->mIdle = data->mLastRmaOp.mTime - enter->getTimestamp();
                    cbmanager.notify(MPI_RMA_EARLY_FREE, event, data);
                }
            }

            // compute lock competition
            data->mWindow = win;
            cbmanager.notify(MPI_RMA_LC_COMPUTE, event, data);
        }
        data->mLastRmaOp.mTime = -DBL_MAX;
    }


    void
    PatternMPI_RmaSync::mpi_rma_gats__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 151 "MPI_RMA.pattern"

        Event         enter  = event.enterptr();
        MpiWindow*    win    = event->get_window();
        const Region* region = &(enter->getRegion());
        MpiGroup*     group  = event->get_group();
        win_t&        entry  = m_windows[win];

        // check which GATS region we are in
        if (is_mpi_rma_wait(region))
        {
            // analyze pairwise RMA sync pattern on target
            data->mCount = group->numRanks();
            cbmanager.notify(MPI_RMA_PWS_COUNT, event, data);

            data->mCount -= data->mCountOrigin;
            cbmanager.notify(MPI_RMA_PWS_UNNEEDED_COUNT, event, data);

            // notify callback for full GATS pattern analysis on target
            cbmanager.notify(MPI_RMA_POST_WAIT, event, data);
        }
        else if (is_mpi_rma_start(region))
        {
            // cache start events for later analysis
            entry.events.add_event(enter, ROLE_ENTER_RMA_START);
            entry.events.add_event(event, ROLE_LEAVE_RMA_START);
        }
        else if (is_mpi_rma_complete(region))
        {
            // analyze pairwise RMA sync pattern on origin
            data->mCount = group->numRanks();
            cbmanager.notify(MPI_RMA_PWS_COUNT, event, data);

            data->mCount -= data->mCountTarget;
            cbmanager.notify(MPI_RMA_PWS_UNNEEDED_COUNT, event, data);

            // set cached data as current
            *data->mLocal = entry.events;

            cbmanager.notify(MPI_RMA_POST_COMPLETE, event, data);

            // Clean up local data event cache
            entry.events.clear();
        }
    }


    void
    PatternMPI_RmaSync::mpi_rma_lc_compute__cb(const CallbackManager& cbmanager,
                                               int                    user_event,
                                               const Event&           event,
                                               CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 284 "MPI_RMA.pattern"

        MpiWindow* win   = data->mWindow;
        win_t&     entry = m_windows[win];
        bool       current_exclusive_epoch;

        #ifdef DEBUG_LOCK_CONTENTION
            bool evaluate_locks = (entry.epochQueue.size() > 0);
            if (evaluate_locks)
            {
                cerr << "Starting MPI_RMA_LC_COMPUTE" << endl;
            }
        #endif

        while (entry.epochQueue.size() >= 2)
        {
            // Priority queue of epochs to be processed
            LockEpochQueue scheduled_epochs;

            // Remove next epoch from queue and schedule it
            RemoteRmaEpochPtr current_epoch(entry.epochQueue.top());
            entry.epochQueue.pop();

            // check if current_epoch is exclusive
            RemoteRmaEpoch::const_iterator lock_it = current_epoch->begin();
            while (  (lock_it != current_epoch->end())
                  && (*lock_it)->isOfType(MPI_RMA_LOCK))
            {
                ++lock_it;
            }

            if ((*lock_it)->is_exclusive())
            {
                current_exclusive_epoch = true;
            }
            else
            {
                current_exclusive_epoch = false;
            }

            scheduled_epochs.push(current_epoch);

            #ifdef DEBUG_LOCK_CONTENTION
                RemoteRmaEpoch::const_iterator ex_it = current_epoch->begin();
                while (  (ex_it != current_epoch->end())
                      && (*ex_it)->isOfType(MPI_RMA_UNLOCK))
                {
                    ++ex_it;
                }

                cerr << "Unlock Time: " << (*ex_it)->getTimestamp() << endl;
            #endif

            RemoteRmaEpochPtr reference_epoch(entry.epochQueue.top());
            bool              exclusive_epoch_found = false;

            if (!current_exclusive_epoch)
            {
                do
                {
                    reference_epoch = entry.epochQueue.top();

                    // get lock event
                    RemoteRmaEpoch::const_iterator lock_it = reference_epoch->begin();
                    while (  (lock_it != reference_epoch->end())
                          && (*lock_it)->isOfType(MPI_RMA_LOCK))
                    {
                        ++lock_it;
                    }

                    // if lock is shared, schedule epoch and get next
                    if (!(*lock_it)->is_exclusive())
                    {
                        scheduled_epochs.push(reference_epoch);
                        entry.epochQueue.pop();
                    }
                    else
                    {
                        exclusive_epoch_found = true;
                    }
                }
                while (!exclusive_epoch_found && !entry.epochQueue.empty());
            }

            if (current_exclusive_epoch || exclusive_epoch_found)
            {
                while (scheduled_epochs.size() > 0)
                {
                    current_epoch = scheduled_epochs.top();
                    scheduled_epochs.pop();
                    RemoteRmaEpoch::reverse_iterator ce_it = current_epoch->rbegin();

                    // get previous exclusive lock owner's unlock event
                    RemoteRmaEpoch::reverse_iterator ref_it = reference_epoch->rbegin();
                    while ((*ref_it)->isOfType(MPI_RMA_UNLOCK))
                    {
                        ++ref_it;
                    }

                    #ifdef DEBUG_LOCK_CONTENTION
                        cerr << "ref_it: " << (*ref_it)->getTimestamp() << "  " << ref_it->get_location()->get_id()
                             << " ce_it: " << (*ce_it)->getTimestamp() << "  " << ce_it->get_location()->get_id()
                             << endl;
                    #endif

                    // Search event of current lock owner preceeding previous owners unlock
                    while (  (ce_it != current_epoch->rend())
                          && ((*ce_it)->getTimestamp() > (*ref_it)->getTimestamp()))
                    {
                        ++ce_it;
                    }

                    if (  (ce_it != current_epoch->rend())
                       && (  (*ce_it)->isOfType(GROUP_ENTER)
                          || (*ce_it)->isOfType(MPI_RMA_LOCK)
                          || (*ce_it)->isOfType(MPI_RMA_PUT_START)    // FIXME: this may have to be PUT_END!
                          || (*ce_it)->isOfType(MPI_RMA_GET_START)))  // FIXME: this may have to be GET_END!
                    {
                        #ifdef DEBUG_LOCK_CONTENTION
                            switch ((*ce_it)->getType())
                            {
                                case ENTER:
                                    cerr << "Lock found on ENTER" << endl;
                                    break;

                                case MPI_RMA_LOCK:
                                    cerr << "Lock found on LOCK" << endl;
                                    break;

                                case MPI_RMA_PUT_START:
                                    cerr << "Lock found on PUT" << endl;
                                    break;

                                case MPI_RMA_GET_START:
                                    cerr << "Lock found on GET" << endl;
                                    break;

                                default:
                                    cerr << "Lock found on unexpected event!" << endl;
                            }
                        #endif
                        timestamp_t idle_time = (*ref_it)->getTimestamp() - (*ce_it)->getTimestamp();

                        if (idle_time > 0)
                        {
                            RemoteEvent& wait_event = *ce_it;

                            // determine next Event type of MPI_RMA_LOCK, MPI_RMA_PUT_START or MPI_RMA_GET_START
                            if ((*ce_it)->isOfType(GROUP_ENTER))
                            {
                                // set iterator to ce_it
                                RemoteRmaEpoch::iterator it = current_epoch->begin();
                                while (  (it != current_epoch->end())
                                      && ((*it)->getTimestamp() <= (*ce_it)->getTimestamp()))
                                {
                                    ++it;
                                }

                                // set it to MPI_RMA_LOCK, MPI_RMA_PUT_START or MPI_RMA_GET_START or MPI_RMA_UNLOCK
                                while (  (it != current_epoch->end())
                                      && !(  (*it)->isOfType(MPI_RMA_LOCK)
                                          || (*it)->isOfType(MPI_RMA_PUT_START)
                                          || (*it)->isOfType(MPI_RMA_GET_START)
                                          || (*it)->isOfType(MPI_RMA_UNLOCK)))
                                {
                                    ++it;
                                }

                                wait_event = *it;
                            }

                            // prepare info to push on stacks for both events
                            RemoteEvent first_event     = *ref_it;
                            ident_t     wait_id         = wait_event.get_remote_index();
                            ident_t     first_id        = first_event.get_remote_index();
                            uint32_t    first_rank      = ref_it->get_location().getId();
                            uint32_t    wait_rank       = ce_it->get_location().getId();
                            uint32_t    first_sync_rank = wait_rank;
                            uint32_t    wait_sync_rank  = first_rank;

                            #ifdef DEBUG_LOCK_CONTENTION
                                cerr << "Stack info first: " << first_id << " " << idle_time << " " << first_sync_rank
                                     << "\nStack info wait: " << wait_id << " " << idle_time << " " << wait_sync_rank << endl;
                            #endif

                            // build info-structs - negative idletime means other processes have waited
                            ContentionInfo wait_info  = { wait_id, idle_time, wait_sync_rank };
                            ContentionInfo first_info = { first_id, -idle_time, first_sync_rank };

                            // push wait_info on stack of correspondent map
                            if (entry.syncPointInfoStacks.find(wait_rank) == entry.syncPointInfoStacks.end())
                            {
                                ContentionInfoStack wait_stack;
                                wait_stack.push(wait_info);
                                entry.syncPointInfoStacks[wait_rank] = wait_stack;
                            }
                            else
                            {
                                entry.syncPointInfoStacks[wait_rank].push(wait_info);
                            }

                            // push first_info on stack of correspondent map
                            if (entry.syncPointInfoStacks.find(first_rank) == entry.syncPointInfoStacks.end())
                            {
                                ContentionInfoStack first_stack;
                                first_stack.push(first_info);
                                entry.syncPointInfoStacks[first_rank] = first_stack;
                            }
                            else
                            {
                                entry.syncPointInfoStacks[first_rank].push(first_info);
                            }
                        }    // if (idle_time > 0)

                        // delete current_epoch;
                    } // if ce_it type_of...
                }     // while (scheduled_epochs.size() > 0)
            }         // if (current_exclusive_epoch || exclusive_epoch_found)
        }             // while (entry.epochQueue.size() >= 2)

        // pop last entry in queue
        if (entry.epochQueue.size() > 0)
        {
            entry.epochQueue.pop();
        }

        // send synchpoint information for lock-epochs to origins
        ContentionInfoStackMap::iterator mit = entry.syncPointInfoStacks.begin();
        while (mit != entry.syncPointInfoStacks.end())
        {
            ContentionInfoStack infoStack = mit->second;

            MpiComm* comm = dynamic_cast< MpiComm* >(win->get_comm());
            assert(comm);

            while (!infoStack.empty())
            {
                ContentionInfo& info    = infoStack.top();
                AmRequest::Ptr  request = AmRequestFactory::create(comm->getCommSet().getLocalRank(mit->first),
                                                                   *comm);
                AmSyncpointExchange::pack(MPI_RMA_LOCK_COMPETITION, 0, info, request.get());

                AmRuntime::getInstance().enqueue(request);
                infoStack.pop();
            }

            ++mit;
        }
        entry.syncPointInfoStacks.clear();

        #ifdef DEBUG_LOCK_CONTENTION
            if (evaluate_locks)
            {
                cerr << "MPI_RMA_LC_COMPUTE done." << endl;
            }
        #endif
    }


    void
    PatternMPI_RmaSync::mpi_rma_lock__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 111 "MPI_RMA.pattern"

        // get reference to window data cache
        MpiWindow* win   = event->get_window();
        win_t&     entry = m_windows[win];

        // save complete lock region to epoch store
        entry.lockEpoch[event->get_remote()].push_back(event.enterptr());
        entry.lockEpoch[event->get_remote()].push_back(event);
        entry.lockEpoch[event->get_remote()].push_back(event.leaveptr());

        // notify direct RMA synchronization
        cbmanager.notify(RMA_SYNC_DIRECT, event, data);
    }


    void
    PatternMPI_RmaSync::mpi_rma_post_complete__cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 541 "MPI_RMA.pattern"

        Event       enter       = event.enterptr();
        timestamp_t enter_time  = enter->getTimestamp();
        Event       start_enter = data->mLocal->get_event(ROLE_ENTER_RMA_START);
        Event       start_exit  = data->mLocal->get_event(ROLE_LEAVE_RMA_START);

        #ifdef GATSDEBUG
            MpiWindow* win = event->get_window();
            assert(win);

            MpiComm* comm = dynamic_cast< MpiComm* >(win->get_comm());
            assert(comm);

            int myrank;
            MPI_Comm_rank(comm->getComm(), &myrank);
        #endif

        if (  (data->mLastPost.mTime > enter_time)
           && (data->mLastPost.mTime < event->getTimestamp()))
        {
            /* non-blocking start and post concurrent to complete */
            data->mIdle       = data->mLastPost.mTime - enter_time;
            data->mCallpathId = enter.get_cnode()->getId();

            #ifdef GATSDEBUG
                ostringstream out;
                out << "LATE POST : Event ID = " << event.get_id()
                    << " global rank = " << myrank
                    << " idle time = " << data->mIdle;
                cerr << out.str() << endl;
            #endif

            cbmanager.notify(MPI_RMA_LATE_POST, event, data);
        }
        else if (  (data->mLastPost.mTime < start_exit->getTimestamp())
                && (data->mLastPost.mTime > start_enter->getTimestamp()))
        {
            /* blocking start */
            data->mIdle       = data->mLastPost.mTime - start_enter->getTimestamp();
            data->mCallpathId = start_enter.get_cnode()->getId();

            #ifdef GATSDEBUG
                ostringstream out;
                out << "LATE POST : Event ID = " << start_exit.get_id()
                    << " global rank = " << myrank
                    << " idle time = " << data->mIdle;
                cerr << out.str() << endl;
            #endif

            cbmanager.notify(MPI_RMA_LATE_POST, start_exit, data);
        }
        else
        {
            // check all RMA operations between start and complete
            MpiWindow* win         = event->get_window();
            Event      evt_it      = event.prev().prev();
            bool       start_found = false;

            while (!start_found)
            {
                if (  (  evt_it->isOfType(MPI_RMA_PUT_START)
                      || evt_it->isOfType(MPI_RMA_GET_START))
                   && (evt_it->get_window() == win))
                {
                    if (  (data->mLastPost.mTime > evt_it->getTimestamp())
                       && (data->mLastPost.mTime < evt_it.leaveptr()->getTimestamp()))
                    {
                        data->mIdle       = data->mLastPost.mTime - evt_it->getTimestamp();
                        data->mCallpathId = evt_it.enterptr().get_cnode()->getId();

                        #ifdef GATSDEBUG
                            ostringstream out;
                            out << "LATE POST : Event ID = " << evt_it.get_id()
                                << " global rank = " << myrank
                                << " idle time = " << data->mIdle;
                            cerr << out.str() << endl;
                        #endif

                        cbmanager.notify(MPI_RMA_LATE_POST, evt_it, data);
                    }
                }

                // FIXME: The 'leaveptr().prev()' construct is only guaranteed to
                //        work for EPIK traces and needs to be revised once OTF2
                //        RMA records are supported!
                if (  evt_it->isOfType(GROUP_ENTER)
                   && is_mpi_rma_start(evt_it->getRegion())
                   && (evt_it.leaveptr().prev()->get_window() == win))
                {
                    start_found = true;
                }

                /* check previous event */
                --evt_it;
            }
        }
    }


    void
    PatternMPI_RmaSync::mpi_rma_unlock__cb(const CallbackManager& cbmanager,
                                           int                    user_event,
                                           const Event&           event,
                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 125 "MPI_RMA.pattern"

        // get reference to window data cache
        MpiWindow* win   = event->get_window();
        win_t&     entry = m_windows[win];

        // save complete unlock region to epoch store
        entry.lockEpoch[event->get_remote()].push_back(event.enterptr());
        entry.lockEpoch[event->get_remote()].push_back(event);
        entry.lockEpoch[event->get_remote()].push_back(event.leaveptr());

        // create active message request
        AmRequest::Ptr request = AmRequestFactory::create(event->get_remote(),
                                                          *(win->get_comm()));

        // pack lock epoch data to check for lock competition
        AmLockContention::pack(win->getId(),
                               entry.lockEpoch[event->get_remote()],
                               request.get());

        // enqueue request
        AmRuntime::getInstance().enqueue(request);

        // clear lock epoch cache
        entry.lockEpoch[event->get_remote()].clear();
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaSyncLatePost
     *  @brief MPI Late Post Time in Synchronizations metric.
     *
     *  Implements the "MPI Late Post Time in Synchronizations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaSyncLatePost
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_SYNC_LATE_POST;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_SYNC_ACTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Post (Synchronization)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_sync_late_post";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in MPI one-sided active target"
                       " synchronization waiting for exposure epoch to start";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_late_post__cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaSyncLatePost::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_LATE_POST,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaSyncLatePost::mpi_rma_late_post__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaSyncLatePost::mpi_rma_late_post__cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 674 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (is_mpi_rma_sync(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaEarlyWait
     *  @brief MPI Early Wait Time metric.
     *
     *  Implements the "MPI Early Wait Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaEarlyWait
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_EARLY_WAIT;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_SYNC_ACTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Early Wait";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_early_wait";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in MPI_Win_wait waiting for last exposure"
                       " epoch to finish";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_post_wait__cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaEarlyWait::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_POST_WAIT,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaEarlyWait::mpi_rma_post_wait__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaEarlyWait::mpi_rma_post_wait__cb(const CallbackManager& cbmanager,
                                                   int                    user_event,
                                                   const Event&           event,
                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 702 "MPI_RMA.pattern"

        Event       enter      = event.enterptr();
        timestamp_t enter_time = enter->getTimestamp();

        if (data->mLastComplete.mTime > enter_time)
        {
            // compute early wait
            data->mIdle                    = data->mLastComplete.mTime - enter_time;
            m_severity[enter.get_cnode()] += data->mIdle;

            #ifdef GATSDEBUG
                MpiWindow* win = event->get_window();
                assert(win);

                MpiComm* comm = dynamic_cast< MpiComm* >(win->get_comm());
                assert(comm);

                int myrank;
                MPI_Comm_rank(comm->getComm(), &myrank);

                ostringstream out;
                out << "\nPattern Early_Wait : Event ID: " << event.get_id()
                    << " mein Rang: " << myrank
                    << " idle time = " << data->mIdle;
                cerr << out.str() << endl;
            #endif

            cbmanager.notify(MPI_RMA_EARLY_WAIT, event, data);
        }
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaLateComplete
     *  @brief MPI Late Complete Time metric.
     *
     *  Implements the "MPI Late Complete Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaLateComplete
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_LATE_COMPLETE;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_EARLY_WAIT;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Complete";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_late_complete";
            }

            virtual string
            get_descr() const
            {
                return "Time spent waiting in MPI_Win_wait between last RMA"
                       " access and last MPI_Win_complete";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_early_wait__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaLateComplete::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_EARLY_WAIT,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaLateComplete::mpi_rma_early_wait__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaLateComplete::mpi_rma_early_wait__cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 754 "MPI_RMA.pattern"

        Event       enter      = event.enterptr();
        timestamp_t enter_time = enter->getTimestamp();
        timestamp_t max_ts     = max(data->mLastRmaOp.mTime, enter_time);

        // save severity (it does not have to be checked as this is only
        // triggered if an Early Wait is found.)
        m_severity[enter.get_cnode()] +=
            (data->mLastComplete.mTime - max_ts);
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaWaitAtFence
     *  @brief MPI Wait at Fence Time metric.
     *
     *  Implements the "MPI Wait at Fence Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaWaitAtFence
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_WAIT_AT_FENCE;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_SYNC_ACTIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Wait at Fence";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_wait_at_fence";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in front of MPI_Win_fence, waiting for"
                       " other processes";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_wait_at_fence__cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaWaitAtFence::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_WAIT_AT_FENCE,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaWaitAtFence::mpi_rma_wait_at_fence__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaWaitAtFence::mpi_rma_wait_at_fence__cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 784 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mIdle;
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaEarlyFence
     *  @brief MPI Early Fence Time metric.
     *
     *  Implements the "MPI Early Fence Time" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaEarlyFence
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_EARLY_FENCE;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_WAIT_AT_FENCE;
            }

            virtual string
            get_name() const
            {
                return "MPI Early Fence";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_early_fence";
            }

            virtual string
            get_descr() const
            {
                return "Waiting time in MPI_Win_fence due to pending RMA"
                       " operations";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_early_fence__cb(const CallbackManager& cbmanager,
                                    int                    user_event,
                                    const Event&           event,
                                    CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaEarlyFence::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_EARLY_FENCE,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaEarlyFence::mpi_rma_early_fence__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaEarlyFence::mpi_rma_early_fence__cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 807 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mIdle;
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMpi_RmaSyncLockContention
     *  @brief MPI Lock Contention Time in Synchronizations metric.
     *
     *  Implements the "MPI Lock Contention Time in Synchronizations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMpi_RmaSyncLockContention
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_SYNC_LOCK_COMPETITION;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_SYNC_PASSIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Lock Contention (Synchronization)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_sync_lock_competition";
            }

            virtual string
            get_descr() const
            {
                return "Time spent waiting in MPI_Win_lock/MPI_Win_unlock"
                       " acquiring a lock for a window";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_lock_competition__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMpi_RmaSyncLockContention::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_LOCK_COMPETITION,
            PEARL_create_callback(
                this,
                &PatternMpi_RmaSyncLockContention::mpi_rma_lock_competition__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMpi_RmaSyncLockContention::mpi_rma_lock_competition__cb(const CallbackManager& cbmanager,
                                                                   int                    user_event,
                                                                   const Event&           event,
                                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 844 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (  (data->mIdle > 0)
           && is_mpi_rma_passive(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMpi_RmaSyncWaitForProgress
     *  @brief MPI Wait for Progress Time in Synchronizations metric.
     *
     *  Implements the "MPI Wait for Progress Time in Synchronizations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMpi_RmaSyncWaitForProgress
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_SYNC_WAIT_FOR_PROGRESS;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_SYNC_PASSIVE;
            }

            virtual string
            get_name() const
            {
                return "MPI Wait for Progress (Synchronization)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_sync_wait_for_progress";
            }

            virtual string
            get_descr() const
            {
                return "Time spent waiting in MPI_Win_lock/MPI_Win_unlock"
                       " waiting for progress on the target";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_wait_for_progress__cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMpi_RmaSyncWaitForProgress::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_WAIT_FOR_PROGRESS,
            PEARL_create_callback(
                this,
                &PatternMpi_RmaSyncWaitForProgress::mpi_rma_wait_for_progress__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMpi_RmaSyncWaitForProgress::mpi_rma_wait_for_progress__cb(const CallbackManager& cbmanager,
                                                                     int                    user_event,
                                                                     const Event&           event,
                                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 872 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (is_mpi_rma_passive(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaCommLatePost
     *  @brief MPI Late Post Time in Communications metric.
     *
     *  Implements the "MPI Late Post Time in Communications" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaCommLatePost
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_COMM_LATE_POST;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_COMMUNICATION;
            }

            virtual string
            get_name() const
            {
                return "MPI Late Post (Communication)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_comm_late_post";
            }

            virtual string
            get_descr() const
            {
                return "Time spent in MPI one-sided communication operations"
                       " waiting for exposure epoch to start";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_late_post__cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaCommLatePost::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_LATE_POST,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaCommLatePost::mpi_rma_late_post__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaCommLatePost::mpi_rma_late_post__cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 917 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (is_mpi_rma_comm(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMpi_RmaCommLockContention
     *  @brief MPI Lock Contention Time in Communications metric.
     *
     *  Implements the "MPI Lock Contention Time in Communications" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMpi_RmaCommLockContention
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_COMM_LOCK_COMPETITION;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_COMMUNICATION;
            }

            virtual string
            get_name() const
            {
                return "MPI Lock Contention (Communication)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_comm_lock_competition";
            }

            virtual string
            get_descr() const
            {
                return "Time spent waiting in MPI_Win_lock/MPI_Win_unlock"
                       " acquiring a lock for a window";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_lock_competition__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMpi_RmaCommLockContention::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_LOCK_COMPETITION,
            PEARL_create_callback(
                this,
                &PatternMpi_RmaCommLockContention::mpi_rma_lock_competition__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMpi_RmaCommLockContention::mpi_rma_lock_competition__cb(const CallbackManager& cbmanager,
                                                                   int                    user_event,
                                                                   const Event&           event,
                                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 943 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (  (data->mIdle > 0)
           && is_mpi_rma_comm(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMpi_RmaCommWaitForProgress
     *  @brief MPI Wait for Progress Time in Communications metric.
     *
     *  Implements the "MPI Wait for Progress Time in Communications" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMpi_RmaCommWaitForProgress
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_COMM_WAIT_FOR_PROGRESS;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_COMMUNICATION;
            }

            virtual string
            get_name() const
            {
                return "MPI Wait for Progress (Communication)";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_comm_wait_for_progress";
            }

            virtual string
            get_descr() const
            {
                return "Time spent waiting in MPI_Win_lock/MPI_Win_unlock"
                       " waiting for progress on the target";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_wait_for_progress__cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMpi_RmaCommWaitForProgress::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_WAIT_FOR_PROGRESS,
            PEARL_create_callback(
                this,
                &PatternMpi_RmaCommWaitForProgress::mpi_rma_wait_for_progress__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMpi_RmaCommWaitForProgress::mpi_rma_wait_for_progress__cb(const CallbackManager& cbmanager,
                                                                     int                    user_event,
                                                                     const Event&           event,
                                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 971 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (is_mpi_rma_comm(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    }
#endif    // _MPI && HAS_MPI2_1SIDED


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaPairsyncCount
     *  @brief Pair-wise MPI One-sided Synchronizations metric.
     *
     *  Implements the "Pair-wise MPI One-sided Synchronizations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaPairsyncCount
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_PAIRSYNC_COUNT;
            }

            virtual long
            get_parent() const
            {
                return PAT_NONE;
            }

            virtual string
            get_name() const
            {
                return "MPI pair-wise one-sided synchronizations";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_pairsync_count";
            }

            virtual string
            get_descr() const
            {
                return "Number of pair-wise MPI one-sided synchronizations";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_pws_count__cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaPairsyncCount::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_PWS_COUNT,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaPairsyncCount::mpi_rma_pws_count__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaPairsyncCount::mpi_rma_pws_count__cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1008 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mCount;
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_RmaPairsyncUnneededCount
     *  @brief Unneeded Pair-wise MPI One-sided Synchronizations metric.
     *
     *  Implements the "Unneeded Pair-wise MPI One-sided Synchronizations" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_RmaPairsyncUnneededCount
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_RMA_PAIRSYNC_UNNEEDED_COUNT;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_RMA_PAIRSYNC_COUNT;
            }

            virtual string
            get_name() const
            {
                return "MPI unneeded pair-wise one-sided synchronizations";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_rma_pairsync_unneeded_count";
            }

            virtual string
            get_descr() const
            {
                return "Number of unneeded pair-wise MPI one-sided"
                       " synchronizations";
            }

            virtual string
            get_unit() const
            {
                return "occ";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            mpi_rma_pws_unneeded_count__cb(const CallbackManager& cbmanager,
                                           int                    user_event,
                                           const Event&           event,
                                           CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_RmaPairsyncUnneededCount::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            MPI_RMA_PWS_UNNEEDED_COUNT,
            PEARL_create_callback(
                this,
                &PatternMPI_RmaPairsyncUnneededCount::mpi_rma_pws_unneeded_count__cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_RmaPairsyncUnneededCount::mpi_rma_pws_unneeded_count__cb(const CallbackManager& cbmanager,
                                                                        int                    user_event,
                                                                        const Event&           event,
                                                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1039 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mCount;
    }
#endif    // _MPI


/*-------------------------------------------------------------------------*/
/**
 *  @class PatternStatistics
 *  @brief STATISTICS metric.
 *
 *  Implements the "STATISTICS" metric.
 */
/*-------------------------------------------------------------------------*/

class PatternStatistics
    : public Pattern
{
    public:
        /// @name Constructors & destructor
        /// @{

        virtual
        ~PatternStatistics()
        {
            #line 1346 "Statistics.pattern"

            // Release statistics objects
            #if defined(_MPI)
                delete ls_quant;
                delete lsw_quant;
                delete lr_quant;
                delete wnxn_quant;
                delete wb_quant;
                delete er_quant;
                delete es_quant;
                delete lb_quant;
                delete bc_quant;
                delete nxnc_quant;
            #endif

            #if defined(_OPENMP)
                delete omp_eb_quant;
                delete omp_ib_quant;
                delete thread_lc_ol_quant;
                delete thread_lc_oc_quant;
                delete thread_lc_pm_quant;
                delete thread_lc_pc_quant;
            #endif    // _OPENMP
        }

        /// @}
        /// @name Registering callbacks
        /// @{

        virtual void
        reg_cb(CallbackManagerMap& cbmanagers);

        /// @}
        /// @name Retrieving pattern information
        /// @{

        virtual long
        get_id() const
        {
            return PAT_STATISTICS;
        }

        virtual long
        get_parent() const
        {
            return PAT_NONE;
        }

        virtual string
        get_name() const
        {
            return "STATISTICS";
        }

        virtual string
        get_unique_name() const
        {
            return "statistics";
        }

        virtual string
        get_descr() const
        {
            return "Statistics for waiting time distribution";
        }

        virtual string
        get_unit() const
        {
            return "sec";
        }

        virtual CubeMetricType
        get_mode() const
        {
            return CUBE_METRIC_EXCLUSIVE;
        }

        virtual bool
        is_hidden() const
        {
            return true;
        }

        /// @}
        /// @name Callback methods
        /// @{

        void
        barrier_compl__cb(const CallbackManager& cbmanager,
                          int                    user_event,
                          const Event&           event,
                          CallbackData*          cdata);

        void
        early_reduce__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

        void
        early_scan__cb(const CallbackManager& cbmanager,
                       int                    user_event,
                       const Event&           event,
                       CallbackData*          cdata);

        void
        late_bcast__cb(const CallbackManager& cbmanager,
                       int                    user_event,
                       const Event&           event,
                       CallbackData*          cdata);

        void
        late_receiver_bws_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

        void
        late_sender__cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        late_sender_wo__cb(const CallbackManager& cbmanager,
                           int                    user_event,
                           const Event&           event,
                           CallbackData*          cdata);

        void
        nxn_compl__cb(const CallbackManager& cbmanager,
                      int                    user_event,
                      const Event&           event,
                      CallbackData*          cdata);

        void
        omp_ebarrier_wait__cb(const CallbackManager& cbmanager,
                              int                    user_event,
                              const Event&           event,
                              CallbackData*          cdata);

        void
        omp_ibarrier_wait__cb(const CallbackManager& cbmanager,
                              int                    user_event,
                              const Event&           event,
                              CallbackData*          cdata);

        void
        prepare__cb(const CallbackManager& cbmanager,
                    int                    user_event,
                    const Event&           event,
                    CallbackData*          cdata);

        void
        thread_lock_contention_ompcritical_wait__cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata);

        void
        thread_lock_contention_omplock_wait__cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata);

        void
        thread_lock_contention_pthreadcondition_wait__cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata);

        void
        thread_lock_contention_pthreadmutex_wait__cb(const CallbackManager& cbmanager,
                                                     int                    user_event,
                                                     const Event&           event,
                                                     CallbackData*          cdata);

        void
        wait_barrier__cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

        void
        wait_nxn__cb(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          cdata);

        /// @}


    protected:
        /// @name Pattern management
        /// @{

        virtual void
        init()
        {
            #line 1321 "Statistics.pattern"

            // Create statistics objects for MPI metrics
            #if defined(_MPI)
                ls_quant   = new Quantile(NUMBER_COEFF);
                lsw_quant  = new Quantile(NUMBER_COEFF);
                lr_quant   = new Quantile(NUMBER_COEFF);
                wnxn_quant = new Quantile(NUMBER_COEFF);
                wb_quant   = new Quantile(NUMBER_COEFF);
                er_quant   = new Quantile(NUMBER_COEFF);
                es_quant   = new Quantile(NUMBER_COEFF);
                lb_quant   = new Quantile(NUMBER_COEFF);
                bc_quant   = new Quantile(NUMBER_COEFF);
                nxnc_quant = new Quantile(NUMBER_COEFF);
            #endif

            #if defined(_OPENMP)
                omp_eb_quant       = new Quantile(NUMBER_COEFF);
                omp_ib_quant       = new Quantile(NUMBER_COEFF);
                thread_lc_ol_quant = new Quantile(NUMBER_COEFF);
                thread_lc_oc_quant = new Quantile(NUMBER_COEFF);
                thread_lc_pm_quant = new Quantile(NUMBER_COEFF);
                thread_lc_pc_quant = new Quantile(NUMBER_COEFF);
            #endif    // _OPENMP
        }

        /// @}


    private:
        #line 72 "Statistics.pattern"

        /// Symbolic names for entries in arrays storing upper bounds of metric
        /// durations
        enum duration_t
        {
            LS_MAX_DURATION = 0,
            LR_MAX_DURATION,
            WNXN_MAX_DURATION,
            WB_MAX_DURATION,
            ER_MAX_DURATION,
            ES_MAX_DURATION,
            LB_MAX_DURATION,
            BC_MAX_DURATION,
            NXNC_MAX_DURATION,
            OMP_EB_MAX_DURATION,
            OMP_IB_MAX_DURATION,
            THREAD_LC_OL_MAX_DURATION,
            THREAD_LC_OC_MAX_DURATION,
            THREAD_LC_PM_MAX_DURATION,
            THREAD_LC_PC_MAX_DURATION,
            MAX_DURATION_ENTRIES
        };


        /// CallbackData struct used for preparation replay
        struct MaxDurations
            : public pearl::CallbackData
        {
            double m_max_duration[MAX_DURATION_ENTRIES];

            MaxDurations()
            {
                memset(m_max_duration, 0, MAX_DURATION_ENTRIES * sizeof(double));
            }

            ~MaxDurations()
            {
            }

            void
            update_duration(duration_t idx,
                            double     value)
            {
                if (value > m_max_duration[idx])
                {
                    m_max_duration[idx] = value;
                }
            }
        };


        // returns the position with first identical cnode
        class find_cnode_position
        {
            public:
                find_cnode_position(pearl::ident_t n)
                    : cnode(n)
                {
                }

                bool
                operator()(TopMostSevere element) const
                {
                    return (element.cnode == cnode);
                }


            private:
                pearl::ident_t cnode;
        };


        // returns the position of first element with cnode not in this list and
        // bigger idletime
        class find_idletime_position
        {
            public:
                find_idletime_position(pearl::ident_t     n,
                                       pearl::timestamp_t m)
                    : cnode(n),
                      idle(m)
                {
                }

                bool
                operator()(TopMostSevere element) const
                {
                    return (  (element.cnode != cnode)
                           && (element.idletime < idle));
                }


            private:
                pearl::ident_t     cnode;
                pearl::timestamp_t idle;
        };


        void
        mpicollective_cb(const pearl::CallbackManager& cbmanager,
                         int                           user_event,
                         const pearl::Event&           event,
                         pearl::CallbackData*          cdata)
        {
            Event         begin    = event.beginptr();
            double        duration = event->getTimestamp() - begin->getTimestamp();
            MaxDurations* data     = static_cast< MaxDurations* >(cdata);

            const Region& region = event.get_cnode()->getRegion();
            if (is_mpi_barrier(region))
            {
                // Wait at Barrier
                data->update_duration(WB_MAX_DURATION, duration);

                // Barrier Completion
                data->update_duration(BC_MAX_DURATION, duration);
            }
            else if (is_mpi_12n(region))
            {
                // Late Broadcast
                data->update_duration(LB_MAX_DURATION, duration);
            }
            else if (is_mpi_n21(region))
            {
                // Early Reduce
                data->update_duration(ER_MAX_DURATION, duration);
            }
            else if (is_mpi_scan(region))
            {
                // Early Scan
                data->update_duration(ES_MAX_DURATION, duration);
            }
            else if (is_mpi_n2n(region))
            {
                // Wait at NxN
                data->update_duration(WNXN_MAX_DURATION, duration);

                // NxN Completion
                data->update_duration(NXNC_MAX_DURATION, duration);
            }
        }

        void
        send_cb(const pearl::CallbackManager& cbmanager,
                int                           user_event,
                const pearl::Event&           event,
                pearl::CallbackData*          cdata)
        {
            Event         enter    = event.enterptr();
            Event         leave    = event.leaveptr();
            double        duration = leave->getTimestamp() - enter->getTimestamp();
            MaxDurations* data     = static_cast< MaxDurations* >(cdata);

            // Late Receiver
            data->update_duration(LR_MAX_DURATION, duration);
        }

        void
        recv_cb(const pearl::CallbackManager& cbmanager,
                int                           user_event,
                const pearl::Event&           event,
                pearl::CallbackData*          cdata)
        {
            Event         enter    = event.enterptr();
            Event         leave    = event.leaveptr();
            double        duration = leave->getTimestamp() - enter->getTimestamp();
            MaxDurations* data     = static_cast< MaxDurations* >(cdata);

            // Late Sender
            data->update_duration(LS_MAX_DURATION, duration);
        }

        void
        leave_cb(const pearl::CallbackManager& cbmanager,
                 int                           user_event,
                 const pearl::Event&           event,
                 pearl::CallbackData*          cdata)
        {
            const Region& region = event.get_cnode()->getRegion();
            if (!is_omp_barrier(region))
            {
                return;
            }

            Event         enter    = event.enterptr();
            double        duration = event->getTimestamp() - enter->getTimestamp();
            MaxDurations* data     = static_cast< MaxDurations* >(cdata);

            if (is_omp_ebarrier(region))
            {
                // OpenMP explicit barrier
                data->update_duration(OMP_EB_MAX_DURATION, duration);
            }
            else if (is_omp_ibarrier(region))
            {
                // OpenMP implicit barrier
                data->update_duration(OMP_IB_MAX_DURATION, duration);
            }
        }

        void
        acquire_cb(const pearl::CallbackManager& cbmanager,
                   int                           user_event,
                   const pearl::Event&           event,
                   pearl::CallbackData*          cdata)
        {
            Event         enter    = event.enterptr();
            double        duration = event->getTimestamp() - enter->getTimestamp();
            MaxDurations* data     = static_cast< MaxDurations* >(cdata);

            LockType lockType = getLockType(event);
            if (lockType == LockType::OMP_LOCK_API)
            {
                data->update_duration(THREAD_LC_OL_MAX_DURATION, duration);
            }
            else if (lockType == LockType::OMP_CRITICAL)
            {
                data->update_duration(THREAD_LC_OC_MAX_DURATION, duration);
            }
            else if (  (lockType == LockType::PTHREAD_MUTEX_LOCK)
                    || (lockType == LockType::PTHREAD_MUTEX_TRYLOCK))
            {
                data->update_duration(THREAD_LC_PM_MAX_DURATION, duration);
            }
            else if (lockType == LockType::PTHREAD_CONDITION_WAIT)
            {
                data->update_duration(THREAD_LC_PC_MAX_DURATION, duration);
            }
        }

        // Statistics collector objects for individual patterns
        #if defined(_MPI)
            Quantile* ls_quant;
            Quantile* lsw_quant;
            Quantile* lr_quant;
            Quantile* lrw_quant;
            Quantile* wnxn_quant;
            Quantile* wb_quant;
            Quantile* er_quant;
            Quantile* es_quant;
            Quantile* lb_quant;
            Quantile* bc_quant;
            Quantile* nxnc_quant;
        #endif    // _MPI

        #if defined(_OPENMP)
            Quantile* omp_eb_quant;
            Quantile* omp_ib_quant;
            Quantile* thread_lc_ol_quant;
            Quantile* thread_lc_oc_quant;
            Quantile* thread_lc_pm_quant;
            Quantile* thread_lc_pc_quant;
        #endif    // (_OPENMP)

        // define deques for most severe instances
        #if defined(_MPI)
            vector< TopMostSevere > LateSender, LateSenderWO, LateReceiver, EarlyReduce;
            vector< TopMostSevere > BarrierNxN, Barrier, NxnCompl, BarrierCompl, LateBcast, EarlyScan;
        #endif    // _MPI

        static MaxDurations sharedDurations;

        #if defined(_OPENMP)
            vector< TopMostSevere > OmpEBarrier, OmpIBarrier;
            vector< TopMostSevere > ThreadLockContentionOMPLockAPI;
            vector< TopMostSevere > ThreadLockContentionOMPCritical;
            vector< TopMostSevere > ThreadLockContentionPthreadMutex;
            vector< TopMostSevere > ThreadLockContentionPthreadCondition;

            static vector< TopMostSevere > sharedLC_OL;
            static vector< TopMostSevere > sharedLC_OC;
            static vector< TopMostSevere > sharedLC_PM;
            static vector< TopMostSevere > sharedLC_PC;

            static TopMostSevereMaxSum sharedMaxSum;


            // merges thread local TopMostSevere vectors into a shared one using the stat_collector method
            void
            result_merge_thread_topmost(vector< TopMostSevere >& sharedInst,
                                        vector< TopMostSevere >  inst)
            {
                #pragma omp critical
                {
                    for (size_t i = 0; i < inst.size(); i++)
                    {
                        stat_collector(sharedInst,
                                       inst[i].idletime,
                                       inst[i].entertime,
                                       inst[i].exittime,
                                       inst[i].cnode,
                                       inst[i].rank);
                    }
                }
            }
        #endif    // _OPENMP

        // merge results of found patterns
        // from all processes and threads
        // static variables used here as an implicit shared variables
        // to perform reduction for OpenMP threads
        void
        result_merge(Quantile* quant)
        {
            // Determine global number of instances
            // (The 'static' variable 'shared_n' is implicitly shared!)
            double        n, global_n;
            static double shared_n;
            #pragma omp master
            {
                shared_n = 0.0;
            }

            // Determine global coefficients for collecting statistics
            // (The 'static' array 'shared_coeff' is implicitly shared!)
            double        coeff[NUMBER_COEFF], global_coeff[NUMBER_COEFF];
            static double shared_coeff[NUMBER_COEFF];

            #pragma omp master
            {
                for (int i = 0; i < NUMBER_COEFF; i++)
                {
                    shared_coeff[i] = 0.0;
                }
            }

            // Determine global coefficients for control values
            // (The 'static' array 'shared_control_val' is implicitly shared!)
            double        control_val[NUMBER_COEFF], global_control_val[NUMBER_COEFF];
            static double shared_control_val[NUMBER_COEFF];

            #pragma omp master
            {
                for (int i = 0; i < NUMBER_COEFF; i++)
                {
                    shared_control_val[i] = 0.0;
                }
            }

            // Determine global max
            // (The 'static' variable 'shared_max' is implicitly shared!)
            double        max_val, global_max;
            static double shared_max;
            #pragma omp master
            {
                shared_max = -DBL_MAX;
            }

            // Determine global min
            // (The 'static' variable 'shared_min' is implicitly shared!)
            double        min_val, global_min;
            static double shared_min;
            #pragma omp master
            {
                shared_min = DBL_MAX;
            }

            // Determine global sum
            // (The 'static' variable 'shared_sum' is implicitly shared!)
            double        sum, global_sum;
            static double shared_sum;
            #pragma omp master
            {
                shared_sum = 0.0;
            }

            // Determine global squared sum
            // (The 'static' variable 'shared_squared_sum' is implicitly shared!)
            double        squared_sum, global_squared_sum;
            static double shared_squared_sum;
            #pragma omp master
            {
                shared_squared_sum = 0.0;
            }

            n = quant->get_n();

            #pragma omp barrier
            #pragma omp critical
            {
                shared_n += n;
            }
            #pragma omp barrier

            #if defined(_MPI)
                #pragma omp master
                {
                    MPI_Allreduce(&shared_n, &global_n, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
                    shared_n = global_n;
                }
            #endif    // _MPI

            #pragma omp barrier
            global_n = shared_n;
            #pragma omp barrier

            for (int i = 0; i < NUMBER_COEFF; i++)
            {
                coeff[i] = (n / global_n) * quant->get_coeff(i);
            }

            #pragma omp barrier
            #pragma omp critical
            {
                for (int i = 0; i < NUMBER_COEFF; i++)
                {
                    shared_coeff[i] += coeff[i];
                }
            }
            #pragma omp barrier

            #if defined(_MPI)
                #pragma omp master
                {
                    MPI_Allreduce(shared_coeff, global_coeff, NUMBER_COEFF, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
                    for (int i = 0; i < NUMBER_COEFF; i++)
                    {
                        shared_coeff[i] = global_coeff[i];
                    }
                }
            #endif    // _MPI

            #pragma omp barrier
            for (int i = 0; i < NUMBER_COEFF; i++)
            {
                global_coeff[i] = shared_coeff[i];
                control_val[i]  = (n / global_n) * quant->get_control_val(i);
            }

            #pragma omp barrier
            #pragma omp critical
            {
                for (int i = 0; i < NUMBER_COEFF; i++)
                {
                    shared_control_val[i] += control_val[i];
                }
            }
            #pragma omp barrier
            #if defined(_MPI)
                #pragma omp master
                {
                    MPI_Allreduce(shared_control_val, global_control_val, NUMBER_COEFF, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
                    for (int i = 0; i < NUMBER_COEFF; i++)
                    {
                        shared_control_val[i] = global_control_val[i];
                    }
                }
            #endif    // _MPI

            #pragma omp barrier
            for (int i = 0; i < NUMBER_COEFF; i++)
            {
                global_control_val[i] = shared_control_val[i];
            }

            max_val     = quant->get_max_val();
            min_val     = quant->get_min_val();
            sum         = quant->get_sum();
            squared_sum = quant->get_squared_sum();

            #pragma omp barrier

            #pragma omp critical
            {
                if (shared_max < max_val)
                {
                    shared_max = max_val;
                }

                if (shared_min > min_val)
                {
                    shared_min = min_val;
                }

                shared_sum         += sum;
                shared_squared_sum += squared_sum;
            }
            #pragma omp barrier

            #if defined(_MPI)
                #pragma omp master
                {
                    MPI_Allreduce(&shared_max, &global_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
                    MPI_Allreduce(&shared_min, &global_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
                    MPI_Allreduce(&shared_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
                    MPI_Allreduce(&shared_squared_sum, &global_squared_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
                    shared_max         = global_max;
                    shared_min         = global_min;
                    shared_sum         = global_sum;
                    shared_squared_sum = global_squared_sum;
                }
            #endif    // _MPI

            #pragma omp barrier
            global_max         = shared_max;
            global_min         = shared_min;
            global_sum         = shared_sum;
            global_squared_sum = shared_squared_sum;
            #pragma omp barrier

            quant->set_global_values(global_n, global_sum, global_squared_sum, global_min, global_max, global_coeff, global_control_val);
        }

        void
        write_cube_file(Quantile*                quant,
                        vector< TopMostSevere >& instance,
                        ReportData&              data,
                        FILE*                    cube_fp)
        {
            string patternName     = quant->get_metric();
            double number_obs      = quant->get_n();
            double sum_obs         = quant->get_sum();
            double squared_sum_obs = quant->get_squared_sum();
            double min_val         = quant->get_min_val();
            double max_val         = quant->get_max_val();
            double variance        = (squared_sum_obs / number_obs) - (sum_obs * sum_obs) / (number_obs * number_obs);
            double mean            = sum_obs / number_obs;
            double lower_quant     = quant->get_lower_quant();
            double median          = quant->get_median();
            double upper_quant     = quant->get_upper_quant();

            if (number_obs > 0)
            {
                fprintf(cube_fp, "%-26s %9.0f %1.7f %1.7f %1.10f %1.10f %1.10f", patternName.c_str(), number_obs, mean, median, min_val, max_val, sum_obs);
                if (number_obs >= 2)
                {
                    fprintf(cube_fp, " %1.10f", variance);
                }
                if (number_obs >= 5)
                {
                    fprintf(cube_fp, " %1.10f %1.10f", lower_quant, upper_quant);
                }

                unsigned int i = 0;
                if (instance[i].idletime > 0.0)
                {
                    fprintf(cube_fp, "\n");
                }
                while (  (i < instance.size())
                      && (instance[i].idletime > 0.0))
                {
                    fprintf(cube_fp, "- cnode %d enter: %1.10f exit: %1.10f duration: %1.10f rank: %d\n", data.cnodes[instance[i].cnode]->id, instance[i].entertime, instance[i].exittime, instance[i].idletime, instance[i].rank);
                    i++;
                }

                fprintf(cube_fp, "\n");
            }
        }

        #ifdef WRITE_CONTROL_VALUES
            #if defined(_MPI)
                vector< double > md_ls, md_lsw, md_lr, md_lrw, md_wnxn, md_wb, md_er, md_es, md_lb, md_bc, md_nxnc;
            #endif    // _MPI

            #if defined(_OPENMP)
                vector< double > md_omp_eb, md_omp_ib;
                vector< double > md_thread_lc_ol, md_thread_lc_oc, md_thread_lc_pm, md_thread_lc_pc;
            #endif    // _OPENMP

            void
            write_values(Quantile* quant)
            {
                string filename = "stats/evaluation/" + quant->get_metric() + "_values.txt";
                FILE*  quant_fp = fopen(filename.c_str(), "w");

                int    opt_number_coeff = quant->get_number_coeff();
                double number_obs       = quant->get_n();
                double sum_obs          = quant->get_sum();
                double squared_sum_obs  = quant->get_squared_sum();
                double min_val          = quant->get_min_val();
                double max_val          = quant->get_max_val();
                double factor           = quant->get_factor();
                double coeff[NUMBER_COEFF];

                for (int i = 0; i < NUMBER_COEFF; i++)
                {
                    coeff[i] = quant->get_coeff(i);
                }

                double mean     = sum_obs / number_obs;
                double variance = (squared_sum_obs / number_obs) - (sum_obs * sum_obs) / (number_obs * number_obs);

                fprintf(quant_fp, "Number of all coefficients and all observations n : %d and %10.0f\n", NUMBER_COEFF, number_obs);
                fprintf(quant_fp, "Optimal number of coefficients: %d\n", opt_number_coeff);
                fprintf(quant_fp, "sum : %2.10f, squared sum: %2.10f\n", sum_obs, squared_sum_obs);
                fprintf(quant_fp, "mean : %2.10f, variance: %2.10f\n", mean, variance);

                double lower_quant = quant->get_lower_quant();
                double median      = quant->get_median();
                double upper_quant = quant->get_upper_quant();

                fprintf(quant_fp, "upper bound   : %2.15f\n\n", (1 / factor));
                fprintf(quant_fp, "minimum       : %2.15f\n", min_val);
                fprintf(quant_fp, "lower quartile: %2.15f\n", (lower_quant));
                fprintf(quant_fp, "median        : %2.15f\n", (median));
                fprintf(quant_fp, "upper quartile: %2.15f\n", (upper_quant));
                fprintf(quant_fp, "maximum       : %2.15f\n\n", max_val);

                for (int i = 0; i < NUMBER_COEFF; i++)
                {
                    fprintf(quant_fp, "coeff nr %2d : %2.15f\n", (i + 1), coeff[i]);
                }

                fclose(quant_fp);
            }
        #endif

        // This function performs insertion
        // sort of array of structs, where
        // arr - input array to be sorted
        // len - length of the array to be sorted
        void
        insertionSort(TopMostSevere* arr,
                      int            len)
        {
            int           j;
            TopMostSevere tmp;
            for (int i = 1; i < len; i++)
            {
                j = i;
                while (  (j > 0)
                      && (arr[j - 1].idletime < arr[j].idletime))
                {
                    tmp        = arr[j];
                    arr[j]     = arr[j - 1];
                    arr[j - 1] = tmp;
                    j--;
                }
            }
        }

        // This function inserts an element
        // into the sorted vector of structures
        // and shifts the rest elements, where
        // in    - First input array
        // inout - Second input array, also storing the result
        // i     - the id of the element which should be inserted
        // j     - pointer to the position where the structure should be inserted
        // len   - length of the vestor os structures
        void
        insertAndShift(TopMostSevere* invec,
                       TopMostSevere* inoutvec,
                       int            i,
                       int&           j,
                       int&           len)
        {
            // shift
            std::memmove(&inoutvec[j + 1], &inoutvec[j], (len - j - 1) * sizeof(TopMostSevere));

            // insert
            inoutvec[j] = invec[i];
        }

        #if defined(_MPI)
            // performs reduction to compute
            // the top most severe instsnces
            void
            result_top_most(vector< TopMostSevere >& instance,
                            TopMostSevere*           tmpRecv)
            {
                int len_sum;
                int len = instance.size();

                // check if one needs to make reduction
                MPI_Allreduce(&len, &len_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
                if (len_sum > 0)
                {
                    if (instance.size() < TOP_SEVERE_INSTANCES)
                    {
                        if (len > 1)
                        {
                            insertionSort(&instance[0], len);
                        }
                        for (int i = TOP_SEVERE_INSTANCES - instance.size();
                             i > 0;
                             i--)
                        {
                            instance.push_back(TopMostSevere());
                        }
                    }

                    // define global rank
                    int rank;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

                    MPI_Reduce(&instance[0], &tmpRecv[0], 1, TOPSEVEREARRAY,
                               MAX_TOPSEVEREARRAY, 0, MPI_COMM_WORLD);

                    if (rank == 0)
                    {
                        for (int i = 0; i < TOP_SEVERE_INSTANCES; ++i)
                        {
                            instance[i] = tmpRecv[i];
                        }
                    }
                }
            }
        #endif    // _MPI

        // collects and keep sorted
        // most severe instances
        // with different cnodes
        void
        stat_collector(vector< TopMostSevere >& instance,
                       pearl::timestamp_t       idle,
                       pearl::timestamp_t       enter,
                       pearl::timestamp_t       exit,
                       pearl::ident_t           cnode,
                       pearl::ident_t           rank)
        {
            TopMostSevere                     tmp;
            vector< TopMostSevere >::iterator iter_cnode, iter_idle;
            int                               pos_cnode, pos_idle, len;
            tmp = TopMostSevere(idle, enter, exit, cnode, rank);

            // if our vector buffer is not full
            if (instance.size() < TOP_SEVERE_INSTANCES)
            {
                // first severity data is inserted without any conditions
                if (instance.empty())
                {
                    instance.push_back(TopMostSevere(idle, enter, exit, cnode, rank));
                }

                // if it is not the first one, we have to check further conditions
                else
                {
                    // find the position of the severity item with an identical "cnode" value
                    iter_cnode = find_if(instance.begin(), instance.end(), find_cnode_position(cnode));
                    pos_cnode  = (int)(iter_cnode - instance.begin());

                    // if we have severity with such "cnode" value and greater "idletime"
                    // then we update data with new values of time
                    if (  ((unsigned int)pos_cnode != instance.size())
                       && (idle > instance[pos_cnode].idletime))
                    {
                        instance[pos_cnode] = tmp;
                    }

                    // if we've not found any sevirity item with identical "cnode" value
                    // it will be inserted into the vector
                    else if ((unsigned int)pos_cnode == instance.size())
                    {
                        instance.push_back(TopMostSevere(idle, enter, exit, cnode, rank));

                        // if the maximum value of severities was reached, sort the vector due to "idletime"
                        if (instance.size() == TOP_SEVERE_INSTANCES)
                        {
                            insertionSort(&instance[0], instance.size());
                        }
                    }
                }
            }

            // now our vector buffer is full
            else
            {
                len = instance.size();

                // find the position of the severity item with an identical "cnode" value
                iter_cnode = find_if(instance.begin(), instance.end(), find_cnode_position(cnode));
                pos_cnode  = (int)(iter_cnode - instance.begin());

                // find the position of the severity item with not identical "cnode" and greater "idletime"
                iter_idle = find_if(instance.begin(), instance.end(), find_idletime_position(cnode, idle));
                pos_idle  = (int)(iter_idle - instance.begin());

                // update the values if our next item with the same "cnode" has greater "idletime"
                // and sort it afterwards due to "idletime"
                if (  ((unsigned int)pos_cnode != instance.size())
                   && (idle > instance[pos_cnode].idletime))
                {
                    instance[pos_cnode] = tmp;
                    insertionSort(&instance[0], len);
                }

                // if we found the severity with a new "cnode" vlue and greater "idletime"
                // then the vector will be shifted
                else if (  ((unsigned int)pos_idle != instance.size())
                        && ((unsigned int)pos_cnode == instance.size()))
                {
                    insertAndShift(&tmp, &instance[0], 0, pos_idle, len);
                }
            }
        }

        #if defined(_MPI)
            // performs MPI reduction of idletime, enter and exit times
            // of most severe instances for collective operations
            void
            stat_collective(vector< TopMostSevere >& inst,
                            pearl::timestamp_t       idle,
                            pearl::timestamp_t       enter,
                            pearl::timestamp_t       exit,
                            pearl::ident_t           cnode,
                            pearl::ident_t           global_rank,
                            MpiComm*                 comm)
            {
                // define local rank
                int local_rank;
                MPI_Comm_rank(comm->getHandle(), &local_rank);

                TopMostSevereMaxSum tmpSend, tmpRecv;
                tmpSend = TopMostSevereMaxSum(idle, idle, enter, exit, cnode, global_rank);

                MPI_Reduce(&tmpSend, &tmpRecv, 1, TOPSEVERECOLL, MAXSUM_TOPSEVERECOLL, 0, comm->getHandle());

                if (local_rank == 0)
                {
                    #if !defined(MOST_SEVERE_MAX)
                        stat_collector(inst, tmpRecv.idletime, tmpRecv.entertime, tmpRecv.exittime, tmpRecv.cnode, tmpRecv.rank);
                    #else
                        stat_collector(inst, tmpRecv.idletime_max, tmpRecv.entertime, tmpRecv.exittime, tmpRecv.cnode, tmpRecv.rank);
                    #endif
                }
            }
        #endif    // _MPI

        #if defined(_OPENMP)
            // performs OpenMP reduction of idletime, enter and exit times
            // of most severe instances for collective operations
            void
            stat_collective(vector< TopMostSevere >& inst,
                            pearl::timestamp_t       idle,
                            pearl::timestamp_t       enter,
                            pearl::timestamp_t       exit,
                            pearl::ident_t           cnode,
                            pearl::ident_t           rank)
            {
                #pragma omp barrier
                #pragma omp master
                {
                    sharedMaxSum = TopMostSevereMaxSum(-DBL_MAX, 0.0, DBL_MAX, -DBL_MAX, cnode, rank);
                }
                #pragma omp barrier
                #pragma omp critical
                {
                    sharedMaxSum.idletime += idle;

                    if (sharedMaxSum.entertime > enter)
                    {
                        sharedMaxSum.entertime = enter;
                    }

                    if (sharedMaxSum.exittime < exit)
                    {
                        sharedMaxSum.exittime = exit;
                    }

                    if (sharedMaxSum.idletime_max < idle)
                    {
                        sharedMaxSum.idletime_max = idle;
                        sharedMaxSum.rank         = rank;
                    }
                }
                #pragma omp barrier

                #pragma omp master
                {
                    #if !defined(MOST_SEVERE_MAX)
                        stat_collector(inst, sharedMaxSum.idletime, sharedMaxSum.entertime, sharedMaxSum.exittime, sharedMaxSum.cnode, sharedMaxSum.rank);
                    #else
                        stat_collector(inst, sharedMaxSum.idletime_max, sharedMaxSum.entertime, sharedMaxSum.exittime, sharedMaxSum.cnode, sharedMaxSum.rank);
                    #endif
                }
            }
        #endif    // _OPENMP


    public:
        void
        gen_severities(ReportData&              data,
                       int                      rank,
                       const pearl::LocalTrace& trace)
        {
            #if defined(_MPI)
                result_merge(ls_quant);
                result_merge(lsw_quant);
                result_merge(lr_quant);
                result_merge(wnxn_quant);
                result_merge(wb_quant);
                result_merge(er_quant);
                result_merge(es_quant);
                result_merge(lb_quant);
                result_merge(bc_quant);
                result_merge(nxnc_quant);
            #endif    // _MPI
            #if defined(_OPENMP)
                result_merge(omp_eb_quant);
                result_merge(omp_ib_quant);
                result_merge(thread_lc_ol_quant);
                result_merge(thread_lc_oc_quant);
                result_merge(thread_lc_pm_quant);
                result_merge(thread_lc_pc_quant);
            #endif

            #if defined(_OPENMP)
                #pragma omp barrier
                result_merge_thread_topmost(sharedLC_OL, ThreadLockContentionOMPLockAPI);
                result_merge_thread_topmost(sharedLC_OC, ThreadLockContentionOMPCritical);
                result_merge_thread_topmost(sharedLC_PM, ThreadLockContentionPthreadMutex);
                result_merge_thread_topmost(sharedLC_PC, ThreadLockContentionPthreadCondition);

                #pragma omp barrier
            #endif    // _OPENMP

            #ifdef WRITE_CONTROL_VALUES
                #pragma omp master
                {
                    if (rank == 0)
                    {
                        mkdir("stats", 0700);
                        mkdir("stats/evaluation", 0700);
                        #if defined(_MPI)
                            mkdir("stats/ls_stats", 0700);
                            mkdir("stats/lsw_stats", 0700);
                            mkdir("stats/lr_stats", 0700);
                            mkdir("stats/wnxn_stats", 0700);
                            mkdir("stats/wb_stats", 0700);
                            mkdir("stats/er_stats", 0700);
                            mkdir("stats/es_stats", 0700);
                            mkdir("stats/lb_stats", 0700);
                            mkdir("stats/bc_stats", 0700);
                            mkdir("stats/nxnc_stats", 0700);
                        #endif    // _MPI

                        #if defined(_OPENMP)
                            mkdir("stats/omp_eb_stats", 0700);
                            mkdir("stats/omp_ib_stats", 0700);
                            mkdir("stats/thread_lc_ol_stats", 0700);
                            mkdir("stats/thread_lc_oc_stats", 0700);
                            mkdir("stats/thread_lc_pm_stats", 0700);
                            mkdir("stats/thread_lc_pc_stats", 0700);
                        #endif    // _OPENMP
                    }
                    #if defined(_MPI)
                        MPI_Barrier(MPI_COMM_WORLD);
                    #endif    // _MPI
                }

                // write values for OpenMp patterns
                #if defined(_OPENMP)
                    char         name_omp[50];
                    FILE*        stats_omp_fp;
                    unsigned int thread_id;
                    thread_id = omp_get_thread_num();
                    #pragma omp barrier

                    // OpenMP explicit barrier
                    snprintf(name_omp, 50, "stats/omp_eb_stats/omp_eb_stats.%06dx%06u", rank, thread_id);
                    stats_omp_fp = fopen(name_omp, "w");
                    for (size_t i = 0; i < md_omp_eb.size(); i++)
                    {
                        fprintf(stats_omp_fp, "%2.14f\n", md_omp_eb[i]);
                    }
                    fclose(stats_omp_fp);

                    // OpenMP implicit barrier
                    snprintf(name_omp, 50, "stats/omp_ib_stats/omp_ib_stats.%06dx%06u", rank, thread_id);
                    stats_omp_fp = fopen(name_omp, "w");
                    for (size_t i = 0; i < md_omp_ib.size(); i++)
                    {
                        fprintf(stats_omp_fp, "%2.14f\n", md_omp_ib[i]);
                    }
                    fclose(stats_omp_fp);

                    // Thread LockContention OMP Lock API
                    snprintf(name_omp, 50, "stats/thread_lc_ol_stats/thread_lc_ol_stats.%06dx%06u", rank, thread_id);
                    stats_omp_fp = fopen(name_omp, "w");
                    for (size_t i = 0; i < md_thread_lc_ol.size(); i++)
                    {
                        fprintf(stats_omp_fp, "%2.14f\n", md_thread_lc_ol[i]);
                    }
                    fclose(stats_omp_fp);

                    // Thread LockContention OMP Critical
                    snprintf(name_omp, 50, "stats/thread_lc_oc_stats/thread_lc_oc_stats.%06dx%06u", rank, thread_id);
                    stats_omp_fp = fopen(name_omp, "w");
                    for (size_t i = 0; i < md_thread_lc_oc.size(); i++)
                    {
                        fprintf(stats_omp_fp, "%2.14f\n", md_thread_lc_oc[i]);
                    }
                    fclose(stats_omp_fp);

                    // Thread LockContention Pthread Mutex
                    snprintf(name_omp, 50, "stats/thread_lc_pm_stats/thread_lc_pm_stats.%06dx%06u", rank, thread_id);
                    stats_omp_fp = fopen(name_omp, "w");
                    for (size_t i = 0; i < md_thread_lc_pm.size(); i++)
                    {
                        fprintf(stats_omp_fp, "%2.14f\n", md_thread_lc_pm[i]);
                    }
                    fclose(stats_omp_fp);

                    // Thread LockContention Pthread Condition
                    snprintf(name_omp, 50, "stats/thread_lc_pc_stats/thread_lc_pc_stats.%06dx%06u", rank, thread_id);
                    stats_omp_fp = fopen(name_omp, "w");
                    for (size_t i = 0; i < md_thread_lc_pc.size(); i++)
                    {
                        fprintf(stats_omp_fp, "%2.14f\n", md_thread_lc_pc[i]);
                    }
                    fclose(stats_omp_fp);
                #endif    // _OPENMP

                #if defined(_MPI)
                    #pragma omp master
                    {
                        char  name_mpi[40];
                        FILE* stats_mpi_fp;

                        // Late Sender values
                        snprintf(name_mpi, 40, "stats/ls_stats/ls_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_ls.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_ls[i]);
                        }
                        fclose(stats_mpi_fp);

                        // Late Sender, Wrong Order values
                        snprintf(name_mpi, 40, "stats/lsw_stats/lsw_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_lsw.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_lsw[i]);
                        }
                        fclose(stats_mpi_fp);

                        // Late Receiver values
                        snprintf(name_mpi, 40, "stats/lr_stats/lr_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_lr.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_lr[i]);
                        }
                        fclose(stats_mpi_fp);

                        // Wait at NxN values
                        snprintf(name_mpi, 40, "stats/wnxn_stats/wnxn_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_wnxn.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_wnxn[i]);
                        }
                        fclose(stats_mpi_fp);

                        // Wait at Barrier values
                        snprintf(name_mpi, 40, "stats/wb_stats/wb_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_wb.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_wb[i]);
                        }
                        fclose(stats_mpi_fp);

                        // Early Reduce values
                        snprintf(name_mpi, 40, "stats/er_stats/er_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_er.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_er[i]);
                        }
                        fclose(stats_mpi_fp);

                        // Early Scan values
                        snprintf(name_mpi, 40, "stats/es_stats/es_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_es.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_es[i]);
                        }
                        fclose(stats_mpi_fp);

                        // Late Broadcast values
                        snprintf(name_mpi, 40, "stats/lb_stats/lb_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_lb.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_lb[i]);
                        }
                        fclose(stats_mpi_fp);

                        // Barrier Completion values
                        snprintf(name_mpi, 40, "stats/bc_stats/bc_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_bc.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_bc[i]);
                        }
                        fclose(stats_mpi_fp);

                        // NxN Completion values
                        snprintf(name_mpi, 40, "stats/nxnc_stats/nxnc_stats.%06d", rank);
                        stats_mpi_fp = fopen(name_mpi, "w");
                        for (size_t i = 0; i < md_nxnc.size(); i++)
                        {
                            fprintf(stats_mpi_fp, "%2.14f\n", md_nxnc[i]);
                        }
                        fclose(stats_mpi_fp);
                    }
                #endif // _MPI
            #endif     // WRITE_CONTROL_VALUES

            #pragma omp master
            {
                // Merge results for each pattern
                #if defined(_MPI)
                    // define temporary receive buffer
                    TopMostSevere* tmpRecv;
                    tmpRecv = new TopMostSevere[TOP_SEVERE_INSTANCES];

                    result_top_most(LateSender, tmpRecv);
                    result_top_most(LateSenderWO, tmpRecv);
                    result_top_most(LateReceiver, tmpRecv);
                    result_top_most(EarlyReduce, tmpRecv);
                    result_top_most(BarrierNxN, tmpRecv);
                    result_top_most(Barrier, tmpRecv);
                    result_top_most(NxnCompl, tmpRecv);
                    result_top_most(BarrierCompl, tmpRecv);
                    result_top_most(LateBcast, tmpRecv);
                    result_top_most(EarlyScan, tmpRecv);
                    #if defined(_OPENMP)
                        result_top_most(OmpEBarrier, tmpRecv);
                        result_top_most(OmpIBarrier, tmpRecv);
                        result_top_most(sharedLC_OL, tmpRecv);
                        result_top_most(sharedLC_OC, tmpRecv);
                        result_top_most(sharedLC_PM, tmpRecv);
                        result_top_most(sharedLC_PC, tmpRecv);
                    #endif    // _OPENMP
                    // free temporary receive buffer
                    delete[] tmpRecv;
                #endif    // _MPI

                if (rank == 0)
                {
                    // Open statistics file
                    string filename = archiveDirectory + "/trace.stat";
                    FILE*  cube_fp  = fopen(filename.c_str(), "w");
                    if (cube_fp == NULL)
                    {
                        // Only warn (instead of abort) since we still might get a
                        // valid CUBE file
                        UTILS_WARNING("Could not create statistics file!");
                    }
                    else
                    {
                        // Write statistics
                        fprintf(cube_fp, "PatternName               Count      Mean    Median      Minimum      Maximum      Sum     Variance    Quartil25    Quartil75\n");

                        #if defined(_MPI)
                            ls_quant->set_metric("mpi_latesender");
                            ls_quant->calc_quantiles();
                            write_cube_file(ls_quant, LateSender, data, cube_fp);

                            lsw_quant->set_metric("mpi_latesender_wo");
                            lsw_quant->calc_quantiles();
                            write_cube_file(lsw_quant, LateSenderWO, data, cube_fp);

                            lr_quant->set_metric("mpi_latereceiver");
                            lr_quant->calc_quantiles();
                            write_cube_file(lr_quant, LateReceiver, data, cube_fp);

                            wnxn_quant->set_metric("mpi_wait_nxn");
                            wnxn_quant->calc_quantiles();
                            write_cube_file(wnxn_quant, BarrierNxN, data, cube_fp);

                            wb_quant->set_metric("mpi_barrier_wait");
                            wb_quant->calc_quantiles();
                            write_cube_file(wb_quant, Barrier, data, cube_fp);

                            er_quant->set_metric("mpi_earlyreduce");
                            er_quant->calc_quantiles();
                            write_cube_file(er_quant, EarlyReduce, data, cube_fp);

                            es_quant->set_metric("mpi_earlyscan");
                            es_quant->calc_quantiles();
                            write_cube_file(es_quant, EarlyScan, data, cube_fp);

                            lb_quant->set_metric("mpi_latebroadcast");
                            lb_quant->calc_quantiles();
                            write_cube_file(lb_quant, LateBcast, data, cube_fp);

                            bc_quant->set_metric("mpi_barrier_completion");
                            bc_quant->calc_quantiles();
                            write_cube_file(bc_quant, BarrierCompl, data, cube_fp);

                            nxnc_quant->set_metric("mpi_nxn_completion");
                            nxnc_quant->calc_quantiles();
                            write_cube_file(nxnc_quant, NxnCompl, data, cube_fp);
                        #endif    // _MPI

                        #if defined(_OPENMP)
                            omp_eb_quant->set_metric("omp_ebarrier_wait");
                            omp_eb_quant->calc_quantiles();
                            write_cube_file(omp_eb_quant, OmpEBarrier, data, cube_fp);

                            omp_ib_quant->set_metric("omp_ibarrier_wait");
                            omp_ib_quant->calc_quantiles();
                            write_cube_file(omp_ib_quant, OmpIBarrier, data, cube_fp);

                            thread_lc_ol_quant->set_metric("omp_lock_contention_api");
                            thread_lc_ol_quant->calc_quantiles();
                            write_cube_file(thread_lc_ol_quant, sharedLC_OL, data, cube_fp);

                            thread_lc_oc_quant->set_metric("omp_lock_contention_critical");
                            thread_lc_oc_quant->calc_quantiles();
                            write_cube_file(thread_lc_oc_quant, sharedLC_OC, data, cube_fp);

                            thread_lc_pm_quant->set_metric("pthread_lock_contention_mutex_lock");
                            thread_lc_pm_quant->calc_quantiles();
                            write_cube_file(thread_lc_pm_quant, sharedLC_PM, data, cube_fp);

                            thread_lc_pc_quant->set_metric("pthread_lock_contention_conditional");
                            thread_lc_pc_quant->calc_quantiles();
                            write_cube_file(thread_lc_pc_quant, sharedLC_PC, data, cube_fp);
                        #endif    // _OPENMP

                        // Close statistics file
                        fclose(cube_fp);

                        #ifdef WRITE_CONTROL_VALUES
                            // Write coefficients per pattern for debugging
                            #if defined(_MPI)
                                write_values(ls_quant);
                                write_values(lsw_quant);
                                write_values(lr_quant);
                                write_values(wnxn_quant);
                                write_values(wb_quant);
                                write_values(er_quant);
                                write_values(es_quant);
                                write_values(lb_quant);
                                write_values(bc_quant);
                                write_values(nxnc_quant);
                            #endif    // _MPI

                            #if defined(_OPENMP)
                                write_values(omp_eb_quant);
                                write_values(omp_ib_quant);
                                write_values(thread_lc_ol_quant);
                                write_values(thread_lc_oc_quant);
                                write_values(thread_lc_pm_quant);
                                write_values(thread_lc_pc_quant);
                            #endif // _OPENMP
                        #endif     // WRITE_CONTROL_VALUES
                    }
                }
            }
        }
};


// --- Static class data ----------------------------------------------------

#line 60 "Statistics.pattern"

#if defined(_OPENMP)
    vector< TopMostSevere > PatternStatistics::sharedLC_OL;
    vector< TopMostSevere > PatternStatistics::sharedLC_OC;
    vector< TopMostSevere > PatternStatistics::sharedLC_PM;
    vector< TopMostSevere > PatternStatistics::sharedLC_PC;

    TopMostSevereMaxSum PatternStatistics::sharedMaxSum;
#endif    // (_OPENMP)

PatternStatistics::MaxDurations PatternStatistics::sharedDurations;


// --- Registering callbacks ------------------------------------------------

void
PatternStatistics::reg_cb(CallbackManagerMap& cbmanagers)
{
    init();

    CallbackManagerMap::iterator    it;
    CallbackManagerMap::mapped_type cbmanager;

    // Register main callbacks
    it = cbmanagers.find("");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        PREPARE,
        PEARL_create_callback(
            this,
            &PatternStatistics::prepare__cb));
    cbmanager->register_callback(
        LATE_SENDER,
        PEARL_create_callback(
            this,
            &PatternStatistics::late_sender__cb));
    cbmanager->register_callback(
        LATE_SENDER_WO,
        PEARL_create_callback(
            this,
            &PatternStatistics::late_sender_wo__cb));
    cbmanager->register_callback(
        WAIT_NXN,
        PEARL_create_callback(
            this,
            &PatternStatistics::wait_nxn__cb));
    cbmanager->register_callback(
        WAIT_BARRIER,
        PEARL_create_callback(
            this,
            &PatternStatistics::wait_barrier__cb));
    cbmanager->register_callback(
        EARLY_REDUCE,
        PEARL_create_callback(
            this,
            &PatternStatistics::early_reduce__cb));
    cbmanager->register_callback(
        EARLY_SCAN,
        PEARL_create_callback(
            this,
            &PatternStatistics::early_scan__cb));
    cbmanager->register_callback(
        LATE_BCAST,
        PEARL_create_callback(
            this,
            &PatternStatistics::late_bcast__cb));
    cbmanager->register_callback(
        NXN_COMPL,
        PEARL_create_callback(
            this,
            &PatternStatistics::nxn_compl__cb));
    cbmanager->register_callback(
        BARRIER_COMPL,
        PEARL_create_callback(
            this,
            &PatternStatistics::barrier_compl__cb));
    cbmanager->register_callback(
        OMP_EBARRIER_WAIT,
        PEARL_create_callback(
            this,
            &PatternStatistics::omp_ebarrier_wait__cb));
    cbmanager->register_callback(
        OMP_IBARRIER_WAIT,
        PEARL_create_callback(
            this,
            &PatternStatistics::omp_ibarrier_wait__cb));
    cbmanager->register_callback(
        THREAD_LOCK_CONTENTION_OMPLOCK_WAIT,
        PEARL_create_callback(
            this,
            &PatternStatistics::thread_lock_contention_omplock_wait__cb));
    cbmanager->register_callback(
        THREAD_LOCK_CONTENTION_OMPCRITICAL_WAIT,
        PEARL_create_callback(
            this,
            &PatternStatistics::thread_lock_contention_ompcritical_wait__cb));
    cbmanager->register_callback(
        THREAD_LOCK_CONTENTION_PTHREADMUTEX_WAIT,
        PEARL_create_callback(
            this,
            &PatternStatistics::thread_lock_contention_pthreadmutex_wait__cb));
    cbmanager->register_callback(
        THREAD_LOCK_CONTENTION_PTHREADCONDITION_WAIT,
        PEARL_create_callback(
            this,
            &PatternStatistics::thread_lock_contention_pthreadcondition_wait__cb));

    // Register bws callbacks
    it = cbmanagers.find("bws");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        LATE_RECEIVER,
        PEARL_create_callback(
            this,
            &PatternStatistics::late_receiver_bws_cb));
}


// --- Callback methods -----------------------------------------------------

void
PatternStatistics::barrier_compl__cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1691 "Statistics.pattern"

    #if defined(_MPI)
        if (data->mCompletion > 0)
        {
            bc_quant->add_value(data->mCompletion);
            #ifdef WRITE_CONTROL_VALUES
                md_bc.push_back(data->mCompletion);
            #endif
        }

        MpiComm* const comm = event->getComm();

        const Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);

        stat_collective(BarrierCompl,
                        data->mCompletion,
                        begin->getTimestamp(),
                        event->getTimestamp(),
                        event.get_cnode()->getId(),
                        event.get_location().getRank(),
                        comm);
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


void
PatternStatistics::early_reduce__cb(const CallbackManager& cbmanager,
                                    int                    user_event,
                                    const Event&           event,
                                    CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1576 "Statistics.pattern"

    #if defined(_MPI)
        if (data->mIdle > 0)
        {
            er_quant->add_value(data->mIdle);
            #ifdef WRITE_CONTROL_VALUES
                md_er.push_back(data->mIdle);
            #endif
        }

        const Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);
        stat_collector(EarlyReduce,
                       data->mIdle,
                       begin->getTimestamp(),
                       event->getTimestamp(),
                       event.get_cnode()->getId(),
                       event.get_location().getRank());
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


void
PatternStatistics::early_scan__cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1600 "Statistics.pattern"

    #if defined(_MPI)
        if (data->mIdle > 0)
        {
            es_quant->add_value(data->mIdle);
            #ifdef WRITE_CONTROL_VALUES
                md_es.push_back(data->mIdle);
            #endif
        }

        MpiComm* comm = event->getComm();

        Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);

        stat_collective(EarlyScan,
                        data->mIdle,
                        begin->getTimestamp(),
                        event->getTimestamp(),
                        event.get_cnode()->getId(),
                        event.get_location().getRank(),
                        comm);
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


void
PatternStatistics::late_bcast__cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1628 "Statistics.pattern"

    #if defined(_MPI)
        if (data->mIdle > 0)
        {
            lb_quant->add_value(data->mIdle);
        }
        else
        {
            data->mIdle = 0.0;
        }

        MpiComm* comm = event->getComm();

        Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);

        stat_collective(LateBcast, data->mIdle,
                        begin->getTimestamp(),
                        event->getTimestamp(),
                        event.get_cnode()->getId(),
                        event.get_location().getRank(),
                        comm);

        #ifdef WRITE_CONTROL_VALUES
            if (data->mIdle > 0)
            {
                md_lb.push_back(data->mIdle);
            }
        #endif
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


void
PatternStatistics::late_receiver_bws_cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1908 "Statistics.pattern"

    #if defined(_MPI)
        lr_quant->add_value(data->mIdle);

        Event       enter = data->mLocal->get_event(ROLE_ENTER_SEND_LR);
        RemoteEvent recv  = data->mRemote->get_event(ROLE_RECV_LR);

        stat_collector(LateReceiver,
                       data->mIdle,
                       enter->getTimestamp(),
                       recv->getTimestamp(),
                       enter.get_cnode()->getId(),
                       enter.get_location().getRank());

        #ifdef WRITE_CONTROL_VALUES
            md_lr.push_back(data->mIdle);
        #endif
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


void
PatternStatistics::late_sender__cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1472 "Statistics.pattern"

    #if defined(_MPI)
        ls_quant->add_value(data->mIdle);

        Event enter = data->mLocal->get_event(ROLE_RECV_LS);

        stat_collector(LateSender,
                       data->mIdle,
                       enter.enterptr()->getTimestamp(),
                       event->getTimestamp(),
                       enter.get_cnode()->getId(),
                       event.get_location().getRank());

        #ifdef WRITE_CONTROL_VALUES
            md_ls.push_back(data->mIdle);
        #endif
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


void
PatternStatistics::late_sender_wo__cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1495 "Statistics.pattern"

    #if defined(_MPI)
        lsw_quant->add_value(data->mIdle);

        RemoteEvent enter = data->mRemote->get_event(ROLE_SEND);
        Event       exit  = data->mLocal->get_event(ROLE_RECV_LSWO);

        stat_collector(LateSenderWO,
                       data->mIdle,
                       enter->getTimestamp(),
                       event->getTimestamp(),
                       exit.get_cnode()->getId(),
                       event.get_location().getRank());

        #ifdef WRITE_CONTROL_VALUES
            md_lsw.push_back(data->mIdle);
        #endif
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


void
PatternStatistics::nxn_compl__cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1663 "Statistics.pattern"

    #if defined(_MPI)
        if (data->mCompletion > 0)
        {
            nxnc_quant->add_value(data->mCompletion);
            #ifdef WRITE_CONTROL_VALUES
                md_nxnc.push_back(data->mCompletion);
            #endif
        }

        MpiComm* const comm = event->getComm();

        const Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);

        stat_collective(NxnCompl,
                        data->mCompletion,
                        begin->getTimestamp(),
                        event->getTimestamp(),
                        event.get_cnode()->getId(),
                        event.get_location().getRank(),
                        comm);
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


void
PatternStatistics::omp_ebarrier_wait__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1719 "Statistics.pattern"

    #if defined(_OPENMP)
        if (data->mIdle > 0)
        {
            omp_eb_quant->add_value(data->mIdle);
        }
        else
        {
            data->mIdle = 0.0;
        }

        #if defined(_MPI)
            stat_collective(OmpEBarrier, data->mIdle,
                            data->mCallstack->top()->getTimestamp(),
                            event->getTimestamp(),
                            event.get_cnode()->getId(),
                            event.get_location().getRank());
        #else    // set rank 0 for pure OpenMP case
            stat_collective(OmpEBarrier, data->mIdle,
                            data->mCallstack->top()->getTimestamp(),
                            event->getTimestamp(),
                            event.get_cnode()->getId(),
                            0);
        #endif    // !_MPI

        #ifdef WRITE_CONTROL_VALUES
            if (data->mIdle > 0)
            {
                md_omp_eb.push_back(data->mIdle);
            }
        #endif
    #else    // !_OPENMP
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _OPENMP
}


void
PatternStatistics::omp_ibarrier_wait__cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1757 "Statistics.pattern"

    #if defined(_OPENMP)
        if (data->mIdle > 0)
        {
            omp_ib_quant->add_value(data->mIdle);
        }
        else
        {
            data->mIdle = 0.0;
        }

        #if defined(_MPI)
            stat_collective(OmpIBarrier, data->mIdle,
                            data->mCallstack->top()->getTimestamp(),
                            event->getTimestamp(),
                            event.get_cnode()->getId(),
                            event.get_location().getRank());
        #else    // set rank 0 for pure OpenMP case
            stat_collective(OmpIBarrier, data->mIdle,
                            data->mCallstack->top()->getTimestamp(),
                            event->getTimestamp(),
                            event.get_cnode()->getId(),
                            0);
        #endif    // !_MPI

        #ifdef WRITE_CONTROL_VALUES
            if (data->mIdle > 0)
            {
                md_omp_ib.push_back(data->mIdle);
            }
        #endif
    #else    // !_OPENMP
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _OPENMP
}


void
PatternStatistics::prepare__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1372 "Statistics.pattern"

    // Determine local upper bounds for pattern durations
    CallbackManager cb;
    MaxDurations    durations;

    #if defined(_MPI)
        cb.register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &PatternStatistics::mpicollective_cb));
        cb.register_callback(GROUP_SEND, PEARL_create_callback(this, &PatternStatistics::send_cb));
        cb.register_callback(GROUP_RECV, PEARL_create_callback(this, &PatternStatistics::recv_cb));
    #endif    // _MPI

    #if defined(_OPENMP)
        cb.register_callback(GROUP_LEAVE, PEARL_create_callback(this, &PatternStatistics::leave_cb));
        cb.register_callback(THREAD_ACQUIRE_LOCK, PEARL_create_callback(this, &PatternStatistics::acquire_cb));
    #endif    // _OPENMP

    PEARL_forward_replay(*(data->mTrace), cb, &durations);

    // Determine global upper bounds for pattern durations
    #pragma omp master
    {
        sharedDurations.m_max_duration[OMP_EB_MAX_DURATION]       = -DBL_MAX;
        sharedDurations.m_max_duration[OMP_IB_MAX_DURATION]       = -DBL_MAX;
        sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION] = -DBL_MAX;
        sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION] = -DBL_MAX;
        sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION] = -DBL_MAX;
        sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION] = -DBL_MAX;
    }

    #pragma omp barrier
    #pragma omp critical
    {
        if (sharedDurations.m_max_duration[OMP_EB_MAX_DURATION] < durations.m_max_duration[OMP_EB_MAX_DURATION])
        {
            sharedDurations.m_max_duration[OMP_EB_MAX_DURATION] = durations.m_max_duration[OMP_EB_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[OMP_IB_MAX_DURATION] < durations.m_max_duration[OMP_IB_MAX_DURATION])
        {
            sharedDurations.m_max_duration[OMP_IB_MAX_DURATION] = durations.m_max_duration[OMP_IB_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION] < durations.m_max_duration[THREAD_LC_OL_MAX_DURATION])
        {
            sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION] = durations.m_max_duration[THREAD_LC_OL_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION] < durations.m_max_duration[THREAD_LC_OC_MAX_DURATION])
        {
            sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION] = durations.m_max_duration[THREAD_LC_OC_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION] < durations.m_max_duration[THREAD_LC_PM_MAX_DURATION])
        {
            sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION] = durations.m_max_duration[THREAD_LC_PM_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION] < durations.m_max_duration[THREAD_LC_PC_MAX_DURATION])
        {
            sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION] = durations.m_max_duration[THREAD_LC_PC_MAX_DURATION];
        }
    }
    #pragma omp barrier

    #if defined(_MPI)
        #pragma omp master
        {
            durations.m_max_duration[OMP_EB_MAX_DURATION]       = sharedDurations.m_max_duration[OMP_EB_MAX_DURATION];
            durations.m_max_duration[OMP_IB_MAX_DURATION]       = sharedDurations.m_max_duration[OMP_IB_MAX_DURATION];
            durations.m_max_duration[THREAD_LC_OL_MAX_DURATION] = sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION];
            durations.m_max_duration[THREAD_LC_OC_MAX_DURATION] = sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION];
            durations.m_max_duration[THREAD_LC_PM_MAX_DURATION] = sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION];
            durations.m_max_duration[THREAD_LC_PC_MAX_DURATION] = sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION];
            MPI_Allreduce(durations.m_max_duration,
                          sharedDurations.m_max_duration,
                          MAX_DURATION_ENTRIES, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        }
    #endif    // _MPI

    #pragma omp barrier

    #if defined(_MPI)
        ls_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[LS_MAX_DURATION]);
        lsw_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[LS_MAX_DURATION]);
        lr_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[LR_MAX_DURATION]);
        wnxn_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[WNXN_MAX_DURATION]);
        wb_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[WB_MAX_DURATION]);
        er_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[ER_MAX_DURATION]);
        es_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[ES_MAX_DURATION]);
        lb_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[LB_MAX_DURATION]);
        bc_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[BC_MAX_DURATION]);
        nxnc_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[NXNC_MAX_DURATION]);
    #endif    // _MPI

    #if defined(_OPENMP)
        omp_eb_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[OMP_EB_MAX_DURATION]);
        omp_ib_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[OMP_IB_MAX_DURATION]);
        thread_lc_ol_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION]);
        thread_lc_oc_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION]);
        thread_lc_pm_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION]);
        thread_lc_pc_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION]);
    #endif    // _OPENMP
}


void
PatternStatistics::thread_lock_contention_ompcritical_wait__cb(const CallbackManager& cbmanager,
                                                               int                    user_event,
                                                               const Event&           event,
                                                               CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1823 "Statistics.pattern"

    #if defined(_OPENMP)
        thread_lc_oc_quant->add_value(data->mIdle);

        #if defined(_MPI)
            stat_collector(ThreadLockContentionOMPCritical, data->mIdle,
                           event.enterptr()->getTimestamp(),
                           event->getTimestamp(),
                           event.get_cnode()->getId(),
                           event.get_location().getRank());
        #else    // set rank 0 for pure OpenMP case
            stat_collector(ThreadLockContentionOMPCritical, data->mIdle,
                           event.enterptr()->getTimestamp(),
                           event->getTimestamp(),
                           event.get_cnode()->getId(),
                           0);
        #endif    // !_MPI

        #ifdef WRITE_CONTROL_VALUES
            md_thread_lc_oc.push_back(data->mIdle);
        #endif
    #else    // !_OPENMP
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _OPENMP
}


void
PatternStatistics::thread_lock_contention_omplock_wait__cb(const CallbackManager& cbmanager,
                                                           int                    user_event,
                                                           const Event&           event,
                                                           CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1795 "Statistics.pattern"

    #if defined(_OPENMP)
        thread_lc_ol_quant->add_value(data->mIdle);

        #if defined(_MPI)
            stat_collector(ThreadLockContentionOMPLockAPI, data->mIdle,
                           event.enterptr()->getTimestamp(),
                           event->getTimestamp(),
                           event.get_cnode()->getId(),
                           event.get_location().getRank());
        #else    // set rank 0 for pure OpenMP case
            stat_collector(ThreadLockContentionOMPLockAPI, data->mIdle,
                           event.enterptr()->getTimestamp(),
                           event->getTimestamp(),
                           event.get_cnode()->getId(),
                           0);
        #endif    // !_MPI

        #ifdef WRITE_CONTROL_VALUES
            md_thread_lc_ol.push_back(data->mIdle);
        #endif
    #else    // !_OPENMP
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _OPENMP
}


void
PatternStatistics::thread_lock_contention_pthreadcondition_wait__cb(const CallbackManager& cbmanager,
                                                                    int                    user_event,
                                                                    const Event&           event,
                                                                    CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1879 "Statistics.pattern"

    #if defined(_OPENMP)
        thread_lc_pc_quant->add_value(data->mIdle);

        #if defined(_MPI)
            stat_collector(ThreadLockContentionPthreadCondition, data->mIdle,
                           event.enterptr()->getTimestamp(),
                           event->getTimestamp(),
                           event.get_cnode()->getId(),
                           event.get_location().getRank());
        #else    // set rank 0 for pure OpenMP case
            stat_collector(ThreadLockContentionPthreadCondition, data->mIdle,
                           event.enterptr()->getTimestamp(),
                           event->getTimestamp(),
                           event.get_cnode()->getId(),
                           0);
        #endif    // !_MPI

        #ifdef WRITE_CONTROL_VALUES
            md_thread_lc_pc.push_back(data->mIdle);
        #endif
    #else    // !_OPENMP
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _OPENMP
}


void
PatternStatistics::thread_lock_contention_pthreadmutex_wait__cb(const CallbackManager& cbmanager,
                                                                int                    user_event,
                                                                const Event&           event,
                                                                CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1851 "Statistics.pattern"

    #if defined(_OPENMP)
        thread_lc_pm_quant->add_value(data->mIdle);

        #if defined(_MPI)
            stat_collector(ThreadLockContentionPthreadMutex, data->mIdle,
                           event.enterptr()->getTimestamp(),
                           event->getTimestamp(),
                           event.get_cnode()->getId(),
                           event.get_location().getRank());
        #else    // set rank 0 for pure OpenMP case
            stat_collector(ThreadLockContentionPthreadMutex, data->mIdle,
                           event.enterptr()->getTimestamp(),
                           event->getTimestamp(),
                           event.get_cnode()->getId(),
                           0);
        #endif    // !_MPI

        #ifdef WRITE_CONTROL_VALUES
            md_thread_lc_pm.push_back(data->mIdle);
        #endif
    #else    // !_OPENMP
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _OPENMP
}


void
PatternStatistics::wait_barrier__cb(const CallbackManager& cbmanager,
                                    int                    user_event,
                                    const Event&           event,
                                    CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1548 "Statistics.pattern"

    #if defined(_MPI)
        if (data->mIdle > 0)
        {
            wb_quant->add_value(data->mIdle);
            #ifdef WRITE_CONTROL_VALUES
                md_wb.push_back(data->mIdle);
            #endif
        }

        MpiComm* const comm = event->getComm();

        const Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);

        stat_collective(Barrier,
                        data->mIdle,
                        begin->getTimestamp(),
                        event->getTimestamp(),
                        event.get_cnode()->getId(),
                        event.get_location().getRank(),
                        comm);
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


void
PatternStatistics::wait_nxn__cb(const CallbackManager& cbmanager,
                                int                    user_event,
                                const Event&           event,
                                CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1519 "Statistics.pattern"

    #if defined(_MPI)
        if (data->mIdle > 0)
        {
            wnxn_quant->add_value(data->mIdle);
            #ifdef WRITE_CONTROL_VALUES
                md_wnxn.push_back(data->mIdle);
            #endif
        }

        MpiComm* const comm = event->getComm();

        // FIXME: This should use ROLE_BEGIN_COLL instead to match WAIT_BARRIER!
        const Event begin = data->mLocal->get_event(ROLE_ENTER_COLL);

        stat_collective(BarrierNxN,
                        data->mIdle,
                        begin->getTimestamp(),
                        event->getTimestamp(),
                        event.get_cnode()->getId(),
                        event.get_location().getRank(),
                        comm);
    #else    // !_MPI
        // Ugly hack to avoid 'unused variable' warning
        (void)data;
    #endif    // _MPI
}


/*-------------------------------------------------------------------------*/
/**
 *  @class PatternCriticalPath
 *  @brief Critical Path Profile metric.
 *
 *  Implements the "Critical Path Profile" metric.
 */
/*-------------------------------------------------------------------------*/

class PatternCriticalPath
    : public Pattern
{
    public:
        /// @name Registering callbacks
        /// @{

        virtual void
        reg_cb(CallbackManagerMap& cbmanagers);

        /// @}
        /// @name Retrieving pattern information
        /// @{

        virtual long
        get_id() const
        {
            return PAT_CRITICAL_PATH;
        }

        virtual long
        get_parent() const
        {
            return PAT_NONE;
        }

        virtual string
        get_name() const
        {
            return "Critical path profile";
        }

        virtual string
        get_unique_name() const
        {
            return "critical_path";
        }

        virtual string
        get_descr() const
        {
            return "Profile of the application's critical path";
        }

        virtual string
        get_unit() const
        {
            return "sec";
        }

        virtual CubeMetricType
        get_mode() const
        {
            return CUBE_METRIC_EXCLUSIVE;
        }

        /// @}
        /// @name Callback methods
        /// @{

        void
        coll_12n_bwc_cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        coll_n21_bwc_cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        coll_n2n_bwc_cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        finalize_bwc_cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        finished_bwc_cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        init_bwc_cb(const CallbackManager& cbmanager,
                    int                    user_event,
                    const Event&           event,
                    CallbackData*          cdata);

        void
        omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                           int                    user_event,
                           const Event&           event,
                           CallbackData*          cdata);

        void
        omp_mgmt_fork_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

        void
        post_inv_recvreq_bwc_cb(const CallbackManager& cbmanager,
                                int                    user_event,
                                const Event&           event,
                                CallbackData*          cdata);

        void
        post_send_bwc_cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

        void
        prepare_bwc_cb(const CallbackManager& cbmanager,
                       int                    user_event,
                       const Event&           event,
                       CallbackData*          cdata);

        void
        prepare_bws_cb(const CallbackManager& cbmanager,
                       int                    user_event,
                       const Event&           event,
                       CallbackData*          cdata);

        void
        pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

        void
        pre_recv_bwc_cb(const CallbackManager& cbmanager,
                        int                    user_event,
                        const Event&           event,
                        CallbackData*          cdata);

        void
        sync_coll_bwc_cb(const CallbackManager& cbmanager,
                         int                    user_event,
                         const Event&           event,
                         CallbackData*          cdata);

        /// @}


    private:
        #line 91 "CriticalPath.pattern"

        // *INDENT-OFF*    Uncrustify issue #2789
        bool  m_on_critical_path;
        Event m_end_event;
        // *INDENT-ON*

        static std::vector< double > m_tmpvec;

        #ifdef CPADEBUG
            std::ofstream m_dbg;
        #endif


        /// @todo Once all Syncpoints are adjusted, `spi.waitTime` should be &le;
        ///       the time spent in the comm/sync operation (i.e., `min` may not
        ///       be necessary unless there are rounding errors)
        void
        update_path_severities(const Event         from,
                               const CbData* const data)
        {
            timemap_t tmap = data->mTmapCacheHandler->getRuntimes(from, m_end_event);

            for (timemap_t::const_iterator it = tmap.begin();
                 it != tmap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }

            const SynchpointInfo spi = data->mSynchpointHandler->getSynchpointInfo(from);

            Callpath* const callpath = from.get_cnode();
            const double    waitT    = min(spi.waitTime, tmap[callpath->getId()]);

            m_severity[callpath] -= waitT;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << get_name() << " @ " << *callpath
                << ": -" << waitT
                << " (wait state) [total: " << m_severity[callpath] << ']';
        }

        void
        update_first_path_severities(const CbData* const data)
        {
            timemap_t tmap = data->mTmapCacheHandler->getRuntimes(Event(), m_end_event);

            for (timemap_t::const_iterator it = tmap.begin();
                 it != tmap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        /// @todo Once all Syncpoints are adjusted, `wtsm` entries should be &le;
        ///       the time spent in the comm/sync operation (i.e., `min` may not
        ///       be necessary unless there are rounding errors)
        void
        collate_global_critical_path(CbData* const data)
        {
            const uint32_t numCallpaths = data->mDefs->numCallpaths();

            // Collate global critical-path profile
            data->mGlobalCriticalPath.resize(numCallpaths, 0.0);
            data->mLocalCriticalPath.resize(numCallpaths, 0.0);

            for (map< Callpath*, double >::const_iterator it = m_severity.begin();
                 it != m_severity.end();
                 ++it)
            {
                data->mLocalCriticalPath[(it->first)->getId()] = it->second;
            }

            #pragma omp single
            {
                m_tmpvec.assign(numCallpaths, 0.0);
            }
            #pragma omp critical
            {
                transform(m_tmpvec.begin(), m_tmpvec.end(),
                          data->mLocalCriticalPath.begin(),
                          m_tmpvec.begin(),
                          plus< double >());
            }
            #pragma omp barrier

            #ifdef _MPI
                #pragma omp master
                {
                    MPI_Allreduce(MPI_IN_PLACE, &m_tmpvec[0], numCallpaths,
                                  MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
                }
                #pragma omp barrier
            #endif

            data->mGlobalCriticalPath.assign(m_tmpvec.begin(), m_tmpvec.end());

            // Collate local non-waiting time profile
            data->mLocalTimeProfile.resize(numCallpaths, 0.0);

            timemap_t  ltmap = data->mTmapCacheHandler->getRuntimes(Event(), Event());
            TimeMapSum wtsm  = CalculateWaitMap(data->mSynchpointHandler->getSynchpoints());

            for (timemap_t::const_iterator it = ltmap.begin();
                 it != ltmap.end();
                 ++it)
            {
                data->mLocalTimeProfile[it->first] += it->second;

                timemap_t::const_iterator wit = wtsm.mMap.find(it->first);
                if (wit != wtsm.mMap.end())
                {
                    data->mLocalTimeProfile[it->first] -= min(it->second, wit->second);
                }
            }
        }
};


// --- Static class data ----------------------------------------------------

#line 88 "CriticalPath.pattern"

std::vector< double > PatternCriticalPath::m_tmpvec;


// --- Registering callbacks ------------------------------------------------

void
PatternCriticalPath::reg_cb(CallbackManagerMap& cbmanagers)
{
    init();

    CallbackManagerMap::iterator    it;
    CallbackManagerMap::mapped_type cbmanager;

    // Register bwc callbacks
    it = cbmanagers.find("bwc");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        PREPARE,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::prepare_bwc_cb));
    cbmanager->register_callback(
        FINISHED,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::finished_bwc_cb));
    cbmanager->register_callback(
        OMP_MGMT_FORK,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::omp_mgmt_fork_bwc_cb));
    cbmanager->register_callback(
        OMP_BARRIER,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::omp_barrier_bwc_cb));
    cbmanager->register_callback(
        FINALIZE,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::finalize_bwc_cb));
    cbmanager->register_callback(
        INIT,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::init_bwc_cb));
    cbmanager->register_callback(
        COLL_12N,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::coll_12n_bwc_cb));
    cbmanager->register_callback(
        COLL_N2N,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::coll_n2n_bwc_cb));
    cbmanager->register_callback(
        SYNC_COLL,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::sync_coll_bwc_cb));
    cbmanager->register_callback(
        COLL_N21,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::coll_n21_bwc_cb));
    cbmanager->register_callback(
        PRE_RECV,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::pre_recv_bwc_cb));
    cbmanager->register_callback(
        POST_SEND,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::post_send_bwc_cb));
    cbmanager->register_callback(
        POST_INV_RECVREQ,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::post_inv_recvreq_bwc_cb));
    cbmanager->register_callback(
        PRE_INV_SENDCMP,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::pre_inv_sendcmp_bwc_cb));

    // Register bws callbacks
    it = cbmanagers.find("bws");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        PREPARE,
        PEARL_create_callback(
            this,
            &PatternCriticalPath::prepare_bws_cb));
}


// --- Callback methods -----------------------------------------------------

void
PatternCriticalPath::coll_12n_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 384 "CriticalPath.pattern"

    #ifdef _MPI
        const CollectiveInfo& ci = data->mCollinfo;

        int inFlag = 0;
        if (  m_on_critical_path
           && data->mSynchpointHandler->isWaitstate(event))
        {
            update_path_severities(event, data);

            inFlag             = 1;
            m_on_critical_path = false;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Moves to rank "
                << event->getComm()->getCommSet().getGlobalRank(ci.root.mRank);
        }

        int outFlag = 0;
        MPI_Reduce(&inFlag, &outFlag, 1, MPI_INT, MPI_MAX, ci.root.mRank,
                   event->getComm()->getHandle());

        if (outFlag)
        {
            m_on_critical_path = true;
            m_end_event        = event;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Continues on " << event.get_location();
        }
    #endif
}


void
PatternCriticalPath::coll_n21_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 483 "CriticalPath.pattern"

    #ifdef _MPI
        const CollectiveInfo& ci = data->mCollinfo;

        int inFlag = 0;
        if (  m_on_critical_path
           && data->mSynchpointHandler->isWaitstate(event))
        {
            update_path_severities(event, data);

            inFlag             = 1;
            m_on_critical_path = false;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Moves to rank "
                << event->getComm()->getCommSet().getGlobalRank(ci.latest.mRank);
        }

        int outFlag = 0;
        MPI_Reduce(&inFlag, &outFlag, 1, MPI_INT, MPI_MAX, ci.latest.mRank,
                   event->getComm()->getHandle());

        if (outFlag)
        {
            m_on_critical_path = true;
            m_end_event        = event;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Continues on " << event.get_location();
        }
    #endif
}


void
PatternCriticalPath::coll_n2n_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 417 "CriticalPath.pattern"

    #ifdef _MPI
        const CollectiveInfo& ci = data->mCollinfo;

        int inFlag = 0;
        if (  m_on_critical_path
           && data->mSynchpointHandler->isWaitstate(event))
        {
            update_path_severities(event, data);

            inFlag             = 1;
            m_on_critical_path = false;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Moves to rank "
                << event->getComm()->getCommSet().getGlobalRank(ci.latest.mRank);
        }

        int outFlag = 0;
        MPI_Reduce(&inFlag, &outFlag, 1, MPI_INT, MPI_MAX, ci.latest.mRank,
                   event->getComm()->getHandle());

        if (outFlag)
        {
            m_on_critical_path = true;
            m_end_event        = event;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Continues on " << event.get_location();
        }
    #endif
}


void
PatternCriticalPath::finalize_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 360 "CriticalPath.pattern"

    const CollectiveInfo& ci = data->mCollinfo;

    if (ci.my.mRank == ci.latest.mRank)
    {
        m_on_critical_path = true;
        m_end_event        = event;

        UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
            << "Critical Path: Starts on " << event.get_location();
    }
}


void
PatternCriticalPath::finished_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 258 "CriticalPath.pattern"

    #ifndef _MPI
        #pragma omp master
        {
            update_first_path_severities(data);

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Finishes on " << event.get_location();
        }
    #endif

    collate_global_critical_path(data);

    cbmanager.notify(CRITICALPATH_FINISHED, event, data);
}


void
PatternCriticalPath::init_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 374 "CriticalPath.pattern"

    if (m_on_critical_path)
    {
        update_path_severities(event, data);

        UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
            << "Critical Path: Finishes on " << event.get_location();
    }
}


void
PatternCriticalPath::omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 318 "CriticalPath.pattern"

    #ifdef _OPENMP
        static bool flag;

        #pragma omp single
        {
            flag = false;
        }
        #pragma omp critical
        {
            if (m_on_critical_path)
            {
                // --- set severities
                update_path_severities(event, data);

                m_on_critical_path = false;
                flag               = true;
                #ifdef CPADEBUG
                    m_dbg << "Was on critical path in omp barrier" << std::endl;
                #endif
            }
        }

        #pragma omp barrier
        { }

        if (  flag
           && !data->mSynchpointHandler->isWaitstate(event))
        {
            // FIXME: bit of a problem here ... might have two critical paths
            m_on_critical_path = true;
            m_end_event        = event;
            #ifdef CPADEBUG
                m_dbg << "Now on critical path in omp barrier" << std::endl;
            #endif
        }
        #pragma omp barrier
        { }
    #endif
}


void
PatternCriticalPath::omp_mgmt_fork_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 274 "CriticalPath.pattern"

    #ifdef _OPENMP
        static bool flag;

        #pragma omp single
        {
            flag = false;
        }
        #pragma omp critical
        {
            if (m_on_critical_path)
            {
                // --- set severities
                update_path_severities(event, data);

                m_on_critical_path = false;
                flag               = true;

                #ifdef CPADEBUG
                    m_dbg << "Was on critical path in fork" << std::endl;
                #endif
            }
        }

        #pragma omp barrier
        { }

        #pragma omp master
        {
            if (flag)
            {
                m_on_critical_path = true;
                m_end_event        = event;

                #ifdef CPADEBUG
                    m_dbg << "Now on critical path in fork" << std::endl;
                #endif
            }
        }
        #pragma omp barrier
        { }
    #endif
}


void
PatternCriticalPath::post_inv_recvreq_bwc_cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 563 "CriticalPath.pattern"

    #ifdef _MPI
        // --- get critical path info flag

        uint32_t flag = data->mInvRemote->get_buffer(BUFFER_CRITICALPATH)->get_uint32();

        if (flag)
        {
            m_on_critical_path = true;
            m_end_event        = event;
            #ifdef CPADEBUG
                if (!data->mSynchpointHandler->isSynchpoint(event))
                {
                    m_dbg << "RECVREQ on critical path is not a registered synchpoint!" << endl;
                }
            #endif
        }
    #endif
}


void
PatternCriticalPath::post_send_bwc_cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 538 "CriticalPath.pattern"

    #ifdef _MPI
        if (!data->mSynchpointHandler->isSynchpoint(event))
        {
            return;
        }

        // --- get critical path info flag

        uint32_t flag = data->mRemote->get_buffer(BUFFER_CRITICALPATH)->get_uint32();

        if (flag)
        {
            m_on_critical_path = true;
            m_end_event        = event;
            #ifdef CPADEBUG
                if (!data->mSynchpointHandler->isSynchpoint(event))
                {
                    m_dbg << "SEND on critical path is not a registered synchpoint!" << endl;
                }
            #endif
        }
    #endif
}


void
PatternCriticalPath::prepare_bwc_cb(const CallbackManager& cbmanager,
                                    int                    user_event,
                                    const Event&           event,
                                    CallbackData*          cdata)
{
    #line 244 "CriticalPath.pattern"

    m_on_critical_path = false;

    #ifndef _MPI
        #pragma omp master
        {
            m_on_critical_path = true;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Starts on " << event.get_location();
        }
    #endif
}


void
PatternCriticalPath::prepare_bws_cb(const CallbackManager& cbmanager,
                                    int                    user_event,
                                    const Event&           event,
                                    CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 225 "CriticalPath.pattern"

    #ifdef CPADEBUG
        int rank = 0, thread = 0;
        #ifdef _MPI
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        #endif
        #ifdef _OPENMP
            thread = omp_get_thread_num();
        #endif
        std::ostringstream filename;
        filename << "cpadbg_r" << rank << "t" << thread << ".out";
        m_dbg.open(filename.str().c_str());

        m_dbg << "bws prepare: mUpdateCallstack is " << data->mCallstack.getUpdate() << std::endl;
    #endif    // CPADEBUG
}


void
PatternCriticalPath::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                            int                    user_event,
                                            const Event&           event,
                                            CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 583 "CriticalPath.pattern"

    #ifdef _MPI
        Buffer*  buffer = new Buffer(16);
        uint32_t flag   = 0;

        if (  m_on_critical_path
           && data->mSynchpointHandler->isWaitstate(event))
        {
            // --- set severities
            update_path_severities(event, data);

            // --- critical path continues on wait-state causing process

            flag               = 1;
            m_on_critical_path = false;
        }

        buffer->put_uint32(flag);
        data->mInvLocal->add_buffer(buffer, BUFFER_CRITICALPATH);
    #endif
}


void
PatternCriticalPath::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 516 "CriticalPath.pattern"

    #ifdef _MPI
        Buffer*  buffer = new Buffer(16);
        uint32_t flag   = 0;

        if (  m_on_critical_path
           && data->mSynchpointHandler->isWaitstate(event))
        {
            // --- set severities
            update_path_severities(event, data);

            // --- critical path continues on wait-state causing process

            flag               = 1;
            m_on_critical_path = false;
        }

        buffer->put_uint32(flag);
        data->mLocal->add_buffer(buffer, BUFFER_CRITICALPATH);
    #endif
}


void
PatternCriticalPath::sync_coll_bwc_cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 450 "CriticalPath.pattern"

    #ifdef _MPI
        const CollectiveInfo& ci = data->mCollinfo;

        int inFlag = 0;
        if (  m_on_critical_path
           && data->mSynchpointHandler->isWaitstate(event))
        {
            update_path_severities(event, data);

            inFlag             = 1;
            m_on_critical_path = false;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Moves to rank "
                << event->getComm()->getCommSet().getGlobalRank(ci.latest.mRank);
        }

        int outFlag = 0;
        MPI_Reduce(&inFlag, &outFlag, 1, MPI_INT, MPI_MAX, ci.latest.mRank,
                   event->getComm()->getHandle());

        if (outFlag)
        {
            m_on_critical_path = true;
            m_end_event        = event;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << "Critical Path: Continues on " << event.get_location();
        }
    #endif
}


/*-------------------------------------------------------------------------*/
/**
 *  @class PatternCriticalPathActivities
 *  @brief Activity Impact metric.
 *
 *  Implements the "Activity Impact" metric.
 */
/*-------------------------------------------------------------------------*/

class PatternCriticalPathActivities
    : public Pattern
{
    public:
        /// @name Registering callbacks
        /// @{

        virtual void
        reg_cb(CallbackManagerMap& cbmanagers);

        /// @}
        /// @name Retrieving pattern information
        /// @{

        virtual long
        get_id() const
        {
            return PAT_CRITICAL_PATH_ACTIVITIES;
        }

        virtual long
        get_parent() const
        {
            return PAT_PERFORMANCE_IMPACT_CRITICALPATH;
        }

        virtual string
        get_name() const
        {
            return "Activity impact";
        }

        virtual string
        get_unique_name() const
        {
            return "critical_path_activities";
        }

        virtual string
        get_descr() const
        {
            return "Resource consumption of activities on the critical"
                   " path";
        }

        virtual string
        get_unit() const
        {
            return "sec";
        }

        virtual CubeMetricType
        get_mode() const
        {
            return CUBE_METRIC_EXCLUSIVE;
        }

        /// @}
        /// @name Callback methods
        /// @{

        void
        criticalpath_finished_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

        /// @}
};


// --- Registering callbacks ------------------------------------------------

void
PatternCriticalPathActivities::reg_cb(CallbackManagerMap& cbmanagers)
{
    init();

    CallbackManagerMap::iterator    it;
    CallbackManagerMap::mapped_type cbmanager;

    // Register bwc callbacks
    it = cbmanagers.find("bwc");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        CRITICALPATH_FINISHED,
        PEARL_create_callback(
            this,
            &PatternCriticalPathActivities::criticalpath_finished_bwc_cb));
}


// --- Callback methods -----------------------------------------------------

void
PatternCriticalPathActivities::criticalpath_finished_bwc_cb(const CallbackManager& cbmanager,
                                                            int                    user_event,
                                                            const Event&           event,
                                                            CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 716 "CriticalPath.pattern"

    // Shortcuts
    const vector< double >& gcpath = data->mGlobalCriticalPath;
    const vector< double >& ltimes = data->mLocalTimeProfile;

    const size_t numCallpaths = data->mDefs->numCallpaths();
    for (size_t i = 0; i < numCallpaths; ++i)
    {
        if (gcpath[i] > 0)
        {
            Callpath* const callpath = data->mDefs->get_cnode(i);
            const double    impact   = min(gcpath[i], ltimes[i]);

            m_severity[callpath] = impact;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << get_name() << " @ " << *callpath
                << ": " << impact;
        }
    }
}


/*-------------------------------------------------------------------------*/
/**
 *  @class PatternCriticalImbalanceImpact
 *  @brief Critical Imbalance Impact metric.
 *
 *  Implements the "Critical Imbalance Impact" metric.
 */
/*-------------------------------------------------------------------------*/

class PatternCriticalImbalanceImpact
    : public Pattern
{
    public:
        /// @name Registering callbacks
        /// @{

        virtual void
        reg_cb(CallbackManagerMap& cbmanagers);

        /// @}
        /// @name Retrieving pattern information
        /// @{

        virtual long
        get_id() const
        {
            return PAT_CRITICAL_IMBALANCE_IMPACT;
        }

        virtual long
        get_parent() const
        {
            return PAT_PERFORMANCE_IMPACT_CRITICALPATH;
        }

        virtual string
        get_name() const
        {
            return "Critical-path imbalance impact";
        }

        virtual string
        get_unique_name() const
        {
            return "critical_imbalance_impact";
        }

        virtual string
        get_descr() const
        {
            return "Performance impact of load imbalance on the critical"
                   " path";
        }

        virtual string
        get_unit() const
        {
            return "sec";
        }

        virtual CubeMetricType
        get_mode() const
        {
            return CUBE_METRIC_EXCLUSIVE;
        }

        /// @}
        /// @name Callback methods
        /// @{

        void
        criticalpath_finished_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

        /// @}


    private:
        #line 768 "CriticalPath.pattern"

        static vector< double > globalImbalanceCosts;
};


// --- Static class data ----------------------------------------------------

#line 771 "CriticalPath.pattern"

vector< double > PatternCriticalImbalanceImpact::globalImbalanceCosts;


// --- Registering callbacks ------------------------------------------------

void
PatternCriticalImbalanceImpact::reg_cb(CallbackManagerMap& cbmanagers)
{
    init();

    CallbackManagerMap::iterator    it;
    CallbackManagerMap::mapped_type cbmanager;

    // Register bwc callbacks
    it = cbmanagers.find("bwc");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        CRITICALPATH_FINISHED,
        PEARL_create_callback(
            this,
            &PatternCriticalImbalanceImpact::criticalpath_finished_bwc_cb));
}


// --- Callback methods -----------------------------------------------------

void
PatternCriticalImbalanceImpact::criticalpath_finished_bwc_cb(const CallbackManager& cbmanager,
                                                             int                    user_event,
                                                             const Event&           event,
                                                             CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 775 "CriticalPath.pattern"

    // Shortcuts
    const size_t numCallpaths = data->mDefs->numCallpaths();

    const vector< double >& gcpath = data->mGlobalCriticalPath;
    const vector< double >& lcpath = data->mLocalCriticalPath;
    const vector< double >& ltimes = data->mLocalTimeProfile;

    // Calculate aggregate local wait time
    // FIXME:
    // In theory, the wait time can be calculated as the totel time on the
    // global critical path minus the local time profile (which excludes
    // wait time).  However, in practice this difference may be negative.
    // This needs to be investigated in more depth...
    const double gcplen    = accumulate(gcpath.begin(), gcpath.end(), 0.0);
    const double lplen     = accumulate(ltimes.begin(), ltimes.end(), 0.0);
    const double aggrWaitT = gcplen - lplen;

    // Calculate local per-callpath critical-path excess time
    // (see thesis D. Boehme, p. 83, Eq. 1)
    vector< double > localImbalanceCosts;
    localImbalanceCosts.reserve(numCallpaths);
    for (size_t i = 0; i < numCallpaths; ++i)
    {
        localImbalanceCosts.push_back(max(gcpath[i] - ltimes[i], 0.0));
    }

    // Calculate aggregate local critical-path excess time (p. 84, Eq. 1);
    // excess time for call paths not on critical path is 0
    const double aggrExcessT = accumulate(localImbalanceCosts.begin(),
                                          localImbalanceCosts.end(), 0.0);

    // Calculate local critical-path imbalance costs (p. 83, Eq. 2)
    const double scalingFactor = aggrWaitT / aggrExcessT;
    transform(localImbalanceCosts.begin(), localImbalanceCosts.end(),
              localImbalanceCosts.begin(),
              bind2nd(multiplies< double >(), scalingFactor));

    // Accumulate global critical-path imbalance costs
    #pragma omp single
    {
        globalImbalanceCosts.assign(numCallpaths, 0.0);
    }
    #pragma omp critical
    {
        transform(globalImbalanceCosts.begin(), globalImbalanceCosts.end(),
                  localImbalanceCosts.begin(),
                  globalImbalanceCosts.begin(),
                  plus< double >());
    }
    #pragma omp barrier

    #ifdef _MPI
        #pragma omp master
        {
            MPI_Allreduce(MPI_IN_PLACE, &globalImbalanceCosts[0], numCallpaths,
                          MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        }
        #pragma omp barrier
    #endif

    // Proportionally distribute global critical-path imbalance costs to
    // call paths on the critical path
    for (size_t i = 0; i < numCallpaths; ++i)
    {
        if (lcpath[i] > 0)    // implies 'gcpath[i] > 0'
        {
            // FIXME: calculated costs should never be negative
            const double costs = (lcpath[i] / gcpath[i]) * globalImbalanceCosts[i];
            if (costs > 0)
            {
                Callpath* const callpath = data->mDefs->get_cnode(i);

                m_severity[callpath] = costs;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                    << get_name() << " @ " << *callpath
                    << ": " << costs;
            }
        }
    }
}


/*-------------------------------------------------------------------------*/
/**
 *  @class PatternInterPartitionImbalance
 *  @brief Inter-partition Imbalance metric.
 *
 *  Implements the "Inter-partition Imbalance" metric.
 */
/*-------------------------------------------------------------------------*/

class PatternInterPartitionImbalance
    : public Pattern
{
    public:
        /// @name Registering callbacks
        /// @{

        virtual void
        reg_cb(CallbackManagerMap& cbmanagers);

        /// @}
        /// @name Retrieving pattern information
        /// @{

        virtual long
        get_id() const
        {
            return PAT_INTER_PARTITION_IMBALANCE;
        }

        virtual long
        get_parent() const
        {
            return PAT_CRITICAL_IMBALANCE_IMPACT;
        }

        virtual string
        get_name() const
        {
            return "Inter-partition imbalance";
        }

        virtual string
        get_unique_name() const
        {
            return "inter_partition_imbalance";
        }

        virtual string
        get_descr() const
        {
            return "Performance impact of load imbalance between process"
                   " partitions that perform different activities";
        }

        virtual string
        get_unit() const
        {
            return "sec";
        }

        virtual CubeMetricType
        get_mode() const
        {
            return CUBE_METRIC_EXCLUSIVE;
        }

        /// @}
        /// @name Callback methods
        /// @{

        void
        criticalpath_finished_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

        /// @}


    private:
        #line 906 "CriticalPath.pattern"

        static vector< double > phr;
};


// --- Static class data ----------------------------------------------------

#line 909 "CriticalPath.pattern"

vector< double > PatternInterPartitionImbalance::phr;


// --- Registering callbacks ------------------------------------------------

void
PatternInterPartitionImbalance::reg_cb(CallbackManagerMap& cbmanagers)
{
    init();

    CallbackManagerMap::iterator    it;
    CallbackManagerMap::mapped_type cbmanager;

    // Register bwc callbacks
    it = cbmanagers.find("bwc");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        CRITICALPATH_FINISHED,
        PEARL_create_callback(
            this,
            &PatternInterPartitionImbalance::criticalpath_finished_bwc_cb));
}


// --- Callback methods -----------------------------------------------------

void
PatternInterPartitionImbalance::criticalpath_finished_bwc_cb(const CallbackManager& cbmanager,
                                                             int                    user_event,
                                                             const Event&           event,
                                                             CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 913 "CriticalPath.pattern"

    //
    // --- calculate local headroom

    // shortcuts
    const size_t            ncnodes = data->mDefs->numCallpaths();
    const vector< double >& gcpath(data->mGlobalCriticalPath);
    const vector< double >& lcpath(data->mLocalCriticalPath);
    const vector< double >& ltimes(data->mLocalTimeProfile);

    double gcplen = std::accumulate(gcpath.begin(), gcpath.end(), 0.0);
    double lplen  = 0.0;

    vector< double > hr(gcpath);

    for (size_t i = 0; i < ncnodes; ++i)
    {
        if (ltimes[i] > 0.0)
        {
            lplen += ltimes[i] + (hr[i] - std::min(ltimes[i], hr[i]));
            hr[i]  = 0.0;
        }
    }

    double lhrlen = std::accumulate(hr.begin(), hr.end(), 0.0);
    double hrf    = (gcplen - lplen) / lhrlen;

    for (size_t i = 0; i < ncnodes; ++i)
    {
        hr[i] *= hrf;
    }

    //
    // --- exchange headrooms and calculate severity

    #pragma omp master
    {
        phr.assign(ncnodes, 0.0);
    }
    #pragma omp barrier
    { }
    #pragma omp critical
    {
        std::transform(phr.begin(), phr.end(), hr.begin(),
                       phr.begin(), std::plus< double >());
    }
    #pragma omp barrier
    { }

    #ifdef _MPI
        #pragma omp master
        {
            MPI_Allreduce(MPI_IN_PLACE, &phr[0], ncnodes, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        }
    #endif

    #pragma omp barrier
    { }
    hr.assign(phr.begin(), phr.end());

    for (uint32_t i = 0; i < ncnodes; ++i)
    {
        if (!(lcpath[i] > 0.0))
        {
            continue;
        }

        double sev = (lcpath[i] / gcpath[i]) * hr[i];

        if (sev > 0.0)
        {
            m_severity[data->mDefs->get_cnode(i)] += sev;
        }
    }
}


/*-------------------------------------------------------------------------*/
/**
 *  @class PatternNonCriticalPathActivities
 *  @brief Non-critical-path Activities metric.
 *
 *  Implements the "Non-critical-path Activities" metric.
 */
/*-------------------------------------------------------------------------*/

class PatternNonCriticalPathActivities
    : public Pattern
{
    public:
        /// @name Registering callbacks
        /// @{

        virtual void
        reg_cb(CallbackManagerMap& cbmanagers);

        /// @}
        /// @name Retrieving pattern information
        /// @{

        virtual long
        get_id() const
        {
            return PAT_NON_CRITICAL_PATH_ACTIVITIES;
        }

        virtual long
        get_parent() const
        {
            return PAT_PERFORMANCE_IMPACT;
        }

        virtual string
        get_name() const
        {
            return "Non-critical-path activities";
        }

        virtual string
        get_unique_name() const
        {
            return "non_critical_path_activities";
        }

        virtual string
        get_descr() const
        {
            return "Overall resource consumption of activities that are"
                   " not on the critical path";
        }

        virtual string
        get_unit() const
        {
            return "sec";
        }

        virtual CubeMetricType
        get_mode() const
        {
            return CUBE_METRIC_EXCLUSIVE;
        }

        /// @}
        /// @name Callback methods
        /// @{

        void
        criticalpath_finished_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

        /// @}
};


// --- Registering callbacks ------------------------------------------------

void
PatternNonCriticalPathActivities::reg_cb(CallbackManagerMap& cbmanagers)
{
    init();

    CallbackManagerMap::iterator    it;
    CallbackManagerMap::mapped_type cbmanager;

    // Register bwc callbacks
    it = cbmanagers.find("bwc");
    assert(it != cbmanagers.end());

    cbmanager = it->second;
    cbmanager->register_callback(
        CRITICALPATH_FINISHED,
        PEARL_create_callback(
            this,
            &PatternNonCriticalPathActivities::criticalpath_finished_bwc_cb));
}


// --- Callback methods -----------------------------------------------------

void
PatternNonCriticalPathActivities::criticalpath_finished_bwc_cb(const CallbackManager& cbmanager,
                                                               int                    user_event,
                                                               const Event&           event,
                                                               CallbackData*          cdata)
{
    CbData* data = static_cast< CbData* >(cdata);

    #line 1007 "CriticalPath.pattern"

    // Shortcuts
    const vector< double >& gcpath = data->mGlobalCriticalPath;
    const vector< double >& ltimes = data->mLocalTimeProfile;

    const size_t numCallpaths = data->mDefs->numCallpaths();
    for (uint32_t i = 0; i < numCallpaths; ++i)
    {
        const double impact = ltimes[i] - gcpath[i];
        if (impact > 0)
        {
            Callpath* const callpath = data->mDefs->get_cnode(i);

            m_severity[callpath] = impact;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_CRITICAL_PATH)
                << get_name() << " @ " << *callpath
                << ": " << impact;
        }
    }
}


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_LateSender
     *  @brief Short-term MPI Late Sender Delay Costs metric.
     *
     *  Implements the "Short-term MPI Late Sender Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_LateSender
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_LATESENDER;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_LATESENDER_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI short-term Late Sender delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_latesender";
            }

            virtual string
            get_descr() const
            {
                return "Delays that directly cause MPI Late Sender wait"
                       " states";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            post_send_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            prepare_bwc_cb(const CallbackManager& cbmanager,
                           int                    user_event,
                           const Event&           event,
                           CallbackData*          cdata);

            void
            pre_recv_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        private:
            #line 187 "Delay.MPI.pattern"

            #ifdef DELAYDEBUG
                std::ofstream m_dbg;
            #endif
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_LateSender::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PREPARE,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender::prepare_bwc_cb));
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender::pre_recv_bwc_cb));
        cbmanager->register_callback(
            POST_SEND,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender::post_send_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_LateSender::post_send_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 229 "Delay.MPI.pattern"

        // Shortcuts
        const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
        const SynchpointHandler* SpH = data->mSynchpointHandler;

        // is this a synchpoint?
        if (!SpH->isSynchpoint(event))
        {
            return;
        }

        // --- get delay info buffer

        Buffer* buffer = data->mRemote->get_buffer(BUFFER_DELAY);
        data->mRwait = buffer->get_double();

        // was there a late-sender wait state?
        if (!(data->mRwait > 0))
        {
            return;
        }

        // --- find previous synchronization point

        int rank = event->getComm()->getCommSet().getGlobalRank(event->getDestination());
        data->mPrevSp = SpH->findPreviousMpiSynchpoint(event, rank);

        // --- get local and remote time maps and calculate delay

        data->mPropSpMap = SpH->getSynchpointsBetween(data->mPrevSp, event);
        data->mWaitInfo  = CalculateWaitMap(data->mPropSpMap);
        data->mDelayInfo =
            CalculateDiff(PruneDelayTimemap(TmH->getRuntimes(data->mPrevSp, event),
                                            data->mWaitInfo.mMap,
                                            data->mPrevSp,
                                            SpH->getSynchpointInfo(data->mPrevSp)),
                          UnpackTimemap(buffer));

        // shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        // --- calculate delay costs

        double scale = 0.0;

        if ((d.mSum + w.mSum) > 0)
        {
            scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
        }

        #ifdef DELAYDEBUG
            m_dbg << "\nLate-sender delay rwait = " << data->mRwait
                  << " delay sum = " << d.mSum << " wait sum = " << w.mSum
                  << "\nEvent" << event << "\nPrevSp" << data->mPrevSp
                  << "Delay map\n";
            for (timemap_t::const_iterator it = d.mMap.begin();
                 it != d.mMap.end();
                 ++it)
            {
                m_dbg << " (" << data->mDefs->getCallpath(it->first).getRegion().getDisplayName().getString()
                      << ", " << it->second << ")\n";
            }
        #endif

        // --- set severities

        if (d.mSum > 0)
        {
            for (timemap_t::const_iterator it = d.mMap.begin();
                 it != d.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
            }
        }

        // --- invoke subsequent handlers

        cbmanager.notify(DELAY_LATE_SENDER, event, data);
    }


    void
    PatternDelay_LateSender::prepare_bwc_cb(const CallbackManager& cbmanager,
                                            int                    user_event,
                                            const Event&           event,
                                            CallbackData*          cdata)
    {
        #line 193 "Delay.MPI.pattern"

        #ifdef DELAYDEBUG
            int rank = 0, thread = 0;
            #ifdef _MPI
                MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            #endif
            std::ostringstream filename;
            filename << "delaydbg_r" << rank << "t" << thread << ".out";
            m_dbg.open(filename.str().c_str());
        #endif    // DELAYDEBUG
    }


    void
    PatternDelay_LateSender::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 205 "Delay.MPI.pattern"

        // Shortcuts
        const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
        const SynchpointHandler* SpH = data->mSynchpointHandler;

        // cost-accounting backward replay

        Buffer*        buffer = new Buffer(256);
        SynchpointInfo spi    = SpH->getSynchpointInfo(event);

        buffer->put_double(spi.waitTime);

        timemap_t tmap;

        if (spi.waitTime > 0)
        {
            int rank = event->getComm()->getCommSet().getGlobalRank(event->getSource());
            tmap = GetMpiDelayTimemap(event, rank, SpH, TmH);
        }

        PackTimemap(buffer, tmap);
        data->mLocal->add_buffer(buffer, BUFFER_DELAY);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_LateSender_LT
     *  @brief Long-term MPI Late Sender Delay Costs metric.
     *
     *  Implements the "Long-term MPI Late Sender Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_LateSender_LT
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_LATESENDER_LONGTERM;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_LATESENDER_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI long-term Late Sender delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_latesender_longterm";
            }

            virtual string
            get_descr() const
            {
                return "Delays that indirectly cause MPI Late Sender wait"
                       " states through wait-state propagation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_12n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            delay_n21_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata);

            void
            pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            pre_recv_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        private:
            #line 344 "Delay.MPI.pattern"

            DelayCostMap m_cost;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_LateSender_LT::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::pre_recv_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::delay_late_sender_bwc_cb));
        cbmanager->register_callback(
            PRE_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::pre_inv_sendcmp_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::delay_late_receiver_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::pre_delay_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::delay_barrier_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::pre_delay_n2n_bwc_cb));
        cbmanager->register_callback(
            DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::delay_n2n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::pre_delay_12n_bwc_cb));
        cbmanager->register_callback(
            DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::delay_12n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::pre_delay_n21_bwc_cb));
        cbmanager->register_callback(
            DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::delay_n21_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::pre_delay_omp_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateSender_LT::delay_omp_barrier_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_LateSender_LT::delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 488 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LS);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LS);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateSender_LT::delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                     int                    user_event,
                                                     const Event&           event,
                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 432 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LS);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LS);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateSender_LT::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                           int                    user_event,
                                                           const Event&           event,
                                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 396 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_LS)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_LateSender_LT::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 355 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost

        double rcost = data->mRemote->get_buffer(BUFFER_RCOST_LS)->get_double();

        // --- set severities

        double scale_w = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
        double scale_c = rcost / (d.mSum + w.mSum);

        if (scale_c > 0)
        {
            for (timemap_t::const_iterator it = d.mMap.begin();
                 it != d.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += scale_c * it->second;
            }
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale_w + scale_c);
    }


    void
    PatternDelay_LateSender_LT::delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 516 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LS);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LS);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateSender_LT::delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 460 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LS);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LS);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateSender_LT::delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 544 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LS);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LS);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateSender_LT::pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                                     int                    user_event,
                                                     const Event&           event,
                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 484 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LS, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateSender_LT::pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 428 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LS, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateSender_LT::pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                                     int                    user_event,
                                                     const Event&           event,
                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 512 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LS, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateSender_LT::pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                                     int                    user_event,
                                                     const Event&           event,
                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 456 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LS, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateSender_LT::pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                             int                    user_event,
                                                             const Event&           event,
                                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 540 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LS, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateSender_LT::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 389 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_LS);
    }


    void
    PatternDelay_LateSender_LT::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 348 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mLocal->add_buffer(buffer, BUFFER_RCOST_LS);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_LateReceiver
     *  @brief Short-term MPI Late Receiver Delay Costs metric.
     *
     *  Implements the "Short-term MPI Late Receiver Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_LateReceiver
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_LATERECEIVER;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_LATERECEIVER_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI short-term Late Receiver delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_latereceiver";
            }

            virtual string
            get_descr() const
            {
                return "Delays that directly cause MPI Late Receiver wait"
                       " states";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            post_inv_recvreq_bwc_cb(const CallbackManager& cbmanager,
                                    int                    user_event,
                                    const Event&           event,
                                    CallbackData*          cdata);

            void
            pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_LateReceiver::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver::pre_inv_sendcmp_bwc_cb));
        cbmanager->register_callback(
            POST_INV_RECVREQ,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver::post_inv_recvreq_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_LateReceiver::post_inv_recvreq_bwc_cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 718 "Delay.MPI.pattern"

        // Shortcuts
        const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
        const SynchpointHandler* SpH = data->mSynchpointHandler;

        // is this a synchpoint?
        if (!SpH->isSynchpoint(event))
        {
            return;
        }

        // --- get delay info buffer

        Buffer* buffer = data->mInvRemote->get_buffer(BUFFER_DELAY);
        data->mRwait = buffer->get_double();

        // was there a late-receiver wait state?
        if (!(data->mRwait > 0))
        {
            return;
        }

        // --- find previous synchronization point

        Event recvcmp = event.completion();
        int   rank    = recvcmp->getComm()->getCommSet().getGlobalRank(recvcmp->getSource());
        data->mPrevSp = SpH->findPreviousMpiSynchpoint(event, rank);

        // --- get local and remote time maps and calculate delay

        data->mPropSpMap = SpH->getSynchpointsBetween(data->mPrevSp, event);
        data->mWaitInfo  = CalculateWaitMap(data->mPropSpMap);
        data->mDelayInfo =
            CalculateDiff(PruneDelayTimemap(TmH->getRuntimes(data->mPrevSp, event),
                                            data->mWaitInfo.mMap,
                                            data->mPrevSp,
                                            SpH->getSynchpointInfo(data->mPrevSp)),
                          UnpackTimemap(buffer));

        // shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        // --- calculate delay costs

        double scale = 0.0;

        if ((d.mSum + w.mSum) > 0)
        {
            scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
        }

        // --- set severities

        if (d.mSum > 0)
        {
            for (timemap_t::const_iterator it = d.mMap.begin();
                 it != d.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
            }
        }

        // --- invoke subsequent handlers

        cbmanager.notify(DELAY_LATE_RECEIVER, event, data);
    }


    void
    PatternDelay_LateReceiver::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 693 "Delay.MPI.pattern"

        // Shortcuts
        const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
        const SynchpointHandler* SpH = data->mSynchpointHandler;

        // cost-accounting backward replay

        Buffer*        buffer = new Buffer(256);
        SynchpointInfo spi    = SpH->getSynchpointInfo(event);

        buffer->put_double(spi.waitTime);

        timemap_t tmap;

        if (spi.waitTime > 0)
        {
            Event sendreq = event.request();
            int   rank    = sendreq->getComm()->getCommSet().getGlobalRank(sendreq->getDestination());
            tmap = GetMpiDelayTimemap(event, rank, SpH, TmH);
        }

        PackTimemap(buffer, tmap);
        data->mInvLocal->add_buffer(buffer, BUFFER_DELAY);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_LateReceiver_LT
     *  @brief Long-term MPI Late Receiver Delay Costs metric.
     *
     *  Implements the "Long-term MPI Late Receiver Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_LateReceiver_LT
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_LATERECEIVER_LONGTERM;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_LATERECEIVER_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI long-term Late Receiver delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_latereceiver_longterm";
            }

            virtual string
            get_descr() const
            {
                return "Delays that indirectly cause MPI Late Receiver wait"
                       " states through wait-state propagation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_12n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            delay_n21_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata);

            void
            pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            pre_recv_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        private:
            #line 820 "Delay.MPI.pattern"

            DelayCostMap m_cost;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_LateReceiver_LT::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::pre_recv_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::delay_late_sender_bwc_cb));
        cbmanager->register_callback(
            PRE_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::pre_inv_sendcmp_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::delay_late_receiver_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::pre_delay_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::delay_barrier_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::pre_delay_n2n_bwc_cb));
        cbmanager->register_callback(
            DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::delay_n2n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::pre_delay_12n_bwc_cb));
        cbmanager->register_callback(
            DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::delay_12n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::pre_delay_n21_bwc_cb));
        cbmanager->register_callback(
            DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::delay_n21_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::pre_delay_omp_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_LateReceiver_LT::delay_omp_barrier_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_LateReceiver_LT::delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                                   int                    user_event,
                                                   const Event&           event,
                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 964 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LR);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LR);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateReceiver_LT::delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 908 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LR);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LR);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateReceiver_LT::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                             int                    user_event,
                                                             const Event&           event,
                                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 870 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost

        double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_LR)->get_double();

        // --- set severities

        double scale_w = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
        double scale_c = rcost / (d.mSum + w.mSum);

        if (scale_c > 0)
        {
            for (timemap_t::const_iterator it = d.mMap.begin();
                 it != d.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += scale_c * it->second;
            }
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale_w + scale_c);
    }


    void
    PatternDelay_LateReceiver_LT::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                           int                    user_event,
                                                           const Event&           event,
                                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 831 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mRemote->get_buffer(BUFFER_RCOST_LR)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_LateReceiver_LT::delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                                   int                    user_event,
                                                   const Event&           event,
                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 992 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LR);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LR);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateReceiver_LT::delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                                   int                    user_event,
                                                   const Event&           event,
                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 936 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LR);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LR);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateReceiver_LT::delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                           int                    user_event,
                                                           const Event&           event,
                                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1020 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LR);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LR);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_LateReceiver_LT::pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 960 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LR, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateReceiver_LT::pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                           int                    user_event,
                                                           const Event&           event,
                                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 904 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LR, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateReceiver_LT::pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 988 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LR, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateReceiver_LT::pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 932 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LR, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateReceiver_LT::pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                               int                    user_event,
                                                               const Event&           event,
                                                               CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1016 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_LR, m_cost.getCosts(event)));
    }


    void
    PatternDelay_LateReceiver_LT::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 863 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_LR);
    }


    void
    PatternDelay_LateReceiver_LT::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 824 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mLocal->add_buffer(buffer, BUFFER_RCOST_LR);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_Barrier
     *  @brief Short-term MPI Barrier Delay Costs metric.
     *
     *  Implements the "Short-term MPI Barrier Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_Barrier
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_BARRIER;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_BARRIER_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI short-term barrier delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_barrier";
            }

            virtual string
            get_descr() const
            {
                return "Delays that directly cause wait states in MPI"
                       " barriers";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            sync_coll_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_Barrier::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            SYNC_COLL,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier::sync_coll_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_Barrier::sync_coll_bwc_cb(const CallbackManager& cbmanager,
                                           int                    user_event,
                                           const Event&           event,
                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1184 "Delay.MPI.pattern"

        const CollectiveInfo& ci = data->mCollinfo;

        data->mInputScales.clear();

        cbmanager.notify(PRE_DELAY_BARRIER, event, data);

        CalculateCollectiveDelay(ci.my, ci.latest, event, data);

        if (ci.my.mRank == ci.latest.mRank)
        {
            // algorithm 1, line 24
            map< uint32_t, TimeMapSum >::const_iterator dWit = data->mCollDelayInfo.find(KEY_WAITTIME);
            if (  (dWit != data->mCollDelayInfo.end())
               && (dWit->second.mSum > 0.0))
            {
                for (timemap_t::const_iterator it = dWit->second.mMap.begin();
                     it != dWit->second.mMap.end();
                     ++it)
                {
                    Callpath* const callpath = data->mDefs->get_cnode(it->first);

                    m_severity[callpath] += it->second;

                    UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                        << get_name() << " @ " << *callpath
                        << ": " << it->second
                        << " [total: " << m_severity[callpath] << ']';
                }
            }

            cbmanager.notify(DELAY_BARRIER, event, data);
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_Barrier_LT
     *  @brief Long-term MPI Barrier Delay Costs metric.
     *
     *  Implements the "Long-term MPI Barrier Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_Barrier_LT
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_BARRIER_LONGTERM;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_BARRIER_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI long-term barrier delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_barrier_longterm";
            }

            virtual string
            get_descr() const
            {
                return "Delays that indirectly cause wait states in MPI"
                       " barriers through wait-state propagation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_12n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            delay_n21_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata);

            void
            pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            pre_recv_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        private:
            #line 1245 "Delay.MPI.pattern"

            DelayCostMap m_cost;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_Barrier_LT::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::pre_recv_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::delay_late_sender_bwc_cb));
        cbmanager->register_callback(
            PRE_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::pre_inv_sendcmp_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::delay_late_receiver_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::pre_delay_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::delay_barrier_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::pre_delay_n2n_bwc_cb));
        cbmanager->register_callback(
            DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::delay_n2n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::pre_delay_12n_bwc_cb));
        cbmanager->register_callback(
            DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::delay_12n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::pre_delay_n21_bwc_cb));
        cbmanager->register_callback(
            DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::delay_n21_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::pre_delay_omp_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_Barrier_LT::delay_omp_barrier_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_Barrier_LT::delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1408 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_BARRIER);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_BARRIER);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_Barrier_LT::delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1331 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_BARRIER);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_BARRIER);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }

        // algorithm 1, line 28 for gamma_short
        map< uint32_t, ScaleMap >::const_iterator sWit = data->mSumScales.find(KEY_WAITTIME);
        if (sWit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sWit->second);
        }
    }


    void
    PatternDelay_Barrier_LT::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                        int                    user_event,
                                                        const Event&           event,
                                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1295 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_BARRIER)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_Barrier_LT::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1256 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mRemote->get_buffer(BUFFER_RCOST_BARRIER)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_Barrier_LT::delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1443 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_BARRIER);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_BARRIER);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_Barrier_LT::delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1373 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_BARRIER);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_BARRIER);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_Barrier_LT::delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1478 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_BARRIER);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_BARRIER);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_Barrier_LT::pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1404 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_BARRIER, m_cost.getCosts(event)));
    }


    void
    PatternDelay_Barrier_LT::pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1327 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_BARRIER, m_cost.getCosts(event)));
    }


    void
    PatternDelay_Barrier_LT::pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1439 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_BARRIER, m_cost.getCosts(event)));
    }


    void
    PatternDelay_Barrier_LT::pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1369 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_BARRIER, m_cost.getCosts(event)));
    }


    void
    PatternDelay_Barrier_LT::pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                          int                    user_event,
                                                          const Event&           event,
                                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1474 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_BARRIER, m_cost.getCosts(event)));
    }


    void
    PatternDelay_Barrier_LT::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1288 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_BARRIER);
    }


    void
    PatternDelay_Barrier_LT::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1249 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mLocal->add_buffer(buffer, BUFFER_RCOST_BARRIER);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_NxN
     *  @brief Short-term MPI N x N Collectives Delay Costs metric.
     *
     *  Implements the "Short-term MPI N x N Collectives Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_NxN
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_N2N;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_N2N_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI short-term N x N delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_n2n";
            }

            virtual string
            get_descr() const
            {
                return "Delays that directly cause wait states in MPI n-to-n"
                       " collective communications";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            coll_n2n_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_NxN::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            COLL_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN::coll_n2n_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_NxN::coll_n2n_bwc_cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1547 "Delay.MPI.pattern"

        const CollectiveInfo& ci = data->mCollinfo;

        data->mInputScales.clear();

        cbmanager.notify(PRE_DELAY_N2N, event, data);

        CalculateCollectiveDelay(ci.my, ci.latest, event, data);

        if (ci.my.mRank == ci.latest.mRank)
        {
            // algorithm 1, line 24
            map< uint32_t, TimeMapSum >::const_iterator dWit = data->mCollDelayInfo.find(KEY_WAITTIME);
            if (  (dWit != data->mCollDelayInfo.end())
               && (dWit->second.mSum > 0.0))
            {
                for (timemap_t::const_iterator it = dWit->second.mMap.begin();
                     it != dWit->second.mMap.end();
                     ++it)
                {
                    Callpath* const callpath = data->mDefs->get_cnode(it->first);

                    m_severity[callpath] += it->second;

                    UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                        << get_name() << " @ " << *callpath
                        << ": " << it->second
                        << " [total: " << m_severity[callpath] << ']';
                }
            }

            cbmanager.notify(DELAY_N2N, event, data);
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_NxN_LT
     *  @brief Long-term MPI N x N Collectives Delay Costs metric.
     *
     *  Implements the "Long-term MPI N x N Collectives Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_NxN_LT
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_N2N_LONGTERM;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_N2N_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI long-term N x N delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_n2n_longterm";
            }

            virtual string
            get_descr() const
            {
                return "Delays that indirectly cause wait states in MPI"
                       " n-to-n collective communications through wait-state"
                       " propagation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_12n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            delay_n21_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata);

            void
            pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            pre_recv_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        private:
            #line 1608 "Delay.MPI.pattern"

            DelayCostMap m_cost;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_NxN_LT::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::pre_recv_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::delay_late_sender_bwc_cb));
        cbmanager->register_callback(
            PRE_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::pre_inv_sendcmp_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::delay_late_receiver_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::pre_delay_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::delay_barrier_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::pre_delay_n2n_bwc_cb));
        cbmanager->register_callback(
            DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::delay_n2n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::pre_delay_12n_bwc_cb));
        cbmanager->register_callback(
            DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::delay_12n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::pre_delay_n21_bwc_cb));
        cbmanager->register_callback(
            DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::delay_n21_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::pre_delay_omp_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_NxN_LT::delay_omp_barrier_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_NxN_LT::delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1771 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N2N);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N2N);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_NxN_LT::delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1694 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N2N);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N2N);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_NxN_LT::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1658 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_N2N)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_NxN_LT::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1619 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mRemote->get_buffer(BUFFER_RCOST_N2N)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_NxN_LT::delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1806 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N2N);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N2N);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_NxN_LT::delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1729 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N2N);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N2N);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }

        // algorithm 1, line 28 for gamma_short
        map< uint32_t, ScaleMap >::const_iterator sWit = data->mSumScales.find(KEY_WAITTIME);
        if (sWit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sWit->second);
        }
    }


    void
    PatternDelay_NxN_LT::delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1841 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N2N);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N2N);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_NxN_LT::pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1767 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N2N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_NxN_LT::pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1690 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N2N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_NxN_LT::pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1802 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N2N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_NxN_LT::pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1725 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N2N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_NxN_LT::pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1837 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N2N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_NxN_LT::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1651 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_N2N);
    }


    void
    PatternDelay_NxN_LT::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1612 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mLocal->add_buffer(buffer, BUFFER_RCOST_N2N);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_12N
     *  @brief Short-term MPI 1-to-N Collectives Delay Costs metric.
     *
     *  Implements the "Short-term MPI 1-to-N Collectives Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_12N
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_12N;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_12N_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI short-term 1-to-N delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_12n";
            }

            virtual string
            get_descr() const
            {
                return "Delays that directly cause wait states in MPI 1-to-n"
                       " collective communications";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            coll_12n_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_12N::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            COLL_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_12N::coll_12n_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_12N::coll_12n_bwc_cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1910 "Delay.MPI.pattern"

        const CollectiveInfo& ci = data->mCollinfo;

        data->mInputScales.clear();

        cbmanager.notify(PRE_DELAY_12N, event, data);

        CalculateCollectiveDelay(ci.my, ci.root, event, data);

        if (ci.my.mRank == ci.root.mRank)
        {
            // algorithm 1, line 24
            map< uint32_t, TimeMapSum >::const_iterator dWit = data->mCollDelayInfo.find(KEY_WAITTIME);
            if (  (dWit != data->mCollDelayInfo.end())
               && (dWit->second.mSum > 0.0))
            {
                for (timemap_t::const_iterator it = dWit->second.mMap.begin();
                     it != dWit->second.mMap.end();
                     ++it)
                {
                    Callpath* const callpath = data->mDefs->get_cnode(it->first);

                    m_severity[callpath] += it->second;

                    UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                        << get_name() << " @ " << *callpath
                        << ": " << it->second
                        << " [total: " << m_severity[callpath] << ']';
                }
            }

            cbmanager.notify(DELAY_12N, event, data);
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_12N_LT
     *  @brief Long-term MPI 1-to-N Collectives Delay Costs metric.
     *
     *  Implements the "Long-term MPI 1-to-N Collectives Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_12N_LT
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_12N_LONGTERM;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_12N_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI long-term 1-to-N delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_12n_longterm";
            }

            virtual string
            get_descr() const
            {
                return "Delays that indirectly cause wait states in MPI"
                       " 1-to-n collective communications through wait-state"
                       " propagation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_12n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            delay_n21_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata);

            void
            pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            pre_recv_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        private:
            #line 1971 "Delay.MPI.pattern"

            DelayCostMap m_cost;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_12N_LT::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::pre_recv_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::delay_late_sender_bwc_cb));
        cbmanager->register_callback(
            PRE_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::pre_inv_sendcmp_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::delay_late_receiver_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::pre_delay_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::delay_barrier_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::pre_delay_n2n_bwc_cb));
        cbmanager->register_callback(
            DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::delay_n2n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::pre_delay_12n_bwc_cb));
        cbmanager->register_callback(
            DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::delay_12n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::pre_delay_n21_bwc_cb));
        cbmanager->register_callback(
            DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::delay_n21_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::pre_delay_omp_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_12N_LT::delay_omp_barrier_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_12N_LT::delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2127 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_12N);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_12N);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }

        // algorithm 1, line 28 for gamma_short
        map< uint32_t, ScaleMap >::const_iterator sWit = data->mSumScales.find(KEY_WAITTIME);
        if (sWit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sWit->second);
        }
    }


    void
    PatternDelay_12N_LT::delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2057 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_12N);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_12N);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_12N_LT::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2021 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_12N)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_12N_LT::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1982 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mRemote->get_buffer(BUFFER_RCOST_12N)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_12N_LT::delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2169 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_12N);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_12N);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_12N_LT::delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2092 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_12N);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_12N);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_12N_LT::delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2204 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_12N);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_12N);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_12N_LT::pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2123 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_12N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_12N_LT::pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2053 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_12N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_12N_LT::pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2165 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_12N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_12N_LT::pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2088 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_12N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_12N_LT::pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2200 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_12N, m_cost.getCosts(event)));
    }


    void
    PatternDelay_12N_LT::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2014 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_12N);
    }


    void
    PatternDelay_12N_LT::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 1975 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mLocal->add_buffer(buffer, BUFFER_RCOST_12N);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_N21
     *  @brief Short-term MPI N-to-1 Collectives Delay Costs metric.
     *
     *  Implements the "Short-term MPI N-to-1 Collectives Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_N21
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_N21;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_N21_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI short-term N-to-1 delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_n21";
            }

            virtual string
            get_descr() const
            {
                return "Delays that directly cause wait states in MPI n-to-1"
                       " collective communications";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            coll_n21_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_N21::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            COLL_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_N21::coll_n21_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_N21::coll_n21_bwc_cb(const CallbackManager& cbmanager,
                                      int                    user_event,
                                      const Event&           event,
                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2273 "Delay.MPI.pattern"

        const CollectiveInfo& ci = data->mCollinfo;

        data->mInputScales.clear();

        cbmanager.notify(PRE_DELAY_N21, event, data);

        CalculateCollectiveDelay(ci.my, ci.latest, event, data);

        if (ci.my.mRank == ci.latest.mRank)
        {
            // algorithm 1, line 24
            map< uint32_t, TimeMapSum >::const_iterator dWit = data->mCollDelayInfo.find(KEY_WAITTIME);
            if (  (dWit != data->mCollDelayInfo.end())
               && (dWit->second.mSum > 0.0))
            {
                for (timemap_t::const_iterator it = dWit->second.mMap.begin();
                     it != dWit->second.mMap.end();
                     ++it)
                {
                    Callpath* const callpath = data->mDefs->get_cnode(it->first);

                    m_severity[callpath] += it->second;

                    UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                        << get_name() << " @ " << *callpath
                        << ": " << it->second
                        << " [total: " << m_severity[callpath] << ']';
                }
            }

            cbmanager.notify(DELAY_N21, event, data);
        }
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_N21_LT
     *  @brief Long-term MPI N-to-1 Collectives Delay Costs metric.
     *
     *  Implements the "Long-term MPI N-to-1 Collectives Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_N21_LT
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_N21_LONGTERM;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_N21_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "MPI long-term N-to-1 delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_n21_longterm";
            }

            virtual string
            get_descr() const
            {
                return "Delays that indirectly cause wait states in MPI"
                       " n-to-1 collective communications through wait-state"
                       " propagation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_12n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            delay_n21_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata);

            void
            pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            pre_recv_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        private:
            #line 2334 "Delay.MPI.pattern"

            DelayCostMap m_cost;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_N21_LT::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::pre_recv_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::delay_late_sender_bwc_cb));
        cbmanager->register_callback(
            PRE_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::pre_inv_sendcmp_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::delay_late_receiver_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::pre_delay_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::delay_barrier_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::pre_delay_n2n_bwc_cb));
        cbmanager->register_callback(
            DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::delay_n2n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::pre_delay_12n_bwc_cb));
        cbmanager->register_callback(
            DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::delay_12n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::pre_delay_n21_bwc_cb));
        cbmanager->register_callback(
            DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::delay_n21_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::pre_delay_omp_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_N21_LT::delay_omp_barrier_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_N21_LT::delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2490 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N21);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N21);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_N21_LT::delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2420 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N21);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N21);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_N21_LT::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2384 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_N21)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_N21_LT::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2345 "Delay.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mRemote->get_buffer(BUFFER_RCOST_N21)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_N21_LT::delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2525 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N21);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N21);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }

        // algorithm 1, line 28 for gamma_short
        map< uint32_t, ScaleMap >::const_iterator sWit = data->mSumScales.find(KEY_WAITTIME);
        if (sWit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sWit->second);
        }
    }


    void
    PatternDelay_N21_LT::delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2455 "Delay.MPI.pattern"

        // Set severities
        // algorithm 1, line 25
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N21);
        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                Callpath* const callpath = data->mDefs->get_cnode(it->first);

                m_severity[callpath] += it->second;

                UTILS_DLOG_LEVEL(SCALASCA_DEBUG_ROOT_CAUSE)
                    << get_name() << " @ " << *callpath
                    << ": " << it->second
                    << " [total: " << m_severity[callpath] << ']';
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        // algorithm 1, line 28 for gamma_long
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N21);
        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_N21_LT::delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2567 "Delay.MPI.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N21);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N21);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_N21_LT::pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2486 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N21, m_cost.getCosts(event)));
    }


    void
    PatternDelay_N21_LT::pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2416 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N21, m_cost.getCosts(event)));
    }


    void
    PatternDelay_N21_LT::pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2521 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N21, m_cost.getCosts(event)));
    }


    void
    PatternDelay_N21_LT::pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2451 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N21, m_cost.getCosts(event)));
    }


    void
    PatternDelay_N21_LT::pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2563 "Delay.MPI.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_N21, m_cost.getCosts(event)));
    }


    void
    PatternDelay_N21_LT::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2377 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_N21);
    }


    void
    PatternDelay_N21_LT::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 2338 "Delay.MPI.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mLocal->add_buffer(buffer, BUFFER_RCOST_N21);
    }
#endif    // _MPI


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_OmpBarrier
     *  @brief Short-term OpenMP Barrier Delay Costs metric.
     *
     *  Implements the "Short-term OpenMP Barrier Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_OmpBarrier
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_OMPBARRIER;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_OMPBARRIER_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "OMP short-term barrier delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_ompbarrier";
            }

            virtual string
            get_descr() const
            {
                return "Delays that directly cause wait states in OpenMP"
                       " barriers";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

            /// @}


        private:
            #line 102 "Delay.OMP.pattern"

            #ifdef OMPDELAYDEBUG
                std::ofstream m_dbg;
            #endif
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_OmpBarrier::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier::omp_barrier_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_OmpBarrier::omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 108 "Delay.OMP.pattern"

        const CollectiveInfo& ci(data->mCollinfo);

        data->mInputScales.clear();

        cbmanager.notify(PRE_DELAY_OMP_BARRIER, event, data);

        CalculateOmpBarrierDelay(ci.my, ci.latest, event, data);

        if (ci.my.mRank == ci.latest.mRank)
        {
            // Set severities
            map< uint32_t, TimeMapSum >::const_iterator dWit = data->mCollDelayInfo.find(KEY_WAITTIME);

            if (  (dWit != data->mCollDelayInfo.end())
               && (dWit->second.mSum > 0.0))
            {
                for (timemap_t::const_iterator it = dWit->second.mMap.begin();
                     it != dWit->second.mMap.end();
                     ++it)
                {
                    m_severity[data->mDefs->get_cnode(it->first)] += it->second;
                }
            }

            cbmanager.notify(DELAY_OMP_BARRIER, event, data);
        }
    }
#endif    // _OPENMP


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_OmpBarrier_LT
     *  @brief Long-term OpenMP Barrier Delay Costs metric.
     *
     *  Implements the "Long-term OpenMP Barrier Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_OmpBarrier_LT
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_OMPBARRIER_LONGTERM;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_OMPBARRIER_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "OMP long-term barrier delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_ompbarrier_longterm";
            }

            virtual string
            get_descr() const
            {
                return "Delays that indirectly cause wait states in OpenMP"
                       " barriers through wait-state propagation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_12n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            delay_n21_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            pre_recv_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        private:
            #line 163 "Delay.OMP.pattern"

            DelayCostMap m_cost;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_OmpBarrier_LT::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::pre_recv_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::delay_late_sender_bwc_cb));
        cbmanager->register_callback(
            PRE_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::pre_inv_sendcmp_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::delay_late_receiver_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::pre_delay_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::delay_barrier_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::pre_delay_n2n_bwc_cb));
        cbmanager->register_callback(
            DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::delay_n2n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::pre_delay_12n_bwc_cb));
        cbmanager->register_callback(
            DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::delay_12n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::pre_delay_n21_bwc_cb));
        cbmanager->register_callback(
            DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::delay_n21_bwc_cb));
        cbmanager->register_callback(
            DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpBarrier_LT::delay_omp_barrier_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_OmpBarrier_LT::delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 305 "Delay.OMP.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPBARRIER);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPBARRIER);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_OmpBarrier_LT::delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                     int                    user_event,
                                                     const Event&           event,
                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 249 "Delay.OMP.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPBARRIER);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPBARRIER);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_OmpBarrier_LT::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                           int                    user_event,
                                                           const Event&           event,
                                                           CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 213 "Delay.OMP.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_OMPBARRIER)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_OmpBarrier_LT::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 174 "Delay.OMP.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mRemote->get_buffer(BUFFER_RCOST_OMPBARRIER)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_OmpBarrier_LT::delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 333 "Delay.OMP.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPBARRIER);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPBARRIER);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_OmpBarrier_LT::delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 277 "Delay.OMP.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPBARRIER);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPBARRIER);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_OmpBarrier_LT::delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 357 "Delay.OMP.pattern"

        map< uint32_t, ScaleMap >::const_iterator it = data->mSumScales.find(KEY_WAITTIME);

        if (it != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, it->second);
        }
    }


    void
    PatternDelay_OmpBarrier_LT::pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                                     int                    user_event,
                                                     const Event&           event,
                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 301 "Delay.OMP.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_OMPBARRIER, m_cost.getCosts(event)));
    }


    void
    PatternDelay_OmpBarrier_LT::pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 245 "Delay.OMP.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_OMPBARRIER, m_cost.getCosts(event)));
    }


    void
    PatternDelay_OmpBarrier_LT::pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                                     int                    user_event,
                                                     const Event&           event,
                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 329 "Delay.OMP.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_OMPBARRIER, m_cost.getCosts(event)));
    }


    void
    PatternDelay_OmpBarrier_LT::pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                                     int                    user_event,
                                                     const Event&           event,
                                                     CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 273 "Delay.OMP.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_OMPBARRIER, m_cost.getCosts(event)));
    }


    void
    PatternDelay_OmpBarrier_LT::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                                       int                    user_event,
                                                       const Event&           event,
                                                       CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 206 "Delay.OMP.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_OMPBARRIER);
    }


    void
    PatternDelay_OmpBarrier_LT::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                                int                    user_event,
                                                const Event&           event,
                                                CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 167 "Delay.OMP.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mLocal->add_buffer(buffer, BUFFER_RCOST_OMPBARRIER);
    }
#endif    // _MPI


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_OmpIdle
     *  @brief Short-term OpenMP Thread Idleness Delay Costs metric.
     *
     *  Implements the "Short-term OpenMP Thread Idleness Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_OmpIdle
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_OMPIDLE;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_OMPIDLE_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "OMP short-term idleness delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_ompidle";
            }

            virtual string
            get_descr() const
            {
                return "Delays on the OpenMP master thread that leave OpenMP"
                       " worker threads idle";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            omp_mgmt_fork_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            prepare_bwc_cb(const CallbackManager& cbmanager,
                           int                    user_event,
                           const Event&           event,
                           CallbackData*          cdata);

            /// @}


        private:
            #line 410 "Delay.OMP.pattern"

            #ifdef OMPDELAYDEBUG
                std::ofstream m_dbg;
            #endif
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_OmpIdle::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            OMP_MGMT_FORK,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle::omp_mgmt_fork_bwc_cb));
        cbmanager->register_callback(
            PREPARE,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle::prepare_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_OmpIdle::omp_mgmt_fork_bwc_cb(const CallbackManager& cbmanager,
                                               int                    user_event,
                                               const Event&           event,
                                               CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 416 "Delay.OMP.pattern"

        #pragma omp master
        {
            // Shortcuts
            const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
            const SynchpointHandler* SpH = data->mSynchpointHandler;

            // Normally, the delay info mini-profile would be calculated
            // between the current event and the previous syncpoint, incl.
            // its completion time.  In this case, however, the previous
            // syncpoint stored (if it exists) is the LEAVE event of the
            // implicit barrier of the preceding parallel region, i.e., the
            // interval based on the syncpoint is too long.  Thus, search
            // for the respective THREAD_JOIN event starting from the
            // syncpoint, and use it to calculate the correct mini-profile.
            //
            // Note that the wait map subtracted from the mini-profile can
            // still be determined based on the syncpoint since there can
            // be no wait states between the LEAVE of the implicit barrier
            // and the THREAD_JOIN event of the parallel region.
            data->mPrevSp = SpH->findPreviousOmpSynchpoint(event);
            Event joinEvt = data->mPrevSp;
            while (joinEvt.is_valid() && !joinEvt->isOfType(THREAD_JOIN))
            {
                ++joinEvt;
            }
            data->mPropSpMap = SpH->getSynchpointsBetween(data->mPrevSp, event);
            data->mWaitInfo  = CalculateWaitMap(data->mPropSpMap);
            data->mDelayInfo = CalculateDiff(TmH->getRuntimes(joinEvt, event),
                                             data->mWaitInfo.mMap);

            // shortcuts
            const TimeMapSum& d(data->mDelayInfo);
            const TimeMapSum& w(data->mWaitInfo);

            // --- calculate delay costs

            double scale = 0.0;

            if ((d.mSum + w.mSum) > 0)
            {
                scale = (omp_get_num_threads() - 1);
            }

            // --- set severities

            if (d.mSum > 0)
            {
                for (timemap_t::const_iterator it = d.mMap.begin();
                     it != d.mMap.end();
                     ++it)
                {
                    m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
                }
            }

            data->mOmpIdleScale = scale;

            cbmanager.notify(DELAY_OMP_IDLE, event, data);
        }
    }


    void
    PatternDelay_OmpIdle::prepare_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 478 "Delay.OMP.pattern"

        #pragma omp master
        {
            // Shortcuts
            const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
            const SynchpointHandler* SpH = data->mSynchpointHandler;

            // Normally, the delay info mini-profile would be calculated
            // between the current event and the previous syncpoint, incl.
            // its completion time.  In this case, however, the previous
            // syncpoint stored (if it exists) is the LEAVE event of the
            // implicit barrier of the preceding parallel region, i.e., the
            // interval based on the syncpoint is too long.  Thus, search
            // for the respective THREAD_JOIN event starting from the
            // syncpoint, and use it to calculate the correct mini-profile.
            //
            // Note that the wait map subtracted from the mini-profile can
            // still be determined based on the syncpoint since there can
            // be no wait states between the LEAVE of the implicit barrier
            // and the THREAD_JOIN event of the parallel region.
            data->mPrevSp = SpH->findPreviousOmpSynchpoint(event);
            Event joinEvt = data->mPrevSp;
            while (joinEvt.is_valid() && !joinEvt->isOfType(THREAD_JOIN))
            {
                ++joinEvt;
            }
            data->mPropSpMap = SpH->getSynchpointsBetween(data->mPrevSp, event);
            data->mWaitInfo  = CalculateWaitMap(data->mPropSpMap);
            data->mDelayInfo = CalculateDiff(TmH->getRuntimes(joinEvt, event),
                                             data->mWaitInfo.mMap);

            // shortcuts
            const TimeMapSum& d(data->mDelayInfo);
            const TimeMapSum& w(data->mWaitInfo);

            // --- calculate delay costs

            double scale = 0.0;

            if ((d.mSum + w.mSum) > 0)
            {
                scale = (omp_get_num_threads() - 1);
            }

            // --- set severities

            if (d.mSum > 0)
            {
                for (timemap_t::const_iterator it = d.mMap.begin();
                     it != d.mMap.end();
                     ++it)
                {
                    m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
                }
            }

            data->mOmpIdleScale = scale;

            cbmanager.notify(DELAY_OMP_IDLE, event, data);
        }
    }
#endif    // _OPENMP


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternDelay_OmpIdle_LT
     *  @brief Long-term OpenMP Thread Idleness Delay Costs metric.
     *
     *  Implements the "Long-term OpenMP Thread Idleness Delay Costs" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternDelay_OmpIdle_LT
        : public Pattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_DELAY_OMPIDLE_LONGTERM;
            }

            virtual long
            get_parent() const
            {
                return PAT_DELAY_OMPIDLE_AGGREGATE;
            }

            virtual string
            get_name() const
            {
                return "OMP long-term idleness delay costs";
            }

            virtual string
            get_unique_name() const
            {
                return "delay_ompidle_longterm";
            }

            virtual string
            get_descr() const
            {
                return "Delays that indirectly leave OpenMP worker threads"
                       " idle";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_12n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            delay_n21_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            delay_omp_idle_bwc_cb(const CallbackManager& cbmanager,
                                  int                    user_event,
                                  const Event&           event,
                                  CallbackData*          cdata);

            void
            pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                 int                    user_event,
                                 const Event&           event,
                                 CallbackData*          cdata);

            void
            pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                         int                    user_event,
                                         const Event&           event,
                                         CallbackData*          cdata);

            void
            pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            pre_recv_bwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            /// @}


        private:
            #line 578 "Delay.OMP.pattern"

            DelayCostMap m_cost;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternDelay_OmpIdle_LT::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_RECV,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::pre_recv_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::delay_late_sender_bwc_cb));
        cbmanager->register_callback(
            PRE_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::pre_inv_sendcmp_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::delay_late_receiver_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::pre_delay_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::delay_barrier_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::pre_delay_n2n_bwc_cb));
        cbmanager->register_callback(
            DELAY_N2N,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::delay_n2n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::pre_delay_12n_bwc_cb));
        cbmanager->register_callback(
            DELAY_12N,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::delay_12n_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::pre_delay_n21_bwc_cb));
        cbmanager->register_callback(
            DELAY_N21,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::delay_n21_bwc_cb));
        cbmanager->register_callback(
            PRE_DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::pre_delay_omp_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_OMP_BARRIER,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::delay_omp_barrier_bwc_cb));
        cbmanager->register_callback(
            DELAY_OMP_IDLE,
            PEARL_create_callback(
                this,
                &PatternDelay_OmpIdle_LT::delay_omp_idle_bwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternDelay_OmpIdle_LT::delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 720 "Delay.OMP.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPIDLE);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPIDLE);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_OmpIdle_LT::delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 664 "Delay.OMP.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPIDLE);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPIDLE);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_OmpIdle_LT::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                        int                    user_event,
                                                        const Event&           event,
                                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 628 "Delay.OMP.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_OMPIDLE)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_OmpIdle_LT::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 589 "Delay.OMP.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!((d.mSum + w.mSum) > 0))
        {
            return;
        }

        // --- get remote cost & set severities

        double rcost = data->mRemote->get_buffer(BUFFER_RCOST_OMPIDLE)->get_double();
        double scale = rcost / (d.mSum + w.mSum);

        if (!(scale > 0))
        {
            return;
        }

        for (timemap_t::const_iterator it = d.mMap.begin();
             it != d.mMap.end();
             ++it)
        {
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;
        }

        // --- set propagating long-term costs

        m_cost.addCosts(data->mPropSpMap, scale);
    }


    void
    PatternDelay_OmpIdle_LT::delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 748 "Delay.OMP.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPIDLE);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPIDLE);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_OmpIdle_LT::delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                              int                    user_event,
                                              const Event&           event,
                                              CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 692 "Delay.OMP.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPIDLE);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPIDLE);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_OmpIdle_LT::delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 776 "Delay.OMP.pattern"

        // Set severities
        map< uint32_t, TimeMapSum >::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPIDLE);

        if (  (dCit != data->mCollDelayInfo.end())
           && (dCit->second.mSum > 0.0))
        {
            for (timemap_t::const_iterator it = dCit->second.mMap.begin();
                 it != dCit->second.mMap.end();
                 ++it)
            {
                m_severity[data->mDefs->get_cnode(it->first)] += it->second;
            }
        }

        // Update long-term costs for propagating wait states in synchronization interval
        map< uint32_t, ScaleMap >::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPIDLE);

        if (sCit != data->mSumScales.end())
        {
            m_cost.addCosts(data->mPropSpMap, sCit->second);
        }
    }


    void
    PatternDelay_OmpIdle_LT::delay_omp_idle_bwc_cb(const CallbackManager& cbmanager,
                                                   int                    user_event,
                                                   const Event&           event,
                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 800 "Delay.OMP.pattern"

        if (data->mWaitInfo.mSum > 0.0)
        {
            m_cost.addCosts(data->mPropSpMap, data->mOmpIdleScale);
        }
    }


    void
    PatternDelay_OmpIdle_LT::pre_delay_12n_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 716 "Delay.OMP.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_OMPIDLE, m_cost.getCosts(event)));
    }


    void
    PatternDelay_OmpIdle_LT::pre_delay_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                      int                    user_event,
                                                      const Event&           event,
                                                      CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 660 "Delay.OMP.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_OMPIDLE, m_cost.getCosts(event)));
    }


    void
    PatternDelay_OmpIdle_LT::pre_delay_n21_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 744 "Delay.OMP.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_OMPIDLE, m_cost.getCosts(event)));
    }


    void
    PatternDelay_OmpIdle_LT::pre_delay_n2n_bwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 688 "Delay.OMP.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_OMPIDLE, m_cost.getCosts(event)));
    }


    void
    PatternDelay_OmpIdle_LT::pre_delay_omp_barrier_bwc_cb(const CallbackManager& cbmanager,
                                                          int                    user_event,
                                                          const Event&           event,
                                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 772 "Delay.OMP.pattern"

        data->mInputScales.insert(make_pair(KEY_RCOST_OMPIDLE, m_cost.getCosts(event)));
    }


    void
    PatternDelay_OmpIdle_LT::pre_inv_sendcmp_bwc_cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 621 "Delay.OMP.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_OMPIDLE);
    }


    void
    PatternDelay_OmpIdle_LT::pre_recv_bwc_cb(const CallbackManager& cbmanager,
                                             int                    user_event,
                                             const Event&           event,
                                             CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 582 "Delay.OMP.pattern"

        Buffer* buffer = new Buffer(16);

        buffer->put_double(m_cost.getCosts(event));
        data->mLocal->add_buffer(buffer, BUFFER_RCOST_OMPIDLE);
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_Wait_Indirect_LS
     *  @brief Indirect MPI Late Sender Wait States metric.
     *
     *  Implements the "Indirect MPI Late Sender Wait States" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_Wait_Indirect_LS
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_WAIT_INDIRECT_LATESENDER;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_WAIT_INDIRECT;
            }

            virtual string
            get_name() const
            {
                return "Indirect MPI Late Sender wait states";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_wait_indirect_latesender";
            }

            virtual string
            get_descr() const
            {
                return "Indirect waiting time in MPI Late Sender wait states"
                       " that results from propagation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            post_recv_fwc_cb(const CallbackManager& cbmanager,
                             int                    user_event,
                             const Event&           event,
                             CallbackData*          cdata);

            void
            pre_send_fwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            rpl_resume_fwd__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

            /// @}


        private:
            #line 125 "Waitstates.MPI.pattern"

            typedef std::map< Event, double, EventKeyCompare > eventmap_t;

            eventmap_t m_pw;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_Wait_Indirect_LS::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            RPL_RESUME_FWD,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_Indirect_LS::rpl_resume_fwd__cb));

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_Indirect_LS::delay_late_sender_bwc_cb));

        // Register fwc callbacks
        it = cbmanagers.find("fwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_SEND,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_Indirect_LS::pre_send_fwc_cb));
        cbmanager->register_callback(
            POST_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_Indirect_LS::post_recv_fwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_Wait_Indirect_LS::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                          int                    user_event,
                                                          const Event&           event,
                                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 136 "Waitstates.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (w.mSum > 0)
        {
            double scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
            m_pw[event] += scale * w.mSum;
        }
    }


    void
    PatternMPI_Wait_Indirect_LS::post_recv_fwc_cb(const CallbackManager& cbmanager,
                                                  int                    user_event,
                                                  const Event&           event,
                                                  CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 157 "Waitstates.MPI.pattern"

        double pw = data->mRemote->get_buffer(BUFFER_PROPWAIT)->get_double();

        if (pw > 0.0)
        {
            m_severity[event.get_cnode()] += pw;
        }
    }


    void
    PatternMPI_Wait_Indirect_LS::pre_send_fwc_cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 149 "Waitstates.MPI.pattern"

        eventmap_t::iterator it = m_pw.find(event);

        Buffer* buf = new Buffer(16);
        buf->put_double(it == m_pw.end() ? 0.0 : it->second);

        data->mLocal->add_buffer(buf, BUFFER_PROPWAIT);
    }


    void
    PatternMPI_Wait_Indirect_LS::rpl_resume_fwd__cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata)
    {
        #line 131 "Waitstates.MPI.pattern"

        m_pw.clear();
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_Wait_Indirect_LR
     *  @brief Indirect MPI Late Receiver Wait States metric.
     *
     *  Implements the "Indirect MPI Late Receiver Wait States" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_Wait_Indirect_LR
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_WAIT_INDIRECT_LATERECEIVER;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_WAIT_INDIRECT;
            }

            virtual string
            get_name() const
            {
                return "Indirect MPI Late Receiver wait states";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_wait_indirect_latereceiver";
            }

            virtual string
            get_descr() const
            {
                return "Indirect waiting time in MPI Late Receiver wait"
                       " states that results from propagation";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            post_inv_sendcmp_fwc_cb(const CallbackManager& cbmanager,
                                    int                    user_event,
                                    const Event&           event,
                                    CallbackData*          cdata);

            void
            pre_inv_recvreq_fwc_cb(const CallbackManager& cbmanager,
                                   int                    user_event,
                                   const Event&           event,
                                   CallbackData*          cdata);

            void
            rpl_resume_fwd__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

            /// @}


        private:
            #line 183 "Waitstates.MPI.pattern"

            typedef std::map< Event, double, EventKeyCompare > eventmap_t;

            eventmap_t m_pw;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_Wait_Indirect_LR::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            RPL_RESUME_FWD,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_Indirect_LR::rpl_resume_fwd__cb));

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_Indirect_LR::delay_late_receiver_bwc_cb));

        // Register fwc callbacks
        it = cbmanagers.find("fwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            PRE_INV_RECVREQ,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_Indirect_LR::pre_inv_recvreq_fwc_cb));
        cbmanager->register_callback(
            POST_INV_SENDCMP,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_Indirect_LR::post_inv_sendcmp_fwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_Wait_Indirect_LR::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                            int                    user_event,
                                                            const Event&           event,
                                                            CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 194 "Waitstates.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (w.mSum > 0)
        {
            double scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
            m_pw[event] += scale * w.mSum;
        }
    }


    void
    PatternMPI_Wait_Indirect_LR::post_inv_sendcmp_fwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 215 "Waitstates.MPI.pattern"

        double pw = data->mInvRemote->get_buffer(BUFFER_PROPWAIT)->get_double();

        if (pw > 0.0)
        {
            m_severity[event.get_cnode()] += pw;
        }
    }


    void
    PatternMPI_Wait_Indirect_LR::pre_inv_recvreq_fwc_cb(const CallbackManager& cbmanager,
                                                        int                    user_event,
                                                        const Event&           event,
                                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 207 "Waitstates.MPI.pattern"

        eventmap_t::iterator it = m_pw.find(event);

        Buffer* buf = new Buffer(16);
        buf->put_double(it == m_pw.end() ? 0.0 : it->second);

        data->mInvLocal->add_buffer(buf, BUFFER_PROPWAIT);
    }


    void
    PatternMPI_Wait_Indirect_LR::rpl_resume_fwd__cb(const CallbackManager& cbmanager,
                                                    int                    user_event,
                                                    const Event&           event,
                                                    CallbackData*          cdata)
    {
        #line 189 "Waitstates.MPI.pattern"

        m_pw.clear();
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class PatternMPI_Wait_PT
     *  @brief Propagating MPI Point-to-point Wait States metric.
     *
     *  Implements the "Propagating MPI Point-to-point Wait States" metric.
     */
    /*---------------------------------------------------------------------*/

    class PatternMPI_Wait_PT
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_WAIT_PROPAGATING;
            }

            virtual long
            get_parent() const
            {
                return PAT_WAITSTATES_PROPAGATING_VS_TERMINAL;
            }

            virtual string
            get_name() const
            {
                return "Propagating MPI point-to-point wait states";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_wait_propagating";
            }

            virtual string
            get_descr() const
            {
                return "Aggregate amount of MPI point-to-point waiting time"
                       " that propagates further";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            virtual bool
            is_hidden() const
            {
                return true;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                       int                    user_event,
                                       const Event&           event,
                                       CallbackData*          cdata);

            void
            delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            group_recv_fwc_cb(const CallbackManager& cbmanager,
                              int                    user_event,
                              const Event&           event,
                              CallbackData*          cdata);

            void
            mpi_send_complete_fwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            void
            mpi_send_fwc_cb(const CallbackManager& cbmanager,
                            int                    user_event,
                            const Event&           event,
                            CallbackData*          cdata);

            void
            rpl_resume_fwd__cb(const CallbackManager& cbmanager,
                               int                    user_event,
                               const Event&           event,
                               CallbackData*          cdata);

            /// @}


        private:
            #line 381 "Waitstates.MPI.pattern"

            typedef std::map< Event, double, EventKeyCompare > eventmap_t;

            eventmap_t m_pw;
    };


    // --- Registering callbacks --------------------------------------------

    void
    PatternMPI_Wait_PT::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register main callbacks
        it = cbmanagers.find("");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            RPL_RESUME_FWD,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_PT::rpl_resume_fwd__cb));

        // Register bwc callbacks
        it = cbmanagers.find("bwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            DELAY_LATE_SENDER,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_PT::delay_late_sender_bwc_cb));
        cbmanager->register_callback(
            DELAY_LATE_RECEIVER,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_PT::delay_late_receiver_bwc_cb));

        // Register fwc callbacks
        it = cbmanagers.find("fwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            GROUP_RECV,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_PT::group_recv_fwc_cb));
        cbmanager->register_callback(
            MPI_SEND,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_PT::mpi_send_fwc_cb));
        cbmanager->register_callback(
            MPI_SEND_COMPLETE,
            PEARL_create_callback(
                this,
                &PatternMPI_Wait_PT::mpi_send_complete_fwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    PatternMPI_Wait_PT::delay_late_receiver_bwc_cb(const CallbackManager& cbmanager,
                                                   int                    user_event,
                                                   const Event&           event,
                                                   CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 430 "Waitstates.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!(w.mSum > 0))
        {
            return;
        }

        double scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);

        if (!(scale > 0))
        {
            return;
        }

        // --- update maximum propagating wait time for wait states in current sync interval

        for (SynchpointInfoMap::const_iterator it = data->mPropSpMap.begin();
             it != data->mPropSpMap.end();
             ++it)
        {
            if (it->second.waitTime > 0)
            {
                eventmap_t::iterator pwit = m_pw.find(it->first);

                if (pwit == m_pw.end())
                {
                    m_pw.insert(std::make_pair(it->first, scale * it->second.waitTime));
                }
                else
                {
                    pwit->second = std::max(scale * it->second.waitTime, pwit->second);
                }
            }
        }
    }


    void
    PatternMPI_Wait_PT::delay_late_sender_bwc_cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 392 "Waitstates.MPI.pattern"

        // Shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        if (!(w.mSum > 0))
        {
            return;
        }

        double scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);

        if (!(scale > 0))
        {
            return;
        }

        // --- update maximum propagating wait time for wait states in current sync interval

        for (SynchpointInfoMap::const_iterator it = data->mPropSpMap.begin();
             it != data->mPropSpMap.end();
             ++it)
        {
            if (it->second.waitTime > 0)
            {
                eventmap_t::iterator pwit = m_pw.find(it->first);

                if (pwit == m_pw.end())
                {
                    m_pw.insert(std::make_pair(it->first, scale * it->second.waitTime));
                }
                else
                {
                    pwit->second = std::max(scale * it->second.waitTime, pwit->second);
                }
            }
        }
    }


    void
    PatternMPI_Wait_PT::group_recv_fwc_cb(const CallbackManager& cbmanager,
                                          int                    user_event,
                                          const Event&           event,
                                          CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 757 "Waitstates.MPI.pattern"

        eventmap_t::iterator pwmit = m_pw.find(event);

        if (pwmit != m_pw.end())
        {
            data->mIdle = pwmit->second;
            cbmanager.notify(WAITSTATE_PROP_LS, event, data);
        }
    }


    void
    PatternMPI_Wait_PT::mpi_send_complete_fwc_cb(const CallbackManager& cbmanager,
                                                 int                    user_event,
                                                 const Event&           event,
                                                 CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 775 "Waitstates.MPI.pattern"

        eventmap_t::iterator pwmit = m_pw.find(event);

        if (pwmit != m_pw.end())
        {
            data->mIdle = pwmit->second;
            cbmanager.notify(WAITSTATE_PROP_LR, event, data);
        }
    }


    void
    PatternMPI_Wait_PT::mpi_send_fwc_cb(const CallbackManager& cbmanager,
                                        int                    user_event,
                                        const Event&           event,
                                        CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 766 "Waitstates.MPI.pattern"

        eventmap_t::iterator pwmit = m_pw.find(event);

        if (pwmit != m_pw.end())
        {
            data->mIdle = pwmit->second;
            cbmanager.notify(WAITSTATE_PROP_LR, event, data);
        }
    }


    void
    PatternMPI_Wait_PT::rpl_resume_fwd__cb(const CallbackManager& cbmanager,
                                           int                    user_event,
                                           const Event&           event,
                                           CallbackData*          cdata)
    {
        #line 387 "Waitstates.MPI.pattern"

        m_pw.clear();
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class Pattern_Wait_PropagatingLS
     *  @brief Propagating MPI Late Sender Wait States metric.
     *
     *  Implements the "Propagating MPI Late Sender Wait States" metric.
     */
    /*---------------------------------------------------------------------*/

    class Pattern_Wait_PropagatingLS
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_WAIT_PROPAGATING_LS;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_WAIT_PROPAGATING;
            }

            virtual string
            get_name() const
            {
                return "Propagating MPI Late Sender wait states";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_wait_propagating_ls";
            }

            virtual string
            get_descr() const
            {
                return "MPI Late Sender wait states which cause further"
                       " waiting time on other processes";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            waitstate_prop_ls_fwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    Pattern_Wait_PropagatingLS::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register fwc callbacks
        it = cbmanagers.find("fwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            WAITSTATE_PROP_LS,
            PEARL_create_callback(
                this,
                &Pattern_Wait_PropagatingLS::waitstate_prop_ls_fwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    Pattern_Wait_PropagatingLS::waitstate_prop_ls_fwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 824 "Waitstates.MPI.pattern"

        m_severity[event.get_cnode()] += data->mIdle;
    }
#endif    // _MPI


#if defined(_MPI)
    /*---------------------------------------------------------------------*/
    /**
     *  @class Pattern_Wait_PropagatingLR
     *  @brief Propagating MPI Late Receiver Wait States metric.
     *
     *  Implements the "Propagating MPI Late Receiver Wait States" metric.
     */
    /*---------------------------------------------------------------------*/

    class Pattern_Wait_PropagatingLR
        : public MpiPattern
    {
        public:
            /// @name Registering callbacks
            /// @{

            virtual void
            reg_cb(CallbackManagerMap& cbmanagers);

            /// @}
            /// @name Retrieving pattern information
            /// @{

            virtual long
            get_id() const
            {
                return PAT_MPI_WAIT_PROPAGATING_LR;
            }

            virtual long
            get_parent() const
            {
                return PAT_MPI_WAIT_PROPAGATING;
            }

            virtual string
            get_name() const
            {
                return "Propagating MPI Late Receiver wait states";
            }

            virtual string
            get_unique_name() const
            {
                return "mpi_wait_propagating_lr";
            }

            virtual string
            get_descr() const
            {
                return "MPI Late Receiver wait states which cause further"
                       " waiting time on other processes";
            }

            virtual string
            get_unit() const
            {
                return "sec";
            }

            virtual CubeMetricType
            get_mode() const
            {
                return CUBE_METRIC_EXCLUSIVE;
            }

            /// @}
            /// @name Callback methods
            /// @{

            void
            waitstate_prop_lr_fwc_cb(const CallbackManager& cbmanager,
                                     int                    user_event,
                                     const Event&           event,
                                     CallbackData*          cdata);

            /// @}
    };


    // --- Registering callbacks --------------------------------------------

    void
    Pattern_Wait_PropagatingLR::reg_cb(CallbackManagerMap& cbmanagers)
    {
        init();

        CallbackManagerMap::iterator    it;
        CallbackManagerMap::mapped_type cbmanager;

        // Register fwc callbacks
        it = cbmanagers.find("fwc");
        assert(it != cbmanagers.end());

        cbmanager = it->second;
        cbmanager->register_callback(
            WAITSTATE_PROP_LR,
            PEARL_create_callback(
                this,
                &Pattern_Wait_PropagatingLR::waitstate_prop_lr_fwc_cb));
    }


    // --- Callback methods -------------------------------------------------

    void
    Pattern_Wait_PropagatingLR::waitstate_prop_lr_fwc_cb(const CallbackManager& cbmanager,
                                                         int                    user_event,
                                                         const Event&           event,
                                                         CallbackData*          cdata)
    {
        CbData* data = static_cast< CbData* >(cdata);

        #line 846 "Waitstates.MPI.pattern"

        m_severity[event.get_cnode()] += data->mIdle;
    }
#endif    // _MPI


// --- Pattern registration -------------------------------------------------

void
scout::create_patterns(AnalyzeTask* analyzer)
{
    analyzer->addPattern(new PatternTime());
    analyzer->addPattern(new PatternVisits());
    #if defined(_MPI)
        if (enableAsynchronous)
        {
            analyzer->addPattern(new PatternAsync());
        }
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_InitCompletion());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_FinalizeWait());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_SyncCollective());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_BarrierWait());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_BarrierCompletion());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_P2P());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_LateSender());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_LateSenderWO());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_LswoDifferent());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_LswoSame());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_LateReceiver());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_Collective());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_EarlyReduce());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_EarlyScan());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_LateBroadcast());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_WaitNxN());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_NxNCompletion());
    #endif    // _MPI
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternOMP_Mgmt());
    #endif    // _OPENMP
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternOMP_Mgmt_Fork());
    #endif    // _OPENMP
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternOMP_Sync_Barrier());
    #endif    // _OPENMP
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternOMP_Sync_Ebarrier_Wait());
    #endif    // _OPENMP
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternOMP_Sync_Ibarrier_Wait());
    #endif    // _OPENMP
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternOMP_Lock_Contention_Critical());
    #endif    // _OPENMP
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternOMP_Lock_Contention_API());
    #endif    // _OPENMP
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternPthread_Lock_Contention_Mutex_Lock());
    #endif    // _OPENMP
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternPthread_Lock_Contention_Conditional());
    #endif    // _OPENMP
    #if defined(_OPENMP)
        analyzer->addPattern(new PatternThread_LockContention());
    #endif    // _OPENMP
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_SyncsSend());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_SyncsRecv());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_SyncsColl());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_CommsSend());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_CommsRecv());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_CommsExch());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_CommsSrc());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_CommsDst());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_BytesSent());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_BytesRcvd());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_BytesCout());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_BytesCin());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_BytesPut());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_BytesGet());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_SLR_Count());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_SLS_Count());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_SLSWO_Count());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_CLR_Count());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_CLS_Count());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_CLSWO_Count());
    #endif    // _MPI
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMPI_RmaWaitAtCreate());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMPI_RmaWaitAtFree());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMPI_RmaSync());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMPI_RmaSyncLatePost());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMPI_RmaEarlyWait());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMPI_RmaLateComplete());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMPI_RmaWaitAtFence());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMPI_RmaEarlyFence());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMpi_RmaSyncLockContention());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMpi_RmaSyncWaitForProgress());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMPI_RmaCommLatePost());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMpi_RmaCommLockContention());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI) && defined(HAS_MPI2_1SIDED)
        analyzer->addPattern(new PatternMpi_RmaCommWaitForProgress());
    #endif    // _MPI && HAS_MPI2_1SIDED
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_RmaPairsyncCount());
    #endif    // _MPI
    #if defined(_MPI)
        analyzer->addPattern(new PatternMPI_RmaPairsyncUnneededCount());
    #endif    // _MPI
    if (enableStatistics)
    {
        analyzer->addPattern(new PatternStatistics());
    }
    if (enableCriticalPath)
    {
        analyzer->addPattern(new PatternCriticalPath());
    }
    if (enableCriticalPath)
    {
        analyzer->addPattern(new PatternCriticalPathActivities());
    }
    if (enableCriticalPath)
    {
        analyzer->addPattern(new PatternCriticalImbalanceImpact());
    }
    if (enableCriticalPath)
    {
        analyzer->addPattern(new PatternInterPartitionImbalance());
    }
    if (enableCriticalPath)
    {
        analyzer->addPattern(new PatternNonCriticalPathActivities());
    }
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_LateSender());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_LateSender_LT());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_LateReceiver());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_LateReceiver_LT());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_Barrier());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_Barrier_LT());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_NxN());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_NxN_LT());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_12N());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_12N_LT());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_N21());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_N21_LT());
        }
    #endif    // _MPI
    #if defined(_OPENMP)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_OmpBarrier());
        }
    #endif    // _OPENMP
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_OmpBarrier_LT());
        }
    #endif    // _MPI
    #if defined(_OPENMP)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_OmpIdle());
        }
    #endif    // _OPENMP
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternDelay_OmpIdle_LT());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternMPI_Wait_Indirect_LS());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternMPI_Wait_Indirect_LR());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new PatternMPI_Wait_PT());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new Pattern_Wait_PropagatingLS());
        }
    #endif    // _MPI
    #if defined(_MPI)
        if (enableDelayAnalysis)
        {
            analyzer->addPattern(new Pattern_Wait_PropagatingLR());
        }
    #endif    // _MPI
}
