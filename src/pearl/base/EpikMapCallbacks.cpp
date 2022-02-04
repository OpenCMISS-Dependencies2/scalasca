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
 *  @brief   Implementation of EPIK callback functions for mapping file
 *           records.
 *
 *  This file provides the implementation of EPIK callback functions for
 *  handling mapping file records.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "EpikMapCallbacks.h"

#include <pearl/Error.h>

#include "elg_readcb.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Local helper functions -----------------------------------------------

namespace
{
ident_t
find_id(const EpkIdMap* container,
        elg_ui4         id,
        const string&   desc)
{
    if (container == 0)
    {
        return id;
    }
    ident_t result = epk_idmap_get(container, id);
    if (result == ELG_NO_ID)
    {
        throw RuntimeError("EpikMapCbData::" + desc + " -- Unknown ID.");
    }

    return result;
}
}    // unnamed namespace


// --- Constructors & destructor --------------------------------------------

EpikMapCbData::EpikMapCbData()
    : LocalIdMaps(),
      mRank(PEARL_NO_ID),
      mOffsetCount(0),
      mLocationMap(0),
      mRegionMap(0),
      mCallsiteMap(0),
      mGroupMap(0),
      mCommunicatorMap(0),
      mWindowMap(0)
{
    // Initialize attributes
    mLocalTime[0]  = 0.0;
    mLocalTime[1]  = 1.0;
    mTimeOffset[0] = 0.0;
    mTimeOffset[1] = 0.0;
}


EpikMapCbData::~EpikMapCbData()
{
    // Release resources
    if (mLocationMap)
    {
        epk_idmap_free(mLocationMap);
    }
    if (mRegionMap)
    {
        epk_idmap_free(mRegionMap);
    }
    if (mCallsiteMap)
    {
        epk_idmap_free(mCallsiteMap);
    }
    if (mGroupMap)
    {
        epk_idmap_free(mGroupMap);
    }
    if (mCommunicatorMap)
    {
        epk_idmap_free(mCommunicatorMap);
    }
    if (mWindowMap)
    {
        epk_idmap_free(mWindowMap);
    }
}


// --- Mapping routines -----------------------------------------------------

timestamp_t
EpikMapCbData::mapTimestamp(elg_d8 time) const
{
    return time
           + (((mTimeOffset[1] - mTimeOffset[0])
               / (mLocalTime[1] - mLocalTime[0]))
              * (time - mLocalTime[0]))
           + mTimeOffset[0];
}


ident_t
EpikMapCbData::mapLocationId(elg_ui4 id) const
{
    return find_id(mLocationMap, id, "mapLocationId(elg_ui4)");
}


ident_t
EpikMapCbData::mapRegionId(elg_ui4 id) const
{
    return find_id(mRegionMap, id, "mapRegionId(elg_ui4)");
}


ident_t
EpikMapCbData::mapCallsiteId(elg_ui4 id) const
{
    return find_id(mCallsiteMap, id, "mapCallsiteId(elg_ui4)");
}


ident_t
EpikMapCbData::mapGroupId(elg_ui4 id) const
{
    return find_id(mGroupMap, id, "mapGroupId(elg_ui4)");
}


ident_t
EpikMapCbData::mapCommunicatorId(elg_ui4 id) const
{
    return find_id(mCommunicatorMap, id, "mapCommunicatorId(elg_ui4)");
}


ident_t
EpikMapCbData::mapWindowId(elg_ui4 id) const
{
    return find_id(mWindowMap, id, "mapWindowId(elg_ui4)");
}


// --- Local helper macros --------------------------------------------------

// *INDENT-OFF*    Uncrustify issues #2742 & #2791
#define CALLBACK_SETUP                                             \
    EpikMapCbData* data = static_cast< EpikMapCbData* >(userdata); \
    try                                                            \
    {

#define CALLBACK_CLEANUP                 \
    }    /* Closes the try block */      \
    catch (exception& ex)                \
    {                                    \
        data->mErrorMessage = ex.what(); \
    }
// *INDENT-ON*


// --- EPIK: Mapping callbacks ----------------------------------------------

void
elg_readcb_MAP_SECTION(elg_ui4 rank,    /* rank number the section relates to */
                       void*   userdata)
{
    CALLBACK_SETUP

    data->mRank = rank;

    CALLBACK_CLEANUP
}


void
elg_readcb_OFFSET(elg_d8 ltime,    /* local time */
                  elg_d8 offset,   /* offset to global time */
                  void*  userdata)
{
    CALLBACK_SETUP

    if (data->mOffsetCount > 1)
    {
        throw RuntimeError("Found more than two ELG_OFFSET records");
    }
    data->mLocalTime[data->mOffsetCount]  = ltime;
    data->mTimeOffset[data->mOffsetCount] = offset;
    data->mOffsetCount++;

    CALLBACK_CLEANUP
}


void
elg_readcb_IDMAP(elg_ui1 type,    /* object type to be mapped */
                 elg_ui1 mode,    /* mapping mode (dense/sparse) */
                 elg_ui4 count,   /* number of entries */
                 elg_ui4 mapv[],  /* vector of mappings */
                 void*   userdata)
{
    CALLBACK_SETUP

    // Determine map
    EpkIdMap** map;
    switch (type)
    {
        case ELG_LOCATION:
            map = &data->mLocationMap;
            break;

        case ELG_REGION:
            map = &data->mRegionMap;
            break;

        case ELG_CALL_SITE:
            map = &data->mCallsiteMap;
            break;

        case ELG_MPI_GROUP:
            map = &data->mGroupMap;
            break;

        case ELG_MPI_COMM:
            map = &data->mCommunicatorMap;
            break;

        case ELG_MPI_WIN:
            map = &data->mWindowMap;
            break;

        // Ignore call-path mapping for backwards compatibility
        case ELG_CALL_PATH:
            return;

        default:
            throw RuntimeError("Unknown table type");
    }
    if (*map)
    {
        throw RuntimeError("Table already defined");
    }
    if (mode == ELG_MAP_DENSE)
    {
        *map = epk_idmap_create(EPK_IDMAP_DENSE, count);
        for (elg_ui4 index = 0; index < count; ++index)
        {
            epk_idmap_add(*map, index, mapv[index]);
        }
    }
    else
    {
        // ELG_MAP_SPARSE
        *map = epk_idmap_create(EPK_IDMAP_SPARSE, count / 2);
        for (elg_ui4 index = 0; index < count; index += 2)
        {
            epk_idmap_add(*map, mapv[index], mapv[index + 1]);
        }
    }

    CALLBACK_CLEANUP
}
