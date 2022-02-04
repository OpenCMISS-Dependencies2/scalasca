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


#ifndef SCOUT_TMAPCACHEHANDLER_H
#define SCOUT_TMAPCACHEHANDLER_H


#include <pearl/ScopedPtr.h>

#include "AnalysisHandler.h"
#include "scout_types.h"


// --- Forward declarations -------------------------------------------------

namespace pearl
{
class Event;
}    // namespace pearl


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class   TmapCacheHandler
 * @ingroup scout
 * @brief   Timemap cache
 *
 * The TmapCacheHandler collects and manages call-path time profiles between
 * synchronization points
 *
 */
/*-------------------------------------------------------------------------*/

class TmapCacheHandler
    : public AnalysisHandler
{
    struct TmapCHImpl;

    const pearl::ScopedPtr< TmapCHImpl > mP;


    public:
        TmapCacheHandler();

        ~TmapCacheHandler();

        timemap_t
        getRuntimes(const pearl::Event& from,
                    const pearl::Event& to) const;

        void
        register_pre_pattern_callbacks(const CallbackManagerMap& cbm);

        void
        register_post_pattern_callbacks(const CallbackManagerMap& cbm);
};
}    // namespace scout


#endif    // !SCOUT_TMAPCACHEHANDLER_H
