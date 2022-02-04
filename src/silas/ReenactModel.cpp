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

#include "ReenactModel.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

#include <pearl/CallbackManager.h>
#include <pearl/Callpath.h>
#include <pearl/CommSet.h>
#include <pearl/LocalTrace.h>
#include <pearl/MpiComm.h>
#include <pearl/Region.h>
#include <pearl/String.h>

#include "Buffer.h"
#include "ReenactCallbackData.h"
#include "silas_events.h"
#include "silas_util.h"

using namespace std;
using namespace pearl;
using namespace silas;


extern int mpiRank;

static double noop_threshold = 0.;
static double skip_threshold = 0.;
static double idle_overhead  = 5e-07;

static double min_latency = 1e-08;

const std::string
ReenactModel::get_name() const
{
    return "Reenact Model";
}


std::vector< Model::Config >
ReenactModel::get_run_configuration() const
{
    // *INDENT-OFF*
    static const Model::Config configuration[] = {
        { true, "Hypothesis application    ", 0               },
        { true, "Idle time aggregation     ", 0               },
        { true, "Simulation                ", silas::INIT     },
        { true, "Postprocessing            ", silas::SKIP_END },
        { true, "Verification              ", 0               }
    };
    // *INDENT-ON*

    return std::vector< Model::Config >(&configuration[0], &configuration[4]);
}


void
ReenactModel::register_callbacks(const int               run,
                                 pearl::CallbackManager* cbmanager)
{
    switch (run)
    {
        case 0:    // Preprocessing
            {
                cbmanager->register_callback(silas::INIT,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_prepare));
                cbmanager->register_callback(silas::START,
                                             PEARL_create_callback(this, &ReenactModel::cb_start_prepare));
                cbmanager->register_callback(silas::FINISHED,
                                             PEARL_create_callback(this, &ReenactModel::cb_finish_prepare));
                break;
            }

        case 1:    // Idle time aggregation
            {
                cbmanager->register_callback(silas::START,
                                             PEARL_create_callback(this, &ReenactModel::cb_start_aggregate));

                /// @todo Check whether AGGREGATE_IGNORE can be removed entirely
                cbmanager->register_callback(silas::AGGREGATE_IGNORE,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::BEGIN,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::END,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::NOOP,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::IDLE,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::IDLE_WRITE,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::SEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::SSEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::BSEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::RECV,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::SENDRECV,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::ISEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::ISSEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::IBSEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::IRECV,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::WAIT,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::WAITALL,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::WAITANY,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::WAITSOME,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::TEST,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::TESTALL,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::TESTANY,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::TESTSOME,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::BARRIER,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::BCAST,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::ALLREDUCE,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::ALLGATHER,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::ALLTOALL,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::SCAN,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::EXSCAN,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::REDUCE,
                                             PEARL_create_callback(this, &ReenactModel::cb_any_aggregate));
                cbmanager->register_callback(silas::FINISHED,
                                             PEARL_create_callback(this, &ReenactModel::cb_finish_aggregate));
                break;
            }

        case 2:    // Simulation
            {
                cbmanager->register_callback(silas::START,
                                             PEARL_create_callback(this, &ReenactModel::cb_start));
                cbmanager->register_callback(silas::FINISHED,
                                             PEARL_create_callback(this, &ReenactModel::cb_finish));
                cbmanager->register_callback(silas::BEGIN,
                                             PEARL_create_callback(this, &ReenactModel::cb_begin));
                cbmanager->register_callback(silas::END,
                                             PEARL_create_callback(this, &ReenactModel::cb_end));

                /// @todo Check whether idle-time aggregation should always be used
                if (!is_set("aggregate idle"))
                {
                    cbmanager->register_callback(silas::NOOP,
                                                 PEARL_create_callback(this, &ReenactModel::cb_begin));
                }
                cbmanager->register_callback(silas::IDLE,
                                             PEARL_create_callback(this, &ReenactModel::cb_idle));
                cbmanager->register_callback(silas::IDLE_WRITE,
                                             PEARL_create_callback(this, &ReenactModel::cb_idle_write));
                cbmanager->register_callback(silas::SEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_send));
                cbmanager->register_callback(silas::SSEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_ssend));
                cbmanager->register_callback(silas::BSEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_bsend));
                cbmanager->register_callback(silas::RECV,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_recv));
                cbmanager->register_callback(silas::ISEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_isend));
                cbmanager->register_callback(silas::ISSEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_issend));
                cbmanager->register_callback(silas::IBSEND,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_ibsend));
                cbmanager->register_callback(silas::IRECV,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_irecv));
                cbmanager->register_callback(silas::MPISTART,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_start));
                cbmanager->register_callback(silas::WAIT,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_wait));
                cbmanager->register_callback(silas::WAITANY,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_waitany));
                cbmanager->register_callback(silas::WAITSOME,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_waitsome));
                cbmanager->register_callback(silas::WAITALL,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_waitall));
                cbmanager->register_callback(silas::TEST,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_test));
                cbmanager->register_callback(silas::TESTANY,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_testany));
                cbmanager->register_callback(silas::TESTSOME,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_testsome));
                cbmanager->register_callback(silas::TESTALL,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_testall));
                cbmanager->register_callback(silas::SENDRECV,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_sendrecv));
                cbmanager->register_callback(silas::BARRIER,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_barrier));
                cbmanager->register_callback(silas::BCAST,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_bcast));
                cbmanager->register_callback(silas::ALLREDUCE,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_allreduce));
                cbmanager->register_callback(silas::ALLGATHER,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_allgather));
                cbmanager->register_callback(silas::ALLTOALL,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_alltoall));
                cbmanager->register_callback(silas::SCAN,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_scan));
                #if HAVE(DECL_MPI_EXSCAN)
                    cbmanager->register_callback(silas::EXSCAN,
                                                 PEARL_create_callback(this, &ReenactModel::cb_mpi_exscan));
                #endif    // HAVE(DECL_MPI_EXSCAN)
                cbmanager->register_callback(silas::REDUCE,
                                             PEARL_create_callback(this, &ReenactModel::cb_mpi_reduce));
                break;
            }

        case 3:    // Postprocessing
            cbmanager->register_callback(silas::START,
                                         PEARL_create_callback(this, &ReenactModel::cb_start_postprocess));
            cbmanager->register_callback(silas::IDLE,
                                         PEARL_create_callback(this, &ReenactModel::cb_recalc));
            cbmanager->register_callback(silas::IDLE_WRITE,
                                         PEARL_create_callback(this, &ReenactModel::cb_recalc));
            cbmanager->register_callback(silas::NOOP,
                                         PEARL_create_callback(this, &ReenactModel::cb_recalc));
            cbmanager->register_callback(silas::SKIP_END,
                                         PEARL_create_callback(this, &ReenactModel::cb_recalc));
            cbmanager->register_callback(silas::IGNORE,
                                         PEARL_create_callback(this, &ReenactModel::cb_ignore_postprocess));
            break;

        case 4:    // Verification
            cbmanager->register_callback(silas::START,
                                         PEARL_create_callback(this, &ReenactModel::cb_start_verify));
            cbmanager->register_callback(silas::ANY,
                                         PEARL_create_callback(this, &ReenactModel::cb_any_verify));
            cbmanager->register_callback(silas::FINISHED,
                                         PEARL_create_callback(this, &ReenactModel::cb_finish_verify));
            break;
    }
}


// --- Preprocessing callbacks ----------------------------------------------

void
ReenactModel::cb_start_prepare(const pearl::CallbackManager& cbmanager,
                               int                           user_event,
                               const pearl::Event&           event,
                               pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    data->deactivate_action_list();

    data->set_delta_t(0.);

    const string idle_overhead_str = get_option("idle overhead");
    if (!idle_overhead_str.empty())
    {
        idle_overhead = strtod(idle_overhead_str.c_str(), NULL);
        log("IDLE Overhead: %e\n", idle_overhead);
    }
    const string noop_threshold_str = get_option("noop threshold");
    if (!noop_threshold_str.empty())
    {
        noop_threshold = strtod(noop_threshold_str.c_str(), NULL);
        log("NOOP threshold: %e\n", noop_threshold);
    }
    const string skip_threshold_str = get_option("skip threshold");
    if (!skip_threshold_str.empty())
    {
        skip_threshold = strtod(skip_threshold_str.c_str(), NULL);
        log("SKIP threshold: %e\n", skip_threshold);
    }
    const string min_latency_str = get_option("mininal latency");
    if (!min_latency_str.empty())
    {
        min_latency = strtod(min_latency_str.c_str(), NULL);
        log("Min. latency: %e\n", min_latency);
    }
    const string max_messagesize_str = get_option("max messagesize");
    if (!max_messagesize_str.empty())
    {
        log("Max. message size: %s\n", max_messagesize_str.c_str());
    }
}


void
ReenactModel::cb_finish_prepare(const pearl::CallbackManager& cbmanager,
                                int                           user_event,
                                const pearl::Event&           event,
                                pearl::CallbackData*          cdata)
{
    silas::CallbackData* data               = static_cast< silas::CallbackData* >(cdata);
    int                  local_max_count[2] = { data->get_max_count(), data->get_max_pt2pt_count() };
    int                  global_max_count[2];

    MPI_Allreduce(local_max_count, global_max_count, 2, MPI_INT,
                  MPI_MAX, MPI_COMM_WORLD);

    log("Max buffer size:       %d\n", global_max_count[0]);
    log("Max Pt2Pt buffer size: %d\n", global_max_count[1]);

    data->set_max_count(global_max_count[0]);
    data->set_max_pt2pt_count(global_max_count[1]);

    local_max_count[1] = data->get_max_buffered_count();
    if (local_max_count[1] > MPI_BSEND_OVERHEAD)
    {
        /* prepare for bufferend send in simulation */
        char* buffer = new char[local_max_count[1] + MPI_BSEND_OVERHEAD];
        MPI_Buffer_attach(buffer, local_max_count[1]);
    }

    /* preallocate dummy buffers */
    data->init_buffers();

    data->reset_action_idx();

    /* clear diff vector so push_back restarts at the beginning */
    data->clear_diff_vector();
}


void
ReenactModel::cb_any_prepare(const pearl::CallbackManager& cbmanager,
                             int                           user_event,
                             const pearl::Event&           event,
                             pearl::CallbackData*          cdata)
{
    silas::CallbackData* data       = static_cast< silas::CallbackData* >(cdata);
    std::string          region     = lowercase(event.get_cnode()->getRegion().getCanonicalName().getString());
    pearl::event_t       event_type = event->getType();

    /*
     * delete tracing overhead from mpi p2p wrappers between enter and
     * send, and recv and exit
     */
    if (is_set("compensate perturbation"))
    {
        /// @todo Replace magic number (from Jugene measurement)
        ///      with some better platform-specifc threshold
        data->set_delta_t(data->get_delta_t() + 1.05e-6);
    }

    // adjust timestamp to current shift
    event->setTimestamp(event->getTimestamp() + data->get_delta_t());

    // ----- Handling of hypothesis callbacks  --------------------------

    // If the region is atomic, we throw the corresponding signal
    if (  event->isOfType(GROUP_LEAVE)
       && event.prev()->isOfType(GROUP_ENTER))
    {
        cbmanager.notify(ATOMIC_REGION_EXIT, event, data);
    }

    // Check conditions for message surgeon

    if (event->isOfType(GROUP_LEAVE))
    {
        static const char* sendreg[] = {
            "mpi_send",  "mpi_ssend",  "mpi_bsend",
            "mpi_isend", "mpi_issend", "mpi_ibsend", 0
        };
        static const char* recvreg[] = {
            "mpi_recv", "mpi_irecv", 0
        };

        int si, ri;

        for (si = 0; sendreg[si] && (region != sendreg[si]); ++si)
        {
        }
        for (ri = 0; recvreg[ri] && (region != recvreg[ri]); ++ri)
        {
        }

        if (sendreg[si])
        {
            cbmanager.notify(SEND_EXIT, event, data);
        }
        if (recvreg[ri])
        {
            cbmanager.notify(RECV_EXIT, event, data);
        }
    }

    // ----- Selection of appropriate action ----------------------------
    if (data->get_action(event) != silas::DELETE)
    {
        struct regcbtbl_t
        {
            const char* region;
            int         action;
        };


        bool det  = is_set("deterministic");
        bool isai = is_set("aggregate idle");
        bool com  = has_completions(event);
        int  idle = silas::IDLE_WRITE; // isai ? silas::IDLE : silas::IDLE_WRITE;

        // *INDENT-OFF*
        const regcbtbl_t exitactions[] = {
            { "mpi_send",     silas::SEND                                                                     },
            { "mpi_ssend",    silas::SSEND                                                                    },
            { "mpi_bsend",    silas::BSEND                                                                    },
            { "mpi_isend",    silas::ISEND                                                                    },
            { "mpi_issend",   silas::ISSEND                                                                   },
            { "mpi_ibsend",   silas::IBSEND                                                                   },
            { "mpi_start",    silas::MPISTART                                                                 },
            { "mpi_startall", silas::MPISTART                                                                 },
            { "mpi_recv",     silas::RECV                                                                     },
            { "mpi_irecv",    silas::IRECV                                                                    },
            { "mpi_sendrecv", silas::SENDRECV                                                                 },
            { "mpi_wait",     com ? silas::WAIT                              : idle                           },
            { "mpi_waitany",  com ? (det ? silas::WAIT    : silas::WAITANY)  : idle                           },
            { "mpi_waitsome", com ? (det ? silas::WAITALL : silas::WAITSOME) : idle                           },
            { "mpi_waitall",  com ? silas::WAITALL                           : idle                           },
            { "mpi_test",     com ? silas::WAIT                              : (det ? idle : silas::TEST)     },
            { "mpi_testany",  com ? (det ? silas::WAIT    : silas::WAITANY)  : (det ? idle : silas::TESTANY)  },
            { "mpi_testsome", com ? (det ? silas::WAITALL : silas::WAITSOME) : (det ? idle : silas::TESTSOME) },
            { "mpi_testall",  com ? silas::WAITALL                           : (det ? idle : silas::TESTALL)  },

            { 0,              -1                                                                              }
        };
        // *INDENT-ON*

        // *INDENT-OFF*
        const regcbtbl_t reqtestedactions[] = {
            { "mpi_waitany",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_waitsome", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_test",     isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_testany",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_testsome", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_testall",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },

            { 0,              -1                                           }
        };
        // *INDENT-ON*

        // *INDENT-OFF*
        const regcbtbl_t recvreqactions[] = {
            { "mpi_irecv",    isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_start",    isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_startall", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },

            { 0,              -1                                           }
        };
        // *INDENT-ON*

        // *INDENT-OFF*
        const regcbtbl_t recvactions[] = {
            { "mpi_recv",     isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_sendrecv", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },

            { 0,              -1                                           }
        };
        // *INDENT-ON*

        // *INDENT-OFF*
        const regcbtbl_t recvcompactions[] = {
            { "mpi_wait",     isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_waitany",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_waitsome", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_waitall",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_test",     isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_testany",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_testsome", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_testall",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },

            { 0,              -1                                           }
        };
        // *INDENT-ON*

        // *INDENT-OFF*
        const regcbtbl_t sendactions[] = {
            { "mpi_send",     isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_ssend",    isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_bsend",    isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_sendrecv", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },

            { 0,              -1                                           }
        };
        // *INDENT-ON*

        // *INDENT-OFF*
        const regcbtbl_t sendreqactions[] = {
            { "mpi_isend",    isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_issend",   isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_ibsend",   isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_start",    isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_startall", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },

            { 0,              -1                                           }
        };
        // *INDENT-ON*

        // *INDENT-OFF*
        const regcbtbl_t sendcompactions[] = {
            { "mpi_wait",     isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_waitany",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_waitsome", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_waitall",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_test",     isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_testany",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_testsome", isai ? silas::AGGREGATE_IGNORE : silas::SKIP },
            { "mpi_testall",  isai ? silas::AGGREGATE_IGNORE : silas::SKIP },

            { 0,              -1                                           }
        };
        // *INDENT-ON*

        // *INDENT-OFF*
        const regcbtbl_t collexitactions[] = {
            { "mpi_barrier",   silas::BARRIER   },
            { "mpi_bcast",     silas::BCAST     },
            { "mpi_allreduce", silas::ALLREDUCE },
            { "mpi_allgather", silas::ALLGATHER },
            { "mpi_alltoall",  silas::ALLTOALL  },
            { "mpi_scan",      silas::SCAN      },
            { "mpi_exscan",    silas::EXSCAN    },
            { "mpi_reduce",    silas::REDUCE    },

            { 0,               -1               }
        };
        // *INDENT-ON*


        // *INDENT-OFF*
        const struct
        {
            pearl::event_t    event_type;
            const regcbtbl_t* cbtbl;
        }
        cbmap[] = {
            { pearl::GROUP_LEAVE,        exitactions      },
            { pearl::MPI_REQUEST_TESTED, reqtestedactions },
            { pearl::MPI_RECV_REQUEST,   recvreqactions   },
            { pearl::MPI_RECV_COMPLETE,  recvcompactions  },
            { pearl::MPI_RECV,           recvactions      },
            { pearl::MPI_SEND,           sendactions      },
            { pearl::MPI_SEND_REQUEST,   sendreqactions   },
            { pearl::MPI_SEND_COMPLETE,  sendcompactions  },
            { pearl::MPI_COLLECTIVE_END, collexitactions  },

            { pearl::GROUP_ALL,          0                }
        };
        // *INDENT-ON*


        int e, a = 0;

        for (e = 0;
             cbmap[e].cbtbl && event->isOfType(cbmap[e].event_type);
             ++e)
        {
        }

        if (cbmap[e].cbtbl)
        {
            for (a = 0;
                 cbmap[e].cbtbl[a].region && (region != cbmap[e].cbtbl[a].region);
                 ++a)
            {
            }
        }

        if (cbmap[e].cbtbl && cbmap[e].cbtbl[a].region)
        {
            data->set_action(cbmap[e].cbtbl[a].action, event);
            /* if nothing of the above applies, the time between the previous
             * and the current event needs to be waited */
        }
        else
        {
            if (event.prev().is_valid())
            {
                if (event.next().is_valid())
                {
                    if ((data->get_original_timestamp()
                         - data->get_preceeding_timestamp()) < noop_threshold)
                    {
                        data->set_action(silas::NOOP, event);
                    }
                    else
                    {
                        data->set_action(idle, event);
                    }
                }
                else
                {
                    data->set_action(silas::END, event);
                }
            }
            else
            {
                data->set_action(silas::BEGIN, event);
            }
        }
    }

    if (event_type == MPI_RECV_REQUEST)
    {
        data->inc_nb_recv_count();
    }
    if (event_type == MPI_RECV_COMPLETE)
    {
        data->dec_nb_recv_count();
    }

    if (event_type == MPI_SEND)
    {
        data->track_max_pt2pt_count(event->getBytesSent());

        if (region == "mpi_bsend")
        {
            data->track_max_buffered_count(event->getBytesSent());
        }
    }

    if (event_type == MPI_SEND_REQUEST)
    {
        data->track_max_pt2pt_count(event->getBytesSent());

        if (region == "mpi_ibsend")
        {
            data->track_max_buffered_count(event->getBytesSent());
        }

        data->get_uncomplete_requests().insert(event->getRequestId());
    }

    if (event->isOfType(GROUP_LEAVE))
    {
        //
        // determine max request array size and
        // update uncompleted request set
        //

        static const char* mcregtbl[] = {
            "mpi_wait", "mpi_waitany", "mpi_waitall", "mpi_waitsome",
            "mpi_test", "mpi_testany", "mpi_testall", "mpi_testsome", 0
        };

        int i = 0;
        for ( ; mcregtbl[i] && (region != mcregtbl[i]); ++i)
        {
        }

        if (mcregtbl[i])
        {
            pearl::Event ev_it = event.prev();
            int          count = 0;

            while (!ev_it->isOfType(GROUP_ENTER))
            {
                data->get_uncomplete_requests().erase(ev_it->getRequestId());

                ++count;
                --ev_it;
            }

            data->track_max_mult_comp_count(count);
        }
    }

    if (event_type == MPI_COLLECTIVE_END)
    {
        size_t scount   = event->getBytesSent();
        size_t rcount   = event->getBytesReceived();
        size_t commsize = event->getComm()->getSize();

        // *INDENT-OFF*
        const struct size_tbl_t
        {
            const char* region;
            size_t      count;
        }
        size_tbl[] = {
            { "mpi_allgather", scount / commsize                   },
            { "mpi_allreduce", scount / commsize                   },
            { "mpi_alltoall",  scount / commsize                   },
            { "mpi_bcast",     rcount                              },
            { "mpi_exscan",    std::max(scount, rcount) / commsize },
            { "mpi_gather",    scount                              },
            { "mpi_reduce",    scount                              },
            { "mpi_scan",      std::max(scount, rcount) / commsize },

            { 0,               0                                   }
        };
        // *INDENT-ON*


        for (const size_tbl_t* stptr = size_tbl; stptr->region; ++stptr)
        {
            if (region == stptr->region)
            {
                data->track_max_count(stptr->count);
                break;
            }
        }
    }
}


void
ReenactModel::cb_mpi_send_prepare(const pearl::CallbackManager& cbmanager,
                                  int                           user_event,
                                  const pearl::Event&           event,
                                  pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    data->track_max_pt2pt_count(event->getBytesSent());
    if (lowercase(event.get_cnode()->getRegion().getCanonicalName().getString()) == "mpi_bsend")
    {
        data->track_max_buffered_count(event->getBytesSent());
    }
}


void
ReenactModel::cb_mpi_collective_prepare(const pearl::CallbackManager& cbmanager,
                                        int                           user_event,
                                        const pearl::Event&           event,
                                        pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    data->track_max_count(event->getBytesSent());
    data->track_max_count(event->getBytesReceived());
}


// --- Aggregation callbacks ------------------------------------------------

void
ReenactModel::cb_start_aggregate(const pearl::CallbackManager& cbmanager,
                                 int                           user_event,
                                 const pearl::Event&           event,
                                 pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    // use action list for this run
    data->activate_action_list();
    data->reset_action_idx();
    data->set_delta_t(0.);
}


void
ReenactModel::cb_any_aggregate(const pearl::CallbackManager& cbmanager,
                               int                           user_event,
                               const pearl::Event&           event,
                               pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    if (data->is_aggregatable(event))
    {
        // track event timespan to previous event
        data->track_idle_time(event);
    }
    else
    {
        // event is not aggregatable

        pearl::Event pe = event.prev();

        if (pe.is_valid() && data->is_aggregatable(pe))
        {
            // previous event was aggregatable, so this is the end of
            // the aggregation epoch
            data->aggregate_idle_time(event);
        }
    }
}


void
ReenactModel::cb_aggregate_ignore(const pearl::CallbackManager& cbmanager,
                                  int                           user_event,
                                  const pearl::Event&           event,
                                  pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    if (data->is_aggregatable(event.prev()))
    {
        // previous event was aggregatable, so this is the end of
        // the aggregation epoch
        data->aggregate_idle_time(event);
    }

    // set to skip for simulation step
    data->set_action(silas::SKIP, event);
}


void
ReenactModel::cb_finish_aggregate(const pearl::CallbackManager& cbmanager,
                                  int                           user_event,
                                  const pearl::Event&           event,
                                  pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    // data->reverse_aggr_time();
    data->reset_action_idx();

    /* clear diff vector so push_back restarts at the beginning */
    data->clear_diff_vector();

    // prepare list of ratio blocks for easy handling
    data->reverse_ratio_list();
    data->reverse_epoch_list();
}


// --- Simulation callbacks -------------------------------------------------

void
ReenactModel::cb_start(const pearl::CallbackManager& cbmanager,
                       int                           user_event,
                       const pearl::Event&           event,
                       pearl::CallbackData*          cdata)
{
    silas::CallbackData* data       = static_cast< silas::CallbackData* >(cdata);
    double               local_time = event->getTimestamp();
    double               min_time;
    double               current_time;

    data->activate_action_list();

    /* reset delta_t */
    data->set_delta_t(0.);

    /* get minimal starting time */
    MPI_Allreduce(&local_time, &min_time, 1, MPI_DOUBLE, MPI_MIN,
                  MPI_COMM_WORLD);
    /* sync all processes */
    MPI_Barrier(MPI_COMM_WORLD);
    /* set reference timestamp */
    current_time = get_wtime() - min_time;
    /* make reference time global */
    MPI_Allreduce(&current_time, &reference_timestamp, 1, MPI_DOUBLE, MPI_MIN,
                  MPI_COMM_WORLD);
    /* wait for process start */
    simulate_timespan(local_time - min_time);
}


void
ReenactModel::cb_finish(const pearl::CallbackManager& cbmanager,
                        int                           user_event,
                        const pearl::Event&           event,
                        pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    data->reset_action_idx();
}


void
ReenactModel::cb_any(const pearl::CallbackManager& cbmanager,
                     int                           user_event,
                     const pearl::Event&           event,
                     pearl::CallbackData*          cdata)
{
    silas::CallbackData* data   = static_cast< silas::CallbackData* >(cdata);
    int                  signal = data->get_current_action();

    // log("cb_any: %i|%i action is %i\n", event.get_id(), event->getType(), signal);
    if (signal != silas::NOOP)
    {
        cbmanager.notify(signal, event, data);
    }

    // else
    //     event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_end(const pearl::CallbackManager& cbmanager,
                     int                           user_event,
                     const pearl::Event&           event,
                     pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    // the preceeding NOOP or IDLE did not save the time yet
    event.prev()->setTimestamp(get_wtime() - reference_timestamp);

    // simulate time to last event
    simulate_timespan(data->get_original_timestamp() - data->get_preceeding_timestamp());
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_begin(const pearl::CallbackManager& cbmanager,
                       int                           user_event,
                       const pearl::Event&           event,
                       pearl::CallbackData*          cdata)
{
    /* do nothing */
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_idle_write(const pearl::CallbackManager& cbmanager,
                            int                           user_event,
                            const pearl::Event&           event,
                            pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    simulate_timespan(data->get_original_timestamp() - data->get_preceeding_timestamp());
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_idle(const pearl::CallbackManager& cbmanager,
                      int                           user_event,
                      const pearl::Event&           event,
                      pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    simulate_timespan(data->get_original_timestamp() - data->get_preceeding_timestamp());
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_ssend(const pearl::CallbackManager& cbmanager,
                           int                           user_event,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event prev  = event.prev();
    MpiComm*     comm  = prev->getComm();
    int          count = std::min(static_cast< int >(prev->getBytesSent()),
                                  data->get_max_pt2pt_count());
    void*    buf     = data->get_p2p_send_buffer(count);
    int      dest    = prev->getDestination();
    int      tag     = prev->getTag();
    MPI_Comm mpicomm = comm->getHandle();

    // set send event timestamp
    prev->setTimestamp(get_wtime() - reference_timestamp);

    // send message
    MPI_Ssend(buf, count, MPI_BYTE, dest, tag, mpicomm);

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_bsend(const pearl::CallbackManager& cbmanager,
                           int                           user_event,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event prev  = event.prev();
    MpiComm*     comm  = prev->getComm();
    int          count = std::min(static_cast< int >(prev->getBytesSent()),
                                  data->get_max_pt2pt_count());
    void*    buf     = data->get_p2p_send_buffer(count);
    int      dest    = prev->getDestination();
    int      tag     = prev->getTag();
    MPI_Comm mpicomm = comm->getHandle();

    // set send event timestamp, which was previously ignored
    prev->setTimestamp(get_wtime() - reference_timestamp);

    // send message
    MPI_Bsend(buf, count, MPI_BYTE, dest, tag, mpicomm);

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_send(const pearl::CallbackManager& cbmanager,
                          int                           user_event,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event prev  = event.prev();
    MpiComm*     comm  = prev->getComm();
    int          count = std::min(static_cast< int >(prev->getBytesSent()),
                                  data->get_max_pt2pt_count());
    void*    buf     = data->get_p2p_send_buffer(count);
    int      dest    = prev->getDestination();
    int      tag     = prev->getTag();
    MPI_Comm mpicomm = comm->getHandle();

    // set send event timestamp
    prev->setTimestamp(get_wtime() - reference_timestamp);

    // send message
    MPI_Send(buf, count, MPI_BYTE, dest, tag, mpicomm);

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_isend(const pearl::CallbackManager& cbmanager,
                           int                           user_event,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event prev  = event.prev();
    MpiComm*     comm  = prev->getComm();
    int          count = std::min(static_cast< int >(prev->getBytesSent()),
                                  data->get_max_pt2pt_count());
    void*    buf       = data->get_p2p_send_buffer(count);
    int      dest      = prev->getDestination();
    int      tag       = prev->getTag();
    uint64_t requestId = prev->getRequestId();
    MPI_Comm mpicomm   = comm->getHandle();

    RequestInfo reqinfo;
    reqinfo.buffer = 0;

    // set send event timestamp
    prev->setTimestamp(get_wtime() - reference_timestamp);

    // send message
    MPI_Isend(buf, count, MPI_BYTE, dest, tag, mpicomm, &reqinfo.request);

    if (data->get_uncomplete_requests().count(requestId))
    {
        MPI_Request_free(&reqinfo.request);
    }
    else
    {
        data->get_requestmap().insert(std::make_pair(requestId, reqinfo));
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_issend(const pearl::CallbackManager& cbmanager,
                            int                           user_event,
                            const pearl::Event&           event,
                            pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event prev  = event.prev();
    MpiComm*     comm  = prev->getComm();
    int          count = std::min(static_cast< int >(prev->getBytesSent()),
                                  data->get_max_pt2pt_count());
    void*    buf       = data->get_p2p_send_buffer(count);
    int      dest      = prev->getDestination();
    int      tag       = prev->getTag();
    uint64_t requestId = prev->getRequestId();
    MPI_Comm mpicomm   = comm->getHandle();

    RequestInfo reqinfo;
    reqinfo.buffer = 0;

    // set send event timestamp
    prev->setTimestamp(get_wtime() - reference_timestamp);

    // send message
    MPI_Issend(buf, count, MPI_BYTE, dest, tag, mpicomm, &reqinfo.request);

    if (data->get_uncomplete_requests().count(requestId))
    {
        MPI_Request_free(&reqinfo.request);
    }
    else
    {
        data->get_requestmap().insert(std::make_pair(requestId, reqinfo));
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_ibsend(const pearl::CallbackManager& cbmanager,
                            int                           user_event,
                            const pearl::Event&           event,
                            pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event prev  = event.prev();
    MpiComm*     comm  = prev->getComm();
    int          count = std::min(static_cast< int >(prev->getBytesSent()),
                                  data->get_max_pt2pt_count());
    void*    buf       = data->get_p2p_send_buffer(count);
    int      dest      = prev->getDestination();
    int      tag       = prev->getTag();
    uint64_t requestId = prev->getRequestId();
    MPI_Comm mpicomm   = comm->getHandle();

    RequestInfo reqinfo;
    reqinfo.buffer = 0;

    // set send event timestamp
    prev->setTimestamp(get_wtime() - reference_timestamp);

    // send message
    MPI_Ibsend(buf, count, MPI_BYTE, dest, tag, mpicomm, &reqinfo.request);

    if (data->get_uncomplete_requests().count(requestId))
    {
        MPI_Request_free(&reqinfo.request);
    }
    else
    {
        data->get_requestmap().insert(std::make_pair(requestId, reqinfo));
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_recv(const pearl::CallbackManager& cbmanager,
                          int                           user_event,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event recv = event.prev();

    MpiComm* comm    = recv->getComm();
    int      source  = recv->getSource();
    int      count   = data->get_max_pt2pt_count();
    void*    buf     = data->get_recv_buffer(count);
    int      tag     = recv->getTag();
    MPI_Comm mpicomm = comm->getHandle();

    // receive message
    MPI_Recv(buf, count, MPI_BYTE, source, tag, mpicomm, MPI_STATUS_IGNORE);

    // set receive event timestamp
    recv->setTimestamp(get_wtime() - reference_timestamp);

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_irecv(const pearl::CallbackManager& cbmanager,
                           int                           user_event,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event recvreq = event.prev();
    pearl::Event comp    = recvreq.completion();

    MpiComm* comm      = comp->getComm();
    int      source    = comp->getSource();
    int      count     = data->get_max_pt2pt_count();
    int      tag       = comp->getTag();
    uint64_t requestId = recvreq->getRequestId();
    MPI_Comm mpicomm   = comm->getHandle();

    RequestInfo reqinfo;
    reqinfo.buffer = data->pop_nb_recv_buffer();

    // receive message
    MPI_Irecv(reqinfo.buffer->get_buffer(count),
              count, MPI_BYTE, source, tag, mpicomm, &reqinfo.request);

    data->get_requestmap().insert(std::make_pair(requestId, reqinfo));

    // set receive event timestamp
    recvreq->setTimestamp(get_wtime() - reference_timestamp);

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_start(const pearl::CallbackManager& cbmanager,
                           int                           user_event,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cdata)
{
    silas::CallbackData* data  = static_cast< silas::CallbackData* >(cdata);
    pearl::Event         ev_it = event.prev();

    while (!ev_it->isOfType(GROUP_ENTER))
    {
        RequestInfo reqinfo;

        reqinfo.request = MPI_REQUEST_NULL;
        reqinfo.buffer  = 0;
        uint64_t requestId = ev_it->getRequestId();

        ev_it->setTimestamp(get_wtime() - reference_timestamp);

        // process send or receive event

        if (ev_it->isOfType(GROUP_SEND))
        {
            MpiComm* comm  = ev_it->getComm();
            int      count = std::min(static_cast< int >(ev_it->getBytesSent()),
                                      data->get_max_pt2pt_count());
            int      dest    = ev_it->getDestination();
            void*    buf     = data->get_p2p_send_buffer(count);
            int      tag     = ev_it->getTag();
            MPI_Comm mpicomm = comm->getHandle();

            MPI_Isend(buf, count, MPI_BYTE, dest, tag, mpicomm, &reqinfo.request);
        }
        else if (ev_it->isOfType(MPI_RECV_REQUEST))
        {
            pearl::Event comp = ev_it.completion();

            MpiComm* comm    = comp->getComm();
            int      count   = data->get_max_pt2pt_count();
            int      source  = comp->getSource();
            int      tag     = comp->getTag();
            MPI_Comm mpicomm = comm->getHandle();
            reqinfo.buffer = data->pop_nb_recv_buffer();

            MPI_Irecv(reqinfo.buffer->get_buffer(count),
                      count, MPI_BYTE, source, tag, mpicomm, &reqinfo.request);
        }

        if (reqinfo.request == MPI_REQUEST_NULL)
        {
            std::cerr << "Warning: invalid event type in MPI_Start* call: "
                      << ev_it << std::endl;
        }

        if (data->get_uncomplete_requests().count(requestId))
        {
            MPI_Request_free(&reqinfo.request);
        }
        else
        {
            data->get_requestmap().insert(std::make_pair(requestId, reqinfo));
        }

        --ev_it;
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_sendrecv(const pearl::CallbackManager& cbmanager,
                              int                           user_event,
                              const pearl::Event&           event,
                              pearl::CallbackData*          cdata)
{
    silas::CallbackData* data            = static_cast< silas::CallbackData* >(cdata);
    int                  max_pt2pt_count = data->get_max_pt2pt_count();

    pearl::Event recv = event.prev();
    pearl::Event send = recv.prev();

    MpiComm* comm    = recv->getComm();
    int      scount  = std::min(static_cast< int >(send->getBytesSent()), max_pt2pt_count);
    void*    sbuf    = data->get_p2p_send_buffer(scount);
    int      dest    = send->getDestination();
    int      stag    = send->getTag();
    void*    rbuf    = data->get_recv_buffer(max_pt2pt_count);
    int      rcount  = max_pt2pt_count;
    int      source  = recv->getSource();
    int      rtag    = recv->getTag();
    MPI_Comm mpicomm = comm->getHandle();

    // set send event timestamp
    send->setTimestamp(get_wtime() - reference_timestamp);

    // start transfers
    MPI_Sendrecv(sbuf, scount, MPI_BYTE, dest, stag,
                 rbuf, rcount, MPI_BYTE, source, rtag,
                 mpicomm, MPI_STATUS_IGNORE);

    // set receive event timestamp
    recv->setTimestamp(get_wtime() - reference_timestamp);

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_wait(const pearl::CallbackManager& cbmanager,
                          int                           user_event,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    // Find completion event.
    // In deterministic mode, cb_mpi_wait is also called for waitany/testany,
    // so we may need to parse over non-completion events.
    pearl::Event comp = event.prev();

    while (comp->isOfType(MPI_REQUEST_TESTED))
    {
        --comp;
    }

    silas::CallbackData::requestmap_t&          reqmap = data->get_requestmap();
    silas::CallbackData::requestmap_t::iterator it     = reqmap.find(comp->getRequestId());

    // comp.enterptr()->setTimestamp(get_wtime() - reference_timestamp);

    if (it != reqmap.end())
    {
        MPI_Wait(&(it->second.request), MPI_STATUS_IGNORE);

        data->push_nb_recv_buffer(it->second.buffer);
        reqmap.erase(it);

        // set wait event timestamp
        comp->setTimestamp(get_wtime() - reference_timestamp);
    }
    else
    {
        // if the event's reqid is not in the request map, the request was deleted.
        // store -1 timestamp to indicate that.
        comp->setTimestamp(-1);
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_waitany(const pearl::CallbackManager& cbmanager,
                             int                           user_event,
                             const pearl::Event&           event,
                             pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MPI_Request*                                 reqarray = data->get_request_array();
    silas::CallbackData::requestmap_t&           reqmap   = data->get_requestmap();
    silas::CallbackData::requestmap_t::iterator* rmiarray = data->get_requestmap_iter_array();
    silas::CallbackData::requestmap_t::iterator  compit   = reqmap.end();

    pearl::Event compev;
    pearl::Event ev_it = event.prev();
    int          count = 0;

    // parse backwards through completion events and fill request array
    // Note: the request array is stored reversed compared to the original run.

    while (!ev_it->isOfType(GROUP_ENTER))
    {
        silas::CallbackData::requestmap_t::iterator reqit = reqmap.find(ev_it->getRequestId());

        if (reqit != reqmap.end())
        {
            reqit->second.event = ev_it;

            reqarray[count] = reqit->second.request;
            rmiarray[count] = reqit;

            if (  ev_it->isOfType(GROUP_RECV)
               || ev_it->isOfType(MPI_SEND_COMPLETE))
            {
                compev = ev_it;
                compit = reqit;
            }

            ++count;
        }
        else
        {
            // if the event's reqid is not in the request map, the request was deleted.
            // store -1 timestamp to indicate that.
            ev_it->setTimestamp(-1);
        }

        --ev_it;
    }

    int index;
    MPI_Waitany(count, reqarray, &index, MPI_STATUS_IGNORE);

    data->push_nb_recv_buffer(rmiarray[index]->second.buffer);

    if (count)
    {
        if (compit == reqmap.end())
        {
            reqmap.erase(rmiarray[index]);
        }
        else
        {
            // remap request ids
            rmiarray[index]->second.event.completion()->setRequestId(compit->first);
            compev->setRequestId(rmiarray[index]->first);

            // swap completion event representations
            data->get_trace().swap(rmiarray[index]->second.event.completion(), compev);

            // remap MPI request objects
            rmiarray[index]->second = compit->second;
            reqmap.erase(compit);
        }

        // parse forward through completion events and store timestamps
        double time = get_wtime();

        for (++ev_it; !ev_it->isOfType(GROUP_LEAVE); ++ev_it)
        {
            if (ev_it->getTimestamp() > 0)
            {
                ev_it->setTimestamp(time - reference_timestamp);
            }
        }
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_waitsome(const pearl::CallbackManager& cbmanager,
                              int                           user_event,
                              const pearl::Event&           event,
                              pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MPI_Request*                                 reqarray = data->get_request_array();
    silas::CallbackData::requestmap_t&           reqmap   = data->get_requestmap();
    silas::CallbackData::requestmap_t::iterator* rmiarray = data->get_requestmap_iter_array();

    std::vector< EventInfo > compevs;
    EventInfo                compev;

    pearl::Event ev_it = event.prev();
    int          count = 0;

    // parse backwards through completion events and fill request array
    // Note: the request array is stored reversed compared to the original run.

    while (!ev_it->isOfType(GROUP_ENTER))
    {
        silas::CallbackData::requestmap_t::iterator reqit = reqmap.find(ev_it->getRequestId());

        if (reqit != reqmap.end())
        {
            reqit->second.event = ev_it;

            reqarray[count] = reqit->second.request;
            rmiarray[count] = reqit;

            if (  ev_it->isOfType(GROUP_RECV)
               || ev_it->isOfType(MPI_SEND_COMPLETE))
            {
                compev.ev = ev_it;
                compev.it = reqit;
                compevs.push_back(compev);
            }

            ++count;
        }
        else
        {
            // if the event's reqid is not in the request map, the request was deleted.
            // store -1 timestamp to indicate that.
            ev_it->setTimestamp(-1);
        }

        --ev_it;
    }

    int  ndone;
    int* indices = new int[count];

    MPI_Waitsome(count, reqarray, &ndone, indices, MPI_STATUSES_IGNORE);

    if (count)
    {
        int ntracedone = compevs.size();

        for (int i = 0; (i < ndone) && (i < ntracedone); ++i)
        {
            silas::CallbackData::requestmap_t::iterator rit = rmiarray[indices[i]];

            data->push_nb_recv_buffer(rit->second.buffer);

            // remap request ids
            rit->second.event.completion()->setRequestId(compevs[i].it->first);
            compevs[i].ev->setRequestId(rit->first);

            // swap completion event representations
            data->get_trace().swap(rit->second.event.completion(), compevs[i].ev);

            // remap MPI request objects
            rit->second = compevs[i].it->second;
            reqmap.erase(compevs[i].it);
        }

        // more completions in sim: predraw additional completions
        for (int i = ntracedone; i < ndone; ++i)
        {
            silas::CallbackData::requestmap_t::iterator rit = rmiarray[indices[i]];

            predraw_completion(data, rit->second.event);

            data->push_nb_recv_buffer(rit->second.buffer);
            reqmap.erase(rit);
        }

        // more completions in trace: wait for missing completions
        for (int i = ndone; i < ntracedone; ++i)
        {
            silas::CallbackData::requestmap_t::iterator rit = compevs[i].it;

            MPI_Wait(&(rit->second.request), MPI_STATUS_IGNORE);

            data->push_nb_recv_buffer(rit->second.buffer);
            reqmap.erase(rit);
        }

        // parse forward through completion events and store timestamps
        double time = get_wtime();

        for (++ev_it; !ev_it->isOfType(GROUP_LEAVE); ++ev_it)
        {
            if (ev_it->getTimestamp() > 0)
            {
                ev_it->setTimestamp(time - reference_timestamp);
            }
        }
    }

    delete[] indices;

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_waitall(const pearl::CallbackManager& cbmanager,
                             int                           user_event,
                             const pearl::Event&           event,
                             pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MPI_Request*                       reqarray  = data->get_request_array();
    MPI_Status*                        statarray = data->get_status_array();
    silas::CallbackData::requestmap_t& reqmap    = data->get_requestmap();

    pearl::Event ev_it = event.prev();
    int          count = 0;

    // parse backwards through completion events and fill request array
    // Note: the request array is stored reversed compared to the original run.

    while (!ev_it->isOfType(GROUP_ENTER))
    {
        // in deterministic mode, WAITALL is also called for WAITSOME/TESTSOME,
        // so skip REQUEST_TESTED events
        if (ev_it->isOfType(MPI_REQUEST_TESTED))
        {
            --ev_it;
            continue;
        }

        silas::CallbackData::requestmap_t::iterator reqit = reqmap.find(ev_it->getRequestId());

        if (reqit != reqmap.end())
        {
            reqarray[count] = reqit->second.request;

            data->push_nb_recv_buffer(reqit->second.buffer);
            reqmap.erase(reqit);

            ++count;
        }
        else
        {
            // if the event's reqid is not in the request map, the request was deleted.
            // store -1 timestamp to indicate that.
            ev_it->setTimestamp(-1);
        }

        --ev_it;
    }

    // ev_it->setTimestamp(get_wtime() - reference_timestamp);

    MPI_Waitall(count, reqarray, statarray);

    if (count)
    {
        // parse forward through completion events and store timestamps
        double time = get_wtime();

        for (++ev_it; !ev_it->isOfType(GROUP_LEAVE); ++ev_it)
        {
            if (ev_it->getTimestamp() > 0)
            {
                ev_it->setTimestamp(time - reference_timestamp);
            }
        }
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_test(const pearl::CallbackManager& cbmanager,
                          int                           user_event,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event req_ev = event.prev();

    silas::CallbackData::requestmap_t&          reqmap = data->get_requestmap();
    silas::CallbackData::requestmap_t::iterator it     = reqmap.find(req_ev->getRequestId());

    if (it != reqmap.end())
    {
        int flag = 0;

        MPI_Test(&(it->second.request), &flag, MPI_STATUS_IGNORE);

        if (flag)
        {
            predraw_completion(data, req_ev);

            data->push_nb_recv_buffer(it->second.buffer);
            reqmap.erase(it);
        }

        // set wait event timestamp
        req_ev->setTimestamp(get_wtime() - reference_timestamp);
    }
    else
    {
        // if the event's reqid is not in the request map, the request was deleted.
        // store -1 timestamp to indicate that.
        req_ev->setTimestamp(-1);
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_testany(const pearl::CallbackManager& cbmanager,
                             int                           user_event,
                             const pearl::Event&           event,
                             pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MPI_Request*                                 reqarray = data->get_request_array();
    silas::CallbackData::requestmap_t&           reqmap   = data->get_requestmap();
    silas::CallbackData::requestmap_t::iterator* rmiarray = data->get_requestmap_iter_array();

    pearl::Event ev_it = event.prev();
    int          count = 0;

    // parse backwards through completion events and fill request array
    // Note: the request array is stored reversed compared to the original run.

    while (!ev_it->isOfType(GROUP_ENTER))
    {
        silas::CallbackData::requestmap_t::iterator reqit = reqmap.find(ev_it->getRequestId());

        if (reqit != reqmap.end())
        {
            reqit->second.event = ev_it;

            reqarray[count] = reqit->second.request;
            rmiarray[count] = reqit;

            ++count;
        }
        else
        {
            // if the event's reqid is not in the request map, the request was deleted.
            // store -1 timestamp to indicate that.
            ev_it->setTimestamp(-1);
        }

        --ev_it;
    }

    if (count)
    {
        int index;
        int flag;

        MPI_Testany(count, reqarray, &index, &flag, MPI_STATUS_IGNORE);

        if (flag)
        {
            predraw_completion(data, rmiarray[index]->second.event);

            data->push_nb_recv_buffer(rmiarray[index]->second.buffer);
            reqmap.erase(rmiarray[index]);
        }

        // parse forward through completion events and store timestamps
        double time = get_wtime();

        for (++ev_it; !ev_it->isOfType(GROUP_LEAVE); ++ev_it)
        {
            if (ev_it->getTimestamp() > 0)
            {
                ev_it->setTimestamp(time - reference_timestamp);
            }
        }
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_testall(const pearl::CallbackManager& cbmanager,
                             int                           user_event,
                             const pearl::Event&           event,
                             pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MPI_Request*                                 reqarray = data->get_request_array();
    silas::CallbackData::requestmap_t&           reqmap   = data->get_requestmap();
    silas::CallbackData::requestmap_t::iterator* rmiarray = data->get_requestmap_iter_array();

    pearl::Event ev_it = event.prev();
    int          count = 0;

    // parse backwards through completion events and fill request array
    // Note: the request array is stored reversed compared to the original run.

    while (!ev_it->isOfType(GROUP_ENTER))
    {
        silas::CallbackData::requestmap_t::iterator reqit = reqmap.find(ev_it->getRequestId());

        if (reqit != reqmap.end())
        {
            reqit->second.event = ev_it;

            reqarray[count] = reqit->second.request;
            rmiarray[count] = reqit;

            ++count;
        }
        else
        {
            // if the event's reqid is not in the request map, the request was deleted.
            // store -1 timestamp to indicate that.
            ev_it->setTimestamp(-1);
        }

        --ev_it;
    }

    if (count)
    {
        int flag;

        MPI_Testall(count, reqarray, &flag, MPI_STATUSES_IGNORE);

        count = flag ? count : 0;

        for (int i = 0; i < count; ++i)
        {
            predraw_completion(data, rmiarray[i]->second.event);

            data->push_nb_recv_buffer(rmiarray[i]->second.buffer);
            reqmap.erase(rmiarray[i]);
        }

        // parse forward through completion events and store timestamps
        double time = get_wtime();

        for (++ev_it; !ev_it->isOfType(GROUP_LEAVE); ++ev_it)
        {
            if (ev_it->getTimestamp() > 0)
            {
                ev_it->setTimestamp(time - reference_timestamp);
            }
        }
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_testsome(const pearl::CallbackManager& cbmanager,
                              int                           user_event,
                              const pearl::Event&           event,
                              pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MPI_Request*                                 reqarray = data->get_request_array();
    silas::CallbackData::requestmap_t&           reqmap   = data->get_requestmap();
    silas::CallbackData::requestmap_t::iterator* rmiarray = data->get_requestmap_iter_array();

    pearl::Event ev_it = event.prev();
    int          count = 0;

    // parse backwards through completion events and fill request array
    // Note: the request array is stored reversed compared to the original run.

    while (!ev_it->isOfType(GROUP_ENTER))
    {
        silas::CallbackData::requestmap_t::iterator reqit = reqmap.find(ev_it->getRequestId());

        if (reqit != reqmap.end())
        {
            reqit->second.event = ev_it;

            reqarray[count] = reqit->second.request;
            rmiarray[count] = reqit;

            ++count;
        }
        else
        {
            // if the event's reqid is not in the request map, the request was deleted.
            // store -1 timestamp to indicate that.
            ev_it->setTimestamp(-1);
        }

        --ev_it;
    }

    if (count)
    {
        int  outcount;
        int* indices = new int[count];

        MPI_Testsome(count, reqarray, &outcount, indices, MPI_STATUSES_IGNORE);

        for (int i = 0; i < outcount; ++i)
        {
            predraw_completion(data, rmiarray[indices[i]]->second.event);

            data->push_nb_recv_buffer(rmiarray[indices[i]]->second.buffer);
            reqmap.erase(rmiarray[indices[i]]);
        }

        delete[] indices;

        // parse forward through completion events and store timestamps
        double time = get_wtime();

        for (++ev_it; !ev_it->isOfType(GROUP_LEAVE); ++ev_it)
        {
            if (ev_it->getTimestamp() > 0)
            {
                ev_it->setTimestamp(time - reference_timestamp);
            }
        }
    }

    // set exit event timestamp
    event->setTimestamp(get_wtime() - reference_timestamp);
}


void
ReenactModel::cb_mpi_barrier(const pearl::CallbackManager& cbmanager,
                             int                           user_event,
                             const pearl::Event&           event,
                             pearl::CallbackData*          cdata)
{
    MPI_Comm mpicomm = event->getComm()->getHandle();

    MPI_Barrier(mpicomm);
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_bcast(const pearl::CallbackManager& cbmanager,
                           int                           user_event,
                           const pearl::Event&           event,
                           pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    int count = std::min(static_cast< int >(event->getBytesReceived()),
                         data->get_max_count());

    MpiComm* comm    = event->getComm();
    void*    sbuf    = data->get_send_buffer(count);
    MPI_Comm mpicomm = comm->getHandle();
    int      root    = event->getRoot();

    // event.prev()->setTimestamp(get_wtime() - reference_timestamp);
    MPI_Bcast(sbuf, count, MPI_BYTE, root, mpicomm);
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_reduce(const pearl::CallbackManager& cbmanager,
                            int                           user_event,
                            const pearl::Event&           event,
                            pearl::CallbackData*          cdata)
{
    /* INFO:
     * The operation and datatype used are not available from the
     * trace data. This implies that reenactment of the reduce
     * operation can be significantly off the 'real' reduce used
     * by the application, as an operation on bytes might take
     * longer than on ints, etc.
     */

    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    int count = std::min(static_cast< int >(event->getBytesSent()),
                         data->get_max_count());

    MpiComm* comm    = event->getComm();
    MPI_Comm mpicomm = comm->getHandle();
    void*    sbuf    = data->get_send_buffer(count);
    void*    rbuf    = data->get_recv_buffer(count * get_commsize(mpicomm));
    int      root    = event->getRoot();

    // event.prev()->setTimestamp(get_wtime() - reference_timestamp);
    MPI_Reduce(sbuf, rbuf, count, MPI_BYTE, MPI_BAND, root, mpicomm);
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_allreduce(const pearl::CallbackManager& cbmanager,
                               int                           user_event,
                               const pearl::Event&           event,
                               pearl::CallbackData*          cdata)
{
    /* INFO:
     * The operation and datatype used are not available from the
     * trace data. This implies that reenactment of the reduce
     * operation can be significantly off the 'real' reduce used
     * by the application, as an operation on bytes might take
     * longer than on ints, etc.
     */
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MpiComm* comm = event->getComm();

    int count = std::min(static_cast< int >(event->getBytesReceived()),
                         data->get_max_count());
    void* sbuf = data->get_send_buffer(count);
    void* rbuf = data->get_recv_buffer(count);

    MPI_Comm mpicomm = comm->getHandle();

    count /= get_commsize(mpicomm);

    // event.prev()->setTimestamp(get_wtime() - reference_timestamp);
    if ((count % sizeof(double)) == 0)
    {
        MPI_Allreduce(sbuf, rbuf, count / sizeof(double), MPI_DOUBLE, MPI_SUM, mpicomm);
    }
    else if ((count % sizeof(int)) == 0)
    {
        MPI_Allreduce(sbuf, rbuf, count / sizeof(int), MPI_INTEGER, MPI_SUM, mpicomm);
    }
    else
    {
        MPI_Allreduce(sbuf, rbuf, count, MPI_BYTE, MPI_BAND, mpicomm);
    }

    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_allgather(const pearl::CallbackManager& cbmanager,
                               int                           user_event,
                               const pearl::Event&           event,
                               pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MPI_Comm mpicomm = event->getComm()->getHandle();
    int      scount  = std::min(static_cast< int >(event->getBytesSent()),
                                data->get_max_count());
    int rcount = std::min(static_cast< int >(event->getBytesReceived()),
                          data->get_max_count());
    scount /= get_commsize(mpicomm);
    rcount /= get_commsize(mpicomm);
    void* sbuf = data->get_send_buffer(scount);
    void* rbuf = data->get_recv_buffer(rcount);

    // event.prev()->setTimestamp(get_wtime() - reference_timestamp);
    MPI_Allgather(sbuf, scount, MPI_BYTE,
                  rbuf, rcount, MPI_BYTE, mpicomm);
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_alltoall(const pearl::CallbackManager& cbmanager,
                              int                           user_event,
                              const pearl::Event&           event,
                              pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MPI_Comm mpicomm = event->getComm()->getHandle();
    int      scount  = std::min(static_cast< int >(event->getBytesSent()),
                                data->get_max_count());
    int rcount = std::min(static_cast< int >(event->getBytesReceived()),
                          data->get_max_count());
    scount /= get_commsize(mpicomm);
    rcount /= get_commsize(mpicomm);
    void* sbuf = data->get_send_buffer(scount);
    void* rbuf = data->get_recv_buffer(rcount);

    // event.prev()->setTimestamp(get_wtime() - reference_timestamp);
    MPI_Alltoall(sbuf, scount, MPI_BYTE,
                 rbuf, rcount, MPI_BYTE, mpicomm);
    event->setTimestamp(get_wtime() - reference_timestamp);
}


/// @todo Size calculation should use uint64_t instead of int
void
ReenactModel::cb_mpi_scan(const pearl::CallbackManager& cbmanager,
                          int                           user_event,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cdata)
{
    /* INFO:
     * The operation and datatype used are not available from the
     * trace data. This implies that reenactment of the reduce
     * operation can be significantly off the 'real' reduce used
     * by the application, as an operation on bytes might take
     * longer than on ints, etc.
     */
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    MpiComm* comm    = event->getComm();
    MPI_Comm mpicomm = comm->getHandle();

    int me    = comm->getCommSet().getLocalRank(mpiRank);
    int count = 0;

    if (me == 0)
    {
        count = static_cast< int >(event->getBytesSent()) / get_commsize(mpicomm);
    }
    else
    {
        count = static_cast< int >(event->getBytesReceived()) / (me + 1);
    }

    count = std::min(count, data->get_max_count());

    void* sbuf = data->get_send_buffer(count);
    void* rbuf = data->get_recv_buffer(count);

    // event.prev()->setTimestamp(get_wtime() - reference_timestamp);
    MPI_Scan(sbuf, rbuf, count, MPI_BYTE, MPI_BAND, mpicomm);
    event->setTimestamp(get_wtime() - reference_timestamp);
}


#if HAVE(DECL_MPI_EXSCAN)
    /// @todo Size calculation should use uint64_t instead of int
    void
    ReenactModel::cb_mpi_exscan(const pearl::CallbackManager& cbmanager,
                                int                           user_event,
                                const pearl::Event&           event,
                                pearl::CallbackData*          cdata)
    {
        /* INFO:
         * The operation and datatype used are not available from the
         * trace data. This implies that reenactment of the reduce
         * operation can be significantly off the 'real' reduce used
         * by the application, as an operation on bytes might take
         * longer than on ints, etc.
         */
        silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

        MpiComm* comm    = event->getComm();
        MPI_Comm mpicomm = comm->getHandle();

        int me    = comm->getCommSet().getLocalRank(mpiRank);
        int count = 0;

        if (me == 0)
        {
            count = static_cast< int >(event->getBytesSent()) / (get_commsize(mpicomm) - 1);
        }
        else
        {
            count = static_cast< int >(event->getBytesReceived()) / me;
        }

        count = std::min(count, data->get_max_count());

        void* sbuf = data->get_send_buffer(count);
        void* rbuf = data->get_recv_buffer(count);

        // event.prev()->setTimestamp(get_wtime() - reference_timestamp);
        MPI_Exscan(sbuf, rbuf, count, MPI_BYTE, MPI_BAND, mpicomm);
        event->setTimestamp(get_wtime() - reference_timestamp);
    }
#endif    // HAVE(DECL_MPI_EXSCAN)


// --- Postprocessing callbacks ---------------------------------------------

void
ReenactModel::cb_start_postprocess(const pearl::CallbackManager& cbmanager,
                                   int                           user_event,
                                   const pearl::Event&           event,
                                   pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    data->activate_action_list();
    data->reset_action_idx();

    // initialization of timestamp variables
    data->set_preceeding_timestamp(reference_timestamp);
    data->set_original_timestamp(reference_timestamp);
}


void
ReenactModel::cb_any_postprocess(const pearl::CallbackManager& cbmanager,
                                 int                           user_event,
                                 const pearl::Event&           event,
                                 pearl::CallbackData*          cdata)
{
    silas::CallbackData* data   = static_cast< silas::CallbackData* >(cdata);
    int                  signal = data->get_action(event);

    cbmanager.notify(signal, event, data);
}


void
ReenactModel::cb_recalc(const pearl::CallbackManager& cbmanager,
                        int                           user_event,
                        const pearl::Event&           event,
                        pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    // calculate timespan
    pearl::timestamp_t start_timestamp = data->get_preceeding_timestamp();
    pearl::timestamp_t end_timestamp   = data->get_original_timestamp();

    // set ratios for skipped events
    data->calculate_timespans(event, start_timestamp, end_timestamp);
}


void
ReenactModel::cb_ignore_postprocess(const pearl::CallbackManager& cbmanager,
                                    int                           user_event,
                                    const pearl::Event&           event,
                                    pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    pearl::Event prev    = data->get_prev_event(event);
    pearl::Event preprev = data->get_prev_event(prev);

    // check for singleton skips
    if (  (data->get_action(prev) == silas::SKIP)
       && (data->get_action(preprev) != silas::SKIP))
    {
        // interpolate singleton skip between this and last valid event
        log("Adjusting singleton skip\n");
        event.prev()->setTimestamp(event->getTimestamp()
                                   - ((event->getTimestamp() - preprev->getTimestamp()) / 2));
    }
}


// --- Verification callbacks -----------------------------------------------

void
ReenactModel::cb_start_verify(const pearl::CallbackManager& cbmanager,
                              int                           user_event,
                              const pearl::Event&           event,
                              pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    data->activate_action_list();
    data->reset_action_idx();
}


void
ReenactModel::cb_finish_verify(const pearl::CallbackManager& cbmanager,
                               int                           user_event,
                               const pearl::Event&           event,
                               pearl::CallbackData*          cdata)
{
    silas::CallbackData* data = static_cast< silas::CallbackData* >(cdata);

    if (is_set("debug"))
    {
        data->write_ts();
    }
}


void
ReenactModel::cb_any_verify(const pearl::CallbackManager& cbmanager,
                            int                           user_event,
                            const pearl::Event&           event,
                            pearl::CallbackData*          cdata)
{
    silas::CallbackData* data   = static_cast< silas::CallbackData* >(cdata);
    int                  signal = data->get_action(event);

    // check monotony of timestamps
    if (signal != silas::DELETE)
    {
        pearl::Event prev = data->get_prev_event(event);

        if (  prev.is_valid()
           && (event->getTimestamp() < prev->getTimestamp()))
        {
            std::cerr << "WARNING: Monotony of time in event stream violated at" << event.get_id()
                      << " of type " << event->getTimestamp()
                      << " by " << (prev->getTimestamp() - event->getTimestamp()) << "s!" << std::endl;
        }
    }
}


/*-- HELPER FUNCTIONS * ------------------------------------------------*/

bool
ReenactModel::has_completions(const pearl::Event& event)
{
    if (!event->isOfType(GROUP_LEAVE))
    {
        return false;
    }

    // Determine if a wait or test region contains completion events

    for (pearl::Event ev_it = event.prev();
         !ev_it->isOfType(GROUP_ENTER);
         --ev_it)
    {
        if (  ev_it->isOfType(GROUP_RECV)
           || ev_it->isOfType(MPI_SEND_COMPLETE))
        {
            return true;
        }
    }

    return false;
}


void
ReenactModel::predraw_completion(silas::CallbackData* data,
                                 const pearl::Event&  req_ev)
{
    pearl::Event ev     = req_ev.next_request_event();
    pearl::Event nextev = ev.next_request_event();

    while (ev != nextev)
    {
        delete_event(data, ev);

        ev     = nextev;
        nextev = nextev.next_request_event();
    }

    data->get_trace().swap(req_ev, ev);
    delete_event(data, ev);
}


void
ReenactModel::delete_event(silas::CallbackData* data,
                           const pearl::Event&  event)
{
    data->set_action(silas::DELETE, event);
    event->setTimestamp(-1);

    pearl::Event enter = event.enterptr();
    pearl::Event it    = enter.next();
    bool         del   = true;

    // check if a multiple completion region can be removed entirely

    for ( ; del && !it->isOfType(GROUP_LEAVE); ++it)
    {
        del = (data->get_action(it) == silas::DELETE);
    }

    if (del)
    {
        data->set_action(silas::DELETE, it);
        it->setTimestamp(-1);

        data->set_action(silas::DELETE, enter);
        enter->setTimestamp(-1);
    }
}


inline void
ReenactModel::simulate_timespan(double time)
{
    pearl::timestamp_t end_time = get_wtime() + time - idle_overhead;

    while (get_wtime() < end_time)
    {
    }
}


inline double
ReenactModel::get_wtime()
{
    /// @todo Use the high resolution timer of the measurement system.
    return MPI_Wtime();
}


inline int
ReenactModel::get_commsize(MPI_Comm comm)
{
    map< MPI_Comm, int >::iterator it;

    if ((it = m_comm_sz.find(comm)) == m_comm_sz.end())
    {
        int size;
        MPI_Comm_size(comm, &size);
        m_comm_sz.insert(make_pair(comm, size));

        return size;
    }
    else
    {
        return it->second;
    }
}
