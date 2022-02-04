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
 *  @brief   Declarations of OTF2 callback functions for event records.
 *
 *  This header file provides the declarations of OTF2 callback functions
 *  and related data types for handling event records.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_OTF2EVTCALLBACKS_H
#define PEARL_OTF2EVTCALLBACKS_H


#include <set>
#include <string>

#include <otf2/OTF2_AttributeList.h>
#include <otf2/OTF2_Events.h>

#include <pearl/Region.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class CallingContext;
class GlobalDefs;
class LocalTrace;


namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @struct  Otf2EvtCbData
 *  @ingroup PEARL_base
 *  @brief   OTF2 event callback data
 *
 *  The Otf2EvtCbData struct combines all data elements which are necessary
 *  to process the event records of an OTF2 trace archive.
 **/
/*-------------------------------------------------------------------------*/

struct Otf2EvtCbData
{
    // --- Public methods ----------------------------------

    /// @name Constructors & destructor
    /// @{

    /// @brief Constructor.
    ///
    /// Creates a new instance and initializes it with the given values.
    ///
    /// @param defs
    ///     Global definitions object
    /// @param trace
    ///     Local event trace object
    /// @param timerResolution
    ///     Timer resolution in ticks per second
    /// @param globalOffset
    ///     Estimated timestamp of the first event
    ///
    Otf2EvtCbData(const GlobalDefs& defs,
                  LocalTrace&       trace,
                  uint64_t          timerResolution,
                  uint64_t          globalOffset)
        : mDefinitions(defs),
          mTrace(trace),
          mTimerResolution(timerResolution),
          mGlobalOffset(globalOffset),
          mPreviousContext(0),
          mProgramRegionId(Region::NO_ID)
    {
    }

    /// @brief Destructor.
    ///
    /// Destroys the instance.
    ///
    ~Otf2EvtCbData();

    /// @}


    // --- Data members ------------------------------------

    /// Global definitions object
    const GlobalDefs& mDefinitions;

    /// Local trace data object
    LocalTrace& mTrace;

    /// %Error message of exceptions thrown (and caught) within callbacks
    std::string mErrorMessage;

    /// %Timer resolution in ticks per second
    uint64_t mTimerResolution;

    /// Estimated timestamp of the first event
    uint64_t mGlobalOffset;

    /// Currently active MPI request IDs
    std::set< uint64_t > mActiveRequests;

    /// Calling context of a previous sample event
    CallingContext* mPreviousContext;

    /// Global region identifier of program region
    Region::IdType mProgramRegionId;
};


// --- Function prototypes --------------------------------------------------

/// @name OTF2: Event callbacks
/// @{

/// @brief BufferFlush event callback
///
/// %Callback function that is triggered whenever a BufferFlush event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param stopTime
///     Timestamp in ticks when the buffer flush finished
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbBufferFlush(OTF2_LocationRef    location,
                     OTF2_TimeStamp      time,
                     uint64_t            eventPosition,
                     void*               userData,
                     OTF2_AttributeList* attributeList,
                     OTF2_TimeStamp      stopTime);

/// @brief CallingContextEnter event callback
///
/// %Callback function that is triggered whenever a CallingContextEnter event
/// record is read from the trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param callingContext
///     The calling context of the region entered (reference to CallingContext
///     definition)
/// @param unwindDistance
///     Number of nodes along the current calling context path (starting
///     from @a callingContext) that have been either newly visited or
///     revisited since the last sample
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbCallingContextEnter(OTF2_LocationRef       location,
                             OTF2_TimeStamp         time,
                             uint64_t               eventPosition,
                             void*                  userData,
                             OTF2_AttributeList*    attributeList,
                             OTF2_CallingContextRef callingContext,
                             uint32_t               unwindDistance);

/// @brief CallingContextLeave event callback
///
/// %Callback function that is triggered whenever a CallingContextLeave event
/// record is read from the trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param callingContext
///     The calling context of the region left (reference to CallingContext
///     definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbCallingContextLeave(OTF2_LocationRef       location,
                             OTF2_TimeStamp         time,
                             uint64_t               eventPosition,
                             void*                  userData,
                             OTF2_AttributeList*    attributeList,
                             OTF2_CallingContextRef callingContext);

/// @brief CallingContextSample event callback
///
/// %Callback function that is triggered whenever a CallingContextSample event
/// record is read from the trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param callingContext
///     The calling context in which the sample happened (reference to
///     CallingContext definition)
/// @param unwindDistance
///     Number of nodes along the current calling context path (starting
///     from @a callingContext) that have been either newly visited or
///     revisited since the last sample
/// @param interruptGenerator
///     The interrupt generator which triggered the sample (reference to
///     InterruptGenerator definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbCallingContextSample(OTF2_LocationRef           location,
                              OTF2_TimeStamp             time,
                              uint64_t                   eventPosition,
                              void*                      userData,
                              OTF2_AttributeList*        attributeList,
                              OTF2_CallingContextRef     callingContext,
                              uint32_t                   unwindDistance,
                              OTF2_InterruptGeneratorRef interruptGenerator);

/// @brief Enter event callback
///
/// %Callback function that is triggered whenever an Enter event record is read
/// from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param region
///     %Region entered (reference to Region definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbEnter(OTF2_LocationRef    location,
               OTF2_TimeStamp      time,
               uint64_t            eventPosition,
               void*               userData,
               OTF2_AttributeList* attributeList,
               OTF2_RegionRef      region);

/// @brief Leave event callback
///
/// %Callback function that is triggered whenever a Leave event record is read
/// from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param region
///     %Region left (reference to Region definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbLeave(OTF2_LocationRef    location,
               OTF2_TimeStamp      time,
               uint64_t            eventPosition,
               void*               userData,
               OTF2_AttributeList* attributeList,
               OTF2_RegionRef      region);

/// @brief MeasurementOnOff event callback
///
/// %Callback function that is triggered whenever a MeasurementOnOff event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param measurementMode
///     Specifies whether measurement was turned on or off at this event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMeasurementOnOff(OTF2_LocationRef     location,
                          OTF2_TimeStamp       time,
                          uint64_t             eventPosition,
                          void*                userData,
                          OTF2_AttributeList*  attributeList,
                          OTF2_MeasurementMode measurementMode);

/// @brief %Metric event callback
///
/// %Callback function that is triggered whenever a Metric event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param metric
///     %Metric class or instance stored by this event (reference to either
///     MetricClass or MetricInstance definition)
/// @param numberOfMetrics
///     Number of metrics within the set
/// @param typeIDs
///     List of metric data types
/// @param metricValues
///     List of metric values
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMetric(OTF2_LocationRef        location,
                OTF2_TimeStamp          time,
                uint64_t                eventPosition,
                void*                   userData,
                OTF2_AttributeList*     attributeList,
                OTF2_MetricRef          metric,
                uint8_t                 numberOfMetrics,
                const OTF2_Type*        typeIDs,
                const OTF2_MetricValue* metricValues);

/// @brief MpiCollectiveBegin event callback
///
/// %Callback function that is triggered whenever a MpiCollectiveBegin event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiCollectiveBegin(OTF2_LocationRef    location,
                            OTF2_TimeStamp      time,
                            uint64_t            eventPosition,
                            void*               userData,
                            OTF2_AttributeList* attributeList);

/// @brief MpiCollectiveEnd event callback
///
/// %Callback function that is triggered whenever a MpiCollectiveEnd event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param collectiveOp
///     Collective operation
/// @param communicator
///     %Communicator (reference to Communicator definition)
/// @param root
///     Rank of root process in @a communicator
/// @param sizeSent
///     Number of bytes sent
/// @param sizeReceived
///     Number of bytes received
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiCollectiveEnd(OTF2_LocationRef    location,
                          OTF2_TimeStamp      time,
                          uint64_t            eventPosition,
                          void*               userData,
                          OTF2_AttributeList* attributeList,
                          OTF2_CollectiveOp   collectiveOp,
                          OTF2_CommRef        communicator,
                          uint32_t            root,
                          uint64_t            sizeSent,
                          uint64_t            sizeReceived);

/// @brief MpiIrecv event callback
///
/// %Callback function that is triggered whenever a MpiIrecv event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param sender
///     Rank of sending process in @a communicator
/// @param communicator
///     %Communicator (reference to Communicator definition)
/// @param msgTag
///     Message tag
/// @param msgLength
///     Number of bytes received
/// @param requestID
///     Unique identifier of the corresponding request
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiIrecv(OTF2_LocationRef    location,
                  OTF2_TimeStamp      time,
                  uint64_t            eventPosition,
                  void*               userData,
                  OTF2_AttributeList* attributeList,
                  uint32_t            sender,
                  OTF2_CommRef        communicator,
                  uint32_t            msgTag,
                  uint64_t            msgLength,
                  uint64_t            requestID);

/// @brief MpiIrecvRequest event callback
///
/// %Callback function that is triggered whenever a MpiIrecvRequest event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param requestID
///     Unique identifier of the corresponding request
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiIrecvRequest(OTF2_LocationRef    location,
                         OTF2_TimeStamp      time,
                         uint64_t            eventPosition,
                         void*               userData,
                         OTF2_AttributeList* attributeList,
                         uint64_t            requestID);

/// @brief MpiIsend event callback
///
/// %Callback function that is triggered whenever a MpiIsend event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param receiver
///     Rank of receiving process in @a communicator
/// @param communicator
///     %Communicator (reference to Communicator definition)
/// @param msgTag
///     Message tag
/// @param msgLength
///     Number of bytes sent
/// @param requestID
///     Unique identifier of the corresponding request
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiIsend(OTF2_LocationRef    location,
                  OTF2_TimeStamp      time,
                  uint64_t            eventPosition,
                  void*               userData,
                  OTF2_AttributeList* attributeList,
                  uint32_t            receiver,
                  OTF2_CommRef        communicator,
                  uint32_t            msgTag,
                  uint64_t            msgLength,
                  uint64_t            requestID);

/// @brief MpiIsendComplete event callback
///
/// %Callback function that is triggered whenever a MpiIsendComplete event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param requestID
///     Unique identifier of the corresponding request
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiIsendComplete(OTF2_LocationRef    location,
                          OTF2_TimeStamp      time,
                          uint64_t            eventPosition,
                          void*               userData,
                          OTF2_AttributeList* attributeList,
                          uint64_t            requestID);

/// @brief MpiRecv event callback
///
/// %Callback function that is triggered whenever a MpiRecv event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param sender
///     Rank of sending process in @a communicator
/// @param communicator
///     %Communicator (reference to Communicator definition)
/// @param msgTag
///     Message tag
/// @param msgLength
///     Number of bytes received
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiRecv(OTF2_LocationRef    location,
                 OTF2_TimeStamp      time,
                 uint64_t            eventPosition,
                 void*               userData,
                 OTF2_AttributeList* attributeList,
                 uint32_t            sender,
                 OTF2_CommRef        communicator,
                 uint32_t            msgTag,
                 uint64_t            msgLength);

/// @brief MpiRequestCancelled event callback
///
/// %Callback function that is triggered whenever a MpiRequestCancelled event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param requestID
///     Unique identifier of the corresponding request
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiRequestCancelled(OTF2_LocationRef    location,
                             OTF2_TimeStamp      time,
                             uint64_t            eventPosition,
                             void*               userData,
                             OTF2_AttributeList* attributeList,
                             uint64_t            requestID);

/// @brief MpiRequestTest event callback
///
/// %Callback function that is triggered whenever a MpiRequestTest event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param requestID
///     Unique identifier of the corresponding request
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiRequestTest(OTF2_LocationRef    location,
                        OTF2_TimeStamp      time,
                        uint64_t            eventPosition,
                        void*               userData,
                        OTF2_AttributeList* attributeList,
                        uint64_t            requestID);

/// @brief MpiSend event callback
///
/// %Callback function that is triggered whenever a MpiSend event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param receiver
///     Rank of receiving process in @a communicator
/// @param communicator
///     %Communicator (reference to Communicator definition)
/// @param msgTag
///     Message tag
/// @param msgLength
///     Number of bytes sent
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbMpiSend(OTF2_LocationRef    location,
                 OTF2_TimeStamp      time,
                 uint64_t            eventPosition,
                 void*               userData,
                 OTF2_AttributeList* attributeList,
                 uint32_t            receiver,
                 OTF2_CommRef        communicator,
                 uint32_t            msgTag,
                 uint64_t            msgLength);

/// @brief ParameterInt event callback
///
/// %Callback function that is triggered whenever a ParameterInt event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param parameter
///     %Parameter stored by this event (reference to Parameter definition)
/// @param value
///     %Parameter value
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbParameterInt(OTF2_LocationRef    location,
                      OTF2_TimeStamp      time,
                      uint64_t            eventPosition,
                      void*               userData,
                      OTF2_AttributeList* attributeList,
                      OTF2_ParameterRef   parameter,
                      int64_t             value);

/// @brief ParameterString event callback
///
/// %Callback function that is triggered whenever a ParameterString event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param parameter
///     %Parameter stored by this event (reference to Parameter definition)
/// @param value
///     %Parameter value
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbParameterString(OTF2_LocationRef    location,
                         OTF2_TimeStamp      time,
                         uint64_t            eventPosition,
                         void*               userData,
                         OTF2_AttributeList* attributeList,
                         OTF2_ParameterRef   parameter,
                         OTF2_StringRef      value);

/// @brief ParameterUnsignedInt event callback
///
/// %Callback function that is triggered whenever a ParameterUnsignedInt event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param parameter
///     %Parameter stored by this event (reference to Parameter definition)
/// @param value
///     %Parameter value
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbParameterUnsignedInt(OTF2_LocationRef    location,
                              OTF2_TimeStamp      time,
                              uint64_t            eventPosition,
                              void*               userData,
                              OTF2_AttributeList* attributeList,
                              OTF2_ParameterRef   parameter,
                              uint64_t            value);

/// @brief ProgramBegin event callback
///
/// %Callback function that is triggered whenever a ProgramBegin event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param programName
///     Name of the program executable (reference to String definition)
/// @param numberOfArguments
///     Number of command-line arguments passed to the program
/// @param programArguments
///     List of command-line arguments (references to String definitions)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbProgramBegin(OTF2_LocationRef      location,
                      OTF2_TimeStamp        time,
                      uint64_t              eventPosition,
                      void*                 userData,
                      OTF2_AttributeList*   attributeList,
                      OTF2_StringRef        programName,
                      uint32_t              numberOfArguments,
                      const OTF2_StringRef* programArguments);

/// @brief ProgramEnd event callback
///
/// %Callback function that is triggered whenever a ProgramEnd event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param exitStatus
///     Program exit status
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbProgramEnd(OTF2_LocationRef    location,
                    OTF2_TimeStamp      time,
                    uint64_t            eventPosition,
                    void*               userData,
                    OTF2_AttributeList* attributeList,
                    int64_t             exitStatus);

/// @brief RmaAcquireLock event callback
///
/// %Callback function that is triggered whenever a RmaAcquireLock event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param remote
///     Rank of the locked remote process
/// @param lockId
///     Unique identifier of the lock
/// @param lockType
///     Type of the lock
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaAcquireLock(OTF2_LocationRef    location,
                        OTF2_TimeStamp      time,
                        uint64_t            eventPosition,
                        void*               userData,
                        OTF2_AttributeList* attributeList,
                        OTF2_RmaWinRef      win,
                        uint32_t            remote,
                        uint64_t            lockId,
                        OTF2_LockType       lockType);

/// @brief RmaAtomic event callback
///
/// %Callback function that is triggered whenever a RmaAtomic event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param remote
///     Rank of the target process
/// @param type
///     Type of the atomic operation
/// @param bytesSent
///     Number of bytes sent to target
/// @param bytesReceived
///     Number of bytes received from target
/// @param matchingId
///     Unique identifier used to match the corresponding completion event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaAtomic(OTF2_LocationRef    location,
                   OTF2_TimeStamp      time,
                   uint64_t            eventPosition,
                   void*               userData,
                   OTF2_AttributeList* attributeList,
                   OTF2_RmaWinRef      win,
                   uint32_t            remote,
                   OTF2_RmaAtomicType  type,
                   uint64_t            bytesSent,
                   uint64_t            bytesReceived,
                   uint64_t            matchingId);

/// @brief RmaCollectiveBegin event callback
///
/// %Callback function that is triggered whenever a RmaCollectiveBegin event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaCollectiveBegin(OTF2_LocationRef    location,
                            OTF2_TimeStamp      time,
                            uint64_t            eventPosition,
                            void*               userData,
                            OTF2_AttributeList* attributeList);

/// @brief RmaCollectiveEnd event callback
///
/// %Callback function that is triggered whenever a RmaCollectiveEnd event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param collectiveOp
///     Collective operation
/// @param syncLevel
///     Level of synchronization
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param root
///     Rank of root process
/// @param bytesSent
///     Number of bytes sent
/// @param bytesReceived
///     Number of bytes received
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaCollectiveEnd(OTF2_LocationRef    location,
                          OTF2_TimeStamp      time,
                          uint64_t            eventPosition,
                          void*               userData,
                          OTF2_AttributeList* attributeList,
                          OTF2_CollectiveOp   collectiveOp,
                          OTF2_RmaSyncLevel   syncLevel,
                          OTF2_RmaWinRef      win,
                          uint32_t            root,
                          uint64_t            bytesSent,
                          uint64_t            bytesReceived);

/// @brief RmaGet event callback
///
/// %Callback function that is triggered whenever a RmaGet event record is read
/// from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param remote
///     Rank of remote process
/// @param bytes
///     Number of bytes received from target
/// @param matchingId
///     Unique identifier used to match the corresponding completion event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaGet(OTF2_LocationRef    location,
                OTF2_TimeStamp      time,
                uint64_t            eventPosition,
                void*               userData,
                OTF2_AttributeList* attributeList,
                OTF2_RmaWinRef      win,
                uint32_t            remote,
                uint64_t            bytes,
                uint64_t            matchingId);

/// @brief RmaGroupSync event callback
///
/// %Callback function that is triggered whenever a RmaGroupSync event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param syncLevel
///     Level of synchronization
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param group
///     %Group of remote processes involved (reference to Group definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaGroupSync(OTF2_LocationRef    location,
                      OTF2_TimeStamp      time,
                      uint64_t            eventPosition,
                      void*               userData,
                      OTF2_AttributeList* attributeList,
                      OTF2_RmaSyncLevel   syncLevel,
                      OTF2_RmaWinRef      win,
                      OTF2_GroupRef       group);

/// @brief RmaOpCompleteBlocking event callback
///
/// %Callback function that is triggered whenever a RmaOpCompleteBlocking event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param matchingId
///     Unique identifier used to match the corresponding RMA operation event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaOpCompleteBlocking(OTF2_LocationRef    location,
                               OTF2_TimeStamp      time,
                               uint64_t            eventPosition,
                               void*               userData,
                               OTF2_AttributeList* attributeList,
                               OTF2_RmaWinRef      win,
                               uint64_t            matchingId);

/// @brief RmaOpCompleteNonBlocking event callback
///
/// %Callback function that is triggered whenever a RmaOpCompleteNonBlocking
/// event record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param matchingId
///     Unique identifier used to match the corresponding RMA operation event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaOpCompleteNonBlocking(OTF2_LocationRef    location,
                                  OTF2_TimeStamp      time,
                                  uint64_t            eventPosition,
                                  void*               userData,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_RmaWinRef      win,
                                  uint64_t            matchingId);

/// @brief RmaOpCompleteRemote event callback
///
/// %Callback function that is triggered whenever a RmaOpCompleteRemote event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param matchingId
///     Unique identifier used to match the corresponding RMA operation event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaOpCompleteRemote(OTF2_LocationRef    location,
                             OTF2_TimeStamp      time,
                             uint64_t            eventPosition,
                             void*               userData,
                             OTF2_AttributeList* attributeList,
                             OTF2_RmaWinRef      win,
                             uint64_t            matchingId);

/// @brief RmaOpTest event callback
///
/// %Callback function that is triggered whenever a RmaOpTest event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param matchingId
///     Unique identifier used to match the corresponding RMA operation event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaOpTest(OTF2_LocationRef    location,
                   OTF2_TimeStamp      time,
                   uint64_t            eventPosition,
                   void*               userData,
                   OTF2_AttributeList* attributeList,
                   OTF2_RmaWinRef      win,
                   uint64_t            matchingId);

/// @brief RmaPut event callback
///
/// %Callback function that is triggered whenever a RmaPut event record is read
/// from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param remote
///     Rank of target process
/// @param bytes
///     Number of bytes sent to target
/// @param matchingId
///     Unique identifier used to match the corresponding completion event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaPut(OTF2_LocationRef    location,
                OTF2_TimeStamp      time,
                uint64_t            eventPosition,
                void*               userData,
                OTF2_AttributeList* attributeList,
                OTF2_RmaWinRef      win,
                uint32_t            remote,
                uint64_t            bytes,
                uint64_t            matchingId);

/// @brief RmaReleaseLock event callback
///
/// %Callback function that is triggered whenever a RmaReleaseLock event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param remote
///     Rank of the locked remote process
/// @param lockId
///     Unique identifier of the lock
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaReleaseLock(OTF2_LocationRef    location,
                        OTF2_TimeStamp      time,
                        uint64_t            eventPosition,
                        void*               userData,
                        OTF2_AttributeList* attributeList,
                        OTF2_RmaWinRef      win,
                        uint32_t            remote,
                        uint64_t            lockId);

/// @brief RmaRequestLock event callback
///
/// %Callback function that is triggered whenever a RmaRequestLock event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param remote
///     Rank of the locked remote process
/// @param lockId
///     Unique identifier of the lock
/// @param lockType
///     Type of the lock
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaRequestLock(OTF2_LocationRef    location,
                        OTF2_TimeStamp      time,
                        uint64_t            eventPosition,
                        void*               userData,
                        OTF2_AttributeList* attributeList,
                        OTF2_RmaWinRef      win,
                        uint32_t            remote,
                        uint64_t            lockId,
                        OTF2_LockType       lockType);

/// @brief RmaSync event callback
///
/// %Callback function that is triggered whenever a RmaSync event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param remote
///     Rank of the remote process
/// @param syncType
///     Type of synchronization
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaSync(OTF2_LocationRef    location,
                 OTF2_TimeStamp      time,
                 uint64_t            eventPosition,
                 void*               userData,
                 OTF2_AttributeList* attributeList,
                 OTF2_RmaWinRef      win,
                 uint32_t            remote,
                 OTF2_RmaSyncType    syncType);

/// @brief RmaTryLock event callback
///
/// %Callback function that is triggered whenever a RmaTryLock event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @param remote
///     Rank of the locked remote process
/// @param lockId
///     Unique identifier of the lock
/// @param lockType
///     Type of the lock
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaTryLock(OTF2_LocationRef    location,
                    OTF2_TimeStamp      time,
                    uint64_t            eventPosition,
                    void*               userData,
                    OTF2_AttributeList* attributeList,
                    OTF2_RmaWinRef      win,
                    uint32_t            remote,
                    uint64_t            lockId,
                    OTF2_LockType       lockType);

/// @brief RmaWaitChange event callback
///
/// %Callback function that is triggered whenever a RmaWaitChange event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaWaitChange(OTF2_LocationRef    location,
                       OTF2_TimeStamp      time,
                       uint64_t            eventPosition,
                       void*               userData,
                       OTF2_AttributeList* attributeList,
                       OTF2_RmaWinRef      win);

/// @brief RmaWinCreate event callback
///
/// %Callback function that is triggered whenever a RmaWinCreate event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaWinCreate(OTF2_LocationRef    location,
                      OTF2_TimeStamp      time,
                      uint64_t            eventPosition,
                      void*               userData,
                      OTF2_AttributeList* attributeList,
                      OTF2_RmaWinRef      win);

/// @brief RmaWinDestroy event callback
///
/// %Callback function that is triggered whenever a RmaWinDestroy event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param win
///     RMA window (reference to RmaWin definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbRmaWinDestroy(OTF2_LocationRef    location,
                       OTF2_TimeStamp      time,
                       uint64_t            eventPosition,
                       void*               userData,
                       OTF2_AttributeList* attributeList,
                       OTF2_RmaWinRef      win);

/// @brief ThreadAcquireLock event callback
///
/// %Callback function that is triggered whenever a ThreadAcquireLock event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param model
///     Threading paradigm
/// @param lockID
///     Unique identifier of the lock
/// @param acquisitionOrder
///     Monotonically increasing number to determine the order of lock
///     acquisitions
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadAcquireLock(OTF2_LocationRef    location,
                           OTF2_TimeStamp      time,
                           uint64_t            eventPosition,
                           void*               userData,
                           OTF2_AttributeList* attributeList,
                           OTF2_Paradigm       model,
                           uint32_t            lockID,
                           uint32_t            acquisitionOrder);

/// @brief ThreadBegin event callback
///
/// %Callback function that is triggered whenever a ThreadBegin event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param threadContingent
///     Thread contingent (reference to Communicator definition)
/// @param sequenceCount
///     Unique identifier used to match the corresponding thread creation event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadBegin(OTF2_LocationRef    location,
                     OTF2_TimeStamp      time,
                     uint64_t            eventPosition,
                     void*               userData,
                     OTF2_AttributeList* attributeList,
                     OTF2_CommRef        threadContingent,
                     uint64_t            sequenceCount);

/// @brief ThreadCreate event callback
///
/// %Callback function that is triggered whenever a ThreadCreate event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param threadContingent
///     Thread contingent (reference to Communicator definition)
/// @param sequenceCount
///     Unique identifier used to match the corresponding thread begin event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadCreate(OTF2_LocationRef    location,
                      OTF2_TimeStamp      time,
                      uint64_t            eventPosition,
                      void*               userData,
                      OTF2_AttributeList* attributeList,
                      OTF2_CommRef        threadContingent,
                      uint64_t            sequenceCount);

/// @brief ThreadEnd event callback
///
/// %Callback function that is triggered whenever a ThreadEnd event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param threadContingent
///     Thread contingent (reference to Communicator definition)
/// @param sequenceCount
///     Unique identifier used to match the corresponding thread wait event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadEnd(OTF2_LocationRef    location,
                   OTF2_TimeStamp      time,
                   uint64_t            eventPosition,
                   void*               userData,
                   OTF2_AttributeList* attributeList,
                   OTF2_CommRef        threadContingent,
                   uint64_t            sequenceCount);

/// @brief ThreadFork event callback
///
/// %Callback function that is triggered whenever a ThreadFork event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param model
///     Threading paradigm
/// @param numberOfRequestedThreads
///     Requested size of the thread team
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadFork(OTF2_LocationRef    location,
                    OTF2_TimeStamp      time,
                    uint64_t            eventPosition,
                    void*               userData,
                    OTF2_AttributeList* attributeList,
                    OTF2_Paradigm       model,
                    uint32_t            numberOfRequestedThreads);

/// @brief ThreadJoin event callback
///
/// %Callback function that is triggered whenever a ThreadJoin event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param model
///     Threading paradigm
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadJoin(OTF2_LocationRef    location,
                    OTF2_TimeStamp      time,
                    uint64_t            eventPosition,
                    void*               userData,
                    OTF2_AttributeList* attributeList,
                    OTF2_Paradigm       model);

/// @brief ThreadReleaseLock event callback
///
/// %Callback function that is triggered whenever a ThreadReleaseLock event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param model
///     Threading paradigm
/// @param lockID
///     Unique identifier of the lock
/// @param acquisitionOrder
///     Monotonically increasing number to determine the order of lock
///     acquisitions
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadReleaseLock(OTF2_LocationRef    location,
                           OTF2_TimeStamp      time,
                           uint64_t            eventPosition,
                           void*               userData,
                           OTF2_AttributeList* attributeList,
                           OTF2_Paradigm       model,
                           uint32_t            lockID,
                           uint32_t            acquisitionOrder);

/// @brief ThreadTaskComplete event callback
///
/// %Callback function that is triggered whenever a ThreadTaskComplete event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param threadTeam
///     Thread team (reference to Communicator definition)
/// @param creatingThread
///     Rank of the thread who created this task w.r.t. @a threadTeam
/// @param generationNumber
///     Unique task identifier w.r.t. @a creatingThread
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadTaskComplete(OTF2_LocationRef    location,
                            OTF2_TimeStamp      time,
                            uint64_t            eventPosition,
                            void*               userData,
                            OTF2_AttributeList* attributeList,
                            OTF2_CommRef        threadTeam,
                            uint32_t            creatingThread,
                            uint32_t            generationNumber);

/// @brief ThreadTaskCreate event callback
///
/// %Callback function that is triggered whenever a ThreadTaskCreate event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param threadTeam
///     Thread team (reference to Communicator definition)
/// @param creatingThread
///     Rank of the thread who created this task w.r.t. @a threadTeam
/// @param generationNumber
///     Unique task identifier w.r.t. @a creatingThread
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadTaskCreate(OTF2_LocationRef    location,
                          OTF2_TimeStamp      time,
                          uint64_t            eventPosition,
                          void*               userData,
                          OTF2_AttributeList* attributeList,
                          OTF2_CommRef        threadTeam,
                          uint32_t            creatingThread,
                          uint32_t            generationNumber);

/// @brief ThreadTaskSwitch event callback
///
/// %Callback function that is triggered whenever a ThreadTaskSwitch event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param threadTeam
///     Thread team (reference to Communicator definition)
/// @param creatingThread
///     Rank of the thread who created this task w.r.t. @a threadTeam
/// @param generationNumber
///     Unique task identifier w.r.t. @a creatingThread
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadTaskSwitch(OTF2_LocationRef    location,
                          OTF2_TimeStamp      time,
                          uint64_t            eventPosition,
                          void*               userData,
                          OTF2_AttributeList* attributeList,
                          OTF2_CommRef        threadTeam,
                          uint32_t            creatingThread,
                          uint32_t            generationNumber);

/// @brief ThreadTeamBegin event callback
///
/// %Callback function that is triggered whenever a ThreadTeamBegin event
/// record is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param threadTeam
///     Thread team (reference to Communicator definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadTeamBegin(OTF2_LocationRef    location,
                         OTF2_TimeStamp      time,
                         uint64_t            eventPosition,
                         void*               userData,
                         OTF2_AttributeList* attributeList,
                         OTF2_CommRef        threadTeam);

/// @brief ThreadTeamEnd event callback
///
/// %Callback function that is triggered whenever a ThreadTeamEnd event record
/// is read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param threadTeam
///     Thread team (reference to Communicator definition)
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadTeamEnd(OTF2_LocationRef    location,
                       OTF2_TimeStamp      time,
                       uint64_t            eventPosition,
                       void*               userData,
                       OTF2_AttributeList* attributeList,
                       OTF2_CommRef        threadTeam);

/// @brief ThreadWait event callback
///
/// %Callback function that is triggered whenever a ThreadWait event record is
/// read from the event trace file.
///
/// @param location
///     %Location where the event happened (reference to Location definition)
/// @param time
///     Timestamp in ticks
/// @param eventPosition
///     Running event number (starting with 1)
/// @param userData
///     Pointer to user-provided callback data
/// @param attributeList
///     Additional event attributes
/// @param threadContingent
///     Thread contingent (reference to Communicator definition)
/// @param sequenceCount
///     Unique identifier used to match the corresponding thread end event
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2EvtCbThreadWait(OTF2_LocationRef    location,
                    OTF2_TimeStamp      time,
                    uint64_t            eventPosition,
                    void*               userData,
                    OTF2_AttributeList* attributeList,
                    OTF2_CommRef        threadContingent,
                    uint64_t            sequenceCount);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_OTF2EVTCALLBACKS_H
