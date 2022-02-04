/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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

#define SCALASCA_DEBUG_MODULE_NAME    PEARL
#include <UTILS_Debug.h>
#include <mpi.h>

#include <pearl/MpiMessage.h>

#include "CallTree.h"
#include "MpiDefsFactory.h"
#include "MpiEventFactory.h"
#include "MutableGlobalDefs.h"
#include "pearl_ipc.h"
#include "pearl_memory.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


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
 *  @note This function is intended to be used in MPI-based PEARL programs.
 *
 *  @see PEARL_init()
 **/
void
pearl::PEARL_mpi_init()
{
    UTILS_DLOG << "Initializing PEARL (MPI)";

    // Register factories & allocators
    DefsFactory::registerFactory(new MpiDefsFactory());
    EventFactory::registerFactory(new MpiEventFactory());

    // Initialize internal layers
    memoryInit();
    ipcInit();
}


// --- Call tree unification ------------------------------------------------

/**
 *  Combines process-local call trees into a global one. Although this step
 *  is implemented in a hierarchical fashion, the actual unification is only
 *  carried out if the process-local call trees have been modified after
 *  reading the trace, i.e., the call tree provided by the global definitions
 *  object was incomplete. Therefore, this function should only be called
 *  after verifying the process-local call tree using PEARL_verify_calltree(),
 *  but before preprocessing the trace data via PEARL_preprocess_trace().
 *
 *  @note This function is intended to be used in pure MPI-based or hybrid
 *        OpenMP/MPI PEARL programs. In the case of a hybrid OpenMP/MPI
 *        program, make sure to call this function <b>only from the master
 *        thread.</b>
 *
 *  @param defs Global definitions object
 *
 *  @see PEARL_verify_calltree(), PEARL_preprocess_trace()
 **/
void
pearl::PEARL_mpi_unify_calltree(GlobalDefs& defs)
{
    MutableGlobalDefs& mutableDefs = dynamic_cast< MutableGlobalDefs& >(defs);

    // Determine global time offset accross processes
    timestamp_t offset = mutableDefs.getGlobalOffset();
    timestamp_t global;
    MPI_Allreduce(&offset, &global, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    mutableDefs.setGlobalOffset(global);

    // Determine whether call tree unification is necessary
    CallTree* ctree      = mutableDefs.getCallTree();
    int       global_mod = 0;
    int       local_mod  = ctree->getModified();
    MPI_Allreduce(&local_mod, &global_mod, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (global_mod == 0)
    {
        return;
    }

    // Call tree unification
    int rank;
    int size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Display informational message
    if (rank == 0)
    {
        UTILS_DEBUG("\nExperiment contains incomplete/inconsistent definition of"
                    "\ncall tree. Reconstructing global call tree from trace data.");
    }

    // # iterations is ceil(log2(size)); if size is a power of 2, the loop
    // counts one iteration too much
    int maxIter = 0;
    int iter    = size;
    while (iter)
    {
        ++maxIter;
        iter /= 2;
    }
    if ((size & (size - 1)) == 0)
    {
        --maxIter;
    }

    // Hierarchical call-tree unification
    for (iter = 0; iter < maxIter; ++iter)
    {
        int dist   = 1 << iter;      // Rank offset sender/receiver: 2^iter
        int step   = 2 * dist;       // Rank offset of two receivers: 2*dist
        int source = rank + dist;    // Sender rank

        // Am I a receiver and is there a corresponding sender?
        if (((rank % step) == 0) && (source < size))
        {
            // Receive and merge local call trees
            MpiMessage message(MPI_COMM_WORLD);
            message.recv(source, 0);
            CallTree local_ctree(defs, message);
            ctree->merge(local_ctree);
        }

        /* Am I a sender? */
        else if ((rank % step) == dist)
        {
            int dest = rank - dist;    // Receiver rank

            // Pack and send local call tree
            MpiMessage message(MPI_COMM_WORLD);
            ctree->pack(message);
            message.send(dest, 0);
        }
    }

    // Broadcast global call tree
    MpiMessage message(MPI_COMM_WORLD);
    ctree->pack(message);
    message.bcast(0);
    CallTree* global_ctree = new CallTree(defs, message);
    mutableDefs.setCallTree(global_ctree);
}
