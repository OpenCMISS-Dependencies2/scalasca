/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
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


#ifndef OTF2_EVENTS_H
#define OTF2_EVENTS_H


/**
 *  @file
 *  @source     templates/OTF2_Events.tmpl.h
 *
 *  @brief      Enums and types used in event records.
 */


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Wrapper for enum @eref{OTF2_MeasurementMode_enum}. */
typedef uint8_t OTF2_MeasurementMode;

/**
 * @brief Types for use in the MeasurementOnOff event.
 *
 * @since Version 1.0
 */
enum OTF2_MeasurementMode_enum
{
    /** @brief The measurement resumed with event recording.
     */
    OTF2_MEASUREMENT_ON  = 1,
    /** @brief The measurement suspended with event recording.
     */
    OTF2_MEASUREMENT_OFF = 2
};


/** @brief Wrapper for enum @eref{OTF2_CollectiveOp_enum}. */
typedef uint8_t OTF2_CollectiveOp;

/**
 * @brief Types of collective operations.
 *
 * @since Version 1.0
 */
enum OTF2_CollectiveOp_enum
{
    /** @brief Barrier synchronization
     */
    OTF2_COLLECTIVE_OP_BARRIER                       = 0,
    /** @brief Broadcast data from one member to all group members
     */
    OTF2_COLLECTIVE_OP_BCAST                         = 1,
    /** @brief Gather data from all group members to one member
     */
    OTF2_COLLECTIVE_OP_GATHER                        = 2,
    /** @brief Gather data from all group members to one member, varying count of
     *  data from each member
     */
    OTF2_COLLECTIVE_OP_GATHERV                       = 3,
    /** @brief Scatter data from one member to all group members
     */
    OTF2_COLLECTIVE_OP_SCATTER                       = 4,
    /** @brief Scatter data from one member to all group members, varying count of
     *  data from each member
     */
    OTF2_COLLECTIVE_OP_SCATTERV                      = 5,
    /** @brief Gather data from all group members, all members of the group will
     *  receive the result
     */
    OTF2_COLLECTIVE_OP_ALLGATHER                     = 6,
    /** @brief Gather data from all group members, varying count of data from each
     *  member, all members of the group will receive the result
     */
    OTF2_COLLECTIVE_OP_ALLGATHERV                    = 7,
    /** @brief Collective scatter/gather operation (complete exchange)
     */
    OTF2_COLLECTIVE_OP_ALLTOALL                      = 8,
    /** @brief Collective scatter/gather operation (complete exchange), varying count
     *  of data from each member
     */
    OTF2_COLLECTIVE_OP_ALLTOALLV                     = 9,
    /** @brief Collective scatter/gather operation (complete exchange), varying count
     *  of data from each member, varying data type from each member
     */
    OTF2_COLLECTIVE_OP_ALLTOALLW                     = 10,
    /** @brief Collective reduction operation, all members of the group will receive
     *  the result
     */
    OTF2_COLLECTIVE_OP_ALLREDUCE                     = 11,
    /** @brief Collective reduction operation
     */
    OTF2_COLLECTIVE_OP_REDUCE                        = 12,
    /** @brief Collective reduce/scatter operation, varying size of scattered blocks
     */
    OTF2_COLLECTIVE_OP_REDUCE_SCATTER                = 13,
    /** @brief Collective scan operation across all members of a group
     */
    OTF2_COLLECTIVE_OP_SCAN                          = 14,
    /** @brief Collective exclusive scan operation across all members of a group
     */
    OTF2_COLLECTIVE_OP_EXSCAN                        = 15,
    /** @brief Collective reduce/scatter operation
     */
    OTF2_COLLECTIVE_OP_REDUCE_SCATTER_BLOCK          = 16,
    /** @brief Collectively create a handle (ie. MPI_Win, MPI_Comm, MPI_File).
     */
    OTF2_COLLECTIVE_OP_CREATE_HANDLE                 = 17,
    /** @brief Collectively destroy a handle.
     */
    OTF2_COLLECTIVE_OP_DESTROY_HANDLE                = 18,
    /** @brief Collectively allocate memory.
     */
    OTF2_COLLECTIVE_OP_ALLOCATE                      = 19,
    /** @brief Collectively deallocate memory.
     */
    OTF2_COLLECTIVE_OP_DEALLOCATE                    = 20,
    /** @brief Collectively create a handle and allocate memory.
     */
    OTF2_COLLECTIVE_OP_CREATE_HANDLE_AND_ALLOCATE    = 21,
    /** @brief Collectively destroy a handle and deallocate memory.
     */
    OTF2_COLLECTIVE_OP_DESTROY_HANDLE_AND_DEALLOCATE = 22
};


/** @brief Wrapper for enum @eref{OTF2_RmaSyncType_enum}. */
typedef uint8_t OTF2_RmaSyncType;

/**
 * @brief Type of direct RMA synchronization call
 *
 * @since Version 1.2
 */
enum OTF2_RmaSyncType_enum
{
    /** @brief Synchronize memory copy.
     */
    OTF2_RMA_SYNC_TYPE_MEMORY     = 0,
    /** @brief Incoming remote notification.
     */
    OTF2_RMA_SYNC_TYPE_NOTIFY_IN  = 1,
    /** @brief Outgoing remote notification.
     */
    OTF2_RMA_SYNC_TYPE_NOTIFY_OUT = 2
};


/** @brief Wrapper for enum @eref{OTF2_RmaSyncLevel_enum}. */
typedef uint32_t OTF2_RmaSyncLevel;

/**
 * @brief Synchronization level used in RMA synchronization records.
 *
 * @since Version 1.2
 */
enum OTF2_RmaSyncLevel_enum
{
    /** @brief No process synchronization or access completion (e.g., MPI_Win_post,
     *  MPI_Win_start).
     */
    OTF2_RMA_SYNC_LEVEL_NONE    = 0,
    /** @brief Synchronize processes (e.g., MPI_Win_create/free).
     */
    OTF2_RMA_SYNC_LEVEL_PROCESS = ( 1 << 0 ),
    /** @brief Complete memory accesses (e.g., MPI_Win_complete, MPI_Win_wait).
     */
    OTF2_RMA_SYNC_LEVEL_MEMORY  = ( 1 << 1 )
};


/** @brief Wrapper for enum @eref{OTF2_LockType_enum}. */
typedef uint8_t OTF2_LockType;

/**
 * @brief General Lock Type.
 *
 * @since Version 1.2
 */
enum OTF2_LockType_enum
{
    /** @brief Exclusive lock. No other lock will be granted.
     */
    OTF2_LOCK_EXCLUSIVE = 0,
    /** @brief Shared lock. Other shared locks will be granted, but no exclusive
     *  locks.
     */
    OTF2_LOCK_SHARED    = 1
};


/** @brief Wrapper for enum @eref{OTF2_RmaAtomicType_enum}. */
typedef uint8_t OTF2_RmaAtomicType;

/**
 * @brief RMA Atomic Operation Type.
 *
 * @since Version 1.2
 */
enum OTF2_RmaAtomicType_enum
{
    /** @brief Atomic accumulate operation.
     */
    OTF2_RMA_ATOMIC_TYPE_ACCUMULATE           = 0,
    /** @brief Atomic increment operation.
     */
    OTF2_RMA_ATOMIC_TYPE_INCREMENT            = 1,
    /** @brief Atomic test-and-set operation.
     */
    OTF2_RMA_ATOMIC_TYPE_TEST_AND_SET         = 2,
    /** @brief Atomic compare-and-swap operation.
     */
    OTF2_RMA_ATOMIC_TYPE_COMPARE_AND_SWAP     = 3,
    /** @brief Atomic swap operation.
     *
     *  @since Version 1.4.
     */
    OTF2_RMA_ATOMIC_TYPE_SWAP                 = 4,
    /** @brief Atomic fetch-and-add operation.
     *
     *  @since Version 1.4.
     */
    OTF2_RMA_ATOMIC_TYPE_FETCH_AND_ADD        = 5,
    /** @brief Atomic fetch-and-increment operation.
     *
     *  @since Version 1.4.
     */
    OTF2_RMA_ATOMIC_TYPE_FETCH_AND_INCREMENT  = 6,
    /** @brief Atomic fetch and accumulate operation with a specified operator.
     *
     *  @since Version 2.1.
     */
    OTF2_RMA_ATOMIC_TYPE_FETCH_AND_ACCUMULATE = 7
};


/** @brief Wrapper for enum @eref{OTF2_IoCreationFlag_enum}. */
typedef uint32_t OTF2_IoCreationFlag;

/**
 * @brief Special I/O handle creation modes when creating the handle with the
 * @eref{IoCreateHandle} event.
 *
 * @since Version 2.1
 */
enum OTF2_IoCreationFlag_enum
{
    /** @brief No flag is set.
     */
    OTF2_IO_CREATION_FLAG_NONE                    = 0,
    /** @brief If the file does not exist, it will be created.
     *
     *  @ioequivalent{POSIX,O_CREAT}
     *  @ioequivalent{ISOC,"w+"\, "a"\, "a+"}
     *  @ioequivalent{MPI-IO,MPI_MODE_CREATE}
     */
    OTF2_IO_CREATION_FLAG_CREATE                  = 1 << 0,
    /** @brief Truncate file to length 0 if possible.
     *
     *  @ioequivalent{POSIX,O_TRUNC}
     *  @ioequivalent{ISOC,"w"\, "w+"}
     */
    OTF2_IO_CREATION_FLAG_TRUNCATE                = 1 << 1,
    /** @brief Open operation will fail if pathname is not a directory.
     *
     *  @ioequivalent{POSIX,O_DIRECTORY}
     */
    OTF2_IO_CREATION_FLAG_DIRECTORY               = 1 << 2,
    /** @brief Ensure that this call creates the file.
     *
     *  @ioequivalent{POSIX,O_EXCL}
     *  @ioequivalent{ISOC,"x" (GNU libc extension)}
     *  @ioequivalent{MPI-IO,MPI_MODE_EXCL}
     */
    OTF2_IO_CREATION_FLAG_EXCLUSIVE               = 1 << 3,
    /** @brief File is a terminal device and should not be promoted to a controlling
     *  terminal, if non existed before.
     *
     *  @ioequivalent{POSIX,O_NOCTTY}
     */
    OTF2_IO_CREATION_FLAG_NO_CONTROLLING_TERMINAL = 1 << 4,
    /** @brief If pathname is a symbolic link, then the open operation will fail.
     *
     *  @ioequivalent{POSIX,O_NOFOLLOW}
     */
    OTF2_IO_CREATION_FLAG_NO_FOLLOW               = 1 << 5,
    /** @brief File is only a location in the filesystem tree and is not suitable for
     *  reading and writing.
     *
     *  @ioequivalent{POSIX,O_PATH}
     */
    OTF2_IO_CREATION_FLAG_PATH                    = 1 << 6,
    /** @brief Create an unnamed temporary file.
     *
     *  @ioequivalent{POSIX,O_TMPFILE}
     */
    OTF2_IO_CREATION_FLAG_TEMPORARY_FILE          = 1 << 7,
    /** @brief Ensure that the file size can be represented by a 64-bit datatype.
     *
     *  @ioequivalentc{POSIX,O_LARGEFILE,
     *                 not listed as a @emph{file creation flag}\, though it
     *                 cannot be changed after creation either}
     */
    OTF2_IO_CREATION_FLAG_LARGEFILE               = 1 << 8,
    /** @brief Gives the advice that no reposition will happen on this I/O handle.
     *  E.g., no seek operation or similar, only sequential read or
     *  write operations.
     *
     *  @ioequivalent{MPI-IO,MPI_MODE_SEQUENTIAL}
     */
    OTF2_IO_CREATION_FLAG_NO_SEEK                 = 1 << 9,
    /** @brief Gives the advice that this will be the only @emph{active}
     *  @eref{IoHandle} of the @eref{IoParadigm} which will operate on
     *  the referenced @eref{IoFile} at any time. E.g., no other
     *  @eref{IoHandle} of the same @eref{IoParadigm} and the same
     *  @eref{IoFile} will be @emph{active}.
     *
     *  @ioequivalent{MPI-IO,MPI_MODE_UNIQUE_OPEN}
     */
    OTF2_IO_CREATION_FLAG_UNIQUE                  = 1 << 10
};


/** @brief Wrapper for enum @eref{OTF2_IoSeekOption_enum}. */
typedef uint8_t OTF2_IoSeekOption;

/**
 * @brief Options for repositioning a file offset with file seek operations.
 *
 * @since Version 2.1
 */
enum OTF2_IoSeekOption_enum
{
    /** @brief The offset is applied to the start of the file.
     *
     *  @ioequivalent{POSIX,SEEK_SET}
     *  @ioequivalent{ISOC,SEEK_SET}
     *  @ioequivalent{MPI-IO,MPI_SEEK_SET}
     */
    OTF2_IO_SEEK_FROM_START   = 0,
    /** @brief The offset is applied to the current file position.
     *
     *  @ioequivalent{POSIX,SEEK_CUR}
     *  @ioequivalent{ISOC,SEEK_CUR}
     *  @ioequivalent{MPI-IO,MPI_SEEK_CUR}
     */
    OTF2_IO_SEEK_FROM_CURRENT = 1,
    /** @brief The offset is applied to the end of the file.
     *
     *  @ioequivalent{POSIX,SEEK_END}
     *  @ioequivalent{ISOC,SEEK_END}
     *  @ioequivalent{MPI-IO,MPI_SEEK_END}
     */
    OTF2_IO_SEEK_FROM_END     = 2,
    /** @brief The offset is applied to the next position in the file containing
     *  data.
     *
     *  @ioequivalent{POSIX,SEEK_DATA}
     */
    OTF2_IO_SEEK_DATA         = 3,
    /** @brief The offset is applied to the next position in the file containing no
     *  data.
     *
     *  @ioequivalent{POSIX,SEEK_HOLE}
     */
    OTF2_IO_SEEK_HOLE         = 4
};


/** @brief Wrapper for enum @eref{OTF2_IoOperationMode_enum}. */
typedef uint8_t OTF2_IoOperationMode;

/**
 * @brief Operation mode of an I/O file operation.
 *
 * @since Version 2.1
 */
enum OTF2_IoOperationMode_enum
{
    /** @brief Read operation.
     */
    OTF2_IO_OPERATION_MODE_READ  = 0,
    /** @brief Write operation.
     */
    OTF2_IO_OPERATION_MODE_WRITE = 1,
    /** @brief Flush operation.
     */
    OTF2_IO_OPERATION_MODE_FLUSH = 2
};


/** @brief Wrapper for enum @eref{OTF2_IoOperationFlag_enum}. */
typedef uint32_t OTF2_IoOperationFlag;

/**
 * @brief Flags for I/O operations to indicate specific semantics of the
 * operation.
 *
 * @since Version 2.1
 */
enum OTF2_IoOperationFlag_enum
{
    /** @brief No special semantics.
     */
    OTF2_IO_OPERATION_FLAG_NONE         = 0,
    /** @brief The I/O operation was performed in a non-blocking mode.
     */
    OTF2_IO_OPERATION_FLAG_NON_BLOCKING = 1 << 0,
    /** @brief The I/O operation is performed collectively over the communicator of
     *  the referenced @eref{IoHandle} handle.
     */
    OTF2_IO_OPERATION_FLAG_COLLECTIVE   = 1 << 1
};


/** @brief Metric value. */
typedef union OTF2_MetricValue_union
{
    int64_t  signed_int;
    uint64_t unsigned_int;
    double   floating_point;
} OTF2_MetricValue;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OTF2_EVENTS_H */
