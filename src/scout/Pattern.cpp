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

#include "Pattern.h"

#include <cstring>

#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/LocationGroup.h>

#include "Patterns_gen.h"
#include "ReportData.h"

#if defined(_MPI)
    #include <mpi.h>
#endif    // _MPI
#if defined(_OPENMP)
    #include <omp.h>
#endif    // _OPENMP

using namespace std;
using namespace pearl;
using namespace scout;


// --- Symbolic names -------------------------------------------------------

#define PATTERNS_URL    "@mirror@scalasca_patterns.html#"


// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Destroys the instance.
 *
 *  Destructor. Destroys the instance and calls the cleanup() member function
 *  to perform pattern-specific data management tasks.
 **/
Pattern::~Pattern()
{
}


// --- Registering callbacks ------------------------------------------------

/**
 *  @brief Registers the pattern-specific replay callbacks.
 *
 *  This method registers the replay callbacks that need to be triggered
 *  during the trace replay for this pattern and calls the init() member
 *  function to perform pattern-specific data management tasks.
 *
 *  @param cbmanager
 *      Map of CallbackManager objects with which the callbacks should be
 *      registered
 **/
void
Pattern::reg_cb(CallbackManagerMap& cbmanagers)
{
    init();
}


// --- Retrieving pattern information ---------------------------------------

/**
 *  @fn    virtual long Pattern::get_id() const
 *  @brief Returns the pattern ID
 *
 *  Returns the numeric identifier associated with this pattern.
 *
 *  @return Pattern ID
 **/


/**
 *  @brief Returns the ID of the parent pattern
 *
 *  Returns the numeric identifier associeted with the parent pattern in the
 *  pattern hierarchy. The default implementation returns PAT_NONE.
 *
 *  @return Parent pattern ID
 **/
long
Pattern::get_parent() const
{
    return PAT_NONE;
}


/**
 *  @fn    virtual std::string Pattern::get_name() const
 *  @brief Returns the pattern name
 *
 *  Returns the display name of the pattern.
 *
 *  @return Pattern name
 **/


/**
 *  @fn    virtual std::string Pattern::get_unique_name() const
 *  @brief Returns the pattern's unique name
 *
 *  Returns the unique name of the pattern which can be used to uniquely
 *  identify a particular pattern.
 *
 *  @return Unique name
 **/


/**
 *  @fn    virtual std::string Pattern::get_descr() const
 *  @brief Returns a brief pattern description
 *
 *  Returns a brief description of the pattern (approximately one line of
 *  text).
 *
 *  @return  Description
 **/


/**
 *  @fn    virtual std::string Pattern::get_unit() const
 *  @brief Returns the pattern's unit of measurement.
 *
 *  Returns the unit of measurement for this pattern. It can be either "sec"
 *  (i.e., seconds), "occ" (i.e., occurrences) or "bytes".
 *
 *  @return Unit of measurement
 **/


/**
 *  @brief Returns the pattern description URL.
 *
 *  Returns the URL where a more detailed description of the pattern can be
 *  found. It is composed of a base URL and the unique name.
 *
 *  @return Description URL
 **/
string
Pattern::get_url() const
{
    return (PATTERNS_URL + get_unique_name());
}


/**
 *  @brief Returns the pattern's visibility.
 *
 *  Returns the external visibility of the pattern. This allows for "internal"
 *  patterns which can define callbacks and store data, but which are not
 *  written to the generated analysis report file.
 *
 *  @return Visibility
 **/
bool
Pattern::is_hidden() const
{
    return false;
}


// --- Writing severity values ----------------------------------------------

/**
 *  @brief Collates the pattern's severities and writes the corresponding
 *         section in the CUBE report.
 *
 *  Generates the severity section of the CUBE report for the pattern.
 *  Therefore, it first collates the values from each process/thread on the
 *  "master node" (the details depend on the parallel programming paradigm
 *  used) and then writes the gathered data to file. This is done incrementally
 *  per call-tree node.
 *
 *  @param  data   Temporary data used for report writing
 *  @param  rank   Global process rank (MPI rank or 0)
 *  @param  trace  Local trace-data object
 *
 *  @todo Fix to deal with non-CPU locations & non-process location groups
 **/
void
Pattern::gen_severities(ReportData&              data,
                        int                      rank,
                        const pearl::LocalTrace& trace)
{
    const pearl::GlobalDefs& defs = trace.get_definitions();

    // --- Determine thread information ---
    // FIXME: deal with non-CPU locations / non-process groups
    int num_threads = defs.getLocationGroup(rank).numLocations();
    int thread_id;
    #if defined(_OPENMP)
        thread_id = omp_get_thread_num();
    #else    // !_OPENMP
        thread_id = 0;
    #endif    // !_OPENMP

    // --- Set up callpath usage bit vectors ---
    uint32_t total_cpaths = defs.numCallpaths();
    uint32_t use_size     = (total_cpaths + 7) / 8;

    // The local bit vector is implicitly shared!
    static uint8_t* local_use  = NULL;
    uint8_t*        global_use = NULL;
    #pragma omp master
    {
        local_use = new uint8_t[use_size];
        memset(local_use, 0, use_size * sizeof(uint8_t));
        if (rank == 0)
        {
            global_use = new uint8_t[use_size];
            memset(global_use, 0, use_size * sizeof(uint8_t));
        }
    }
    {
        // FCCpx requires block
        #pragma omp barrier
    }

    // --- Collate callpath usage bit vectors ---
    uint32_t num_unknowns = 0;
    for (uint32_t i = 0; i < total_cpaths; i++)
    {
        Callpath* callpath = defs.get_cnode(i);

        // Special case: "UNKNOWN" callpath found in EPIK traces
        const Region& region = callpath->getRegion();
        if (  (region.getDisplayName().getString() == "UNKNOWN")
           && (region.getFilename().getString() == "EPIK")
           && (region.getDescription().getString() == "EPIK"))
        {
            ++num_unknowns;
            continue;
        }

        // Skip callpaths for which no data exists
        if (skip_cnode(*callpath))
        {
            continue;
        }

        map< Callpath*, double >::const_iterator it = m_severity.find(callpath);
        if (it != m_severity.end())
        {
            #pragma omp atomic
            local_use[(i - num_unknowns) / 8] |= (128 >> (i - num_unknowns) % 8);
        }
    }
    #pragma omp barrier
    #pragma omp master
    {
        #if defined(_MPI)
            MPI_Reduce(local_use, global_use, use_size, MPI_BYTE, MPI_BOR, 0,
                       MPI_COMM_WORLD);
        #else    // !_MPI
            delete[] global_use;
            global_use = local_use;
            local_use  = NULL;
        #endif    // !_MPI
        delete[] local_use;

        if (rank == 0)
        {
            // Set callpath bit vector
            cube_set_known_cnodes_for_metric(data.cb, data.metrics[get_id()],
                                             reinterpret_cast< char* >(global_use));
        }
    }

    // --- Determine optimal callpath order for CUBE writing ---
    static uint32_t* cpath_order = NULL;
    #pragma omp master
    {
        cpath_order = new uint32_t[total_cpaths];

        // Determine order on master
        if (rank == 0)
        {
            carray* callpaths = cube_get_cnodes_for_metric(data.cb,
                                                           data.metrics[get_id()]);
            for (uint32_t i = 0; i < (total_cpaths - num_unknowns); ++i)
            {
                cpath_order[i] = static_cast< cube_cnode* >(callpaths->data[i])->id;
            }
        }
        #if defined(_MPI)
            MPI_Bcast(cpath_order, total_cpaths - num_unknowns, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        #endif    // _MPI
    }
    {
        // FCCpx requires block
        #pragma omp barrier
    }

    // --- Collate results ---
    for (uint32_t i = 0; i < (total_cpaths - num_unknowns); i++)
    {
        // FIXME: This assumes that only one "UNKNOWN" callpath exists and has ID 0
        Callpath* callpath = defs.get_cnode(cpath_order[i] + num_unknowns);
        if (skip_cnode(*callpath))
        {
            continue;
        }

        // Collate local severities
        #pragma omp barrier
        data.mLocalSevBuffer[thread_id] = 0;
        map< Callpath*, double >::const_iterator it = m_severity.find(callpath);
        if (it != m_severity.end())
        {
            data.mLocalSevBuffer[thread_id] = it->second;
        }
        #pragma omp barrier

        #pragma omp master
        {
            #if defined(_MPI)
                // Gather data from all processes
                if (data.mEqualThreads)
                {
                    MPI_Gather(data.mLocalSevBuffer, num_threads, MPI_DOUBLE,
                               data.mGlobalSevBuffer, num_threads, MPI_DOUBLE,
                               0, MPI_COMM_WORLD);
                }
                else
                {
                    MPI_Gatherv(data.mLocalSevBuffer, num_threads, MPI_DOUBLE,
                                data.mGlobalSevBuffer, data.mRecvCounts, data.mRecvDispls, MPI_DOUBLE,
                                0, MPI_COMM_WORLD);
                }
                MPI_Barrier(MPI_COMM_WORLD);
            #else    // !_MPI
                memcpy(data.mGlobalSevBuffer, data.mLocalSevBuffer,
                       num_threads * sizeof(double));
            #endif    // !_MPI

            if (rank == 0)
            {
                // Write data to file
                cube_write_sev_row_of_doubles(data.cb, data.metrics[get_id()],
                                              data.cnodes[callpath->getId()],
                                              data.mGlobalSevBuffer);
            }
        }
    }

    // --- Release local & global severity buffers ---
    #pragma omp barrier
    #pragma omp master
    {
        delete[] cpath_order;
        delete[] global_use;
    }

    // Synchronize threads to avoid race conditions due to temporary
    // data fields freed above
    #pragma omp barrier
}


// --- Pattern management ---------------------------------------------------

/**
 *  @brief Initializes local data.
 *
 *  This method can be overwritten in derived classes to initialize local
 *  data. The default implementation is empty.
 **/
void
Pattern::init()
{
}


// --- Data collation control -----------------------------------------------

/**
 *  @brief Controls collation of data for the given call-tree node.
 *
 *  This method can be overwritten in derived classes to control the collation
 *  of severity data for the given call-tree node @a cnode. The method should
 *  return @em true if it is guaranteed that no severity data exists for the
 *  call-tree node (e.g., non-MPI call-tree nodes will always have a severity
 *  of zero for MPI-specific patterns). Otherwise, the method returns @em false
 *  (default).
 *
 *  @param  cnode  Current call-tree node
 *  @return Returns @em true if no severity data can exist, @em false otherwise
 **/
bool
Pattern::skip_cnode(const pearl::Callpath& cnode) const
{
    return false;
}
