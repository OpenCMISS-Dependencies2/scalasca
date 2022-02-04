/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
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

#include <unistd.h>

#include <climits>
#include <cstdlib>
#include <sstream>

#include <pearl/CompoundTask.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocationGroup.h>
#include <pearl/String.h>
#include <pearl/TraceArchive.h>
#include <pearl/pearl.h>

#include "AnalyzeTask.h"
#include "CheckedTask.h"
#include "Logging.h"
#include "OpenArchiveTask.h"
#include "PreprocessTask.h"
#include "ReadDefinitionsTask.h"
#include "ReadTraceTask.h"
#include "ReportWriterTask.h"
#include "StatisticsTask.h"
#include "TaskData.h"
#include "TimedPhase.h"
#include "Timer.h"
#include "scout_types.h"

#if defined(_MPI)
    #include <mpi.h>

    #include "MpiDatatypes.h"
    #include "MpiOperators.h"
    #include "SynchronizeTask.h"
#endif    // _MPI
#if defined(_OPENMP)
    #include <omp.h>
#endif    // _OPENMP

using namespace std;
using namespace pearl;
using namespace scout;


// --- Constants ------------------------------------------------------------

const char* const copyright =
    "Copyright (c) 1998-2021 Forschungszentrum Juelich GmbH\n"
    "Copyright (c) 2014-2021 RWTH Aachen University\n"
    "Copyright (c) 2009-2014 German Research School for Simulation"
    " Sciences GmbH";


// --- Global variables -----------------------------------------------------

namespace
{
/// Global process rank; always zero for non-MPI
int mpiRank = 0;

#if defined(_MPI)
    /// Flag indicating whether CLC-based timestamp synchronization should
    /// be applied. Only makes sense for MPI and hybrid MPI/OpenMP.
    bool synchronize = false;

    /// Flag indicating whether MPI has been finalized
    bool finalized = false;
#endif // _MPI
}      // unnamed namespace


/// Experiment archive anchor name
string anchorName;

/// Experiment archive directory
string archiveDirectory;

/// Flag indicating whether metric statistics and most-severe instance
/// tracking should be enabled
bool enableStatistics = true;

/// Flag indicating whether critical-path analysis should be enabled
bool enableCriticalPath = true;

/// Flag indicating whether delay analysis should be enabled
bool enableDelayAnalysis = true;

/// Flag indicating whether asynchronous message handling (a.k.a. active
/// messages) should be enabled
bool enableAsynchronous = false;

/// Flag indicating whether only a single-pass replay should be used
bool enableSinglePass = false;


// --- Function prototypes --------------------------------------------------

// ----- Generic functions -----

int
main(int    argc,
     char** argv);

void
Initialize(int*    argc,
           char*** argv);

void
Finalize();

bool
ProcessArguments(int    argc,
                 char** argv);

void
Usage(const char* command);

bool
ReadDefinitions(TaskDataShared& sharedData);

void
SetupPhases(CompoundTask&    scout,
            TaskDataShared&  sharedData,
            TaskDataPrivate& privateData);

bool
Analyze();


// ----- MPI-specific functions -----

#if defined(_MPI)
    void
    InitializeMPI(int*    argc,
                  char*** argv);

    void
    FinalizeMPI();

    extern "C" void
    ExitHandlerMPI();

    bool
    ValidateParallelizationMPI(const GlobalDefs& defs);
#endif    // _MPI


// ----- OpenMP-specific functions -----

#if defined(_OPENMP)
    bool
    ValidateParallelizationOMP(int num_threads);
#endif    // _OPENMP


// --------------------------------------------------------------------------
//
// SCOUT  -  SCalable Optimization UTility
//
// --------------------------------------------------------------------------

int
main(int    argc,
     char** argv)
{
    // Setup
    Initialize(&argc, &argv);
    LogMsg(0, "SCOUT   (%s)\n%s\n\n", PACKAGE_STRING, copyright);
    bool error = ProcessArguments(argc, argv);

    // Parallel trace analysis;
    // skipped on prior error due to short-circuit evaluation
    error = error || Analyze();

    // Cleanup
    Finalize();

    return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}


// --- Generic functions ----------------------------------------------------

/**
 *  @brief Program initialization.
 *
 *  Performs all necessary initialization steps at program startup.  This
 *  includes the initialization of global variables as well as required
 *  libraries.
 *
 *  @param argc
 *      Pointer to argument count parameter of main()
 *  @param argv
 *      Pointer to argument vector parameter of main()
 **/
void
Initialize(int*    argc,
           char*** argv)
{
    #if defined(_MPI)
        InitializeMPI(argc, argv);
        PEARL_mpi_init();
    #else    // !_MPI
        PEARL_init();
    #endif    // !_MPI
}


/**
 *  @brief Program finalization.
 *
 *  Performs all necessary clean-up steps at program finalization.
 **/
void
Finalize()
{
    PEARL_finalize();

    #if defined(_MPI)
        FinalizeMPI();
    #endif    // _MPI
}


/**
 *  @brief Process command line arguments.
 *
 *  Processes the command line arguments given to the analyzer.
 *
 *  @param argc
 *      Argument count parameter of main()
 *  @param argv
 *      Argument vector parameter of main()
 *  @returns
 *      `true` on error, `false` otherwise
 **/
bool
ProcessArguments(int    argc,
                 char** argv)
{
    bool error = false;

    // Process argument list
    int index = 1;
    while (index < argc)
    {
        // Get next option
        string arg(argv[index]);

        // Deprecated options
        if (arg == "-i")
        {
            LogMsg(0,
                   "Ignoring deprecated command-line option '-i':\n"
                   "    Instance tracking and statistics now enabled by default.\n\n");
        }
        else if (arg == "-s")
        {
            LogMsg(0,
                   "Command-line option '-s' is deprecated, use '--time-correct' instead.\n\n");
            #if defined(_MPI)
                synchronize = true;
            #endif    // _MPI
        }

        // Generic options
        else if (arg == "--help")
        {
            Usage(argv[0]);
            error = true;
            break;
        }
        else if (  (arg == "-v")
                || (arg == "--verbose"))
        {
            LogSetVerbosity(LogGetVerbosity() + 1);
        }
        else if (arg == "--statistics")
        {
            enableStatistics = true;
        }
        else if (arg == "--no-statistics")
        {
            enableStatistics = false;
        }
        else if (arg == "--critical-path")
        {
            enableCriticalPath = true;
        }
        else if (arg == "--no-critical-path")
        {
            enableCriticalPath = false;
        }
        else if (arg == "--rootcause")
        {
            enableDelayAnalysis = true;
        }
        else if (arg == "--no-rootcause")
        {
            enableDelayAnalysis = false;
        }
        else if (arg == "--single-pass")
        {
            enableSinglePass = true;
            LogMsg(0,
                   "Using single-pass replay.\n"
                   "   Critical-path and root-cause analysis will be disabled.\n"
                   "   Late-receiver wait states can not be detected.\n\n");
        }

        // MPI-specific options
        #if defined(_MPI)
            else if (arg == "--time-correct")
            {
                synchronize = true;
            }
            else if (arg == "--no-time-correct")
            {
                synchronize = false;
            }
        #endif    // _MPI

        // Unknown option
        else
        {
            #if !defined(_MPI)
                if (  (arg == "--time-correct")
                   || (arg == "--no-time-correct"))
                {
                    LogMsg(0,
                           "Ignoring command-line option '%s': not supported.\n\n",
                           arg.c_str());
                    ++index;
                    continue;
                }
            #endif    // !_MPI

            if ((argc - index) != 1)
            {
                LogMsg(0, "Unknown command-line option '%s'.\n\n", arg.c_str());
                Usage(argv[0]);
                error = true;
                break;
            }
        }

        ++index;
    }

    // Disable critical-path analysis if single pass is requested
    if (enableSinglePass)
    {
        enableCriticalPath  = false;
        enableDelayAnalysis = false;
    }

    // Treat first remaining argument as experiment archive name
    anchorName = argv[index - 1];

    return CheckGlobalError(error, "Aborted.");
}


/**
 *  @brief Print usage information.
 *
 *  Prints usage information and aborts the execution.
 *
 *  @param command
 *      Executable name
 **/
void
Usage(const char* command)
{
    #if defined(_MPI)
        const char* const launcher = "<launchcmd> ";
    #else    // !_MPI
        const char* const launcher = "";
    #endif    // !_MPI

    // Print usage information
    LogMsg(0,
           "Usage: %s%s [OPTION]... <ANCHORFILE | EPIK DIRECTORY>\n"
           "Options:\n"
           "  --statistics       Enables instance tracking and statistics [default]\n"
           "  --no-statistics    Disables instance tracking and statistics\n"
           "  --critical-path    Enables critical-path analysis [default]\n"
           "  --no-critical-path Disables critical-path analysis\n"
           "  --rootcause        Enables root-cause analysis [default]\n"
           "  --no-rootcause     Disables root-cause analysis\n"
           "  --single-pass      Single-pass forward analysis only\n"
           #if defined(_MPI)
               "  --time-correct     Enables enhanced timestamp correction\n"
               "  --no-time-correct  Disables enhanced timestamp correction [default]\n"
           #endif    // _MPI
           "  --verbose, -v      Increase verbosity\n"
           "  --help             Display this information and exit\n\n",
           launcher,
           command);
}


/**
 *  @brief Reads the global definitions.
 *
 *  Reads the global definition data from disk and sets up the corresponding
 *  data structure.
 *
 *  @param sharedData
 *      Shared task data object
 *  @returns
 *      `true` on error, `false` otherwise
 **/
bool
ReadDefinitions(TaskDataShared& sharedData)
{
    CompoundTask prep;
    TimedPhase*  phase;

    // Setup opening phase
    OpenArchiveTask* open;
    phase = new TimedPhase("Opening experiment archive");
    open  = new OpenArchiveTask(anchorName, sharedData);
    phase->add_task(CheckedTask::make_checked(open));
    prep.add_task(phase);

    // Set up definition reading phase
    ReadDefinitionsTask* reader;
    phase  = new TimedPhase("Reading definition data   ");
    reader = new ReadDefinitionsTask(sharedData);
    phase->add_task(CheckedTask::make_checked(reader));
    prep.add_task(phase);

    // Execute phases & handle errors
    if (!prep.execute())
    {
        return true;
    }

    anchorName       = sharedData.mArchive->getAnchorName();
    archiveDirectory = sharedData.mArchive->getArchiveDirectory();

    return false;
}


/**
 *  @brief Sets up the individual execution phases of SCOUT.
 *
 *  This function creates the task objects for the individual execution phases
 *  of SCOUT and adds them to the given compound task @p scout.
 *
 *  @param scout
 *      Compound task to be set up
 *  @param sharedData
 *      Shared task data object
 *  @param privateData
 *      Private task data object
 **/
void
SetupPhases(CompoundTask&    scout,
            TaskDataShared&  sharedData,
            TaskDataPrivate& privateData)
{
    TimedPhase*    phase;
    ReadTraceTask* reader;
    phase  = new TimedPhase("Reading event trace data  ");
    reader = new ReadTraceTask(sharedData, privateData);
    phase->add_task(reader);
    scout.add_task(phase);

    PreprocessTask* prep;
    phase = new TimedPhase("Preprocessing             ");
    prep  = new PreprocessTask(sharedData, privateData);
    phase->add_task(prep);
    scout.add_task(phase);

    SynchronizeTask* synchronizer = NULL;
    #if defined(_MPI)
        if (synchronize)
        {
            phase        = new TimedPhase("Timestamp correction      ");
            synchronizer = new SynchronizeTask(privateData);
            phase->add_task(CheckedTask::make_checked(synchronizer));
            scout.add_task(phase);
        }
    #endif    // _MPI

    AnalyzeTask* analyzer;
    phase    = new TimedPhase("Analyzing trace data      ");
    analyzer = new AnalyzeTask(sharedData, privateData);
    phase->add_task(analyzer);
    scout.add_task(phase);

    ReportWriterTask* writer;
    phase  = new TimedPhase("Writing analysis report   ");
    writer = new ReportWriterTask(sharedData,
                                  privateData,
                                  analyzer,
                                  archiveDirectory,
                                  mpiRank);
    phase->add_task(CheckedTask::make_checked(writer));
    scout.add_task(phase);

    StatisticsTask* stats;
    stats = new StatisticsTask(privateData, synchronizer);
    analyzer->register_statistics_cbs(stats);
    scout.add_task(stats);
}


/**
 *  @brief Parallel trace analysis core.
 *
 *  This is the core of the parallel trace analyzer.  It first reads in the
 *  required data files (definitions, trace data), performs some preprocessing,
 *  executes the parallel pattern search, and then collates the analysis report.
 *
 *  @returns
 *      `true` on error, `false` otherwise
 *
 *  @todo Fix to deal with non-CPU locations & non-process location groups
 **/
bool
Analyze()
{
    Timer total;

    // Display archive name
    LogMsg(0, "Analyzing experiment archive %s\n\n", anchorName.c_str());

    // Open archive and read global definitions
    TaskDataShared sharedData;
    bool           error = ReadDefinitions(sharedData);
    if (error)
    {
        return error;
    }

    #if defined(_MPI) || defined(_OPENMP)
        const GlobalDefs& defs = *sharedData.mDefinitions;
    #endif    // _MPI || _OPENMP
    #if defined(_MPI)
        error = ValidateParallelizationMPI(defs);
        if (error)
        {
            return error;
        }
    #endif    // _MPI

    // Eventually go hybrid...
    #if defined(_OPENMP)
        // FIXME: deal with non-CPU locations and non-proces groups
        const LocationGroup& group       = defs.getLocationGroup(mpiRank);
        uint32_t             count       = group.numLocations();
        int                  num_threads = 0;
        for (uint32_t i = 0; i < count; ++i)
        {
            if (group.getLocation(i).getName().getString() != "VOID")
            {
                ++num_threads;
            }
        }

        // Configure OpenMP runtime appropriately
        omp_set_num_threads(num_threads);
    #endif    // _OPENMP
    #pragma omp parallel reduction(|:error)
    {
        #if defined(_OPENMP)
            error = ValidateParallelizationOMP(num_threads);
        #endif    // _OPENMP

        // Set up SCOUT phases
        CompoundTask    scout;
        TaskDataPrivate privateData;
        SetupPhases(scout, sharedData, privateData);

        // Execute SCOUT phases;
        // skipped on prior error due to short-circuit evaluation
        error = error || CheckGlobalError(!scout.execute(), "Internal error!");
    }    // omp parallel

    // Print processing time
    if (!error)
    {
        LogMsg(0, "\nTotal processing time     : %.3fs\n", total.value());
    }

    return error;
}


// --- MPI-specific functions -----------------------------------------------

#if defined(_MPI)
    /**
     *  @brief MPI-specific initialization.
     *
     *  Performs all MPI-specific initialization steps, i.e., initialization of
     *  the library, registration of user-defined reduction operators, datatypes,
     *  etc.
     *
     *  @param argc
     *      Pointer to argument count parameter of main()
     *  @param argv
     *      Pointer to argument vector parameter of main()
     **/
    void
    InitializeMPI(int*    argc,
                  char*** argv)
    {
        #if defined(HAVE_DECL_MPI_INIT_THREAD)
            // Initialize MPI with required level of thread support
            int required;
            int provided;
            #if defined(_OPENMP)
                required = MPI_THREAD_FUNNELED;
            #else    // !_OPENMP
                required = MPI_THREAD_SINGLE;
            #endif    // !_OPENMP
            MPI_Init_thread(argc, argv, required, &provided);
        #else    // !HAS_MPI_INIT_THREAD
            // Initialize MPI
            MPI_Init(argc, argv);
        #endif    // !HAS_MPI_INIT_THREAD

        // Determine global process rank
        MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);

        // Initialize logging
        if (mpiRank != 0)
        {
            LogSetVerbosity(INT_MIN);
        }

        #if defined(HAVE_DECL_MPI_INIT_THREAD)
            // Verify thread level
            if (provided < required)
            {
                LogMsg(0, "MPI thread mode 'funneled' not supported!\n");
                LogMsg(0,
                       "Continuing with analysis, but you might experience problems.\n\n");
            }
        #elif defined(_OPENMP)    // !HAS_MPI_INIT_THREAD && _OPENMP
            // Display warning since level of thread support is implementation defined
            LogMsg(0, "Using MPI_Init; level of thread support unknown!\n");
            LogMsg(0,
                   "Continuing with analysis, but you might experience problems.\n\n");
        #endif    // !HAS_MPI_INIT_THREAD && _OPENMP

        // Register user-defined reduction operators
        InitDatatypes();
        InitOperators();

        // Register MPI-specific exit handler
        if (atexit(ExitHandlerMPI) != 0)
        {
            LogMsg(0, "Unable to register exit handler!\n");
        }
    }


    /**
     *  @brief MPI-specific finalization.
     *
     *  Performs all MPI-specific finalization steps, i.e., de-registration of
     *  user-defined datatypes, reduction operators, etc.
     **/
    void
    FinalizeMPI()
    {
        // De-register user-defined operators & datatypes
        FreeOperators();
        FreeDatatypes();

        // MPI_Finalize is not required to return and may call exit(), in which
        // case our own exit handler should *not* call MPI_Abort
        finalized = true;
        MPI_Finalize();
    }


    /**
     *  @brief MPI-specific exit handler.
     *
     *  This MPI-specific exit handler triggers MPI_Abort() when the application
     *  exits unexpectedly (e.g., when only a single process calls exit()).
     **/
    void
    ExitHandlerMPI()
    {
        // In case of a "controlled" exit (i.e., at the end of execution or as a
        // result of handling an error) MPI is already finalized. We need to call
        // MPI_Abort() only in unexpected situations.
        if (!finalized)
        {
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
    }


    /**
     *  @brief Validates whether the number of processes matches the experiment.
     *
     *  This function validates whether the number of MPI processes used matches
     *  the number of processes used to record the experiment.  If this condition
     *  is not fulfilled, SCOUT is gracefully aborted.
     *
     *  @param defs
     *      Global definitions object
     *  @returns
     *      `true` on error, `false` otherwise
     *
     *  @todo Handle non-process location groups
     **/
    bool
    ValidateParallelizationMPI(const GlobalDefs& defs)
    {
        // Determine number of required processes
        // FIXME: Handle non-process location groups
        int required = defs.numLocationGroups();

        // Determine number of provided processes
        int provided;
        MPI_Comm_size(MPI_COMM_WORLD, &provided);

        // Make sure that the number of processes matches the experiment
        ostringstream message;
        message << "Number of processes does not match experiment (required "
                << required << ", got " << provided << ")!";

        return CheckGlobalError((required != provided), message.str());
    }
#endif    // _MPI


// --- OpenMP-specific functions --------------------------------------------

#if defined(_OPENMP)
    /**
     *  @brief Validates whether the number of threads matches the experiment.
     *
     *  This function validates whether the number of threads used matches the
     *  number of threads used to record the experiment (on a per-process basis).
     *  If this condition is not fulfilled, SCOUT is gracefully aborted.
     *
     *  @param num_threads
     *      Number of threads used in the experiment
     *  @returns
     *      `true` on error, `false` otherwise
     **/
    bool
    ValidateParallelizationOMP(int num_threads)
    {
        // Determine number of provided threads
        int provided = omp_get_num_threads();

        // Make sure that the number of threads matches the experiment
        ostringstream message;
        message << "Number of threads does not match experiment (required "
                << num_threads << ", got " << provided << ")!";

        return CheckGlobalError((num_threads != provided), message.str());
    }
#endif    // _OPENMP
