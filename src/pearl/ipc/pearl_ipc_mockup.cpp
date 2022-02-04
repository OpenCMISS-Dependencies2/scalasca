/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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
 *  @ingroup PEARL_ipc
 *  @brief   Implementation of inter-process communication functions for
 *           single-process codes.
 *
 *  This file provides a "dummy" implementation of the PEARL-internal
 *  inter-process communication layer for single-process codes.  See
 *  pearl_ipc.h for a detailed description of the API.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "pearl_ipc.h"

#include <cassert>
#include <climits>
#include <cstring>

using namespace std;
using namespace pearl::detail;


// --- Local helper functions -----------------------------------------------

namespace
{
/// @brief Map PEARL IPC datatype to element size in bytes
///
/// Returns the size of a single element of the given PEARL IPC @a datatype
/// in bytes.
///
/// @param ipcDatatype
///     PEARL IPC datatype
/// @returns
///     Size of a single element in bytes
///
inline size_t
getElementSize(const IpcDatatype ipcDatatype)
{
    static const size_t elementSizes[] = {
        sizeof(int8_t),
        sizeof(uint8_t),
        sizeof(int16_t),
        sizeof(uint16_t),
        sizeof(int32_t),
        sizeof(uint32_t),
        sizeof(int64_t),
        sizeof(uint64_t),
        sizeof(float),
        sizeof(double)
    };

    return elementSizes[ipcDatatype];
}
}    // unnamed namespace


// --- Type definitions -----------------------------------------------------

struct pearl::detail::IpcComm
{
};


// --- Global variables -----------------------------------------------------

IpcComm* pearl::detail::ipcCommWorld = 0;


// --- IPC layer: Environmental management ----------------------------------

void
pearl::detail::ipcInit()
{
    assert(ipcCommWorld == 0);

    ipcCommWorld = new IpcComm;
}


void
pearl::detail::ipcFinalize()
{
    assert(ipcCommWorld != 0);

    delete ipcCommWorld;
    ipcCommWorld = 0;
}


// --- IPC layer: Communication contexts ------------------------------------

uint32_t
pearl::detail::ipcGetSize(const IpcComm* const comm)
{
    assert(comm);

    return 1;
}


uint32_t
pearl::detail::ipcGetRank(const IpcComm* const comm)
{
    assert(comm);

    return 0;
}


void
pearl::detail::ipcCommSplit(const IpcComm* const comm,
                            const uint32_t       color,
                            const uint32_t       key,
                            IpcComm** const      newcomm)
{
    assert(comm);
    assert(color <= INT_MAX);
    assert(key <= INT_MAX);
    assert(newcomm);

    *newcomm = new IpcComm;
}


void
pearl::detail::ipcCommFree(IpcComm** const comm)
{
    assert(comm && (*comm != ipcCommWorld));

    delete *comm;
    *comm = 0;
}


// --- IPC layer: Collective communication ----------------------------------

void
pearl::detail::ipcBarrier(const IpcComm* const comm)
{
    assert(comm);

    // Single process => no synchronization necessary
}


void
pearl::detail::ipcBroadcast(void* const          buffer,
                            const uint32_t       count,
                            const IpcDatatype    datatype,
                            const uint32_t       root,
                            const IpcComm* const comm)
{
    assert(comm);
    assert(buffer || (count == 0));
    assert(count <= INT_MAX);
    assert(root == 0);

    // Single process => data stays in buffer on root
}


void
pearl::detail::ipcGather(const void* const    sendBuffer,
                         void* const          recvBuffer,
                         const uint32_t       count,
                         const IpcDatatype    datatype,
                         const uint32_t       root,
                         const IpcComm* const comm)
{
    assert(comm);
    assert(sendBuffer || (count == 0));
    assert(recvBuffer || (count == 0));
    assert((sendBuffer != recvBuffer) || (count == 0));
    assert(count <= INT_MAX);
    assert(root == 0);

    // Single process => copy data from sendBuffer to recvBuffer
    const size_t numBytes = count * getElementSize(datatype);
    memcpy(recvBuffer, sendBuffer, numBytes);
}


void
pearl::detail::ipcGatherv(const void* const     sendBuffer,
                          const uint32_t        sendCount,
                          void* const           recvBuffer,
                          const uint32_t* const recvCounts,
                          const IpcDatatype     datatype,
                          const uint32_t        root,
                          const IpcComm* const  comm)
{
    assert(comm);
    assert(sendBuffer || (sendCount == 0));
    assert(recvBuffer || (sendCount == 0));
    assert((sendBuffer != recvBuffer) || (sendCount == 0));
    assert(recvCounts && (recvCounts[0] == sendCount));
    assert(root == 0);

    // Single process => copy data from sendBuffer to recvBuffer
    const size_t numBytes = sendCount * getElementSize(datatype);
    memcpy(recvBuffer, sendBuffer, numBytes);
}


void
pearl::detail::ipcScatter(const void* const    sendBuffer,
                          void* const          recvBuffer,
                          const uint32_t       count,
                          const IpcDatatype    datatype,
                          const uint32_t       root,
                          const IpcComm* const comm)
{
    assert(comm);
    assert(sendBuffer || (count == 0));
    assert(recvBuffer || (count == 0));
    assert((sendBuffer != recvBuffer) || (count == 0));
    assert(count <= INT_MAX);
    assert(root == 0);

    // Single process => copy data from sendBuffer to recvBuffer
    const size_t numBytes = count * getElementSize(datatype);
    memcpy(recvBuffer, sendBuffer, numBytes);
}


void
pearl::detail::ipcScatterv(const void* const     sendBuffer,
                           const uint32_t* const sendCounts,
                           void* const           recvBuffer,
                           const uint32_t        recvCount,
                           const IpcDatatype     datatype,
                           const uint32_t        root,
                           const IpcComm* const  comm)
{
    assert(comm);
    assert(sendBuffer || (recvCount == 0));
    assert(sendCounts && (sendCounts[0] == recvCount));
    assert(recvBuffer || (recvCount == 0));
    assert((sendBuffer != recvBuffer) || (recvCount == 0));
    assert(root == 0);

    // Single process => copy data from sendBuffer to recvBuffer
    const size_t numBytes = recvCount * getElementSize(datatype);
    memcpy(recvBuffer, sendBuffer, numBytes);
}
