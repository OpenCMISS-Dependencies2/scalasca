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

#include <sys/stat.h>

#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <mpi.h>

#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/Otf2Writer.h>
#include <pearl/ScopedPtr.h>
#include <pearl/TraceArchive.h>
#include <pearl/pearl.h>

#include "HypBalance.h"
#include "HypCutMessage.h"
#include "HypScale.h"
#include "Hypothesis.h"
#include "Model.h"
#include "SilasConfig.h"
#include "Simulator.h"
#include "Timer.h"

using namespace std;
using namespace pearl;
using namespace silas;


// --- Constants ------------------------------------------------------------

const char* const copyright =
    "Copyright (c) 1998-2021 Forschungszentrum Juelich GmbH\n"
    "        Copyright (c) 2009-2013 German Research School for Simulation\n"
    "                                Sciences GmbH";


// --- Global variables -----------------------------------------------------

int mpiRank;

/* Using reenact model for simulation */
Model* model = NULL;
/* Using an empty hypothesis will result in identity simulation */
Hypothesis* hypothesis = new Hypothesis();
/* Hypothesis Part to simulate load balance */
HypBalance* balancer = new HypBalance();
/* Hypothesis Part to cut zero sized messages */
HypCutMessage* message_surgeon = new HypCutMessage();
/* Region scaler */
HypScale* rescaler = new HypScale();


// --- Function prototypes --------------------------------------------------

int
main(int    argc,
     char** argv);

extern "C" void
cleanup();

void
log(const char* fmt,
    ...);

void
global_error(bool          error,
             const string& message);

TraceArchive*
open_archive(const string& archiveName);

GlobalDefs*
read_definitions(TraceArchive& archive);

LocalTrace*
read_trace(TraceArchive&     archive,
           const GlobalDefs& defs);

void
preprocess_trace(GlobalDefs& defs,
                 LocalTrace& trace);

void
simulate_trace(LocalTrace& trace);

void
write_trace(GlobalDefs& defs,
            LocalTrace& trace,
            const char* filename);


extern void
parse_config(const std::string& filename);


// --------------------------------------------------------------------------
//
// SILAS - SImulation of LArge Scale applications
//
// --------------------------------------------------------------------------

void
show_help(int exitcode)
{
    log("Usage: silas [options]\n\n"
        "Options:\n"
        "    -c <config>   Configuration file to use for simulation\n"
        "    -h            Show this help and exit\n"
        "    -i            Input trace (anchor file | EPIK directory)\n"
        "    -o            Output experiment directory name\n");

    exit(exitcode);
}


void
parse_commandline(int            argc,
                  char**         argv,
                  silas::Config* config)
{
    for (int i = 1; i < argc; i++)
    {
        /* check for options */
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
                case 'c':
                    {
                        /* parse the configuration file given in the next
                         * commandline option */
                        if (i < argc)
                        {
                            parse_config(argv[++i]);
                        }
                        break;
                    }

                case 'h':
                    {
                        /* show help and exit */
                        show_help(EXIT_SUCCESS);
                        break;
                    }

                case 'i':
                    {
                        if (i < argc)
                        {
                            config->set_input_trace(argv[++i]);
                        }
                        break;
                    }

                case 'o':
                    {
                        if (i < argc)
                        {
                            config->set_output_trace(argv[++i]);
                        }
                        break;
                    }

                default:
                    {
                        /* unknown option */
                        cerr << "Unknown option '-" << argv[i][1] << "'\n";
                        show_help(EXIT_FAILURE);
                    }
            }
        }
    }
}


int
main(int    argc,
     char** argv)
{
    // Initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    PEARL_mpi_init();
    atexit(cleanup);
    Timer   total_time;
    Config* config = new Config();

    /* display copyright notice */
    log("SILAS   %s\n\n", copyright);

    parse_commandline(argc, argv, config);

    /* validate and open trace archive */
    log("Using experiment archive %s\n\n", config->get_input_trace().c_str());

    /* open trace archive */
    const ScopedPtr< TraceArchive > archive(open_archive(config->get_input_trace()));
    /* read event definitions */
    const ScopedPtr< GlobalDefs > defs(read_definitions(*archive));
    /* read local traces to memory */
    const ScopedPtr< LocalTrace > trace(read_trace(*archive, *defs));

    /* preprocess trace for simulation: unification, etc. */
    preprocess_trace(*defs, *trace);
    /* trigger simulation */
    simulate_trace(*trace);

    /* write trace to disk */
    write_trace(*defs, *trace, config->get_output_trace().c_str());

    log("\nTotal processing time: %s\n\n", total_time.value_str().c_str());

    delete config;

    PEARL_finalize();
}


// --- Utility functions ----------------------------------------------------

/**
 * Providing a graceful exit by cleaning up the parallel environment,
 * e.g. calling MPI_Finalize;
 */
void
cleanup()
{
    MPI_Finalize();
}


/**
 * Logging function
 */
void
log(const char* fmt,
    ...)
{
    va_list ap;

    // Display log message on master node
    va_start(ap, fmt);
    if (mpiRank == 0)
    {
        vprintf(fmt, ap);
        fflush(NULL);
    }
    va_end(ap);
}


/**
 * Checking whether this error is a global error or not
 * @param error Flag indicating an error
 * @param message Message string indicating the cause of the error
 */
void
global_error(bool          error,
             const string& message)
{
    string msg    = "SILAS: " + message;
    int    local  = error ? 1 : 0;
    int    global = 0;
    int    size;

    // Check whether it is a global error or not
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Allreduce(&local, &global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // No error at all
    if (global == 0)
    {
        return;
    }

    // Display error message and exit
    if (global == size)
    {
        UTILS_DEBUG_PRINTF(SCALASCA_DEBUG_VERBOSE_ERRORS, "%s", msg.c_str());
    }
    if (  ((global != size) && error)
       || ((global == size) && (mpiRank == 0)))
    {
        UTILS_FATAL("%s", msg.c_str());
    }
    exit(EXIT_FAILURE);
}


/**
 * Open trace experiment archive
 * @param archiveName Name of the experiment archive
 * @return Pointer to experiment archive object
 */
TraceArchive*
open_archive(const string& archiveName)
{
    log("Opening experiment archive ... ");
    Timer read_time;

    // Open archive
    string        message;
    bool          error  = false;
    TraceArchive* result = NULL;
    try
    {
        result = TraceArchive::open(archiveName);
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    error = error || (result == NULL);
    global_error(error, message);

    log("done (%s).\n", read_time.value_str().c_str());

    return result;
}


/**
 * Read global definitions
 * @param archive Experiment archive object
 * @return Pointer to GlobalDefs object containing the global definitions
 */
GlobalDefs*
read_definitions(TraceArchive& archive)
{
    log("Reading definitions file   ... ");
    Timer read_time;

    // Read definition file
    string      message;
    bool        error  = false;
    GlobalDefs* result = NULL;
    try
    {
        result = archive.getDefinitions();
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    error = error || (result == NULL);
    global_error(error, message);

    log("done (%s).\n", read_time.value_str().c_str());

    return result;
}


/**
 * Read event trace into memory
 * @param archive Experiment archive object
 * @param defs    Global definitions object
 * @return Pointer to LocalTrace object containing the event stream
 */
LocalTrace*
read_trace(TraceArchive&     archive,
           const GlobalDefs& defs)
{
    log("Reading event trace files  ... ");
    Timer read_time;

    // Determine location
    const LocationGroup& process  = defs.getLocationGroup(mpiRank);
    const Location&      location = process.getLocation(0);

    // Open trace container
    string message;
    bool   error = false;
    try
    {
        archive.openTraceContainer(process);
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    global_error(error, message);

    // Read trace file
    LocalTrace* result = NULL;
    try
    {
        result = archive.getTrace(defs, location);
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    error = error || (result == NULL);
    global_error(error, message);

    // Close trace container
    try
    {
        archive.closeTraceContainer();
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    global_error(error, message);

    log("done (%s).\n", read_time.value_str().c_str());

    return result;
}


/**
 * Preprocessing of trace data, e.g. call tree unification.
 * @param defs Reference on global definitions
 * @param trace Reference on event trace
 */
void
preprocess_trace(GlobalDefs& defs,
                 LocalTrace& trace)
{
    log("Preprocessing local traces ... ");
    Timer preprocess_time;

    string message;
    bool   error = false;

    // Verify call trees & check for errors
    try
    {
        PEARL_verify_calltree(defs, trace);
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    global_error(error, message);

    // Unify call trees & check for errors
    try
    {
        PEARL_mpi_unify_calltree(defs);
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    global_error(error, message);

    // Preprocess trace data & check for errors
    try
    {
        PEARL_preprocess_trace(defs, trace);
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    global_error(error, message);

    log("done (%s).\n", preprocess_time.value_str().c_str());
}


/**
 * Start simulation process
 * @param trace Reference to event trace
 */
void
simulate_trace(LocalTrace& trace)
{
    /* Creating simulator object with model and hypothesis */
    Simulator simulator(trace, model, hypothesis);
    /* Time object to time overall simulation time */
    Timer simulation_time;

    hypothesis->add_part(balancer);
    hypothesis->add_part(message_surgeon);
    hypothesis->add_part(rescaler);

    /* start simulation */
    string message;
    bool   error = false;
    try
    {
        simulator.run();
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    global_error(error, message);
}


/**
 * Writing the predicted event trace to disk
 * @param defs Reference to global definitions
 * @param trace Reference to event trace
 * @param filename Name of the experiment archive
 */
void
write_trace(GlobalDefs& defs,
            LocalTrace& trace,
            const char* filename)
{
    Timer  writing_time;
    string message;
    bool   error = false;

    /* write modified trace */
    log("Writing predicted trace    ... ");
    writing_time.start();
    try
    {
        /* write trace */
        Otf2Writer* writer = new Otf2Writer("Scalasca Trace Simulator " PACKAGE_VERSION);
        writer->write(filename, trace, defs);
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }

    global_error(error, message);
    log("done (%s).\n", writing_time.value_str().c_str());
}
