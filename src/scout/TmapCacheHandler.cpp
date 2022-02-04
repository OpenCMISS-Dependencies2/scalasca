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


#include <config.h>

#include "TmapCacheHandler.h"

#include <pearl/CallbackManager.h>
#include <pearl/Event.h>
#include <pearl/EventSet.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalData.h>

#include "Callstack.h"
#include "CbData.h"
#include "user_events.h"

#ifdef _MPI
    #include "SynchpointHandler.h"
#endif    // _MPI
#ifdef _OPENMP
    #include <omp.h>

    #include "OmpEventHandler.h"
#endif    // _OPENMP

using namespace scout;
using namespace pearl;
using namespace std;


struct TmapCacheHandler::TmapCHImpl
{
    //
    // --- private data --------------------------------------------------
    //

    struct cnode_time_t
    {
        Callpath::IdType id;
        double           time;
    };


    typedef vector< cnode_time_t >
        tmapcache_vec_t;
    typedef map< Event, tmapcache_vec_t::size_type, EventKeyCompare >
        tmapcache_map_t;


    tmapcache_map_t mCacheIndices;
    tmapcache_vec_t mCache;

    timemap_t mTmap;

    double mLaststamp;


    //
    // --- constructor
    //

    TmapCHImpl()
        : mLaststamp(0)
    {
    }

    //
    // --- query interface
    //

    timemap_t
    getRuntimes(const Event& from,
                const Event& to) const
    {
        tmapcache_vec_t::const_iterator begin, end;

        if (from.is_valid())
        {
            tmapcache_map_t::const_iterator itf = mCacheIndices.find(from);

            assert(itf != mCacheIndices.end());

            begin = mCache.begin() + itf->second;
        }
        else
        {
            begin = mCache.begin();
        }

        if (to.is_valid())
        {
            tmapcache_map_t::const_iterator itt = mCacheIndices.find(to);

            assert(itt != mCacheIndices.end());

            end = mCache.begin() + itt->second;
        }
        else
        {
            end = mCache.end();
        }

        assert(begin <= end);

        timemap_t tmap;

        for (tmapcache_vec_t::const_iterator it = begin; it != end; ++it)
        {
            tmap[it->id] += it->time;
        }

        return tmap;
    }

    //
    // --- helper functions
    //

    void
    save_synchpoint(const Event& event)
    {
        // --- update tmap & cmap cache

        for (timemap_t::const_iterator it = mTmap.begin();
             it != mTmap.end();
             ++it)
        {
            cnode_time_t e = { it->first, it->second };

            mCache.push_back(e);
        }

        mCacheIndices.insert(std::make_pair(event, mCache.size()));

        // --- reset current tmap

        mTmap.clear();
    }

    //
    // --- fws replay callbacks
    //

    #ifdef _MPI
        SCOUT_CALLBACK(cb_fws_pre_mpi_synchpoint)
        {
            CbData* data = static_cast< CbData* >(cdata);

            if (!data->mSynchpointHandler->isSynchpoint(event))
            {
                return;
            }

            save_synchpoint(event);
        }

        SCOUT_CALLBACK(cb_fws_pre_recvreq)
        {
            // Save data on all receive requests (in case it is a late receiver synchpoint)
            // and delete non-synchpoint ones in receive completion handler

            save_synchpoint(event);
        }

        SCOUT_CALLBACK(cb_fws_pre_recvcomp)
        {
            CbData* data = static_cast< CbData* >(cdata);

            // Synchpoint info for receive requests in the case of late-receiver events
            // is only set on receive completion, so we need to check for that here and
            // remove non-synchpoint ones.
            // The receive completion event itself is already handled by cb_fws_pre_p2p.

            if (!data->mSynchpointHandler->isSynchpoint(event.request()))
            {
                mCacheIndices.erase(event.request());
            }
        }
    #endif    // _MPI

    #ifdef _OPENMP
        SCOUT_CALLBACK(cb_fws_omp_fork)
        {
            CbData* data = static_cast< CbData* >(cdata);

            // Fork time should be added to the parallel region itself, not the parent
            double    forktime = data->mOmpEventHandler->getForkJoinTime(event);
            Callpath* parent   = event.get_cnode()->getParent();

            if (parent)
            {
                mTmap[parent->getId()] -= forktime;
            }

            // Ignore idle process times for non-master OMP threads
            if (omp_get_thread_num() > 0)
            {
                mTmap.clear();
            }

            save_synchpoint(event);

            mTmap[event.get_cnode()->getId()] += forktime;
        }

        SCOUT_CALLBACK(cb_fws_omp_join)
        {
            CbData* data = static_cast< CbData* >(cdata);

            // Join time should be added to the parallel region itself, not the parent
            double    jointime = data->mOmpEventHandler->getForkJoinTime(event);
            Callpath* cnode    = event.get_cnode();
            Callpath* parent   = cnode->getParent();

            if (parent)
            {
                mTmap[parent->getId()] -= jointime;
            }

            mTmap[cnode->getId()] += jointime;
        }

        SCOUT_CALLBACK(cb_fws_omp_synchpoint)
        {
            save_synchpoint(event);
        }

        SCOUT_CALLBACK(cb_fws_omp_join_checkpoint)
        {
            const double   time = event->getTimestamp();
            const uint32_t cid  = event.get_cnode()->getId();

            mTmap[cid] += (time - mLaststamp);
            mLaststamp  = time;

            save_synchpoint(event);
        }
    #endif

    SCOUT_CALLBACK(cb_fws_finished)
    {
        #ifdef _OPENMP
            // Ignore idle process times for non-master OMP threads
            if (omp_get_thread_num() > 0)
            {
                mTmap.clear();
            }
        #endif    // _OPENMP

        // Just save last Tmap in cache
        for (timemap_t::const_iterator it = mTmap.begin();
             it != mTmap.end();
             ++it)
        {
            cnode_time_t e = { it->first, it->second };

            mCache.push_back(e);
        }
    }

    SCOUT_CALLBACK(cb_fws_pre_enter)
    {
        // update current tmap

        double    time   = event->getTimestamp();
        Callpath* parent = event.get_cnode()->getParent();

        if (parent)
        {
            mTmap[parent->getId()] += (time - mLaststamp);
        }

        mLaststamp = time;
    }

    SCOUT_CALLBACK(cb_fws_post_leave)
    {
        CbData* data = static_cast< CbData* >(cdata);

        double   time = event->getTimestamp();
        uint32_t cid  = data->mCallstack->top().get_cnode()->getId();

        // --- update current tmap

        mTmap[cid] += (time - mLaststamp);
        mLaststamp  = time;
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
            void           (TmapCHImpl::* cbp)(const pearl::CallbackManager&,
                                               int,
                                               const pearl::Event&,
                                               pearl::CallbackData*);
        };


        struct cb_uevt_table_t
        {
            int  evt;
            void (TmapCHImpl::* cbp)(const pearl::CallbackManager&,
                                     int,
                                     const pearl::Event&,
                                     pearl::CallbackData*);
        };


        // --- forward synchpoint detection callbacks

        // *INDENT-OFF*
        const struct cb_evt_table_t fws_evt_tbl[] = {
            { GROUP_ENTER,        &TmapCHImpl::cb_fws_pre_enter          },
          #ifdef _MPI
            { GROUP_SEND,         &TmapCHImpl::cb_fws_pre_mpi_synchpoint },
            { GROUP_RECV,         &TmapCHImpl::cb_fws_pre_mpi_synchpoint },
            { MPI_SEND_COMPLETE,  &TmapCHImpl::cb_fws_pre_mpi_synchpoint },
            { MPI_COLLECTIVE_END, &TmapCHImpl::cb_fws_pre_mpi_synchpoint },

            { MPI_RECV_REQUEST,   &TmapCHImpl::cb_fws_pre_recvreq        },
            { MPI_RECV_COMPLETE,  &TmapCHImpl::cb_fws_pre_recvcomp       },
          #endif

            { NUM_EVENT_TYPES,    0                                      }
        };
        // *INDENT-ON*


        // *INDENT-OFF*
        const struct cb_uevt_table_t fws_uevt_tbl[] = {
          #ifdef _MPI
            { INIT,        &TmapCHImpl::cb_fws_pre_mpi_synchpoint },
            { FINALIZE,    &TmapCHImpl::cb_fws_pre_mpi_synchpoint },
          #endif
          #ifdef _OPENMP
            { OMP_BARRIER, &TmapCHImpl::cb_fws_omp_synchpoint     },
          #endif
            { FINISHED,    &TmapCHImpl::cb_fws_finished           },

            { 0,           0                                      }
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
            { "fws", fws_evt_tbl, fws_uevt_tbl },

            { 0,     0,           0            }
        };
        // *INDENT-ON*


        // --- register callbacks

        for (const struct cb_tables_t* t = cb_tables; t->stage; ++t)
        {
            CallbackManagerMap::const_iterator it = cbm.find(t->stage);

            assert(it != cbm.end());

            for (const struct cb_evt_table_t* c = t->evts; c->cbp; ++c)
            {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
            for (const struct cb_uevt_table_t* c = t->uevts; c->cbp; ++c)
            {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
        }
    }

    void
    register_post_pattern_callbacks(const CallbackManagerMap& cbm)
    {
        struct cb_evt_table_t
        {
            pearl::event_t evt;
            void           (TmapCHImpl::* cbp)(const pearl::CallbackManager&,
                                               int,
                                               const pearl::Event&,
                                               pearl::CallbackData*);
        };


        struct cb_uevt_table_t
        {
            int  evt;
            void (TmapCHImpl::* cbp)(const pearl::CallbackManager&,
                                     int,
                                     const pearl::Event&,
                                     pearl::CallbackData*);
        };


        // --- forward synchpoint detection callbacks

        // *INDENT-OFF*
        const struct cb_evt_table_t fws_evt_tbl[] = {
            { GROUP_LEAVE,     &TmapCHImpl::cb_fws_post_leave          },
          #ifdef _OPENMP
            { THREAD_JOIN,     &TmapCHImpl::cb_fws_omp_join_checkpoint },
          #endif

            { NUM_EVENT_TYPES, 0                                       }
        };
        // *INDENT-ON*


        // *INDENT-OFF*
        const struct cb_uevt_table_t fws_uevt_tbl[] = {
          #ifdef _OPENMP
            // This callback is triggered on ENTER but must run after the
            // TmapCache ENTER handler, hence make this a post callback
            { OMP_MGMT_POST_FORK, &TmapCHImpl::cb_fws_omp_fork },

            // Doesn't matter if this is post or pre
            { OMP_MGMT_JOIN,      &TmapCHImpl::cb_fws_omp_join },
          #endif

            { 0,                  0                            }
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
            { "fws", fws_evt_tbl, fws_uevt_tbl },

            { 0,     0,           0            }
        };
        // *INDENT-ON*


        // --- register callbacks

        for (const struct cb_tables_t* t = cb_tables; t->stage; ++t)
        {
            CallbackManagerMap::const_iterator it = cbm.find(t->stage);

            assert(it != cbm.end());

            for (const struct cb_evt_table_t* c = t->evts; c->cbp; ++c)
            {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
            for (const struct cb_uevt_table_t* c = t->uevts; c->cbp; ++c)
            {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
        }
    }
};    // TmapCHImpl


TmapCacheHandler::TmapCacheHandler()
    : AnalysisHandler(),
      mP(new TmapCHImpl)
{
}


TmapCacheHandler::~TmapCacheHandler()
{
}


void
TmapCacheHandler::register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
{
    mP->register_pre_pattern_callbacks(cbm);
}


void
TmapCacheHandler::register_post_pattern_callbacks(const CallbackManagerMap& cbm)
{
    mP->register_post_pattern_callbacks(cbm);
}


timemap_t
TmapCacheHandler::getRuntimes(const Event& from,
                              const Event& to) const
{
    return mP->getRuntimes(from, to);
}
