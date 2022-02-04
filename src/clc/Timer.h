/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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


#ifndef CLC_TIMER_H
#define CLC_TIMER_H


#include <pearl/CallbackData.h>
#include <pearl/CallbackManager.h>
#include <pearl/Event.h>
#include <pearl/LocalTrace.h>
#include <pearl/pearl_types.h>

#include "AmortData.h"
#include "clc_types.h"


class Network;


class Timer
{
    public:
        // Constructors and Destructor
        Timer(int                      r,
              const pearl::LocalTrace& myTrace);

        ~Timer();

        // Used to sanity check the event stream
        void
        check();

        // Statistic functions
        void
        print_statistics();


    private:
        // Each Timer has a local event trace
        const pearl::LocalTrace& trace;

        // Network the clock is running on
        Network* network;

        // Some statistics
        int                direct_violation;
        int                clock_violation;
        int                m_messages;
        int                m_p2p_violation;
        int                m_coll_violation;
        int                m_omp_violation;
        pearl::timestamp_t m_backward_error;
        pearl::timestamp_t m_sum_error;

        // Additional information
        clc_location loc;
        int          m_rank;


        // Callback functions for amortization
        void
        check_send(const pearl::CallbackManager& cbmanager,
                   int                           user_event,
                   const pearl::Event&           event,
                   pearl::CallbackData*          cdata);

        void
        check_recv(const pearl::CallbackManager& cbmanager,
                   int                           user_event,
                   const pearl::Event&           event,
                   pearl::CallbackData*          cdata);

        void
        check_coll(const pearl::CallbackManager& cbmanager,
                   int                           user_event,
                   const pearl::Event&           event,
                   pearl::CallbackData*          cdata);

        void
        check_fork(const pearl::CallbackManager& cbmanager,
                   int                           user_event,
                   const pearl::Event&           event,
                   pearl::CallbackData*          cdata);

        void
        check_join(const pearl::CallbackManager& cbmanager,
                   int                           user_event,
                   const pearl::Event&           event,
                   pearl::CallbackData*          cdata);

        void
        check_enter(const pearl::CallbackManager& cbmanager,
                    int                           user_event,
                    const pearl::Event&           event,
                    pearl::CallbackData*          cdata);

        void
        check_leave(const pearl::CallbackManager& cbmanager,
                    int                           user_event,
                    const pearl::Event&           event,
                    pearl::CallbackData*          cdata);

        void
        check_violation(const pearl::Event& event,
                        amortization_data&  ex_object,
                        clc_event_t         event_type);

        // Useful helper functions
        void
        init();
};


#endif    // !CLC_TIMER_H
