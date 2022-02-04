/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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


#ifndef CLC_INTERVALSTAT_H
#define CLC_INTERVALSTAT_H


#include <vector>

#include <pearl/LocalTrace.h>
#include <pearl/pearl_types.h>

#include "StatData.h"


class IntervalStat
{
    public:
        /* Constructors & destructor */
        IntervalStat(const pearl::LocalTrace& trace,
                     int                      rank,
                     int                      tid = 0);

        ~IntervalStat();

        // Get methods
        void
        get_org_timestamps();

        void
        get_fa_timestamps();

        void
        get_ba_timestamps();

        // Print methods
        void
        print_timestamps();

        void
        calculate();


    private:
        // Local event trace
        const pearl::LocalTrace& trace;

        // My rank and thread id
        int m_rank;
        int m_tid;

        // Data vector
        std::vector< statistic_data > data_vec;

        // Timestamp buffers
        pearl::timestamp_t* org_ts;
        pearl::timestamp_t* ba_ts;
        pearl::timestamp_t* fa_ts;
        pearl::timestamp_t* tsa;

        // Number of events
        long               num_events;
        pearl::timestamp_t t_last_org;
        pearl::timestamp_t t_last_sync;
        pearl::timestamp_t t_diff_org;
        pearl::timestamp_t t_diff_sync;
        pearl::timestamp_t t_sum_time;
        pearl::timestamp_t abs_sum;
        pearl::timestamp_t rel_sum;


        // Calculate functions
        void
        calculate_data();

        void
        calculate_relative_errors();

        void
        calculate_absolute_errors();

        void
        calculate_histograms();

        void
        calculate_histogram_class(statistic_data     data,
                                  pearl::timestamp_t num_data,
                                  pearl::timestamp_t num_interval,
                                  pearl::timestamp_t g_org_exe_time,
                                  pearl::timestamp_t g_sync_exe_time);

        // Helper function
        void
        prepare_calculation(pearl::timestamp_t* t_array);
};


#endif    // !CLC_INTERVALSTAT_H
