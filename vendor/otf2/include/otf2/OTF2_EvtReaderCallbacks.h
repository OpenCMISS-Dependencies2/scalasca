/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_EVT_READER_CALLBACKS_H
#define OTF2_EVT_READER_CALLBACKS_H


/**
 *  @file
 *  @source     templates/OTF2_EvtReaderCallbacks.tmpl.h
 *
 *  @brief      This defines the callbacks for the event reader.
 */


#include <stdint.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>
#include <otf2/OTF2_AttributeList.h>
#include <otf2/OTF2_Events.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Opaque struct which holds all event record callbacks.
 */
typedef struct OTF2_EvtReaderCallbacks_struct OTF2_EvtReaderCallbacks;


/** @brief Allocates a new struct for the event callbacks.
 *
 *  @return A newly allocated struct of type @eref{OTF2_EvtReaderCallbacks}.
 */
OTF2_EvtReaderCallbacks*
OTF2_EvtReaderCallbacks_New( void );


/** @brief Deallocates a struct for the event callbacks.
 *
 *  @param evtReaderCallbacks Handle to a struct previously allocated
 *                            with @eref{OTF2_EvtReaderCallbacks_New}.
 */
void
OTF2_EvtReaderCallbacks_Delete( OTF2_EvtReaderCallbacks* evtReaderCallbacks );


/** @brief Clears a struct for the event callbacks.
 *
 *  @param evtReaderCallbacks Handle to a struct previously allocated
 *                            with @eref{OTF2_EvtReaderCallbacks_New}.
 */
void
OTF2_EvtReaderCallbacks_Clear( OTF2_EvtReaderCallbacks* evtReaderCallbacks );


/** @brief Callback for an unknown event record.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace.
 *                       Starting with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_Unknown )( OTF2_LocationRef    location,
                                      OTF2_TimeStamp      time,
                                      uint64_t            eventPosition,
                                      void*               userData,
                                      OTF2_AttributeList* attributeList );


/** @brief Registers the callback for the Unknown event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param unknownCallback    Function which should be called for all
 *                            unknown events.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetUnknownCallback(
    OTF2_EvtReaderCallbacks*       evtReaderCallbacks,
    OTF2_EvtReaderCallback_Unknown unknownCallback );


/** @brief Callback for the BufferFlush event record.
 *
 *  This event signals that the internal buffer was flushed at the given
 *  time.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param stopTime      The time the buffer flush finished.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_BufferFlush )( OTF2_LocationRef    location,
                                          OTF2_TimeStamp      time,
                                          uint64_t            eventPosition,
                                          void*               userData,
                                          OTF2_AttributeList* attributeList,
                                          OTF2_TimeStamp      stopTime );


/** @brief Registers the callback for the BufferFlush event.
 *
 *  @param evtReaderCallbacks  Struct for all callbacks.
 *  @param bufferFlushCallback Function which should be called for all
 *                             @eref{BufferFlush} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetBufferFlushCallback(
    OTF2_EvtReaderCallbacks*           evtReaderCallbacks,
    OTF2_EvtReaderCallback_BufferFlush bufferFlushCallback );


/** @brief Callback for the MeasurementOnOff event record.
 *
 *  This event signals where the measurement system turned measurement on
 *  or off.
 *
 *  @param location        The location where this event happened.
 *  @param time            The time when this event happened.
 *  @param eventPosition   The event position of this event in the trace.
 *                         Starting with 1.
 *  @param userData        User data as set by
 *                         @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                         @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList   Additional attributes for this event.
 *  @param measurementMode Is the measurement turned on
 *                         (@eref{OTF2_MEASUREMENT_ON}) or off
 *                         (@eref{OTF2_MEASUREMENT_OFF})?
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MeasurementOnOff )( OTF2_LocationRef     location,
                                               OTF2_TimeStamp       time,
                                               uint64_t             eventPosition,
                                               void*                userData,
                                               OTF2_AttributeList*  attributeList,
                                               OTF2_MeasurementMode measurementMode );


/** @brief Registers the callback for the MeasurementOnOff event.
 *
 *  @param evtReaderCallbacks       Struct for all callbacks.
 *  @param measurementOnOffCallback Function which should be called for all
 *                                  @eref{MeasurementOnOff} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMeasurementOnOffCallback(
    OTF2_EvtReaderCallbacks*                evtReaderCallbacks,
    OTF2_EvtReaderCallback_MeasurementOnOff measurementOnOffCallback );


/** @brief Callback for the Enter event record.
 *
 *  An enter record indicates that the program enters a code region.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param region        Needs to be defined in a definition record References a
 *                       @eref{Region} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_REGION} is available.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_Enter )( OTF2_LocationRef    location,
                                    OTF2_TimeStamp      time,
                                    uint64_t            eventPosition,
                                    void*               userData,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_RegionRef      region );


/** @brief Registers the callback for the Enter event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param enterCallback      Function which should be called for all
 *                            @eref{Enter} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetEnterCallback(
    OTF2_EvtReaderCallbacks*     evtReaderCallbacks,
    OTF2_EvtReaderCallback_Enter enterCallback );


/** @brief Callback for the Leave event record.
 *
 *  A leave record indicates that the program leaves a code region.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param region        Needs to be defined in a definition record References a
 *                       @eref{Region} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_REGION} is available.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_Leave )( OTF2_LocationRef    location,
                                    OTF2_TimeStamp      time,
                                    uint64_t            eventPosition,
                                    void*               userData,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_RegionRef      region );


/** @brief Registers the callback for the Leave event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param leaveCallback      Function which should be called for all
 *                            @eref{Leave} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetLeaveCallback(
    OTF2_EvtReaderCallbacks*     evtReaderCallbacks,
    OTF2_EvtReaderCallback_Leave leaveCallback );


/** @brief Callback for the MpiSend event record.
 *
 *  A MpiSend record indicates that a MPI message send process was
 *  initiated (MPI_SEND). It keeps the necessary information for this
 *  event: receiver of the message, communicator, and the message tag.
 *  You can optionally add further information like the message length
 *  (size of the send buffer).
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param receiver      MPI rank of receiver in @p communicator.
 *  @param communicator  Communicator ID. References a @eref{Comm} definition and
 *                       will be mapped to the global definition if a mapping
 *                       table of type @eref{OTF2_MAPPING_COMM} is available.
 *  @param msgTag        Message tag
 *  @param msgLength     Message length
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiSend )( OTF2_LocationRef    location,
                                      OTF2_TimeStamp      time,
                                      uint64_t            eventPosition,
                                      void*               userData,
                                      OTF2_AttributeList* attributeList,
                                      uint32_t            receiver,
                                      OTF2_CommRef        communicator,
                                      uint32_t            msgTag,
                                      uint64_t            msgLength );


/** @brief Registers the callback for the MpiSend event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param mpiSendCallback    Function which should be called for all
 *                            @eref{MpiSend} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiSendCallback(
    OTF2_EvtReaderCallbacks*       evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiSend mpiSendCallback );


/** @brief Callback for the MpiIsend event record.
 *
 *  A MpiIsend record indicates that a MPI message send process was
 *  initiated (MPI_ISEND). It keeps the necessary information for this
 *  event: receiver of the message, communicator, and the message tag.
 *  You can optionally add further information like the message length
 *  (size of the send buffer).
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param receiver      MPI rank of receiver in @p communicator.
 *  @param communicator  Communicator ID. References a @eref{Comm} definition and
 *                       will be mapped to the global definition if a mapping
 *                       table of type @eref{OTF2_MAPPING_COMM} is available.
 *  @param msgTag        Message tag
 *  @param msgLength     Message length
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiIsend )( OTF2_LocationRef    location,
                                       OTF2_TimeStamp      time,
                                       uint64_t            eventPosition,
                                       void*               userData,
                                       OTF2_AttributeList* attributeList,
                                       uint32_t            receiver,
                                       OTF2_CommRef        communicator,
                                       uint32_t            msgTag,
                                       uint64_t            msgLength,
                                       uint64_t            requestID );


/** @brief Registers the callback for the MpiIsend event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param mpiIsendCallback   Function which should be called for all
 *                            @eref{MpiIsend} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiIsendCallback(
    OTF2_EvtReaderCallbacks*        evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiIsend mpiIsendCallback );


/** @brief Callback for the MpiIsendComplete event record.
 *
 *  Signals the completion of non-blocking send request.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiIsendComplete )( OTF2_LocationRef    location,
                                               OTF2_TimeStamp      time,
                                               uint64_t            eventPosition,
                                               void*               userData,
                                               OTF2_AttributeList* attributeList,
                                               uint64_t            requestID );


/** @brief Registers the callback for the MpiIsendComplete event.
 *
 *  @param evtReaderCallbacks       Struct for all callbacks.
 *  @param mpiIsendCompleteCallback Function which should be called for all
 *                                  @eref{MpiIsendComplete} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiIsendCompleteCallback(
    OTF2_EvtReaderCallbacks*                evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiIsendComplete mpiIsendCompleteCallback );


/** @brief Callback for the MpiIrecvRequest event record.
 *
 *  Signals the request of a receive, which can be completed later.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param requestID     ID of the requested receive
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiIrecvRequest )( OTF2_LocationRef    location,
                                              OTF2_TimeStamp      time,
                                              uint64_t            eventPosition,
                                              void*               userData,
                                              OTF2_AttributeList* attributeList,
                                              uint64_t            requestID );


/** @brief Registers the callback for the MpiIrecvRequest event.
 *
 *  @param evtReaderCallbacks      Struct for all callbacks.
 *  @param mpiIrecvRequestCallback Function which should be called for all
 *                                 @eref{MpiIrecvRequest} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiIrecvRequestCallback(
    OTF2_EvtReaderCallbacks*               evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiIrecvRequest mpiIrecvRequestCallback );


/** @brief Callback for the MpiRecv event record.
 *
 *  A MpiRecv record indicates that a MPI message was received (MPI_RECV).
 *  It keeps the necessary information for this event: sender of the
 *  message, communicator, and the message tag. You can optionally add
 *  further information like the message length (size of the receive
 *  buffer).
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param sender        MPI rank of sender in @p communicator.
 *  @param communicator  Communicator ID. References a @eref{Comm} definition and
 *                       will be mapped to the global definition if a mapping
 *                       table of type @eref{OTF2_MAPPING_COMM} is available.
 *  @param msgTag        Message tag
 *  @param msgLength     Message length
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiRecv )( OTF2_LocationRef    location,
                                      OTF2_TimeStamp      time,
                                      uint64_t            eventPosition,
                                      void*               userData,
                                      OTF2_AttributeList* attributeList,
                                      uint32_t            sender,
                                      OTF2_CommRef        communicator,
                                      uint32_t            msgTag,
                                      uint64_t            msgLength );


/** @brief Registers the callback for the MpiRecv event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param mpiRecvCallback    Function which should be called for all
 *                            @eref{MpiRecv} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiRecvCallback(
    OTF2_EvtReaderCallbacks*       evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiRecv mpiRecvCallback );


/** @brief Callback for the MpiIrecv event record.
 *
 *  A MpiIrecv record indicates that a MPI message was received
 *  (MPI_IRECV). It keeps the necessary information for this event:
 *  sender of the message, communicator, and the message tag. You can
 *  optionally add further information like the message length (size
 *  of the receive buffer).
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param sender        MPI rank of sender in @p communicator.
 *  @param communicator  Communicator ID. References a @eref{Comm} definition and
 *                       will be mapped to the global definition if a mapping
 *                       table of type @eref{OTF2_MAPPING_COMM} is available.
 *  @param msgTag        Message tag
 *  @param msgLength     Message length
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiIrecv )( OTF2_LocationRef    location,
                                       OTF2_TimeStamp      time,
                                       uint64_t            eventPosition,
                                       void*               userData,
                                       OTF2_AttributeList* attributeList,
                                       uint32_t            sender,
                                       OTF2_CommRef        communicator,
                                       uint32_t            msgTag,
                                       uint64_t            msgLength,
                                       uint64_t            requestID );


/** @brief Registers the callback for the MpiIrecv event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param mpiIrecvCallback   Function which should be called for all
 *                            @eref{MpiIrecv} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiIrecvCallback(
    OTF2_EvtReaderCallbacks*        evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiIrecv mpiIrecvCallback );


/** @brief Callback for the MpiRequestTest event record.
 *
 *  This events appears if the program tests if a request has already
 *  completed but the test failed.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiRequestTest )( OTF2_LocationRef    location,
                                             OTF2_TimeStamp      time,
                                             uint64_t            eventPosition,
                                             void*               userData,
                                             OTF2_AttributeList* attributeList,
                                             uint64_t            requestID );


/** @brief Registers the callback for the MpiRequestTest event.
 *
 *  @param evtReaderCallbacks     Struct for all callbacks.
 *  @param mpiRequestTestCallback Function which should be called for all
 *                                @eref{MpiRequestTest} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiRequestTestCallback(
    OTF2_EvtReaderCallbacks*              evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiRequestTest mpiRequestTestCallback );


/** @brief Callback for the MpiRequestCancelled event record.
 *
 *  This events appears if the program canceled a request.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param requestID     ID of the related request
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiRequestCancelled )( OTF2_LocationRef    location,
                                                  OTF2_TimeStamp      time,
                                                  uint64_t            eventPosition,
                                                  void*               userData,
                                                  OTF2_AttributeList* attributeList,
                                                  uint64_t            requestID );


/** @brief Registers the callback for the MpiRequestCancelled event.
 *
 *  @param evtReaderCallbacks          Struct for all callbacks.
 *  @param mpiRequestCancelledCallback Function which should be called for all
 *                                     @eref{MpiRequestCancelled}
 *                                     definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiRequestCancelledCallback(
    OTF2_EvtReaderCallbacks*                   evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiRequestCancelled mpiRequestCancelledCallback );


/** @brief Callback for the MpiCollectiveBegin event record.
 *
 *  A MpiCollectiveBegin record marks the begin of a MPI collective
 *  operation (MPI_GATHER, MPI_SCATTER etc.).
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiCollectiveBegin )( OTF2_LocationRef    location,
                                                 OTF2_TimeStamp      time,
                                                 uint64_t            eventPosition,
                                                 void*               userData,
                                                 OTF2_AttributeList* attributeList );


/** @brief Registers the callback for the MpiCollectiveBegin event.
 *
 *  @param evtReaderCallbacks         Struct for all callbacks.
 *  @param mpiCollectiveBeginCallback Function which should be called for all
 *                                    @eref{MpiCollectiveBegin} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiCollectiveBeginCallback(
    OTF2_EvtReaderCallbacks*                  evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiCollectiveBegin mpiCollectiveBeginCallback );


/** @brief Callback for the MpiCollectiveEnd event record.
 *
 *  A MpiCollectiveEnd record marks the end of a MPI collective operation
 *  (MPI_GATHER, MPI_SCATTER etc.). It keeps the necessary information
 *  for this event: type of collective operation, communicator, the
 *  root of this collective operation. You can optionally add further
 *  information like sent and received bytes.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param collectiveOp  Determines which collective operation it is.
 *  @param communicator  Communicator References a @eref{Comm} definition and
 *                       will be mapped to the global definition if a mapping
 *                       table of type @eref{OTF2_MAPPING_COMM} is available.
 *  @param root          MPI rank of root in @p communicator or
 *                       @eref{OTF2_UNDEFINED_UINT32} if the call has no root
 *                       rank.
 *  @param sizeSent      Size of the sent message.
 *  @param sizeReceived  Size of the received message.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_MpiCollectiveEnd )( OTF2_LocationRef    location,
                                               OTF2_TimeStamp      time,
                                               uint64_t            eventPosition,
                                               void*               userData,
                                               OTF2_AttributeList* attributeList,
                                               OTF2_CollectiveOp   collectiveOp,
                                               OTF2_CommRef        communicator,
                                               uint32_t            root,
                                               uint64_t            sizeSent,
                                               uint64_t            sizeReceived );


/** @brief Registers the callback for the MpiCollectiveEnd event.
 *
 *  @param evtReaderCallbacks       Struct for all callbacks.
 *  @param mpiCollectiveEndCallback Function which should be called for all
 *                                  @eref{MpiCollectiveEnd} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMpiCollectiveEndCallback(
    OTF2_EvtReaderCallbacks*                evtReaderCallbacks,
    OTF2_EvtReaderCallback_MpiCollectiveEnd mpiCollectiveEndCallback );


/** @brief Callback for the OmpFork event record.
 *
 *  An OmpFork record marks that an OpenMP Thread forks a thread team.
 *
 *  This event record is superseded by the @eref{ThreadFork} event record
 *  and should not be used when the @eref{ThreadFork} event record is
 *  in use.
 *
 *  @param location                 The location where this event happened.
 *  @param time                     The time when this event happened.
 *  @param eventPosition            The event position of this event in the
 *                                  trace. Starting with 1.
 *  @param userData                 User data as set by
 *                                  @eref{OTF2_Reader_RegisterEvtCallbacks}
 *                                  or @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList            Additional attributes for this event.
 *  @param numberOfRequestedThreads Requested size of the team.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_OmpFork )( OTF2_LocationRef    location,
                                      OTF2_TimeStamp      time,
                                      uint64_t            eventPosition,
                                      void*               userData,
                                      OTF2_AttributeList* attributeList,
                                      uint32_t            numberOfRequestedThreads );


/** @brief Registers the callback for the OmpFork event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param ompForkCallback    Function which should be called for all
 *                            @eref{OmpFork} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetOmpForkCallback(
    OTF2_EvtReaderCallbacks*       evtReaderCallbacks,
    OTF2_EvtReaderCallback_OmpFork ompForkCallback );


/** @brief Callback for the OmpJoin event record.
 *
 *  An OmpJoin record marks that a team of threads is joint and only the
 *  master thread continues execution.
 *
 *  This event record is superseded by the @eref{ThreadJoin} event record
 *  and should not be used when the @eref{ThreadJoin} event record is
 *  in use.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_OmpJoin )( OTF2_LocationRef    location,
                                      OTF2_TimeStamp      time,
                                      uint64_t            eventPosition,
                                      void*               userData,
                                      OTF2_AttributeList* attributeList );


/** @brief Registers the callback for the OmpJoin event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param ompJoinCallback    Function which should be called for all
 *                            @eref{OmpJoin} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetOmpJoinCallback(
    OTF2_EvtReaderCallbacks*       evtReaderCallbacks,
    OTF2_EvtReaderCallback_OmpJoin ompJoinCallback );


/** @brief Callback for the OmpAcquireLock event record.
 *
 *  An OmpAcquireLock record marks that a thread acquires an OpenMP lock.
 *
 *  This event record is superseded by the @eref{ThreadAcquireLock} event
 *  record and should not be used when the @eref{ThreadAcquireLock}
 *  event record is in use.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param lockID           ID of the lock.
 *  @param acquisitionOrder A monotonically increasing number to determine the
 *                          order of lock acquisitions (with unsynchronized
 *                          clocks this is otherwise not possible).
 *                          Corresponding acquire-release events have same
 *                          number.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_OmpAcquireLock )( OTF2_LocationRef    location,
                                             OTF2_TimeStamp      time,
                                             uint64_t            eventPosition,
                                             void*               userData,
                                             OTF2_AttributeList* attributeList,
                                             uint32_t            lockID,
                                             uint32_t            acquisitionOrder );


/** @brief Registers the callback for the OmpAcquireLock event.
 *
 *  @param evtReaderCallbacks     Struct for all callbacks.
 *  @param ompAcquireLockCallback Function which should be called for all
 *                                @eref{OmpAcquireLock} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetOmpAcquireLockCallback(
    OTF2_EvtReaderCallbacks*              evtReaderCallbacks,
    OTF2_EvtReaderCallback_OmpAcquireLock ompAcquireLockCallback );


/** @brief Callback for the OmpReleaseLock event record.
 *
 *  An OmpReleaseLock record marks that a thread releases an OpenMP lock.
 *
 *  This event record is superseded by the @eref{ThreadReleaseLock} event
 *  record and should not be used when the @eref{ThreadReleaseLock}
 *  event record is in use.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param lockID           ID of the lock.
 *  @param acquisitionOrder A monotonically increasing number to determine the
 *                          order of lock acquisitions (with unsynchronized
 *                          clocks this is otherwise not possible).
 *                          Corresponding acquire-release events have same
 *                          number.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_OmpReleaseLock )( OTF2_LocationRef    location,
                                             OTF2_TimeStamp      time,
                                             uint64_t            eventPosition,
                                             void*               userData,
                                             OTF2_AttributeList* attributeList,
                                             uint32_t            lockID,
                                             uint32_t            acquisitionOrder );


/** @brief Registers the callback for the OmpReleaseLock event.
 *
 *  @param evtReaderCallbacks     Struct for all callbacks.
 *  @param ompReleaseLockCallback Function which should be called for all
 *                                @eref{OmpReleaseLock} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetOmpReleaseLockCallback(
    OTF2_EvtReaderCallbacks*              evtReaderCallbacks,
    OTF2_EvtReaderCallback_OmpReleaseLock ompReleaseLockCallback );


/** @brief Callback for the OmpTaskCreate event record.
 *
 *  An OmpTaskCreate record marks that an OpenMP Task was/will be created
 *  in the current region.
 *
 *  This event record is superseded by the @eref{ThreadTaskCreate} event
 *  record and should not be used when the @eref{ThreadTaskCreate}
 *  event record is in use.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param taskID        Identifier of the newly created task instance.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_OmpTaskCreate )( OTF2_LocationRef    location,
                                            OTF2_TimeStamp      time,
                                            uint64_t            eventPosition,
                                            void*               userData,
                                            OTF2_AttributeList* attributeList,
                                            uint64_t            taskID );


/** @brief Registers the callback for the OmpTaskCreate event.
 *
 *  @param evtReaderCallbacks    Struct for all callbacks.
 *  @param ompTaskCreateCallback Function which should be called for all
 *                               @eref{OmpTaskCreate} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetOmpTaskCreateCallback(
    OTF2_EvtReaderCallbacks*             evtReaderCallbacks,
    OTF2_EvtReaderCallback_OmpTaskCreate ompTaskCreateCallback );


/** @brief Callback for the OmpTaskSwitch event record.
 *
 *  An OmpTaskSwitch record indicates that the execution of the current
 *  task will be suspended and another task starts/restarts its
 *  execution. Please note that this may change the current call stack
 *  of the executing location.
 *
 *  This event record is superseded by the @eref{ThreadTaskSwitch} event
 *  record and should not be used when the @eref{ThreadTaskSwitch}
 *  event record is in use.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param taskID        Identifier of the now active task instance.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_OmpTaskSwitch )( OTF2_LocationRef    location,
                                            OTF2_TimeStamp      time,
                                            uint64_t            eventPosition,
                                            void*               userData,
                                            OTF2_AttributeList* attributeList,
                                            uint64_t            taskID );


/** @brief Registers the callback for the OmpTaskSwitch event.
 *
 *  @param evtReaderCallbacks    Struct for all callbacks.
 *  @param ompTaskSwitchCallback Function which should be called for all
 *                               @eref{OmpTaskSwitch} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetOmpTaskSwitchCallback(
    OTF2_EvtReaderCallbacks*             evtReaderCallbacks,
    OTF2_EvtReaderCallback_OmpTaskSwitch ompTaskSwitchCallback );


/** @brief Callback for the OmpTaskComplete event record.
 *
 *  An OmpTaskComplete record indicates that the execution of an OpenMP
 *  task has finished.
 *
 *  This event record is superseded by the @eref{ThreadTaskComplete} event
 *  record and should not be used when the @eref{ThreadTaskComplete}
 *  event record is in use.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param taskID        Identifier of the completed task instance.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_OmpTaskComplete )( OTF2_LocationRef    location,
                                              OTF2_TimeStamp      time,
                                              uint64_t            eventPosition,
                                              void*               userData,
                                              OTF2_AttributeList* attributeList,
                                              uint64_t            taskID );


/** @brief Registers the callback for the OmpTaskComplete event.
 *
 *  @param evtReaderCallbacks      Struct for all callbacks.
 *  @param ompTaskCompleteCallback Function which should be called for all
 *                                 @eref{OmpTaskComplete} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetOmpTaskCompleteCallback(
    OTF2_EvtReaderCallbacks*               evtReaderCallbacks,
    OTF2_EvtReaderCallback_OmpTaskComplete ompTaskCompleteCallback );


/** @brief Callback for the Metric event record.
 *
 *  A metric event is always stored at the location that recorded the
 *  metric. A metric event can reference a metric class or metric
 *  instance. Therefore, metric classes and instances share same ID
 *  space. Synchronous metrics are always located right before the
 *  according enter and leave event.
 *
 *  @param location        The location where this event happened.
 *  @param time            The time when this event happened.
 *  @param eventPosition   The event position of this event in the trace.
 *                         Starting with 1.
 *  @param userData        User data as set by
 *                         @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                         @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList   Additional attributes for this event.
 *  @param metric          Could be a metric class or a metric instance.
 *                         References a @eref{MetricClass}, or a
 *                         @eref{MetricInstance} definition and will be
 *                         mapped to the global definition if a mapping table
 *                         of type @eref{OTF2_MAPPING_METRIC} is available.
 *  @param numberOfMetrics Number of metrics with in the set.
 *  @param typeIDs         List of metric types. These types must match that of
 *                         the corresponding @eref{MetricMember} definitions.
 *  @param metricValues    List of metric values.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_Metric )( OTF2_LocationRef        location,
                                     OTF2_TimeStamp          time,
                                     uint64_t                eventPosition,
                                     void*                   userData,
                                     OTF2_AttributeList*     attributeList,
                                     OTF2_MetricRef          metric,
                                     uint8_t                 numberOfMetrics,
                                     const OTF2_Type*        typeIDs,
                                     const OTF2_MetricValue* metricValues );


/** @brief Registers the callback for the Metric event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param metricCallback     Function which should be called for all
 *                            @eref{Metric} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetMetricCallback(
    OTF2_EvtReaderCallbacks*      evtReaderCallbacks,
    OTF2_EvtReaderCallback_Metric metricCallback );


/** @brief Callback for the ParameterString event record.
 *
 *  A ParameterString record marks that in the current region, the
 *  specified string parameter has the specified value.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param parameter     Parameter ID. References a @eref{Parameter} definition
 *                       and will be mapped to the global definition if a
 *                       mapping table of type @eref{OTF2_MAPPING_PARAMETER}
 *                       is available.
 *  @param string        Value: Handle of a string definition References a
 *                       @eref{String} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_STRING} is available.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ParameterString )( OTF2_LocationRef    location,
                                              OTF2_TimeStamp      time,
                                              uint64_t            eventPosition,
                                              void*               userData,
                                              OTF2_AttributeList* attributeList,
                                              OTF2_ParameterRef   parameter,
                                              OTF2_StringRef      string );


/** @brief Registers the callback for the ParameterString event.
 *
 *  @param evtReaderCallbacks      Struct for all callbacks.
 *  @param parameterStringCallback Function which should be called for all
 *                                 @eref{ParameterString} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetParameterStringCallback(
    OTF2_EvtReaderCallbacks*               evtReaderCallbacks,
    OTF2_EvtReaderCallback_ParameterString parameterStringCallback );


/** @brief Callback for the ParameterInt event record.
 *
 *  A ParameterInt record marks that in the current region, the specified
 *  integer parameter has the specified value.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param parameter     Parameter ID. References a @eref{Parameter} definition
 *                       and will be mapped to the global definition if a
 *                       mapping table of type @eref{OTF2_MAPPING_PARAMETER}
 *                       is available.
 *  @param value         Value of the recorded parameter.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ParameterInt )( OTF2_LocationRef    location,
                                           OTF2_TimeStamp      time,
                                           uint64_t            eventPosition,
                                           void*               userData,
                                           OTF2_AttributeList* attributeList,
                                           OTF2_ParameterRef   parameter,
                                           int64_t             value );


/** @brief Registers the callback for the ParameterInt event.
 *
 *  @param evtReaderCallbacks   Struct for all callbacks.
 *  @param parameterIntCallback Function which should be called for all
 *                              @eref{ParameterInt} definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetParameterIntCallback(
    OTF2_EvtReaderCallbacks*            evtReaderCallbacks,
    OTF2_EvtReaderCallback_ParameterInt parameterIntCallback );


/** @brief Callback for the ParameterUnsignedInt event record.
 *
 *  A ParameterUnsignedInt record marks that in the current region, the
 *  specified unsigned integer parameter has the specified value.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param parameter     Parameter ID. References a @eref{Parameter} definition
 *                       and will be mapped to the global definition if a
 *                       mapping table of type @eref{OTF2_MAPPING_PARAMETER}
 *                       is available.
 *  @param value         Value of the recorded parameter.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ParameterUnsignedInt )( OTF2_LocationRef    location,
                                                   OTF2_TimeStamp      time,
                                                   uint64_t            eventPosition,
                                                   void*               userData,
                                                   OTF2_AttributeList* attributeList,
                                                   OTF2_ParameterRef   parameter,
                                                   uint64_t            value );


/** @brief Registers the callback for the ParameterUnsignedInt event.
 *
 *  @param evtReaderCallbacks           Struct for all callbacks.
 *  @param parameterUnsignedIntCallback Function which should be called for all
 *                                      @eref{ParameterUnsignedInt}
 *                                      definitions.
 *
 *  @since Version 1.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetParameterUnsignedIntCallback(
    OTF2_EvtReaderCallbacks*                    evtReaderCallbacks,
    OTF2_EvtReaderCallback_ParameterUnsignedInt parameterUnsignedIntCallback );


/** @brief Callback for the RmaWinCreate event record.
 *
 *  A RmaWinCreate record denotes the creation of a RMA window.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window created. References a @eref{RmaWin}
 *                       definition and will be mapped to the global
 *                       definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaWinCreate )( OTF2_LocationRef    location,
                                           OTF2_TimeStamp      time,
                                           uint64_t            eventPosition,
                                           void*               userData,
                                           OTF2_AttributeList* attributeList,
                                           OTF2_RmaWinRef      win );


/** @brief Registers the callback for the RmaWinCreate event.
 *
 *  @param evtReaderCallbacks   Struct for all callbacks.
 *  @param rmaWinCreateCallback Function which should be called for all
 *                              @eref{RmaWinCreate} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaWinCreateCallback(
    OTF2_EvtReaderCallbacks*            evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaWinCreate rmaWinCreateCallback );


/** @brief Callback for the RmaWinDestroy event record.
 *
 *  A RmaWinDestroy record denotes the destruction of a RMA window.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window destructed. References a @eref{RmaWin}
 *                       definition and will be mapped to the global
 *                       definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaWinDestroy )( OTF2_LocationRef    location,
                                            OTF2_TimeStamp      time,
                                            uint64_t            eventPosition,
                                            void*               userData,
                                            OTF2_AttributeList* attributeList,
                                            OTF2_RmaWinRef      win );


/** @brief Registers the callback for the RmaWinDestroy event.
 *
 *  @param evtReaderCallbacks    Struct for all callbacks.
 *  @param rmaWinDestroyCallback Function which should be called for all
 *                               @eref{RmaWinDestroy} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaWinDestroyCallback(
    OTF2_EvtReaderCallbacks*             evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaWinDestroy rmaWinDestroyCallback );


/** @brief Callback for the RmaCollectiveBegin event record.
 *
 *  A RmaCollectiveBegin record denotes the beginning of a collective RMA
 *  operation.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaCollectiveBegin )( OTF2_LocationRef    location,
                                                 OTF2_TimeStamp      time,
                                                 uint64_t            eventPosition,
                                                 void*               userData,
                                                 OTF2_AttributeList* attributeList );


/** @brief Registers the callback for the RmaCollectiveBegin event.
 *
 *  @param evtReaderCallbacks         Struct for all callbacks.
 *  @param rmaCollectiveBeginCallback Function which should be called for all
 *                                    @eref{RmaCollectiveBegin} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaCollectiveBeginCallback(
    OTF2_EvtReaderCallbacks*                  evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaCollectiveBegin rmaCollectiveBeginCallback );


/** @brief Callback for the RmaCollectiveEnd event record.
 *
 *  A RmaCollectiveEnd record denotes the end of a collective RMA
 *  operation.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param collectiveOp  Determines which collective operation it is.
 *  @param syncLevel     Synchronization level of this collective operation.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param root          Root process for this operation or
 *                       @eref{OTF2_UNDEFINED_UINT32} if the call has no root
 *                       rank.
 *  @param bytesSent     Bytes sent in operation.
 *  @param bytesReceived Bytes receives in operation.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaCollectiveEnd )( OTF2_LocationRef    location,
                                               OTF2_TimeStamp      time,
                                               uint64_t            eventPosition,
                                               void*               userData,
                                               OTF2_AttributeList* attributeList,
                                               OTF2_CollectiveOp   collectiveOp,
                                               OTF2_RmaSyncLevel   syncLevel,
                                               OTF2_RmaWinRef      win,
                                               uint32_t            root,
                                               uint64_t            bytesSent,
                                               uint64_t            bytesReceived );


/** @brief Registers the callback for the RmaCollectiveEnd event.
 *
 *  @param evtReaderCallbacks       Struct for all callbacks.
 *  @param rmaCollectiveEndCallback Function which should be called for all
 *                                  @eref{RmaCollectiveEnd} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaCollectiveEndCallback(
    OTF2_EvtReaderCallbacks*                evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaCollectiveEnd rmaCollectiveEndCallback );


/** @brief Callback for the RmaGroupSync event record.
 *
 *  A RmaGroupSync record denotes the synchronization with a subgroup of
 *  processes on a window.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param syncLevel     Synchronization level of this collective operation.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param group         Group of remote processes involved in synchronization.
 *                       References a @eref{Group} definition and will be
 *                       mapped to the global definition if a mapping table
 *                       of type @eref{OTF2_MAPPING_GROUP} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaGroupSync )( OTF2_LocationRef    location,
                                           OTF2_TimeStamp      time,
                                           uint64_t            eventPosition,
                                           void*               userData,
                                           OTF2_AttributeList* attributeList,
                                           OTF2_RmaSyncLevel   syncLevel,
                                           OTF2_RmaWinRef      win,
                                           OTF2_GroupRef       group );


/** @brief Registers the callback for the RmaGroupSync event.
 *
 *  @param evtReaderCallbacks   Struct for all callbacks.
 *  @param rmaGroupSyncCallback Function which should be called for all
 *                              @eref{RmaGroupSync} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaGroupSyncCallback(
    OTF2_EvtReaderCallbacks*            evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaGroupSync rmaGroupSyncCallback );


/** @brief Callback for the RmaRequestLock event record.
 *
 *  A RmaRequestLock record denotes the time a lock was requested and with
 *  it the earliest time it could have been granted.  It is used to
 *  mark (possibly) non-blocking lock request, as defined by the MPI
 *  standard.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process or
 *                       @eref{OTF2_UNDEFINED_UINT32} if all processes of the
 *                       specified window are locked.
 *  @param lockId        ID of the lock acquired, if multiple locks are defined
 *                       on a window.
 *  @param lockType      Type of lock acquired.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaRequestLock )( OTF2_LocationRef    location,
                                             OTF2_TimeStamp      time,
                                             uint64_t            eventPosition,
                                             void*               userData,
                                             OTF2_AttributeList* attributeList,
                                             OTF2_RmaWinRef      win,
                                             uint32_t            remote,
                                             uint64_t            lockId,
                                             OTF2_LockType       lockType );


/** @brief Registers the callback for the RmaRequestLock event.
 *
 *  @param evtReaderCallbacks     Struct for all callbacks.
 *  @param rmaRequestLockCallback Function which should be called for all
 *                                @eref{RmaRequestLock} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaRequestLockCallback(
    OTF2_EvtReaderCallbacks*              evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaRequestLock rmaRequestLockCallback );


/** @brief Callback for the RmaAcquireLock event record.
 *
 *  A RmaAcquireLock record denotes the time a lock was acquired by the
 *  process.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process or
 *                       @eref{OTF2_UNDEFINED_UINT32} if all processes of the
 *                       specified window are locked.
 *  @param lockId        ID of the lock acquired, if multiple locks are defined
 *                       on a window.
 *  @param lockType      Type of lock acquired.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaAcquireLock )( OTF2_LocationRef    location,
                                             OTF2_TimeStamp      time,
                                             uint64_t            eventPosition,
                                             void*               userData,
                                             OTF2_AttributeList* attributeList,
                                             OTF2_RmaWinRef      win,
                                             uint32_t            remote,
                                             uint64_t            lockId,
                                             OTF2_LockType       lockType );


/** @brief Registers the callback for the RmaAcquireLock event.
 *
 *  @param evtReaderCallbacks     Struct for all callbacks.
 *  @param rmaAcquireLockCallback Function which should be called for all
 *                                @eref{RmaAcquireLock} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaAcquireLockCallback(
    OTF2_EvtReaderCallbacks*              evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaAcquireLock rmaAcquireLockCallback );


/** @brief Callback for the RmaTryLock event record.
 *
 *  A RmaTryLock record denotes the time of an unsuccessful attempt to
 *  acquire the lock.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process or
 *                       @eref{OTF2_UNDEFINED_UINT32} if all processes of the
 *                       specified window are locked.
 *  @param lockId        ID of the lock acquired, if multiple locks are defined
 *                       on a window.
 *  @param lockType      Type of lock acquired.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaTryLock )( OTF2_LocationRef    location,
                                         OTF2_TimeStamp      time,
                                         uint64_t            eventPosition,
                                         void*               userData,
                                         OTF2_AttributeList* attributeList,
                                         OTF2_RmaWinRef      win,
                                         uint32_t            remote,
                                         uint64_t            lockId,
                                         OTF2_LockType       lockType );


/** @brief Registers the callback for the RmaTryLock event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param rmaTryLockCallback Function which should be called for all
 *                            @eref{RmaTryLock} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaTryLockCallback(
    OTF2_EvtReaderCallbacks*          evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaTryLock rmaTryLockCallback );


/** @brief Callback for the RmaReleaseLock event record.
 *
 *  A RmaReleaseLock record denotes the time the lock was released.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process or
 *                       @eref{OTF2_UNDEFINED_UINT32} if all processes of the
 *                       specified window are locked.
 *  @param lockId        ID of the lock released, if multiple locks are defined
 *                       on a window.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaReleaseLock )( OTF2_LocationRef    location,
                                             OTF2_TimeStamp      time,
                                             uint64_t            eventPosition,
                                             void*               userData,
                                             OTF2_AttributeList* attributeList,
                                             OTF2_RmaWinRef      win,
                                             uint32_t            remote,
                                             uint64_t            lockId );


/** @brief Registers the callback for the RmaReleaseLock event.
 *
 *  @param evtReaderCallbacks     Struct for all callbacks.
 *  @param rmaReleaseLockCallback Function which should be called for all
 *                                @eref{RmaReleaseLock} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaReleaseLockCallback(
    OTF2_EvtReaderCallbacks*              evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaReleaseLock rmaReleaseLockCallback );


/** @brief Callback for the RmaSync event record.
 *
 *  A RmaSync record denotes the direct synchronization with a possibly
 *  remote process.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the locked remote process.
 *  @param syncType      Type of synchronization.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaSync )( OTF2_LocationRef    location,
                                      OTF2_TimeStamp      time,
                                      uint64_t            eventPosition,
                                      void*               userData,
                                      OTF2_AttributeList* attributeList,
                                      OTF2_RmaWinRef      win,
                                      uint32_t            remote,
                                      OTF2_RmaSyncType    syncType );


/** @brief Registers the callback for the RmaSync event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param rmaSyncCallback    Function which should be called for all
 *                            @eref{RmaSync} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaSyncCallback(
    OTF2_EvtReaderCallbacks*       evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaSync rmaSyncCallback );


/** @brief Callback for the RmaWaitChange event record.
 *
 *  A RmaWaitChange record denotes the change of a window that was waited
 *  for.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaWaitChange )( OTF2_LocationRef    location,
                                            OTF2_TimeStamp      time,
                                            uint64_t            eventPosition,
                                            void*               userData,
                                            OTF2_AttributeList* attributeList,
                                            OTF2_RmaWinRef      win );


/** @brief Registers the callback for the RmaWaitChange event.
 *
 *  @param evtReaderCallbacks    Struct for all callbacks.
 *  @param rmaWaitChangeCallback Function which should be called for all
 *                               @eref{RmaWaitChange} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaWaitChangeCallback(
    OTF2_EvtReaderCallbacks*             evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaWaitChange rmaWaitChangeCallback );


/** @brief Callback for the RmaPut event record.
 *
 *  A RmaPut record denotes the time a put operation was issued.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the target process.
 *  @param bytes         Bytes sent to target.
 *  @param matchingId    ID used for matching the corresponding completion
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaPut )( OTF2_LocationRef    location,
                                     OTF2_TimeStamp      time,
                                     uint64_t            eventPosition,
                                     void*               userData,
                                     OTF2_AttributeList* attributeList,
                                     OTF2_RmaWinRef      win,
                                     uint32_t            remote,
                                     uint64_t            bytes,
                                     uint64_t            matchingId );


/** @brief Registers the callback for the RmaPut event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param rmaPutCallback     Function which should be called for all
 *                            @eref{RmaPut} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaPutCallback(
    OTF2_EvtReaderCallbacks*      evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaPut rmaPutCallback );


/** @brief Callback for the RmaGet event record.
 *
 *  A RmaGet record denotes the time a get operation was issued.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the target process.
 *  @param bytes         Bytes received from target.
 *  @param matchingId    ID used for matching the corresponding completion
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaGet )( OTF2_LocationRef    location,
                                     OTF2_TimeStamp      time,
                                     uint64_t            eventPosition,
                                     void*               userData,
                                     OTF2_AttributeList* attributeList,
                                     OTF2_RmaWinRef      win,
                                     uint32_t            remote,
                                     uint64_t            bytes,
                                     uint64_t            matchingId );


/** @brief Registers the callback for the RmaGet event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param rmaGetCallback     Function which should be called for all
 *                            @eref{RmaGet} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaGetCallback(
    OTF2_EvtReaderCallbacks*      evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaGet rmaGetCallback );


/** @brief Callback for the RmaAtomic event record.
 *
 *  A RmaAtomic record denotes the time a atomic operation was issued.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param remote        Rank of the target process.
 *  @param type          Type of atomic operation.
 *  @param bytesSent     Bytes sent to target.
 *  @param bytesReceived Bytes received from target.
 *  @param matchingId    ID used for matching the corresponding completion
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaAtomic )( OTF2_LocationRef    location,
                                        OTF2_TimeStamp      time,
                                        uint64_t            eventPosition,
                                        void*               userData,
                                        OTF2_AttributeList* attributeList,
                                        OTF2_RmaWinRef      win,
                                        uint32_t            remote,
                                        OTF2_RmaAtomicType  type,
                                        uint64_t            bytesSent,
                                        uint64_t            bytesReceived,
                                        uint64_t            matchingId );


/** @brief Registers the callback for the RmaAtomic event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param rmaAtomicCallback  Function which should be called for all
 *                            @eref{RmaAtomic} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaAtomicCallback(
    OTF2_EvtReaderCallbacks*         evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaAtomic rmaAtomicCallback );


/** @brief Callback for the RmaOpCompleteBlocking event record.
 *
 *  A RmaOpCompleteBlocking record denotes the local completion of a
 *  blocking RMA operation.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param matchingId    ID used for matching the corresponding RMA operation
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaOpCompleteBlocking )( OTF2_LocationRef    location,
                                                    OTF2_TimeStamp      time,
                                                    uint64_t            eventPosition,
                                                    void*               userData,
                                                    OTF2_AttributeList* attributeList,
                                                    OTF2_RmaWinRef      win,
                                                    uint64_t            matchingId );


/** @brief Registers the callback for the RmaOpCompleteBlocking event.
 *
 *  @param evtReaderCallbacks            Struct for all callbacks.
 *  @param rmaOpCompleteBlockingCallback Function which should be called for all
 *                                       @eref{RmaOpCompleteBlocking}
 *                                       definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaOpCompleteBlockingCallback(
    OTF2_EvtReaderCallbacks*                     evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaOpCompleteBlocking rmaOpCompleteBlockingCallback );


/** @brief Callback for the RmaOpCompleteNonBlocking event record.
 *
 *  A RmaOpCompleteNonBlocking record denotes the local completion of a
 *  non-blocking RMA operation.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param matchingId    ID used for matching the corresponding RMA operation
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaOpCompleteNonBlocking )( OTF2_LocationRef    location,
                                                       OTF2_TimeStamp      time,
                                                       uint64_t            eventPosition,
                                                       void*               userData,
                                                       OTF2_AttributeList* attributeList,
                                                       OTF2_RmaWinRef      win,
                                                       uint64_t            matchingId );


/** @brief Registers the callback for the RmaOpCompleteNonBlocking event.
 *
 *  @param evtReaderCallbacks               Struct for all callbacks.
 *  @param rmaOpCompleteNonBlockingCallback Function which should be called for
 *                                          all
 *                                          @eref{RmaOpCompleteNonBlocking}
 *                                          definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaOpCompleteNonBlockingCallback(
    OTF2_EvtReaderCallbacks*                        evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaOpCompleteNonBlocking rmaOpCompleteNonBlockingCallback );


/** @brief Callback for the RmaOpTest event record.
 *
 *  A RmaOpTest record denotes that a non-blocking RMA operation has been
 *  tested for completion unsuccessfully.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param matchingId    ID used for matching the corresponding RMA operation
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaOpTest )( OTF2_LocationRef    location,
                                        OTF2_TimeStamp      time,
                                        uint64_t            eventPosition,
                                        void*               userData,
                                        OTF2_AttributeList* attributeList,
                                        OTF2_RmaWinRef      win,
                                        uint64_t            matchingId );


/** @brief Registers the callback for the RmaOpTest event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param rmaOpTestCallback  Function which should be called for all
 *                            @eref{RmaOpTest} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaOpTestCallback(
    OTF2_EvtReaderCallbacks*         evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaOpTest rmaOpTestCallback );


/** @brief Callback for the RmaOpCompleteRemote event record.
 *
 *  A RmaOpCompleteRemote record denotes the remote completion of a RMA
 *  operation.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param win           ID of the window used for this operation. References a
 *                       @eref{RmaWin} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_RMA_WIN} is available.
 *  @param matchingId    ID used for matching the corresponding RMA operation
 *                       record.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_RmaOpCompleteRemote )( OTF2_LocationRef    location,
                                                  OTF2_TimeStamp      time,
                                                  uint64_t            eventPosition,
                                                  void*               userData,
                                                  OTF2_AttributeList* attributeList,
                                                  OTF2_RmaWinRef      win,
                                                  uint64_t            matchingId );


/** @brief Registers the callback for the RmaOpCompleteRemote event.
 *
 *  @param evtReaderCallbacks          Struct for all callbacks.
 *  @param rmaOpCompleteRemoteCallback Function which should be called for all
 *                                     @eref{RmaOpCompleteRemote}
 *                                     definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetRmaOpCompleteRemoteCallback(
    OTF2_EvtReaderCallbacks*                   evtReaderCallbacks,
    OTF2_EvtReaderCallback_RmaOpCompleteRemote rmaOpCompleteRemoteCallback );


/** @brief Callback for the ThreadFork event record.
 *
 *  A ThreadFork record marks that a thread forks a thread team.
 *
 *  @param location                 The location where this event happened.
 *  @param time                     The time when this event happened.
 *  @param eventPosition            The event position of this event in the
 *                                  trace. Starting with 1.
 *  @param userData                 User data as set by
 *                                  @eref{OTF2_Reader_RegisterEvtCallbacks}
 *                                  or @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList            Additional attributes for this event.
 *  @param model                    The threading paradigm this event took place.
 *  @param numberOfRequestedThreads Requested size of the team.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadFork )( OTF2_LocationRef    location,
                                         OTF2_TimeStamp      time,
                                         uint64_t            eventPosition,
                                         void*               userData,
                                         OTF2_AttributeList* attributeList,
                                         OTF2_Paradigm       model,
                                         uint32_t            numberOfRequestedThreads );


/** @brief Registers the callback for the ThreadFork event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param threadForkCallback Function which should be called for all
 *                            @eref{ThreadFork} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadForkCallback(
    OTF2_EvtReaderCallbacks*          evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadFork threadForkCallback );


/** @brief Callback for the ThreadJoin event record.
 *
 *  A ThreadJoin record marks that a team of threads is joint and only the
 *  master thread continues execution.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param model         The threading paradigm this event took place.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadJoin )( OTF2_LocationRef    location,
                                         OTF2_TimeStamp      time,
                                         uint64_t            eventPosition,
                                         void*               userData,
                                         OTF2_AttributeList* attributeList,
                                         OTF2_Paradigm       model );


/** @brief Registers the callback for the ThreadJoin event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param threadJoinCallback Function which should be called for all
 *                            @eref{ThreadJoin} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadJoinCallback(
    OTF2_EvtReaderCallbacks*          evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadJoin threadJoinCallback );


/** @brief Callback for the ThreadTeamBegin event record.
 *
 *  The current location enters the specified thread team.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param threadTeam    Thread team References a @eref{Comm} definition and will
 *                       be mapped to the global definition if a mapping
 *                       table of type @eref{OTF2_MAPPING_COMM} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadTeamBegin )( OTF2_LocationRef    location,
                                              OTF2_TimeStamp      time,
                                              uint64_t            eventPosition,
                                              void*               userData,
                                              OTF2_AttributeList* attributeList,
                                              OTF2_CommRef        threadTeam );


/** @brief Registers the callback for the ThreadTeamBegin event.
 *
 *  @param evtReaderCallbacks      Struct for all callbacks.
 *  @param threadTeamBeginCallback Function which should be called for all
 *                                 @eref{ThreadTeamBegin} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadTeamBeginCallback(
    OTF2_EvtReaderCallbacks*               evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadTeamBegin threadTeamBeginCallback );


/** @brief Callback for the ThreadTeamEnd event record.
 *
 *  The current location leaves the specified thread team.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param threadTeam    Thread team References a @eref{Comm} definition and will
 *                       be mapped to the global definition if a mapping
 *                       table of type @eref{OTF2_MAPPING_COMM} is available.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadTeamEnd )( OTF2_LocationRef    location,
                                            OTF2_TimeStamp      time,
                                            uint64_t            eventPosition,
                                            void*               userData,
                                            OTF2_AttributeList* attributeList,
                                            OTF2_CommRef        threadTeam );


/** @brief Registers the callback for the ThreadTeamEnd event.
 *
 *  @param evtReaderCallbacks    Struct for all callbacks.
 *  @param threadTeamEndCallback Function which should be called for all
 *                               @eref{ThreadTeamEnd} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadTeamEndCallback(
    OTF2_EvtReaderCallbacks*             evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadTeamEnd threadTeamEndCallback );


/** @brief Callback for the ThreadAcquireLock event record.
 *
 *  A ThreadAcquireLock record marks that a thread acquires a lock.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param model            The threading paradigm this event took place.
 *  @param lockID           ID of the lock.
 *  @param acquisitionOrder A monotonically increasing number to determine the
 *                          order of lock acquisitions (with unsynchronized
 *                          clocks this is otherwise not possible).
 *                          Corresponding acquire-release events have same
 *                          number.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadAcquireLock )( OTF2_LocationRef    location,
                                                OTF2_TimeStamp      time,
                                                uint64_t            eventPosition,
                                                void*               userData,
                                                OTF2_AttributeList* attributeList,
                                                OTF2_Paradigm       model,
                                                uint32_t            lockID,
                                                uint32_t            acquisitionOrder );


/** @brief Registers the callback for the ThreadAcquireLock event.
 *
 *  @param evtReaderCallbacks        Struct for all callbacks.
 *  @param threadAcquireLockCallback Function which should be called for all
 *                                   @eref{ThreadAcquireLock} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadAcquireLockCallback(
    OTF2_EvtReaderCallbacks*                 evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadAcquireLock threadAcquireLockCallback );


/** @brief Callback for the ThreadReleaseLock event record.
 *
 *  A ThreadReleaseLock record marks that a thread releases a lock.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param model            The threading paradigm this event took place.
 *  @param lockID           ID of the lock.
 *  @param acquisitionOrder A monotonically increasing number to determine the
 *                          order of lock acquisitions (with unsynchronized
 *                          clocks this is otherwise not possible).
 *                          Corresponding acquire-release events have same
 *                          number.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadReleaseLock )( OTF2_LocationRef    location,
                                                OTF2_TimeStamp      time,
                                                uint64_t            eventPosition,
                                                void*               userData,
                                                OTF2_AttributeList* attributeList,
                                                OTF2_Paradigm       model,
                                                uint32_t            lockID,
                                                uint32_t            acquisitionOrder );


/** @brief Registers the callback for the ThreadReleaseLock event.
 *
 *  @param evtReaderCallbacks        Struct for all callbacks.
 *  @param threadReleaseLockCallback Function which should be called for all
 *                                   @eref{ThreadReleaseLock} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadReleaseLockCallback(
    OTF2_EvtReaderCallbacks*                 evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadReleaseLock threadReleaseLockCallback );


/** @brief Callback for the ThreadTaskCreate event record.
 *
 *  A ThreadTaskCreate record marks that a task in was/will be created and
 *  will be processed by the specified thread team.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param threadTeam       Thread team References a @eref{Comm} definition and
 *                          will be mapped to the global definition if a
 *                          mapping table of type @eref{OTF2_MAPPING_COMM} is
 *                          available.
 *  @param creatingThread   Creating thread of this task.
 *  @param generationNumber Thread-private generation number of task's creating
 *                          thread.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadTaskCreate )( OTF2_LocationRef    location,
                                               OTF2_TimeStamp      time,
                                               uint64_t            eventPosition,
                                               void*               userData,
                                               OTF2_AttributeList* attributeList,
                                               OTF2_CommRef        threadTeam,
                                               uint32_t            creatingThread,
                                               uint32_t            generationNumber );


/** @brief Registers the callback for the ThreadTaskCreate event.
 *
 *  @param evtReaderCallbacks       Struct for all callbacks.
 *  @param threadTaskCreateCallback Function which should be called for all
 *                                  @eref{ThreadTaskCreate} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadTaskCreateCallback(
    OTF2_EvtReaderCallbacks*                evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadTaskCreate threadTaskCreateCallback );


/** @brief Callback for the ThreadTaskSwitch event record.
 *
 *  A ThreadTaskSwitch record indicates that the execution of the current
 *  task will be suspended and another task starts/restarts its
 *  execution. Please note that this may change the current call stack
 *  of the executing location.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param threadTeam       Thread team References a @eref{Comm} definition and
 *                          will be mapped to the global definition if a
 *                          mapping table of type @eref{OTF2_MAPPING_COMM} is
 *                          available.
 *  @param creatingThread   Creating thread of this task.
 *  @param generationNumber Thread-private generation number of task's creating
 *                          thread.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadTaskSwitch )( OTF2_LocationRef    location,
                                               OTF2_TimeStamp      time,
                                               uint64_t            eventPosition,
                                               void*               userData,
                                               OTF2_AttributeList* attributeList,
                                               OTF2_CommRef        threadTeam,
                                               uint32_t            creatingThread,
                                               uint32_t            generationNumber );


/** @brief Registers the callback for the ThreadTaskSwitch event.
 *
 *  @param evtReaderCallbacks       Struct for all callbacks.
 *  @param threadTaskSwitchCallback Function which should be called for all
 *                                  @eref{ThreadTaskSwitch} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadTaskSwitchCallback(
    OTF2_EvtReaderCallbacks*                evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadTaskSwitch threadTaskSwitchCallback );


/** @brief Callback for the ThreadTaskComplete event record.
 *
 *  A ThreadTaskComplete record indicates that the execution of an OpenMP
 *  task has finished.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param threadTeam       Thread team References a @eref{Comm} definition and
 *                          will be mapped to the global definition if a
 *                          mapping table of type @eref{OTF2_MAPPING_COMM} is
 *                          available.
 *  @param creatingThread   Creating thread of this task.
 *  @param generationNumber Thread-private generation number of task's creating
 *                          thread.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadTaskComplete )( OTF2_LocationRef    location,
                                                 OTF2_TimeStamp      time,
                                                 uint64_t            eventPosition,
                                                 void*               userData,
                                                 OTF2_AttributeList* attributeList,
                                                 OTF2_CommRef        threadTeam,
                                                 uint32_t            creatingThread,
                                                 uint32_t            generationNumber );


/** @brief Registers the callback for the ThreadTaskComplete event.
 *
 *  @param evtReaderCallbacks         Struct for all callbacks.
 *  @param threadTaskCompleteCallback Function which should be called for all
 *                                    @eref{ThreadTaskComplete} definitions.
 *
 *  @since Version 1.2
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadTaskCompleteCallback(
    OTF2_EvtReaderCallbacks*                  evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadTaskComplete threadTaskCompleteCallback );


/** @brief Callback for the ThreadCreate event record.
 *
 *  The location created successfully a new thread.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param threadContingent The thread contingent. References a @eref{Comm}
 *                          definition and will be mapped to the global
 *                          definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param sequenceCount    A @p threadContingent unique number. The
 *                          corresponding @eref{ThreadBegin} event does have
 *                          the same number.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadCreate )( OTF2_LocationRef    location,
                                           OTF2_TimeStamp      time,
                                           uint64_t            eventPosition,
                                           void*               userData,
                                           OTF2_AttributeList* attributeList,
                                           OTF2_CommRef        threadContingent,
                                           uint64_t            sequenceCount );


/** @brief Registers the callback for the ThreadCreate event.
 *
 *  @param evtReaderCallbacks   Struct for all callbacks.
 *  @param threadCreateCallback Function which should be called for all
 *                              @eref{ThreadCreate} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadCreateCallback(
    OTF2_EvtReaderCallbacks*            evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadCreate threadCreateCallback );


/** @brief Callback for the ThreadBegin event record.
 *
 *  Marks the begin of a thread created by another thread.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param threadContingent The thread contingent. References a @eref{Comm}
 *                          definition and will be mapped to the global
 *                          definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param sequenceCount    A @p threadContingent unique number. The
 *                          corresponding @eref{ThreadCreate} event does have
 *                          the same number.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadBegin )( OTF2_LocationRef    location,
                                          OTF2_TimeStamp      time,
                                          uint64_t            eventPosition,
                                          void*               userData,
                                          OTF2_AttributeList* attributeList,
                                          OTF2_CommRef        threadContingent,
                                          uint64_t            sequenceCount );


/** @brief Registers the callback for the ThreadBegin event.
 *
 *  @param evtReaderCallbacks  Struct for all callbacks.
 *  @param threadBeginCallback Function which should be called for all
 *                             @eref{ThreadBegin} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadBeginCallback(
    OTF2_EvtReaderCallbacks*           evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadBegin threadBeginCallback );


/** @brief Callback for the ThreadWait event record.
 *
 *  The location waits for the completion of another thread.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param threadContingent The thread contingent. References a @eref{Comm}
 *                          definition and will be mapped to the global
 *                          definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param sequenceCount    A @p threadContingent unique number. The
 *                          corresponding @eref{ThreadEnd} event does have
 *                          the same number.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadWait )( OTF2_LocationRef    location,
                                         OTF2_TimeStamp      time,
                                         uint64_t            eventPosition,
                                         void*               userData,
                                         OTF2_AttributeList* attributeList,
                                         OTF2_CommRef        threadContingent,
                                         uint64_t            sequenceCount );


/** @brief Registers the callback for the ThreadWait event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param threadWaitCallback Function which should be called for all
 *                            @eref{ThreadWait} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadWaitCallback(
    OTF2_EvtReaderCallbacks*          evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadWait threadWaitCallback );


/** @brief Callback for the ThreadEnd event record.
 *
 *  Marks the end of a thread.
 *
 *  @param location         The location where this event happened.
 *  @param time             The time when this event happened.
 *  @param eventPosition    The event position of this event in the trace.
 *                          Starting with 1.
 *  @param userData         User data as set by
 *                          @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                          @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList    Additional attributes for this event.
 *  @param threadContingent The thread contingent. References a @eref{Comm}
 *                          definition and will be mapped to the global
 *                          definition if a mapping table of type
 *                          @eref{OTF2_MAPPING_COMM} is available.
 *  @param sequenceCount    A @p threadContingent unique number. The
 *                          corresponding @eref{ThreadWait} event does have
 *                          the same number. @eref{OTF2_UNDEFINED_UINT64} in
 *                          case no corresponding @eref{ThreadWait} event
 *                          exists.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ThreadEnd )( OTF2_LocationRef    location,
                                        OTF2_TimeStamp      time,
                                        uint64_t            eventPosition,
                                        void*               userData,
                                        OTF2_AttributeList* attributeList,
                                        OTF2_CommRef        threadContingent,
                                        uint64_t            sequenceCount );


/** @brief Registers the callback for the ThreadEnd event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param threadEndCallback  Function which should be called for all
 *                            @eref{ThreadEnd} definitions.
 *
 *  @since Version 1.3
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetThreadEndCallback(
    OTF2_EvtReaderCallbacks*         evtReaderCallbacks,
    OTF2_EvtReaderCallback_ThreadEnd threadEndCallback );


/** @brief Callback for the CallingContextEnter event record.
 *
 *  The thread entered an instrumented region, represented by the
 *  referenced @eref{CallingContext}. In contrast to the @eref{Enter}
 *  event, it gives the full calling context through the
 *  @eref{CallingContext} tree.
 *
 *  Events based on the @eref{CallingContext} definition are mutually
 *  exclusive with the @eref{Enter}/@eref{Leave} events in a trace.
 *
 *  If no callback for this event is set but a callback for @eref{Enter}
 *  events is defined, the reader will automatically generate an
 *  @eref{Enter} callback call for the @eref{Region} referenced by the
 *  @eref{CallingContext} attribute of this event.  Note that this
 *  emulation does @b not re-create the full calling context!  It only
 *  re-creates the event order for instrumented regions.
 *
 *  @param location       The location where this event happened.
 *  @param time           The time when this event happened.
 *  @param eventPosition  The event position of this event in the trace. Starting
 *                        with 1.
 *  @param userData       User data as set by
 *                        @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                        @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList  Additional attributes for this event.
 *  @param callingContext The entered region as referenced by the
 *                        @eref{CallingContext} definition. References a
 *                        @eref{CallingContext} definition and will be mapped
 *                        to the global definition if a mapping table of type
 *                        @eref{OTF2_MAPPING_CALLING_CONTEXT} is available.
 *  @param unwindDistance The unwindDistance for this @p callingContext. See the
 *                        description in @eref{CallingContext}.
 *
 *  @since Version 2.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_CallingContextEnter )( OTF2_LocationRef       location,
                                                  OTF2_TimeStamp         time,
                                                  uint64_t               eventPosition,
                                                  void*                  userData,
                                                  OTF2_AttributeList*    attributeList,
                                                  OTF2_CallingContextRef callingContext,
                                                  uint32_t               unwindDistance );


/** @brief Registers the callback for the CallingContextEnter event.
 *
 *  @param evtReaderCallbacks          Struct for all callbacks.
 *  @param callingContextEnterCallback Function which should be called for all
 *                                     @eref{CallingContextEnter}
 *                                     definitions.
 *
 *  @since Version 2.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetCallingContextEnterCallback(
    OTF2_EvtReaderCallbacks*                   evtReaderCallbacks,
    OTF2_EvtReaderCallback_CallingContextEnter callingContextEnterCallback );


/** @brief Callback for the CallingContextLeave event record.
 *
 *  The thread left an instrumented region, represented by the referenced
 *  @eref{CallingContext}. In contrast to the @eref{Leave} event, it
 *  gives the full calling context through the @eref{CallingContext}
 *  tree.
 *
 *  The unwind distance for this @eref{CallingContext} is defined to be 1.
 *  Because it must be assumed that the instrumented region made
 *  progress since the previous @eref{CallingContext} event.
 *
 *  Events based on the @eref{CallingContext} definition are mutually
 *  exclusive with the @eref{Enter}/@eref{Leave} events in a trace.
 *
 *  The parent of the @eref{CallingContext} must be used as the previous
 *  calling context for the next event.
 *
 *  If no callback for this event is set but a callback for @eref{Leave}
 *  events is defined, the reader will automatically generate an
 *  @eref{Leave} callback call for the @eref{Region} referenced by the
 *  @eref{CallingContext} attribute of this event.  Note that this
 *  emulation does @b not re-create the full calling context!  It only
 *  re-creates the event order for instrumented regions.
 *
 *  @param location       The location where this event happened.
 *  @param time           The time when this event happened.
 *  @param eventPosition  The event position of this event in the trace. Starting
 *                        with 1.
 *  @param userData       User data as set by
 *                        @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                        @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList  Additional attributes for this event.
 *  @param callingContext The left region as referenced by the
 *                        @eref{CallingContext} definition. References a
 *                        @eref{CallingContext} definition and will be mapped
 *                        to the global definition if a mapping table of type
 *                        @eref{OTF2_MAPPING_CALLING_CONTEXT} is available.
 *
 *  @since Version 2.0
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_CallingContextLeave )( OTF2_LocationRef       location,
                                                  OTF2_TimeStamp         time,
                                                  uint64_t               eventPosition,
                                                  void*                  userData,
                                                  OTF2_AttributeList*    attributeList,
                                                  OTF2_CallingContextRef callingContext );


/** @brief Registers the callback for the CallingContextLeave event.
 *
 *  @param evtReaderCallbacks          Struct for all callbacks.
 *  @param callingContextLeaveCallback Function which should be called for all
 *                                     @eref{CallingContextLeave}
 *                                     definitions.
 *
 *  @since Version 2.0
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetCallingContextLeaveCallback(
    OTF2_EvtReaderCallbacks*                   evtReaderCallbacks,
    OTF2_EvtReaderCallback_CallingContextLeave callingContextLeaveCallback );


/** @brief Callback for the CallingContextSample event record.
 *
 *  The thread was interrupted to take a sample of its current state
 *  (region and source code location).
 *
 *  Events based on the @eref{CallingContext} definition are mutually
 *  exclusive with the @eref{Enter}/@eref{Leave} events in a trace.
 *
 *  @param location           The location where this event happened.
 *  @param time               The time when this event happened.
 *  @param eventPosition      The event position of this event in the trace.
 *                            Starting with 1.
 *  @param userData           User data as set by
 *                            @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                            @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList      Additional attributes for this event.
 *  @param callingContext     Describes the calling context of the thread when it
 *                            was interrupted. References a
 *                            @eref{CallingContext} definition and will be
 *                            mapped to the global definition if a mapping
 *                            table of type
 *                            @eref{OTF2_MAPPING_CALLING_CONTEXT} is
 *                            available.
 *  @param unwindDistance     The unwindDistance for this @p callingContext. See
 *                            the description in @eref{CallingContext}.
 *  @param interruptGenerator  References a @eref{InterruptGenerator} definition
 *                            and will be mapped to the global definition if
 *                            a mapping table of type
 *                            @eref{OTF2_MAPPING_INTERRUPT_GENERATOR} is
 *                            available.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_CallingContextSample )( OTF2_LocationRef           location,
                                                   OTF2_TimeStamp             time,
                                                   uint64_t                   eventPosition,
                                                   void*                      userData,
                                                   OTF2_AttributeList*        attributeList,
                                                   OTF2_CallingContextRef     callingContext,
                                                   uint32_t                   unwindDistance,
                                                   OTF2_InterruptGeneratorRef interruptGenerator );


/** @brief Registers the callback for the CallingContextSample event.
 *
 *  @param evtReaderCallbacks           Struct for all callbacks.
 *  @param callingContextSampleCallback Function which should be called for all
 *                                      @eref{CallingContextSample}
 *                                      definitions.
 *
 *  @since Version 1.5
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetCallingContextSampleCallback(
    OTF2_EvtReaderCallbacks*                    evtReaderCallbacks,
    OTF2_EvtReaderCallback_CallingContextSample callingContextSampleCallback );


/** @brief Callback for the IoCreateHandle event record.
 *
 *  An IoCreateHandle record marks the creation of a new @emph{active} I/O
 *  handle that can be used by subsequent I/O operation events.
 *
 *  An @eref{IoHandle} is @emph{active} between a pair of consecutive
 *  @eref{IoCreateHandle} and @eref{IoDestroyHandle} events. All
 *  @eref{Location}s of a @eref{LocationGroup} have access to an
 *  @emph{active} @eref{IoHandle}.
 *
 *  If the @eref{Comm} attribute of the @eref{IoHandle} handle is not
 *  @eref{OTF2_UNDEFINED_COMM}, this is a collective operation over
 *  @eref{Comm}.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        A previously @emph{inactive} I/O handle which will be
 *                       activated by this record. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param mode          Determines which I/O operations can be applied to this
 *                       I/O handle (e.g., read-only, write-only, read-
 *                       write).
 *  @param creationFlags Requested I/O handle creation flags (e.g., create,
 *                       exclusive, etc.).
 *  @param statusFlags   I/O handle status flags which will be associated with
 *                       the @p handle attribute (e.g., append, create,
 *                       close-on-exec, async, etc).
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoCreateHandle )( OTF2_LocationRef    location,
                                             OTF2_TimeStamp      time,
                                             uint64_t            eventPosition,
                                             void*               userData,
                                             OTF2_AttributeList* attributeList,
                                             OTF2_IoHandleRef    handle,
                                             OTF2_IoAccessMode   mode,
                                             OTF2_IoCreationFlag creationFlags,
                                             OTF2_IoStatusFlag   statusFlags );


/** @brief Registers the callback for the IoCreateHandle event.
 *
 *  @param evtReaderCallbacks     Struct for all callbacks.
 *  @param ioCreateHandleCallback Function which should be called for all
 *                                @eref{IoCreateHandle} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoCreateHandleCallback(
    OTF2_EvtReaderCallbacks*              evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoCreateHandle ioCreateHandleCallback );


/** @brief Callback for the IoDestroyHandle event record.
 *
 *  An IoDestroyHandle record marks the end of an @emph{active} I/O
 *  handle's lifetime.
 *
 *  An @eref{IoHandle} is @emph{active} between a pair of consecutive
 *  @eref{IoCreateHandle} and @eref{IoDestroyHandle} events. All
 *  @eref{Location}s of a @eref{LocationGroup} have access to an
 *  @emph{active} @eref{IoHandle}.
 *
 *  If the @eref{Comm} attribute of the @eref{IoHandle} handle is not
 *  @eref{OTF2_UNDEFINED_COMM}, this is a collective operation over
 *  @eref{Comm}.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle which will be inactivated by
 *                       this records. References a @eref{IoHandle}
 *                       definition and will be mapped to the global
 *                       definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoDestroyHandle )( OTF2_LocationRef    location,
                                              OTF2_TimeStamp      time,
                                              uint64_t            eventPosition,
                                              void*               userData,
                                              OTF2_AttributeList* attributeList,
                                              OTF2_IoHandleRef    handle );


/** @brief Registers the callback for the IoDestroyHandle event.
 *
 *  @param evtReaderCallbacks      Struct for all callbacks.
 *  @param ioDestroyHandleCallback Function which should be called for all
 *                                 @eref{IoDestroyHandle} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoDestroyHandleCallback(
    OTF2_EvtReaderCallbacks*               evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoDestroyHandle ioDestroyHandleCallback );


/** @brief Callback for the IoDuplicateHandle event record.
 *
 *  An IoDuplicateHandle record marks the duplication of an already
 *  existing @emph{active} I/O handle.
 *
 *  The new I/O handle @p newHandle is @emph{active} after this event.
 *
 *  Both @eref{IoHandle}s must reference the same @eref{Comm} definition
 *  or be @eref{OTF2_UNDEFINED_COMM}.  If the @eref{Comm} attribute of
 *  the @eref{IoHandle} handles is not @eref{OTF2_UNDEFINED_COMM},
 *  this is a collective operation over @eref{Comm}.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param oldHandle     An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param newHandle     A previously @emph{inactive} I/O handle which will be
 *                       activated by this record. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param statusFlags   The status flag for the new I/O handle @p newHandle. No
 *                       status flags will be inherited from the I/O handle
 *                       @p oldHandle.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoDuplicateHandle )( OTF2_LocationRef    location,
                                                OTF2_TimeStamp      time,
                                                uint64_t            eventPosition,
                                                void*               userData,
                                                OTF2_AttributeList* attributeList,
                                                OTF2_IoHandleRef    oldHandle,
                                                OTF2_IoHandleRef    newHandle,
                                                OTF2_IoStatusFlag   statusFlags );


/** @brief Registers the callback for the IoDuplicateHandle event.
 *
 *  @param evtReaderCallbacks        Struct for all callbacks.
 *  @param ioDuplicateHandleCallback Function which should be called for all
 *                                   @eref{IoDuplicateHandle} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoDuplicateHandleCallback(
    OTF2_EvtReaderCallbacks*                 evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoDuplicateHandle ioDuplicateHandleCallback );


/** @brief Callback for the IoSeek event record.
 *
 *  An IoSeek record marks a change of the position, e.g., within a file.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param offsetRequest Requested offset.
 *  @param whence        Position inside the file from where @p offsetRequest
 *                       should be applied (e.g., absolute from the start or
 *                       end, relative to the current position).
 *  @param offsetResult  Resulting offset, e.g., within the file relative to the
 *                       beginning of the file.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoSeek )( OTF2_LocationRef    location,
                                     OTF2_TimeStamp      time,
                                     uint64_t            eventPosition,
                                     void*               userData,
                                     OTF2_AttributeList* attributeList,
                                     OTF2_IoHandleRef    handle,
                                     int64_t             offsetRequest,
                                     OTF2_IoSeekOption   whence,
                                     uint64_t            offsetResult );


/** @brief Registers the callback for the IoSeek event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param ioSeekCallback     Function which should be called for all
 *                            @eref{IoSeek} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoSeekCallback(
    OTF2_EvtReaderCallbacks*      evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoSeek ioSeekCallback );


/** @brief Callback for the IoChangeStatusFlags event record.
 *
 *  An IoChangeStatusFlags record marks a change to the status flags
 *  associated with an @emph{active} I/O handle.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param statusFlags   Set flags (e.g., close-on-exec, append, etc.).
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoChangeStatusFlags )( OTF2_LocationRef    location,
                                                  OTF2_TimeStamp      time,
                                                  uint64_t            eventPosition,
                                                  void*               userData,
                                                  OTF2_AttributeList* attributeList,
                                                  OTF2_IoHandleRef    handle,
                                                  OTF2_IoStatusFlag   statusFlags );


/** @brief Registers the callback for the IoChangeStatusFlags event.
 *
 *  @param evtReaderCallbacks          Struct for all callbacks.
 *  @param ioChangeStatusFlagsCallback Function which should be called for all
 *                                     @eref{IoChangeStatusFlags}
 *                                     definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoChangeStatusFlagsCallback(
    OTF2_EvtReaderCallbacks*                   evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoChangeStatusFlags ioChangeStatusFlagsCallback );


/** @brief Callback for the IoDeleteFile event record.
 *
 *  An IoDeleteFile record marks the deletion of an I/O file.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param ioParadigm    The I/O paradigm which induced the deletion. References
 *                       a @eref{IoParadigm} definition.
 *  @param file          File identifier. References a @eref{IoRegularFile}, or a
 *                       @eref{IoDirectory} definition and will be mapped to
 *                       the global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_FILE} is available.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoDeleteFile )( OTF2_LocationRef    location,
                                           OTF2_TimeStamp      time,
                                           uint64_t            eventPosition,
                                           void*               userData,
                                           OTF2_AttributeList* attributeList,
                                           OTF2_IoParadigmRef  ioParadigm,
                                           OTF2_IoFileRef      file );


/** @brief Registers the callback for the IoDeleteFile event.
 *
 *  @param evtReaderCallbacks   Struct for all callbacks.
 *  @param ioDeleteFileCallback Function which should be called for all
 *                              @eref{IoDeleteFile} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoDeleteFileCallback(
    OTF2_EvtReaderCallbacks*            evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoDeleteFile ioDeleteFileCallback );


/** @brief Callback for the IoOperationBegin event record.
 *
 *  An IoOperationBegin record marks the begin of a file operation (read,
 *  write, etc.).
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param location       The location where this event happened.
 *  @param time           The time when this event happened.
 *  @param eventPosition  The event position of this event in the trace. Starting
 *                        with 1.
 *  @param userData       User data as set by
 *                        @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                        @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList  Additional attributes for this event.
 *  @param handle         An @emph{active} I/O handle. References a
 *                        @eref{IoHandle} definition and will be mapped to
 *                        the global definition if a mapping table of type
 *                        @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param mode           Mode of an I/O handle operation (e.g., read or write).
 *  @param operationFlags Special semantic of this operation.
 *  @param bytesRequest   Requested bytes to write/read.
 *  @param matchingId     Identifier used to correlate associated event records
 *                        of an I/O operation. This identifier is unique for
 *                        the referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoOperationBegin )( OTF2_LocationRef     location,
                                               OTF2_TimeStamp       time,
                                               uint64_t             eventPosition,
                                               void*                userData,
                                               OTF2_AttributeList*  attributeList,
                                               OTF2_IoHandleRef     handle,
                                               OTF2_IoOperationMode mode,
                                               OTF2_IoOperationFlag operationFlags,
                                               uint64_t             bytesRequest,
                                               uint64_t             matchingId );


/** @brief Registers the callback for the IoOperationBegin event.
 *
 *  @param evtReaderCallbacks       Struct for all callbacks.
 *  @param ioOperationBeginCallback Function which should be called for all
 *                                  @eref{IoOperationBegin} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoOperationBeginCallback(
    OTF2_EvtReaderCallbacks*                evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoOperationBegin ioOperationBeginCallback );


/** @brief Callback for the IoOperationTest event record.
 *
 *  An IoOperationTest record marks an unsuccessful test whether an I/O
 *  operation has already finished.
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param matchingId    Identifier used to correlate associated event records of
 *                       an I/O operation. This identifier is unique for the
 *                       referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoOperationTest )( OTF2_LocationRef    location,
                                              OTF2_TimeStamp      time,
                                              uint64_t            eventPosition,
                                              void*               userData,
                                              OTF2_AttributeList* attributeList,
                                              OTF2_IoHandleRef    handle,
                                              uint64_t            matchingId );


/** @brief Registers the callback for the IoOperationTest event.
 *
 *  @param evtReaderCallbacks      Struct for all callbacks.
 *  @param ioOperationTestCallback Function which should be called for all
 *                                 @eref{IoOperationTest} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoOperationTestCallback(
    OTF2_EvtReaderCallbacks*               evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoOperationTest ioOperationTestCallback );


/** @brief Callback for the IoOperationIssued event record.
 *
 *  An IoOperationIssued record marks the successful initiation of a non-
 *  blocking operation (read, write etc.) on an @emph{active} I/O
 *  handle.
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param matchingId    Identifier used to correlate associated event records of
 *                       an I/O operation. This identifier is unique for the
 *                       referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoOperationIssued )( OTF2_LocationRef    location,
                                                OTF2_TimeStamp      time,
                                                uint64_t            eventPosition,
                                                void*               userData,
                                                OTF2_AttributeList* attributeList,
                                                OTF2_IoHandleRef    handle,
                                                uint64_t            matchingId );


/** @brief Registers the callback for the IoOperationIssued event.
 *
 *  @param evtReaderCallbacks        Struct for all callbacks.
 *  @param ioOperationIssuedCallback Function which should be called for all
 *                                   @eref{IoOperationIssued} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoOperationIssuedCallback(
    OTF2_EvtReaderCallbacks*                 evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoOperationIssued ioOperationIssuedCallback );


/** @brief Callback for the IoOperationComplete event record.
 *
 *  An IoOperationComplete record marks the end of a file operation (read,
 *  write etc.) on an @emph{active} I/O handle.
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param bytesResult   Number of actual transferred bytes.
 *  @param matchingId    Identifier used to correlate associated event records of
 *                       an I/O operation. This identifier is unique for the
 *                       referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoOperationComplete )( OTF2_LocationRef    location,
                                                  OTF2_TimeStamp      time,
                                                  uint64_t            eventPosition,
                                                  void*               userData,
                                                  OTF2_AttributeList* attributeList,
                                                  OTF2_IoHandleRef    handle,
                                                  uint64_t            bytesResult,
                                                  uint64_t            matchingId );


/** @brief Registers the callback for the IoOperationComplete event.
 *
 *  @param evtReaderCallbacks          Struct for all callbacks.
 *  @param ioOperationCompleteCallback Function which should be called for all
 *                                     @eref{IoOperationComplete}
 *                                     definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoOperationCompleteCallback(
    OTF2_EvtReaderCallbacks*                   evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoOperationComplete ioOperationCompleteCallback );


/** @brief Callback for the IoOperationCancelled event record.
 *
 *  An IoOperationCancelled record marks the successful cancellation of a
 *  non-blocking operation (read, write etc.) on an @emph{active} I/O
 *  handle.
 *
 *  See @ref otf2_io_operation_event_order for the possible event orders.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param matchingId    Identifier used to correlate associated event records of
 *                       an I/O operation. This identifier is unique for the
 *                       referenced @eref{IoHandle}.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoOperationCancelled )( OTF2_LocationRef    location,
                                                   OTF2_TimeStamp      time,
                                                   uint64_t            eventPosition,
                                                   void*               userData,
                                                   OTF2_AttributeList* attributeList,
                                                   OTF2_IoHandleRef    handle,
                                                   uint64_t            matchingId );


/** @brief Registers the callback for the IoOperationCancelled event.
 *
 *  @param evtReaderCallbacks           Struct for all callbacks.
 *  @param ioOperationCancelledCallback Function which should be called for all
 *                                      @eref{IoOperationCancelled}
 *                                      definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoOperationCancelledCallback(
    OTF2_EvtReaderCallbacks*                    evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoOperationCancelled ioOperationCancelledCallback );


/** @brief Callback for the IoAcquireLock event record.
 *
 *  An IoAcquireLock record marks the acquisition of an I/O lock.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param lockType      Type of the lock.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoAcquireLock )( OTF2_LocationRef    location,
                                            OTF2_TimeStamp      time,
                                            uint64_t            eventPosition,
                                            void*               userData,
                                            OTF2_AttributeList* attributeList,
                                            OTF2_IoHandleRef    handle,
                                            OTF2_LockType       lockType );


/** @brief Registers the callback for the IoAcquireLock event.
 *
 *  @param evtReaderCallbacks    Struct for all callbacks.
 *  @param ioAcquireLockCallback Function which should be called for all
 *                               @eref{IoAcquireLock} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoAcquireLockCallback(
    OTF2_EvtReaderCallbacks*             evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoAcquireLock ioAcquireLockCallback );


/** @brief Callback for the IoReleaseLock event record.
 *
 *  An IoReleaseLock record marks the release of an I/O lock.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param lockType      Type of the lock.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoReleaseLock )( OTF2_LocationRef    location,
                                            OTF2_TimeStamp      time,
                                            uint64_t            eventPosition,
                                            void*               userData,
                                            OTF2_AttributeList* attributeList,
                                            OTF2_IoHandleRef    handle,
                                            OTF2_LockType       lockType );


/** @brief Registers the callback for the IoReleaseLock event.
 *
 *  @param evtReaderCallbacks    Struct for all callbacks.
 *  @param ioReleaseLockCallback Function which should be called for all
 *                               @eref{IoReleaseLock} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoReleaseLockCallback(
    OTF2_EvtReaderCallbacks*             evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoReleaseLock ioReleaseLockCallback );


/** @brief Callback for the IoTryLock event record.
 *
 *  An IoTryLock record marks when an I/O lock was requested but not
 *  granted.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param handle        An @emph{active} I/O handle. References a
 *                       @eref{IoHandle} definition and will be mapped to the
 *                       global definition if a mapping table of type
 *                       @eref{OTF2_MAPPING_IO_HANDLE} is available.
 *  @param lockType      Type of the lock.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_IoTryLock )( OTF2_LocationRef    location,
                                        OTF2_TimeStamp      time,
                                        uint64_t            eventPosition,
                                        void*               userData,
                                        OTF2_AttributeList* attributeList,
                                        OTF2_IoHandleRef    handle,
                                        OTF2_LockType       lockType );


/** @brief Registers the callback for the IoTryLock event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param ioTryLockCallback  Function which should be called for all
 *                            @eref{IoTryLock} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetIoTryLockCallback(
    OTF2_EvtReaderCallbacks*         evtReaderCallbacks,
    OTF2_EvtReaderCallback_IoTryLock ioTryLockCallback );


/** @brief Callback for the ProgramBegin event record.
 *
 *  The ProgramBegin record marks the begin of the program.
 *
 *  This event is restricted to happen at most once on any @eref{Location}
 *  in a @eref{LocationGroup} that is of type
 *  @eref{OTF2_LOCATION_GROUP_TYPE_PROCESS}.
 *
 *  If there is a ProgramBegin record, a corresponding @eref{ProgramEnd}
 *  record on any @eref{Location} in the same @eref{LocationGroup} is
 *  mandatory and vice versa.
 *
 *  None of the timestamps recorded within the same @eref{LocationGroup}
 *  must be smaller than ProgramBegin's timestamp.
 *
 *  @param location          The location where this event happened.
 *  @param time              The time when this event happened.
 *  @param eventPosition     The event position of this event in the trace.
 *                           Starting with 1.
 *  @param userData          User data as set by
 *                           @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                           @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList     Additional attributes for this event.
 *  @param programName       The name of the program. References a @eref{String}
 *                           definition and will be mapped to the global
 *                           definition if a mapping table of type
 *                           @eref{OTF2_MAPPING_STRING} is available.
 *  @param numberOfArguments Number of additional arguments to the program.
 *  @param programArguments  List of additional arguments to the program.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ProgramBegin )( OTF2_LocationRef      location,
                                           OTF2_TimeStamp        time,
                                           uint64_t              eventPosition,
                                           void*                 userData,
                                           OTF2_AttributeList*   attributeList,
                                           OTF2_StringRef        programName,
                                           uint32_t              numberOfArguments,
                                           const OTF2_StringRef* programArguments );


/** @brief Registers the callback for the ProgramBegin event.
 *
 *  @param evtReaderCallbacks   Struct for all callbacks.
 *  @param programBeginCallback Function which should be called for all
 *                              @eref{ProgramBegin} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetProgramBeginCallback(
    OTF2_EvtReaderCallbacks*            evtReaderCallbacks,
    OTF2_EvtReaderCallback_ProgramBegin programBeginCallback );


/** @brief Callback for the ProgramEnd event record.
 *
 *  The ProgramEnd record marks the end of the program.
 *
 *  This event is restricted to happen at most once on any @eref{Location}
 *  in a @eref{LocationGroup} that is of type
 *  @eref{OTF2_LOCATION_GROUP_TYPE_PROCESS}.
 *
 *  If there is a ProgramEnd record, a corresponding @eref{ProgramBegin}
 *  record on any @eref{Location} in the same @eref{LocationGroup} is
 *  mandatory, and vice versa.
 *
 *  None of the timestamps recorded within the same @eref{LocationGroup}
 *  must be larger than ProgramEnd's timestamp.
 *
 *  @param location      The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param eventPosition The event position of this event in the trace. Starting
 *                       with 1.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterEvtCallbacks} or
 *                       @eref{OTF2_EvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *  @param exitStatus    The exit status of the program.  Note, that on some
 *                       systems only the least significant 8 bits may be
 *                       visible to other processes.  Use
 *                       @eref{OTF2_UNDEFINED_INT64}, if the exit status was
 *                       not available.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( * OTF2_EvtReaderCallback_ProgramEnd )( OTF2_LocationRef    location,
                                         OTF2_TimeStamp      time,
                                         uint64_t            eventPosition,
                                         void*               userData,
                                         OTF2_AttributeList* attributeList,
                                         int64_t             exitStatus );


/** @brief Registers the callback for the ProgramEnd event.
 *
 *  @param evtReaderCallbacks Struct for all callbacks.
 *  @param programEndCallback Function which should be called for all
 *                            @eref{ProgramEnd} definitions.
 *
 *  @since Version 2.1
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_EvtReaderCallbacks_SetProgramEndCallback(
    OTF2_EvtReaderCallbacks*          evtReaderCallbacks,
    OTF2_EvtReaderCallback_ProgramEnd programEndCallback );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_EVT_READER_CALLBACKS_H */
