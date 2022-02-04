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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declarations of EPIK callback functions for mapping file
 *           records.
 *
 *  This header file provides the declarations of EPIK callback functions
 *  and related data types for handling mapping file records.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_EPIKMAPCALLBACKS_H
#define PEARL_EPIKMAPCALLBACKS_H


#include <string>

#include <pearl/pearl_types.h>

#include "LocalIdMaps.h"
#include "elg_types.h"
#include "epk_idmap.h"


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @struct  EpikMapCbData
 *  @ingroup PEARL_base
 *  @brief   EPIK mapping file callback data
 *
 *  The EpikMapCbData struct combines all data elements which are necessary
 *  to process the mapping file records of an EPIK trace archive.
 **/
/*-------------------------------------------------------------------------*/

struct EpikMapCbData
    : public LocalIdMaps
{
    // --- Public methods ----------------------------------

    /// @name Constructors & destructor
    /// @{

    EpikMapCbData();

    ~EpikMapCbData();

    /// @}
    /// @name Mapping routines
    /// @{

    timestamp_t
    mapTimestamp(elg_d8 time) const;

    ident_t
    mapLocationId(elg_ui4 id) const;

    ident_t
    mapRegionId(elg_ui4 id) const;

    ident_t
    mapCallsiteId(elg_ui4 id) const;

    ident_t
    mapGroupId(elg_ui4 id) const;

    ident_t
    mapCommunicatorId(elg_ui4 id) const;

    ident_t
    mapWindowId(elg_ui4 id) const;

    /// @}


    // --- Data members ------------------------------------

    /// Stores the rank number found in the section header
    uint32_t mRank;

    /// Counts the number of offset records read
    int mOffsetCount;

    /// Local time at beginning and end of execution
    elg_d8 mLocalTime[2];

    /// Time offset at beginning and end of execution
    elg_d8 mTimeOffset[2];

    /// Mapping data: local location id |-@> global location id
    EpkIdMap* mLocationMap;

    /// Mapping data: local region id |-@> global region id
    EpkIdMap* mRegionMap;

    /// Mapping data: local call site id |-@> global call site id
    EpkIdMap* mCallsiteMap;

    /// Mapping data: local MPI group id |-@> global MPI group id
    EpkIdMap* mGroupMap;

    /// Mapping data: local MPI communicator id |-@> global MPI communicator id
    EpkIdMap* mCommunicatorMap;

    /// Mapping data: local RMA window id |-@> global RMA window id
    EpkIdMap* mWindowMap;

    /// %Error message of exceptions thrown (and caught) within callbacks
    std::string mErrorMessage;
};
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_EPIKMAPCALLBACKS_H
