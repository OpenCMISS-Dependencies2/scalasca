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


#ifndef SCOUT_REPLAYCONTROLHANDLER_H
#define SCOUT_REPLAYCONTROLHANDLER_H


#include <pearl/ScopedPtr.h>

#include "AnalysisHandler.h"


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class   ReplayControlHandler
 * @ingroup scout
 * @brief   Basic settings for forward/backward replays
 *
 * The ReplayControlHandler updates handles collective and point-to-point
 * MPI message exchanges
 */
/*-------------------------------------------------------------------------*/

class ReplayControlHandler
    : public AnalysisHandler
{
    struct RepCHImpl;

    const pearl::ScopedPtr< RepCHImpl > rP;


    public:
        ReplayControlHandler();

        ~ReplayControlHandler();

        virtual void
        register_pre_pattern_callbacks(const CallbackManagerMap& cbm);
};
}    // namespace scout


#endif    // !SCOUT_REPLAYCONTROLHANDLER_H
