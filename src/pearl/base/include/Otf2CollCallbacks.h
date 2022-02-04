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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declarations of OTF2 callback functions for collective
 *           inter-process communcication.
 *
 *  This header file provides the declarations of OTF2 callback functions
 *  implementing collective communication operations.
 *
 *  @todo
 *      Move from 'base/include' to 'base' once the trace writer has been
 *      incorporated in PEARL's base library.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_OTF2COLLCALLBACKS_H
#define PEARL_OTF2COLLCALLBACKS_H


#include <otf2/OTF2_Callbacks.h>


namespace pearl
{
namespace detail
{
// --- Forward declarations -------------------------------------------------

struct IpcComm;


// --- Function prototypes --------------------------------------------------

/// @name OTF2: Collective context handling
/// @{

/// @brief Create new OTF2 collective context from IPC communication context.
///
/// Creates a new OTF2 collective context handle from the given PEARL IPC
/// communication context handle @a comm.
///
/// @param comm
///     Communication context
/// @returns
///     Pointer to OTF2 collective context handle if successful
///
OTF2_CollectiveContext*
otf2CreateCollectiveContext(IpcComm* comm);

/// @}
/// @name OTF2: Collective callbacks
/// @{

/// @brief Release communication contexts.
///
/// %Callback function to release the local and global collective communication
/// contexts.  Called when the archive is closed.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param globalCommContext
///     Global communication context
/// @param localCommContext
///     Local communication context
///
extern "C" void
otf2CollCbRelease(void*                   userData,
                  OTF2_CollectiveContext* globalCommContext,
                  OTF2_CollectiveContext* localCommContext);

/// @brief Provide the size of a communication context.
///
/// %Callback function providing the size (i.e., the number of locations) of
/// the communication context @a commContext in the variable pointed to by
/// @a size.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param commContext
///     Communication context
/// @param size
///     Pointer to output variable
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbGetSize(void*                   userData,
                  OTF2_CollectiveContext* commContext,
                  uint32_t*               size);

/// @brief Provide the rank within a communication context.
///
/// %Callback function providing the "rank" of the caller (i.e., a unique ID
/// in the range [0,size(@a commContext)-1]) within the communication context
/// @a commContext in the variable pointed to by @a rank.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param commContext
///     Communication context
/// @param rank
///     Pointer to output variable
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbGetRank(void*                   userData,
                  OTF2_CollectiveContext* commContext,
                  uint32_t*               rank);

/// @brief Create a new local communication context.
///
/// %Callback function called to create a new local collective communication
/// context @a localCommContext from the given global communication context
/// @a globalCommContext according to the provided parameters.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param localCommContext
///     Pointer to output variable (new local communication context)
/// @param globalCommContext
///     Global collective communication context
/// @param globalRank
///     Rank in global communication context
/// @param globalSize
///     Size of global communication context
/// @param localRank
///     Desired rank in new local communication context
/// @param localSize
///     Desired size of new local communication context
/// @param fileNumber
///     SION file number the context is associated with
/// @param numberOfFiles
///     Total number of SION files
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbCreateLocalComm(void*                    userData,
                          OTF2_CollectiveContext** localCommContext,
                          OTF2_CollectiveContext*  globalCommContext,
                          uint32_t                 globalRank,
                          uint32_t                 globalSize,
                          uint32_t                 localRank,
                          uint32_t                 localSize,
                          uint32_t                 fileNumber,
                          uint32_t                 numberOfFiles);

/// @brief Destroy a local communication context.
///
/// Destroys the given local communication context @a localCommContext,
/// previously created by otf2CollCbCreateLocalComm().
///
/// @param userData
///     Pointer to user-provided callback data
/// @param localCommContext
///     Local communication context to destroy
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbFreeLocalComm(void*                   userData,
                        OTF2_CollectiveContext* localCommContext);

/// @brief Synchronization callback.
///
/// %Callback function synchronizing all location within the provided
/// communication context @a commContext. It returns at any location only
/// after all other locations within the context have entered the call.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param commContext
///     Communication context
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbBarrier(void*                   userData,
                  OTF2_CollectiveContext* commContext);

/// @brief Broadcast callback.
///
/// %Callback function broadcasting a message from the location with rank
/// @a root to all other locations in the communication context @a commContext.
/// On return, the contents of @a root's @a buffer (i.e., a message of @a count
/// elements of the given @a type) is copied to all other locations. All
/// locations have to provide the same arguments for @a count, @a type and
/// @a root.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param commContext
///     Communication context
/// @param buffer
///     Pointer to send/receive buffer
/// @param count
///     Number of data elements
/// @param type
///     Type of the data elements
/// @param root
///     Rank of broadcast root
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbBroadcast(void*                   userData,
                    OTF2_CollectiveContext* commContext,
                    void*                   buffer,
                    uint32_t                count,
                    OTF2_Type               type,
                    uint32_t                root);

/// @brief Fixed-size gather callback.
///
/// %Callback function gathering messages of @a count elements of the given
/// @a type provided in @a sendBuffer from each location in the communication
/// context @a commContext on the location with rank @a root. The messages
/// are stored in the root's @a recvBuffer in rank order. All locations have
/// to provide the same arguments for @a count, @a type and @a root; the
/// @a recvBuffer argument is only significant at the root location.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param commContext
///     Communication context
/// @param sendBuffer
///     Pointer to send buffer
/// @param recvBuffer
///     Pointer to receive buffer (significant only at root)
/// @param count
///     Number of data elements in each send buffer
/// @param type
///     Type of the data elements
/// @param root
///     Rank of receiving location
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbGather(void*                   userData,
                 OTF2_CollectiveContext* commContext,
                 const void*             sendBuffer,
                 void*                   recvBuffer,
                 uint32_t                count,
                 OTF2_Type               type,
                 uint32_t                root);

/// @brief Variable-size gather callback.
///
/// %Callback function gathering messages of @a sendCount elements of the given
/// @a type provided in @a sendBuffer from each location in the communication
/// context @a commContext on the location with rank @a root. The messages are
/// stored in root's @a recvBuffer in rank order. All locations have to provide
/// the same arguments for @a type and @a root, however, the values for
/// @a sendCount may differ, with @a sendCount of location <i>i</i> matching
/// @a recvCounts[i]. The @a recvBuffer and @a recvCounts arguments are only
/// significant at the root location.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param commContext
///     Communication context
/// @param sendBuffer
///     Pointer to send buffer
/// @param sendCount
///     Number of data elements in send buffer
/// @param recvBuffer
///     Pointer to receive buffer (significant only at root)
/// @param recvCounts
///     Array containing number of data elements received from each location
///     (significant only at root)
/// @param type
///     Type of the data elements
/// @param root
///     Rank of receiving location
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbGatherv(void*                   userData,
                  OTF2_CollectiveContext* commContext,
                  const void*             sendBuffer,
                  uint32_t                sendCount,
                  void*                   recvBuffer,
                  const uint32_t*         recvCounts,
                  OTF2_Type               type,
                  uint32_t                root);

/// @brief Fixed-size scatter callback.
///
/// %Callback function scattering the data provided in @a sendBuffer on the
/// location with rank @a root to each other location in the communication
/// context @a commContext. The data in @a sendBuffer is split into equal
/// segments of @a count elements of the given @a type, with the <i>i</i>-th
/// segment sent to rank <i>i</i>. All locations have to provide the same
/// arguments for @a count, @a type and @a root, the @a sendBuffer argument
/// is only significant at the root location.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param commContext
///     Communication context
/// @param sendBuffer
///     Pointer to send buffer (significant only at root)
/// @param recvBuffer
///     Pointer to receive buffer
/// @param count
///     Number of data elements to be sent to each location
/// @param type
///     Type of the data elements
/// @param root
///     Rank of sending location
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbScatter(void*                   userData,
                  OTF2_CollectiveContext* commContext,
                  const void*             sendBuffer,
                  void*                   recvBuffer,
                  uint32_t                count,
                  OTF2_Type               type,
                  uint32_t                root);

/// @brief Variable-size scatter callback.
///
/// %Callback function scattering the data provided in @a sendBuffer on the
/// location with rank @a root to each other location in the communication
/// context @a commContext. The data in @a sendBuffer is split into
/// variable-sized segments, with the <i>i</i>-th segment consisting of
/// @a sendCounts[i] elements of the given @a type send to rank <i>i</i>. All
/// locations have to provide the same arguments for @a type and @a root,
/// however, locations can provide different values for @a recvCount, with
/// @a recvCount of rank <i>i</i> matching @a sendCounts[i]. The @a sendBuffer
/// and @a sendCounts arguments are only significant at the root location.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param commContext
///     Communication context
/// @param sendBuffer
///     Pointer to send buffer (significant only at root)
/// @param sendCounts
///     Array containing number of data elements sent to each location
///     (significant only at root)
/// @param recvBuffer
///     Pointer to receive buffer
/// @param recvCount
///     Number of data elements to be received
/// @param type
///     Type of the data elements
/// @param root
///     Rank of sending location
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_CALLBACK_INTERRUPT otherwise
///
extern "C" OTF2_CallbackCode
otf2CollCbScatterv(void*                   userData,
                   OTF2_CollectiveContext* commContext,
                   const void*             sendBuffer,
                   const uint32_t*         sendCounts,
                   void*                   recvBuffer,
                   uint32_t                recvCount,
                   OTF2_Type               type,
                   uint32_t                root);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_OTF2COLLCALLBACKS_H
