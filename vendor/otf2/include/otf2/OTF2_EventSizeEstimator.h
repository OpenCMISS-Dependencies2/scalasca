/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_EVENT_SIZE_ESTIMATOR_H
#define OTF2_EVENT_SIZE_ESTIMATOR_H


/**
 *  @file
 *  @source     templates/OTF2_EventSizeEstimator.tmpl.h
 *
 *  @brief      Provides a interface to estimate the size of a resulting trace
 *              file.
 */


#include <stdint.h>
#include <stdlib.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_AttributeList.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Keeps all necessary information about the event size estimator.
 *  See OTF2_EventSizeEstimator_struct for detailed information. */
typedef struct OTF2_EventSizeEstimator OTF2_EventSizeEstimator;


/** @brief Creates a new OTF2_EventSizeEstimator object.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_EventSizeEstimator*
OTF2_EventSizeEstimator_New( void );


/** @brief Deletes an OTF2_EventSizeEstimator object.
 *
 *  @param estimator   Estimator object.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_Delete( OTF2_EventSizeEstimator* estimator );


/** @brief Sets the number of String definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfStringDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfStringDefinitions The number of definitions.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfStringDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfStringDefinitions );


/** @brief Sets the number of Attribute definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfAttributeDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfAttributeDefinitions The number of definitions.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfAttributeDefinitions( OTF2_EventSizeEstimator* estimator,
                                                         uint32_t                 numberOfAttributeDefinitions );


/** @brief Sets the number of Location definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfLocationDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfLocationDefinitions The number of definitions.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfLocationDefinitions( OTF2_EventSizeEstimator* estimator,
                                                        uint64_t                 numberOfLocationDefinitions );


/** @brief Sets the number of Region definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfRegionDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfRegionDefinitions The number of definitions.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfRegionDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfRegionDefinitions );


/** @brief Sets the number of Group definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfGroupDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfGroupDefinitions The number of definitions.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfGroupDefinitions( OTF2_EventSizeEstimator* estimator,
                                                     uint32_t                 numberOfGroupDefinitions );


/** @brief Sets the number of Metric definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfMetricDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfMetricDefinitions The number of definitions.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfMetricDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfMetricDefinitions );


/** @brief Sets the number of Comm definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfCommDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfCommDefinitions The number of definitions.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfCommDefinitions( OTF2_EventSizeEstimator* estimator,
                                                    uint32_t                 numberOfCommDefinitions );


/** @brief Sets the number of Parameter definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfParameterDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfParameterDefinitions The number of definitions.
 *
 *  @since Version 1.0
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfParameterDefinitions( OTF2_EventSizeEstimator* estimator,
                                                         uint32_t                 numberOfParameterDefinitions );


/** @brief Sets the number of RmaWin definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfRmaWinDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfRmaWinDefinitions The number of definitions.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfRmaWinDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfRmaWinDefinitions );


/** @brief Sets the number of SourceCodeLocation definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfSourceCodeLocationDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfSourceCodeLocationDefinitions The number of definitions.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfSourceCodeLocationDefinitions( OTF2_EventSizeEstimator* estimator,
                                                                  uint32_t                 numberOfSourceCodeLocationDefinitions );


/** @brief Sets the number of CallingContext definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfCallingContextDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfCallingContextDefinitions The number of definitions.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfCallingContextDefinitions( OTF2_EventSizeEstimator* estimator,
                                                              uint32_t                 numberOfCallingContextDefinitions );


/** @brief Sets the number of InterruptGenerator definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfInterruptGeneratorDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfInterruptGeneratorDefinitions The number of definitions.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfInterruptGeneratorDefinitions( OTF2_EventSizeEstimator* estimator,
                                                                  uint32_t                 numberOfInterruptGeneratorDefinitions );


/** @brief Sets the number of IoFile definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfIoFileDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfIoFileDefinitions The number of definitions.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfIoFileDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfIoFileDefinitions );


/** @brief Sets the number of IoHandle definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOfIoHandleDefinitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOfIoHandleDefinitions The number of definitions.
 *
 *  @since Version 2.1
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfIoHandleDefinitions( OTF2_EventSizeEstimator* estimator,
                                                        uint32_t                 numberOfIoHandleDefinitions );


/** @brief Returns an estimate for the size of the definition chunk size.
 *
 *  As definition records like the @eref{Group} definition may break the
 *  chunk size, this gives an upper bound for a chunksize, so that all
 *  individual definition records fit into one chunk. Note, this estimate
 *  does not include @eref{String} definitions, which may have an arbitrary
 *  length.
 *
 *  @param estimator   Estimator object.
 *
 *  @return The estimated definition chunk size, rounded-up to a multiple of
 *          @eref{OTF2_CHUNK_SIZE_MIN}. Or 0 if the estimate is larger than
 *          @eref{OTF2_CHUNK_SIZE_MAX}.
 */
size_t
OTF2_EventSizeEstimator_GetDefChunkSize( OTF2_EventSizeEstimator* estimator );


/** @brief Returns the size for a timestamp record.
 *
 *  OTF2 does only store a timestamp, if it changed between two events.
 *
 *  @param estimator   Estimator object.
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfTimestamp( OTF2_EventSizeEstimator* estimator );


/** @brief Returns the size estimate for an attribute list.
 *
 *  The attribute list should be filled with the used types. The attribute
 *  references are taken from the number of attribute definitions and the values
 *  are the upper bounds for integral and floating point types, and the
 *  estimates for definition references.
 *
 *  @param estimator      Estimator object.
 *  @param attributeList  Attribute List.
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfAttributeList( const OTF2_EventSizeEstimator* estimator,
                                                const OTF2_AttributeList*      attributeList );


/** @brief Calculates the size estimate for the BufferFlush event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfBufferFlushEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MeasurementOnOff event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMeasurementOnOffEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the Enter event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfEnterEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the Leave event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfLeaveEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiSend event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiSendEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiIsend event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiIsendEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiIsendComplete event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiIsendCompleteEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiIrecvRequest event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiIrecvRequestEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiRecv event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiRecvEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiIrecv event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiIrecvEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiRequestTest event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiRequestTestEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiRequestCancelled event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiRequestCancelledEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiCollectiveBegin event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiCollectiveBeginEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the MpiCollectiveEnd event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMpiCollectiveEndEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the OmpFork event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfOmpForkEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the OmpJoin event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfOmpJoinEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the OmpAcquireLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfOmpAcquireLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the OmpReleaseLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfOmpReleaseLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the OmpTaskCreate event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfOmpTaskCreateEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the OmpTaskSwitch event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfOmpTaskSwitchEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the OmpTaskComplete event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @deprecated In version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfOmpTaskCompleteEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the Metric event.
 *
 *  @param estimator       Estimator object.
 *  @param numberOfMetrics Number of metrics with in the set.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfMetricEvent( OTF2_EventSizeEstimator* estimator,
                                              uint8_t                  numberOfMetrics );


/** @brief Calculates the size estimate for the ParameterString event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfParameterStringEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ParameterInt event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfParameterIntEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ParameterUnsignedInt event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfParameterUnsignedIntEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaWinCreate event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaWinCreateEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaWinDestroy event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaWinDestroyEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaCollectiveBegin event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaCollectiveBeginEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaCollectiveEnd event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaCollectiveEndEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaGroupSync event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaGroupSyncEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaRequestLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaRequestLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaAcquireLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaAcquireLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaTryLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaTryLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaReleaseLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaReleaseLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaSync event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaSyncEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaWaitChange event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaWaitChangeEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaPut event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaPutEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaGet event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaGetEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaAtomic event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaAtomicEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaOpCompleteBlocking event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteBlockingEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaOpCompleteNonBlocking event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteNonBlockingEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaOpTest event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaOpTestEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the RmaOpCompleteRemote event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteRemoteEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadFork event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadForkEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadJoin event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadJoinEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadTeamBegin event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTeamBeginEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadTeamEnd event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTeamEndEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadAcquireLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadAcquireLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadReleaseLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadReleaseLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadTaskCreate event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTaskCreateEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadTaskSwitch event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTaskSwitchEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadTaskComplete event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.2
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTaskCompleteEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadCreate event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.3
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadCreateEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadBegin event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.3
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadBeginEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadWait event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.3
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadWaitEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ThreadEnd event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.3
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfThreadEndEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the CallingContextEnter event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfCallingContextEnterEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the CallingContextLeave event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.0
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfCallingContextLeaveEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the CallingContextSample event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 1.5
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfCallingContextSampleEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoCreateHandle event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoCreateHandleEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoDestroyHandle event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoDestroyHandleEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoDuplicateHandle event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoDuplicateHandleEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoSeek event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoSeekEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoChangeStatusFlags event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoChangeStatusFlagsEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoDeleteFile event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoDeleteFileEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoOperationBegin event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationBeginEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoOperationTest event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationTestEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoOperationIssued event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationIssuedEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoOperationComplete event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationCompleteEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoOperationCancelled event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationCancelledEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoAcquireLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoAcquireLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoReleaseLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoReleaseLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the IoTryLock event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfIoTryLockEvent( OTF2_EventSizeEstimator* estimator );


/** @brief Calculates the size estimate for the ProgramBegin event.
 *
 *  @param estimator         Estimator object.
 *  @param numberOfArguments Number of additional arguments to the program.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfProgramBeginEvent( OTF2_EventSizeEstimator* estimator,
                                                    uint32_t                 numberOfArguments );


/** @brief Calculates the size estimate for the ProgramEnd event.
 *
 *  @param estimator Estimator object.
 *
 *  @since Version 2.1
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfProgramEndEvent( OTF2_EventSizeEstimator* estimator );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_EVENT_SIZE_ESTIMATOR_H */
