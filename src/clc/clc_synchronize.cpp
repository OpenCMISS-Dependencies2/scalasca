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

#include <cstdlib>
#include <iostream>

#include <mpi.h>

#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/Otf2Writer.h>
#include <pearl/TraceArchive.h>
#include <pearl/pearl.h>

#include "Synchronizer.h"

#ifdef _OPENMP
    #include <omp.h>
#endif    // _OPENMP

using namespace std;
using namespace pearl;


int
main(int    argc,
     char** argv)
{
    #if defined(_OPENMP) && (MPI_VERSION >= 2)
        int provided;
        MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
        if (provided < MPI_THREAD_FUNNELED)
        {
            cerr << "Continuing even though MPI thread mode 'funneled' is not supported..." << endl;
        }
    #else
        MPI_Init(&argc, &argv);
    #endif

    // Determine MPI rank
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Initialize PEARL & validate arguments
    PEARL_mpi_init();

    // Open the trace archive
    // All processes will open the same archive simultaneously. In case of an
    // error, bad things can happen if not all processes fail at the same time.
    // More error checking is needed here...
    TraceArchive* archive = NULL;
    try
    {
        archive = TraceArchive::open(argv[1]);
    }
    catch (const Error& error)
    {
        cerr << error.what() << endl;
        exit(EXIT_FAILURE);
    }

    if (!archive)
    {
        cerr << "Error opening archive." << endl;
        exit(EXIT_FAILURE);
    }

    // Read global definitions
    // All processes will read the same file simultaneously. In case of an error,
    // bad things can happen if not all processes fail at the same time. More
    // error checking is needed here...
    GlobalDefs* defs;
    try
    {
        defs = archive->getDefinitions();
    }
    catch (const Error& error)
    {
        cerr << error.what() << endl;
        exit(EXIT_FAILURE);
    }

    // Determine location group & open trace container
    const LocationGroup& process = defs->getLocationGroup(rank);
    try
    {
        archive->openTraceContainer(process);
    }
    catch (const Error& error)
    {
        cerr << error.what() << endl;
        exit(EXIT_FAILURE);
    }

    // Eventually go hybrid...
    #pragma omp parallel
    {
        // Determine thread ID
        int tid = 0;
        #ifdef _OPENMP
            tid = omp_get_thread_num();
        #endif

        // Determine location
        const Location& location = process.getLocation(tid);

        // Read trace data
        // Each process/thread reads only "its" trace. In case of an error, bad
        // things can happen if not all processes fail at the same time. More
        // error checking is needed here...
        LocalTrace* trace;
        try
        {
            trace = archive->getTrace(*defs, location);
        }
        catch (const Error& error)
        {
            cerr << error.what() << endl;
            exit(EXIT_FAILURE);
        }

        // Verify whether definitions provided complete call tree;
        // if not, re-generate process-local call tree from trace data
        // Both calls can throw exceptions which we silently ignore here...
        PEARL_verify_calltree(*defs, *trace);

        // Generate global call tree if per-process call trees were incomplete
        #pragma omp master
        {
            PEARL_mpi_unify_calltree(*defs);
        }
        #pragma omp barrier

        // Required trace preprocessing
        PEARL_preprocess_trace(*defs, *trace);

        // Create one Synchronizer object per process
        Synchronizer* sync = new Synchronizer(rank, *trace);

        // Synchronize event stream
        sync->synchronize();

        // Print results
        sync->print_statistics();

        // Write synchronized event stream to file
        try
        {
            Otf2Writer* writer = new Otf2Writer("Scalasca Trace Timestamp Corrector " PACKAGE_VERSION);
            writer->write("clc_sync", *trace, *defs);
            delete writer;
        }
        catch (const Error& error)
        {
            cerr << error.what() << endl;
            exit(EXIT_FAILURE);
        }

        // Clean up allocated memory
        #pragma omp barrier
        delete sync;
        delete trace;
    }

    // Close trace container
    try
    {
        archive->closeTraceContainer();
    }
    catch (const Error& error)
    {
        cerr << error.what() << endl;
        exit(EXIT_FAILURE);
    }

    // Free resources
    delete defs;
    delete archive;

    // Finalize PEARL, MPI, and exit
    PEARL_finalize();
    MPI_Finalize();

    return EXIT_SUCCESS;
}
