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


#ifndef SCOUT_ANALYSISHANDLER_H
#define SCOUT_ANALYSISHANDLER_H


#include "scout_types.h"


// --- Forward Declarations -------------------------------------------------


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class   AnalysisHandler
 * @ingroup scout
 * @brief   Abstract base class for pattern-orthogonal callback handlers
 */
/*-------------------------------------------------------------------------*/

class AnalysisHandler
{
    public:
        virtual
        ~AnalysisHandler()
        {
        }

        /// @name Callback registration
        /// @{

        virtual void
        register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
        {
        }

        virtual void
        register_post_pattern_callbacks(const CallbackManagerMap& cbm)
        {
        }

        /// @}
};
}    // namespace scout


// --- Helper macros

// *INDENT-OFF*    Uncrustify issue #1779
#define SCOUT_CALLBACK(cb)                       \
    void (cb)(const CallbackManager& cbmanager,  \
              int                    user_event, \
              const Event&           event,      \
              CallbackData*          cdata)
// *INDENT-ON*


#endif    // !SCOUT_ANALYSISHANDLER_H
