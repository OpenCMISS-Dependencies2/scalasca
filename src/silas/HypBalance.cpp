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


#include <config.h>

#include "HypBalance.h"

#include <mpi.h>

#include <pearl/Callpath.h>
#include <pearl/Event.h>
#include <pearl/Region.h>
#include <pearl/String.h>

#include "ReenactCallbackData.h"
#include "silas_events.h"
#include "silas_util.h"

using namespace silas;
using namespace std;


HypBalance::HypBalance()
{
}


HypBalance::~HypBalance()
{
}


void
HypBalance::register_callbacks(const int               run,
                               pearl::CallbackManager* cbmanager)
{
    if (  (run == 0)
       && !registered_regions.empty())
    {
        log("Registering balance hypothesis part for ATOMIC_REGION_EXIT\n");
        cbmanager->register_callback(ATOMIC_REGION_EXIT,
                                     PEARL_create_callback(this,
                                                           &HypBalance::cb_balance_region));
    }
}


void
HypBalance::cb_balance_region(const pearl::CallbackManager& cbmanager,
                              int                           user_event,
                              const pearl::Event&           event,
                              pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    double local_timespan, new_timespan;
    double sum_timespan;
    int    num_processes;

    /* check if current region is registered to be balanced */
    if (registered_regions.find(event.get_cnode()->getRegion().getDisplayName().getString())
        == registered_regions.end())
    {
        return;
    }

    /* calculate original timespan of current region */
    local_timespan = event->getTimestamp() - event.prev()->getTimestamp();

    MPI_Allreduce(&local_timespan, &sum_timespan, 1, MPI_DOUBLE, MPI_SUM,
                  MPI_COMM_WORLD);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    /* calculate balanced timespan */
    new_timespan = sum_timespan / num_processes;

    /* set new time */
    event->setTimestamp(event.prev()->getTimestamp() + new_timespan);
    data->set_delta_t(data->get_delta_t() + new_timespan - local_timespan);
}


void
HypBalance::register_region(string region_name)
{
    log("Registering region \"%s\" for balancing\n", region_name.c_str());
    registered_regions.insert(region_name);
}
