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


#ifndef SILAS_HYPBALANCE_H
#define SILAS_HYPBALANCE_H


#include <set>
#include <string>

#include <pearl/CallbackManager.h>

#include "HypothesisPart.h"


namespace silas
{
/**
 * Balance the timespan of atomic regions
 */
class HypBalance
    : public HypothesisPart
{
    public:
        /// @name Constructors & Destructors
        /// @{

        HypBalance();

        virtual
        ~HypBalance();

        /// @}
        /**
         * Register the needed callbacks to enable the management of
         * user defined modifications on specific events.
         */
        virtual void
        register_callbacks(const int               run,
                           pearl::CallbackManager* cb_manager);

        /**
         * register a region to be balanced
         */
        virtual void
        register_region(std::string region_name);


    private:
        /**
         * callback to perform balancing of region timespans
         */
        void
        cb_balance_region(const pearl::CallbackManager& cbmanager,
                          int                           user_event,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cdata);

        std::set< std::string > registered_regions;
};
}    // namespace silas


#endif    // !SILAS_HYPBALANCE_H
