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


#include <config.h>

#include "Timer.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <mpi.h>

#include <pearl/CommSet.h>
#include <pearl/Location.h>
#include <pearl/MpiComm.h>
#include <pearl/Region.h>
#include <pearl/pearl_replay.h>

#include "Latency.h"
#include "Network.h"
#include "Operator.h"
#include "Predicates.h"

#ifdef _OPENMP
    #include <omp.h>

    #include "OmpCommunication.h"
    #include "OmpData.h"
#endif    // _OPENMP

using namespace std;
using namespace pearl;


Timer::Timer(int               r,
             const LocalTrace& myTrace)
    : trace(myTrace),
      m_rank(r)
{
    init();
}


Timer::~Timer()
{
    #pragma omp master
    {
        free_operators();
    }
}


void
Timer::init()
{
    // Set up statistics
    direct_violation = 0;
    clock_violation  = 0;
    m_backward_error = 0.0;
    m_sum_error      = 0.0;
    m_messages       = 0;
    m_p2p_violation  = 0;
    m_coll_violation = 0;
    m_omp_violation  = 0;

    // Retrieve node id from trace
    const Location& m_loc = trace.get_location();
    loc.machine = m_loc.getMachineId();
    loc.node    = m_loc.getNodeId();
    loc.process = m_loc.getRank();
    loc.thread  = m_loc.getThreadId();

    // Create Latency (default)
    //
    // FIXME:
    // These generic minimal latencies should be reviewed and ultimately made
    // configurable, as they are system-dependent parameters
    Latency* latency = new Latency(0.0, 1.0e-6, 3.0e-6, 1.0e-6, 3.0e-6, 0.1e-6);

    // Create Network with Latency
    network = new Network(loc, latency);

    // Initialize operator and datatypes
    #pragma omp master
    {
        init_operators();
    }
}


void
Timer::check()
{
    // Set up callback manager
    CallbackManager cbmanager;

    // Register callbacks
    cbmanager.register_callback(GROUP_SEND, PEARL_create_callback(this, &Timer::check_send));
    cbmanager.register_callback(GROUP_RECV, PEARL_create_callback(this, &Timer::check_recv));
    cbmanager.register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &Timer::check_coll));
    #if defined(_OPENMP)
        cbmanager.register_callback(THREAD_FORK, PEARL_create_callback(this, &Timer::check_fork));
        cbmanager.register_callback(THREAD_JOIN, PEARL_create_callback(this, &Timer::check_join));
        cbmanager.register_callback(GROUP_ENTER, PEARL_create_callback(this, &Timer::check_enter));
        cbmanager.register_callback(GROUP_LEAVE, PEARL_create_callback(this, &Timer::check_leave));
    #endif    // _OPENMP

    // Replay the target application and check timestamps...
    PEARL_forward_replay(trace, cbmanager, NULL);
}


void
Timer::check_send(const CallbackManager& cbmanager,
                  int                    user_event,
                  const Event&           event,
                  CallbackData*          cdata)
{
    amortization_data local_object = pack_data(loc, event->getTimestamp());

    // Send send-event timestamp
    MPI_Send(&local_object, 1, AMORT_DATA, event->getDestination(),
             event->getTag(), event->getComm()->getHandle());
}


void
Timer::check_recv(const CallbackManager& cbmanager,
                  int                    user_event,
                  const Event&           event,
                  CallbackData*          cdata)
{
    // Receive send event time
    MPI_Status        m_stat;
    amortization_data ex_object;
    MPI_Recv(&ex_object, 1, AMORT_DATA, event->getSource(),
             event->getTag(), event->getComm()->getHandle(), &m_stat);
    m_messages++;

    check_violation(event, ex_object, CLC_P2P);
}


void
Timer::check_coll(const CallbackManager& cbmanager,
                  int                    user_event,
                  const Event&           event,
                  CallbackData*          cdata)
{
    // Determine corresponding BEGIN event, region, and communicator
    Event         begin  = event.beginptr();
    const Region& region = event.enterptr()->getRegion();
    MpiComm*      comm   = event->getComm();

    // Pack local data
    amortization_data local_object = pack_data(loc, begin->getTimestamp());

    // CASE 1: Barrier and N-to-N communication
    if (  is_mpi_barrier(region)
       || is_mpi_n2n(region))
    {
        amortization_data ex_object;
        MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                      CLC_MAX, comm->getHandle());
        m_messages += comm->getSize();

        check_violation(event, ex_object, CLC_COLL);
    }

    // CASE 2: 1-to-N communication
    else if (is_mpi_12n(region))
    {
        MPI_Bcast(&local_object, 1, AMORT_DATA,
                  event->getRoot(),
                  comm->getHandle());
        if (!isOnRoot(event, m_rank))
        {
            m_messages++;
            check_violation(event, local_object, CLC_COLL);
        }
    }

    // CASE 3: N-to-1 communication
    else if (is_mpi_n21(region))
    {
        // Receive max ( all BEGIN event times + latency )
        amortization_data ex_object;
        MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA, CLC_MAX,
                   event->getRoot(),
                   comm->getHandle());
        if (isOnRoot(event, m_rank))
        {
            m_messages += comm->getSize();
            check_violation(event, ex_object, CLC_COLL);
        }
    }

    // CASE 4: Prefix reduction
    else if (is_mpi_scan(region))
    {
        // Receive latest BEGIN event timestamp
        amortization_data ex_object;
        MPI_Scan(&local_object, &ex_object, 1, AMORT_DATA, CLC_MAX,
                 comm->getHandle());
        m_messages++;
        check_violation(event, ex_object, CLC_COLL);
    }

    // We should never reach the (missing) 'else' part -- but if we do,
    // it would be a no-op anyway...
}


void
Timer::check_fork(const CallbackManager& cbmanager,
                  int                    user_event,
                  const Event&           event,
                  CallbackData*          cdata)
{
    #ifdef _OPENMP
        // Exchange FORK event timestamp
        omp_global_max_timestamp = event->getTimestamp();
        m_messages              += omp_get_num_threads();
    #endif    // _OPENMP
}


void
Timer::check_join(const CallbackManager& cbmanager,
                  int                    user_event,
                  const Event&           event,
                  CallbackData*          cdata)
{
    #ifdef _OPENMP
        amortization_data ex_object;

        // Receive latest LEAVE event timestamp of parallel region
        ex_object   = pack_data(loc, omp_global_max_timestamp);
        m_messages += omp_get_num_threads();

        check_violation(event, ex_object, CLC_OMP);
    #endif    // _OPENMP
}


void
Timer::check_enter(const CallbackManager& cbmanager,
                   int                    user_event,
                   const Event&           event,
                   CallbackData*          cdata)
{
    #ifdef _OPENMP
        if (!is_omp_parallel(event->getRegion()))
        {
            return;
        }

        // Receive FORK event timestamp
        {
            // FCCpx requires block
            #pragma omp barrier
        }
        amortization_data ex_object;
        ex_object = pack_data(loc, omp_global_max_timestamp);

        check_violation(event, ex_object, CLC_OMP);
    #endif    // _OPENMP
}


void
Timer::check_leave(const CallbackManager& cbmanager,
                   int                    user_event,
                   const Event&           event,
                   CallbackData*          cdata)
{
    #ifdef _OPENMP
        const Region& region = event.enterptr()->getRegion();
        if (is_omp_barrier(region))
        {
            // Receive latest ENTER event timestamp
            OMP_Allreduce_max(event.enterptr()->getTimestamp(), omp_global_max_timestamp);
            amortization_data ex_object;
            ex_object   = pack_data(loc, omp_global_max_timestamp);
            m_messages += omp_get_num_threads();

            check_violation(event, ex_object, CLC_OMP);
        }
        else if (is_omp_parallel(region))
        {
            // Exchange latest LEAVE event timestamp
            OMP_Allreduce_max(event->getTimestamp(), omp_global_max_timestamp);
        }
    #endif    // _OPENMP
}


void
Timer::check_violation(const Event&       event,
                       amortization_data& ex_object,
                       clc_event_t        event_type)
{
    // Receive event timestamp
    timestamp_t recvEvtT = event->getTimestamp();

    // Get corresponding timestamp, location, and event type via replay
    clc_location ex_loc;
    timestamp_t  sendEvtT = unpack_data(ex_loc, ex_object);

    if (recvEvtT < sendEvtT)
    {
        direct_violation++;
        m_backward_error = max(m_backward_error, (sendEvtT - recvEvtT));
        m_sum_error     += (sendEvtT - recvEvtT);

        switch (event_type)
        {
            case CLC_P2P:
                m_p2p_violation++;
                break;

            case CLC_COLL:
                m_coll_violation++;
                break;

            case CLC_OMP:
                m_omp_violation++;
                break;

            default:
                assert(false);
                break;
        }
    }

    sendEvtT += network->get_latency(ex_loc, event_type);
    if (recvEvtT < sendEvtT)
    {
        clock_violation++;
    }
}


void
Timer::print_statistics()
{
    int sum_direct_violation = 0;

    #ifdef _OPENMP
        OMP_Allreduce_sum(direct_violation, omp_global_sum_count);
        direct_violation = omp_global_sum_count;
    #endif    // _OPENMP
    #pragma omp master
    {
        MPI_Allreduce(&direct_violation, &sum_direct_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }

    int sum_clock_violation = 0;
    #ifdef _OPENMP
        OMP_Allreduce_sum(clock_violation, omp_global_sum_count);
        clock_violation = omp_global_sum_count;
    #endif    // _OPENMP
    #pragma omp master
    {
        MPI_Allreduce(&clock_violation, &sum_clock_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }

    int sum_messages = 0;
    #ifdef _OPENMP
        OMP_Allreduce_sum(m_messages, omp_global_sum_count);
        m_messages = omp_global_sum_count;
    #endif    // _OPENMP
    #pragma omp master
    {
        MPI_Allreduce(&m_messages, &sum_messages, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }

    timestamp_t max_error = 0.0;
    #ifdef _OPENMP
        OMP_Allreduce_max(m_backward_error, omp_global_max_timestamp);
        m_backward_error = omp_global_max_timestamp;
    #endif    // _OPENMP
    #pragma omp master
    {
        MPI_Allreduce(&m_backward_error, &max_error, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    }

    timestamp_t sum_error = 0.0;
    #ifdef _OPENMP
        OMP_Allreduce_max(m_sum_error, omp_global_max_timestamp);
        m_sum_error = omp_global_max_timestamp;
    #endif    // _OPENMP
    #pragma omp master
    {
        MPI_Allreduce(&m_sum_error, &sum_error, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    }

    int sum_p2p_violation = 0;
    #ifdef _OPENMP
        OMP_Allreduce_sum(m_p2p_violation, omp_global_sum_count);
        m_p2p_violation = omp_global_sum_count;
    #endif    // _OPENMP
    #pragma omp master
    {
        MPI_Allreduce(&m_p2p_violation, &sum_p2p_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }

    int sum_coll_violation = 0;
    #ifdef _OPENMP
        OMP_Allreduce_sum(m_coll_violation, omp_global_sum_count);
        m_coll_violation = omp_global_sum_count;
    #endif    // _OPENMP
    #pragma omp master
    {
        MPI_Allreduce(&m_coll_violation, &sum_coll_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }

    int sum_omp_violation = 0;
    #ifdef _OPENMP
        OMP_Allreduce_sum(m_omp_violation, omp_global_sum_count);
        m_omp_violation = omp_global_sum_count;
    #endif    // _OPENMP
    #pragma omp master
    {
        MPI_Allreduce(&m_omp_violation, &sum_omp_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }

    {
        #pragma omp barrier
    }

    #pragma omp master
    {
        if (m_rank == 0)
        {
            printf("\n\t# messages       : %d\n", sum_messages);
            printf("\t# reversed       : %d\n", sum_direct_violation);
            printf("\t# reversed-p2p   : %d\n", sum_p2p_violation);
            printf("\t# reversed-coll  : %d\n", sum_coll_violation);
            printf("\t# reversed-omp   : %d\n", sum_omp_violation);
            printf("\t# reversed-mu    : %d\n", sum_clock_violation);
            printf("\t# max. error [s] : %.9f\n", max_error);
            if (sum_direct_violation != 0)
            {
                printf("\t# avg. error [s]: %.9f\n", (sum_error / sum_direct_violation));
            }
        }
    }
}
