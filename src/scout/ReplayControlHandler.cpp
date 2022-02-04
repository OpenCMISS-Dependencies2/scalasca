/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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

#include "ReplayControlHandler.h"

#include <pearl/CallbackManager.h>

#include "Callstack.h"
#include "CbData.h"
#include "user_events.h"

using namespace scout;
using namespace pearl;


struct ReplayControlHandler::RepCHImpl
{
    //
    // --- callbacks -----------------------------------------------------
    //

    SCOUT_CALLBACK(cb_prepare_fw_replay)
    {
        CbData* data = static_cast< CbData* >(cdata);
        data->mCallstack->setUpdate(true);
    }

    SCOUT_CALLBACK(cb_prepare_bw_replay)
    {
        CbData* data = static_cast< CbData* >(cdata);
        data->mCallstack->setUpdate(false);
    }

    //
    // --- callback registration -----------------------------------------
    //

    void
    register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
    {
        struct cb_evt_table_t
        {
            pearl::event_t evt;
            void           (RepCHImpl::* cbp)(const pearl::CallbackManager&,
                                              int,
                                              const pearl::Event&,
                                              pearl::CallbackData*);
        };


        struct cb_uevt_table_t
        {
            int  evt;
            void (RepCHImpl::* cbp)(const pearl::CallbackManager&,
                                    int,
                                    const pearl::Event&,
                                    pearl::CallbackData*);
        };


        // --- main forward replay callbacks

        // *INDENT-OFF*
        const struct cb_uevt_table_t fw_uevt_tbl[] = {
            { PREPARE, &RepCHImpl::cb_prepare_fw_replay },

            { 0,       0                                }
        };
        // *INDENT-ON*


        // --- backward wait-state / synchpoint detection callbacks

        // *INDENT-OFF*
        const struct cb_uevt_table_t bw_uevt_tbl[] = {
            { PREPARE, &RepCHImpl::cb_prepare_bw_replay },

            { 0,       0                                }
        };
        // *INDENT-ON*


        // --- global callback tables table :)

        // *INDENT-OFF*
        const struct cb_tables_t
        {
            const char*                   stage;
            const struct cb_evt_table_t*  evts;
            const struct cb_uevt_table_t* uevts;
        }
        cb_tables[] = {
            { "",    0, fw_uevt_tbl },
            { "bws", 0, bw_uevt_tbl },
            { "fws", 0, fw_uevt_tbl },
            { "bwc", 0, bw_uevt_tbl },

            { 0,     0, 0           }
        };
        // *INDENT-ON*


        // --- register callbacks

        for (const struct cb_tables_t* t = cb_tables; t->stage; ++t)
        {
            CallbackManagerMap::const_iterator it = cbm.find(t->stage);

            assert(it != cbm.end());

            for (const struct cb_evt_table_t* c = t->evts; c && c->cbp; ++c)
            {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
            for (const struct cb_uevt_table_t* c = t->uevts; c && c->cbp; ++c)
            {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
        }
    }
};


ReplayControlHandler::ReplayControlHandler()
    : AnalysisHandler(),
      rP(new RepCHImpl)
{
}


ReplayControlHandler::~ReplayControlHandler()
{
}


void
ReplayControlHandler::register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
{
    rP->register_pre_pattern_callbacks(cbm);
}
