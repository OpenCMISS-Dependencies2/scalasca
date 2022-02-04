/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
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


#ifndef OTF2_CALLBACKS_H
#define OTF2_CALLBACKS_H


/**
 *  @file
 *
 *  @brief      This header file provides all user callbacks.
 */


#ifndef __cplusplus
#include <stdbool.h>
#endif


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 *  @defgroup callbacks OTF2 callbacks
 *  @{
 */


/**
 *  @defgroup callbacks_flush Controlling OTF2 flush behavior in writing mode
 *
 *  The flushing behavior from OTF2 can be controlled via callbacks. Calling
 *  @eref{OTF2_Archive_SetFlushCallbacks} is mandatory when writing and
 *  erroneous when reading an archive.
 *
 *  The pre-flush callback decides whether a flush should actually happen.
 *  When missing, the default is not to flush any data for event writers,
 *  all others will flush there data by default.
 *
 *  The post-flush callback is used to decide whether a buffer flush record
 *  should be written after the flush finished. This only applies to event
 *  writers.
 *
 *  @{
 */


/** @brief Definition for the pre flush callback.
 *
 *  This callback is triggered right before flushing the recorded data into
 *  file when running out of memory.
 *
 *  @param userData    Data passed to the call @eref{OTF2_Archive_SetFlushCallbacks}.
 *  @param fileType    The type of file for what this buffer holds data.
 *  @param location    The location id for what this buffer holds data.
 *                     This is only valid for files of type
 *                     @eref{OTF2_FILETYPE_LOCAL_DEFS} or @eref{OTF2_FILETYPE_EVENTS}.
 *                     For other files this is @eref{OTF2_UNDEFINED_LOCATION}.
 *                     A special case exists for files of type @eref{OTF2_FILETYPE_EVENTS}
 *                     in writing mode. The location ID may still be
 *                     @eref{OTF2_UNDEFINED_LOCATION}. In this case if the application
 *                     wants to write the data from the buffer into the file,
 *                     the application needs to provide a valid location ID
 *                     via a call to @eref{OTF2_EvtWriter_SetLocationID}() and
 *                     utilizing the @p callerData argument.
 *  @param callerData  Depending of the fileType, this can be an @eref{OTF2_EvtWriter},
 *                     @eref{OTF2_GlobalDefWriter}, @eref{OTF2_DefWriter}.
 *  @param final       Indicates whether this is the final flush when closing
 *                     the writer objects.
 *
 *  @return            Returns @eref{OTF2_FLUSH} or @eref{OTF2_NO_FLUSH}.
 */
typedef OTF2_FlushType
( * OTF2_PreFlushCallback )( void*            userData,
                             OTF2_FileType    fileType,
                             OTF2_LocationRef location,
                             void*            callerData,
                             bool final );


/** @brief Definition for the post flush callback.
 *
 *  This callback is triggered right after flushing the recorded data into
 *  file when running out of memory. The main function of this callback is to
 *  provide a timestamp for the end of flushing data into a file. So an
 *  according record can be written correctly.
 *
 *  @param userData Data passed to the call @eref{OTF2_Archive_SetFlushCallbacks}.
 *  @param fileType The file type for which the flush has happened.
 *  @param location The location ID of the writer for which the flush has happened
 *                  (for file types without an ID this is
 *                  @eref{OTF2_UNDEFINED_LOCATION}).
 *
 *  @return Returns a timestamp for the end of flushing data into a file.
 */
typedef OTF2_TimeStamp
( * OTF2_PostFlushCallback )( void*            userData,
                              OTF2_FileType    fileType,
                              OTF2_LocationRef location );


/** @brief Structure holding the flush callbacks.
 *
 *  To be used in a call to @eref{OTF2_Archive_SetFlushCallbacks}.
 *
 *  otf2_post_flush callback may be NULL to suppress writing a
 *  BufferFlush record.
 */
typedef struct OTF2_FlushCallbacks
{
    /** @brief Callback which is called prior a flush. */
    OTF2_PreFlushCallback  otf2_pre_flush;
    /** @brief Callback which is called after a flush. */
    OTF2_PostFlushCallback otf2_post_flush;
} OTF2_FlushCallbacks;


/**
 * @}
 */


/**
 *  @defgroup callbacks_memory Memory pooling for OTF2
 *
 *  It is possible to provide memory for the record chunks to OTF2 via this
 *  callback interface. It is only used for writing. The default memory pool
 *  has a size of 128 MiB per writer.
 *
 *  Note that these callbacks must be thread safe. They are not protected by
 *  the locking callbacks.
 *
 *  @{
 */


/** @brief Function pointer for allocating memory for chunks.
 *
 *  Please note: Do not use this feature if you do not really understand it.
 *  The OTF2 library is not able to do any kind of checks to validate if your
 *  memory management works properly. If you do not use it correctly OTF2's
 *  behavior is undefined including dead locks and all that nasty stuff.
 *
 *  This function must return a pointer to a valid allocated memory location
 *  (just like malloc). This memory location must be of exact same size as the
 *  parameter 'chunkSize' provided with OTF2_Archive_Open().
 *
 *  @param userData      Data passed to the call @eref{OTF2_Archive_SetMemoryCallbacks}.
 *  @param fileType      The file type for which the chunk is requested.
 *  @param location      The location ID of the writer for which the flush has
 *                       happened (for file types without an ID this is
 *                       @eref{OTF2_UNDEFINED_LOCATION}).
 *  @param perBufferData A writable pointer to store callee data. For the first
 *                       call this will be @p NULL.
 *  @param chunkSize     The size of the requested chunk.
 *
 *  @return              Returns a the allocated memory on success, @p NULL if
 *                       an error occurs.
 */
typedef void*
( * OTF2_MemoryAllocate )( void*            userData,
                           OTF2_FileType    fileType,
                           OTF2_LocationRef location,
                           void**           perBufferData,
                           uint64_t         chunkSize );


/** @brief Function pointer to release all allocated chunks.
 *
 *  Please note: Do not use this feature if you do not really understand it.
 *  The OTF2 library is not able to do any kind of checks to validate if your
 *  memory management works properly. If you do not use it correctly OTF2's
 *  behavior is undefined including dead locks and all that nasty stuff.
 *
 *  This function must free all those memory locations that were allocated for a
 *  buffer handle with the according allocate function. Please note: This is
 *  different from a posix free(). You must free _all_ memory locations for that
 *  were allocated for exactly this buffer handle.
 *
 *  @param userData      Data passed to the call @eref{OTF2_Archive_SetMemoryCallbacks}.
 *  @param fileType      The file type for which free is requested.
 *  @param location      The location ID of the writer for which the flush has
 *                       happened (for file types without an ID this is
 *                       @eref{OTF2_UNDEFINED_LOCATION}).
 *  @param perBufferData A writable pointer to store callee data. For the first
 *                       call this will be @p NULL.
 *  @param final         Indicates whether this is the final free when closing
 *                       the writer objects. @p perBufferData should be handled
 *                       than.
 */
typedef void
( * OTF2_MemoryFreeAll )( void*            userData,
                          OTF2_FileType    fileType,
                          OTF2_LocationRef location,
                          void**           perBufferData,
                          bool final );


/** @brief Structure holding the memory callbacks.
 *
 *  To be used in a call to @eref{OTF2_Archive_SetMemoryCallbacks}.
 */
typedef struct OTF2_MemoryCallbacks
{
    /** @brief Callback which is called to allocate a new chunk. */
    OTF2_MemoryAllocate otf2_allocate;
    /** @brief Callback which is called to release all previous allocated chunks. */
    OTF2_MemoryFreeAll  otf2_free_all;
} OTF2_MemoryCallbacks;


/**
 * @}
 */


/**
 *  @defgroup callbacks_collective Operating OTF2 in a collective context
 *
 *  To operate multiple @eref{OTF2_Archive} objects in a collective context, the
 *  following callbacks need to be implemented. These are mandatory, when
 *  writing a trace file with multiple @eref{OTF2_Archive} objects. The
 *  @eref{OTF2_Archive} for which the @eref{OTF2_Collectives_GetRank} result is
 *  @eref{OTF2_COLLECTIVES_ROOT} will be marked as the master object. Only this
 *  is allowed to write global definitions, markers, ….
 *  For reading a set of serial callbacks are provided (See
 *  @eref{OTF2_Archive_SetSerialCollectiveCallbacks} and
 *  @eref{OTF2_Reader_SetSerialCollectiveCallbacks}). The struct
 *  OTF2_CollectiveContext needs to be declared too.
 *
 *  Only @eref{OTF2_Type} of the integer and floating point category need to be
 *  considered as values when the callbacks are called.
 *
 *  Except for the @eref{OTF2_Collectives_GetSize} and
 *  @eref{OTF2_Collectives_GetRank} callbacks, the return value must always be
 *  the same for all participating tasks. In particular all calls should either
 *  return @eref{OTF2_CALLBACK_SUCCESS} or !@eref{OTF2_CALLBACK_SUCCESS}, but
 *  it is undefined, if some of the calls return @eref{OTF2_CALLBACK_SUCCESS}
 *  and other !@eref{OTF2_CALLBACK_SUCCESS}.
 *
 *  The @eref{OTF2_Collectives_CreateLocalComm} and
 *  @eref{OTF2_Collectives_FreeLocalComm} are ignored when writing and optional
 *  when reading, but than both are mandatory. These are used to created the
 *  same local communication context as was given at writing time, if possible.
 *
 *  On the contrary the @a localCommContext to
 *  @eref{OTF2_Archive_SetCollectiveCallbacks} is ignored when reading and
 *  optional (i.e., not NULL) when writing. It determines the number of files
 *  to use when the SION substrate is used. these @a localCommContext must
 *  be a disjoint partitioning of the used @a globalCommContext than.
 *
 *  The @eref{OTF2_Collectives_Release} is optional and will be called as one
 *  of the last actions before the OTF2_Archive or the OTF2_Reader will be
 *  closed.
 *
 *  If any collective callback returns !@eref{OTF2_CALLBACK_SUCCESS}, than
 *  OTF2 returns to the caller the error @eref{OTF2_ERROR_COLLECTIVE_CALLBACK}.
 * @{
 */


/**
 * @brief User provided type for collective groups.
 *
 *  @since Version 1.3
 */
typedef struct OTF2_CollectiveContext OTF2_CollectiveContext;

/** @brief Constant which determines that the master object should be the root
 *         for the collective operation.
 */
#define OTF2_COLLECTIVES_ROOT 0


/** @brief Returns the number of OTF2_Archive objects operating in this
 *  communication context.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_GetSize )( void*                   userData,
                                OTF2_CollectiveContext* commContext,
                                uint32_t*               size );


/** @brief Returns the rank of this OTF2_Archive objects in this
 *  communication context. A number between 0 and one less of the size
 *  of the communication context.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_GetRank )( void*                   userData,
                                OTF2_CollectiveContext* commContext,
                                uint32_t*               rank );


/** @brief Create a new disjoint partitioning of the the @a globalCommContext
 *  communication context. @a numberOfFiles denotes the number of the partitions.
 *  @a fileNumber denotes in which of the partitions this OTF2_Archive
 *  should belong. @a localSize is the size of this partition and @a localRank
 *  the rank of this OTF2_Archive in the partition.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_CreateLocalComm )( void*                    userData,
                                        OTF2_CollectiveContext** localCommContext,
                                        OTF2_CollectiveContext*  globalCommContext,
                                        uint32_t                 globalRank,
                                        uint32_t                 globalSize,
                                        uint32_t                 localRank,
                                        uint32_t                 localSize,
                                        uint32_t                 fileNumber,
                                        uint32_t                 numberOfFiles );


/** @brief Destroys the communication context previous created by the
 *  @eref{OTF2_Collectives_CreateLocalComm} callback.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_FreeLocalComm )( void*                   userData,
                                      OTF2_CollectiveContext* localCommContext );


/** @brief Performs a barrier collective on the given communication context.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_Barrier )( void*                   userData,
                                OTF2_CollectiveContext* commContext );


/** @brief Performs a broadcast collective on the given communication context.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_Bcast )( void*                   userData,
                              OTF2_CollectiveContext* commContext,
                              void*                   data,
                              uint32_t                numberElements,
                              OTF2_Type               type,
                              uint32_t                root );


/** @brief Performs a gather collective on the given communication context
 *  where each ranks contribute the same number of elements.
 *  @a outData is only valid at rank @a root.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_Gather )( void*                   userData,
                               OTF2_CollectiveContext* commContext,
                               const void*             inData,
                               void*                   outData,
                               uint32_t                numberElements,
                               OTF2_Type               type,
                               uint32_t                root );


/** @brief Performs a gather collective on the given communication context
 *  where each ranks contribute different number of elements.
 *  @a outData and @a outElements are only valid at rank @a root.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_Gatherv )( void*                   userData,
                                OTF2_CollectiveContext* commContext,
                                const void*             inData,
                                uint32_t                inElements,
                                void*                   outData,
                                const uint32_t*         outElements,
                                OTF2_Type               type,
                                uint32_t                root );


/** @brief Performs a scatter collective on the given communication context
 *  where each ranks contribute the same number of elements.
 *  @a inData is only valid at rank @a root.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_Scatter )( void*                   userData,
                                OTF2_CollectiveContext* commContext,
                                const void*             inData,
                                void*                   outData,
                                uint32_t                numberElements,
                                OTF2_Type               type,
                                uint32_t                root );


/** @brief Performs a scatter collective on the given communication context
 *  where each ranks contribute different number of elements.
 *  @a inData and @a inElements are only valid at rank @a root.
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Collectives_Scatterv )( void*                   userData,
                                 OTF2_CollectiveContext* commContext,
                                 const void*             inData,
                                 const uint32_t*         inElements,
                                 void*                   outData,
                                 uint32_t                outElements,
                                 OTF2_Type               type,
                                 uint32_t                root );


/** @brief Optionally called in @eref{OTF2_Archive_Close} or
 *  @eref{OTF2_Reader_Close} respectively.
 *
 *
 *  @since Version 1.3
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef void
( * OTF2_Collectives_Release )( void*                   userData,
                                OTF2_CollectiveContext* globalCommContext,
                                OTF2_CollectiveContext* localCommContext );


/** @brief Struct which holds all collective callbacks.
 *
 *  @since Version 1.3
 */
typedef struct OTF2_CollectiveCallbacks
{
    OTF2_Collectives_Release         otf2_release;
    OTF2_Collectives_GetSize         otf2_get_size;
    OTF2_Collectives_GetRank         otf2_get_rank;
    OTF2_Collectives_CreateLocalComm otf2_create_local_comm;
    OTF2_Collectives_FreeLocalComm   otf2_free_local_comm;
    OTF2_Collectives_Barrier         otf2_barrier;
    OTF2_Collectives_Bcast           otf2_bcast;
    OTF2_Collectives_Gather          otf2_gather;
    OTF2_Collectives_Gatherv         otf2_gatherv;
    OTF2_Collectives_Scatter         otf2_scatter;
    OTF2_Collectives_Scatterv        otf2_scatterv;
} OTF2_CollectiveCallbacks;


/**
 * @}
 */


/**
 *  @defgroup callbacks_locking Operating OTF2 in a multi-threads context
 *
 *  The OTF2 objects @eref{OTF2_Archive} and @eref{OTF2_Reader} including all
 *  derived objects from these are by default not thread safe.  That means it is
 *  undefined behavior to operate any of these objects concurrently by multiple
 *  threads.  Note that two independent @eref{OTF2_Archive} or @eref{OTF2_Reader}
 *  objects and there derived objects can be operated by multiple threads
 *  concurrently though.
 *
 *  It is necessary to register the following locking callbacks to make a
 *  @eref{OTF2_Archive} and @eref{OTF2_Reader} and their derived objects thread
 *  safe.  The created locking objects should have normal locking semantics, no
 *  recursive or nesting capability is needed.
 *
 *  OTF2 provides two locking callbacks implementations for Pthread and OpenMP.
 *  See the header files @eref{otf2/OTF2_Pthread_Locks.h} and
 *  @eref{otf2/OTF2_OpenMP_Locks.h}.  For a usage of these headers have a look
 *  into the installed usage examples @refexample{otf2_pthread_writer_example,c}
 *  and @refexample{otf2_openmp_writer_example,c}.
 *
 *  If any locking callback returns !@eref{OTF2_CALLBACK_SUCCESS}, than OTF2
 *  returns to the caller the error @eref{OTF2_ERROR_LOCKING_CALLBACK}.
 * @{
 */


/**
 *  @brief Opaque type for a locking object.
 *
 *  A callback implementation should implement the struct @a OTF2_LockObject.
 *
 *  @since Version 1.5
 */
typedef struct OTF2_LockObject* OTF2_Lock;


/** @brief Creates a new locking object.
 *
 *  @param userData   Value from parameter @a userData passed to
 *                    @eref{OTF2_Archive_SetLockingCallbacks} or
 *                    @eref{OTF2_Reader_SetLockingCallbacks} respectively.
 *  @param[out] lock  Reference to pointer to new lock object.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Locking_Create )( void*      userData,
                           OTF2_Lock* lock );


/** @brief Destroys a locking object.
 *
 *  @param userData  Value from parameter @a userData passed to
 *                   @eref{OTF2_Archive_SetLockingCallbacks} or
 *                   @eref{OTF2_Reader_SetLockingCallbacks} respectively.
 *  @param lock      Lock object to destroy.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Locking_Destroy )( void*     userData,
                            OTF2_Lock lock );


/** @brief Locks a locking object.
 *
 *  @param userData  Value from parameter @a userData passed to
 *                   @eref{OTF2_Archive_SetLockingCallbacks} or
 *                   @eref{OTF2_Reader_SetLockingCallbacks} respectively.
 *  @param lock      Lock object to lock.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Locking_Lock )( void*     userData,
                         OTF2_Lock lock );


/** @brief Unlocks a locking object.
 *
 *  @param userData  Value from parameter @a userData passed to
 *                   @eref{OTF2_Archive_SetLockingCallbacks} or
 *                   @eref{OTF2_Reader_SetLockingCallbacks} respectively.
 *  @param lock      Lock object to unlock.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef OTF2_CallbackCode
( * OTF2_Locking_Unlock )( void*     userData,
                           OTF2_Lock lock );


/** @brief Signals the end of life of the @eref{OTF2_Archive} or
 *  @eref{OTF2_Reader} object to the locking callbacks.
 *
 *  All locking objects created by the OTF2 object were already destroyed
 *  and no further calls to the callbacks will be done after this call.
 *
 *  This callback is optional.
 *
 *  @param userData  Value from parameter @a userData passed to
 *                   @eref{OTF2_Archive_SetLockingCallbacks} or
 *                   @eref{OTF2_Reader_SetLockingCallbacks} respectively.
 *
 *  @since Version 1.5
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_ERROR}.
 */
typedef void
( * OTF2_Locking_Release )( void* userData );


/** @brief Struct which holds all locking callbacks.
 *
 *  @since Version 1.5
 */
typedef struct OTF2_LockingCallbacks
{
    OTF2_Locking_Release otf2_release;
    OTF2_Locking_Create  otf2_create;
    OTF2_Locking_Destroy otf2_destroy;
    OTF2_Locking_Lock    otf2_lock;
    OTF2_Locking_Unlock  otf2_unlock;
} OTF2_LockingCallbacks;


/**
 * @}
 */


/**
 * @}
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_CALLBACKS_H */
