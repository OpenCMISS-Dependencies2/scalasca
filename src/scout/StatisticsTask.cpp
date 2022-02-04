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

#include "StatisticsTask.h"

#include <pearl/Callback.h>
#include <pearl/CallbackManager.h>
#include <pearl/LocalTrace.h>

#include "Logging.h"
#include "TaskData.h"
#include "epk_memory.h"
#include "user_events.h"

#if defined(_MPI)
    #include <mpi.h>

    #include "SynchronizeTask.h"
#endif    // _MPI

using namespace pearl;
using namespace scout;


// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new statistics task.
 *
 *  This constructor creates a new statistics task instance using the given
 *  parameters.
 *
 *  @param  privateData  Private task data object
 *  @param  synctask     Pointer to timestamp synchronization task to be used,
 *                       NULL otherwise
 **/
StatisticsTask::StatisticsTask(const TaskDataPrivate& privateData,
                               SynchronizeTask*       synctask)
    : Task(),
      mPrivateData(privateData),
      mSyncTask(synctask),
      mCCV_p2p(0),
      mCCV_coll(0)
{
}


// --- Callback registration ------------------------------------------------

/**
 *  @brief Registers event counting callbacks.
 *
 *  This method registers the callback methods for counting clock-condition
 *  violations with the given callback manager @a cbmgr.
 *
 *  @param cbmanagers
 *      Map of CallbackManager objects with which the callbacks should be
 *      registered
 **/
void
StatisticsTask::registerAnalysisCallbacks(const CallbackManagerMap& cbmanagers)
{
    CallbackManagerMap::const_iterator cbmgrIt = cbmanagers.find("");

    // Register clock-condition violation callbacks
    (cbmgrIt->second)->register_callback(CCV_P2P,
                                         PEARL_create_callback(this,
                                                               &StatisticsTask::ccv_p2p_cb));
    (cbmgrIt->second)->register_callback(CCV_COLL,
                                         PEARL_create_callback(this,
                                                               &StatisticsTask::ccv_coll_cb));
}


// --- Executing the task ---------------------------------------------------

/**
 *  @brief Executes the statistics task.
 *
 *  This operation collates the various statistics across all participating
 *  processes and/or threads (depending on the parallel programming paradigm
 *  being used) and prints them to standard output.
 *
 *  @return Always @em true
 **/
bool
StatisticsTask::execute()
{
    // These variables are implicitly shared!
    static double num_events;
    static double ccv_p2p;
    static double ccv_coll;

    #pragma omp master
    {
        // Determine memory usage
        int heap_usage = epk_memusage();
        int max_heap   = 0;

        #if defined(_MPI)
            MPI_Reduce(&heap_usage, &max_heap, 1, MPI_INT,
                       MPI_MAX, 0, MPI_COMM_WORLD);
        #else    // !_MPI
            max_heap = heap_usage;
        #endif    // !_MPI
        LogMsg(0, "\nMax. memory usage         : %4.3fMB\n", max_heap / 1024.0);

        num_events = 0;
        ccv_p2p    = 0;
        ccv_coll   = 0;
    }
    #pragma omp barrier

    // Calculate event statistics
    #pragma omp critical
    {
        num_events += mPrivateData.mTrace->size();
    }
    #pragma omp barrier
    #pragma omp master
    {
        double min_events = num_events;
        double max_events = num_events;
        double sum_events = num_events;
        int    num_procs  = 1;

        #if defined(_MPI)
            MPI_Reduce(&num_events, &min_events, 1, MPI_DOUBLE,
                       MPI_MIN, 0, MPI_COMM_WORLD);
            MPI_Reduce(&num_events, &max_events, 1, MPI_DOUBLE,
                       MPI_MAX, 0, MPI_COMM_WORLD);
            MPI_Reduce(&num_events, &sum_events, 1, MPI_DOUBLE,
                       MPI_SUM, 0, MPI_COMM_WORLD);

            MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
        #endif    // _MPI

        LogMsg(1, "\nTotal number of events    : %12.0f\n"
                  "         Trace minimum    : %12.0f\n"
                  "         Trace maximum    : %12.0f\n"
                  "         Trace average    : %12.0f\n",
               sum_events, min_events, max_events, (sum_events / num_procs));
    }

    // Calculate clock-condition violation statistics
    #pragma omp critical
    {
        ccv_p2p  += mCCV_p2p;
        ccv_coll += mCCV_coll;
    }
    #pragma omp barrier
    #pragma omp master
    {
        double sum_ccv_p2p  = ccv_p2p;
        double sum_ccv_coll = ccv_coll;

        #if defined(_MPI)
            MPI_Reduce(&ccv_p2p, &sum_ccv_p2p, 1, MPI_DOUBLE,
                       MPI_SUM, 0, MPI_COMM_WORLD);
            MPI_Reduce(&ccv_coll, &sum_ccv_coll, 1, MPI_DOUBLE,
                       MPI_SUM, 0, MPI_COMM_WORLD);
        #endif    // _MPI

        double sum_ccv_total = sum_ccv_p2p + sum_ccv_coll;

        if (sum_ccv_total > 0)
        {
            LogMsg(0, "*** WARNING ***\n"
                      "%.0f clock condition violations detected:\n"
                      "   Point-to-point: %8.0f\n"
                      "   Collective    : %8.0f\n"
                      "This usually leads to inconsistent analysis results.\n\n",
                   sum_ccv_total, sum_ccv_p2p, sum_ccv_coll);
            if (!mSyncTask)
            {
                LogMsg(0, "Try running the analyzer using the '--time-correct' command-line\n"
                          "option to apply a correction algorithm.\n\n");
            }
        }
    }

    #if defined(_MPI)
        if (mSyncTask)
        {
            mSyncTask->print_statistics();
        }
    #endif    // _MPI

    return true;
}


// --- Callback methods (private) -------------------------------------------

/**
 *  @brief Increments the number of point-to-point clock-condition violaitions.
 *
 *  This callback is triggered by the trace-analysis replay performed by the
 *  pattern detection task whenever a point-to-point clock-condition violation
 *  is found (i.e., the receive ends before the corresponding send operation
 *  started).
 **/
void
StatisticsTask::ccv_p2p_cb(const CallbackManager&,
                           int,
                           const Event&,
                           CallbackData*)
{
    ++mCCV_p2p;
}


/**
 *  @brief Increments the number of collective clock-condition violaitions.
 *
 *  This callback is triggered by the trace-analysis replay performed by the
 *  pattern detection task whenever a collective clock-condition violation
 *  is found.
 **/
void
StatisticsTask::ccv_coll_cb(const CallbackManager&,
                            int,
                            const Event&,
                            CallbackData*)
{
    ++mCCV_coll;
}
