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
 * \file cubew_allocs.c
   \brief Provides implementation of own allocation functions
 */
#include "config.h"
#include "cubew_allocs.h"
#include <stdio.h>
#include <stdlib.h>  /* getenv */
#include <string.h>  /* memset, memcpy */
#include "UTILS_Error.h"

#ifdef HAVE_ADVANCED_MEMORY

static allocator_t cubew_allocator;

#ifndef max
        #define max( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif

#ifndef min
        #define min( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif


#ifndef HAVE_MEMORY_TRACKING
#define  BLOCK_SIZE_SHIFT  0
#define track_highwater_mark( x )
#define track_malloc( x, y, z )
#define track_calloc( x, y, z )
#define track_realloc( x, y, z, w )
#define track_free( x )

void
cubew_report_allocs()
{
}
#else
#define BLOCK_SIZE_SHIFT  sizeof( size_t )

static
void
track_highwater_mark( size_t size, size_t op )
{
    cubew_allocator.memory_stats.allocations_counter++;
    cubew_allocator.memory_stats.current_nallocs  += op;
    cubew_allocator.memory_stats.highwater_nallocs =
        max(
            cubew_allocator.memory_stats.highwater_nallocs,
            cubew_allocator.memory_stats.current_nallocs
            );

    cubew_allocator.memory_stats.current_memory  += op * size;
    cubew_allocator.memory_stats.highwater_memory =
        max(
            cubew_allocator.memory_stats.highwater_memory,
            cubew_allocator.memory_stats.current_memory
            );
}


static
void
track_malloc( void* ptr,  size_t size, int ext )
{
    *( ( size_t* )ptr ) = size;
    if ( cubew_allocator.tracking_on != 0 )
    {
        if ( ext == 0 )
        {
            cubew_allocator.memory_stats.mallocs.total_allocated += size;
            cubew_allocator.memory_stats.mallocs.nallocs++;
            cubew_allocator.memory_stats.mallocs.max_allocations =
                ( size > cubew_allocator.memory_stats.mallocs.max_allocations )
                ? size
                :
                cubew_allocator.memory_stats.mallocs.max_allocations
            ;
            cubew_allocator.memory_stats.mallocs.min_allocations =
                ( cubew_allocator.memory_stats.mallocs.min_allocations == 0 )
                ? size
                : (
                    ( size < cubew_allocator.memory_stats.mallocs.min_allocations )
                    ? size
                    :
                    cubew_allocator.memory_stats.mallocs.min_allocations
                    )
            ;
        }
        else
        {
            cubew_allocator.memory_stats.extmallocs.total_allocated += size;
            cubew_allocator.memory_stats.extmallocs.nallocs++;
            cubew_allocator.memory_stats.extmallocs.max_allocations =
                ( size > cubew_allocator.memory_stats.extmallocs.max_allocations )
                ? size
                :
                cubew_allocator.memory_stats.extmallocs.max_allocations
            ;
            cubew_allocator.memory_stats.extmallocs.min_allocations =
                ( cubew_allocator.memory_stats.extmallocs.min_allocations == 0 )
                ? size
                : (
                    ( size < cubew_allocator.memory_stats.extmallocs.min_allocations )
                    ? size
                    :
                    cubew_allocator.memory_stats.extmallocs.min_allocations
                    )
            ;
        }
    }
    track_highwater_mark( size, 1 );
}


static
void
track_calloc( void* ptr,  size_t size, int ext )
{
    *( ( size_t* )ptr ) = size;
    if ( cubew_allocator.tracking_on != 0 )
    {
        if ( ext == 0 )
        {
            cubew_allocator.memory_stats.callocs.total_allocated += size;
            cubew_allocator.memory_stats.callocs.nallocs++;
            cubew_allocator.memory_stats.callocs.max_allocations =
                ( size > cubew_allocator.memory_stats.callocs.max_allocations )
                ? size
                :
                cubew_allocator.memory_stats.callocs.max_allocations
            ;
            cubew_allocator.memory_stats.callocs.min_allocations =
                ( cubew_allocator.memory_stats.callocs.min_allocations == 0 )
                ? size
                :
                (
                    ( size < cubew_allocator.memory_stats.callocs.min_allocations )
                    ? size
                    :
                    cubew_allocator.memory_stats.callocs.min_allocations
                )
            ;
        }
        else
        {
            cubew_allocator.memory_stats.extmallocs.total_allocated += size;
            cubew_allocator.memory_stats.extmallocs.nallocs++;
            cubew_allocator.memory_stats.extmallocs.max_allocations =
                ( size > cubew_allocator.memory_stats.extmallocs.max_allocations )
                ? size
                :
                cubew_allocator.memory_stats.extmallocs.max_allocations
            ;
            cubew_allocator.memory_stats.extmallocs.min_allocations =
                ( cubew_allocator.memory_stats.extmallocs.min_allocations == 0 )
                ? size
                : (
                    ( size < cubew_allocator.memory_stats.extmallocs.min_allocations )
                    ? size
                    :
                    cubew_allocator.memory_stats.extmallocs.min_allocations
                    )
            ;
        }
    }
    track_highwater_mark( size, 1 );
}

static
void
track_realloc( void* ptr,  size_t osize, size_t nsize, int ext )
{
    *( ( size_t* )ptr ) = nsize;
    if ( cubew_allocator.tracking_on != 0 )
    {
        if ( ext == 0 )
        {
            cubew_allocator.memory_stats.reallocs.total_allocated -= osize;
            cubew_allocator.memory_stats.reallocs.total_allocated += nsize;
            cubew_allocator.memory_stats.reallocs.nallocs++;
            cubew_allocator.memory_stats.reallocs.max_allocations =
                ( nsize > cubew_allocator.memory_stats.reallocs.max_allocations )
                ? nsize
                :
                cubew_allocator.memory_stats.reallocs.max_allocations
            ;
            cubew_allocator.memory_stats.reallocs.min_allocations =
                ( cubew_allocator.memory_stats.reallocs.min_allocations == 0 )
                ? nsize
                :
                (
                    ( nsize < cubew_allocator.memory_stats.reallocs.min_allocations )
                    ? nsize
                    :
                    cubew_allocator.memory_stats.reallocs.min_allocations
                )
            ;
        }
        else
        {
            cubew_allocator.memory_stats.extmallocs.total_allocated -= osize;
            cubew_allocator.memory_stats.extmallocs.total_allocated += nsize;
            cubew_allocator.memory_stats.extmallocs.nallocs++;
            cubew_allocator.memory_stats.extmallocs.max_allocations =
                ( nsize > cubew_allocator.memory_stats.extmallocs.max_allocations )
                ? nsize
                :
                cubew_allocator.memory_stats.extmallocs.max_allocations
            ;
            cubew_allocator.memory_stats.extmallocs.min_allocations =
                ( cubew_allocator.memory_stats.extmallocs.min_allocations == 0 )
                ? nsize
                : (
                    ( nsize < cubew_allocator.memory_stats.extmallocs.min_allocations )
                    ? nsize
                    :
                    cubew_allocator.memory_stats.extmallocs.min_allocations
                    )
            ;
        }
    }
    track_highwater_mark( nsize - osize, 1 );
}



static
void
track_free( void* ptr )
{
    size_t size = (  *( ( size_t* )( ( char* )ptr - BLOCK_SIZE_SHIFT ) ) );
    if ( cubew_allocator.tracking_on != 0 )
    {
        /* free page only if no external malloc given or it reports being thir pointer*/
        if ( cubew_allocator.alloc != 0 && cubew_allocator.is_external_memory( ptr, cubew_allocator.user_ptr ) == 1 )
        {
            cubew_allocator.memory_stats.extfrees.total_allocated += size;
            cubew_allocator.memory_stats.extfrees.nallocs++;
            cubew_allocator.memory_stats.extfrees.max_allocations =
                ( size > cubew_allocator.memory_stats.extfrees.max_allocations )
                ? size
                :
                cubew_allocator.memory_stats.extfrees.max_allocations
            ;
            cubew_allocator.memory_stats.extfrees.min_allocations =
                ( cubew_allocator.memory_stats.extfrees.min_allocations == 0 )
                ? size
                :
                (
                    ( size < cubew_allocator.memory_stats.extfrees.min_allocations )
                    ? size
                    :
                    cubew_allocator.memory_stats.extfrees.min_allocations
                )
            ;
        }
        else
        {
            cubew_allocator.memory_stats.frees.total_allocated += size;
            cubew_allocator.memory_stats.frees.nallocs++;
            cubew_allocator.memory_stats.frees.max_allocations =
                ( size > cubew_allocator.memory_stats.frees.max_allocations )
                ? size
                :
                cubew_allocator.memory_stats.frees.max_allocations
            ;
            cubew_allocator.memory_stats.frees.min_allocations =
                ( cubew_allocator.memory_stats.frees.min_allocations == 0 )
                ? size
                :
                (
                    ( size < cubew_allocator.memory_stats.frees.min_allocations )
                    ? size
                    :
                    cubew_allocator.memory_stats.frees.min_allocations
                )
            ;
        }
    }
    track_highwater_mark( size, -1 );
}




void
cubew_report_allocs()
{
    if (  cubew_allocator.tracking_on == 1 )
    {
        size_t total_allocated =
            cubew_allocator.memory_stats.extmallocs.total_allocated +
            cubew_allocator.memory_stats.mallocs.total_allocated +
            cubew_allocator.memory_stats.callocs.total_allocated
        ;
        size_t max_allocations =
            max( cubew_allocator.memory_stats.extmallocs.max_allocations,
                 max( cubew_allocator.memory_stats.mallocs.max_allocations,
                      cubew_allocator.memory_stats.callocs.max_allocations ) );

        size_t min_allocations =
            min( cubew_allocator.memory_stats.extmallocs.min_allocations,
                 min( cubew_allocator.memory_stats.mallocs.min_allocations,
                      cubew_allocator.memory_stats.callocs.min_allocations ) );

        size_t nallocs =
            cubew_allocator.memory_stats.extmallocs.nallocs +
            cubew_allocator.memory_stats.mallocs.nallocs +
            cubew_allocator.memory_stats.callocs.nallocs
        ;

        size_t total_freed =
            cubew_allocator.memory_stats.extfrees.total_allocated +
            cubew_allocator.memory_stats.frees.total_allocated
        ;
        size_t max_freed =
            max( cubew_allocator.memory_stats.extfrees.max_allocations,
                 cubew_allocator.memory_stats.frees.max_allocations );

        size_t min_freed =
            min( cubew_allocator.memory_stats.extfrees.min_allocations,
                 cubew_allocator.memory_stats.frees.min_allocations );

        size_t nfreed =
            cubew_allocator.memory_stats.extfrees.nallocs +
            cubew_allocator.memory_stats.frees.nallocs
        ;



        printf( " =========== Cube writer memory usage statistics ===========\n" );
        printf( " Type       : #           Size        Max         Min\n" );
        printf( " -----------------------------------------------------------\n" );
        printf( " Hightwater : %-12zu%-12zu\n",
                cubew_allocator.memory_stats.highwater_nallocs,
                cubew_allocator.memory_stats.highwater_memory
                );
        printf( " -----------------------------------------------------------\n" );
        printf( " Malloc     : %-12zu%-12zu%-12zu%-12zu\n",
                cubew_allocator.memory_stats.mallocs.nallocs,
                cubew_allocator.memory_stats.mallocs.total_allocated,
                cubew_allocator.memory_stats.mallocs.max_allocations,
                cubew_allocator.memory_stats.mallocs.min_allocations
                );
        printf( " Calloc     : %-12zu%-12zu%-12zu%-12zu\n",
                cubew_allocator.memory_stats.callocs.nallocs,
                cubew_allocator.memory_stats.callocs.total_allocated,
                cubew_allocator.memory_stats.callocs.max_allocations,
                cubew_allocator.memory_stats.callocs.min_allocations
                );
        printf( " Ext.Malloc : %-12zu%-12zu%-12zu%-12zu\n",
                cubew_allocator.memory_stats.extmallocs.nallocs,
                cubew_allocator.memory_stats.extmallocs.total_allocated,
                cubew_allocator.memory_stats.extmallocs.max_allocations,
                cubew_allocator.memory_stats.extmallocs.min_allocations
                );
        printf( " Total      : %-12zu%-12zu%-12zu%-12zu\n",
                nallocs,
                total_allocated,
                max_allocations,
                min_allocations
                );
        printf( " -----------------------------------------------------------\n" );
        printf( " Frees(sys) : %-12zu%-12zu%-12zu%-12zu\n",
                cubew_allocator.memory_stats.frees.nallocs,
                cubew_allocator.memory_stats.frees.total_allocated,
                cubew_allocator.memory_stats.frees.max_allocations,
                cubew_allocator.memory_stats.frees.min_allocations

                );
        printf( " Frees(ext) : %-12zu%-12zu%-12zu%-12zu\n",
                cubew_allocator.memory_stats.extfrees.nallocs,
                cubew_allocator.memory_stats.extfrees.total_allocated,
                cubew_allocator.memory_stats.extfrees.max_allocations,
                cubew_allocator.memory_stats.extfrees.min_allocations
                );
        printf( " Total      : %-12zu%-12zu%-12zu%-12zu\n",
                nfreed,
                total_freed,
                max_freed,
                min_freed
                );
        printf( " -----------------------------------------------------------\n" );
        printf( " Realloc    : %-12zu%-12zu%-12zu%-12zu\n",
                cubew_allocator.memory_stats.reallocs.nallocs,
                cubew_allocator.memory_stats.reallocs.total_allocated,
                cubew_allocator.memory_stats.reallocs.max_allocations,
                cubew_allocator.memory_stats.reallocs.min_allocations
                );
        printf( " Tracking   :             %-12zu\n",
                cubew_allocator.memory_stats.highwater_nallocs * BLOCK_SIZE_SHIFT
                );
        printf( " ===========================================================\n" );
    }
}



#endif


void
cubew_free( void* ptr,
            char* message )
{
    if ( ptr == NULL )
    {
        return;
    }
    track_free( ptr ); /* we have to track before releasing*/
    /* free page only if no external malloc given or it reports being thir pointer*/
    if ( cubew_allocator.alloc == 0 || ( cubew_allocator.is_external_memory != 0 && cubew_allocator.is_external_memory( ptr, cubew_allocator.user_ptr ) == 0 ) )
    {
        free( ( char* )ptr - BLOCK_SIZE_SHIFT );
    }
#ifdef HAVE_MEMORY_TRACING
    UTILS_WARNING( "%d %d %d [CUBEW FREE] : %s %d", cubew_allocator.memory_stats.allocations_counter, cubew_allocator.memory_stats.current_nallocs, cubew_allocator.memory_stats.current_memory, message, ( char* )ptr - BLOCK_SIZE_SHIFT );
#endif
}


void*
cubew_malloc( size_t size,
              char*  message )
{
    void* ptr = NULL;
    int   ext = 1;
    if ( cubew_allocator.alloc != 0 )
    {
        ptr = cubew_allocator.alloc( size + BLOCK_SIZE_SHIFT, cubew_allocator.user_ptr );
    }
    if ( ptr == NULL )
    {
        ptr = malloc( size  + BLOCK_SIZE_SHIFT );
        ext = 0;
    }
    track_malloc( ptr,  size, ext );
#ifdef HAVE_MEMORY_TRACING
    UTILS_WARNING( "%d %d %d %d [CUBEW MALLOC] : %s (%d, %d) ", cubew_allocator.memory_stats.allocations_counter, cubew_allocator.memory_stats.current_nallocs, cubew_allocator.memory_stats.current_memory, size,  message, ( ptr ), ( ext )  );
#endif
    return ( void* )( ( char* )ptr + BLOCK_SIZE_SHIFT );  //
}

void*
cubew_calloc( size_t nitems,
              size_t size,
              char*  message )
{
    void* ptr = NULL;
    int   ext = 1;
    if ( cubew_allocator.alloc != 0 )
    {
        ptr = cubew_allocator.alloc( size * nitems  + BLOCK_SIZE_SHIFT, cubew_allocator.user_ptr  );
        if ( ptr != NULL )
        {
            memset( ( char* )ptr + BLOCK_SIZE_SHIFT, 0, size * nitems );
        }
    }
    if ( ptr == NULL )
    {
        ptr = malloc( nitems * size  + BLOCK_SIZE_SHIFT );
        memset( ( char* )ptr + BLOCK_SIZE_SHIFT, 0, size * nitems );
        ext = 0;
    }

    track_calloc( ptr,  size * nitems, ext );
 #ifdef HAVE_MEMORY_TRACING
    UTILS_WARNING( "%d %d %d %d %d [CUBEW CALLOC] : %s (%d %d)", cubew_allocator.memory_stats.allocations_counter, cubew_allocator.memory_stats.current_nallocs, cubew_allocator.memory_stats.current_memory, nitems, size,  message, ( ptr ), ( ext ) );
#endif
    return ( char* )ptr + BLOCK_SIZE_SHIFT;
}

/*
 * changed signature of realloc, because in cube realloc used only
   in cubew_vector.h and it is growing win deterministic way.
 */
void*
cubew_realloc( void*  ptr,
               size_t osize,
               size_t nsize,
               char*  message )
{
    void* new_ptr = NULL;
    int   ext     = 1;

    if ( nsize == 0 ) /* works like free*/
    {
        cubew_free( ptr, "realloc: new size is zero" );
    }
    else if ( ptr == NULL ) /* works like malloc*/
    {
        new_ptr = cubew_malloc( nsize, "realloc: pointer is zero, alloc" );
    }
    else /* nsize <= osize */
    if ( nsize <= osize )
    {
#ifdef HAVE_MEMORY_TRACING
        UTILS_WARNING( "realloc: same" );
#endif
        return ptr;     /* return it without changing */
    }
    else
    {
        if ( cubew_allocator.alloc != 0 )
        {
            new_ptr = cubew_allocator.alloc( nsize + BLOCK_SIZE_SHIFT, cubew_allocator.user_ptr );
            if ( new_ptr == NULL )
            {
                new_ptr = malloc( nsize + BLOCK_SIZE_SHIFT );
                ext     = 0;
            }
            track_malloc( new_ptr, nsize, ext );
            memset( ( char* )new_ptr + BLOCK_SIZE_SHIFT, 0, nsize );
            memcpy( ( char* )new_ptr + BLOCK_SIZE_SHIFT, ptr, osize );
            cubew_free( ptr, "realloc: release old pointer" );
        }
        else
        {
            new_ptr = realloc( ( ptr != NULL ) ? ( ( char* )ptr - BLOCK_SIZE_SHIFT ) : NULL, nsize + BLOCK_SIZE_SHIFT );
            ext     = 0;
            track_realloc( new_ptr, osize, nsize, ext );
        }
        new_ptr = ( char* )new_ptr +  BLOCK_SIZE_SHIFT; /* correct here, coz cubew_malloc above delivers shifter pointer*/
    }
#ifdef HAVE_MEMORY_TRACING
    UTILS_WARNING( "%d %d %d %d %d %d [CUBEW REALLOC] : %s  (%d %d )", cubew_allocator.memory_stats.allocations_counter, cubew_allocator.memory_stats.current_nallocs, cubew_allocator.memory_stats.current_memory, ptr, osize, nsize,  message, ptr, ext );
#endif
    return new_ptr;
}




void
cubew_init_allocs( void* ( *own_alloc )( size_t, void* ),
                   bool ( * is_external_mem )( void*, void* ),
                   void*  usrptr )
{
    cubew_allocator.alloc              = own_alloc;
    cubew_allocator.is_external_memory = is_external_mem;
    cubew_allocator.user_ptr           = usrptr;
    char* pTracking;
    cubew_allocator.tracking_on                      = 0;
    cubew_allocator.initialized                      = 1;
    cubew_allocator.memory_stats.allocations_counter = 0;

    pTracking = getenv( "SCOREP_DEVELOPMENT_MEMORY_STATS" );
    if ( pTracking == NULL )
    {
        pTracking = getenv( "CUBEW_DEVELOPMENT_MEMORY_STATS" );
    }
    if ( pTracking != NULL )
    {
        cubew_allocator.tracking_on = 1;
    }
}

int
cubew_initialized( void )
{
    return cubew_allocator.initialized;
}

#endif
