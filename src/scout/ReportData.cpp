/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2014-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "ReportData.h"

#include <cstring>

using namespace std;
using namespace pearl;
using namespace scout;


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Implementation of the ReportData struct.
 *
 *  This file provides the implementation of the member functions defined
 *  for the ReportData struct.
 **/
/*-------------------------------------------------------------------------*/


ReportData::ReportData(const GlobalDefs& defs,
                       int               rank)
    : cb(0),
      mLocalSevBuffer(0),
      mGlobalSevBuffer(0),
      mRecvCounts(0),
      mRecvDispls(0),
      mEqualThreads(true)
{
    // This variable is implicitly shared!
    static double* localSevBuffer = 0;

    // Initialize collation arrays
    #pragma omp master
    {
        // Allocate process-local severity buffer
        // FIXME: deal with non-CPU locations / non-process groups
        const uint32_t numThreads = defs.getLocationGroup(rank).numLocations();
        localSevBuffer = new double[numThreads];
        memset(localSevBuffer, 0, numThreads * sizeof(double));

        // Allocate and initialize global collation data structures
        // (only relevant on rank 0)
        const uint32_t numRanks = defs.numLocationGroups();
        if (rank == 0)
        {
            // Global severity buffer
            uint32_t totalThreads = defs.numLocations();
            mGlobalSevBuffer = new double[totalThreads];
            memset(mGlobalSevBuffer, 0, totalThreads * sizeof(double));

            // Gatherv receive counts and displacements
            mRecvCounts = new int[numRanks];
            mRecvDispls = new int[numRanks];

            // Determine whether all processes used the same number of threads
            // and initialize gatherv arrays
            int displacement = 0;
            for (uint32_t i = 0; i < numRanks; ++i)
            {
                const LocationGroup& lgroup  = defs.getLocationGroup(i);
                const uint32_t       threads = lgroup.numLocations();

                mEqualThreads = mEqualThreads && (threads == numThreads);

                mRecvCounts[i] = threads;
                mRecvDispls[i] = displacement;

                displacement += threads;
            }
        }
        else
        {
            // Determine whether all processes used the same number of threads
            for (uint32_t i = 0; i < numRanks; ++i)
            {
                const LocationGroup& lgroup  = defs.getLocationGroup(i);
                const uint32_t       threads = lgroup.numLocations();

                mEqualThreads = mEqualThreads && (threads == numThreads);
            }
        }
    }

    // Share process-local severity buffer across all threads
    #pragma omp barrier
    mLocalSevBuffer = localSevBuffer;
}


ReportData::~ReportData()
{
    #pragma omp master
    {
        delete[] mLocalSevBuffer;
        delete[] mGlobalSevBuffer;
        delete[] mRecvCounts;
        delete[] mRecvDispls;
    }
}
