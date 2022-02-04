/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/**
 * \file cubew_allocs.h
   \brief Provides own implementation of memory allocation functions with tracking
   functionality.
 */
#ifndef CUBEW_ALLOCS_H
#define CUBEW_ALLOCS_H
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct allocations_t
{
    size_t total_allocated;
    size_t max_allocations;
    size_t min_allocations;
    size_t nallocs;
} allocations_t;

typedef struct memory_stats_t
{
    allocations_t extmallocs;
    allocations_t mallocs;
    allocations_t callocs;
    allocations_t reallocs;
    allocations_t frees;
    allocations_t extfrees;
    size_t        allocations_counter;
    size_t        current_nallocs;
    size_t        highwater_nallocs;
    size_t        current_memory;
    size_t        highwater_memory;
    size_t        ntracking;
} memory_stats_t;

typedef struct allocator_t
{
    void* ( *alloc )( size_t, void* );
    bool ( * is_external_memory )( void*,
                                   void* );
    int            tracking_on;
    int            initialized;
    memory_stats_t memory_stats;
    void*          user_ptr;
} allocator_t;



void*
cubew_malloc( size_t size,
              char*  message
              );

void*
cubew_calloc( size_t nitems,
              size_t size,
              char*  message );

/*
 * changed signature of realloc, because in cube realloc used only
   in cubew_vector.h and it is growing win deterministic way.
 */
void*
cubew_realloc( void*  ptr,
               size_t osize,
               size_t nsize,
               char*  message );

void
cubew_free( void* ptr,
            char* message );


void
cubew_init_allocs( void* ( *own_alloc )( size_t, void* ),
                   bool ( * is_external_mem )( void*, void* ),
                   void*
                   );
int
cubew_initialized( void );

void
cubew_report_allocs();


#ifdef __cplusplus
}
#endif

#endif
