/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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


#ifndef SCOUT_REPORTDATA_H
#define SCOUT_REPORTDATA_H


#include <map>
#include <vector>

#include <cubew_cube.h>

#include <pearl/GlobalDefs.h>
#include <pearl/Location.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Declaration of the ReportData struct.
 *
 *  This header file provides the declaration of the ReportData struct which
 *  stores the data required for generating the final analysis report.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 *  @struct ReportData
 *  @brief  Datatype aggregating all data required for generating the final
 *          analysis report.
 *
 *  The ReportData struct provides a datatype which aggregates a number of
 *  data structures required to produce the final analysis report. In
 *  particular, these are mappings from SCOUT-internal object identifiers
 *  to the corresponding CUBE objects. In addition, a ReportData instance
 *  includes a number of arrays which are used while collating the report
 *  data on the master process.
 **/
/*-------------------------------------------------------------------------*/

struct ReportData
{
    // --- Public methods ----------------------------------

    /// @name Constructors & destructor
    /// @{

    /// @brief Constructor.
    ///
    /// Creates a new instance and initializes the data collation arrays
    /// according to the process/thread configuration from the global
    /// definitions object.
    ///
    /// @param  defs  Global defintions object
    /// @param  rank  MPI rank
    ///
    ReportData(const pearl::GlobalDefs& defs,
               int                      rank);

    /// @brief Destructor.
    ///
    /// Releases all allocated resources and destroys the instance.
    ///
    ~ReportData();

    /// @}


    // --- Data members ------------------------------------

    /// CUBE output object
    cube_t* cb;

    /// Location ID to CUBE thread mapping
    std::map< pearl::Location::IdType, cube_location* > locs;

    /// Region ID to CUBE region mapping
    std::vector< cube_region* > regions;

    /// Call-tree node to CUBE call-tree node mapping
    std::vector< cube_cnode* > cnodes;

    /// Pattern ID to CUBE metric mapping
    std::map< long, cube_metric* > metrics;

    /// Process-local severity buffer; shared across all threads
    double* mLocalSevBuffer;

    /// Global severity buffer; only relevant on MPI rank 0
    double* mGlobalSevBuffer;

    /// Global gatherv receive counts; only relevant on MPI rank 0
    int* mRecvCounts;

    /// Global gatherv receive displacements; only relevant on MPI rank 0
    int* mRecvDispls;

    /// Flag indicating whether all MPI ranks use the same number of threads
    bool mEqualThreads;
};
}    // namespace scout


#endif    // !SCOUT_REPORTDATA_H
