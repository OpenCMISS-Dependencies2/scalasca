/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_INTERNAL_EVENTS_H
#define OTF2_INTERNAL_EVENTS_H


/**
 *  @internal
 *
 *  @file
 *  @source     templates/otf2_events_int.tmpl.h
 *
 *  @brief      This file list all event records supported in OTF2.
 */


typedef uint8_t OTF2_EventType;


enum OTF2_EventType_enum
{
    /* First five entries are reserved for OTF2 internals */

    OTF2_EVENT_BUFFER_FLUSH                 = 10, /** Event record identifier for the BufferFlush event. */
    OTF2_EVENT_MEASUREMENT_ON_OFF           = 11, /** Event record identifier for the MeasurementOnOff event. */
    OTF2_EVENT_ENTER                        = 12, /** Event record identifier for the Enter event. */
    OTF2_EVENT_LEAVE                        = 13, /** Event record identifier for the Leave event. */
    OTF2_EVENT_MPI_SEND                     = 14, /** Event record identifier for the MpiSend event. */
    OTF2_EVENT_MPI_ISEND                    = 15, /** Event record identifier for the MpiIsend event. */
    OTF2_EVENT_MPI_ISEND_COMPLETE           = 16, /** Event record identifier for the MpiIsendComplete event. */
    OTF2_EVENT_MPI_IRECV_REQUEST            = 17, /** Event record identifier for the MpiIrecvRequest event. */
    OTF2_EVENT_MPI_RECV                     = 18, /** Event record identifier for the MpiRecv event. */
    OTF2_EVENT_MPI_IRECV                    = 19, /** Event record identifier for the MpiIrecv event. */
    OTF2_EVENT_MPI_REQUEST_TEST             = 20, /** Event record identifier for the MpiRequestTest event. */
    OTF2_EVENT_MPI_REQUEST_CANCELLED        = 21, /** Event record identifier for the MpiRequestCancelled event. */
    OTF2_EVENT_MPI_COLLECTIVE_BEGIN         = 22, /** Event record identifier for the MpiCollectiveBegin event. */
    OTF2_EVENT_MPI_COLLECTIVE_END           = 23, /** Event record identifier for the MpiCollectiveEnd event. */
    OTF2_EVENT_OMP_FORK                     = 24, /** Event record identifier for the OmpFork event. */
    OTF2_EVENT_OMP_JOIN                     = 25, /** Event record identifier for the OmpJoin event. */
    OTF2_EVENT_OMP_ACQUIRE_LOCK             = 26, /** Event record identifier for the OmpAcquireLock event. */
    OTF2_EVENT_OMP_RELEASE_LOCK             = 27, /** Event record identifier for the OmpReleaseLock event. */
    OTF2_EVENT_OMP_TASK_CREATE              = 28, /** Event record identifier for the OmpTaskCreate event. */
    OTF2_EVENT_OMP_TASK_SWITCH              = 29, /** Event record identifier for the OmpTaskSwitch event. */
    OTF2_EVENT_OMP_TASK_COMPLETE            = 30, /** Event record identifier for the OmpTaskComplete event. */
    OTF2_EVENT_METRIC                       = 31, /** Event record identifier for the Metric event. */
    OTF2_EVENT_PARAMETER_STRING             = 32, /** Event record identifier for the ParameterString event. */
    OTF2_EVENT_PARAMETER_INT                = 33, /** Event record identifier for the ParameterInt event. */
    OTF2_EVENT_PARAMETER_UNSIGNED_INT       = 34, /** Event record identifier for the ParameterUnsignedInt event. */
    OTF2_EVENT_RMA_WIN_CREATE               = 35, /** Event record identifier for the RmaWinCreate event. */
    OTF2_EVENT_RMA_WIN_DESTROY              = 36, /** Event record identifier for the RmaWinDestroy event. */
    OTF2_EVENT_RMA_COLLECTIVE_BEGIN         = 37, /** Event record identifier for the RmaCollectiveBegin event. */
    OTF2_EVENT_RMA_COLLECTIVE_END           = 38, /** Event record identifier for the RmaCollectiveEnd event. */
    OTF2_EVENT_RMA_GROUP_SYNC               = 39, /** Event record identifier for the RmaGroupSync event. */
    OTF2_EVENT_RMA_REQUEST_LOCK             = 40, /** Event record identifier for the RmaRequestLock event. */
    OTF2_EVENT_RMA_ACQUIRE_LOCK             = 41, /** Event record identifier for the RmaAcquireLock event. */
    OTF2_EVENT_RMA_TRY_LOCK                 = 42, /** Event record identifier for the RmaTryLock event. */
    OTF2_EVENT_RMA_RELEASE_LOCK             = 43, /** Event record identifier for the RmaReleaseLock event. */
    OTF2_EVENT_RMA_SYNC                     = 44, /** Event record identifier for the RmaSync event. */
    OTF2_EVENT_RMA_WAIT_CHANGE              = 45, /** Event record identifier for the RmaWaitChange event. */
    OTF2_EVENT_RMA_PUT                      = 46, /** Event record identifier for the RmaPut event. */
    OTF2_EVENT_RMA_GET                      = 47, /** Event record identifier for the RmaGet event. */
    OTF2_EVENT_RMA_ATOMIC                   = 48, /** Event record identifier for the RmaAtomic event. */
    OTF2_EVENT_RMA_OP_COMPLETE_BLOCKING     = 49, /** Event record identifier for the RmaOpCompleteBlocking event. */
    OTF2_EVENT_RMA_OP_COMPLETE_NON_BLOCKING = 50, /** Event record identifier for the RmaOpCompleteNonBlocking event. */
    OTF2_EVENT_RMA_OP_TEST                  = 51, /** Event record identifier for the RmaOpTest event. */
    OTF2_EVENT_RMA_OP_COMPLETE_REMOTE       = 52, /** Event record identifier for the RmaOpCompleteRemote event. */
    OTF2_EVENT_THREAD_FORK                  = 53, /** Event record identifier for the ThreadFork event. */
    OTF2_EVENT_THREAD_JOIN                  = 54, /** Event record identifier for the ThreadJoin event. */
    OTF2_EVENT_THREAD_TEAM_BEGIN            = 55, /** Event record identifier for the ThreadTeamBegin event. */
    OTF2_EVENT_THREAD_TEAM_END              = 56, /** Event record identifier for the ThreadTeamEnd event. */
    OTF2_EVENT_THREAD_ACQUIRE_LOCK          = 57, /** Event record identifier for the ThreadAcquireLock event. */
    OTF2_EVENT_THREAD_RELEASE_LOCK          = 58, /** Event record identifier for the ThreadReleaseLock event. */
    OTF2_EVENT_THREAD_TASK_CREATE           = 59, /** Event record identifier for the ThreadTaskCreate event. */
    OTF2_EVENT_THREAD_TASK_SWITCH           = 60, /** Event record identifier for the ThreadTaskSwitch event. */
    OTF2_EVENT_THREAD_TASK_COMPLETE         = 61, /** Event record identifier for the ThreadTaskComplete event. */
    OTF2_EVENT_THREAD_CREATE                = 62, /** Event record identifier for the ThreadCreate event. */
    OTF2_EVENT_THREAD_BEGIN                 = 63, /** Event record identifier for the ThreadBegin event. */
    OTF2_EVENT_THREAD_WAIT                  = 64, /** Event record identifier for the ThreadWait event. */
    OTF2_EVENT_THREAD_END                   = 65, /** Event record identifier for the ThreadEnd event. */
    OTF2_EVENT_CALLING_CONTEXT_ENTER        = 66, /** Event record identifier for the CallingContextEnter event. */
    OTF2_EVENT_CALLING_CONTEXT_LEAVE        = 67, /** Event record identifier for the CallingContextLeave event. */
    OTF2_EVENT_CALLING_CONTEXT_SAMPLE       = 68, /** Event record identifier for the CallingContextSample event. */
    OTF2_EVENT_IO_CREATE_HANDLE             = 69, /** Event record identifier for the IoCreateHandle event. */
    OTF2_EVENT_IO_DESTROY_HANDLE            = 70, /** Event record identifier for the IoDestroyHandle event. */
    OTF2_EVENT_IO_DUPLICATE_HANDLE          = 71, /** Event record identifier for the IoDuplicateHandle event. */
    OTF2_EVENT_IO_SEEK                      = 72, /** Event record identifier for the IoSeek event. */
    OTF2_EVENT_IO_CHANGE_STATUS_FLAGS       = 73, /** Event record identifier for the IoChangeStatusFlags event. */
    OTF2_EVENT_IO_DELETE_FILE               = 74, /** Event record identifier for the IoDeleteFile event. */
    OTF2_EVENT_IO_OPERATION_BEGIN           = 75, /** Event record identifier for the IoOperationBegin event. */
    OTF2_EVENT_IO_OPERATION_TEST            = 76, /** Event record identifier for the IoOperationTest event. */
    OTF2_EVENT_IO_OPERATION_ISSUED          = 77, /** Event record identifier for the IoOperationIssued event. */
    OTF2_EVENT_IO_OPERATION_COMPLETE        = 78, /** Event record identifier for the IoOperationComplete event. */
    OTF2_EVENT_IO_OPERATION_CANCELLED       = 79, /** Event record identifier for the IoOperationCancelled event. */
    OTF2_EVENT_IO_ACQUIRE_LOCK              = 80, /** Event record identifier for the IoAcquireLock event. */
    OTF2_EVENT_IO_RELEASE_LOCK              = 81, /** Event record identifier for the IoReleaseLock event. */
    OTF2_EVENT_IO_TRY_LOCK                  = 82, /** Event record identifier for the IoTryLock event. */
    OTF2_EVENT_PROGRAM_BEGIN                = 83, /** Event record identifier for the ProgramBegin event. */
    OTF2_EVENT_PROGRAM_END                  = 84, /** Event record identifier for the ProgramEnd event. */

    OTF2_EVENT_MAX_ID
};


/** @brief BufferFlush event.
 */
typedef struct OTF2_BufferFlush_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_TimeStamp stop_time;
} OTF2_BufferFlush;


/** @brief MeasurementOnOff event.
 */
typedef struct OTF2_MeasurementOnOff_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp       time;
    OTF2_MeasurementMode measurement_mode;
} OTF2_MeasurementOnOff;


/** @brief Enter event.
 */
typedef struct OTF2_Enter_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RegionRef region;
} OTF2_Enter;


/** @brief Leave event.
 */
typedef struct OTF2_Leave_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RegionRef region;
} OTF2_Leave;


/** @brief MpiSend event.
 */
typedef struct OTF2_MpiSend_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint32_t       receiver;
    OTF2_CommRef   communicator;
    uint32_t       msg_tag;
    uint64_t       msg_length;
} OTF2_MpiSend;


/** @brief MpiIsend event.
 */
typedef struct OTF2_MpiIsend_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint32_t       receiver;
    OTF2_CommRef   communicator;
    uint32_t       msg_tag;
    uint64_t       msg_length;
    uint64_t       request_id;
} OTF2_MpiIsend;


/** @brief MpiIsendComplete event.
 */
typedef struct OTF2_MpiIsendComplete_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint64_t       request_id;
} OTF2_MpiIsendComplete;


/** @brief MpiIrecvRequest event.
 */
typedef struct OTF2_MpiIrecvRequest_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint64_t       request_id;
} OTF2_MpiIrecvRequest;


/** @brief MpiRecv event.
 */
typedef struct OTF2_MpiRecv_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint32_t       sender;
    OTF2_CommRef   communicator;
    uint32_t       msg_tag;
    uint64_t       msg_length;
} OTF2_MpiRecv;


/** @brief MpiIrecv event.
 */
typedef struct OTF2_MpiIrecv_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint32_t       sender;
    OTF2_CommRef   communicator;
    uint32_t       msg_tag;
    uint64_t       msg_length;
    uint64_t       request_id;
} OTF2_MpiIrecv;


/** @brief MpiRequestTest event.
 */
typedef struct OTF2_MpiRequestTest_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint64_t       request_id;
} OTF2_MpiRequestTest;


/** @brief MpiRequestCancelled event.
 */
typedef struct OTF2_MpiRequestCancelled_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint64_t       request_id;
} OTF2_MpiRequestCancelled;


/** @brief MpiCollectiveBegin event.
 */
typedef struct OTF2_MpiCollectiveBegin_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
} OTF2_MpiCollectiveBegin;


/** @brief MpiCollectiveEnd event.
 */
typedef struct OTF2_MpiCollectiveEnd_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_CollectiveOp collective_op;
    OTF2_CommRef      communicator;
    uint32_t          root;
    uint64_t          size_sent;
    uint64_t          size_received;
} OTF2_MpiCollectiveEnd;


/** @brief OmpFork event.
 */
typedef struct OTF2_OmpFork_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint32_t       number_of_requested_threads;
} OTF2_OmpFork;


/** @brief OmpJoin event.
 */
typedef struct OTF2_OmpJoin_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
} OTF2_OmpJoin;


/** @brief OmpAcquireLock event.
 */
typedef struct OTF2_OmpAcquireLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint32_t       lock_id;
    uint32_t       acquisition_order;
} OTF2_OmpAcquireLock;


/** @brief OmpReleaseLock event.
 */
typedef struct OTF2_OmpReleaseLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint32_t       lock_id;
    uint32_t       acquisition_order;
} OTF2_OmpReleaseLock;


/** @brief OmpTaskCreate event.
 */
typedef struct OTF2_OmpTaskCreate_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint64_t       task_id;
} OTF2_OmpTaskCreate;


/** @brief OmpTaskSwitch event.
 */
typedef struct OTF2_OmpTaskSwitch_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint64_t       task_id;
} OTF2_OmpTaskSwitch;


/** @brief OmpTaskComplete event.
 */
typedef struct OTF2_OmpTaskComplete_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    uint64_t       task_id;
} OTF2_OmpTaskComplete;


/** @brief Metric event.
 */
typedef struct OTF2_Metric_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_MetricRef    metric;
    uint8_t           number_of_metrics;
    OTF2_Type*        type_ids;
    OTF2_MetricValue* metric_values;
} OTF2_Metric;


/** @brief ParameterString event.
 */
typedef struct OTF2_ParameterString_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_ParameterRef parameter;
    OTF2_StringRef    string;
} OTF2_ParameterString;


/** @brief ParameterInt event.
 */
typedef struct OTF2_ParameterInt_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_ParameterRef parameter;
    int64_t           value;
} OTF2_ParameterInt;


/** @brief ParameterUnsignedInt event.
 */
typedef struct OTF2_ParameterUnsignedInt_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_ParameterRef parameter;
    uint64_t          value;
} OTF2_ParameterUnsignedInt;


/** @brief RmaWinCreate event.
 */
typedef struct OTF2_RmaWinCreate_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
} OTF2_RmaWinCreate;


/** @brief RmaWinDestroy event.
 */
typedef struct OTF2_RmaWinDestroy_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
} OTF2_RmaWinDestroy;


/** @brief RmaCollectiveBegin event.
 */
typedef struct OTF2_RmaCollectiveBegin_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
} OTF2_RmaCollectiveBegin;


/** @brief RmaCollectiveEnd event.
 */
typedef struct OTF2_RmaCollectiveEnd_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_CollectiveOp collective_op;
    OTF2_RmaSyncLevel sync_level;
    OTF2_RmaWinRef    win;
    uint32_t          root;
    uint64_t          bytes_sent;
    uint64_t          bytes_received;
} OTF2_RmaCollectiveEnd;


/** @brief RmaGroupSync event.
 */
typedef struct OTF2_RmaGroupSync_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_RmaSyncLevel sync_level;
    OTF2_RmaWinRef    win;
    OTF2_GroupRef     group;
} OTF2_RmaGroupSync;


/** @brief RmaRequestLock event.
 */
typedef struct OTF2_RmaRequestLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint32_t       remote;
    uint64_t       lock_id;
    OTF2_LockType  lock_type;
} OTF2_RmaRequestLock;


/** @brief RmaAcquireLock event.
 */
typedef struct OTF2_RmaAcquireLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint32_t       remote;
    uint64_t       lock_id;
    OTF2_LockType  lock_type;
} OTF2_RmaAcquireLock;


/** @brief RmaTryLock event.
 */
typedef struct OTF2_RmaTryLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint32_t       remote;
    uint64_t       lock_id;
    OTF2_LockType  lock_type;
} OTF2_RmaTryLock;


/** @brief RmaReleaseLock event.
 */
typedef struct OTF2_RmaReleaseLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint32_t       remote;
    uint64_t       lock_id;
} OTF2_RmaReleaseLock;


/** @brief RmaSync event.
 */
typedef struct OTF2_RmaSync_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp   time;
    OTF2_RmaWinRef   win;
    uint32_t         remote;
    OTF2_RmaSyncType sync_type;
} OTF2_RmaSync;


/** @brief RmaWaitChange event.
 */
typedef struct OTF2_RmaWaitChange_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
} OTF2_RmaWaitChange;


/** @brief RmaPut event.
 */
typedef struct OTF2_RmaPut_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint32_t       remote;
    uint64_t       bytes;
    uint64_t       matching_id;
} OTF2_RmaPut;


/** @brief RmaGet event.
 */
typedef struct OTF2_RmaGet_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint32_t       remote;
    uint64_t       bytes;
    uint64_t       matching_id;
} OTF2_RmaGet;


/** @brief RmaAtomic event.
 */
typedef struct OTF2_RmaAtomic_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp     time;
    OTF2_RmaWinRef     win;
    uint32_t           remote;
    OTF2_RmaAtomicType type;
    uint64_t           bytes_sent;
    uint64_t           bytes_received;
    uint64_t           matching_id;
} OTF2_RmaAtomic;


/** @brief RmaOpCompleteBlocking event.
 */
typedef struct OTF2_RmaOpCompleteBlocking_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint64_t       matching_id;
} OTF2_RmaOpCompleteBlocking;


/** @brief RmaOpCompleteNonBlocking event.
 */
typedef struct OTF2_RmaOpCompleteNonBlocking_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint64_t       matching_id;
} OTF2_RmaOpCompleteNonBlocking;


/** @brief RmaOpTest event.
 */
typedef struct OTF2_RmaOpTest_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint64_t       matching_id;
} OTF2_RmaOpTest;


/** @brief RmaOpCompleteRemote event.
 */
typedef struct OTF2_RmaOpCompleteRemote_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_RmaWinRef win;
    uint64_t       matching_id;
} OTF2_RmaOpCompleteRemote;


/** @brief ThreadFork event.
 */
typedef struct OTF2_ThreadFork_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_Paradigm  model;
    uint32_t       number_of_requested_threads;
} OTF2_ThreadFork;


/** @brief ThreadJoin event.
 */
typedef struct OTF2_ThreadJoin_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_Paradigm  model;
} OTF2_ThreadJoin;


/** @brief ThreadTeamBegin event.
 */
typedef struct OTF2_ThreadTeamBegin_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_CommRef   thread_team;
} OTF2_ThreadTeamBegin;


/** @brief ThreadTeamEnd event.
 */
typedef struct OTF2_ThreadTeamEnd_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_CommRef   thread_team;
} OTF2_ThreadTeamEnd;


/** @brief ThreadAcquireLock event.
 */
typedef struct OTF2_ThreadAcquireLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_Paradigm  model;
    uint32_t       lock_id;
    uint32_t       acquisition_order;
} OTF2_ThreadAcquireLock;


/** @brief ThreadReleaseLock event.
 */
typedef struct OTF2_ThreadReleaseLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_Paradigm  model;
    uint32_t       lock_id;
    uint32_t       acquisition_order;
} OTF2_ThreadReleaseLock;


/** @brief ThreadTaskCreate event.
 */
typedef struct OTF2_ThreadTaskCreate_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_CommRef   thread_team;
    uint32_t       creating_thread;
    uint32_t       generation_number;
} OTF2_ThreadTaskCreate;


/** @brief ThreadTaskSwitch event.
 */
typedef struct OTF2_ThreadTaskSwitch_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_CommRef   thread_team;
    uint32_t       creating_thread;
    uint32_t       generation_number;
} OTF2_ThreadTaskSwitch;


/** @brief ThreadTaskComplete event.
 */
typedef struct OTF2_ThreadTaskComplete_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_CommRef   thread_team;
    uint32_t       creating_thread;
    uint32_t       generation_number;
} OTF2_ThreadTaskComplete;


/** @brief ThreadCreate event.
 */
typedef struct OTF2_ThreadCreate_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_CommRef   thread_contingent;
    uint64_t       sequence_count;
} OTF2_ThreadCreate;


/** @brief ThreadBegin event.
 */
typedef struct OTF2_ThreadBegin_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_CommRef   thread_contingent;
    uint64_t       sequence_count;
} OTF2_ThreadBegin;


/** @brief ThreadWait event.
 */
typedef struct OTF2_ThreadWait_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_CommRef   thread_contingent;
    uint64_t       sequence_count;
} OTF2_ThreadWait;


/** @brief ThreadEnd event.
 */
typedef struct OTF2_ThreadEnd_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    OTF2_CommRef   thread_contingent;
    uint64_t       sequence_count;
} OTF2_ThreadEnd;


/** @brief CallingContextEnter event.
 */
typedef struct OTF2_CallingContextEnter_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp         time;
    OTF2_CallingContextRef calling_context;
    uint32_t               unwind_distance;
} OTF2_CallingContextEnter;


/** @brief CallingContextLeave event.
 */
typedef struct OTF2_CallingContextLeave_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp         time;
    OTF2_CallingContextRef calling_context;
} OTF2_CallingContextLeave;


/** @brief CallingContextSample event.
 */
typedef struct OTF2_CallingContextSample_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp             time;
    OTF2_CallingContextRef     calling_context;
    uint32_t                   unwind_distance;
    OTF2_InterruptGeneratorRef interrupt_generator;
} OTF2_CallingContextSample;


/** @brief IoCreateHandle event.
 */
typedef struct OTF2_IoCreateHandle_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp      time;
    OTF2_IoHandleRef    handle;
    OTF2_IoAccessMode   mode;
    OTF2_IoCreationFlag creation_flags;
    OTF2_IoStatusFlag   status_flags;
} OTF2_IoCreateHandle;


/** @brief IoDestroyHandle event.
 */
typedef struct OTF2_IoDestroyHandle_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp   time;
    OTF2_IoHandleRef handle;
} OTF2_IoDestroyHandle;


/** @brief IoDuplicateHandle event.
 */
typedef struct OTF2_IoDuplicateHandle_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_IoHandleRef  old_handle;
    OTF2_IoHandleRef  new_handle;
    OTF2_IoStatusFlag status_flags;
} OTF2_IoDuplicateHandle;


/** @brief IoSeek event.
 */
typedef struct OTF2_IoSeek_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_IoHandleRef  handle;
    int64_t           offset_request;
    OTF2_IoSeekOption whence;
    uint64_t          offset_result;
} OTF2_IoSeek;


/** @brief IoChangeStatusFlags event.
 */
typedef struct OTF2_IoChangeStatusFlags_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp    time;
    OTF2_IoHandleRef  handle;
    OTF2_IoStatusFlag status_flags;
} OTF2_IoChangeStatusFlags;


/** @brief IoDeleteFile event.
 */
typedef struct OTF2_IoDeleteFile_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp     time;
    OTF2_IoParadigmRef io_paradigm;
    OTF2_IoFileRef     file;
} OTF2_IoDeleteFile;


/** @brief IoOperationBegin event.
 */
typedef struct OTF2_IoOperationBegin_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp       time;
    OTF2_IoHandleRef     handle;
    OTF2_IoOperationMode mode;
    OTF2_IoOperationFlag operation_flags;
    uint64_t             bytes_request;
    uint64_t             matching_id;
} OTF2_IoOperationBegin;


/** @brief IoOperationTest event.
 */
typedef struct OTF2_IoOperationTest_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp   time;
    OTF2_IoHandleRef handle;
    uint64_t         matching_id;
} OTF2_IoOperationTest;


/** @brief IoOperationIssued event.
 */
typedef struct OTF2_IoOperationIssued_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp   time;
    OTF2_IoHandleRef handle;
    uint64_t         matching_id;
} OTF2_IoOperationIssued;


/** @brief IoOperationComplete event.
 */
typedef struct OTF2_IoOperationComplete_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp   time;
    OTF2_IoHandleRef handle;
    uint64_t         bytes_result;
    uint64_t         matching_id;
} OTF2_IoOperationComplete;


/** @brief IoOperationCancelled event.
 */
typedef struct OTF2_IoOperationCancelled_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp   time;
    OTF2_IoHandleRef handle;
    uint64_t         matching_id;
} OTF2_IoOperationCancelled;


/** @brief IoAcquireLock event.
 */
typedef struct OTF2_IoAcquireLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp   time;
    OTF2_IoHandleRef handle;
    OTF2_LockType    lock_type;
} OTF2_IoAcquireLock;


/** @brief IoReleaseLock event.
 */
typedef struct OTF2_IoReleaseLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp   time;
    OTF2_IoHandleRef handle;
    OTF2_LockType    lock_type;
} OTF2_IoReleaseLock;


/** @brief IoTryLock event.
 */
typedef struct OTF2_IoTryLock_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp   time;
    OTF2_IoHandleRef handle;
    OTF2_LockType    lock_type;
} OTF2_IoTryLock;


/** @brief ProgramBegin event.
 */
typedef struct OTF2_ProgramBegin_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp  time;
    OTF2_StringRef  program_name;
    uint32_t        number_of_arguments;
    OTF2_StringRef* program_arguments;
} OTF2_ProgramBegin;


/** @brief ProgramEnd event.
 */
typedef struct OTF2_ProgramEnd_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    int64_t        exit_status;
} OTF2_ProgramEnd;


typedef struct OTF2_GenericEvent_struct OTF2_GenericEvent;


struct OTF2_GenericEvent_struct
{
    OTF2_EventType type;
    union
    {
        OTF2_TimeStamp                time; /**< Timestamp, which is exactly here in every record */
        OTF2_BufferFlush              buffer_flush;
        OTF2_MeasurementOnOff         measurement_on_off;
        OTF2_Enter                    enter;
        OTF2_Leave                    leave;
        OTF2_MpiSend                  mpi_send;
        OTF2_MpiIsend                 mpi_isend;
        OTF2_MpiIsendComplete         mpi_isend_complete;
        OTF2_MpiIrecvRequest          mpi_irecv_request;
        OTF2_MpiRecv                  mpi_recv;
        OTF2_MpiIrecv                 mpi_irecv;
        OTF2_MpiRequestTest           mpi_request_test;
        OTF2_MpiRequestCancelled      mpi_request_cancelled;
        OTF2_MpiCollectiveBegin       mpi_collective_begin;
        OTF2_MpiCollectiveEnd         mpi_collective_end;
        OTF2_OmpFork                  omp_fork;
        OTF2_OmpJoin                  omp_join;
        OTF2_OmpAcquireLock           omp_acquire_lock;
        OTF2_OmpReleaseLock           omp_release_lock;
        OTF2_OmpTaskCreate            omp_task_create;
        OTF2_OmpTaskSwitch            omp_task_switch;
        OTF2_OmpTaskComplete          omp_task_complete;
        OTF2_Metric                   metric;
        OTF2_ParameterString          parameter_string;
        OTF2_ParameterInt             parameter_int;
        OTF2_ParameterUnsignedInt     parameter_unsigned_int;
        OTF2_RmaWinCreate             rma_win_create;
        OTF2_RmaWinDestroy            rma_win_destroy;
        OTF2_RmaCollectiveBegin       rma_collective_begin;
        OTF2_RmaCollectiveEnd         rma_collective_end;
        OTF2_RmaGroupSync             rma_group_sync;
        OTF2_RmaRequestLock           rma_request_lock;
        OTF2_RmaAcquireLock           rma_acquire_lock;
        OTF2_RmaTryLock               rma_try_lock;
        OTF2_RmaReleaseLock           rma_release_lock;
        OTF2_RmaSync                  rma_sync;
        OTF2_RmaWaitChange            rma_wait_change;
        OTF2_RmaPut                   rma_put;
        OTF2_RmaGet                   rma_get;
        OTF2_RmaAtomic                rma_atomic;
        OTF2_RmaOpCompleteBlocking    rma_op_complete_blocking;
        OTF2_RmaOpCompleteNonBlocking rma_op_complete_non_blocking;
        OTF2_RmaOpTest                rma_op_test;
        OTF2_RmaOpCompleteRemote      rma_op_complete_remote;
        OTF2_ThreadFork               thread_fork;
        OTF2_ThreadJoin               thread_join;
        OTF2_ThreadTeamBegin          thread_team_begin;
        OTF2_ThreadTeamEnd            thread_team_end;
        OTF2_ThreadAcquireLock        thread_acquire_lock;
        OTF2_ThreadReleaseLock        thread_release_lock;
        OTF2_ThreadTaskCreate         thread_task_create;
        OTF2_ThreadTaskSwitch         thread_task_switch;
        OTF2_ThreadTaskComplete       thread_task_complete;
        OTF2_ThreadCreate             thread_create;
        OTF2_ThreadBegin              thread_begin;
        OTF2_ThreadWait               thread_wait;
        OTF2_ThreadEnd                thread_end;
        OTF2_CallingContextEnter      calling_context_enter;
        OTF2_CallingContextLeave      calling_context_leave;
        OTF2_CallingContextSample     calling_context_sample;
        OTF2_IoCreateHandle           io_create_handle;
        OTF2_IoDestroyHandle          io_destroy_handle;
        OTF2_IoDuplicateHandle        io_duplicate_handle;
        OTF2_IoSeek                   io_seek;
        OTF2_IoChangeStatusFlags      io_change_status_flags;
        OTF2_IoDeleteFile             io_delete_file;
        OTF2_IoOperationBegin         io_operation_begin;
        OTF2_IoOperationTest          io_operation_test;
        OTF2_IoOperationIssued        io_operation_issued;
        OTF2_IoOperationComplete      io_operation_complete;
        OTF2_IoOperationCancelled     io_operation_cancelled;
        OTF2_IoAcquireLock            io_acquire_lock;
        OTF2_IoReleaseLock            io_release_lock;
        OTF2_IoTryLock                io_try_lock;
        OTF2_ProgramBegin             program_begin;
        OTF2_ProgramEnd               program_end;
    } record;
};


struct OTF2_EvtReaderCallbacks_struct
{
    OTF2_EvtReaderCallback_Unknown                  unknown;
    OTF2_EvtReaderCallback_BufferFlush              buffer_flush;
    OTF2_EvtReaderCallback_MeasurementOnOff         measurement_on_off;
    OTF2_EvtReaderCallback_Enter                    enter;
    OTF2_EvtReaderCallback_Leave                    leave;
    OTF2_EvtReaderCallback_MpiSend                  mpi_send;
    OTF2_EvtReaderCallback_MpiIsend                 mpi_isend;
    OTF2_EvtReaderCallback_MpiIsendComplete         mpi_isend_complete;
    OTF2_EvtReaderCallback_MpiIrecvRequest          mpi_irecv_request;
    OTF2_EvtReaderCallback_MpiRecv                  mpi_recv;
    OTF2_EvtReaderCallback_MpiIrecv                 mpi_irecv;
    OTF2_EvtReaderCallback_MpiRequestTest           mpi_request_test;
    OTF2_EvtReaderCallback_MpiRequestCancelled      mpi_request_cancelled;
    OTF2_EvtReaderCallback_MpiCollectiveBegin       mpi_collective_begin;
    OTF2_EvtReaderCallback_MpiCollectiveEnd         mpi_collective_end;
    OTF2_EvtReaderCallback_OmpFork                  omp_fork;
    OTF2_EvtReaderCallback_OmpJoin                  omp_join;
    OTF2_EvtReaderCallback_OmpAcquireLock           omp_acquire_lock;
    OTF2_EvtReaderCallback_OmpReleaseLock           omp_release_lock;
    OTF2_EvtReaderCallback_OmpTaskCreate            omp_task_create;
    OTF2_EvtReaderCallback_OmpTaskSwitch            omp_task_switch;
    OTF2_EvtReaderCallback_OmpTaskComplete          omp_task_complete;
    OTF2_EvtReaderCallback_Metric                   metric;
    OTF2_EvtReaderCallback_ParameterString          parameter_string;
    OTF2_EvtReaderCallback_ParameterInt             parameter_int;
    OTF2_EvtReaderCallback_ParameterUnsignedInt     parameter_unsigned_int;
    OTF2_EvtReaderCallback_RmaWinCreate             rma_win_create;
    OTF2_EvtReaderCallback_RmaWinDestroy            rma_win_destroy;
    OTF2_EvtReaderCallback_RmaCollectiveBegin       rma_collective_begin;
    OTF2_EvtReaderCallback_RmaCollectiveEnd         rma_collective_end;
    OTF2_EvtReaderCallback_RmaGroupSync             rma_group_sync;
    OTF2_EvtReaderCallback_RmaRequestLock           rma_request_lock;
    OTF2_EvtReaderCallback_RmaAcquireLock           rma_acquire_lock;
    OTF2_EvtReaderCallback_RmaTryLock               rma_try_lock;
    OTF2_EvtReaderCallback_RmaReleaseLock           rma_release_lock;
    OTF2_EvtReaderCallback_RmaSync                  rma_sync;
    OTF2_EvtReaderCallback_RmaWaitChange            rma_wait_change;
    OTF2_EvtReaderCallback_RmaPut                   rma_put;
    OTF2_EvtReaderCallback_RmaGet                   rma_get;
    OTF2_EvtReaderCallback_RmaAtomic                rma_atomic;
    OTF2_EvtReaderCallback_RmaOpCompleteBlocking    rma_op_complete_blocking;
    OTF2_EvtReaderCallback_RmaOpCompleteNonBlocking rma_op_complete_non_blocking;
    OTF2_EvtReaderCallback_RmaOpTest                rma_op_test;
    OTF2_EvtReaderCallback_RmaOpCompleteRemote      rma_op_complete_remote;
    OTF2_EvtReaderCallback_ThreadFork               thread_fork;
    OTF2_EvtReaderCallback_ThreadJoin               thread_join;
    OTF2_EvtReaderCallback_ThreadTeamBegin          thread_team_begin;
    OTF2_EvtReaderCallback_ThreadTeamEnd            thread_team_end;
    OTF2_EvtReaderCallback_ThreadAcquireLock        thread_acquire_lock;
    OTF2_EvtReaderCallback_ThreadReleaseLock        thread_release_lock;
    OTF2_EvtReaderCallback_ThreadTaskCreate         thread_task_create;
    OTF2_EvtReaderCallback_ThreadTaskSwitch         thread_task_switch;
    OTF2_EvtReaderCallback_ThreadTaskComplete       thread_task_complete;
    OTF2_EvtReaderCallback_ThreadCreate             thread_create;
    OTF2_EvtReaderCallback_ThreadBegin              thread_begin;
    OTF2_EvtReaderCallback_ThreadWait               thread_wait;
    OTF2_EvtReaderCallback_ThreadEnd                thread_end;
    OTF2_EvtReaderCallback_CallingContextEnter      calling_context_enter;
    OTF2_EvtReaderCallback_CallingContextLeave      calling_context_leave;
    OTF2_EvtReaderCallback_CallingContextSample     calling_context_sample;
    OTF2_EvtReaderCallback_IoCreateHandle           io_create_handle;
    OTF2_EvtReaderCallback_IoDestroyHandle          io_destroy_handle;
    OTF2_EvtReaderCallback_IoDuplicateHandle        io_duplicate_handle;
    OTF2_EvtReaderCallback_IoSeek                   io_seek;
    OTF2_EvtReaderCallback_IoChangeStatusFlags      io_change_status_flags;
    OTF2_EvtReaderCallback_IoDeleteFile             io_delete_file;
    OTF2_EvtReaderCallback_IoOperationBegin         io_operation_begin;
    OTF2_EvtReaderCallback_IoOperationTest          io_operation_test;
    OTF2_EvtReaderCallback_IoOperationIssued        io_operation_issued;
    OTF2_EvtReaderCallback_IoOperationComplete      io_operation_complete;
    OTF2_EvtReaderCallback_IoOperationCancelled     io_operation_cancelled;
    OTF2_EvtReaderCallback_IoAcquireLock            io_acquire_lock;
    OTF2_EvtReaderCallback_IoReleaseLock            io_release_lock;
    OTF2_EvtReaderCallback_IoTryLock                io_try_lock;
    OTF2_EvtReaderCallback_ProgramBegin             program_begin;
    OTF2_EvtReaderCallback_ProgramEnd               program_end;
};


struct OTF2_GlobalEvtReaderCallbacks_struct
{
    OTF2_GlobalEvtReaderCallback_Unknown                  unknown;
    OTF2_GlobalEvtReaderCallback_BufferFlush              buffer_flush;
    OTF2_GlobalEvtReaderCallback_MeasurementOnOff         measurement_on_off;
    OTF2_GlobalEvtReaderCallback_Enter                    enter;
    OTF2_GlobalEvtReaderCallback_Leave                    leave;
    OTF2_GlobalEvtReaderCallback_MpiSend                  mpi_send;
    OTF2_GlobalEvtReaderCallback_MpiIsend                 mpi_isend;
    OTF2_GlobalEvtReaderCallback_MpiIsendComplete         mpi_isend_complete;
    OTF2_GlobalEvtReaderCallback_MpiIrecvRequest          mpi_irecv_request;
    OTF2_GlobalEvtReaderCallback_MpiRecv                  mpi_recv;
    OTF2_GlobalEvtReaderCallback_MpiIrecv                 mpi_irecv;
    OTF2_GlobalEvtReaderCallback_MpiRequestTest           mpi_request_test;
    OTF2_GlobalEvtReaderCallback_MpiRequestCancelled      mpi_request_cancelled;
    OTF2_GlobalEvtReaderCallback_MpiCollectiveBegin       mpi_collective_begin;
    OTF2_GlobalEvtReaderCallback_MpiCollectiveEnd         mpi_collective_end;
    OTF2_GlobalEvtReaderCallback_OmpFork                  omp_fork;
    OTF2_GlobalEvtReaderCallback_OmpJoin                  omp_join;
    OTF2_GlobalEvtReaderCallback_OmpAcquireLock           omp_acquire_lock;
    OTF2_GlobalEvtReaderCallback_OmpReleaseLock           omp_release_lock;
    OTF2_GlobalEvtReaderCallback_OmpTaskCreate            omp_task_create;
    OTF2_GlobalEvtReaderCallback_OmpTaskSwitch            omp_task_switch;
    OTF2_GlobalEvtReaderCallback_OmpTaskComplete          omp_task_complete;
    OTF2_GlobalEvtReaderCallback_Metric                   metric;
    OTF2_GlobalEvtReaderCallback_ParameterString          parameter_string;
    OTF2_GlobalEvtReaderCallback_ParameterInt             parameter_int;
    OTF2_GlobalEvtReaderCallback_ParameterUnsignedInt     parameter_unsigned_int;
    OTF2_GlobalEvtReaderCallback_RmaWinCreate             rma_win_create;
    OTF2_GlobalEvtReaderCallback_RmaWinDestroy            rma_win_destroy;
    OTF2_GlobalEvtReaderCallback_RmaCollectiveBegin       rma_collective_begin;
    OTF2_GlobalEvtReaderCallback_RmaCollectiveEnd         rma_collective_end;
    OTF2_GlobalEvtReaderCallback_RmaGroupSync             rma_group_sync;
    OTF2_GlobalEvtReaderCallback_RmaRequestLock           rma_request_lock;
    OTF2_GlobalEvtReaderCallback_RmaAcquireLock           rma_acquire_lock;
    OTF2_GlobalEvtReaderCallback_RmaTryLock               rma_try_lock;
    OTF2_GlobalEvtReaderCallback_RmaReleaseLock           rma_release_lock;
    OTF2_GlobalEvtReaderCallback_RmaSync                  rma_sync;
    OTF2_GlobalEvtReaderCallback_RmaWaitChange            rma_wait_change;
    OTF2_GlobalEvtReaderCallback_RmaPut                   rma_put;
    OTF2_GlobalEvtReaderCallback_RmaGet                   rma_get;
    OTF2_GlobalEvtReaderCallback_RmaAtomic                rma_atomic;
    OTF2_GlobalEvtReaderCallback_RmaOpCompleteBlocking    rma_op_complete_blocking;
    OTF2_GlobalEvtReaderCallback_RmaOpCompleteNonBlocking rma_op_complete_non_blocking;
    OTF2_GlobalEvtReaderCallback_RmaOpTest                rma_op_test;
    OTF2_GlobalEvtReaderCallback_RmaOpCompleteRemote      rma_op_complete_remote;
    OTF2_GlobalEvtReaderCallback_ThreadFork               thread_fork;
    OTF2_GlobalEvtReaderCallback_ThreadJoin               thread_join;
    OTF2_GlobalEvtReaderCallback_ThreadTeamBegin          thread_team_begin;
    OTF2_GlobalEvtReaderCallback_ThreadTeamEnd            thread_team_end;
    OTF2_GlobalEvtReaderCallback_ThreadAcquireLock        thread_acquire_lock;
    OTF2_GlobalEvtReaderCallback_ThreadReleaseLock        thread_release_lock;
    OTF2_GlobalEvtReaderCallback_ThreadTaskCreate         thread_task_create;
    OTF2_GlobalEvtReaderCallback_ThreadTaskSwitch         thread_task_switch;
    OTF2_GlobalEvtReaderCallback_ThreadTaskComplete       thread_task_complete;
    OTF2_GlobalEvtReaderCallback_ThreadCreate             thread_create;
    OTF2_GlobalEvtReaderCallback_ThreadBegin              thread_begin;
    OTF2_GlobalEvtReaderCallback_ThreadWait               thread_wait;
    OTF2_GlobalEvtReaderCallback_ThreadEnd                thread_end;
    OTF2_GlobalEvtReaderCallback_CallingContextEnter      calling_context_enter;
    OTF2_GlobalEvtReaderCallback_CallingContextLeave      calling_context_leave;
    OTF2_GlobalEvtReaderCallback_CallingContextSample     calling_context_sample;
    OTF2_GlobalEvtReaderCallback_IoCreateHandle           io_create_handle;
    OTF2_GlobalEvtReaderCallback_IoDestroyHandle          io_destroy_handle;
    OTF2_GlobalEvtReaderCallback_IoDuplicateHandle        io_duplicate_handle;
    OTF2_GlobalEvtReaderCallback_IoSeek                   io_seek;
    OTF2_GlobalEvtReaderCallback_IoChangeStatusFlags      io_change_status_flags;
    OTF2_GlobalEvtReaderCallback_IoDeleteFile             io_delete_file;
    OTF2_GlobalEvtReaderCallback_IoOperationBegin         io_operation_begin;
    OTF2_GlobalEvtReaderCallback_IoOperationTest          io_operation_test;
    OTF2_GlobalEvtReaderCallback_IoOperationIssued        io_operation_issued;
    OTF2_GlobalEvtReaderCallback_IoOperationComplete      io_operation_complete;
    OTF2_GlobalEvtReaderCallback_IoOperationCancelled     io_operation_cancelled;
    OTF2_GlobalEvtReaderCallback_IoAcquireLock            io_acquire_lock;
    OTF2_GlobalEvtReaderCallback_IoReleaseLock            io_release_lock;
    OTF2_GlobalEvtReaderCallback_IoTryLock                io_try_lock;
    OTF2_GlobalEvtReaderCallback_ProgramBegin             program_begin;
    OTF2_GlobalEvtReaderCallback_ProgramEnd               program_end;
};


bool
otf2_event_thread_fork_convert_to_omp_fork( OTF2_Archive*          archive,
                                            const OTF2_ThreadFork* threadForkRecord,
                                            OTF2_AttributeList*    attributeList,
                                            OTF2_OmpFork*          ompForkRecord );


bool
otf2_event_thread_join_convert_to_omp_join( OTF2_Archive*          archive,
                                            const OTF2_ThreadJoin* threadJoinRecord,
                                            OTF2_AttributeList*    attributeList,
                                            OTF2_OmpJoin*          ompJoinRecord );


bool
otf2_event_thread_acquire_lock_convert_to_omp_acquire_lock( OTF2_Archive*                 archive,
                                                            const OTF2_ThreadAcquireLock* threadAcquireLockRecord,
                                                            OTF2_AttributeList*           attributeList,
                                                            OTF2_OmpAcquireLock*          ompAcquireLockRecord );


bool
otf2_event_thread_release_lock_convert_to_omp_release_lock( OTF2_Archive*                 archive,
                                                            const OTF2_ThreadReleaseLock* threadReleaseLockRecord,
                                                            OTF2_AttributeList*           attributeList,
                                                            OTF2_OmpReleaseLock*          ompReleaseLockRecord );


bool
otf2_event_calling_context_enter_convert_to_enter( OTF2_Archive*                   archive,
                                                   const OTF2_CallingContextEnter* callingContextEnterRecord,
                                                   OTF2_AttributeList*             attributeList,
                                                   OTF2_Enter*                     enterRecord );


bool
otf2_event_calling_context_leave_convert_to_leave( OTF2_Archive*                   archive,
                                                   const OTF2_CallingContextLeave* callingContextLeaveRecord,
                                                   OTF2_AttributeList*             attributeList,
                                                   OTF2_Leave*                     leaveRecord );


#endif /* OTF2_INTERNAL_EVENTS_H */
