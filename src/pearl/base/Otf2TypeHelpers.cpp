/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of OTF2 helper and type conversion functions.
 *
 *  This file provides the implementation of helper functions to convert
 *  OTF2 data types into equivalent PEARL data types.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "Otf2TypeHelpers.h"

#include <cassert>

#include <UTILS_Error.h>

#include "Otf2EvtCallbacks.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Helper functions -----------------------------------------------------

namespace pearl
{
namespace detail
{
Paradigm
otf2ConvertParadigm(OTF2_Paradigm paradigm)
{
    switch (paradigm)
    {
        case OTF2_PARADIGM_UNKNOWN:
            return Paradigm::UNKNOWN;

        case OTF2_PARADIGM_USER:
            return Paradigm::USER;

        case OTF2_PARADIGM_COMPILER:
            return Paradigm::COMPILER;

        case OTF2_PARADIGM_OPENMP:
            return Paradigm::OPENMP;

        case OTF2_PARADIGM_MPI:
            return Paradigm::MPI;

        case OTF2_PARADIGM_CUDA:
            return Paradigm::CUDA;

        case OTF2_PARADIGM_MEASUREMENT_SYSTEM:
            return Paradigm::MEASUREMENT_SYSTEM;

        case OTF2_PARADIGM_PTHREAD:
            return Paradigm::PTHREAD;

        case OTF2_PARADIGM_HMPP:
            return Paradigm::HMPP;

        case OTF2_PARADIGM_OMPSS:
            return Paradigm::OMPSS;

        case OTF2_PARADIGM_HARDWARE:
            return Paradigm::HARDWARE;

        case OTF2_PARADIGM_GASPI:
            return Paradigm::GASPI;

        case OTF2_PARADIGM_UPC:
            return Paradigm::UPC;

        case OTF2_PARADIGM_SHMEM:
            return Paradigm::SHMEM;

        case OTF2_PARADIGM_WINTHREAD:
            return Paradigm::WINTHREAD;

        case OTF2_PARADIGM_QTTHREAD:
            return Paradigm::QTTHREAD;

        case OTF2_PARADIGM_ACETHREAD:
            return Paradigm::ACETHREAD;

        case OTF2_PARADIGM_TBBTHREAD:
            return Paradigm::TBBTHREAD;

        case OTF2_PARADIGM_OPENACC:
            return Paradigm::OPENACC;

        case OTF2_PARADIGM_OPENCL:
            return Paradigm::OPENCL;

        case OTF2_PARADIGM_MTAPI:
            return Paradigm::MTAPI;

        case OTF2_PARADIGM_SAMPLING:
            return Paradigm::SAMPLING;

        case OTF2_PARADIGM_NONE:
            return Paradigm::NONE;

        default:
            break;
    }

    UTILS_WARN_ONCE("Mapping unhandled OTF2 paradigm %u to 'unknown'.",
                    paradigm);

    return Paradigm::UNKNOWN;
}


timestamp_t
otf2ConvertTime(uint64_t                   timestamp,
                const Otf2EvtCbData* const data)
{
    return (static_cast< double >(timestamp - data->mGlobalOffset)
            / data->mTimerResolution);
}


MpiCollEnd_rep::coll_type
otf2ConvertCollectiveType(OTF2_CollectiveOp type)
{
    switch (type)
    {
        case OTF2_COLLECTIVE_OP_BARRIER:
            return MpiCollEnd_rep::BARRIER;

        case OTF2_COLLECTIVE_OP_BCAST:
            return MpiCollEnd_rep::BCAST;

        case OTF2_COLLECTIVE_OP_GATHER:
            return MpiCollEnd_rep::GATHER;

        case OTF2_COLLECTIVE_OP_GATHERV:
            return MpiCollEnd_rep::GATHERV;

        case OTF2_COLLECTIVE_OP_SCATTER:
            return MpiCollEnd_rep::SCATTER;

        case OTF2_COLLECTIVE_OP_SCATTERV:
            return MpiCollEnd_rep::SCATTERV;

        case OTF2_COLLECTIVE_OP_ALLGATHER:
            return MpiCollEnd_rep::ALLGATHER;

        case OTF2_COLLECTIVE_OP_ALLGATHERV:
            return MpiCollEnd_rep::ALLGATHERV;

        case OTF2_COLLECTIVE_OP_ALLTOALL:
            return MpiCollEnd_rep::ALLTOALL;

        case OTF2_COLLECTIVE_OP_ALLTOALLV:
            return MpiCollEnd_rep::ALLTOALLV;

        case OTF2_COLLECTIVE_OP_ALLTOALLW:
            return MpiCollEnd_rep::ALLTOALLW;

        case OTF2_COLLECTIVE_OP_ALLREDUCE:
            return MpiCollEnd_rep::ALLREDUCE;

        case OTF2_COLLECTIVE_OP_REDUCE:
            return MpiCollEnd_rep::REDUCE;

        case OTF2_COLLECTIVE_OP_REDUCE_SCATTER:
            return MpiCollEnd_rep::REDUCE_SCATTER;

        case OTF2_COLLECTIVE_OP_SCAN:
            return MpiCollEnd_rep::SCAN;

        case OTF2_COLLECTIVE_OP_EXSCAN:
            return MpiCollEnd_rep::EXSCAN;

        case OTF2_COLLECTIVE_OP_REDUCE_SCATTER_BLOCK:
            return MpiCollEnd_rep::REDUCE_SCATTER_BLOCK;
    }

    // We should never reach this point...
    assert(false);

    return MpiCollEnd_rep::NUMBER_OF_COLL_TYPES;
}


Location::Type
otf2ConvertLocationType(OTF2_LocationType type)
{
    switch (type)
    {
        case OTF2_LOCATION_TYPE_CPU_THREAD:
            return Location::TYPE_CPU_THREAD;

        case OTF2_LOCATION_TYPE_GPU:
            return Location::TYPE_GPU;

        case OTF2_LOCATION_TYPE_METRIC:
            return Location::TYPE_METRIC;

        default:
            break;
    }

    return Location::TYPE_UNKNOWN;
}


LocationGroup::Type
otf2ConvertLocationGroupType(OTF2_LocationGroupType type)
{
    switch (type)
    {
        case OTF2_LOCATION_GROUP_TYPE_PROCESS:
            return LocationGroup::TYPE_PROCESS;

        default:
            break;
    }

    return LocationGroup::TYPE_UNKNOWN;
}


Paradigm
otf2ConvertRegionParadigm(OTF2_Paradigm   paradigm,
                          OTF2_RegionRole role,
                          const string&   name)
{
    Paradigm result = otf2ConvertParadigm(paradigm);

    switch (native_value(result))
    {
        case Paradigm::OPENMP:
            // Fix paradigm of artificial "task_root" root node (Score-P < v1.4)
            if (role == OTF2_REGION_ROLE_TASK)
            {
                if (name == "task_root")
                {
                    result = Paradigm::MEASUREMENT_SYSTEM;
                }
            }
            break;

        case Paradigm::PTHREAD:
            // Fix paradigm of artificial "THREADS" root node (Score-P < v1.4)
            if (role == OTF2_REGION_ROLE_ARTIFICIAL)
            {
                if (name == "THREADS")
                {
                    result = Paradigm::MEASUREMENT_SYSTEM;
                }
            }
            break;

        default:
            break;
    }

    return result;
}


Region::Role
otf2ConvertRegionRole(OTF2_Paradigm   paradigm,
                      OTF2_RegionRole role,
                      const string&   name)
{
    switch (role)
    {
        case OTF2_REGION_ROLE_UNKNOWN:
            return Region::ROLE_UNKNOWN;

        case OTF2_REGION_ROLE_FUNCTION:
            if (paradigm == OTF2_PARADIGM_MPI)
            {
                // Fix role of some MPI point-to-point functions (Score-P < v1.4)
                if (  (name == "MPI_Bsend_init")
                   || (name == "MPI_Iprobe")
                   || (name == "MPI_Irecv")
                   || (name == "MPI_Probe")
                   || (name == "MPI_Recv_init")
                   || (name == "MPI_Rsend_init")
                   || (name == "MPI_Send_init")
                   || (name == "MPI_Ssend_init"))
                {
                    return Region::ROLE_POINT2POINT;
                }

                // Fix role of some MPI one-sided operations (Score-P < v2.0)
                else if (  (name == "MPI_Put")
                        || (name == "MPI_Get"))
                {
                    return Region::ROLE_RMA;
                }
                else if (name == "MPI_Accumulate")
                {
                    return Region::ROLE_ATOMIC;
                }

                // Fix role of MPI memory operations (Score-P < v2.0)
                else if (name == "MPI_Alloc_mem")
                {
                    return Region::ROLE_ALLOCATE;
                }
                else if (name == "MPI_Free_mem")
                {
                    return Region::ROLE_DEALLOCATE;
                }
            }

            return Region::ROLE_FUNCTION;

        case OTF2_REGION_ROLE_WRAPPER:
            return Region::ROLE_WRAPPER;

        case OTF2_REGION_ROLE_LOOP:
            return Region::ROLE_LOOP;

        case OTF2_REGION_ROLE_CODE:
            return Region::ROLE_CODE;

        case OTF2_REGION_ROLE_PARALLEL:
            return Region::ROLE_PARALLEL;

        case OTF2_REGION_ROLE_SECTIONS:
            return Region::ROLE_SECTIONS;

        case OTF2_REGION_ROLE_SECTION:
            return Region::ROLE_SECTION;

        case OTF2_REGION_ROLE_WORKSHARE:
            return Region::ROLE_WORKSHARE;

        case OTF2_REGION_ROLE_SINGLE:
            return Region::ROLE_SINGLE;

        case OTF2_REGION_ROLE_SINGLE_SBLOCK:
            return Region::ROLE_SINGLE_SBLOCK;

        case OTF2_REGION_ROLE_MASTER:
            return Region::ROLE_MASTER;

        case OTF2_REGION_ROLE_CRITICAL:
            return Region::ROLE_CRITICAL;

        case OTF2_REGION_ROLE_CRITICAL_SBLOCK:
            return Region::ROLE_CRITICAL_SBLOCK;

        case OTF2_REGION_ROLE_ATOMIC:
            return Region::ROLE_ATOMIC;

        case OTF2_REGION_ROLE_BARRIER:
            return Region::ROLE_BARRIER;

        case OTF2_REGION_ROLE_IMPLICIT_BARRIER:
            return Region::ROLE_IMPLICIT_BARRIER;

        case OTF2_REGION_ROLE_FLUSH:
            return Region::ROLE_FLUSH;

        case OTF2_REGION_ROLE_ORDERED:
            return Region::ROLE_ORDERED;

        case OTF2_REGION_ROLE_ORDERED_SBLOCK:
            return Region::ROLE_ORDERED_SBLOCK;

        case OTF2_REGION_ROLE_TASK:
            // Fix role of artificial tasking root node (Score-P < v2.0)
            if (  (paradigm == OTF2_PARADIGM_OPENMP)
               || (paradigm == OTF2_PARADIGM_MEASUREMENT_SYSTEM))
            {
                if (name == "task_root")
                {
                    return Region::ROLE_ARTIFICIAL;
                }
            }

            return Region::ROLE_TASK;

        case OTF2_REGION_ROLE_TASK_CREATE:
            return Region::ROLE_TASK_CREATE;

        case OTF2_REGION_ROLE_TASK_WAIT:
            return Region::ROLE_TASK_WAIT;

        case OTF2_REGION_ROLE_COLL_ONE2ALL:
            return Region::ROLE_COLL_ONE2ALL;

        case OTF2_REGION_ROLE_COLL_ALL2ONE:
            return Region::ROLE_COLL_ALL2ONE;

        case OTF2_REGION_ROLE_COLL_ALL2ALL:
            return Region::ROLE_COLL_ALL2ALL;

        case OTF2_REGION_ROLE_COLL_OTHER:
            return Region::ROLE_COLL_OTHER;

        case OTF2_REGION_ROLE_FILE_IO:
            return Region::ROLE_FILE_IO;

        case OTF2_REGION_ROLE_POINT2POINT:
            // Fix role of MPI request handling functions (Score-P < v2.0)
            if (paradigm == OTF2_PARADIGM_MPI)
            {
                if (  (name == "MPI_Test")
                   || (name == "MPI_Testall")
                   || (name == "MPI_Testany")
                   || (name == "MPI_Testsome")
                   || (name == "MPI_Wait")
                   || (name == "MPI_Waitall")
                   || (name == "MPI_Waitany")
                   || (name == "MPI_Waitsome"))
                {
                    return Region::ROLE_FUNCTION;
                }
            }

            return Region::ROLE_POINT2POINT;

        case OTF2_REGION_ROLE_RMA:
            return Region::ROLE_RMA;

        case OTF2_REGION_ROLE_DATA_TRANSFER:
            return Region::ROLE_DATA_TRANSFER;

        case OTF2_REGION_ROLE_ARTIFICIAL:
            return Region::ROLE_ARTIFICIAL;

        case OTF2_REGION_ROLE_THREAD_CREATE:
            return Region::ROLE_THREAD_CREATE;

        case OTF2_REGION_ROLE_THREAD_WAIT:
            return Region::ROLE_THREAD_WAIT;

        case OTF2_REGION_ROLE_TASK_UNTIED:
            return Region::ROLE_TASK_UNTIED;

        case OTF2_REGION_ROLE_ALLOCATE:
            return Region::ROLE_ALLOCATE;

        case OTF2_REGION_ROLE_DEALLOCATE:
            return Region::ROLE_DEALLOCATE;

        case OTF2_REGION_ROLE_REALLOCATE:
            return Region::ROLE_REALLOCATE;

        default:
            break;
    }

    return Region::ROLE_UNKNOWN;
}


IpcDatatype
otf2GetIpcDatatype(const OTF2_Type type)
{
    switch (type)
    {
        case OTF2_TYPE_UINT8:
            return PEARL_UINT8_T;

        case OTF2_TYPE_UINT16:
            return PEARL_UINT16_T;

        case OTF2_TYPE_UINT32:
            return PEARL_UINT32_T;

        case OTF2_TYPE_UINT64:
            return PEARL_UINT64_T;

        case OTF2_TYPE_INT8:
            return PEARL_INT8_T;

        case OTF2_TYPE_INT16:
            return PEARL_INT16_T;

        case OTF2_TYPE_INT32:
            return PEARL_INT32_T;

        case OTF2_TYPE_INT64:
            return PEARL_INT64_T;

        case OTF2_TYPE_FLOAT:
            return PEARL_FLOAT;

        case OTF2_TYPE_DOUBLE:
            return PEARL_DOUBLE;

        default:
            break;
    }

    // This point should never be reached...
    assert(false);

    return PEARL_UINT8_T;
}
}    // namespace pearl::detail
}    // namespace pearl
