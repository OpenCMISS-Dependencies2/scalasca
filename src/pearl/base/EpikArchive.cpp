/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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
 *  @brief   Implementation of the class EpikArchive.
 *
 *  This file provides the implementation of the class EpikArchive and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "EpikArchive.h"

#include <sys/stat.h>

#include <cassert>
#include <cstdlib>

#define SCALASCA_DEBUG_MODULE_NAME    EPIK
#include <UTILS_Debug.h>

#include <pearl/CommSet.h>
#include <pearl/Error.h>
#include <pearl/LocalTrace.h>
#include <pearl/ScopedPtr.h>
#include <pearl/Utils.h>

#include "DefsFactory.h"
#include "EpikDefCallbacks.h"
#include "EpikEvtCallbacks.h"
#include "EpikMapCallbacks.h"
#include "MutableGlobalDefs.h"
#include "elg_readcb.h"
#include "epk_archive.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

EpikArchive::EpikArchive(const string& archiveDir)
    : TraceArchive(archiveDir, archiveDir),
      mOmpThreadTeamOffset(0)
{
    // Sanity checks
    const char* name = archiveDir.c_str();
    if (!epk_archive_set_name(name))
    {
        throw RuntimeError("Experiment archive \"" + archiveDir
                           + "\" is not an EPIK archive.");
    }
    if (!epk_archive_exists(name))
    {
        throw RuntimeError("Experiment archive \"" + archiveDir
                           + "\"does not exist.");
    }
}


// --- Identifier mapping ---------------------------------------------------

void
EpikArchive::addCommSetIdMapping(const uint32_t        epkCommSetId,
                                 const CommSet::IdType intCommSetId)
{
    CommSetMap::iterator it = mCommSetMap.find(epkCommSetId);
    if (it != mCommSetMap.end())
    {
        throw FatalError("pearl::detail::EpikArchive::addCommSetIdMapping -- "
                         "Communication set ID mapping already defined: "
                         + toStdString(epkCommSetId));
    }
    mCommSetMap.insert(it, CommSetMap::value_type(epkCommSetId,
                                                  intCommSetId));
}


CommSet::IdType
EpikArchive::mapCommSetId(const uint32_t epkCommSetId) const
{
    CommSetMap::const_iterator it = mCommSetMap.find(epkCommSetId);
    if (it == mCommSetMap.end())
    {
        throw FatalError("pearl::detail::EpikArchive::mapCommSetId -- "
                         "Unknown identifier: "
                         + toStdString(epkCommSetId));
    }

    return it->second;
}


void
EpikArchive::addLocationIdMapping(const uint32_t         epkLocationId,
                                  const Location::IdType intLocationId)
{
    LocationMap::iterator it = mLocationMap.find(epkLocationId);
    if (it != mLocationMap.end())
    {
        throw FatalError("pearl::detail::EpikArchive::addLocationIdMapping -- "
                         "Location ID mapping already defined: "
                         + toStdString(epkLocationId));
    }
    mLocationMap.insert(it, LocationMap::value_type(epkLocationId,
                                                    intLocationId));
}


Location::IdType
EpikArchive::mapLocationId(const uint32_t epkLocationId) const
{
    LocationMap::const_iterator it = mLocationMap.find(epkLocationId);
    if (it == mLocationMap.end())
    {
        throw FatalError("pearl::detail::EpikArchive::mapLocationId -- "
                         "Unknown identifier: "
                         + toStdString(epkLocationId));
    }

    return it->second;
}


uint32_t
EpikArchive::getThreadTeamId(const uint32_t rank) const
{
    return (mOmpThreadTeamOffset + rank);
}


// --- Mapping file offset handling -----------------------------------------

void
EpikArchive::setFileOffset(const uint32_t rank,
                           const uint32_t offset)
{
    if (rank >= mFileOffsetMap.size())
    {
        mFileOffsetMap.resize(rank + 1);
    }
    mFileOffsetMap[rank] = offset;
}


uint32_t
EpikArchive::getFileOffset(const uint32_t rank) const
{
    if (rank >= mFileOffsetMap.size())
    {
        throw RuntimeError("EpikArchive::getFileOffset(uint32_t) -- Rank out of bounds");
    }

    return mFileOffsetMap[rank];
}


// --- Private member functions ---------------------------------------------

void
EpikArchive::openArchive()
{
}


void
EpikArchive::readDefinitions(GlobalDefs* defs)
{
    // Determine file name
    char*  tmp_str  = epk_archive_filename(EPK_TYPE_ESD, epk_archive_get_name());
    string filename = tmp_str;
    free(tmp_str);

    // Open definition file
    ElgRCB* file = elg_read_open(filename.c_str());
    if (!file)
    {
        throw RuntimeError("Cannot open EPILOG trace definition file \""
                           + filename + "\".");
    }

    // Read data
    MutableGlobalDefs* mutableDefs = dynamic_cast< MutableGlobalDefs* >(defs);
    assert(mutableDefs);
    EpikDefCbData data(*this, *mutableDefs);
    while (elg_read_next_def(file, &data))
    {
        // Check for errors
        if (!data.mErrorMessage.empty())
        {
            break;
        }
    }
    elg_read_close(file);

    // Check for errors
    if (!data.mErrorMessage.empty())
    {
        throw Error(data.mErrorMessage);
    }

    if (data.mIncludesOpenMP)
    {
        map< Location::IdType, uint32_t > globalRanks;
        vector< uint64_t >                members;

        // Reconstruct OpenMP communication location set
        const size_t numMembers = defs->numLocations();
        members.reserve(numMembers);

        // NOTE: EPIK traces only contain location groups of type PROCESS
        const size_t numLocationGroups = defs->numLocationGroups();
        for (size_t lgIndex = 0; lgIndex < numLocationGroups; ++lgIndex)
        {
            const LocationGroup& group = defs->getLocationGroupByIndex(lgIndex);

            const size_t numLocations = group.numLocations();
            for (size_t locIndex = 0; locIndex < numLocations; ++locIndex)
            {
                const Location::IdType id = group.getLocation(locIndex).getId();

                globalRanks[id] = members.size();
                members.push_back(id);
            }
        }

        DefsFactory::instance()->createGroupingSet(*mutableDefs,
                                                   defs->numGroupingSets(),
                                                   String::NO_ID,
                                                   GroupingSet::COMM_LOCATION_SET,
                                                   Paradigm::OPENMP,
                                                   GroupingSet::PROPERTY_NONE,
                                                   numMembers,
                                                   &members[0]);
        members.clear();

        // Reconstruct communication sets for OpenMP thread teams
        // NOTE: EPIK traces only contain location groups of type PROCESS,
        //       listed in rank order
        mOmpThreadTeamOffset = defs->numCommunicators();
        for (size_t lgIndex = 0; lgIndex < numLocationGroups; ++lgIndex)
        {
            const LocationGroup& group = defs->getLocationGroupByIndex(lgIndex);

            const size_t numLocations = group.numLocations();
            members.reserve(numLocations);
            for (size_t locIndex = 0; locIndex < numLocations; ++locIndex)
            {
                const Location::IdType id = group.getLocation(locIndex).getId();

                members.push_back(globalRanks[id]);
            }

            CommSet::IdType commSetId = defs->numGroupingSets();
            DefsFactory::instance()->createGroupingSet(*mutableDefs,
                                                       commSetId,
                                                       String::NO_ID,
                                                       GroupingSet::COMM_SET,
                                                       Paradigm::OPENMP,
                                                       GroupingSet::PROPERTY_NONE,
                                                       numLocations,
                                                       &members[0]);
            DefsFactory::instance()->createCommunicator(*mutableDefs,
                                                        defs->numCommunicators(),
                                                        String::NO_ID,
                                                        commSetId,
                                                        Communicator::NO_ID);
            members.clear();
        }
    }

    if (!data.mUserTopoRankMaps.empty())
    {
        // Reconstruct hardware/user topology communication location set:
        // The EPIK definition data does not allow to distinguish between
        // hardware and user topologies.  Thus, we only create a single set
        // with paradigm USER covering all locations.
        const size_t       numLocations = defs->numLocations();
        vector< uint64_t > members;
        members.reserve(numLocations);
        for (uint32_t locId = 0; locId < numLocations; ++locId)
        {
            members.push_back(mapLocationId(locId));
        }

        DefsFactory::instance()->createGroupingSet(*mutableDefs,
                                                   defs->numGroupingSets(),
                                                   String::NO_ID,
                                                   GroupingSet::COMM_LOCATION_SET,
                                                   Paradigm::USER,
                                                   GroupingSet::PROPERTY_NONE,
                                                   numLocations,
                                                   &members[0]);
        members.clear();

        // Reconstruct hardware/user topology communication sets and communicators
        map< ident_t, map< ident_t, uint32_t > >::iterator topoIt =
            data.mUserTopoRankMaps.begin();
        while (topoIt != data.mUserTopoRankMaps.end())
        {
            map< ident_t, uint32_t >& rankMap = topoIt->second;

            members.resize(rankMap.size());
            map< ident_t, uint32_t >::iterator rankIt = rankMap.begin();
            while (rankIt != rankMap.end())
            {
                members[rankIt->second] = rankIt->first;

                ++rankIt;
            }

            CommSet::IdType commSetId = defs->numGroupingSets();
            DefsFactory::instance()->createGroupingSet(*mutableDefs,
                                                       commSetId,
                                                       String::NO_ID,
                                                       GroupingSet::COMM_SET,
                                                       Paradigm::USER,
                                                       GroupingSet::PROPERTY_NONE,
                                                       rankMap.size(),
                                                       &members[0]);
            Communicator::IdType commId = defs->numCommunicators();
            DefsFactory::instance()->createCommunicator(*mutableDefs,
                                                        commId,
                                                        String::NO_ID,
                                                        commSetId,
                                                        Communicator::NO_ID);

            mutableDefs->assignCartCommunicator(topoIt->first,
                                                defs->getCommunicator(commId));

            members.clear();
            ++topoIt;
        }
    }
}


void
EpikArchive::openContainer(const LocationGroup& locGroup)
{
}


void
EpikArchive::closeContainer()
{
}


LocalIdMaps*
EpikArchive::readIdMaps(const Location& location)
{
    // Determine filename
    char*  tmp_str  = epk_archive_filename(EPK_TYPE_EMP, getAnchorName().c_str());
    string filename = tmp_str;
    free(tmp_str);

    // Create mapping data object
    ScopedPtr< EpikMapCbData > mapData(new EpikMapCbData);

    // Check whether mapping file exists
    struct stat sbuf;
    if (stat(filename.c_str(), &sbuf) != 0)
    {
        UTILS_DEBUG("No mapping file found -- Assuming already globalized trace");

        return mapData.release();
    }

    uint32_t rank   = location.getRank();
    uint32_t offset = 0;
    if (!mFileOffsetMap.empty())
    {
        offset = getFileOffset(rank);
    }

    // Open mapping file
    ElgRCB* file = elg_read_open(filename.c_str());
    if (!file)
    {
        throw RuntimeError("Cannot open EPILOG mapping file \""
                           + filename + "\".");
    }

    // Read header
    elg_read_seek(file, offset);
    elg_read_next_def(file, mapData.get());
    if (mapData->mRank != rank)
    {
        elg_read_close(file);
        throw FatalError("Found unrelated mapping file section");
    }

    // Read data in section
    while (  elg_read_next_def(file, mapData.get())
          && (mapData->mRank == rank))
    {
        // Check for errors
        if (!mapData->mErrorMessage.empty())
        {
            break;
        }
    }
    elg_read_close(file);

    // Check for errors
    if (!mapData->mErrorMessage.empty())
    {
        throw Error(mapData->mErrorMessage);
    }

    return mapData.release();
}


void
EpikArchive::readTrace(const GlobalDefs& defs,
                       const Location&   location,
                       LocalIdMaps*      idMaps,
                       LocalTrace*       trace)
{
    EpikMapCbData* mapData = static_cast< EpikMapCbData* >(idMaps);

    // Determine file name
    uint32_t rank    = location.getRank();
    char*    tmp_str = epk_archive_rankname(EPK_TYPE_ELG,
                                            getAnchorName().c_str(), rank);
    string filename = tmp_str;
    free(tmp_str);

    // Open local event trace file
    ElgRCB* file = elg_read_open(filename.c_str());
    if (!file)
    {
        throw RuntimeError("Cannot open EPILOG event trace file \""
                           + filename + "\".");
    }

    // Read trace data
    EpikEvtCbData data(*this, defs, *trace, *mapData, elg_read_version(file),
                       location.getId(), location.getThreadId());
    while (elg_read_next_event(file, &data))
    {
        // Check for errors
        if (!data.mErrorMessage.empty())
        {
            break;
        }
    }
    elg_read_close(file);

    // Check for errors
    if (!data.mErrorMessage.empty())
    {
        throw Error(data.mErrorMessage);
    }

    // Consistency check
    if (!data.mCallstack.empty())
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many ENTERs).");
    }
}
