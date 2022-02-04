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


#ifndef SILAS_HYPOTHESISPART_H
#define SILAS_HYPOTHESISPART_H


#include <pearl/CallbackManager.h>


namespace silas
{
/**
 * Base class for partial hypotheses
 */
class HypothesisPart
{
    public:
        /**
         * Destructor.
         */
        virtual
        ~HypothesisPart()
        {
        }

        /**
         * Register the needed callbacks to enable the management of
         * user defined modifications on specific events.
         */
        virtual void
        register_callbacks(const int               run,
                           pearl::CallbackManager* cb_manager) = 0;
};
}    // namespace silas


#endif    // !SILAS_HYPOTHESISPART_H
