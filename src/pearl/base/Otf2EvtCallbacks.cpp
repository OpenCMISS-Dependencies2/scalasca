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
 *  @brief   Implementation of OTF2 callback functions for event records.
 *
 *  This file provides the implementation of OTF2 callback functions for
 *  handling event records.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "Otf2EvtCallbacks.h"

#include <string>

#include <UTILS_Error.h>
#include <UTILS_IO.h>

#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/Region.h>

#include "EventFactory.h"
#include "Otf2TypeHelpers.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Local helper functions -----------------------------------------------

namespace
{
CallingContext*
getContext(const GlobalDefs&      defs,
           OTF2_CallingContextRef contextId);

void
processContext(const Otf2EvtCbData* data,
               timestamp_t          time,
               CallingContext*      context,
               uint32_t             distance);
}    // unnamed namespace


// --- Constructors & destructor --------------------------------------------

/// @todo
///     Find a more elegant/explicit solution to clean up the call stack
///     at the end of a CCT trace.
Otf2EvtCbData::~Otf2EvtCbData()
{
    // "Close" call stack after last sample of CCT traces
    if (mPreviousContext != 0)
    {
        processContext(this, mTrace.end().prev()->getTimestamp(), 0, 1);
    }
}


// --- Local helper macros --------------------------------------------------

// *INDENT-OFF*    Uncrustify issues #2742 & #2791
#define CALLBACK_SETUP                                                 \
    Otf2EvtCbData*    data  = static_cast< Otf2EvtCbData* >(userData); \
    const GlobalDefs& defs  = data->mDefinitions;                      \
    LocalTrace&       trace = data->mTrace;                            \
    try                                                                \
    {

#define CALLBACK_CLEANUP                 \
    }    /* Closes the try block */      \
    catch (std::exception& ex)           \
    {                                    \
        data->mErrorMessage = ex.what(); \
                                         \
        return OTF2_CALLBACK_INTERRUPT;  \
    }
// *INDENT-ON*


// --- OTF2: Event callbacks ------------------------------------------------

OTF2_CallbackCode
pearl::detail::otf2EvtCbBufferFlush(const OTF2_LocationRef    location,
                                    const OTF2_TimeStamp      time,
                                    const uint64_t            eventPosition,
                                    void* const               userData,
                                    OTF2_AttributeList* const attributeList,
                                    const OTF2_TimeStamp      stopTime)
{
    CALLBACK_SETUP
    Region::IdType regionId = defs.getFlushingRegion().getId();
    Event_rep*     event    =
        EventFactory::instance()->createEnter(defs,
                                              otf2ConvertTime(time, data),
                                              regionId,
                                              NULL);
    if (event)
    {
        trace.add_event(event);
    }
    event =
        EventFactory::instance()->createLeave(defs,
                                              otf2ConvertTime(stopTime, data),
                                              regionId,
                                              NULL);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbCallingContextEnter(const OTF2_LocationRef       location,
                                            const OTF2_TimeStamp         time,
                                            const uint64_t               eventPosition,
                                            void* const                  userData,
                                            OTF2_AttributeList* const    attributeList,
                                            const OTF2_CallingContextRef callingContext,
                                            const uint32_t               unwindDistance)
{
    CALLBACK_SETUP
    CallingContext* context = getContext(defs, callingContext);
    if (context != 0)
    {
        const timestamp_t timestamp = otf2ConvertTime(time, data);
        processContext(data, timestamp, context, unwindDistance);
        data->mPreviousContext = context;
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbCallingContextLeave(const OTF2_LocationRef       location,
                                            const OTF2_TimeStamp         time,
                                            const uint64_t               eventPosition,
                                            void* const                  userData,
                                            OTF2_AttributeList* const    attributeList,
                                            const OTF2_CallingContextRef callingContext)
{
    CALLBACK_SETUP
    CallingContext* context = getContext(defs, callingContext);
    if (context != 0)
    {
        const timestamp_t timestamp = otf2ConvertTime(time, data);
        processContext(data, timestamp, context, 1);

        Event_rep* event =
            EventFactory::instance()->createLeave(data->mDefinitions,
                                                  timestamp,
                                                  context->getRegion().getId(),
                                                  NULL);
        if (event)
        {
            data->mTrace.add_event(event);
        }

        data->mPreviousContext = context->getParent();
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbCallingContextSample(const OTF2_LocationRef           location,
                                             const OTF2_TimeStamp             time,
                                             const uint64_t                   eventPosition,
                                             void* const                      userData,
                                             OTF2_AttributeList* const        attributeList,
                                             const OTF2_CallingContextRef     callingContext,
                                             const uint32_t                   unwindDistance,
                                             const OTF2_InterruptGeneratorRef interruptGenerator)
{
    CALLBACK_SETUP
    CallingContext* context = getContext(defs, callingContext);
    if (context != 0)
    {
        const timestamp_t timestamp = otf2ConvertTime(time, data);
        processContext(data, timestamp, context, unwindDistance);
        data->mPreviousContext = context;
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Handle HWC metrics
OTF2_CallbackCode
pearl::detail::otf2EvtCbEnter(const OTF2_LocationRef    location,
                              const OTF2_TimeStamp      time,
                              const uint64_t            eventPosition,
                              void* const               userData,
                              OTF2_AttributeList* const attributeList,
                              const OTF2_RegionRef      region)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createEnter(defs,
                                              otf2ConvertTime(time, data),
                                              region,
                                              NULL);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Handle HWC metrics
OTF2_CallbackCode
pearl::detail::otf2EvtCbLeave(const OTF2_LocationRef    location,
                              const OTF2_TimeStamp      time,
                              const uint64_t            eventPosition,
                              void* const               userData,
                              OTF2_AttributeList* const attributeList,
                              const OTF2_RegionRef      region)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createLeave(defs,
                                              otf2ConvertTime(time, data),
                                              region,
                                              NULL);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMeasurementOnOff(const OTF2_LocationRef     location,
                                         const OTF2_TimeStamp       time,
                                         const uint64_t             eventPosition,
                                         void* const                userData,
                                         OTF2_AttributeList* const  attributeList,
                                         const OTF2_MeasurementMode measurementMode)
{
    CALLBACK_SETUP

    Region::IdType regionId = defs.getPausingRegion().getId();
    Event_rep*     event;
    if (measurementMode == OTF2_MEASUREMENT_OFF)
    {
        event =
            EventFactory::instance()->createEnter(defs,
                                                  otf2ConvertTime(time, data),
                                                  regionId,
                                                  NULL);
    }
    else
    {
        event =
            EventFactory::instance()->createLeave(defs,
                                                  otf2ConvertTime(time, data),
                                                  regionId,
                                                  NULL);
    }
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; Metric events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbMetric(const OTF2_LocationRef        location,
                               const OTF2_TimeStamp          time,
                               const uint64_t                eventPosition,
                               void* const                   userData,
                               OTF2_AttributeList* const     attributeList,
                               const OTF2_MetricRef          metric,
                               const uint8_t                 numberOfMetrics,
                               const OTF2_Type* const        typeIDs,
                               const OTF2_MetricValue* const metricValues)
{
    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiCollectiveBegin(const OTF2_LocationRef    location,
                                           const OTF2_TimeStamp      time,
                                           const uint64_t            eventPosition,
                                           void* const               userData,
                                           OTF2_AttributeList* const attributeList)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createMpiCollBegin(defs,
                                                     otf2ConvertTime(time, data));
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiCollectiveEnd(const OTF2_LocationRef    location,
                                         const OTF2_TimeStamp      time,
                                         const uint64_t            eventPosition,
                                         void* const               userData,
                                         OTF2_AttributeList* const attributeList,
                                         const OTF2_CollectiveOp   collectiveOp,
                                         const OTF2_CommRef        communicator,
                                         const uint32_t            root,
                                         const uint64_t            sizeSent,
                                         const uint64_t            sizeReceived)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createMpiCollEnd(defs,
                                                   otf2ConvertTime(time, data),
                                                   otf2ConvertCollectiveType(collectiveOp),
                                                   communicator,
                                                   root,
                                                   sizeSent,
                                                   sizeReceived);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiIrecv(const OTF2_LocationRef    location,
                                 const OTF2_TimeStamp      time,
                                 const uint64_t            eventPosition,
                                 void* const               userData,
                                 OTF2_AttributeList* const attributeList,
                                 const uint32_t            sender,
                                 const OTF2_CommRef        communicator,
                                 const uint32_t            msgTag,
                                 const uint64_t            msgLength,
                                 const uint64_t            requestID)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createMpiRecvComplete(defs,
                                                        otf2ConvertTime(time, data),
                                                        communicator,
                                                        sender,
                                                        msgTag,
                                                        msgLength,
                                                        requestID);
    data->mActiveRequests.erase(requestID);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiIrecvRequest(const OTF2_LocationRef    location,
                                        const OTF2_TimeStamp      time,
                                        const uint64_t            eventPosition,
                                        void* const               userData,
                                        OTF2_AttributeList* const attributeList,
                                        const uint64_t            requestID)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createMpiRecvRequest(defs,
                                                       otf2ConvertTime(time, data),
                                                       requestID);
    data->mActiveRequests.insert(requestID);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiIsend(const OTF2_LocationRef    location,
                                 const OTF2_TimeStamp      time,
                                 const uint64_t            eventPosition,
                                 void* const               userData,
                                 OTF2_AttributeList* const attributeList,
                                 const uint32_t            receiver,
                                 const OTF2_CommRef        communicator,
                                 const uint32_t            msgTag,
                                 const uint64_t            msgLength,
                                 const uint64_t            requestID)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createMpiSendRequest(defs,
                                                       otf2ConvertTime(time, data),
                                                       communicator,
                                                       receiver,
                                                       msgTag,
                                                       msgLength,
                                                       requestID);
    data->mActiveRequests.insert(requestID);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiIsendComplete(const OTF2_LocationRef    location,
                                         const OTF2_TimeStamp      time,
                                         const uint64_t            eventPosition,
                                         void* const               userData,
                                         OTF2_AttributeList* const attributeList,
                                         const uint64_t            requestID)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createMpiSendComplete(defs,
                                                        otf2ConvertTime(time, data),
                                                        requestID);
    data->mActiveRequests.erase(requestID);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiRecv(const OTF2_LocationRef    location,
                                const OTF2_TimeStamp      time,
                                const uint64_t            eventPosition,
                                void* const               userData,
                                OTF2_AttributeList* const attributeList,
                                const uint32_t            sender,
                                const OTF2_CommRef        communicator,
                                const uint32_t            msgTag,
                                const uint64_t            msgLength)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createMpiRecv(defs,
                                                otf2ConvertTime(time, data),
                                                communicator,
                                                sender,
                                                msgTag,
                                                msgLength);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiRequestCancelled(const OTF2_LocationRef    location,
                                            const OTF2_TimeStamp      time,
                                            const uint64_t            eventPosition,
                                            void* const               userData,
                                            OTF2_AttributeList* const attributeList,
                                            const uint64_t            requestID)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createMpiCancelled(defs,
                                                     otf2ConvertTime(time, data),
                                                     requestID);
    data->mActiveRequests.erase(requestID);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiRequestTest(const OTF2_LocationRef    location,
                                       const OTF2_TimeStamp      time,
                                       const uint64_t            eventPosition,
                                       void* const               userData,
                                       OTF2_AttributeList* const attributeList,
                                       const uint64_t            requestID)
{
    CALLBACK_SETUP

    // Only keep REQUEST_TESTED events of active requests
    if (data->mActiveRequests.find(requestID) != data->mActiveRequests.end())
    {
        Event_rep* event =
            EventFactory::instance()->createMpiRequestTested(defs,
                                                             otf2ConvertTime(time, data),
                                                             requestID);
        if (event)
        {
            trace.add_event(event);
        }
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbMpiSend(const OTF2_LocationRef    location,
                                const OTF2_TimeStamp      time,
                                const uint64_t            eventPosition,
                                void* const               userData,
                                OTF2_AttributeList* const attributeList,
                                const uint32_t            receiver,
                                const OTF2_CommRef        communicator,
                                const uint32_t            msgTag,
                                const uint64_t            msgLength)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createMpiSend(defs,
                                                otf2ConvertTime(time, data),
                                                communicator,
                                                receiver,
                                                msgTag,
                                                msgLength);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; ParameterInt events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbParameterInt(const OTF2_LocationRef    location,
                                     const OTF2_TimeStamp      time,
                                     const uint64_t            eventPosition,
                                     void* const               userData,
                                     OTF2_AttributeList* const attributeList,
                                     const OTF2_ParameterRef   parameter,
                                     const int64_t             value)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; ParameterString events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbParameterString(const OTF2_LocationRef    location,
                                        const OTF2_TimeStamp      time,
                                        const uint64_t            eventPosition,
                                        void* const               userData,
                                        OTF2_AttributeList* const attributeList,
                                        const OTF2_ParameterRef   parameter,
                                        const OTF2_StringRef      value)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; ParameterUnsignedInt events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbParameterUnsignedInt(const OTF2_LocationRef    location,
                                             const OTF2_TimeStamp      time,
                                             const uint64_t            eventPosition,
                                             void* const               userData,
                                             OTF2_AttributeList* const attributeList,
                                             const OTF2_ParameterRef   parameter,
                                             const uint64_t            value)
{
    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbProgramBegin(const OTF2_LocationRef      location,
                                     const OTF2_TimeStamp        time,
                                     const uint64_t              eventPosition,
                                     void* const                 userData,
                                     OTF2_AttributeList* const   attributeList,
                                     const OTF2_StringRef        programName,
                                     const uint32_t              numberOfArguments,
                                     const OTF2_StringRef* const programArguments)
{
    CALLBACK_SETUP
    const String& fullName = defs.getString(programName);

    // Region display name: executable name w/o path
    string displayName(UTILS_IO_GetWithoutPath(fullName.getCString()));

    // Region canonical name: executable name w/ path & arguments
    string canonicalName(fullName.getString());
    for (uint32_t i = 0; i < numberOfArguments; ++i)
    {
        canonicalName += ' ';
        canonicalName += defs.getString(programArguments[i]).getString();
    }

    // Search region in global definitions
    size_t       regionIndex;
    const size_t numRegions = defs.numRegions();
    for (regionIndex = 0; regionIndex < numRegions; ++regionIndex)
    {
        const Region& region = defs.getRegionByIndex(regionIndex);

        if (  (region.getRole() == Region::ROLE_ARTIFICIAL)
           && (region.getParadigm() == Paradigm::MEASUREMENT_SYSTEM)
           && (region.getDisplayName().getString() == displayName)
           && (region.getCanonicalName().getString() == canonicalName))
        {
            data->mProgramRegionId = region.getId();
            break;
        }
    }
    UTILS_BUG_ON(regionIndex == numRegions,
                 "Unable to find program region in global definitions!");

    // Create event representation
    Event_rep* event =
        EventFactory::instance()->createEnterProgram(defs,
                                                     otf2ConvertTime(time, data),
                                                     data->mProgramRegionId,
                                                     NULL,
                                                     programName,
                                                     numberOfArguments,
                                                     programArguments);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbProgramEnd(const OTF2_LocationRef    location,
                                   const OTF2_TimeStamp      time,
                                   const uint64_t            eventPosition,
                                   void* const               userData,
                                   OTF2_AttributeList* const attributeList,
                                   const int64_t             exitStatus)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createLeaveProgram(defs,
                                                     otf2ConvertTime(time, data),
                                                     data->mProgramRegionId,
                                                     NULL,
                                                     exitStatus);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaAcquireLock events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaAcquireLock(const OTF2_LocationRef    location,
                                       const OTF2_TimeStamp      time,
                                       const uint64_t            eventPosition,
                                       void* const               userData,
                                       OTF2_AttributeList* const attributeList,
                                       const OTF2_RmaWinRef      win,
                                       const uint32_t            remote,
                                       const uint64_t            lockId,
                                       const OTF2_LockType       lockType)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaAtomic events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaAtomic(const OTF2_LocationRef    location,
                                  const OTF2_TimeStamp      time,
                                  const uint64_t            eventPosition,
                                  void* const               userData,
                                  OTF2_AttributeList* const attributeList,
                                  const OTF2_RmaWinRef      win,
                                  const uint32_t            remote,
                                  const OTF2_RmaAtomicType  type,
                                  const uint64_t            bytesSent,
                                  const uint64_t            bytesReceived,
                                  const uint64_t            matchingId)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaCollectiveBegin events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaCollectiveBegin(const OTF2_LocationRef    location,
                                           const OTF2_TimeStamp      time,
                                           const uint64_t            eventPosition,
                                           void* const               userData,
                                           OTF2_AttributeList* const attributeList)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaCollectiveEnd events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaCollectiveEnd(const OTF2_LocationRef    location,
                                         const OTF2_TimeStamp      time,
                                         const uint64_t            eventPosition,
                                         void* const               userData,
                                         OTF2_AttributeList* const attributeList,
                                         const OTF2_CollectiveOp   collectiveOp,
                                         const OTF2_RmaSyncLevel   syncLevel,
                                         const OTF2_RmaWinRef      win,
                                         const uint32_t            root,
                                         const uint64_t            bytesSent,
                                         const uint64_t            bytesReceived)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaGet events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaGet(const OTF2_LocationRef    location,
                               const OTF2_TimeStamp      time,
                               const uint64_t            eventPosition,
                               void* const               userData,
                               OTF2_AttributeList* const attributeList,
                               const OTF2_RmaWinRef      win,
                               const uint32_t            remote,
                               const uint64_t            bytes,
                               const uint64_t            matchingId)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaGroupSync events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaGroupSync(const OTF2_LocationRef    location,
                                     const OTF2_TimeStamp      time,
                                     const uint64_t            eventPosition,
                                     void* const               userData,
                                     OTF2_AttributeList* const attributeList,
                                     const OTF2_RmaSyncLevel   syncLevel,
                                     const OTF2_RmaWinRef      win,
                                     const OTF2_GroupRef       group)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaOpCompleteBlocking events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaOpCompleteBlocking(const OTF2_LocationRef    location,
                                              const OTF2_TimeStamp      time,
                                              const uint64_t            eventPosition,
                                              void* const               userData,
                                              OTF2_AttributeList* const attributeList,
                                              const OTF2_RmaWinRef      win,
                                              const uint64_t            matchingId)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaOpCompleteNonBlocking events are currently
///     ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaOpCompleteNonBlocking(const OTF2_LocationRef    location,
                                                 const OTF2_TimeStamp      time,
                                                 const uint64_t            eventPosition,
                                                 void* const               userData,
                                                 OTF2_AttributeList* const attributeList,
                                                 const OTF2_RmaWinRef      win,
                                                 const uint64_t            matchingId)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaOpCompleteRemote events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaOpCompleteRemote(const OTF2_LocationRef    location,
                                            const OTF2_TimeStamp      time,
                                            const uint64_t            eventPosition,
                                            void* const               userData,
                                            OTF2_AttributeList* const attributeList,
                                            const OTF2_RmaWinRef      win,
                                            const uint64_t            matchingId)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaOpTest events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaOpTest(const OTF2_LocationRef    location,
                                  const OTF2_TimeStamp      time,
                                  const uint64_t            eventPosition,
                                  void* const               userData,
                                  OTF2_AttributeList* const attributeList,
                                  const OTF2_RmaWinRef      win,
                                  const uint64_t            matchingId)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaPut events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaPut(const OTF2_LocationRef    location,
                               const OTF2_TimeStamp      time,
                               const uint64_t            eventPosition,
                               void* const               userData,
                               OTF2_AttributeList* const attributeList,
                               const OTF2_RmaWinRef      win,
                               const uint32_t            remote,
                               const uint64_t            bytes,
                               const uint64_t            matchingId)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaReleaseLock events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaReleaseLock(const OTF2_LocationRef    location,
                                       const OTF2_TimeStamp      time,
                                       const uint64_t            eventPosition,
                                       void* const               userData,
                                       OTF2_AttributeList* const attributeList,
                                       const OTF2_RmaWinRef      win,
                                       const uint32_t            remote,
                                       const uint64_t            lockId)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaRequestLock events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaRequestLock(const OTF2_LocationRef    location,
                                       const OTF2_TimeStamp      time,
                                       const uint64_t            eventPosition,
                                       void* const               userData,
                                       OTF2_AttributeList* const attributeList,
                                       const OTF2_RmaWinRef      win,
                                       const uint32_t            remote,
                                       const uint64_t            lockId,
                                       const OTF2_LockType       lockType)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaSync events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaSync(const OTF2_LocationRef    location,
                                const OTF2_TimeStamp      time,
                                const uint64_t            eventPosition,
                                void* const               userData,
                                OTF2_AttributeList* const attributeList,
                                const OTF2_RmaWinRef      win,
                                const uint32_t            remote,
                                const OTF2_RmaSyncType    syncType)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaTryLock events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaTryLock(const OTF2_LocationRef    location,
                                   const OTF2_TimeStamp      time,
                                   const uint64_t            eventPosition,
                                   void* const               userData,
                                   OTF2_AttributeList* const attributeList,
                                   const OTF2_RmaWinRef      win,
                                   const uint32_t            remote,
                                   const uint64_t            lockId,
                                   const OTF2_LockType       lockType)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaWaitChange events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaWaitChange(const OTF2_LocationRef    location,
                                      const OTF2_TimeStamp      time,
                                      const uint64_t            eventPosition,
                                      void* const               userData,
                                      OTF2_AttributeList* const attributeList,
                                      const OTF2_RmaWinRef      win)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaWinCreate events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaWinCreate(const OTF2_LocationRef    location,
                                     const OTF2_TimeStamp      time,
                                     const uint64_t            eventPosition,
                                     void* const               userData,
                                     OTF2_AttributeList* const attributeList,
                                     const OTF2_RmaWinRef      win)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaWinDestroy events are currently ignored
OTF2_CallbackCode
pearl::detail::otf2EvtCbRmaWinDestroy(const OTF2_LocationRef    location,
                                      const OTF2_TimeStamp      time,
                                      const uint64_t            eventPosition,
                                      void* const               userData,
                                      OTF2_AttributeList* const attributeList,
                                      const OTF2_RmaWinRef      win)
{
    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadAcquireLock(const OTF2_LocationRef    location,
                                          const OTF2_TimeStamp      time,
                                          const uint64_t            eventPosition,
                                          void* const               userData,
                                          OTF2_AttributeList* const attributeList,
                                          const OTF2_Paradigm       model,
                                          const uint32_t            lockID,
                                          const uint32_t            acquisitionOrder)
{
    // Score-P < v1.3 did not track the lock acquisition order (always 0).
    // Without this information, the lock analysis will deadlock.  Thus,
    // discard the corresponding lock events.
    if (acquisitionOrder == 0)
    {
        return OTF2_CALLBACK_SUCCESS;
    }

    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadAcquireLock(defs,
                                                          otf2ConvertTime(time, data),
                                                          lockID,
                                                          acquisitionOrder,
                                                          otf2ConvertParadigm(model));
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadBegin(const OTF2_LocationRef    location,
                                    const OTF2_TimeStamp      time,
                                    const uint64_t            eventPosition,
                                    void* const               userData,
                                    OTF2_AttributeList* const attributeList,
                                    const OTF2_CommRef        threadContingent,
                                    const uint64_t            sequenceCount)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadBegin(defs,
                                                    otf2ConvertTime(time, data),
                                                    threadContingent,
                                                    sequenceCount);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadCreate(const OTF2_LocationRef    location,
                                     const OTF2_TimeStamp      time,
                                     const uint64_t            eventPosition,
                                     void* const               userData,
                                     OTF2_AttributeList* const attributeList,
                                     const OTF2_CommRef        threadContingent,
                                     const uint64_t            sequenceCount)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadCreate(defs,
                                                     otf2ConvertTime(time, data),
                                                     threadContingent,
                                                     sequenceCount);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadEnd(const OTF2_LocationRef    location,
                                  const OTF2_TimeStamp      time,
                                  const uint64_t            eventPosition,
                                  void* const               userData,
                                  OTF2_AttributeList* const attributeList,
                                  const OTF2_CommRef        threadContingent,
                                  const uint64_t            sequenceCount)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadEnd(defs,
                                                  otf2ConvertTime(time, data),
                                                  threadContingent,
                                                  sequenceCount);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadFork(const OTF2_LocationRef    location,
                                   const OTF2_TimeStamp      time,
                                   const uint64_t            eventPosition,
                                   void* const               userData,
                                   OTF2_AttributeList* const attributeList,
                                   const OTF2_Paradigm       model,
                                   const uint32_t            numberOfRequestedThreads)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadFork(defs,
                                                   otf2ConvertTime(time, data),
                                                   numberOfRequestedThreads,
                                                   otf2ConvertParadigm(model));
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadJoin(const OTF2_LocationRef    location,
                                   const OTF2_TimeStamp      time,
                                   const uint64_t            eventPosition,
                                   void* const               userData,
                                   OTF2_AttributeList* const attributeList,
                                   const OTF2_Paradigm       model)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadJoin(defs,
                                                   otf2ConvertTime(time, data),
                                                   otf2ConvertParadigm(model));
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadReleaseLock(const OTF2_LocationRef    location,
                                          const OTF2_TimeStamp      time,
                                          const uint64_t            eventPosition,
                                          void* const               userData,
                                          OTF2_AttributeList* const attributeList,
                                          const OTF2_Paradigm       model,
                                          const uint32_t            lockID,
                                          const uint32_t            acquisitionOrder)
{
    // Score-P < v1.3 did not track the lock acquisition order (always 0).
    // Without this information, the lock analysis will deadlock.  Thus,
    // discard the corresponding lock events.
    if (acquisitionOrder == 0)
    {
        return OTF2_CALLBACK_SUCCESS;
    }

    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadReleaseLock(defs,
                                                          otf2ConvertTime(time, data),
                                                          lockID,
                                                          acquisitionOrder,
                                                          otf2ConvertParadigm(model));
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadTaskComplete(const OTF2_LocationRef    location,
                                           const OTF2_TimeStamp      time,
                                           const uint64_t            eventPosition,
                                           void* const               userData,
                                           OTF2_AttributeList* const attributeList,
                                           const OTF2_CommRef        threadTeam,
                                           const uint32_t            creatingThread,
                                           const uint32_t            generationNumber)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadTaskComplete(defs,
                                                           otf2ConvertTime(time, data),
                                                           threadTeam,
                                                           (static_cast< uint64_t >(creatingThread) << 32) + generationNumber);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadTaskCreate(const OTF2_LocationRef    location,
                                         const OTF2_TimeStamp      time,
                                         const uint64_t            eventPosition,
                                         void* const               userData,
                                         OTF2_AttributeList* const attributeList,
                                         const OTF2_CommRef        threadTeam,
                                         const uint32_t            creatingThread,
                                         const uint32_t            generationNumber)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadTaskCreate(defs,
                                                         otf2ConvertTime(time, data),
                                                         threadTeam,
                                                         (static_cast< uint64_t >(creatingThread) << 32) + generationNumber);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadTaskSwitch(const OTF2_LocationRef    location,
                                         const OTF2_TimeStamp      time,
                                         const uint64_t            eventPosition,
                                         void* const               userData,
                                         OTF2_AttributeList* const attributeList,
                                         const OTF2_CommRef        threadTeam,
                                         uint32_t                  creatingThread,
                                         const uint32_t            generationNumber)
{
    CALLBACK_SETUP

    // Fix creating thread ID (always 0 for Score-P < v1.4)
    if ((generationNumber == 0) && (creatingThread == 0))
    {
        creatingThread = trace.get_location().getThreadId();
    }

    Event_rep* event =
        EventFactory::instance()->createThreadTaskSwitch(defs,
                                                         otf2ConvertTime(time, data),
                                                         threadTeam,
                                                         (static_cast< uint64_t >(creatingThread) << 32) + generationNumber);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadTeamBegin(const OTF2_LocationRef    location,
                                        const OTF2_TimeStamp      time,
                                        const uint64_t            eventPosition,
                                        void* const               userData,
                                        OTF2_AttributeList* const attributeList,
                                        const OTF2_CommRef        threadTeam)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadTeamBegin(defs,
                                                        otf2ConvertTime(time, data),
                                                        threadTeam);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadTeamEnd(const OTF2_LocationRef    location,
                                      const OTF2_TimeStamp      time,
                                      const uint64_t            eventPosition,
                                      void* const               userData,
                                      OTF2_AttributeList* const attributeList,
                                      const OTF2_CommRef        threadTeam)
{
    CALLBACK_SETUP
    const timestamp_t timestamp = otf2ConvertTime(time, data);

    // CCT traces: "Close" call stack before leaving the thread team
    if (data->mPreviousContext != 0)
    {
        if (trace.get_location().getThreadId() != 0)
        {
            processContext(data, timestamp, 0, 1);
            data->mPreviousContext = 0;
        }
    }

    Event_rep* event =
        EventFactory::instance()->createThreadTeamEnd(defs,
                                                      timestamp,
                                                      threadTeam);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2EvtCbThreadWait(const OTF2_LocationRef    location,
                                   const OTF2_TimeStamp      time,
                                   const uint64_t            eventPosition,
                                   void* const               userData,
                                   OTF2_AttributeList* const attributeList,
                                   const OTF2_CommRef        threadContingent,
                                   const uint64_t            sequenceCount)
{
    CALLBACK_SETUP
    Event_rep* event =
        EventFactory::instance()->createThreadWait(defs,
                                                   otf2ConvertTime(time, data),
                                                   threadContingent,
                                                   sequenceCount);
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


// --- Local helper functions -----------------------------------------------

namespace
{
/// @brief Get pointer of calling context definition object.
///
/// Returns a pointer to the calling context definition object referenced by
/// @a contextId or `nullptr` if undefined.
///
/// @param defs
///     Global definitions object
/// @param contextId
///     Global calling context definition identifier
/// @returns
///     Pointer to calling context definition object if defined, `nullptr`
///     otherwise
///
CallingContext*
getContext(const GlobalDefs&            defs,
           const OTF2_CallingContextRef contextId)
{
    const CallingContext& context = defs.getCallingContext(contextId);
    if (context == CallingContext::UNDEFINED)
    {
        return 0;
    }

    return const_cast< CallingContext* >(&context);
}


/// @brief Enter a calling context after leaving contexts of previous sample.
///
/// Determines which calling context nodes have been left since the previous
/// sample stored in @a data based on the given unwind @a distance and creates
/// a corresponding sequence of LEAVE events as well as a sequence of ENTER
/// events then leading to the current @a context, all with the timestamp
/// @a time.
///
/// @param data
///     OTF2 event callback data
/// @param time
///     Timestamp to use for generated events
/// @param context
///     Current calling context
/// @param distance
///     Number of nodes in the calling context tree that have been left since
///     the previous sample
///
void
processContext(const Otf2EvtCbData* const data,
               const timestamp_t          time,
               CallingContext* const      context,
               const uint32_t             distance)
{
    if (distance == 1)
    {
        CallingContext* current = data->mPreviousContext;
        CallingContext* unwind  = context;
        if (unwind)
        {
            unwind = unwind->getParent();
        }
        while (current != unwind)
        {
            CallingContext* parent = current->getParent();
            if (context && (parent == unwind))
            {
                break;
            }

            const Region& region = current->getRegion();
            Event_rep*    event  =
                EventFactory::instance()->createLeave(data->mDefinitions,
                                                      time,
                                                      region.getId(),
                                                      NULL);
            if (event)
            {
                data->mTrace.add_event(event);
            }

            current = parent;
        }

        return;
    }

    processContext(data, time, context->getParent(), distance - 1);

    Event_rep* event =
        EventFactory::instance()->createEnter(data->mDefinitions,
                                              time,
                                              context->getRegion().getId(),
                                              NULL);
    if (event)
    {
        data->mTrace.add_event(event);
    }
}
}    // unnamed namespace
