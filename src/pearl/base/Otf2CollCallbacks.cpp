/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "Otf2CollCallbacks.h"

#include <cassert>
#include <exception>

#include "Otf2TypeHelpers.h"
#include "pearl_ipc.h"

using namespace std;
using namespace pearl::detail;


// --- Type definitions -----------------------------------------------------

struct OTF2_CollectiveContext
{
    OTF2_CollectiveContext(IpcComm* const comm)
        : mComm(comm)
    {
    }

    IpcComm* mComm;
};


// --- OTF2: Collective context handling ------------------------------------

OTF2_CollectiveContext*
pearl::detail::otf2CreateCollectiveContext(IpcComm* const comm)
{
    return new OTF2_CollectiveContext(comm);
}


// --- OTF2: Collective callbacks -------------------------------------------

void
pearl::detail::otf2CollCbRelease(void* const                   userData,
                                 OTF2_CollectiveContext* const globalCommContext,
                                 OTF2_CollectiveContext* const localCommContext)
{
    delete globalCommContext;
    delete localCommContext;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbGetSize(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext,
                                 uint32_t* const               size)
{
    assert(commContext);
    assert(size);

    try
    {
        *size = ipcGetSize(commContext->mComm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbGetRank(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext,
                                 uint32_t* const               rank)
{
    assert(commContext);
    assert(rank);

    try
    {
        *rank = ipcGetRank(commContext->mComm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbCreateLocalComm(void* const                   userData,
                                         OTF2_CollectiveContext**      localCommContext,
                                         OTF2_CollectiveContext* const globalCommContext,
                                         const uint32_t                globalRank,
                                         const uint32_t                globalSize,
                                         const uint32_t                localRank,
                                         const uint32_t                localSize,
                                         const uint32_t                fileNumber,
                                         const uint32_t                numberOfFiles)
{
    assert(localCommContext);
    assert(globalCommContext);

    try
    {
        IpcComm* comm;
        ipcCommSplit(globalCommContext->mComm, fileNumber, localRank, &comm);

        *localCommContext = otf2CreateCollectiveContext(comm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbFreeLocalComm(void* const                   userData,
                                       OTF2_CollectiveContext* const localCommContext)
{
    assert(localCommContext);

    try
    {
        ipcCommFree(&localCommContext->mComm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbBarrier(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext)
{
    assert(commContext);

    try
    {
        ipcBarrier(commContext->mComm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbBroadcast(void* const                   userData,
                                   OTF2_CollectiveContext* const commContext,
                                   void* const                   buffer,
                                   const uint32_t                count,
                                   const OTF2_Type               type,
                                   const uint32_t                root)
{
    assert(commContext);

    try
    {
        ipcBroadcast(buffer, count, otf2GetIpcDatatype(type), root,
                     commContext->mComm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbGather(void* const                   userData,
                                OTF2_CollectiveContext* const commContext,
                                const void* const             sendBuffer,
                                void* const                   recvBuffer,
                                const uint32_t                count,
                                const OTF2_Type               type,
                                const uint32_t                root)
{
    assert(commContext);

    try
    {
        ipcGather(sendBuffer, recvBuffer, count, otf2GetIpcDatatype(type), root,
                  commContext->mComm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbGatherv(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext,
                                 const void* const             sendBuffer,
                                 const uint32_t                sendCount,
                                 void* const                   recvBuffer,
                                 const uint32_t* const         recvCounts,
                                 const OTF2_Type               type,
                                 const uint32_t                root)
{
    assert(commContext);

    try
    {
        ipcGatherv(sendBuffer, sendCount,
                   recvBuffer, recvCounts, otf2GetIpcDatatype(type), root,
                   commContext->mComm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbScatter(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext,
                                 const void* const             sendBuffer,
                                 void* const                   recvBuffer,
                                 const uint32_t                count,
                                 const OTF2_Type               type,
                                 const uint32_t                root)
{
    assert(commContext);

    try
    {
        ipcScatter(sendBuffer, recvBuffer, count, otf2GetIpcDatatype(type),
                   root, commContext->mComm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbScatterv(void* const                   userData,
                                  OTF2_CollectiveContext* const commContext,
                                  const void* const             sendBuffer,
                                  const uint32_t* const         sendCounts,
                                  void* const                   recvBuffer,
                                  const uint32_t                recvCount,
                                  const OTF2_Type               type,
                                  const uint32_t                root)
{
    assert(commContext);

    try
    {
        ipcScatterv(sendBuffer, sendCounts,
                    recvBuffer, recvCount, otf2GetIpcDatatype(type), root,
                    commContext->mComm);
    }
    catch (exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}
