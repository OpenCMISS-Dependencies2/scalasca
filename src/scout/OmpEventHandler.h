/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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


#ifndef SCOUT_OMPEVENTHANDLER_H
#define SCOUT_OMPEVENTHANDLER_H


#include <pearl/ScopedPtr.h>

#include "AnalysisHandler.h"


// --- Forward declarations -------------------------------------------------

namespace pearl
{
class Event;
}    // namespace pearl


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class   OmpEventHandler
 * @ingroup scout
 * @brief   OpenMP event dispatch
 *
 * The OmpEventHandler handles OpenMP information (fork time, join time) and
 * event dispatch for fork, join, and barrier regions
 */
/*-------------------------------------------------------------------------*/

class OmpEventHandler
    : public AnalysisHandler
{
    class OmpEvtHImpl;

    const pearl::ScopedPtr< OmpEvtHImpl > mP;


    public:
        OmpEventHandler();

        ~OmpEventHandler();

        pearl::timestamp_t
        getForkJoinTime(const pearl::Event& e) const;

        void
        register_pre_pattern_callbacks(const CallbackManagerMap& cbm);

        void
        register_post_pattern_callbacks(const CallbackManagerMap& cbm);
};
}    // namespace scout


#endif    // !SCOUT_OMPEVENTHANDLER_H
