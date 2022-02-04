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


#include <cstdlib>
#include <iostream>

#include <pearl/CallbackData.h>
#include <pearl/CallbackManager.h>
#include <pearl/Error.h>
#include <pearl/Event.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/TraceArchive.h>
#include <pearl/pearl.h>
#include <pearl/pearl_replay.h>

#ifdef _MPI
    #include <mpi.h>
#endif    // _MPI
#ifdef _OPENMP
    #include <omp.h>
#endif    // _OPENMP

using namespace std;
using namespace pearl;


class DummyPrinter
{
    public:
        DummyPrinter()
            : m_count(0)
        {
        }

        ~DummyPrinter()
        {
            cout << m_count << endl;
        }

        void
        print(const CallbackManager& cbmanager,
              int                    user_event,
              const Event&           event,
              CallbackData*          data)
        {
            cout << event << endl;
            cbmanager.notify(1, event, data);
        }

        void
        count(const CallbackManager& cbmanager,
              int                    user_event,
              const Event&           event,
              CallbackData*          data)
        {
            if (event->getType() == ENTER)
            {
                m_count++;
            }
        }


    private:
        long m_count;
};


/*
 *  Simple PEARL test program
 *
 *  Reads the trace data of the given experiment archive into memory, prints
 *  the trace information of a given rank and/or thread to stdout.
 *
 *  Parameters:
 *    <archive>   Name of experiment archive
 *    <rank>      Rank number
 *    <thread>    OpenMP thread number
 */
int
main(int    argc,
     char** argv)
{
    // Initialize MPI
    #if defined(_MPI) && defined(_OPENMP)
        #if (MPI_VERSION >= 2)
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
    #elif defined(_MPI)
        MPI_Init(&argc, &argv);
    #endif

    // Determine MPI rank
    int rank = 0;
    #if defined(_MPI)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    #endif

    // Initialize PEARL
    #if defined(_MPI)
        PEARL_mpi_init();
    #else
        PEARL_init();
    #endif

    // Validate arguments
    #if defined(_MPI) && defined(_OPENMP)
        if (argc != 4)
        {
            cerr << "Usage: pearl_print.hyb <archive> <rank> <thread>" << endl;
            exit(EXIT_FAILURE);
        }
    #elif defined(_MPI)
        if (argc != 3)
        {
            cerr << "Usage: pearl_print.mpi <archive> <rank>" << endl;
            exit(EXIT_FAILURE);
        }
    #elif defined(_OPENMP)
        if (argc != 3)
        {
            cerr << "Usage: pearl_print.omp <archive> <thread>" << endl;
            exit(EXIT_FAILURE);
        }
    #else
        if (argc != 2)
        {
            cerr << "Usage: pearl_print.ser <archive>" << endl;
            exit(EXIT_FAILURE);
        }
    #endif

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
        // Each process/thread reads only "his" trace. In case of an error, bad
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
        #ifdef _MPI
            #pragma omp master
            {
                PEARL_mpi_unify_calltree(*defs);
            }
            #pragma omp barrier
        #endif

        // Required trace preprocessing
        PEARL_preprocess_trace(*defs, *trace);

        // ***** Two ways of processing the data *****
        #if defined(_MPI) && defined(_OPENMP)
            if ((rank == atoi(argv[2])) && (tid == atoi(argv[3])))
        #elif defined(_MPI)
            if (rank == atoi(argv[2]))
        #elif defined(_OPENMP)
            if (tid == atoi(argv[2]))
        #endif
        {
            #if 0
                // 1. Using iterator semantics
                Event ev = trace->begin();
                while (ev != trace->end())
                {
                    cout << ev;

                    // Here, one could also check for the event type and query some
                    // values, e.g.,
                    //
                    //    cout << ev->getTimestamp() << endl;
                    //
                    // The basic interface is defined in "Event_rep.h", however, keep in
                    // mind that not every event implements all methods (for example,
                    // ENTER events do not provide a communicator).
                    //
                    // NOTE: Calltree information is provided as a member function of the
                    // Event class, i.e., use
                    //
                    //    cout << ev.cnode().getRegion().getName() << endl;
                    //
                    // for this purpose.

                    ++ev;
                }
            #else
                // 2. Using the PEARL replay functionality
                DummyPrinter    printer;
                CallbackManager manager;

                // Register callback function for event of type "GROUP_ALL"
                // Try changing the 1st parameter to, e.g., MPI_SEND and compare the
                // result. The possible types are defined in "pearl_types.h"
                manager.register_callback(GROUP_ALL, PEARL_create_callback(&printer,
                                                                           &DummyPrinter::print));

                // Register callback function for user-defined event with ID 1,
                // triggered from within DummyPrinter::print()
                manager.register_callback(1, PEARL_create_callback(&printer,
                                                                   &DummyPrinter::count));

                // As 3rd parameter, you could also provide a user-defined data structure
                // which will be passed as the last argument to the registered callback.
                // Here, it's not needed, so NULL is passed.
                //
                // If you want the events in reverse order, use PEARL_backward_replay
                // instead.
                PEARL_forward_replay(*trace, manager, NULL);
            #endif
        }
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
    #ifdef _MPI
        MPI_Finalize();
    #endif

    return EXIT_SUCCESS;
}
