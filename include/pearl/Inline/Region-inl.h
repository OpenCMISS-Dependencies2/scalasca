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


#ifndef PEARL_REGION_H
    #error "Do not include this file!  Use <pearl/Region.h> instead."
#endif


// --- Access definition data (inline) --------------------------------------

inline uint32_t
Region::getClassification() const
{
    return mClass;
}


// --- Inline function implementation ---------------------------------------

inline bool
is_omp_api(const pearl::Region& region)
{
    return (region.getClassification() & pearl::Region::cCLASS_OMP);
}


inline bool
is_omp_parallel(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_OMP
                    | pearl::Region::cCAT_OMP_PARALLEL;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_omp_barrier(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_OMP
                    | pearl::Region::cCAT_OMP_SYNC
                    | pearl::Region::cTYPE_OMP_BARRIER;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_omp_ebarrier(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_OMP
                    | pearl::Region::cCAT_OMP_SYNC
                    | pearl::Region::cTYPE_OMP_BARRIER
                    | pearl::Region::cMODE_OMP_EXPLICIT;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_omp_ibarrier(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_OMP
                    | pearl::Region::cCAT_OMP_SYNC
                    | pearl::Region::cTYPE_OMP_BARRIER
                    | pearl::Region::cMODE_OMP_IMPLICIT;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_pthread_api(const pearl::Region& region)
{
    return (region.getClassification() & pearl::Region::cCLASS_PTHREAD);
}


inline bool
is_pthread_mutex_lock(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_PTHREAD
                    | pearl::Region::cCAT_PTHREAD_SYNC
                    | pearl::Region::cTYPE_PTHREAD_MUTEX
                    | pearl::Region::cMODE_PTHREAD_LOCK;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_pthread_mutex_trylock(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_PTHREAD
                    | pearl::Region::cCAT_PTHREAD_SYNC
                    | pearl::Region::cTYPE_PTHREAD_MUTEX
                    | pearl::Region::cMODE_PTHREAD_TRYLOCK;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_pthread_spin_lock(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_PTHREAD
                    | pearl::Region::cCAT_PTHREAD_SYNC
                    | pearl::Region::cTYPE_PTHREAD_SPIN
                    | pearl::Region::cMODE_PTHREAD_LOCK;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_pthread_condition(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_PTHREAD
                    | pearl::Region::cCAT_PTHREAD_SYNC
                    | pearl::Region::cTYPE_PTHREAD_CONDITION;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_api(const pearl::Region& region)
{
    return (region.getClassification() & pearl::Region::cCLASS_MPI);
}


inline bool
is_mpi_init(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_SETUP
                    | pearl::Region::cTYPE_MPI_INIT;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_finalize(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_SETUP
                    | pearl::Region::cTYPE_MPI_FINALIZE;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_collective(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_COLLECTIVE;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_barrier(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_COLLECTIVE
                    | pearl::Region::cTYPE_MPI_BARRIER;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_12n(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_COLLECTIVE
                    | pearl::Region::cTYPE_MPI_ONE_TO_N;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_n21(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_COLLECTIVE
                    | pearl::Region::cTYPE_MPI_N_TO_ONE;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_n2n(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_COLLECTIVE
                    | pearl::Region::cTYPE_MPI_N_TO_N;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_scan(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_COLLECTIVE
                    | pearl::Region::cTYPE_MPI_PARTIAL;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_block_send(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_P2P
                    | pearl::Region::cTYPE_MPI_SEND
                    | pearl::Region::cMODE_MPI_STANDARD
                    | pearl::Region::cMODE_MPI_SYNCHRONOUS;
    uint32_t value = region.getClassification() ^ mask;

    return (  (value == pearl::Region::cMODE_MPI_STANDARD)
           || (value == pearl::Region::cMODE_MPI_SYNCHRONOUS));
}


inline bool
is_mpi_testx(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_P2P
                    | pearl::Region::cTYPE_MPI_TEST;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_wait_single(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_P2P
                    | pearl::Region::cTYPE_MPI_WAIT
                    | pearl::Region::cMODE_MPI_SINGLE;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_wait_multi(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_P2P
                    | pearl::Region::cTYPE_MPI_WAIT
                    | pearl::Region::cMODE_MPI_MANY;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_rma_start(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_RMA
                    | pearl::Region::cTYPE_MPI_RMA_GATS
                    | pearl::Region::cMODE_RMA_START;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_rma_complete(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_RMA
                    | pearl::Region::cTYPE_MPI_RMA_GATS
                    | pearl::Region::cMODE_RMA_COMPLETE;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_rma_post(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_RMA
                    | pearl::Region::cTYPE_MPI_RMA_GATS
                    | pearl::Region::cMODE_RMA_POST;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_rma_wait(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_RMA
                    | pearl::Region::cTYPE_MPI_RMA_GATS
                    | pearl::Region::cMODE_RMA_WAIT;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_rma_collective(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_RMA
                    | pearl::Region::cTYPE_MPI_RMA_COLL;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_rma_create(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_RMA
                    | pearl::Region::cTYPE_MPI_RMA_COLL
                    | pearl::Region::cMODE_RMA_WIN_CREATE;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_rma_fence(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_RMA
                    | pearl::Region::cTYPE_MPI_RMA_COLL
                    | pearl::Region::cMODE_RMA_FENCE;

    return ((region.getClassification() & mask) == mask);
}


inline bool
is_mpi_rma_free(const pearl::Region& region)
{
    uint32_t mask = pearl::Region::cCLASS_MPI
                    | pearl::Region::cCAT_MPI_RMA
                    | pearl::Region::cTYPE_MPI_RMA_COLL
                    | pearl::Region::cMODE_RMA_WIN_FREE;

    return ((region.getClassification() & mask) == mask);
}
