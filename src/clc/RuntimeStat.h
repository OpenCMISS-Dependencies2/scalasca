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


#ifndef CLC_RUNTIMESTAT_H
#define CLC_RUNTIMESTAT_H


#include <pearl/pearl_types.h>


class RuntimeStat
{
    public:
        // Constructors and Destructor
        RuntimeStat();

        RuntimeStat(int passes);

        ~RuntimeStat();

        // Pass handling
        int
        get_num_passes();

        int
        get_pass();

        void
        set_num_passes(int num);

        void
        set_pass();

        bool
        next_pass();

        // Error handling
        pearl::timestamp_t
        get_gl_error();

        pearl::timestamp_t
        get_clock_error();

        pearl::timestamp_t
        get_sum_clock_error();

        pearl::timestamp_t
        get_max_clock_error();

        pearl::timestamp_t
        get_runtime()
        {
            return (e_time - s_time);
        }

        pearl::timestamp_t
        get_num_clock_corr();

        // Statistic handling
        void
        set_statistics(const pearl::timestamp_t curEvtT,
                       const pearl::timestamp_t value);


    private:
        // Some statistics
        int num_passes;
        int pass;

        // Actual clock difference
        pearl::timestamp_t clock_diff;

        // Maximum clock difference
        pearl::timestamp_t max_clock_diff;

        // Sum of clock difference
        pearl::timestamp_t sum_clock_diff;

        // Number of clock corrections
        long num_clock_corr;

        // Start time
        pearl::timestamp_t s_time;

        // End time
        pearl::timestamp_t e_time;


        // Helper function
        void
        m_init();
};


#endif    // !CLC_RUNTIMESTAT_H
