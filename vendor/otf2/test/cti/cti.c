/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>

#include <pthread.h>

#include "cti.h"

static char*
cti_strdup( const char* str )
{
    size_t len = strlen( str );
    char*  cpy = malloc( len + 1 );
    memcpy( cpy, str, len + 1 );
    return cpy;
}

struct cti_group
{
    int                rank, size;
    void**             op_data;
    size_t*            aux_data;
    pthread_barrier_t* barrier;
};

struct cti_rank_info
{
    int              ac, ec;
    char**           av;
    struct cti_group world;
};

static pthread_key_t cti_rank_info_key;
static int           cti_world_size;
static pthread_t*    cti_threads;
static int           cti_initialized;

CTI_Group
cti_get_group_world( void )
{
    struct cti_rank_info* info = pthread_getspecific( cti_rank_info_key );
    return &info->world;
}

static int
cti_barrier( struct cti_group* group )
{
    int ret = pthread_barrier_wait( group->barrier );
    if ( ret == 0 || ret == PTHREAD_BARRIER_SERIAL_THREAD )
    {
        return CTI_SUCCESS;
    }
    return ret;
}

static void*
cti_start_thread( void* _info )
{
    struct cti_rank_info* info = _info;
    pthread_setspecific( cti_rank_info_key, info );

    /* TODO: copy env after av */
    char** orig_av = info->av;
    int    i;
    info->av = malloc( ( info->ac + 1 + info->ec + 1 ) * sizeof( char* ) );
    for ( i = 0; i < info->ac; i++ )
    {
        info->av[ i ] = cti_strdup( orig_av[ i ] );
    }
    info->av[ i++ ] = NULL;
    for (; i < info->ac + 1 + info->ec; i++ )
    {
        info->av[ i ] = cti_strdup( orig_av[ i ] );
    }
    info->av[ i++ ] = NULL;

    extern int
    main( int    ac,
          char** av );

    main( info->ac, info->av );

    /* never reached, application should have called CTI_Finalize()
       before leaving main */
    return NULL;
}

int
CTI_Init( int*    ac,
          char*** av )
{
    if ( !ac || !av )
    {
        return EINVAL;
    }

    if ( !cti_initialized )
    {
        /* master thread */
        cti_initialized = 1;

        cti_world_size = 0;
        int i;
        int skip = 2;
        for ( i = 1; i < *ac; i++ )
        {
            char* arg      = ( *av )[ i ];
            char* next_arg = ( *av )[ i + 1 ];
            char* nt_arg   = NULL;
            /* make --nt and -nt equal */
            if ( arg[ 0 ] == '-' && arg[ 1 ] == '-' )
            {
                arg++;
            }
            if ( !strncmp( arg, "-nt=", 4 ) )
            {
                nt_arg = arg + 4;
                skip   = 1;
            }
            if ( !strcmp( arg, "-nt" ) )
            {
                if ( !next_arg )
                {
                    /* missing arguement for -nt/--nt */
                    return EINVAL;
                }
                nt_arg = next_arg;
            }
            if ( nt_arg )
            {
                char* ptr;
                long  sizel = strtol( nt_arg, &ptr, 10 );
                if ( *ptr || ptr == nt_arg || sizel <= 0 || sizel >= INT_MAX )
                {
                    return EINVAL;
                }
                cti_world_size = ( int )sizel;
                break;
            }
        }
        if ( i == *ac ) /* missing -nt/--nt */
        {
            return EINVAL;
        }
        for ( int j = i + skip; j <= *ac; i++, j++ )
        {
            ( *av )[ i ] = ( *av )[ j ];
        }
        ( *ac ) -= skip;
        int    ec = 0;
        char** ev = ( *av ) + *ac + 1;
        while ( *ev )
        {
            ec++, ev++;
        }

        pthread_key_create( &cti_rank_info_key, NULL );

        cti_threads = malloc( cti_world_size * sizeof( *cti_threads ) );
        struct cti_rank_info* info = malloc( sizeof( *info ) );

        size_t*            aux_data = malloc( cti_world_size * sizeof( *aux_data ) );
        void**             op_data  = malloc( sizeof( *op_data ) );
        pthread_barrier_t* barrier  = malloc( sizeof( *barrier ) );
        pthread_barrier_init( barrier, NULL, cti_world_size );

        info->ac = 0;    /* not used by root */
        info->ec = 0;    /* not used by root */
        info->av = NULL; /* not used by root */

        info->world.rank     = 0;
        info->world.size     = cti_world_size;
        info->world.barrier  = barrier;
        info->world.op_data  = op_data;
        info->world.aux_data = aux_data;
        pthread_setspecific( cti_rank_info_key, info );

        for ( int i = 1; i < cti_world_size; i++ )
        {
            info                 = malloc( sizeof( *info ) );
            info->ac             = *ac;
            info->av             = *av;
            info->ec             = ec;
            info->world.rank     = i;
            info->world.size     = cti_world_size;
            info->world.barrier  = barrier;
            info->world.op_data  = op_data;
            info->world.aux_data = aux_data;
            pthread_create( &cti_threads[ i ], NULL, cti_start_thread, info );
        }
    }

    cti_barrier( CTI_GROUP_WORLD );

    return CTI_SUCCESS;
}

int
CTI_Finalize( void )
{
    if ( !cti_initialized )
    {
        return EINVAL;
    }

    cti_barrier( CTI_GROUP_WORLD );

    struct cti_rank_info* info = pthread_getspecific( cti_rank_info_key );
    pthread_setspecific( cti_rank_info_key, NULL );
    if ( info->world.rank != 0 )
    {
        char** av = info->av;
        while ( *av ) /* argv */
        {
            free( *av++ );
        }
        while ( *av ) /* env */
        {
            free( *av++ );
        }
        free( info->av );
        free( info );
        pthread_exit( NULL );
    }

    for ( int i = 1; i < cti_world_size; i++ )
    {
        pthread_join( cti_threads[ i ], NULL );
    }

    /* destroy our group world */
    free( info->world.op_data );
    free( info->world.aux_data );
    pthread_barrier_destroy( info->world.barrier );
    free( info->world.barrier );
    free( info );

    /* destroy global state */
    free( cti_threads );
    cti_threads = NULL;
    pthread_key_delete( cti_rank_info_key );
    cti_initialized = 0;

    return CTI_SUCCESS;
}

int
CTI_Group_size( CTI_Group group,
                int*      size )
{
    if ( !size )
    {
        return EINVAL;
    }

    *size = group->size;
    return CTI_SUCCESS;
}

int
CTI_Group_rank( CTI_Group group,
                int*      rank )
{
    if ( !rank )
    {
        return EINVAL;
    }

    *rank = group->rank;
    return CTI_SUCCESS;
}

int
CTI_Group_dup( CTI_Group  group,
               CTI_Group* newgroup )
{
    cti_barrier( group );

    if ( group->rank == 0 )
    {
        *group->op_data = malloc( 3 * sizeof( void* ) );
    }
    cti_barrier( group );

    void**            new_data  = *group->op_data;
    struct cti_group* new_group = malloc( sizeof( *new_group ) );
    new_group->rank = group->rank;
    new_group->size = group->size;
    if ( new_group->rank == 0 )
    {
        new_group->op_data  = malloc( sizeof( *new_group->op_data ) );
        new_data[ 0 ]       = new_group->op_data;
        new_group->aux_data = malloc( new_group->size * sizeof( *new_group->aux_data ) );
        new_data[ 1 ]       = new_group->aux_data;
        new_group->barrier  = malloc( sizeof( *new_group->barrier ) );
        new_data[ 2 ]       = new_group->barrier;
        pthread_barrier_init( new_group->barrier, NULL, new_group->size );
    }
    cti_barrier( group );

    if ( new_group->rank != 0 )
    {
        new_group->op_data  = new_data[ 0 ];
        new_group->aux_data = new_data[ 1 ];
        new_group->barrier  = new_data[ 2 ];
    }
    cti_barrier( group );

    if ( group->rank == 0 )
    {
        free( *group->op_data );
        *group->op_data = NULL;
    }
    cti_barrier( group );

    *newgroup = new_group;

    return CTI_SUCCESS;
}

int
CTI_Group_free( CTI_Group* group )
{
    if ( *group == CTI_GROUP_WORLD )
    {
        return EINVAL;
    }

    cti_barrier( *group );

    if ( ( *group )->rank == 0 )
    {
        free( ( *group )->op_data );
        free( ( *group )->aux_data );
        pthread_barrier_destroy( ( *group )->barrier );
        free( ( *group )->barrier );
    }
    free( *group );
    *group = NULL;

    return CTI_SUCCESS;
}

static int
compare( const void* _a, const void* _b )
{
    const int* a = _a, * b = _b;

    int res = ( a[ 0 ] > b[ 0 ] ) - ( a[ 0 ] < b[ 0 ] );
    if ( res )
    {
        return res;
    }

    res = ( a[ 1 ] > b[ 1 ] ) - ( a[ 1 ] < b[ 1 ] );
    if ( res )
    {
        return res;
    }

    return ( a[ 2 ] > b[ 2 ] ) - ( a[ 2 ] < b[ 2 ] );
}

int
CTI_Group_split( CTI_Group  group,
                 int        color,
                 int        key,
                 CTI_Group* newgroup )
{
    cti_barrier( group );

    if ( group->rank == 0 )
    {
        *group->op_data = malloc( group->size * 3 * sizeof( int ) );
    }
    cti_barrier( group );

    int* results = *group->op_data;
    results[ group->rank * 3 + 0 ] = color;
    results[ group->rank * 3 + 1 ] = key;
    results[ group->rank * 3 + 2 ] = group->rank;
    cti_barrier( group );

    if ( group->rank == 0 )
    {
        qsort( results, group->size, 3 * sizeof( *results ), compare );
    }
    cti_barrier( group );

    int new_size      = 0;
    int new_rank      = 0;
    int siblings_size = 0;
    int siblings_rank = 0;
    int last_color    = !results[ 0 ];
    for ( int i = 0; i < group->size; i++ )
    {
        if ( results[ i * 3 ] == color )
        {
            if ( results[ i * 3 + 2 ] == group->rank )
            {
                new_rank = new_size;
            }
            new_size++;
        }
        if ( last_color != results[ i * 3 ] )
        {
            if ( results[ i * 3 ] == color )
            {
                siblings_rank = siblings_size;
            }
            siblings_size++;
            last_color = results[ i * 3 ];
        }
    }
    cti_barrier( group );

    if ( group->rank == 0 )
    {
        free( *group->op_data );
        *group->op_data = malloc( siblings_size * 3 * sizeof( void* ) );
    }
    cti_barrier( group );

    void**            siblings_data = *group->op_data;
    struct cti_group* new_group     = malloc( sizeof( *new_group ) );
    new_group->rank = new_rank;
    new_group->size = new_size;
    if ( new_group->rank == 0 )
    {
        new_group->op_data                     = malloc( sizeof( *new_group->op_data ) );
        siblings_data[ 3 * siblings_rank + 0 ] = new_group->op_data;
        new_group->aux_data                    = malloc( new_group->size * sizeof( *new_group->aux_data ) );
        siblings_data[ 3 * siblings_rank + 1 ] = new_group->aux_data;
        new_group->barrier                     = malloc( sizeof( *new_group->barrier ) );
        siblings_data[ 3 * siblings_rank + 2 ] = new_group->barrier;
        pthread_barrier_init( new_group->barrier, NULL, new_group->size );
    }
    cti_barrier( group );

    if ( new_group->rank != 0 )
    {
        new_group->op_data  = siblings_data[ 3 * siblings_rank + 0 ];
        new_group->aux_data = siblings_data[ 3 * siblings_rank + 1 ];
        new_group->barrier  = siblings_data[ 3 * siblings_rank + 2 ];
    }
    cti_barrier( group );

    if ( group->rank == 0 )
    {
        free( *group->op_data );
    }
    cti_barrier( group );

    *newgroup = new_group;

    return CTI_SUCCESS;
}

int
CTI_Barrier( CTI_Group group )
{
    return cti_barrier( group );
}

int
CTI_Bcast( CTI_Group group,
           void*     buf,
           size_t    size,
           int       root )
{
    cti_barrier( group );

    if ( group->rank == root )
    {
        *group->op_data = buf;
    }
    cti_barrier( group );

    if ( group->rank != root )
    {
        memcpy( buf, *group->op_data, size );
    }
    cti_barrier( group );

    return CTI_SUCCESS;
}

int
CTI_Gather( CTI_Group   group,
            const void* inbuf,
            void*       outbuf,
            size_t      size,
            int         root )
{
    cti_barrier( group );

    if ( group->rank == root )
    {
        *group->op_data = outbuf;
    }
    cti_barrier( group );

    memcpy( ( char* )( *group->op_data ) + group->rank * size, inbuf, size );
    cti_barrier( group );

    return CTI_SUCCESS;
}

int
CTI_Gatherv( CTI_Group   group,
             const void* inbuf,
             void*       outbuf,
             size_t      size,
             int         root )
{
    cti_barrier( group );

    if ( group->rank == root )
    {
        *group->op_data = outbuf;
    }
    group->aux_data[ group->rank ] = size;
    cti_barrier( group );

    if ( group->rank == root )
    {
        size_t sum = 0;
        for ( int i = 0; i < group->size; i++ )
        {
            size_t tmp = group->aux_data[ i ];
            group->aux_data[ i ] = sum;
            sum                 += tmp;
        }
    }
    cti_barrier( group );

    memcpy( ( char* )( *group->op_data ) + group->aux_data[ group->rank ], inbuf, size );
    cti_barrier( group );

    return CTI_SUCCESS;
}

int
CTI_Scatter( CTI_Group   group,
             const void* inbuf,
             void*       outbuf,
             size_t      size,
             int         root )
{
    cti_barrier( group );

    if ( group->rank == root )
    {
        *group->op_data = ( void* )inbuf;
    }
    cti_barrier( group );

    memcpy( outbuf, ( const char* )( *group->op_data ) + group->rank * size, size );
    cti_barrier( group );

    return CTI_SUCCESS;
}

int
CTI_Scatterv( CTI_Group   group,
              const void* inbuf,
              void*       outbuf,
              size_t      size,
              int         root )
{
    cti_barrier( group );

    if ( group->rank == root )
    {
        *group->op_data = ( void* )inbuf;
    }
    group->aux_data[ group->rank ] = size;
    cti_barrier( group );

    if ( group->rank == root )
    {
        size_t sum = 0;
        for ( int i = 0; i < group->size; i++ )
        {
            size_t tmp = group->aux_data[ i ];
            group->aux_data[ i ] = sum;
            sum                 += tmp;
        }
    }
    cti_barrier( group );

    memcpy( outbuf,
            ( const char* )( *group->op_data ) + group->aux_data[ group->rank ],
            size );
    cti_barrier( group );

    return CTI_SUCCESS;
}
