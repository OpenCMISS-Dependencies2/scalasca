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

#include "Simulator.h"

#include <pearl/CallbackManager.h>

#include "Hypothesis.h"
#include "Model.h"
#include "ReenactCallbackData.h"
#include "Timer.h"
#include "silas_events.h"
#include "silas_replay.h"
#include "silas_util.h"

using namespace std;
using namespace pearl;
using namespace silas;


/**
 * Create a simulator object with references on the event trace, a
 * pointer to the model, as well as the hypothesis.
 * @param trace Reference of the local event trace
 * @param model Pointer to the model used for simulation
 * @param hypothesis Pointer to the simulation hypothesis
 */
Simulator::Simulator(pearl::LocalTrace& trace,
                     Model*             model,
                     Hypothesis*        hypothesis)
    : m_trace(trace)
{
    /* initialize local objects */
    m_model      = model;
    m_hypothesis = hypothesis;
}


/**
 * Clean up model and hypothesis objects
 */
Simulator::~Simulator()
{
    /* free objects */
    delete m_model;
    delete m_hypothesis;
}


/**
 * Start the simulation.
 */
void
Simulator::run() const
{
    Timer timer;

    /* Setup callback data */
    silas::CallbackData          data(m_trace.get_definitions(), m_trace, *m_model);
    std::vector< Model::Config > runtime_config(m_model->get_run_configuration());

    for (unsigned run = 0; run < runtime_config.size(); run++)
    {
        pearl::CallbackManager m_cbmanager;

        m_model->register_callbacks(run, &m_cbmanager);
        m_hypothesis->register_callbacks(run, &m_cbmanager);

        log("%s ...\n", runtime_config[run].description);
        timer.start();

        m_cbmanager.notify(START, m_trace.begin(), &data);

        if (runtime_config[run].forward)
        {
            SILAS_forward_replay(m_trace, m_cbmanager, &data,
                                 runtime_config[run].mincbevent);
        }
        else
        {
            SILAS_backward_replay(m_trace, m_cbmanager, &data,
                                  runtime_config[run].mincbevent);
        }

        m_cbmanager.notify(FINISHED, m_trace.begin(), &data);

        log("                               done (%s).\n", timer.value_str().c_str());
    }
}
