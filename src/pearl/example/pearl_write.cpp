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


#include <cstdlib>
#include <iostream>

#include <mpi.h>

#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/Otf2Writer.h>
#include <pearl/TraceArchive.h>
#include <pearl/pearl.h>
#include <pearl/pearl_replay.h>

#ifdef _OPENMP
    #include <omp.h>
#endif    // _OPENMP

using namespace std;
using namespace pearl;


/*
 *  Simple test program for the PEARL writer
 *
 *  Reads the trace data of the given experiment archive into memory and
 *  writes the data to an archive 'pearl_test'.
 *
 *  Parameters:
 *    <archive>   Name of experiment archive
 */
int
main(int    argc,
     char** argv)
{
    // Initialize MPI
    #if defined(_OPENMP) && (MPI_VERSION >= 2)
        int provided;
        MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
        if (provided == MPI_THREAD_SINGLE)
        {
            cerr << "MPI thread mode 'funneled' not supported!" << endl;
            exit(EXIT_FAILURE);
        }
    #else
        MPI_Init(&argc, &argv);
    #endif

    // Determine MPI rank
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Initialize PEARL
    PEARL_mpi_init();

    // Validate arguments
    if (argc != 2)
    {
        cerr << "Usage: pearl_write <archive>" << endl;
        exit(EXIT_FAILURE);
    }

    // Open trace archive
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

        // Verify whether definitions provided complete call tree
        PEARL_verify_calltree(*defs, *trace);

        // Generate global call tree if per-process call trees were incomplete
        #pragma omp master
        {
            PEARL_mpi_unify_calltree(*defs);
        }
        #pragma omp barrier

        // Required trace preprocessing
        PEARL_preprocess_trace(*defs, *trace);

        // Re-write trace data
        Otf2Writer writer("PEARL writer example");
        writer.write("pearl_test", *trace, *defs);

        #pragma omp barrier

        // Delete trace data
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

    // Delete definition data & archive object
    delete defs;
    delete archive;

    // Finalize PEARL & MPI
    PEARL_finalize();
    MPI_Finalize();

    return EXIT_SUCCESS;
}
