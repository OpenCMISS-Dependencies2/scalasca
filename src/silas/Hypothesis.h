/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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


#ifndef SILAS_HYPOTHESIS_H
#define SILAS_HYPOTHESIS_H


#include <vector>

#include <pearl/CallbackManager.h>


namespace silas
{
class HypothesisPart;


/**
 * A hypothesis is a set of smaller hypothesis parts each defining a
 * hypothetic behaviour of some kind. Each part defines its own callback
 * functions an the hypothesis serves as a container to ease callback
 * registration for the simulator.
 */
class Hypothesis
{
    public:
        /**
         * Default constructor.
         */
        Hypothesis();

        /**
         * Destructor.  Cleans up registered partial hypotheses.
         */
        virtual
        ~Hypothesis();

        /**
         * Add a new partial hypothesis.
         * @param part Pointer to new partial hypothesis
         */
        void
        add_part(HypothesisPart* part);

        /**
         * Register all callbacks of the included partial hypotheses
         * @param run       ID of the replay run the callbacks will
         *                  be registered for
         * @param cbmanager Reference of callback manager used for
         *                  registration
         */
        void
        register_callbacks(const int               run,
                           pearl::CallbackManager* cbmanager) const;


    private:
        /**
         * List of partial hypotheses
         */
        std::vector< HypothesisPart* > parts;
};
}    // namespace silas


#endif    // !SILAS_HYPOTHESIS_H
