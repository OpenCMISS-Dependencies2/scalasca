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


#ifndef PEARL_PEARL_TYPES_H
#define PEARL_PEARL_TYPES_H


#include <stdint.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    pearl_types.h
 *  @ingroup PEARL_base
 *  @brief   Definition of data types and constants.
 *
 *  This header file provides definitions of PEARL-related data types and
 *  constants.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Type definitions -----------------------------------------------------

/// Data type for timestamp values
typedef double timestamp_t;

/// Data type for identifiers
typedef uint32_t ident_t;

/// Data type for line number information
typedef uint32_t line_t;

/// Enumeration defining available event types
enum event_t
{
    // Generic event types
    ENTER,
    ENTER_CS,
    ENTER_PROGRAM,
    LEAVE,
    LEAVE_PROGRAM,

    // MPI-1 specific event types
    MPI_COLLECTIVE_BEGIN,
    MPI_COLLECTIVE_END,
    MPI_SEND,
    MPI_SEND_REQUEST,    // nonblocking send request
    MPI_SEND_COMPLETE,   // nonblocking send completion
    MPI_RECV,
    MPI_RECV_REQUEST,   // nonblocking receive request
    MPI_RECV_COMPLETE,  // nonblocking receive completion
    MPI_REQUEST_TESTED, // unsuccessful test of request
    MPI_CANCELLED,      // cancel of request

    // Threading specific event types
    // thread model independent
    THREAD_ACQUIRE_LOCK,
    THREAD_RELEASE_LOCK,
    THREAD_TASK_CREATE,
    THREAD_TASK_COMPLETE,
    THREAD_TASK_SWITCH,

    // create wait
    THREAD_BEGIN,
    THREAD_END,
    THREAD_CREATE,
    THREAD_WAIT,

    // fork join
    THREAD_FORK,
    THREAD_JOIN,
    THREAD_TEAM_BEGIN,
    THREAD_TEAM_END,

    // RMA specific event types
    RMA_PUT_START,
    RMA_PUT_END,
    RMA_GET_START,
    RMA_GET_END,

    // MPI-2 RMA specific event types
    MPI_RMA_PUT_START,
    MPI_RMA_PUT_END,

    MPI_RMA_GET_START,
    MPI_RMA_GET_END,

    MPI_RMA_GATS,
    MPI_RMA_COLLECTIVE_BEGIN,
    MPI_RMA_COLLECTIVE_END,

    MPI_RMA_LOCK,
    MPI_RMA_UNLOCK,

    GROUP_ALL,      ///< Refers to an arbitrary event
    GROUP_ENTER,    ///< Refers to all ENTER-type events (i.e., ENTER, ENTER_CS, ENTER_PROGRAM)
    GROUP_LEAVE,    ///< Refers to all LEAVE-type events (i.e., LEAVE, LEAVE_PROGRAM)
    GROUP_SEND,     ///< Refers to all SEND-type events (i.e., MPI_SEND, MPI_SEND_REQUEST)
    GROUP_RECV,     ///< Refers to all RECV-type events (i.e., MPI_RECV, MPI_RECV_COMPLETE)
    GROUP_BEGIN,    ///< Refers to all BEGIN-type events
    GROUP_END,      ///< Refers to all END-type events
    GROUP_NONBLOCK, ///< Refers to all non-blocking events (e.g., MPI_SEND_REQUEST, MPI_SEND_COMPLETE, etc.)

    NUM_EVENT_TYPES    // symbolic name to retrieve the number of event types
};


// --- Constants ------------------------------------------------------------

/// Constant used to identify unused/unknown identifier fields
const uint32_t PEARL_NO_ID = (~static_cast< uint32_t >(0));

/// Constant used to identify undefined/unknown request identifier fields
const uint64_t PEARL_NO_REQUEST = (~static_cast< uint64_t >(0));

/// Constant used to identify unused/unknown number fields
const line_t PEARL_NO_NUM = 0;
}    // namespace pearl


#endif    // !PEARL_PEARL_TYPES_H
