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


#ifndef SILAS_SIMULATOR_H
#define SILAS_SIMULATOR_H


#include <pearl/LocalTrace.h>


namespace silas
{
class Hypothesis;
class Model;


/**
 * Core component of the simulator, responsible for execution of the
 * simulation.
 */
class Simulator
{
    public:
        /**
         * Construct a simulator object working on the given trace.
         * The trace object will be modified by the simulation.
         */
        Simulator(pearl::LocalTrace& trace,
                  Model*             model,
                  Hypothesis*        hypothesis);

        /**
         * Cleaning up simulator
         */
        ~Simulator();

        /**
         * Starting the simulator
         */
        void
        run() const;


    private:
        /** local event trace */
        pearl::LocalTrace& m_trace;
        /** model to be used */
        Model* m_model;
        /** hypothesis to be simulated */
        Hypothesis* m_hypothesis;
};
}    // namespace silas


#endif    // !SILAS_SIMULATOR_H
