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


#ifndef CLC_SYNCHRONIZER_H
#define CLC_SYNCHRONIZER_H


#include <functional>
#include <map>
#include <vector>

#include <mpi.h>

#include <pearl/CallbackData.h>
#include <pearl/CallbackManager.h>
#include <pearl/Event.h>
#include <pearl/LocalTrace.h>
#include <pearl/MpiComm.h>
#include <pearl/Uncopyable.h>
#include <pearl/pearl_types.h>

#include "clc_types.h"


// --- Forward declarations -------------------------------------------------

class Clock;
class FwdAmortData;
class IntervalStat;
class Network;
class RuntimeStat;
class TimeStat;

struct amortization_data;


/// Helper class comparing events based on their ID rather than on their
/// timestamps.
struct EventCompare
    : public std::binary_function< pearl::Event, pearl::Event, bool >
{
    bool
    operator()(const pearl::Event& ev1,
               const pearl::Event& ev2) const
    {
        return (ev1.get_id() < ev2.get_id());
    }
};


/**
 *  Scalable timestamp synchronization class for
 *  postmortem performance analysis.
 *
 *  Uses the Extended Controlled Logical Clock
 **/
class Synchronizer
    : private pearl::Uncopyable
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        Synchronizer(int                      rank,
                     const pearl::LocalTrace& trace,
                     TimeStat*                timeStat = 0,
                     IntervalStat*            intervalStat = 0);

        ~Synchronizer();

        /// @}
        /// @name Timestamp correction
        /// @{

        void
        synchronize();

        void
        forward_amortization();

        void
        backward_amortization();

        /// @}
        /// @name Statistics
        /// @{

        void
        print_statistics();

        /// @}


    private:
        // --- Data members --------------------------------

        /// Extended controlled logical clock
        Clock* m_clc;

        /// Network the clock is running on
        Network* m_network;

        /// Location of the clock
        clc_location m_loc;

        /// Statistics
        RuntimeStat* r_stat;

        /// Local event trace
        const pearl::LocalTrace& trace;

        /// First event's timestamp
        pearl::timestamp_t firstEvtT;

        /// Initial slope for the backward amortization
        double max_error;

        /// Maximum slope
        pearl::timestamp_t m_max_slope;

        /// Threshold for synchronization error at synchronization end
        double threshold;

        /// Global rank during the replay
        int m_rank;

        /// Number of logical send events
        long num_sends;

        /// Local time statistics
        TimeStat* t_stat;

        /// Local interval statistics
        IntervalStat* i_stat;

        /// Map with the violating receive events
        std::map< pearl::Event, pearl::timestamp_t, EventCompare > m_violations;

        /// Timestamp array
        pearl::timestamp_t* tsa;

        // Asynchronous message handling
        std::vector< amortization_data* > m_data;
        std::vector< MPI_Request >        m_requests;
        std::vector< int >                m_indices;
        std::vector< MPI_Status >         m_statuses;

        // Reverse communicator handling
        std::map< uint32_t, MPI_Comm > m_reverse_comms;


        // --- Private methods -----------------------------

        /// @name Callback functions for forward amortization
        /// @{

        void
        amortize_fwd_leave(const pearl::CallbackManager& cbManager,
                           int                           userEvent,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cbData);

        void
        amortize_fwd_send(const pearl::CallbackManager& cbManager,
                          int                           userEvent,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cbData);

        void
        amortize_fwd_recv(const pearl::CallbackManager& cbManager,
                          int                           userEvent,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cbData);

        void
        amortize_fwd_coll(const pearl::CallbackManager& cbManager,
                          int                           userEvent,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cbData);

        void
        amortize_fwd_fork(const pearl::CallbackManager& cbManager,
                          int                           userEvent,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cbData);

        void
        amortize_fwd_ttb(const pearl::CallbackManager& cbManager,
                         int                           userEvent,
                         const pearl::Event&           event,
                         pearl::CallbackData*          cbData);

        void
        amortize_fwd_tte(const pearl::CallbackManager& cbManager,
                         int                           userEvent,
                         const pearl::Event&           event,
                         pearl::CallbackData*          cbData);

        void
        amortize_fwd_join(const pearl::CallbackManager& cbManager,
                          int                           userEvent,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cbData);

        /// @}
        /// @name Callback functions for backward amortization
        /// @{

        void
        amortize_bkwd_leave(const pearl::CallbackManager& cbManager,
                            int                           userEvent,
                            const pearl::Event&           event,
                            pearl::CallbackData*          cbData);

        void
        amortize_bkwd_send(const pearl::CallbackManager& cbManager,
                           int                           userEvent,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cbData);

        void
        amortize_bkwd_recv(const pearl::CallbackManager& cbManager,
                           int                           userEvent,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cbData);

        void
        amortize_bkwd_coll(const pearl::CallbackManager& cbManager,
                           int                           userEvent,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cbData);

        void
        amortize_bkwd_fork(const pearl::CallbackManager& cbManager,
                           int                           userEvent,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cbData);

        void
        amortize_bkwd_ttb(const pearl::CallbackManager& cbManager,
                          int                           userEvent,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cbData);

        void
        amortize_bkwd_tte(const pearl::CallbackManager& cbManager,
                          int                           userEvent,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cbData);

        void
        amortize_bkwd_join(const pearl::CallbackManager& cbManager,
                           int                           userEvent,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cbData);

        /// @}
        /// @name Forward amortization functions
        /// @{

        void
        amortize_fwd_omp_barrier(const pearl::Event& event,
                                 FwdAmortData*       data);

        void
        amortize_fwd_mpi_init_finalize(const pearl::Event& event,
                                       FwdAmortData*       data);

        void
        amortize_fwd_mpi_barrier(const pearl::Event& event,
                                 FwdAmortData*       data);

        void
        amortize_fwd_mpi_12n(const pearl::Event& event,
                             FwdAmortData*       data);

        void
        amortize_fwd_mpi_n2n(const pearl::Event& event,
                             FwdAmortData*       data);

        void
        amortize_fwd_mpi_n21(const pearl::Event& event,
                             FwdAmortData*       data);

        void
        amortize_fwd_mpi_scan(const pearl::Event& event,
                              FwdAmortData*       data);

        void
        fa_amortize_recv(const pearl::Event& event,
                         pearl::timestamp_t  sendEvtT,
                         clc_location&       ex_loc,
                         clc_event_t         event_type,
                         FwdAmortData*       data);

        void
        fa_amortize_intern(const pearl::Event& event,
                           FwdAmortData*       data);

        /// @}
        /// @name Backward amortization functions
        /// @{

        void
        amortize_bkwd_omp_barrier(const pearl::Event& event);

        void
        amortize_bkwd_mpi_init_finalize(const pearl::Event& event);

        void
        amortize_bkwd_mpi_barrier(const pearl::Event& event);

        void
        amortize_bkwd_mpi_12n(const pearl::Event& event);

        void
        amortize_bkwd_mpi_n2n(const pearl::Event& event);

        void
        amortize_bkwd_mpi_n21(const pearl::Event& event);

        void
        amortize_bkwd_mpi_scan(const pearl::Event& event);

        void
        ba_amortize();

        void
        ba_piecewise_interpolation(const pearl::Event& event_rhs,
                                   pearl::timestamp_t  time_rhs,
                                   pearl::timestamp_t  delta_rhs,
                                   pearl::timestamp_t  slope_g1,
                                   long                send_idx);

        void
        ba_linear_amortization_event_to_event(const pearl::Event& event_lhs,
                                              const pearl::Event& event_rhs,
                                              pearl::timestamp_t  slope_g,
                                              pearl::timestamp_t  c_g);

        void
        ba_linear_amortization_time_to_event(pearl::timestamp_t  time_lhs,
                                             const pearl::Event& event_rhs,
                                             pearl::timestamp_t  slope_g,
                                             pearl::timestamp_t  c_g);

        /// @}
        /// @name Asynchronous message handling
        /// @{

        void
        update_requests();

        void
        delete_requests();

        /// @}
        /// @name Reverse communicator handling
        /// @{

        MPI_Comm
        getReverseComm(pearl::MpiComm* comm);

        void
        freeReverseComms();

        /// @}
        /// @name Helper functions
        /// @{

        void
        prepare_replay();

        void
        get_ts();

        void
        set_ts();

        /// @}


        friend class FwdAmortData;
};


#endif    // !CLC_SYNCHRONIZER_H
