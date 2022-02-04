/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_SYNCHPOINTHANDLER_H
#define SCOUT_SYNCHPOINTHANDLER_H


#include <pearl/ScopedPtr.h>

#include "AnalysisHandler.h"


// --- Forward declarations -------------------------------------------------

namespace pearl
{
class CommSet;
class Event;
class GlobalDefs;
}    // namespace pearl


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class   SynchpointHandler
 * @ingroup scout
 * @brief   Synchronization-point detection callback routines
 *
 * The SynchpointHandler handles synchronization-point detection
 * for collective and point-to-point MPI communication
 */
/*-------------------------------------------------------------------------*/

class SynchpointHandler
    : public AnalysisHandler
{
    class SpHImpl;

    const pearl::ScopedPtr< SpHImpl > mP;


    public:
        SynchpointHandler();

        ~SynchpointHandler();

        bool
        isSynchpoint(const pearl::Event& e) const;

        bool
        isWaitstate(const pearl::Event& e) const;

        SynchpointInfo
        getSynchpointInfo(const pearl::Event& e) const;

        pearl::Event
        findPreviousMpiSynchpoint(const pearl::Event& e,
                                  int                 rank) const;

        pearl::Event
        findPreviousOmpSynchpoint(const pearl::Event& e) const;

        // *INDENT-OFF*    Uncrustify issue #2757
        pearl::Event
        findPreviousMpiGroupSynchpoints(const pearl::Event&            from,
                                        const pearl::CommSet&          commSet,
                                        int                            rank,
                                        uint32_t                       nprocs,
                                        std::map< int, pearl::Event >& spmap) const;
        // *INDENT-ON*

        SynchpointInfoMap
        getSynchpoints() const;

        SynchpointInfoMap
        getSynchpointsBetween(const pearl::Event& from,
                              const pearl::Event& to) const;

        void
        register_pre_pattern_callbacks(const CallbackManagerMap& cbm);
};
}    // namespace scout


#endif    // !SCOUT_SYNCHPOINTHANDLER_H
