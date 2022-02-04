/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2019-2020                                                **
**  RWTH Aachen University, IT Center                                      **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "MpiCommunicationHandler.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <map>
#include <vector>

#define SCALASCA_DEBUG_MODULE_NAME    SCOUT
#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <mpi.h>

#include <pearl/AmRuntime.h>
#include <pearl/CallbackManager.h>
#include <pearl/CommSet.h>
#include <pearl/Event.h>
#include <pearl/EventSet.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalData.h>
#include <pearl/MpiCollEnd_rep.h>
#include <pearl/MpiComm.h>
#include <pearl/MpiGroup.h>
#include <pearl/MpiMessage.h>
#include <pearl/MpiWindow.h>
#include <pearl/Region.h>
#include <pearl/RemoteData.h>
#include <pearl/RemoteEventSet.h>

#include "Callstack.h"
#include "CbData.h"
#include "Roles.h"
#include "SynchpointHandler.h"
#include "scout_types.h"
#include "user_events.h"

using namespace scout;
using namespace pearl;
using namespace std;


namespace scout
{
namespace detail
{
static uint8_t
numBitsSet(uint8_t byte)
{
    static bool    is_initialized = false;
    static uint8_t bitSetTable[256];

    if (!is_initialized)
    {
        bitSetTable[0] = 0;
        for (int i = 0; i < 256; i++)
        {
            bitSetTable[i] = (i & 1) + bitSetTable[i / 2];
        }
        is_initialized = true;
    }

    return bitSetTable[byte];
}
}    // namespace scout::detail
}    // namespace scout


struct MpiCommunicationHandler::MpiCHImpl
{
    //
    // --- private data --------------------------------------------------
    //

    typedef map< Event, CollectiveInfo, EventKeyCompare > collinfo_map_t;
    typedef map< Communicator::IdType, MpiComm* >         communicator_map_t;

    collinfo_map_t mCollInfo;

    vector< MpiMessage* > mPendingMsgs;    ///< Pending message objects (main replay)
    vector< MPI_Request > mPendingReqs;    ///< Pending MPI requests (main replay)
    vector< MPI_Status >  mStatuses;
    vector< int >         mIndices;

    communicator_map_t mInvComms;

    pearl::Event mInitEnd;
    pearl::Event mFinalizeEnd;

    // Offsets into RMA analysis window
    static const int LOCAL_POST     = 0;
    static const int LAST_POST      = 1 * sizeof(TimeRank);
    static const int LAST_COMPLETE  = 2 * sizeof(TimeRank);
    static const int LAST_RMA_OP    = 3 * sizeof(TimeRank);
    static const int BITFIELD_START = 4 * sizeof(TimeRank);

    static const TimeRank UNDEFINED_TIME_RANK;

    typedef map< uint32_t, pearl::timestamp_t > TimeMap;
    typedef map< uint32_t, TimeRank >           TimeRankMap;
    typedef struct
    {
        uint8_t*           mBuffer;
        uint8_t            mRankMask;
        MPI_Aint           mMaskOffset;
        size_t             mBufferSize;
        size_t             mBitfieldSize;
        pearl::timestamp_t mCompleteEnterTime;
        MPI_Win            mWin;
        TimeRankMap        mLastOpTime;
        TimeMap            mPostTime;
        SynchpointInfo     mLastPostSyncPoint;
        #ifdef PEARL_ENABLE_AM
            MpiNonBlockingBarrier* mBarrier;
        #endif
        set< uint32_t > mAccessProcesses;
    } WindowInfo;
    typedef std::map< RmaWindow*, WindowInfo > WindowInfoMap;

    WindowInfoMap mWindows;

    ~MpiCHImpl()
    {
        for (communicator_map_t::iterator it = mInvComms.begin();
             it != mInvComms.end();
             ++it)
        {
            delete it->second;
        }
    }

    //
    // --- helper functions ----------------------------------------------
    //

    void
    processPendingMsgs()
    {
        // No pending requests? ==> continue
        if (mPendingReqs.empty())
        {
            return;
        }

        // --- Check for completed messages

        int completed;
        int count = mPendingReqs.size();

        mIndices.resize(count);
        mStatuses.resize(count);

        MPI_Testsome(count, &mPendingReqs[0], &completed, &mIndices[0], &mStatuses[0]);


        // --- Update array of pending messages

        for (int i = 0; i < completed; ++i)
        {
            int index = mIndices[i];

            delete mPendingMsgs[index];
            mPendingMsgs[index] = 0;
        }

        if (completed > 0)
        {
            mPendingMsgs.erase(remove(mPendingMsgs.begin(), mPendingMsgs.end(),
                                      static_cast< MpiMessage* >(0)),
                               mPendingMsgs.end());
            mPendingReqs.erase(remove(mPendingReqs.begin(), mPendingReqs.end(),
                                      static_cast< MPI_Request >(MPI_REQUEST_NULL)),
                               mPendingReqs.end());
        }
    }

    void
    processPendingActiveMsgs(RmaWindow* window)
    {
        #if defined(PEARL_ENABLE_AM)
            WindowInfo& winInfo = mWindows[window];
            AmRuntime&  runtime = AmRuntime::getInstance();
            MpiComm*    comm    = dynamic_cast< MpiComm* >(window->get_comm());

            unsigned int global_pending = 0;

            do
            {
                // complete all local requests
                runtime.fence(*comm);

                // wait until all processes completed local requests
                winInfo.mBarrier->start();
                while (!winInfo.mBarrier->test())
                {
                    runtime.advance();
                }

                // check if response requests were scheduled anywhere
                unsigned int local_pending = runtime.num_pending(*comm);
                MPI_Allreduce(&local_pending, &global_pending, 1, MPI_UNSIGNED, MPI_SUM, comm->get_comm());
            }
            while (global_pending > 0);
        #endif
    }

    void
    setRemoteAccessBit(const pearl::Event& event,
                       CbData*             data)
    {
        RmaWindow* window = event->get_window();

        WindowInfo& winInfo = mWindows[window];

        /* set bit corresponding to my rank on every remote location */
        for (set< uint32_t >::iterator it = winInfo.mAccessProcesses.begin();
             it != winInfo.mAccessProcesses.end();
             ++it)
        {
            MPI_Accumulate(&winInfo.mRankMask, 1, SCALASCA_MPI_UINT8_T,
                           static_cast< int >(*it), winInfo.mMaskOffset, 1,
                           SCALASCA_MPI_UINT8_T, MPI_BOR, winInfo.mWin);
        }
    }

    void
    setRemoteLastOp(const pearl::Event& event,
                    CbData*             data)
    {
        RmaWindow* window = event->get_window();
        MpiComm*   comm   = dynamic_cast< MpiComm* >(window->get_comm());
        assert(comm);

        WindowInfo& winInfo = mWindows[window];

        /* send last rma op times to remote processes */
        TimeRankMap::iterator it = winInfo.mLastOpTime.begin();
        while (it != winInfo.mLastOpTime.end())
        {
            MPI_Accumulate(&(it->second), 1, MPI_DOUBLE_INT,
                           it->first, LAST_RMA_OP,
                           1, MPI_DOUBLE_INT, MPI_MAXLOC, winInfo.mWin);
            ++it;
        }
    }

    //
    // --- main replay callbacks -----------------------------------------
    //

    SCOUT_CALLBACK(cb_finished)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        processPendingMsgs();

        // Handle remaining messages
        if (!mPendingReqs.empty())
        {
            int count = mPendingReqs.size();

            // Print warning
            UTILS_WARNING("Encountered %d unreceived send operations!", count);

            // Cancel pending messages
            for (int i = 0; i < count; ++i)
            {
                MPI_Cancel(&mPendingReqs[i]);
                mPendingMsgs[i]->wait();
                delete mPendingMsgs[i];
            }
        }

        mPendingMsgs.clear();
        mPendingReqs.clear();
    }

    //
    // --- init/finalize
    //

    SCOUT_CALLBACK(cb_pre_leave_cache)
    {
        const Region& region = event->getRegion();
        if (  !mInitEnd.is_valid()
           && is_mpi_init(region))
        {
            mInitEnd = event;
        }
        else if (  !mFinalizeEnd.is_valid()
                && is_mpi_finalize(region))
        {
            mFinalizeEnd = event;
        }
    }

    SCOUT_CALLBACK(cb_pre_leave)
    {
        if (event == mInitEnd)
        {
            cbmanager.notify(PRE_INIT, event, cdata);
            cbmanager.notify(INIT, event, cdata);
        }
        else if (event == mFinalizeEnd)
        {
            cbmanager.notify(PRE_FINALIZE, event, cdata);
            cbmanager.notify(FINALIZE, event, cdata);
        }
    }

    /// @todo Use communicator handle from GlobalDefs
    SCOUT_CALLBACK(cb_pre_init_finalize)
    {
        CbData* const data = static_cast< CbData* >(cdata);

        UTILS_DLOG_LEVEL(SCALASCA_DEBUG_TRIGGER) << event;

        const Event enter = data->mCallstack->top();

        data->mLocal->add_event(enter, ROLE_ENTER_COLL);
        data->mLocal->add_event(event, ROLE_LEAVE_COLL);

        CollectiveInfo& ci = data->mCollinfo;

        ci.my.mTime = enter->getTimestamp();
        ci.my.mRank = event.get_location().getRank();

        // All ranks are synchronizing at Init/Finalize
        data->mIsSyncpoint = true;

        // latest ENTER
        TimeRank sendBuf = ci.my;
        MPI_Allreduce(&sendBuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC,
                      MPI_COMM_WORLD);

        // earliest LEAVE
        sendBuf.mTime = event->getTimestamp();
        MPI_Allreduce(&sendBuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC,
                      MPI_COMM_WORLD);

        UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
            << "latest ENTER: " << ci.latest
            << ", earliest LEAVE: " << ci.earliest_end
            << " (full overlap)";

        mCollInfo.insert(make_pair(event, ci));
    }

    //
    // --- point-to-point communication
    //

    SCOUT_CALLBACK(cb_process_msgs)
    {
        processPendingMsgs();
    }

    SCOUT_CALLBACK(cb_pre_send)
    {
        CbData* data = static_cast< CbData* >(cdata);

        if (  !is_mpi_api(data->mCallstack->top()->getRegion())
           || event.completion()->isOfType(MPI_CANCELLED))
        {
            return;
        }

        // --- run pre-send callbacks

        cbmanager.notify(PRE_SEND, event, data);

        // --- transfer message

        MpiComm*    comm = event->getComm();
        MpiMessage* msg;

        msg = data->mLocal->isend(*comm, event->getDestination(), event->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());

        // --- run post-send callbacks

        cbmanager.notify(POST_SEND, event, data);
    }

    SCOUT_CALLBACK(cb_pre_recv)
    {
        CbData* data = static_cast< CbData* >(cdata);

        if (!is_mpi_api(data->mCallstack->top()->getRegion()))
        {
            return;
        }

        // --- run pre-recv callbacks

        cbmanager.notify(PRE_RECV, event, data);

        // --- receive message

        MpiComm* comm = event->getComm();
        data->mRemote->recv(*data->mDefs, *comm, event->getSource(), event->getTag());

        // --- run post-recv callbacks

        cbmanager.notify(POST_RECV, event, data);
    }

    //
    // --- mpi collective communication
    //

    // --- collective communication dispatch

    /// @todo Avoid calling getLocalRank() on every invocation; it's \f$O(n)\f$
    SCOUT_CALLBACK(cb_pre_mpi_collend)
    {
        CbData* const data = static_cast< CbData* >(cdata);

        const MpiCollEnd_rep& endRep = event_cast< MpiCollEnd_rep >(*event);

        // Collectives on self-like communicators do not synchronize
        const MpiComm* const comm = endRep.getComm();
        if (comm->getSize() == 1)
        {
            return;
        }

        // MPI_Alltoallv and MPI_Alltoallw can be used to implement arbitrary
        // communication patterns (e.g., pair-wise message exchange).  However,
        // the trace data currently does not provide enough information for a
        // proper reconstruction, thus, ignore those calls.
        const MpiCollEnd_rep::coll_type collType = endRep.getCollType();
        if (  (collType == MpiCollEnd_rep::ALLTOALLV)
           || (collType == MpiCollEnd_rep::ALLTOALLW))
        {
            return;
        }

        UTILS_DLOG_LEVEL(SCALASCA_DEBUG_TRIGGER) << event;

        const Event enter = data->mCallstack->top();
        const Event begin = event.beginptr();
        const Event leave = event.leaveptr();

        data->mLocal->add_event(enter, ROLE_ENTER_COLL);
        data->mLocal->add_event(begin, ROLE_BEGIN_COLL);
        data->mLocal->add_event(event, ROLE_END_COLL);
        data->mLocal->add_event(leave, ROLE_LEAVE_COLL);

        CollectiveInfo& ci = data->mCollinfo;

        ci.my.mTime = begin->getTimestamp();
        ci.my.mRank = comm->getCommSet().getLocalRank(event.get_location().getRank());

        // Dispatch
        switch (collType)
        {
            // CASE 1: Barrier
            case MpiCollEnd_rep::BARRIER:
                cbmanager.notify(PRE_SYNC_COLL, event, data);
                cbmanager.notify(SYNC_COLL, event, data);
                break;

            // CASE 2: 1-to-N communication
            case MpiCollEnd_rep::BCAST:
            case MpiCollEnd_rep::SCATTER:
            case MpiCollEnd_rep::SCATTERV:
                // FIXME: Refactor setup code before switch into support
                //        function that can be called right before notify
                {
                    uint64_t bytesSent = event->getBytesSent();
                    MPI_Bcast(&bytesSent, 1, SCALASCA_MPI_UINT64_T,
                              event->getRoot(), comm->getHandle());

                    // No payload -> no syncpoint -> no waitstate
                    if (bytesSent == 0)
                    {
                        return;
                    }

                    cbmanager.notify(PRE_COLL_12N, event, data);
                    cbmanager.notify(COLL_12N, event, data);
                }
                break;

            // CASE 3: N-to-N communication w/o MPI_Alltoallv & MPI_Alltoallw
            case MpiCollEnd_rep::ALLGATHER:
            case MpiCollEnd_rep::ALLGATHERV:
            case MpiCollEnd_rep::ALLTOALL:
            case MpiCollEnd_rep::ALLREDUCE:
            case MpiCollEnd_rep::REDUCE_SCATTER:
            case MpiCollEnd_rep::REDUCE_SCATTER_BLOCK:
                // FIXME: Refactor setup code before switch into support
                //        function that can be called right before notify
                {
                    const uint64_t bytesSent     = event->getBytesSent();
                    const uint64_t bytesReceived = event->getBytesReceived();

                    // No payload -> no syncpoint -> no waitstate
                    if ((bytesSent == 0) && (bytesReceived == 0))
                    {
                        return;
                    }

                    cbmanager.notify(PRE_COLL_N2N, event, data);
                    cbmanager.notify(COLL_N2N, event, data);
                }
                break;

            // CASE 4: N-to-1 communication
            case MpiCollEnd_rep::GATHER:
            case MpiCollEnd_rep::GATHERV:
            case MpiCollEnd_rep::REDUCE:
                // FIXME: Refactor setup code before switch into support
                //        function that can be called right before notify
                {
                    uint64_t bytesReceived = event->getBytesReceived();
                    MPI_Bcast(&bytesReceived, 1, SCALASCA_MPI_UINT64_T,
                              event->getRoot(), comm->getHandle());

                    // No payload -> no syncpoint -> no waitstate
                    if (bytesReceived == 0)
                    {
                        return;
                    }

                    cbmanager.notify(PRE_COLL_N21, event, data);
                    cbmanager.notify(COLL_N21, event, data);
                }
                break;

            // CASE 5: Prefix reduction
            case MpiCollEnd_rep::SCAN:
            case MpiCollEnd_rep::EXSCAN:
                // FIXME: Refactor setup code before switch into support
                //        function that can be called right before notify
                {
                    const uint64_t bytesSent     = event->getBytesSent();
                    const uint64_t bytesReceived = event->getBytesReceived();

                    // No payload -> no syncpoint -> no waitstate
                    if (  (bytesSent == 0)
                       && (bytesReceived == 0))
                    {
                        return;
                    }

                    cbmanager.notify(PRE_COLL_SCAN, event, data);
                    cbmanager.notify(COLL_SCAN, event, data);
                }
                break;

            default:
                UTILS_BUG("Unknown MPI collective operation encountered");
                break;
        }

        mCollInfo.insert(make_pair(event, data->mCollinfo));
    }

    // --- mpi collective operation handlers

    SCOUT_CALLBACK(cb_pre_coll_12n)
    {
        CbData* const        data = static_cast< CbData* >(cdata);
        const MpiComm* const comm = event->getComm();

        CollectiveInfo& ci = data->mCollinfo;

        // Distribute root BEGIN
        ci.root = ci.my;
        MPI_Bcast(&ci.root, 1, MPI_DOUBLE_INT, event->getRoot(),
                  comm->getHandle());

        // Earliest END of receivers waiting for root
        const uint64_t bytesReceived = event->getBytesReceived();
        const bool     waitsForRoot  = (  (bytesReceived > 0)
                                       && (ci.my.mTime < ci.root.mTime));

        TimeRank sendBuf(event->getTimestamp(), ci.my.mRank);
        if (!waitsForRoot)
        {
            sendBuf.mTime = numeric_limits< double >::max();
        }
        MPI_Allreduce(&sendBuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC,
                      comm->getHandle());

        data->mIsSyncpoint = (  waitsForRoot
                             || (  (ci.root.mRank == ci.my.mRank)
                                && (ci.earliest_end.mTime < numeric_limits< double >::max())));

        if (data->mIsSyncpoint)
        {
            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << "root BEGIN: " << globalize(ci.root, *comm)
                << ", earliest sync END: " << globalize(ci.earliest_end, *comm);
        }
    }

    /// @todo [C++11] Replace `-max()` by `lowest()`
    SCOUT_CALLBACK(cb_pre_coll_n21)
    {
        CbData* const        data = static_cast< CbData* >(cdata);
        const MpiComm* const comm = event->getComm();

        CollectiveInfo& ci = data->mCollinfo;

        // Distribute root BEGIN
        ci.root = ci.my;
        MPI_Bcast(&ci.root, 1, MPI_DOUBLE_INT, event->getRoot(),
                  comm->getHandle());

        // Latest BEGIN of all senders
        const uint64_t bytesSent = event->getBytesSent();

        TimeRank sendBuf = ci.my;
        if (bytesSent == 0)
        {
            sendBuf.mTime = -numeric_limits< double >::max();
        }
        MPI_Allreduce(&sendBuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC,
                      comm->getHandle());

        // The latest sender and the root rank are considered synchronizing
        // if the root enters the operation earlier than the sender.
        data->mIsSyncpoint = (  (ci.root.mTime < ci.latest.mTime)
                             && (  (ci.my.mRank == ci.root.mRank)
                                || (ci.my.mRank == ci.latest.mRank)));

        // Earliest END of synchronizing ranks (i.e., root and latest sender)
        //
        // Although only two ranks are involved and interested in the result,
        // a collective operation over all involved ranks is used here.  A
        // point-to-point transfer (e.g., via MPI_Sendrecv) may interfere
        // with other non-blocking point-to-point message transfers currently
        // in flight unless a separate communicator or unique tag (which can
        // not be guaranteed) is used.
        sendBuf.mTime = numeric_limits< double >::max();
        if (data->mIsSyncpoint)
        {
            sendBuf.mTime = event->getTimestamp();
        }
        MPI_Allreduce(&sendBuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC,
                      comm->getHandle());

        if (data->mIsSyncpoint)
        {
            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << "root BEGIN: " << globalize(ci.root, *comm)
                << ", latest sync BEGIN: " << globalize(ci.latest, *comm)
                << ", earliest sync END: " << globalize(ci.earliest_end, *comm);
        }
    }

    /// @todo [C++11] Replace `-max()` by `lowest()`
    SCOUT_CALLBACK(cb_pre_coll_scan)
    {
        CbData* const        data = static_cast< CbData* >(cdata);
        const MpiComm* const comm = event->getComm();

        CollectiveInfo& ci = data->mCollinfo;

        // Latest BEGIN of all senders < own rank
        const uint64_t bytesSent = event->getBytesSent();

        TimeRank sendBuf = ci.my;
        if (bytesSent == 0)
        {
            sendBuf.mTime = -numeric_limits< double >::max();
        }
        MPI_Scan(&sendBuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC,
                 comm->getHandle());

        // FIXME: Currently only covers the receiver side
        data->mIsSyncpoint = (ci.my.mTime < ci.latest.mTime);
    }

    /// @todo [C++11] Replace `-max()` by `lowest()`
    SCOUT_CALLBACK(cb_pre_coll_n2n)
    {
        CbData* const        data = static_cast< CbData* >(cdata);
        const MpiComm* const comm = event->getComm();

        CollectiveInfo& ci = data->mCollinfo;

        // --- Step 1: check for full overlap

        // Latest BEGIN of all ranks
        TimeRank sendBuf = ci.my;
        MPI_Allreduce(&sendBuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC,
                      comm->getHandle());


        // Earliest END of all ranks
        sendBuf.mTime = event->getTimestamp();
        MPI_Allreduce(&sendBuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC,
                      comm->getHandle());

        if (ci.earliest_end.mTime > ci.latest.mTime)
        {
            // With full overlap, all ranks are considered to be synchronizing
            data->mIsSyncpoint = true;

            UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
                << "latest BEGIN: " << globalize(ci.latest, *comm)
                << ", earliest END: " << globalize(ci.earliest_end, *comm)
                << " (full overlap)";

            return;
        }

        // --- Step 2: otherwise, handle partial overlap

        const uint64_t bytesSent     = event->getBytesSent();
        const uint64_t bytesReceived = event->getBytesReceived();

        // Latest BEGIN of all senders
        sendBuf.mTime = ci.my.mTime;
        if (bytesSent == 0)
        {
            sendBuf.mTime = -numeric_limits< double >::max();
        }
        MPI_Allreduce(&sendBuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC,
                      comm->getHandle());

        // With partial overlap, only receivers overlapping with the latest
        // sender and the latest sender itself are considered synchronizing
        data->mIsSyncpoint = (  (  (bytesReceived > 0)
                                && (ci.my.mTime < ci.latest.mTime))
                             || (  (bytesSent > 0)
                                && (ci.my.mRank == ci.latest.mRank)));

        // Earliest END of synchronizing ranks
        sendBuf.mTime = event->getTimestamp();
        if (!data->mIsSyncpoint)
        {
            sendBuf.mTime = numeric_limits< double >::max();
        }
        MPI_Allreduce(&sendBuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC,
                      comm->getHandle());

        UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
            << "latest send BEGIN: " << globalize(ci.latest, *comm)
            << ", earliest sync END: " << globalize(ci.earliest_end, *comm)
            << " (partial overlap)";
    }

    SCOUT_CALLBACK(cb_pre_sync_coll)
    {
        CbData* const        data = static_cast< CbData* >(cdata);
        const MpiComm* const comm = event->getComm();

        CollectiveInfo& ci = data->mCollinfo;

        // All ranks are synchronizing at a Barrier
        data->mIsSyncpoint = true;

        // latest BEGIN
        TimeRank sendBuf = ci.my;
        MPI_Allreduce(&sendBuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC,
                      comm->getHandle());

        // earliest END
        sendBuf.mTime = event->getTimestamp();
        MPI_Allreduce(&sendBuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC,
                      comm->getHandle());

        UTILS_DLOG_LEVEL(SCALASCA_DEBUG_WAITSTATE)
            << "latest BEGIN: " << globalize(ci.latest, *comm)
            << ", earliest END: " << globalize(ci.earliest_end, *comm)
            << " (full overlap)";
    }

    // --- rma collective operation handlers

    SCOUT_CALLBACK(cb_pre_rma_collend)
    {
        CbData*       data   = static_cast< CbData* >(cdata);
        RmaWindow*    window = event->get_window();
        Communicator* comm   = window->get_comm();
        assert(comm);

        // A single process will not wait for itself
        if (comm->getSize() < 2)
        {
            return;
        }
        Event begin = event.beginptr();
        Event leave = event.leaveptr();
        Event enter = data->mCallstack->top();

        data->mLocal->add_event(enter, ROLE_ENTER_COLL);
        data->mLocal->add_event(begin, ROLE_BEGIN_COLL);
        data->mLocal->add_event(event, ROLE_END_COLL);
        data->mLocal->add_event(leave, ROLE_LEAVE_COLL);

        CollectiveInfo ci;

        ci.my.mTime = begin->getTimestamp();
        ci.my.mRank = comm->getCommSet().getLocalRank(event.get_location().getRank());

        data->mCollinfo = ci;

        // Compute potential waiting time
        cbmanager.notify(RMA_SYNC_COLLECTIVE, event, data);

        // Dispatch
        const Region& region = enter->getRegion();
        if (is_mpi_rma_fence(region))
        {
            cbmanager.notify(MPI_RMA_WIN_FENCE, event, data);
        }
        else if (is_mpi_rma_create(region))
        {
            cbmanager.notify(MPI_RMA_WIN_CREATE, event, data);
        }
        else if (is_mpi_rma_free(region))
        {
            cbmanager.notify(MPI_RMA_WIN_FREE, event, data);
        }

        #if defined(PEARL_ENABLE_AM)
            if (enableAsynchronous)
            {
                processPendingActiveMsgs(window);
            }
        #endif

        mCollInfo.insert(make_pair(event, data->mCollinfo));
    }

    SCOUT_CALLBACK(cb_pre_rma_sync_coll)
    {
        CbData*  data = static_cast< CbData* >(cdata);
        MpiComm* comm = dynamic_cast< MpiComm* >(event->get_window()->get_comm());
        assert(comm);

        CollectiveInfo& ci(data->mCollinfo);

        TimeRank trbuf;

        // latest BEGIN
        trbuf = ci.my;

        MPI_Allreduce(&trbuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC, comm->getHandle());

        // earliest END
        trbuf.mTime = event->getTimestamp();
        trbuf.mRank = ci.my.mRank;

        MPI_Allreduce(&trbuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC, comm->getHandle());
    }

    SCOUT_CALLBACK(cb_pre_rma_win_create)
    {
        CbData*    data   = static_cast< CbData* >(cdata);
        RmaWindow* window = event->get_window();
        MpiComm*   comm   = dynamic_cast< MpiComm* >(window->get_comm());
        assert(comm);

        WindowInfo& winInfo = mWindows[window];

        // allocate & initialize window buffer
        #if !defined(SCOUT_DISABLE_RMA_ACCESS_TRACKING)
            const int comm_size = comm->getSize();
            winInfo.mBitfieldSize = ((comm_size % 8) ? 1 : 0) + (comm_size / 8);
        #else
            winInfo.mBitfieldSize = 0;
        #endif
        winInfo.mBufferSize = BITFIELD_START + winInfo.mBitfieldSize;
        winInfo.mBuffer     = new uint8_t[winInfo.mBufferSize];
        assert(winInfo.mBuffer);

        // initialize TimeRank part of buffer
        TimeRank* timerank = reinterpret_cast< TimeRank* >(winInfo.mBuffer);
        assert(timerank);

        timerank[LAST_COMPLETE / sizeof(TimeRank)] = UNDEFINED_TIME_RANK;
        timerank[LAST_RMA_OP   / sizeof(TimeRank)] = UNDEFINED_TIME_RANK;
        timerank[LAST_POST     / sizeof(TimeRank)] = UNDEFINED_TIME_RANK;
        timerank[LOCAL_POST    / sizeof(TimeRank)] = TimeRank(event.enterptr()->getTimestamp(), -1);

        // setup access mask & offset
        int myrank = comm->getCommSet().getLocalRank(event.get_location().getRank());
        winInfo.mRankMask   = 1 << (myrank % 8);
        winInfo.mMaskOffset = BITFIELD_START + (myrank / 8);

        MPI_Win_create(winInfo.mBuffer, winInfo.mBufferSize, sizeof(uint8_t),
                       MPI_INFO_NULL, comm->getHandle(), &winInfo.mWin);

        #if defined(PEARL_ENABLE_AM)
            if (enableAsynchronous)
            {
                // initialize non-blocking barrier
                winInfo.mBarrier = new MpiNonBlockingBarrier(comm->get_comm());
                assert(winInfo.mBarrier);
                winInfo.mBarrier->init();
                #if defined(DEBUG_AM)
                    UTILS_DLOG << "Initialized non-blocking barrier on communicator "
                               << comm->get_id();
                #endif

                // start listening on window communicator
                AmRuntime::getInstance().start_listen(*window->get_comm());
                #if defined(DEBUG_AM)
                    UTILS_DLOG << "Listening for AM requests on communicator "
                               << window->get_comm()->get_id()
                               << " (" << window->get_comm() << ")"
                               << " of window " << window->get_id();
                #endif
            }
        #endif
    }

    SCOUT_CALLBACK(cb_pre_rma_win_fence)
    {
        CbData*    data   = static_cast< CbData* >(cdata);
        RmaWindow* window = event->get_window();
        MpiComm*   comm   = dynamic_cast< MpiComm* >(window->get_comm());
        assert(comm);

        WindowInfo& winInfo = mWindows[window];

        // initialize TimeRank part of buffer
        TimeRank* timerank = reinterpret_cast< TimeRank* >(winInfo.mBuffer);
        assert(timerank);

        timerank[LAST_COMPLETE / sizeof(TimeRank)] = UNDEFINED_TIME_RANK;
        timerank[LAST_RMA_OP   / sizeof(TimeRank)] = UNDEFINED_TIME_RANK;
        timerank[LAST_POST     / sizeof(TimeRank)] = UNDEFINED_TIME_RANK;
        timerank[LOCAL_POST    / sizeof(TimeRank)] = TimeRank(event.enterptr()->getTimestamp(), -1);

        #if !defined(SCOUT_DISABLE_RMA_ACCESS_TRACKING)
            memset(winInfo.mBuffer + BITFIELD_START, 0, winInfo.mBitfieldSize);
        #endif

        /* get latest rma op to this location */
        MPI_Win_fence(MPI_MODE_NOPRECEDE, winInfo.mWin);

        #if !defined(SCOUT_DISABLE_RMA_ACCESS_TRACKING)
            setRemoteAccessBit(event, data);
        #endif
        setRemoteLastOp(event, data);

        MPI_Win_fence(MPI_MODE_NOPUT | MPI_MODE_NOSUCCEED | MPI_MODE_NOSTORE, winInfo.mWin);

        // get last RMA Operation to local process
        data->mLastRmaOp.mTime = timerank[LAST_RMA_OP / sizeof(TimeRank)].mTime;

        data->mCount = 0;
        #if !defined(SCOUT_DISABLE_RMA_ACCESS_TRACKING)
            /* add own accesses to access tracking bitfield */
            set< uint32_t >::iterator target_rank = winInfo.mAccessProcesses.begin();
            while (target_rank != winInfo.mAccessProcesses.end())
            {
                winInfo.mBuffer[BITFIELD_START + (*target_rank / 8)] |= *target_rank % 8;
                ++target_rank;
            }
            winInfo.mAccessProcesses.clear();

            /* calculate number of accesses */
            for (size_t i = BITFIELD_START;
                 i < (BITFIELD_START + winInfo.mBitfieldSize);
                 ++i)
            {
                data->mCount += detail::numBitsSet(winInfo.mBuffer[i]);
            }
        #endif
    }

    SCOUT_CALLBACK(cb_pre_rma_win_free)
    {
        CbData*    data   = static_cast< CbData* >(cdata);
        RmaWindow* window = event->get_window();
        MpiComm*   comm   = dynamic_cast< MpiComm* >(window->get_comm());
        assert(comm);

        WindowInfo& winInfo = mWindows[window];

        // free window for active target exchange
        MPI_Win_free(&winInfo.mWin);
        delete[] winInfo.mBuffer;

        #if defined(PEARL_ENABLE_AM)
            if (enableAsynchronous)
            {
                AmRuntime& runtime = AmRuntime::getInstance();

                // cleanup barrier
                winInfo.mBarrier->finalize();
                delete winInfo.mBarrier;
                #if defined(DEBUG_AM)
                    UTILS_DLOG << "Finalized non-blocking barrier on communicator "
                               << comm->get_id();
                #endif

                runtime.stop_listen(*window->get_comm());
            }
        #endif

        mWindows.erase(window);
    }

    // --- rma group synchronization  handlers -----------------------

    SCOUT_CALLBACK(cb_pre_mpi_win_start)
    {
        MpiWindow* window = dynamic_cast< MpiWindow* >(event->get_window());
        assert(window);

        MpiComm* comm = dynamic_cast< MpiComm* >(window->get_comm());
        assert(comm);

        MpiGroup* group = event->get_group();
        assert(group);

        WindowInfo& winInfo = mWindows[window];

        /* start access epoch to exchange timestamps */
        MPI_Win_start(group->getHandle(), 0, winInfo.mWin);

        /* get post enter-time from each exposure epoch */
        for (uint32_t rank = 0; rank < group->numRanks(); ++rank)
        {
            /* create entry for location */
            timestamp_t& postTime = winInfo.mPostTime[ comm->getCommSet().getLocalRank(group->getGlobalRank(rank)) ];

            MPI_Get(&(postTime), 1, MPI_DOUBLE,
                    comm->getCommSet().getLocalRank(group->getGlobalRank(rank)), LOCAL_POST,
                    1, MPI_DOUBLE, winInfo.mWin);
        }
    }

    SCOUT_CALLBACK(cb_pre_mpi_win_complete)
    {
        CbData* data = static_cast< CbData* >(cdata);

        MpiWindow* window = dynamic_cast< MpiWindow* >(event->get_window());
        assert(window);

        MpiComm* comm = dynamic_cast< MpiComm* >(window->get_comm());
        assert(comm);

        MpiGroup* group = event->get_group();
        assert(group);

        WindowInfo& winInfo = mWindows[window];

        /* compute complete timestamp and rank*/
        timestamp_t completeEnterTime = event.enterptr()->getTimestamp();
        int         myrank            = comm->getCommSet().getLocalRank(event.get_location().getRank());
        TimeRank    completeEnter;
        completeEnter.mTime = completeEnterTime;
        completeEnter.mRank = myrank;

        #if !defined(SCOUT_DISABLE_RMA_ACCESS_TRACKING)
            setRemoteAccessBit(event, data);
        #endif
        setRemoteLastOp(event, data);

        /* send complete timestamp and rank to remote processes */
        for (uint32_t rank = 0; rank < group->numRanks(); ++rank)
        {
            MPI_Accumulate(&completeEnter, 1, MPI_DOUBLE_INT,
                           comm->getCommSet().getLocalRank(group->getGlobalRank(rank)), LAST_COMPLETE,
                           1, MPI_DOUBLE_INT, MPI_MAXLOC, winInfo.mWin);
        }

        /* complete access epoch on window */
        MPI_Win_complete(winInfo.mWin);

        // search maximum post timestamp
        data->mLastPost.mTime = -numeric_limits< pearl::timestamp_t >::max();

        map< uint32_t, timestamp_t >::iterator it = winInfo.mPostTime.begin();
        while (it != winInfo.mPostTime.end())
        {
            if (data->mLastPost.mTime < it->second)
            {
                data->mLastPost.mTime = it->second;
                data->mLastPost.mRank = it->first;
            }
            ++it;
        }

        data->mCountTarget = winInfo.mAccessProcesses.size();

        // reset data structure
        winInfo.mLastOpTime.clear();
        winInfo.mPostTime.clear();
        winInfo.mAccessProcesses.clear();
    }

    SCOUT_CALLBACK(cb_pre_mpi_win_post)
    {
        CbData* data = static_cast< CbData* >(cdata);

        MpiWindow* window = dynamic_cast< MpiWindow* >(event->get_window());
        assert(window);

        MpiGroup* group = event->get_group();
        assert(group);

        WindowInfo& winInfo  = mWindows[window];
        TimeRank*   timerank = reinterpret_cast< TimeRank* >(winInfo.mBuffer);
        assert(timerank);

        /* initialise window buffer */
        timerank[LAST_COMPLETE / sizeof(TimeRank)] = UNDEFINED_TIME_RANK;
        timerank[LAST_RMA_OP   / sizeof(TimeRank)] = UNDEFINED_TIME_RANK;
        timerank[LAST_POST     / sizeof(TimeRank)] = UNDEFINED_TIME_RANK;
        timerank[LOCAL_POST    / sizeof(TimeRank)] = TimeRank(event.enterptr()->getTimestamp(), -1);

        /* clear bitfield */
        memset(winInfo.mBuffer + BITFIELD_START, 0, winInfo.mBitfieldSize);

        /* start exposure epoch to exchange timestamps and ranks */
        MPI_Win_post(group->getHandle(), 0, winInfo.mWin);
    }

    SCOUT_CALLBACK(cb_pre_mpi_win_wait)
    {
        CbData* data = static_cast< CbData* >(cdata);

        MpiWindow* window = dynamic_cast< MpiWindow* >(event->get_window());
        assert(window);

        WindowInfo& winInfo  = mWindows[window];
        TimeRank*   timerank = reinterpret_cast< TimeRank* >(winInfo.mBuffer);
        assert(timerank);

        MPI_Win_wait(winInfo.mWin);

        data->mCountOrigin = 0;
        #if !defined(SCOUT_DISABLE_RMA_ACCESS_TRACKING)
            // Count number of origin proccesses that accessed during exposure epoch
            for (size_t i = BITFIELD_START;
                 i < (BITFIELD_START + winInfo.mBitfieldSize);
                 ++i)
            {
                data->mCountOrigin += detail::numBitsSet(winInfo.mBuffer[i]);
            }
        #endif

        /* Set TimeRank structures for time-based GATS patterns */
        data->mLastComplete = timerank[LAST_COMPLETE / sizeof(TimeRank)];
        data->mLastRmaOp    = timerank[LAST_RMA_OP   / sizeof(TimeRank)];
    }

    SCOUT_CALLBACK(cb_pre_rma_group_sync)
    {
        // --- dispatch to specific handlers

        const Region& region = event.enterptr()->getRegion();

        if (is_mpi_rma_start(region))
        {
            cbmanager.notify(MPI_RMA_WIN_START, event, cdata);
        }
        else if (is_mpi_rma_complete(region))
        {
            cbmanager.notify(MPI_RMA_WIN_COMPLETE, event, cdata);
        }
        else if (is_mpi_rma_post(region))
        {
            cbmanager.notify(MPI_RMA_WIN_POST, event, cdata);
        }
        else if (is_mpi_rma_wait(region))
        {
            cbmanager.notify(MPI_RMA_WIN_WAIT, event, cdata);
        }
    }

    // --- rma operations handlers ---------------------------------------

    SCOUT_CALLBACK(cb_pre_rma_op_start)
    {
        Event         leave  = event.leaveptr();
        RmaWindow*    window = event->get_window();
        Communicator* comm   = window->get_comm();

        int myrank = comm->getCommSet().getLocalRank(event.get_location().getRank());

        WindowInfo& winInfo = mWindows[window];

        TimeRank timerank(leave->getTimestamp(), myrank);
        winInfo.mLastOpTime[event->get_remote()] = timerank;

        winInfo.mAccessProcesses.insert(event->get_remote());
    }

    //
    // --- backward wait-state / synchpoint detection (bws) callbacks ----
    //

    SCOUT_CALLBACK(cb_progress_barrier)
    {
        // In p2p-only replays (bws, fws, fwc), add artificial barriers
        // at collective events to prevent p2p overload
        MPI_Barrier(event->getComm()->getHandle());
    }

    SCOUT_CALLBACK(cb_bw_pre_send)
    {
        // backward communication: receive message from original sender

        CbData* data = static_cast< CbData* >(cdata);

        if (  !is_mpi_api(event.get_cnode()->getRegion())
           || event.completion()->isOfType(MPI_CANCELLED))
        {
            return;
        }

        // --- run pre-recv callbacks

        cbmanager.notify(PRE_SEND, event, data);

        // --- receive message

        MpiComm* comm = event->getComm();
        data->mRemote->recv(*data->mDefs, *comm, event->getDestination(), event->getTag());

        // --- run post-recv callbacks

        cbmanager.notify(POST_SEND, event, data);
    }

    SCOUT_CALLBACK(cb_bw_pre_recv)
    {
        // backward communication: send message to the original sender

        CbData* data = static_cast< CbData* >(cdata);

        if (!is_mpi_api(event.get_cnode()->getRegion()))
        {
            return;
        }

        // --- run pre-send callbacks

        cbmanager.notify(PRE_RECV, event, data);

        // --- transfer message

        MpiComm*    comm = event->getComm();
        MpiMessage* msg;

        msg = data->mLocal->isend(*comm, event->getSource(), event->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());

        // --- run post-send callbacks

        cbmanager.notify(POST_RECV, event, data);
    }

    //
    // --- forward synchpoint detection (fws) callbacks ------------------
    //

    SCOUT_CALLBACK(cb_fw_pre_send)
    {
        CbData* data = static_cast< CbData* >(cdata);

        if (  !is_mpi_api(event.get_cnode()->getRegion())
           || event.completion()->isOfType(MPI_CANCELLED))
        {
            return;
        }

        // --- run pre-send callbacks

        cbmanager.notify(PRE_SEND, event, data);

        // --- transfer message

        MpiComm*    comm = event->getComm();
        MpiMessage* msg;

        msg = data->mLocal->isend(*comm, event->getDestination(), event->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());

        mInvComms.insert(make_pair(comm->getId(), static_cast< MpiComm* >(0)));

        // --- run post-send callbacks

        cbmanager.notify(POST_SEND, event, data);
    }

    SCOUT_CALLBACK(cb_fw_pre_recv)
    {
        CbData* data = static_cast< CbData* >(cdata);

        if (!is_mpi_api(event.get_cnode()->getRegion()))
        {
            return;
        }

        // --- run pre-recv callbacks

        cbmanager.notify(PRE_RECV, event, data);

        // --- receive message

        MpiComm* comm = event->getComm();
        data->mRemote->recv(*data->mDefs, *comm, event->getSource(), event->getTag());

        mInvComms.insert(make_pair(comm->getId(), static_cast< MpiComm* >(0)));

        // --- run post-recv callbacks

        cbmanager.notify(POST_RECV, event, data);
    }

    SCOUT_CALLBACK(cb_fw_pre_sendcmp)
    {
        CbData* data = static_cast< CbData* >(cdata);

        // --- Handle inverse communication.
        // In forward replay, this means msg from receive request to send completion:
        // receive message from receive request here.

        // --- check if this is a late-receiver synchpoint

        if (!data->mSynchpointHandler->isWaitstate(event))
        {
            return;
        }

        // --- receive message

        Event sendevt = event.request();

        communicator_map_t::iterator cit = mInvComms.find(sendevt->getComm()->getId());

        assert((cit != mInvComms.end()) && (cit->second != 0));

        data->mInvRemote->recv(*data->mDefs,
                               *(cit->second),
                               sendevt->getDestination(),
                               sendevt->getTag());

        // --- run post-recv callbacks

        cbmanager.notify(POST_INV_SENDCMP, event, data);
    }

    SCOUT_CALLBACK(cb_fw_pre_recvreq)
    {
        CbData* data = static_cast< CbData* >(cdata);

        // --- Handle inverse communication.
        // In forward replay, this means msg from receive request to send completion:
        // send message to send completion here.

        // --- check if this is a late-receiver synchpoint

        if (  !data->mSynchpointHandler->isSynchpoint(event)
           || data->mSynchpointHandler->isWaitstate(event))
        {
            return;
        }

        // --- run pre-send callbacks

        cbmanager.notify(PRE_INV_RECVREQ, event, data);

        // --- transfer message

        Event recvevt = event.completion();

        communicator_map_t::iterator cit = mInvComms.find(recvevt->getComm()->getId());

        assert((cit != mInvComms.end()) && (cit->second != 0));

        MpiMessage* msg;

        msg = data->mInvLocal->isend(*(cit->second),
                                     recvevt->getSource(),
                                     recvevt->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());
    }

    //
    // --- backward delay / critical-path analysis (bwc) -----------------
    //

    SCOUT_CALLBACK(cb_bwc_prepare)
    {
        CbData* data = static_cast< CbData* >(cdata);

        // --- duplicate communicators for inverse communication

        // Find out which communicators were used for p2p

        std::vector< uint32_t > lcomms;

        for (communicator_map_t::const_iterator it = mInvComms.begin();
             it != mInvComms.end();
             ++it)
        {
            lcomms.push_back(it->first);
        }

        std::vector< uint32_t >::iterator lcit = lcomms.begin();

        for (uint32_t c = 0; c < data->mDefs->numCommunicators(); ++c)
        {
            uint32_t lid = (lcit == lcomms.end() ? Communicator::NO_ID : *lcit);
            uint32_t gid;

            MPI_Allreduce(&lid, &gid, 1, SCALASCA_MPI_UINT32_T, MPI_MIN, MPI_COMM_WORLD);

            // FIXME: This assumes that Communicator::NO_ID is the max element!
            if (gid == Communicator::NO_ID)
            {
                break;
            }

            // Create communicator duplicate
            const MpiComm& comm = static_cast< const MpiComm& >(data->mDefs->getCommunicator(gid));

            if (comm.getHandle() != MPI_COMM_NULL)
            {
                mInvComms[gid] = comm.duplicate();
            }

            if (  (lcit != lcomms.end())
               && (*lcit == gid))
            {
                ++lcit;
            }
        }
    }

    // --- init/finalize

    SCOUT_CALLBACK(cb_bwc_pre_init_finalize)
    {
        CbData* data = static_cast< CbData* >(cdata);

        collinfo_map_t::iterator it = mCollInfo.find(event);
        assert(it != mCollInfo.end());

        UTILS_DLOG_LEVEL(SCALASCA_DEBUG_TRIGGER) << event;

        data->mCollinfo = it->second;
    }

    // --- collective communication dispatch

    SCOUT_CALLBACK(cb_bw_pre_mpi_collend)
    {
        CbData* const data = static_cast< CbData* >(cdata);

        // Skip collectives that have not been handled by the forward pass
        // (e.g., due to self-like communicators or MPI_Alltoallv/MPI_Alltoallw)
        collinfo_map_t::iterator it = mCollInfo.find(event);
        if (it == mCollInfo.end())
        {
            return;
        }

        UTILS_DLOG_LEVEL(SCALASCA_DEBUG_TRIGGER) << event;

        data->mCollinfo = it->second;

        // Dispatch
        const MpiCollEnd_rep& endRep = event_cast< MpiCollEnd_rep >(*event);
        switch (endRep.getCollType())
        {
            // CASE 1: Barrier
            case MpiCollEnd_rep::BARRIER:
                cbmanager.notify(SYNC_COLL, event, data);
                break;

            // CASE 2: 1-to-N communication
            case MpiCollEnd_rep::BCAST:
            case MpiCollEnd_rep::SCATTER:
            case MpiCollEnd_rep::SCATTERV:
                cbmanager.notify(COLL_12N, event, data);
                break;

            // CASE 3: N-to-N communication w/o MPI_Alltoallv & MPI_Alltoallw
            case MpiCollEnd_rep::ALLGATHER:
            case MpiCollEnd_rep::ALLGATHERV:
            case MpiCollEnd_rep::ALLTOALL:
            case MpiCollEnd_rep::ALLREDUCE:
            case MpiCollEnd_rep::REDUCE_SCATTER:
            case MpiCollEnd_rep::REDUCE_SCATTER_BLOCK:
                cbmanager.notify(COLL_N2N, event, data);
                break;

            // CASE 4: N-to-1 communication
            case MpiCollEnd_rep::GATHER:
            case MpiCollEnd_rep::GATHERV:
            case MpiCollEnd_rep::REDUCE:
                cbmanager.notify(COLL_N21, event, data);
                break;

            // CASE 5: Prefix reduction
            case MpiCollEnd_rep::SCAN:
            case MpiCollEnd_rep::EXSCAN:
                cbmanager.notify(COLL_SCAN, event, data);
                break;

            default:
                UTILS_BUG("Unknown MPI collective operation encountered");
                break;
        }
    }

    SCOUT_CALLBACK(cb_bw_pre_sendcmp)
    {
        CbData* data = static_cast< CbData* >(cdata);

        // --- Handle inverse communication.
        // In backward replay, this means msg from send completion to receive request:
        // send message to receive request here.

        // --- check if this is a late-receiver synchpoint

        if (!data->mSynchpointHandler->isWaitstate(event))
        {
            return;
        }

        // --- run pre-send callbacks

        cbmanager.notify(PRE_INV_SENDCMP, event, data);

        // --- transfer message

        Event sendevt = event.request();

        communicator_map_t::iterator cit = mInvComms.find(sendevt->getComm()->getId());

        assert((cit != mInvComms.end()) && (cit->second != 0));

        MpiMessage* msg;

        msg = data->mInvLocal->isend(*(cit->second),
                                     sendevt->getDestination(),
                                     sendevt->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());
    }

    SCOUT_CALLBACK(cb_bw_pre_recvreq)
    {
        CbData* data = static_cast< CbData* >(cdata);

        // --- Handle inverse communication.
        // In backward replay, this means msg from send completion to receive request:
        // receive message from send completion here.

        // --- check if this is a late-receiver synchpoint

        if (  !data->mSynchpointHandler->isSynchpoint(event)
           || data->mSynchpointHandler->isWaitstate(event))
        {
            return;
        }

        // --- receive message

        Event recvevt = event.completion();

        communicator_map_t::iterator cit = mInvComms.find(recvevt->getComm()->getId());

        assert((cit != mInvComms.end()) && (cit->second != 0));

        data->mInvRemote->recv(*data->mDefs,
                               *(cit->second),
                               recvevt->getSource(),
                               recvevt->getTag());

        // --- run post-recv callbacks

        cbmanager.notify(POST_INV_RECVREQ, event, data);
    }

    //
    // --- callback registration -----------------------------------------
    //

    void
    register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
    {
        struct cb_evt_table_t
        {
            pearl::event_t evt;
            void           (MpiCHImpl::* cbp)(const pearl::CallbackManager&,
                                              int,
                                              const pearl::Event&,
                                              pearl::CallbackData*);
        };


        struct cb_uevt_table_t
        {
            int  evt;
            void (MpiCHImpl::* cbp)(const pearl::CallbackManager&,
                                    int,
                                    const pearl::Event&,
                                    pearl::CallbackData*);
        };


        // --- main forward replay callbacks

        // *INDENT-OFF*
        const struct cb_evt_table_t main_evt_tbl[] = {
            { MPI_COLLECTIVE_END,     &MpiCHImpl::cb_pre_mpi_collend    },
            { MPI_RMA_COLLECTIVE_END, &MpiCHImpl::cb_pre_rma_collend    },

            { MPI_RMA_GATS,           &MpiCHImpl::cb_pre_rma_group_sync },

            { GROUP_SEND,             &MpiCHImpl::cb_pre_send           },
            { GROUP_RECV,             &MpiCHImpl::cb_pre_recv           },

            // Order of the following two entries is important.  Do not change!
            { GROUP_LEAVE,            &MpiCHImpl::cb_pre_leave_cache    },
            { GROUP_LEAVE,            &MpiCHImpl::cb_pre_leave          },

            { MPI_RMA_PUT_START,      &MpiCHImpl::cb_pre_rma_op_start   },
            { MPI_RMA_GET_START,      &MpiCHImpl::cb_pre_rma_op_start   },

            { MPI_COLLECTIVE_END,     &MpiCHImpl::cb_process_msgs       },
            { GROUP_SEND,             &MpiCHImpl::cb_process_msgs       },
            { GROUP_RECV,             &MpiCHImpl::cb_process_msgs       },
            { GROUP_END,              &MpiCHImpl::cb_process_msgs       },

            { NUM_EVENT_TYPES,        0                                 }
        };
        // *INDENT-ON*


        // *INDENT-OFF*
        const struct cb_uevt_table_t main_uevt_tbl[] = {
            { PRE_INIT,             &MpiCHImpl::cb_pre_init_finalize    },
            { PRE_FINALIZE,         &MpiCHImpl::cb_pre_init_finalize    },
            { PRE_COLL_12N,         &MpiCHImpl::cb_pre_coll_12n         },
            { PRE_COLL_N2N,         &MpiCHImpl::cb_pre_coll_n2n         },
            { PRE_COLL_N21,         &MpiCHImpl::cb_pre_coll_n21         },
            { PRE_COLL_SCAN,        &MpiCHImpl::cb_pre_coll_scan        },
            { PRE_SYNC_COLL,        &MpiCHImpl::cb_pre_sync_coll        },

            { RMA_SYNC_COLLECTIVE,  &MpiCHImpl::cb_pre_rma_sync_coll    },
            { MPI_RMA_WIN_CREATE,   &MpiCHImpl::cb_pre_rma_win_create   },
            { MPI_RMA_WIN_FENCE,    &MpiCHImpl::cb_pre_rma_win_fence    },
            { MPI_RMA_WIN_FREE,     &MpiCHImpl::cb_pre_rma_win_free     },
            { MPI_RMA_WIN_START,    &MpiCHImpl::cb_pre_mpi_win_start    },
            { MPI_RMA_WIN_COMPLETE, &MpiCHImpl::cb_pre_mpi_win_complete },
            { MPI_RMA_WIN_POST,     &MpiCHImpl::cb_pre_mpi_win_post     },
            { MPI_RMA_WIN_WAIT,     &MpiCHImpl::cb_pre_mpi_win_wait     },

            { FINISHED,             &MpiCHImpl::cb_finished             },

            { 0,                    0                                   }
        };
        // *INDENT-ON*


        // --- backward wait-state / synchpoint detection callbacks

        // *INDENT-OFF*
        const struct cb_evt_table_t bws_evt_tbl[] = {
            { GROUP_LEAVE,        &MpiCHImpl::cb_pre_leave        },

            { GROUP_SEND,         &MpiCHImpl::cb_bw_pre_send      },
            { GROUP_RECV,         &MpiCHImpl::cb_bw_pre_recv      },

            { GROUP_SEND,         &MpiCHImpl::cb_process_msgs     },
            { GROUP_RECV,         &MpiCHImpl::cb_process_msgs     },
            { GROUP_END,          &MpiCHImpl::cb_process_msgs     },

            { MPI_COLLECTIVE_END, &MpiCHImpl::cb_progress_barrier },

            { NUM_EVENT_TYPES,    0                               }
        };
        // *INDENT-ON*


        // *INDENT-OFF*
        const struct cb_uevt_table_t bws_uevt_tbl[] = {
            { FINISHED, &MpiCHImpl::cb_finished },

            { 0,        0                       }
        };
        // *INDENT-ON*


        // --- forward wait-state / synchpoint detection (fws) callbacks

        // *INDENT-OFF*
        const struct cb_evt_table_t fws_evt_tbl[] = {
            { GROUP_LEAVE,        &MpiCHImpl::cb_pre_leave        },

            { GROUP_SEND,         &MpiCHImpl::cb_fw_pre_send      },
            { GROUP_RECV,         &MpiCHImpl::cb_fw_pre_recv      },

            { GROUP_SEND,         &MpiCHImpl::cb_process_msgs     },
            { GROUP_RECV,         &MpiCHImpl::cb_process_msgs     },
            { GROUP_END,          &MpiCHImpl::cb_process_msgs     },

            { MPI_COLLECTIVE_END, &MpiCHImpl::cb_progress_barrier },

            { NUM_EVENT_TYPES,    0                               }
        };
        // *INDENT-ON*


        // *INDENT-OFF*
        const struct cb_uevt_table_t fws_uevt_tbl[] = {
            { FINISHED, &MpiCHImpl::cb_finished },

            { 0,        0                       }
        };
        // *INDENT-ON*


        // --- delay / critical-path analysis backward replay (bwc) callbacks

        // *INDENT-OFF*
        const struct cb_evt_table_t bwc_evt_tbl[] = {
            { GROUP_LEAVE,        &MpiCHImpl::cb_pre_leave          },

            { GROUP_SEND,         &MpiCHImpl::cb_bw_pre_send        },
            { GROUP_RECV,         &MpiCHImpl::cb_bw_pre_recv        },

            { MPI_SEND,           &MpiCHImpl::cb_bw_pre_sendcmp     },
            { MPI_SEND_COMPLETE,  &MpiCHImpl::cb_bw_pre_sendcmp     },
            { MPI_RECV,           &MpiCHImpl::cb_bw_pre_recvreq     },
            { MPI_RECV_REQUEST,   &MpiCHImpl::cb_bw_pre_recvreq     },

            { MPI_COLLECTIVE_END, &MpiCHImpl::cb_bw_pre_mpi_collend },

            { GROUP_SEND,         &MpiCHImpl::cb_process_msgs       },
            { GROUP_RECV,         &MpiCHImpl::cb_process_msgs       },
            { GROUP_END,          &MpiCHImpl::cb_process_msgs       },

            { NUM_EVENT_TYPES,    0                                 }
        };
        // *INDENT-ON*


        // *INDENT-OFF*
        const struct cb_uevt_table_t bwc_uevt_tbl[] = {
            { PREPARE,      &MpiCHImpl::cb_bwc_prepare           },
            { FINISHED,     &MpiCHImpl::cb_finished              },

            { PRE_INIT,     &MpiCHImpl::cb_bwc_pre_init_finalize },
            { PRE_FINALIZE, &MpiCHImpl::cb_bwc_pre_init_finalize },

            { 0,            0                                    }
        };
        // *INDENT-ON*


        // --- forward propagating/indirect wait-state analysis (fwc) callbacks

        // *INDENT-OFF*
        const struct cb_evt_table_t fwc_evt_tbl[] = {
            { GROUP_LEAVE,       &MpiCHImpl::cb_pre_leave      },

            { GROUP_SEND,        &MpiCHImpl::cb_fw_pre_send    },
            { GROUP_RECV,        &MpiCHImpl::cb_fw_pre_recv    },

            { MPI_SEND,          &MpiCHImpl::cb_fw_pre_sendcmp },
            { MPI_SEND_COMPLETE, &MpiCHImpl::cb_fw_pre_sendcmp },
            { MPI_RECV,          &MpiCHImpl::cb_fw_pre_recvreq },
            { MPI_RECV_REQUEST,  &MpiCHImpl::cb_fw_pre_recvreq },

            { GROUP_SEND,        &MpiCHImpl::cb_process_msgs   },
            { GROUP_RECV,        &MpiCHImpl::cb_process_msgs   },
            { GROUP_END,         &MpiCHImpl::cb_process_msgs   },

            { NUM_EVENT_TYPES,   0                             }
        };
        // *INDENT-ON*


        // *INDENT-OFF*
        const struct cb_uevt_table_t fwc_uevt_tbl[] = {
            { FINISHED, &MpiCHImpl::cb_finished },

            { 0,        0                       }
        };
        // *INDENT-ON*


        // --- global callback tables table :)

        // *INDENT-OFF*
        const struct cb_tables_t
        {
            const char*                   stage;
            const struct cb_evt_table_t*  evts;
            const struct cb_uevt_table_t* uevts;
        }
        cb_tables[] = {
            { "",    main_evt_tbl, main_uevt_tbl },
            { "bws", bws_evt_tbl,  bws_uevt_tbl  },
            { "fws", fws_evt_tbl,  fws_uevt_tbl  },
            { "bwc", bwc_evt_tbl,  bwc_uevt_tbl  },
            { "fwc", fwc_evt_tbl,  fwc_uevt_tbl  },

            { 0,     0,            0             }
        };
        // *INDENT-ON*


        // --- register callbacks

        for (const struct cb_tables_t* t = cb_tables; t->stage; ++t)
        {
            CallbackManagerMap::const_iterator it = cbm.find(t->stage);

            assert(it != cbm.end());

            for (const struct cb_evt_table_t* c = t->evts; c->cbp; ++c)
            {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
            for (const struct cb_uevt_table_t* c = t->uevts; c->cbp; ++c)
            {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
        }
    }
};


MpiCommunicationHandler::MpiCommunicationHandler()
    : AnalysisHandler(),
      mP(new MpiCHImpl)
{
}


MpiCommunicationHandler::~MpiCommunicationHandler()
{
}


void
MpiCommunicationHandler::register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
{
    mP->register_pre_pattern_callbacks(cbm);
}


const TimeRank MpiCommunicationHandler::MpiCHImpl::UNDEFINED_TIME_RANK;
