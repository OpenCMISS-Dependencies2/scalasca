/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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
 *  @brief   Implementation of EPIK callback functions for event records.
 *
 *  This file provides the implementation of EPIK callback functions for
 *  handling event records.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "EpikEvtCallbacks.h"

#include <pearl/CommSet.h>
#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/Location.h>
#include <pearl/MpiCollEnd_rep.h>
#include <pearl/RmaWindow.h>

#include "EpikArchive.h"
#include "EpikMapCallbacks.h"
#include "EventFactory.h"
#include "elg_readcb.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

EpikEvtCbData::EpikEvtCbData(const EpikArchive& archive,
                             const GlobalDefs&  defs,
                             LocalTrace&        trace,
                             EpikMapCbData&     table,
                             uint32_t           version,
                             Location::IdType   locationId,
                             uint32_t           threadId)
    : mArchive(archive),
      mDefinitions(defs),
      mTrace(trace),
      mMappingTable(table),
      mVersion(version),
      mLocation(locationId),
      mThreadId(threadId),
      mRequestId(PEARL_NO_REQUEST)
{
}


// --- Local helper functions -----------------------------------------------

namespace
{
MpiCollEnd_rep::coll_type
convertCollectiveType(const Region& region)
{
    // Sanity check
    if (!is_mpi_collective(region))
    {
        throw FatalError("Inconsistent trace data -- "
                         "MPI collective event found in non-collective region!");
    }

    // Barrier
    if (is_mpi_barrier(region))
    {
        return MpiCollEnd_rep::BARRIER;
    }

    // Prepare region name
    const string name = region.getCanonicalName().getString();

    // 1-to-N collectives
    if (is_mpi_12n(region))
    {
        if (name == "MPI_Bcast")
        {
            return MpiCollEnd_rep::BCAST;
        }
        else if (name == "MPI_Scatter")
        {
            return MpiCollEnd_rep::SCATTER;
        }
        else if (name == "MPI_Scatterv")
        {
            return MpiCollEnd_rep::SCATTERV;
        }
    }

    // N-to-1 collectives
    if (is_mpi_n21(region))
    {
        if (name == "MPI_Reduce")
        {
            return MpiCollEnd_rep::REDUCE;
        }
        else if (name == "MPI_Gather")
        {
            return MpiCollEnd_rep::GATHER;
        }
        else if (name == "MPI_Gatherv")
        {
            return MpiCollEnd_rep::GATHERV;
        }
    }

    // N-to-N collectives
    if (is_mpi_n2n(region))
    {
        if (name == "MPI_Allgather")
        {
            return MpiCollEnd_rep::ALLGATHER;
        }
        else if (name == "MPI_Allgatherv")
        {
            return MpiCollEnd_rep::ALLGATHERV;
        }
        else if (name == "MPI_Allreduce")
        {
            return MpiCollEnd_rep::ALLREDUCE;
        }
        else if (name == "MPI_Alltoall")
        {
            return MpiCollEnd_rep::ALLTOALL;
        }
        else if (name == "MPI_Alltoallv")
        {
            return MpiCollEnd_rep::ALLTOALLV;
        }
        else if (name == "MPI_Alltoallw")
        {
            return MpiCollEnd_rep::ALLTOALLW;
        }
        else if (name == "MPI_Reduce_scatter")
        {
            return MpiCollEnd_rep::REDUCE_SCATTER;
        }
        else if (name == "MPI_Reduce_scatter_block")
        {
            return MpiCollEnd_rep::REDUCE_SCATTER_BLOCK;
        }
    }

    // Prefix reductions
    if (is_mpi_scan(region))
    {
        if (name == "MPI_Scan")
        {
            return MpiCollEnd_rep::SCAN;
        }
        else if (name == "MPI_Exscan")
        {
            return MpiCollEnd_rep::EXSCAN;
        }
    }

    // We should never reach this point...
    assert(false);

    return MpiCollEnd_rep::BARRIER;
}
}    // unnamed namespace


// --- Local helper macros --------------------------------------------------

#define CONV_ID(id)    (((id) == ELG_NO_ID) ? PEARL_NO_ID : (id))

// *INDENT-OFF*    Uncrustify issues #2742 & 2791
#define CALLBACK_SETUP                                                       \
    EpikEvtCbData*    data  = static_cast< EpikEvtCbData* >(userdata);       \
    const GlobalDefs& defs  = data->mDefinitions;                            \
    LocalTrace&       trace = data->mTrace;                                  \
    EpikMapCbData&    table = data->mMappingTable;                           \
                                                                             \
    try                                                                      \
    {                                                                        \
        Location::IdType mapped_lid;                                         \
        mapped_lid = data->mArchive.mapLocationId(table.mapLocationId(lid)); \
        if (mapped_lid != data->mLocation)                                   \
        {                                                                    \
            data->mRequestId = PEARL_NO_REQUEST;                             \
                                                                             \
            return;                                                          \
        }

#define CALLBACK_CLEANUP                 \
    data->mRequestId = PEARL_NO_REQUEST; \
    }    /* Closes the try block */      \
    catch (exception& ex)                \
    {                                    \
        data->mErrorMessage = ex.what(); \
    }

#define ATTR_CALLBACK_SETUP \
    EpikEvtCbData* data = static_cast< EpikEvtCbData* >(userdata);
// *INDENT-ON*


// --- EPIK: Event callbacks ------------------------------------------------

void
elg_readcb_ATTR_UI1(elg_ui1 type,
                    elg_ui1 val,
                    void*   userdata)
{
}


void
elg_readcb_ATTR_UI4(elg_ui1 type,
                    elg_ui4 val,
                    void*   userdata)
{
    ATTR_CALLBACK_SETUP

    switch (type)
    {
        case ELG_ATTR_REQUEST:
            data->mRequestId = val;
            break;

        default:
            break;
    }
}


void
elg_readcb_ENTER(elg_ui4  lid,     /* location identifier */
                 elg_d8   time,    /* time stamp */
                 elg_ui4  rid,     /* region identifier of the region being entered */
                 elg_ui1  metc,    /* number of metrics */
                 elg_ui8* metv,    /* metric values */
                 void*    userdata)
{
    CALLBACK_SETUP

    // Callstack update
    uint32_t regionId = table.mapRegionId(CONV_ID(rid));
    data->mCallstack.push(regionId);
    const Region& region = defs.getRegion(regionId);
    Event_rep*    event;

    // artificial ThreadTeamBegin on enter of a parallel region
    const uint32_t rank = trace.get_location().getRank();
    if (is_omp_parallel(region))
    {
        event =
            EventFactory::instance()->createThreadTeamBegin(defs,
                                                            table.mapTimestamp(time),
                                                            data->mArchive.getThreadTeamId(rank));
        if (event)
        {
            trace.add_event(event);
        }
    }

    // Create event
    event =
        EventFactory::instance()->createEnter(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }

    // Special handling for MPI collectives
    if (is_mpi_collective(region))
    {
        event =
            EventFactory::instance()->createMpiCollBegin(defs,
                                                         table.mapTimestamp(time));

        if (event)
        {
            trace.add_event(event);
        }
    }

    // Special handling for MPI RMA collectives
    else if (is_mpi_rma_collective(region))
    {
        event =
            EventFactory::instance()->createMpiRmaCollBegin(defs,
                                                            table.mapTimestamp(time));

        if (event)
        {
            trace.add_event(event);
        }
    }

    CALLBACK_CLEANUP
}


void
elg_readcb_EXIT(elg_ui4  lid,     /* location identifier */
                elg_d8   time,    /* time stamp */
                elg_ui1  metc,    /* number of metrics */
                elg_ui8* metv,    /* metric values */
                void*    userdata)
{
    CALLBACK_SETUP

    // Consistency check and callstack update
    if (data->mCallstack.empty())
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many LEAVEs).");
    }
    uint32_t regionId = data->mCallstack.top();
    data->mCallstack.pop();

    const Region& region = defs.getRegion(regionId);
    Event_rep*    event;

    // Create event
    event =
        EventFactory::instance()->createLeave(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }

    if (is_omp_parallel(region))
    {
        const uint32_t rank = trace.get_location().getRank();
        event =
            EventFactory::instance()->createThreadTeamEnd(defs,
                                                          table.mapTimestamp(time),
                                                          data->mArchive.getThreadTeamId(rank));
        if (event)
        {
            trace.add_event(event);
        }
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_ENTER_CS(elg_ui4  lid,     /* location identifier */
                    elg_d8   time,    /* time stamp */
                    elg_ui4  csid,    /* call site identifier of the call site being entered */
                    elg_ui1  metc,    /* number of metrics */
                    elg_ui8* metv,    /* metric values */
                    void*    userdata)
{
    CALLBACK_SETUP

    // Callstack update
    uint32_t callsiteId = table.mapCallsiteId(CONV_ID(csid));
    uint32_t regionId   = defs.getCallsite(callsiteId).getCallee().getId();
    data->mCallstack.push(regionId);
    const Region& region = defs.getRegion(regionId);
    Event_rep*    event;

    if (is_omp_parallel(region))
    {
        const uint32_t rank = trace.get_location().getRank();
        event =
            EventFactory::instance()->createThreadTeamBegin(defs,
                                                            table.mapTimestamp(time),
                                                            data->mArchive.getThreadTeamId(rank));
        if (event)
        {
            trace.add_event(event);
        }
    }

    // Create event
    event =
        EventFactory::instance()->createEnterCS(defs,
                                                table.mapTimestamp(time),
                                                callsiteId,
                                                metv);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_SEND(elg_ui4 lid,    /* location identifier */
                    elg_d8  time,   /* time stamp */
                    elg_ui4 dlid,   /* destination location identifier */
                    elg_ui4 cid,    /* communicator identifier */
                    elg_ui4 tag,    /* message tag */
                    elg_ui4 sent,   /* message length in bytes */
                    void*   userdata)
{
    CALLBACK_SETUP

    // Map communicator ID
    ident_t comm_id = table.mapCommunicatorId(CONV_ID(cid));

    // Pre-1.8 traces: convert global |-> local destination rank
    if (data->mVersion < 1008)
    {
        dlid = defs.get_comm(comm_id)->getCommSet().getLocalRank(dlid);
    }
    Event_rep* event;
    if (data->mRequestId == PEARL_NO_REQUEST)
    {
        event =
            EventFactory::instance()->createMpiSend(defs,
                                                    table.mapTimestamp(time),
                                                    comm_id,
                                                    dlid,
                                                    tag,
                                                    sent);
    }
    else
    {
        event =
            EventFactory::instance()->createMpiSendRequest(defs,
                                                           table.mapTimestamp(time),
                                                           comm_id,
                                                           dlid,
                                                           tag,
                                                           sent,
                                                           data->mRequestId);
        data->mActiveRequests.insert(data->mRequestId);
    }
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_RECV(elg_ui4 lid,    /* location identifier */
                    elg_d8  time,   /* time stamp */
                    elg_ui4 slid,   /* source location identifier */
                    elg_ui4 cid,    /* communicator identifier */
                    elg_ui4 tag,    /* message tag */
                    void*   userdata)
{
    CALLBACK_SETUP

    // Map communicator ID
    ident_t comm_id = table.mapCommunicatorId(CONV_ID(cid));

    // Pre-1.8 traces: convert global |-> local source rank
    if (data->mVersion < 1008)
    {
        slid = defs.get_comm(comm_id)->getCommSet().getLocalRank(slid);
    }
    Event_rep* event;
    if (data->mRequestId == PEARL_NO_REQUEST)
    {
        event =
            EventFactory::instance()->createMpiRecv(defs,
                                                    table.mapTimestamp(time),
                                                    comm_id,
                                                    slid,
                                                    tag,
                                                    0);
    }
    else
    {
        event =
            EventFactory::instance()->createMpiRecvComplete(defs,
                                                            table.mapTimestamp(time),
                                                            comm_id,
                                                            slid,
                                                            tag,
                                                            0,
                                                            data->mRequestId);
        data->mActiveRequests.erase(data->mRequestId);
    }
    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_COLLEXIT(elg_ui4  lid,     /* location identifier */
                        elg_d8   time,    /* time stamp */
                        elg_ui1  metc,    /* number of metrics */
                        elg_ui8* metv,    /* metric values */
                        elg_ui4  rlid,    /* root location identifier */
                        elg_ui4  cid,     /* communicator identifier */
                        elg_ui4  sent,    /* bytes sent */
                        elg_ui4  recvd,   /* bytes received */
                        void*    userdata)
{
    CALLBACK_SETUP

    // Consistency check and callstack update
    if (data->mCallstack.empty())
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many LEAVEs).");
    }
    uint32_t regionId = data->mCallstack.top();
    data->mCallstack.pop();

    // Map communicator ID
    ident_t comm_id = table.mapCommunicatorId(CONV_ID(cid));

    // Pre-1.8 traces: convert global |-> local root rank
    if ((rlid != ELG_NO_ID) && (data->mVersion < 1008))
    {
        rlid = defs.get_comm(comm_id)->getCommSet().getLocalRank(rlid);
    }
    const Region& region = defs.getRegion(regionId);
    Event_rep*    event  =
        EventFactory::instance()->createMpiCollEnd(defs,
                                                   table.mapTimestamp(time),
                                                   convertCollectiveType(region),
                                                   comm_id,
                                                   (rlid == ELG_NO_ID)
                                                   ? PEARL_NO_ID
                                                   : rlid,
                                                   sent,
                                                   recvd);

    if (event)
    {
        trace.add_event(event);
    }
    event =
        EventFactory::instance()->createLeave(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_SEND_COMPLETE(elg_ui4 lid,    /* location identifier */
                             elg_d8  time,   /* time stamp */
                             elg_ui4 reqid,  /* request identifier of completed send */
                             void*   userdata)
{
    CALLBACK_SETUP

    assert(reqid != PEARL_NO_ID);

    Event_rep* event =
        EventFactory::instance()->createMpiSendComplete(defs,
                                                        table.mapTimestamp(time),
                                                        reqid);
    data->mActiveRequests.erase(static_cast< uint64_t >(reqid));

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_RECV_REQUEST(elg_ui4 lid,    /* location identifier */
                            elg_d8  time,   /* time stamp */
                            elg_ui4 reqid,  /* receive request identifier */
                            void*   userdata)
{
    CALLBACK_SETUP

    assert(reqid != PEARL_NO_ID);

    Event_rep* event =
        EventFactory::instance()->createMpiRecvRequest(defs,
                                                       table.mapTimestamp(time),
                                                       reqid);
    data->mActiveRequests.insert(static_cast< uint64_t >(reqid));

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_REQUEST_TESTED(elg_ui4 lid,    /* location identifier */
                              elg_d8  time,   /* time stamp */
                              elg_ui4 reqid,  /* receive request identifier */
                              void*   userdata)
{
    CALLBACK_SETUP

    assert(reqid != PEARL_NO_ID);

    // Only keep REQUEST_TESTED events of active requests
    const uint64_t id = static_cast< uint64_t >(reqid);
    if (data->mActiveRequests.find(id) != data->mActiveRequests.end())
    {
        Event_rep* event =
            EventFactory::instance()->createMpiRequestTested(defs,
                                                             table.mapTimestamp(time),
                                                             reqid);

        if (event)
        {
            trace.add_event(event);
        }
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_CANCELLED(elg_ui4 lid,    /* location identifier */
                         elg_d8  time,   /* time stamp */
                         elg_ui4 reqid,  /* receive request identifier */
                         void*   userdata)
{
    CALLBACK_SETUP

    Event_rep* event =
        EventFactory::instance()->createMpiCancelled(defs,
                                                     table.mapTimestamp(time),
                                                     reqid);
    data->mActiveRequests.erase(static_cast< uint64_t >(reqid));

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_PUT_1TS(elg_ui4 lid,    /* location identifier */
                       elg_d8  time,   /* time stamp */
                       elg_ui4 tlid,   /* target location identifier */
                       elg_ui4 wid,    /* window identifier */
                       elg_ui4 rmaid,  /* RMA operation identifier */
                       elg_ui4 nbytes, /* message length in bytes */
                       void*   userdata)
{
    CALLBACK_SETUP

    // Map window ID
    ident_t win_id = table.mapWindowId(CONV_ID(wid));

    // Pre-1.8 traces: convert global |-> local target rank
    if (data->mVersion < 1008)
    {
        tlid = defs.get_window(win_id)->get_comm()->getCommSet().getLocalRank(CONV_ID(tlid));
    }
    Event_rep* event =
        EventFactory::instance()->createMpiRmaPutStart(defs,
                                                       table.mapTimestamp(time),
                                                       rmaid,
                                                       tlid,
                                                       nbytes,
                                                       win_id);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_PUT_1TE(elg_ui4 lid,    /* location identifier */
                       elg_d8  time,   /* time stamp */
                       elg_ui4 dlid,   /* destination location identifier */
                       elg_ui4 wid,    /* window identifier */
                       elg_ui4 rmaid,  /* RMA operation identifier */
                       void*   userdata)
{
    /* NOTE: As we are no longer rewriting the trace, these events will
     *       never exist in the remote trace. We work directly on the
     *       _REMOTE events. Eventually, these event will deprecate. */
}


void
elg_readcb_MPI_PUT_1TE_REMOTE(elg_ui4 lid,    /* location identifier */
                              elg_d8  time,   /* time stamp */
                              elg_ui4 dlid,   /* destination location identifier */
                              elg_ui4 wid,    /* window identifier */
                              elg_ui4 rmaid,  /* RMA operation identifier */
                              void*   userdata)
{
    CALLBACK_SETUP

    Event_rep* event =
        EventFactory::instance()->createMpiRmaPutEnd(defs,
                                                     table.mapTimestamp(time),
                                                     rmaid);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_GET_1TS(elg_ui4 lid,    /* location identifier */
                       elg_d8  time,   /* time stamp */
                       elg_ui4 dlid,   /* destination location identifier */
                       elg_ui4 wid,    /* window identifier */
                       elg_ui4 rmaid,  /* RMA operation identifier */
                       elg_ui4 nbytes, /* message length in bytes */
                       void*   userdata)
{
    /* NOTE: As we are no longer rewriting the trace, these events will
     *       never exist in the remote trace. We work directly on the
     *       _REMOTE events. Eventually, these event will deprecate. */
}


void
elg_readcb_MPI_GET_1TS_REMOTE(elg_ui4 lid,    /* location identifier */
                              elg_d8  time,   /* time stamp */
                              elg_ui4 dlid,   /* destination location identifier */
                              elg_ui4 wid,    /* window identifier */
                              elg_ui4 rmaid,  /* RMA operation identifier */
                              elg_ui4 nbytes, /* message length in bytes */
                              void*   userdata)
{
    CALLBACK_SETUP

    // Map window ID
    ident_t win_id = table.mapWindowId(CONV_ID(wid));

    // Pre-1.8 traces: convert global |-> local origin rank
    if (data->mVersion < 1008)
    {
        dlid = defs.get_window(win_id)->get_comm()->getCommSet().getLocalRank(CONV_ID(dlid));
    }
    Event_rep* event =
        EventFactory::instance()->createMpiRmaGetStart(defs,
                                                       table.mapTimestamp(time),
                                                       rmaid,
                                                       dlid,
                                                       nbytes,
                                                       win_id);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_GET_1TE(elg_ui4 lid,    /* location identifier */
                       elg_d8  time,   /* time stamp */
                       elg_ui4 slid,   /* source location identifier */
                       elg_ui4 wid,    /* window identifier */
                       elg_ui4 rmaid,  /* RMA operation identifier */
                       void*   userdata)
{
    CALLBACK_SETUP

    Event_rep* event =
        EventFactory::instance()->createMpiRmaGetEnd(defs,
                                                     table.mapTimestamp(time),
                                                     rmaid);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_GET_1TO(elg_ui4 lid,    /* location identifier */
                       elg_d8  time,   /* time stamp */
                       elg_ui4 rmaid,  /* RMA operation identifier */
                       void*   userdata)
{
    /* NOTE: These events are currently completely ignored, as they are
     *       not used for analysis, but only for visualisation. */
}


void
elg_readcb_MPI_WINEXIT(elg_ui4  lid,     /* location identifier */
                       elg_d8   time,    /* time stamp */
                       elg_ui1  metc,    /* number of metrics */
                       elg_ui8* metv,    /* metric values */
                       elg_ui4  wid,     /* window identifier */
                       elg_ui4  cid,     /* communicator identifier */
                       elg_ui1  synex,   /* synchronization exit flag */
                       void*    userdata)
{
    CALLBACK_SETUP

    // Consistency check and callstack update
    if (data->mCallstack.empty())
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many LEAVEs).");
    }
    uint32_t regionId = data->mCallstack.top();
    data->mCallstack.pop();

    // Map communication set ID
    const uint32_t commSetId = (data->mVersion < 1008)
                               ? table.mapCommunicatorId(CONV_ID(cid))
                               : table.mapGroupId(CONV_ID(cid));

    // Create event
    Event_rep* event =
        EventFactory::instance()->createMpiRmaGats(defs,
                                                   table.mapTimestamp(time),
                                                   table.mapWindowId(CONV_ID(wid)),
                                                   data->mArchive.mapCommSetId(commSetId),
                                                   synex);

    if (event)
    {
        trace.add_event(event);
    }
    event =
        EventFactory::instance()->createLeave(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_WINCOLLEXIT(elg_ui4  lid,     /* location identifier */
                           elg_d8   time,    /* time stamp */
                           elg_ui1  metc,    /* number of metrics */
                           elg_ui8* metv,    /* metric values */
                           elg_ui4  wid,     /* window identifier */
                           void*    userdata)
{
    CALLBACK_SETUP

    // Consistency check and callstack update
    if (data->mCallstack.empty())
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many LEAVEs).");
    }
    uint32_t regionId = data->mCallstack.top();
    data->mCallstack.pop();

    // Create event
    Event_rep* event =
        EventFactory::instance()->createMpiRmaCollEnd(defs,
                                                      table.mapTimestamp(time),
                                                      table.mapWindowId(CONV_ID(wid)));

    if (event)
    {
        trace.add_event(event);
    }

    // Create event
    event =
        EventFactory::instance()->createLeave(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_WIN_LOCK(elg_ui4 lid,    /* location identifier */
                        elg_d8  time,   /* time stamp */
                        elg_ui4 llid,   /* lock location identifier */
                        elg_ui4 wid,    /* window identifier */
                        elg_ui1 ltype,  /* lock type */
                        void*   userdata)
{
    CALLBACK_SETUP

    // Map window ID
    ident_t win_id = table.mapWindowId(CONV_ID(wid));

    // Pre-1.8 traces: convert global |-> local remote rank
    if (data->mVersion < 1008)
    {
        llid = defs.get_window(win_id)->get_comm()->getCommSet().getLocalRank(CONV_ID(llid));
    }
    Event_rep* event =
        EventFactory::instance()->createMpiRmaLock(defs,
                                                   table.mapTimestamp(time),
                                                   llid,
                                                   win_id,
                                                   ltype);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_WIN_UNLOCK(elg_ui4 lid,    /* location identifier */
                          elg_d8  time,   /* time stamp */
                          elg_ui4 llid,   /* lock location identifier */
                          elg_ui4 wid,    /* window identifier */
                          void*   userdata)
{
    CALLBACK_SETUP

    // Map window ID
    ident_t win_id = table.mapWindowId(CONV_ID(wid));

    // Pre-1.8 traces: convert global |-> local remote rank
    if (data->mVersion < 1008)
    {
        llid = defs.get_window(win_id)->get_comm()->getCommSet().getLocalRank(CONV_ID(llid));
    }
    Event_rep* event =
        EventFactory::instance()->createMpiRmaUnlock(defs,
                                                     table.mapTimestamp(time),
                                                     llid,
                                                     win_id);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_PUT_1TS(elg_ui4 lid,    /* location identifier */
                   elg_d8  time,   /* time stamp */
                   elg_ui4 dlid,   /* destination location identifier */
                   elg_ui4 rmaid,  /* RMA operation identifier */
                   elg_ui4 nbytes, /* message length in bytes */
                   void*   userdata)
{
    CALLBACK_SETUP

    Event_rep* event =
        EventFactory::instance()->createRmaPutStart(defs,
                                                    table.mapTimestamp(time),
                                                    rmaid,
                                                    dlid,
                                                    nbytes);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_PUT_1TE(elg_ui4 lid,    /* location identifier */
                   elg_d8  time,   /* time stamp */
                   elg_ui4 slid,   /* source location identifier */
                   elg_ui4 rmaid,  /* RMA operation identifier */
                   void*   userdata)
{
    /* NOTE: As we are no longer rewriting the trace, these events will
     *       never exist in the remote trace. We work directly on the
     *       _REMOTE events. Eventually, these event will deprecate. */
}


void
elg_readcb_PUT_1TE_REMOTE(elg_ui4 lid,    /* location identifier */
                          elg_d8  time,   /* time stamp */
                          elg_ui4 dlid,   /* destination location identifier */
                          elg_ui4 rmaid,  /* RMA operation identifier */
                          void*   userdata)
{
    CALLBACK_SETUP

    Event_rep* event =
        EventFactory::instance()->createRmaPutEnd(defs,
                                                  table.mapTimestamp(time),
                                                  rmaid);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_GET_1TS(elg_ui4 lid,    /* location identifier */
                   elg_d8  time,   /* time stamp */
                   elg_ui4 tlid,   /* target location identifier */
                   elg_ui4 rmaid,  /* RMA operation identifier */
                   elg_ui4 nbytes, /* message length in bytes */
                   void*   userdata)
{
    /* NOTE: As we are no longer rewriting the trace, these events will
     *       never exist in the remote trace. We work directly on the
     *       _REMOTE events. Eventually, these event will deprecate.*/
}


void
elg_readcb_GET_1TS_REMOTE(elg_ui4 lid,    /* location identifier */
                          elg_d8  time,   /* time stamp */
                          elg_ui4 tlid,   /* target location identifier */
                          elg_ui4 rmaid,  /* RMA operation identifier */
                          elg_ui4 nbytes, /* message length in bytes */
                          void*   userdata)
{
    CALLBACK_SETUP

    Event_rep* event =
        EventFactory::instance()->createRmaGetStart(defs,
                                                    table.mapTimestamp(time),
                                                    rmaid,
                                                    tlid,
                                                    nbytes);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_GET_1TE(elg_ui4 lid,    /* location identifier */
                   elg_d8  time,   /* time stamp */
                   elg_ui4 slid,   /* source location identifier */
                   elg_ui4 rmaid,  /* RMA operation identifier */
                   void*   userdata)
{
    /* NOTE: In pearl most of the data provided in the event can be
     *       obtained with a reference to the Start event, using the
     *       rma_id. */
    CALLBACK_SETUP

    Event_rep* event =
        EventFactory::instance()->createRmaGetEnd(defs,
                                                  table.mapTimestamp(time),
                                                  rmaid);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_COLLEXIT(elg_ui4  lid,     /* location identifier */
                    elg_d8   time,    /* time stamp */
                    elg_ui1  metc,    /* number of metrics */
                    elg_ui8* metv,    /* metric values */
                    elg_ui4  rlid,    /* root location identifier */
                    elg_ui4  cid,     /* communicator identifier */
                    elg_ui4  sent,    /* bytes sent */
                    elg_ui4  recvd,   /* bytes received */
                    void*    userdata)
{
    /*
     * One-sided communication is currently ignored. However, EXIT events
     * need to be handled correctly.
     */

    CALLBACK_SETUP

    // Consistency check and callstack update
    if (data->mCallstack.empty())
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many LEAVEs).");
    }
    uint32_t regionId = data->mCallstack.top();
    data->mCallstack.pop();

    // Create event
    Event_rep* event =
        EventFactory::instance()->createLeave(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_ALOCK(elg_ui4 lid,    /* location identifier */
                 elg_d8  time,   /* time stamp */
                 elg_ui4 lkid,   /* identifier of the lock being aquired */
                 void*   userdata)
{
    /* One-sided communication is currently ignored. */
}


void
elg_readcb_RLOCK(elg_ui4 lid,    /* location identifier */
                 elg_d8  time,   /* time stamp */
                 elg_ui4 lkid,   /* identifier of the lock being aquired */
                 void*   userdata)
{
    /* One-sided communication is currently ignored. */
}


void
elg_readcb_OMP_FORK(elg_ui4 lid,    /* location identifier */
                    elg_d8  time,   /* time stamp */
                    void*   userdata)
{
    CALLBACK_SETUP

    uint32_t   teamSize = defs.getLocation(data->mLocation).getParent()->numLocations();
    Event_rep* event    =
        EventFactory::instance()->createThreadFork(defs,
                                                   table.mapTimestamp(time),
                                                   teamSize,
                                                   Paradigm::OPENMP);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_OMP_JOIN(elg_ui4 lid,    /* location identifier */
                    elg_d8  time,   /* time stamp */
                    void*   userdata)
{
    CALLBACK_SETUP

    Event_rep* event =
        EventFactory::instance()->createThreadJoin(defs,
                                                   table.mapTimestamp(time),
                                                   Paradigm::OPENMP);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_OMP_ALOCK(elg_ui4 lid,    /* location identifier */
                     elg_d8  time,   /* time stamp */
                     elg_ui4 lkid,   /* identifier of the lock being acquired */
                     void*   userdata)
{
    // EPIK did not track the lock acquisition order.  Without this
    // information, the lock analysis will deadlock.  Thus, discard
    // the corresponding lock events.
}


void
elg_readcb_OMP_RLOCK(elg_ui4 lid,    /* location identifier */
                     elg_d8  time,   /* time stamp */
                     elg_ui4 lkid,   /* identifier of the lock being acquired */
                     void*   userdata)
{
    // EPIK did not track the lock acquisition order.  Without this
    // information, the lock analysis will deadlock.  Thus, discard
    // the corresponding lock events.
}


void
elg_readcb_OMP_COLLEXIT(elg_ui4  lid,     /* location identifier */
                        elg_d8   time,    /* time stamp */
                        elg_ui1  metc,    /* number of metrics */
                        elg_ui8* metv,    /* metric values */
                        void*    userdata)
{
    CALLBACK_SETUP

    // Consistency check and callstack update
    if (data->mCallstack.empty())
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many LEAVEs).");
    }
    uint32_t regionId = data->mCallstack.top();
    data->mCallstack.pop();

    // Create event
    Event_rep* event =
        EventFactory::instance()->createLeave(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }

    const Region& region = defs.getRegion(regionId);
    if (is_omp_parallel(region))
    {
        const uint32_t rank = trace.get_location().getRank();
        event =
            EventFactory::instance()->createThreadTeamEnd(defs,
                                                          table.mapTimestamp(time),
                                                          data->mArchive.getThreadTeamId(rank));
        if (event)
        {
            trace.add_event(event);
        }
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_LOG_OFF(elg_ui4  lid,     /* location identifier */
                   elg_d8   time,    /* time stamp */
                   elg_ui1  metc,    /* number of metrics */
                   elg_ui8* metv,    /* metric values */
                   void*    userdata)
{
    CALLBACK_SETUP

    // Callstack update
    uint32_t regionId = defs.getPausingRegion().getId();
    data->mCallstack.push(regionId);

    Event_rep* event =
        EventFactory::instance()->createEnter(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_LOG_ON(elg_ui4  lid,     /* location identifier */
                  elg_d8   time,    /* time stamp */
                  elg_ui1  metc,    /* number of metrics */
                  elg_ui8* metv,    /* metric values */
                  void*    userdata)
{
    CALLBACK_SETUP

    // Consistency check and callstack update
    if (data->mCallstack.empty())
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many LEAVEs).");
    }
    uint32_t regionId = data->mCallstack.top();
    data->mCallstack.pop();

    // Create event
    Event_rep* event =
        EventFactory::instance()->createLeave(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_ENTER_TRACING(elg_ui4  lid,     /* location identifier */
                         elg_d8   time,    /* time stamp */
                         elg_ui1  metc,    /* number of metrics */
                         elg_ui8* metv,    /* metric values */
                         void*    userdata)
{
    CALLBACK_SETUP

    // Callstack update
    uint32_t regionId = defs.getFlushingRegion().getId();
    data->mCallstack.push(regionId);

    Event_rep* event =
        EventFactory::instance()->createEnter(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_EXIT_TRACING(elg_ui4  lid,     /* location identifier */
                        elg_d8   time,    /* time stamp */
                        elg_ui1  metc,    /* number of metrics */
                        elg_ui8* metv,    /* metric values */
                        void*    userdata)
{
    CALLBACK_SETUP

    // Consistency check and callstack update
    if (data->mCallstack.empty())
    {
        throw FatalError("Unbalanced ENTER/LEAVE events (Too many LEAVEs).");
    }
    uint32_t regionId = data->mCallstack.top();
    data->mCallstack.pop();

    // Create event
    Event_rep* event =
        EventFactory::instance()->createLeave(defs,
                                              table.mapTimestamp(time),
                                              regionId,
                                              metv);

    if (event)
    {
        trace.add_event(event);
    }
    CALLBACK_CLEANUP
}
