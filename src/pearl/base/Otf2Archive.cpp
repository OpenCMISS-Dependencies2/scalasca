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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class Otf2Archive.
 *
 *  This file provides the implementation of the class Otf2Archive and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "Otf2Archive.h"

#include <cassert>

#include <pearl/Error.h>

#include "LocalIdMaps.h"
#include "MutableGlobalDefs.h"
#include "Otf2CollCallbacks.h"
#include "Otf2DefCallbacks.h"
#include "Otf2EvtCallbacks.h"
#include "Otf2LockCallbacks.h"
#include "pearl_ipc.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Local helper macros --------------------------------------------------

#define SET_DEF_CALLBACK(name)                                                                        \
    do                                                                                                \
    {                                                                                                 \
        OTF2_ErrorCode result;                                                                        \
        result = OTF2_GlobalDefReaderCallbacks_Set ## name ## Callback(callbacks, otf2DefCb ## name); \
        if (result != OTF2_SUCCESS)                                                                   \
        {                                                                                             \
            throw RuntimeError("Error setting OTF2 " #name " definition callback!");                  \
        }                                                                                             \
    }                                                                                                 \
    while (0)

#define SET_EVT_CALLBACK(name)                                                                  \
    do                                                                                          \
    {                                                                                           \
        OTF2_ErrorCode result;                                                                  \
        result = OTF2_EvtReaderCallbacks_Set ## name ## Callback(callbacks, otf2EvtCb ## name); \
        if (result != OTF2_SUCCESS)                                                             \
        {                                                                                       \
            throw RuntimeError("Error setting OTF2 " #name " event callback!");                 \
        }                                                                                       \
    }                                                                                           \
    while (0)


// --- Constructors & destructor --------------------------------------------

Otf2Archive::Otf2Archive(const string& anchorName,
                         const string& archiveDir)
    : TraceArchive(anchorName, archiveDir),
      mArchiveReader(0),
      mTimerResolution(0),
      mGlobalOffset(0)
{
}


Otf2Archive::~Otf2Archive()
{
    if (mArchiveReader)
    {
        // Intentionally ignoring error code here since destructors should
        // *never* throw exceptions.
        OTF2_Reader_Close(mArchiveReader);
    }
}


// --- Private member functions ---------------------------------------------

void
Otf2Archive::openArchive()
{
    mArchiveReader = OTF2_Reader_Open(getAnchorName().c_str());
    if (!mArchiveReader)
    {
        throw RuntimeError("Error opening OTF2 experiment archive \""
                           + getAnchorName() + "\"!");
    }

    // Set locking callbacks
    static const OTF2_LockingCallbacks lockCbs = {
        0,                              // .otf2_release
        pearl::detail::otf2LockCreate,  // .otf2_create
        pearl::detail::otf2LockDestroy, // .otf2_destroy
        pearl::detail::otf2LockAcquire, // .otf2_lock
        pearl::detail::otf2LockRelease  // .otf2_unlock
    };

    OTF2_ErrorCode result;
    result = OTF2_Reader_SetLockingCallbacks(mArchiveReader, &lockCbs, 0);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error setting OTF2 locking callbacks!");
    }

    // Check OTF2 format version
    uint8_t major;
    uint8_t minor;
    uint8_t bugfix;
    result = OTF2_Reader_GetVersion(mArchiveReader, &major, &minor, &bugfix);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error determining OTF2 format version!");
    }
    if ((major == 1) && (minor < 2))
    {
        throw RuntimeError("OTF2 format version <1.2 not supported!");
    }
}


void
Otf2Archive::readDefinitions(GlobalDefs* defs)
{
    // Create definition reader
    OTF2_GlobalDefReader* defReader;
    defReader = OTF2_Reader_GetGlobalDefReader(mArchiveReader);
    if (!defReader)
    {
        throw RuntimeError("Error creating OTF2 global definition reader!");
    }

    // Create callback struct
    OTF2_GlobalDefReaderCallbacks* callbacks;
    callbacks = OTF2_GlobalDefReaderCallbacks_New();
    if (!callbacks)
    {
        throw RuntimeError("Error allocating OTF2 definition callback struct!");
    }

    // Fill callback struct
    SET_DEF_CALLBACK(Attribute);
    SET_DEF_CALLBACK(CallingContext);
    SET_DEF_CALLBACK(CallingContextProperty);
    SET_DEF_CALLBACK(Callpath);
    SET_DEF_CALLBACK(Callsite);
    SET_DEF_CALLBACK(CartCoordinate);
    SET_DEF_CALLBACK(CartDimension);
    SET_DEF_CALLBACK(CartTopology);
    SET_DEF_CALLBACK(ClockProperties);
    SET_DEF_CALLBACK(Comm);
    SET_DEF_CALLBACK(Group);
    SET_DEF_CALLBACK(InterruptGenerator);
    SET_DEF_CALLBACK(Location);
    SET_DEF_CALLBACK(LocationGroup);
    SET_DEF_CALLBACK(LocationGroupProperty);
    SET_DEF_CALLBACK(LocationProperty);
    SET_DEF_CALLBACK(MetricClass);
    SET_DEF_CALLBACK(MetricClassRecorder);
    SET_DEF_CALLBACK(MetricInstance);
    SET_DEF_CALLBACK(MetricMember);
    SET_DEF_CALLBACK(Paradigm);
    SET_DEF_CALLBACK(ParadigmProperty);
    SET_DEF_CALLBACK(Parameter);
    SET_DEF_CALLBACK(Region);
    SET_DEF_CALLBACK(RmaWin);
    SET_DEF_CALLBACK(SourceCodeLocation);
    SET_DEF_CALLBACK(String);
    SET_DEF_CALLBACK(SystemTreeNode);
    SET_DEF_CALLBACK(SystemTreeNodeDomain);
    SET_DEF_CALLBACK(SystemTreeNodeProperty);

    // Install callbacks
    MutableGlobalDefs* mutableDefs = dynamic_cast< MutableGlobalDefs* >(defs);
    assert(mutableDefs);
    Otf2DefCbData  cbData(*mutableDefs);
    OTF2_ErrorCode result;
    result = OTF2_GlobalDefReader_SetCallbacks(defReader, callbacks, &cbData);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error setting OTF2 definition callbacks!");
    }
    OTF2_GlobalDefReaderCallbacks_Delete(callbacks);

    // Read definition data
    uint64_t recordsRead;
    result = OTF2_GlobalDefReader_ReadDefinitions(defReader, UINT64_MAX, &recordsRead);
    if (  (result != OTF2_SUCCESS)
       || !cbData.mErrorMessage.empty())
    {
        throw RuntimeError(cbData.mErrorMessage);
    }

    // Store timer data
    mTimerResolution = cbData.mTimerResolution;
    mGlobalOffset    = cbData.mGlobalOffset;

    // Close definition reader
    result = OTF2_Reader_CloseGlobalDefReader(mArchiveReader, defReader);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error closing OTF2 global definition reader!");
    }

    // For OTF2 traces, global time offset subtraction is implicitly
    // applied during trace reading
    mutableDefs->setGlobalOffset(0.0);
}


void
Otf2Archive::openContainer(const LocationGroup& locGroup)
{
    static const OTF2_CollectiveCallbacks collCbs = {
        pearl::detail::otf2CollCbRelease,         // .otf2_release
        pearl::detail::otf2CollCbGetSize,         // .otf2_get_size
        pearl::detail::otf2CollCbGetRank,         // .otf2_get_rank
        pearl::detail::otf2CollCbCreateLocalComm, // .otf2_create_local_comm
        pearl::detail::otf2CollCbFreeLocalComm,   // .otf2_free_local_comm
        pearl::detail::otf2CollCbBarrier,         // .otf2_barrier
        pearl::detail::otf2CollCbBroadcast,       // .otf2_bcast
        pearl::detail::otf2CollCbGather,          // .otf2_gather
        pearl::detail::otf2CollCbGatherv,         // .otf2_gatherv
        pearl::detail::otf2CollCbScatter,         // .otf2_scatter
        pearl::detail::otf2CollCbScatterv         // .otf2_scatterv
    };

    // Install collective callbacks
    OTF2_ErrorCode result;
    result = OTF2_Reader_SetCollectiveCallbacks(mArchiveReader, &collCbs, 0,
                                                otf2CreateCollectiveContext(ipcCommWorld),
                                                0);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error setting OTF2 collective callbacks!");
    }

    // Select locations
    uint32_t count = locGroup.numLocations();
    for (uint32_t index = 0; index < count; ++index)
    {
        const Location& location = locGroup.getLocation(index);

        result = OTF2_Reader_SelectLocation(mArchiveReader, location.getId());
        if (result != OTF2_SUCCESS)
        {
            throw RuntimeError("Error selecting location for reading!");
        }
    }

    // Open containers
    result = OTF2_Reader_OpenDefFiles(mArchiveReader);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error opening OTF2 local definition container!");
    }
    result = OTF2_Reader_OpenEvtFiles(mArchiveReader);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error opening OTF2 trace data container!");
    }
}


void
Otf2Archive::closeContainer()
{
    // Close containers
    OTF2_ErrorCode result;
    result = OTF2_Reader_CloseEvtFiles(mArchiveReader);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error closing trace data container!");
    }
    result = OTF2_Reader_CloseDefFiles(mArchiveReader);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error closing local definition container!");
    }
}


/// @todo Suppress error messages thrown by OTF2 if optional local definition
///       file could not be found
LocalIdMaps*
Otf2Archive::readIdMaps(const Location& location)
{
    // Create local definition file reader
    OTF2_DefReader* defReader = OTF2_Reader_GetDefReader(mArchiveReader, location.getId());

    // NOTE:
    // The creation of the local definition reader may fail since local
    // definitions are optional. We therefore do not raise an exception
    // in this case.

    // Read local definition data
    if (defReader)
    {
        uint64_t       recordsRead;
        OTF2_ErrorCode result;
        result = OTF2_DefReader_ReadDefinitions(defReader, UINT64_MAX,
                                                &recordsRead);
        OTF2_Reader_CloseDefReader(mArchiveReader, defReader);
        if (result != OTF2_SUCCESS)
        {
            throw RuntimeError("Error reading OTF2 local definition data!");
        }
    }

    // Return dummy mapping data object
    return new LocalIdMaps;
}


void
Otf2Archive::readTrace(const GlobalDefs& defs,
                       const Location&   location,
                       LocalIdMaps*      idMaps,
                       LocalTrace*       trace)
{
    // Create event trace reader
    OTF2_EvtReader* evtReader = OTF2_Reader_GetEvtReader(mArchiveReader, location.getId());
    if (!evtReader)
    {
        throw RuntimeError("Error creating OTF2 local trace reader!");
    }

    // Create callback struct
    OTF2_EvtReaderCallbacks* callbacks = OTF2_EvtReaderCallbacks_New();
    if (!callbacks)
    {
        throw RuntimeError("Error allocating OTF2 event callback struct!");
    }

    // Fill callback struct
    SET_EVT_CALLBACK(BufferFlush);
    SET_EVT_CALLBACK(CallingContextEnter);
    SET_EVT_CALLBACK(CallingContextLeave);
    SET_EVT_CALLBACK(CallingContextSample);
    SET_EVT_CALLBACK(Enter);
    SET_EVT_CALLBACK(Leave);
    SET_EVT_CALLBACK(MeasurementOnOff);
    SET_EVT_CALLBACK(Metric);
    SET_EVT_CALLBACK(MpiCollectiveBegin);
    SET_EVT_CALLBACK(MpiCollectiveEnd);
    SET_EVT_CALLBACK(MpiIrecv);
    SET_EVT_CALLBACK(MpiIrecvRequest);
    SET_EVT_CALLBACK(MpiIsend);
    SET_EVT_CALLBACK(MpiIsendComplete);
    SET_EVT_CALLBACK(MpiRecv);
    SET_EVT_CALLBACK(MpiRequestCancelled);
    SET_EVT_CALLBACK(MpiRequestTest);
    SET_EVT_CALLBACK(MpiSend);
    SET_EVT_CALLBACK(ParameterInt);
    SET_EVT_CALLBACK(ParameterString);
    SET_EVT_CALLBACK(ParameterUnsignedInt);
    SET_EVT_CALLBACK(ProgramBegin);
    SET_EVT_CALLBACK(ProgramEnd);
    SET_EVT_CALLBACK(RmaAcquireLock);
    SET_EVT_CALLBACK(RmaAtomic);
    SET_EVT_CALLBACK(RmaCollectiveBegin);
    SET_EVT_CALLBACK(RmaCollectiveEnd);
    SET_EVT_CALLBACK(RmaGet);
    SET_EVT_CALLBACK(RmaGroupSync);
    SET_EVT_CALLBACK(RmaOpCompleteBlocking);
    SET_EVT_CALLBACK(RmaOpCompleteNonBlocking);
    SET_EVT_CALLBACK(RmaOpCompleteRemote);
    SET_EVT_CALLBACK(RmaOpTest);
    SET_EVT_CALLBACK(RmaPut);
    SET_EVT_CALLBACK(RmaReleaseLock);
    SET_EVT_CALLBACK(RmaRequestLock);
    SET_EVT_CALLBACK(RmaSync);
    SET_EVT_CALLBACK(RmaTryLock);
    SET_EVT_CALLBACK(RmaWaitChange);
    SET_EVT_CALLBACK(RmaWinCreate);
    SET_EVT_CALLBACK(RmaWinDestroy);
    SET_EVT_CALLBACK(ThreadAcquireLock);
    SET_EVT_CALLBACK(ThreadBegin);
    SET_EVT_CALLBACK(ThreadCreate);
    SET_EVT_CALLBACK(ThreadEnd);
    SET_EVT_CALLBACK(ThreadFork);
    SET_EVT_CALLBACK(ThreadJoin);
    SET_EVT_CALLBACK(ThreadReleaseLock);
    SET_EVT_CALLBACK(ThreadTaskComplete);
    SET_EVT_CALLBACK(ThreadTaskCreate);
    SET_EVT_CALLBACK(ThreadTaskSwitch);
    SET_EVT_CALLBACK(ThreadTeamBegin);
    SET_EVT_CALLBACK(ThreadTeamEnd);
    SET_EVT_CALLBACK(ThreadWait);

    // Install callbacks
    Otf2EvtCbData  cbData(defs, *trace, mTimerResolution, mGlobalOffset);
    OTF2_ErrorCode result;
    result = OTF2_EvtReader_SetCallbacks(evtReader, callbacks, &cbData);
    if (result != OTF2_SUCCESS)
    {
        throw RuntimeError("Error setting OTF2 event callbacks!");
    }
    OTF2_EvtReaderCallbacks_Delete(callbacks);

    // Read event trace data
    uint64_t recordsRead;
    result = OTF2_EvtReader_ReadEvents(evtReader, UINT64_MAX, &recordsRead);
    if (  (result != OTF2_SUCCESS)
       || !cbData.mErrorMessage.empty())
    {
        throw RuntimeError(cbData.mErrorMessage);
    }
    OTF2_Reader_CloseEvtReader(mArchiveReader, evtReader);
}
