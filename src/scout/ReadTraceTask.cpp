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

#include "ReadTraceTask.h"

#include <cstddef>

#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/TraceArchive.h>

#include "CheckedTask.h"
#include "TaskData.h"
#include "TimedPhase.h"

#if defined(_MPI)
    #include <mpi.h>
#endif    // _MPI
#if defined(_OPENMP)
    #include <omp.h>
#endif    // _OPENMP

using namespace pearl;
using namespace scout;


// --- Local helper macros --------------------------------------------------

#define SUBTASK(task, message)                                       \
    add_task(TimedPhase::make_timed(CheckedTask::make_checked(task), \
                                    message, 1, true))


// --- Helper classes -------------------------------------------------------

namespace
{
/*-------------------------------------------------------------------------*/
/**
 *  @class OpenTraceContainer
 *  @brief Task opening the trace container.
 *
 *  The OpenTraceContainer class implements a sub-task of ReadTraceTask which
 *  opens the event trace container (i.e., the local definition and event
 *  trace container files).
 **/
/*-------------------------------------------------------------------------*/

class OpenTraceContainer
    : public pearl::Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        OpenTraceContainer(const TaskDataShared& sharedData);

        /// @}
        /// @name Execution control
        /// @{

        virtual bool
        execute();

        /// @}


    private:
        /// Shared task data object
        const TaskDataShared& mSharedData;
};


/*-------------------------------------------------------------------------*/
/**
 *  @class ReadIdMappings
 *  @brief Task reading the identifier mapping data.
 *
 *  The ReadIdMappings class implememts a sub-task of ReadTraceTask which
 *  reads the local-to-global definition identifier mapping data.
 **/
/*-------------------------------------------------------------------------*/

class ReadIdMappings
    : public pearl::Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        ReadIdMappings(const TaskDataShared& sharedData,
                       TaskDataPrivate&      privateData);

        /// @}
        /// @name Execution control
        /// @{

        virtual bool
        execute();

        /// @}


    private:
        /// Shared task data object
        const TaskDataShared& mSharedData;

        /// Private task data object
        TaskDataPrivate& mPrivateData;
};


/*-------------------------------------------------------------------------*/
/**
 *  @class ReadTraceData
 *  @brief Task used to read the per-location trace data.
 *
 *  The ReadTraceData class is used to read the entire trace data which is
 *  stored per location into memory.
 **/
/*-------------------------------------------------------------------------*/

class ReadTraceData
    : public pearl::Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        ReadTraceData(const TaskDataShared& sharedData,
                      TaskDataPrivate&      privateData);

        /// @}
        /// @name Execution control
        /// @{

        virtual bool
        execute();

        /// @}


    private:
        /// Shared task data object
        const TaskDataShared& mSharedData;

        /// Private task data object
        TaskDataPrivate& mPrivateData;
};


/*-------------------------------------------------------------------------*/
/**
 *  @class CloseTraceContainer
 *  @brief Task closing the trace container.
 *
 *  The CloseTraceContainer class implements a sub-task of ReadTraceTask which
 *  closes the event trace container (i.e., the local definition and event
 *  trace container files).
 **/
/*-------------------------------------------------------------------------*/

class CloseTraceContainer
    : public pearl::Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        CloseTraceContainer(const TaskDataShared& sharedData);

        /// @}
        /// @name Execution control
        /// @{

        virtual bool
        execute();

        /// @}


    private:
        /// Shared task data object
        const TaskDataShared& mSharedData;
};


#if defined(_OPENMP)
    /*---------------------------------------------------------------------*/
    /**
     *  @class CheckTraceData
     *  @brief Task verifying some consistency requirements for OpenMP.
     *
     *  This task verifies whether some consietency requirements for OpenMP
     *  traces (no varying number of threads, MPI only on master thread, etc.)
     *  are fulfilled.
     **/
    /*---------------------------------------------------------------------*/

    class CheckTraceData
        : public pearl::Task
    {
        public:
            /// @name Constructors & destructor
            /// @{

            CheckTraceData(const TaskDataPrivate& privateData);

            /// @}
            /// @name Execution control
            /// @{

            virtual bool
            execute();

            /// @}


        private:
            /// Private task data object
            const TaskDataPrivate& mPrivateData;
    };
#endif // _OPENMP
}      // unnamed namespace


// --------------------------------------------------------------------------
//
//  class ReadTraceTask
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new trace reader task instance.
 *
 *  This constructor creates a new trace data reader task with the given
 *  parameters.
 *
 *  @param  sharedData   Shared task data object
 *  @param  privateData  Private task data object
 **/
ReadTraceTask::ReadTraceTask(const TaskDataShared& sharedData,
                             TaskDataPrivate&      privateData)
    : CompoundTask()
{
    SUBTASK(new OpenTraceContainer(sharedData),
            "\n"
            "  Opening trace container        ");
    SUBTASK(new ReadIdMappings(sharedData, privateData),
            "  Reading ID maps (local defs)   ");
    SUBTASK(new ReadTraceData(sharedData, privateData),
            "  Reading trace data             ");
    SUBTASK(new CloseTraceContainer(sharedData),
            "  Closing trace container        ");
    #if defined(_OPENMP)
        SUBTASK(new CheckTraceData(privateData),
                "  Checking trace properties      ");
    #endif    // _OPENMP
}


// --------------------------------------------------------------------------
//
//  class OpenTraceContainer
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new OpenTraceContainer task instance.
 *
 *  This constructor creates a new task instance for opening the event
 *  trace container.
 *
 *  @param  sharedData  Shared task data object
 **/
OpenTraceContainer::OpenTraceContainer(const TaskDataShared& sharedData)
    : Task(),
      mSharedData(sharedData)
{
}


// --- Execution control ----------------------------------------------------

/**
 *  @brief Executes the task.
 *
 *  Opens the event trace container.
 *
 *  @return Returns true if successful, false otherwise.
 *  @throw  pearl::FatalError  if the trace data contains inconsistent data
 **/
bool
OpenTraceContainer::execute()
{
    // Determine process rank
    int rank = 0;
    #if defined(_MPI)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    #endif    // _MPI

    // Determine location group
    const GlobalDefs&    defs     = *mSharedData.mDefinitions;
    const LocationGroup& locGroup = defs.getLocationGroup(rank);

    // Open event trace container
    #pragma omp master
    {
        mSharedData.mArchive->openTraceContainer(locGroup);
    }

    return true;
}


// --------------------------------------------------------------------------
//
//  class ReadIdMappings
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new ReadIdMappings task instance.
 *
 *  This constructor creates a new task instance for reading the
 *  local-to-global definition identifier mappings.
 *
 *  @param  sharedData   Shared task data object
 *  @param  privateData  Private task data object
 **/
ReadIdMappings::ReadIdMappings(const TaskDataShared& sharedData,
                               TaskDataPrivate&      privateData)
    : Task(),
      mSharedData(sharedData),
      mPrivateData(privateData)
{
}


// --- Execution control ----------------------------------------------------

/**
 *  @brief Executes the task.
 *
 *  Reads the local-to-global definition identifier mappings.
 *
 *  @return Returns true if successful, false otherwise.
 *  @throw  pearl::FatalError  if the mapping data contains inconsistent data
 **/
bool
ReadIdMappings::execute()
{
    // Determine thread ID
    int tid = 0;
    #if defined(_OPENMP)
        tid = omp_get_thread_num();
    #endif    // _OPENMP

    // Determine process rank
    int rank = 0;
    #if defined(_MPI)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    #endif    // _MPI

    // Determine location
    const GlobalDefs&    defs     = *mSharedData.mDefinitions;
    const LocationGroup& process  = defs.getLocationGroup(rank);
    const Location&      location = process.getLocation(tid);

    // Read identifier mapping data
    mPrivateData.mIdMaps = mSharedData.mArchive->getIdMaps(location);

    return (mPrivateData.mIdMaps != NULL);
}


// --------------------------------------------------------------------------
//
//  class ReadTraceData
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new ReadTraceData task instance.
 *
 *  This constructor creates a new task instance for reading the per-location
 *  event trace data into memory.
 *
 *  @param  sharedData   Shared task data object
 *  @param  privateData  Private task data object
 **/
ReadTraceData::ReadTraceData(const TaskDataShared& sharedData,
                             TaskDataPrivate&      privateData)
    : Task(),
      mSharedData(sharedData),
      mPrivateData(privateData)
{
}


// --- Execution control ----------------------------------------------------

/**
 *  @brief Executes the task.
 *
 *  Reads the trace data associated to the executing process/thread into
 *  memory.
 *
 *  @return Returns true if successful, false otherwise.
 *  @throw  pearl::FatalError  if the trace data contains inconsistent data
 **/
bool
ReadTraceData::execute()
{
    // Determine thread ID
    int tid = 0;
    #if defined(_OPENMP)
        tid = omp_get_thread_num();
    #endif    // _OPENMP

    // Determine process rank
    int rank = 0;
    #if defined(_MPI)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    #endif    // _MPI

    // Determine location
    const GlobalDefs&    defs     = *mSharedData.mDefinitions;
    const LocationGroup& process  = defs.getLocationGroup(rank);
    const Location&      location = process.getLocation(tid);

    // Read trace data
    mPrivateData.mTrace = mSharedData.mArchive->getTrace(defs,
                                                         location,
                                                         mPrivateData.mIdMaps);

    return (mPrivateData.mTrace != NULL);
}


// --------------------------------------------------------------------------
//
//  class CloseTraceContainer
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new CloseTraceContainer task instance.
 *
 *  This constructor creates a new task instance for closing the event
 *  trace container.
 *
 *  @param  sharedData  Shared task data object
 **/
CloseTraceContainer::CloseTraceContainer(const TaskDataShared& sharedData)
    : Task(),
      mSharedData(sharedData)
{
}


// --- Execution control ----------------------------------------------------

/**
 *  @brief Executes the task.
 *
 *  Closes the event trace container.
 *
 *  @return Returns true if successful, false otherwise.
 *  @throw  pearl::FatalError  if the trace data contains inconsistent data
 **/
bool
CloseTraceContainer::execute()
{
    #pragma omp master
    {
        mSharedData.mArchive->closeTraceContainer();
    }

    return true;
}


// --------------------------------------------------------------------------
//
//  class CheckTraceData
//
// --------------------------------------------------------------------------

#if defined(_OPENMP)
    // --- Constructors & destructor ----------------------------------------

    /**
     *  @brief Constructor. Creates a new CheckTraceData task instance.
     *
     *  This constructor creates a new task instance for verifying some
     *  consistency requirements for OpenMP.
     *
     *  @param  privateData  Private task data object
     **/
    CheckTraceData::CheckTraceData(const TaskDataPrivate& privateData)
        : Task(),
          mPrivateData(privateData)
    {
    }


    // --- Execution control ------------------------------------------------

    /**
     *  @brief Executes the task.
     *
     *  Verifies whether some consistency requirements for OpenMP (no varying
     *  number of threads, MPI only on master thread, etc.) are fulfilled.
     *
     *  @return Always true
     *  @throw  pearl::FatalError  if the OpenMP consistency requirements are not
     *                             fulfilled
     **/
    bool
    CheckTraceData::execute()
    {
        // FIXME: This useless extra block is needed to make some broken versions
        //        of the Intel 11 compilers happy... :-(
        {
            // These variables are implicitly shared
            static uint32_t num_parallel = 0;
            static bool     is_ok        = true;

            const LocalTrace& trace = *mPrivateData.mTrace;

            // --- Ensure equal number of OpenMP parallel regions per thread ---
            // Initialize number of OpenMP parallel regions by master thread
            #pragma omp master
            {
                num_parallel = trace.num_omp_parallel();
                is_ok        = true;
            }

            // Compare parallel region count on each thread and update status
            #pragma omp barrier
            #pragma omp critical
            {
                is_ok = is_ok && (trace.num_omp_parallel() == num_parallel);
            }
            #pragma omp barrier

            // Check status and abort if numbers don't match
            if (!is_ok)
            {
                throw FatalError("Varying numbers of threads in OpenMP parallel regions "
                                 "not supported!");
            }

            // --- Ensure that MPI calls only occur on master thread ---
            // Compare MPI region count on each thread and update status
            #pragma omp barrier
            #pragma omp critical
            {
                if (omp_get_thread_num() != 0)
                {
                    is_ok = is_ok && (trace.num_mpi_regions() == 0);
                }
            }
            #pragma omp barrier

            // Check status and abort if MPI calls found on non-master thread
            if (!is_ok)
            {
                throw FatalError("MPI calls on threads other than the master thread "
                                 "not supported!");
            }
        }

        return true;
    }
#endif    // _OPENMP
