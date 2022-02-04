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
#include <stdarg.h>

#include <sion.h>

#include <cti.h>

#include "sion-cti.h"

static volatile int ret;

int
main( int argc, char* argv[] )
{
    CTI_Init( &argc, &argv );

    int size;
    CTI_Group_size( CTI_GROUP_WORLD, &size );
    int rank;
    CTI_Group_rank( CTI_GROUP_WORLD, &rank );

    struct sion_comm_group global_comm_group;
    global_comm_group.cti_group = CTI_GROUP_WORLD;

    int api_id = sion_cti_get_api();

    char*      file_name      = argv[ 1 ];
    char*      real_file_name = NULL;
    int        numfiles       = -1;
    sion_int32 fsblksize      = -1;

    int         sion_id;
    int         mapped_mode             = 0;
    int         nlocaltasks             = -1;
    int*        globalranks             = NULL;
    sion_int64* chunksizes              = NULL;
    int*        mapping_filenrs         = NULL;
    int*        mapping_lranks          = NULL;
    int         number_of_ranks_in_file = size;
    if ( argc > 2 )
    {
        number_of_ranks_in_file = atoi( argv[ 2 ] );
        mapped_mode             = 1;

        nlocaltasks = 0;
        globalranks = calloc( number_of_ranks_in_file, sizeof( *globalranks ) );
        for ( int i = 0; i < number_of_ranks_in_file; i++ )
        {
            if ( i % size == rank )
            {
                globalranks[ nlocaltasks++ ] = i;
            }
        }

        printf( "sion_generic_paropen_mapped( aid=%d, fname=\"%s\", mode=\"br\", numfiles=&, global, grank=%u, gsize=%u, nlocaltasks=%d, globalranks=[], chunksizes=&[], mapping_filenrs=&[], mapping_lranks=&[], fsblksize=&, fp=NULL )\n",
                api_id,
                file_name,
                rank,
                size,
                nlocaltasks );
        for ( int i = 0; i < nlocaltasks; i++ )
        {
            printf( "sion_generic_paropen_mapped( ..., grank=%u, gsize=%u, ..., globalranks[%d]=%d, ... )\n",
                    rank, size,
                    i, globalranks[ i ] );
        }

        sion_id = sion_generic_paropen_mapped(
            api_id,
            file_name,
            "br",
            &numfiles,
            &global_comm_group,
            rank,
            size,
            &nlocaltasks,
            &globalranks,
            &chunksizes,
            &mapping_filenrs,
            &mapping_lranks,
            &fsblksize,
            NULL );

        printf( "sion_generic_paropen_mapped( ..., grank=%u, gsize=%u, ... ) = ( sid=%d, numfiles=%d, fsblksize=%d )\n",
                rank, size,
                sion_id,
                numfiles,
                fsblksize );

        for ( int i = 0; i < nlocaltasks; i++ )
        {
            printf( "sion_generic_paropen_mapped( ..., grank=%u, gsize=%u, ... ) = ( chunksizes[%d]=%zu, mapping_filenrs[%d]=%d, mapping_lranks[%d]=%d )\n",
                    rank, size,
                    i, ( size_t )chunksizes[ i ],
                    i, mapping_filenrs[ i ],
                    i, mapping_lranks[ i ] );
        }
    }
    else
    {
        sion_int64 chunksize  = -1;
        int        filenumber = -1;
        int        local_size = -1;
        int        local_rank = -1;

        nlocaltasks      = 1;
        globalranks      = calloc( 1, sizeof( *globalranks ) );
        globalranks[ 0 ] = rank;

        printf( "sion_generic_paropen( aid=%d, fname=\"%s\", mode=\"br\", chunksize=&, fsblksize=&, global, grank=%u, gsize=%u, filenumber=&, numfiles=&, lrank=&, lsize=&, fp=NULL, newfname=NULL )\n",
                api_id,
                file_name,
                rank,
                size );
        sion_id = sion_generic_paropen( api_id,
                                        file_name,
                                        "br",
                                        &chunksize,
                                        &fsblksize,
                                        &global_comm_group,
                                        rank,
                                        size,
                                        &filenumber,
                                        &numfiles,
                                        &local_rank,
                                        &local_size,
                                        NULL,
                                        &real_file_name );

        printf( "sion_generic_paropen( ..., grank=%u, gsize=%u, ... ) = ( sid=%d, chunksize=%zu, fsblksize=%d, filenumber=%d, numfiles=%d, lrank=%d, lsize=%d )\n",
                rank,
                size,
                sion_id,
                ( size_t )chunksize,
                fsblksize,
                filenumber,
                numfiles,
                local_rank,
                local_size );

        chunksizes           = calloc( 1, sizeof( *chunksizes ) );
        chunksizes[ 0 ]      = chunksize;
        mapping_filenrs      = calloc( 1, sizeof( *mapping_filenrs ) );
        mapping_filenrs[ 0 ] = filenumber;
        mapping_lranks       = calloc( 1, sizeof( *mapping_lranks ) );
        mapping_lranks[ 0 ]  = local_rank;
    }

    if ( sion_id < 0 )
    {
        fprintf( stderr, "error opening SION file" );
        exit( 1 );
    }

    int*  numbers = calloc( nlocaltasks, sizeof( *numbers ) );
    int** chunks  = calloc( nlocaltasks, sizeof( *chunks ) );
    for ( int l = 0; l < nlocaltasks; l++ )
    {
        chunks[ l ] = malloc( chunksizes[ l ] );
    }
    for ( int i = 0; i < 2; i++ )
    {
        for ( int l = 0; l < nlocaltasks; l++ )
        {
            int err;
            int elements_per_chunk = chunksizes[ l ] / sizeof( int );
            if ( mapped_mode )
            {
                printf( "sion_seek( %d, %d, SION_CURRENT_CHUNK, SION_CURRENT_POS )\n",
                        sion_id,
                        globalranks[ l ] );
                err = sion_seek( sion_id,
                                 globalranks[ l ],
                                 SION_CURRENT_BLK,
                                 SION_CURRENT_POS );
                printf( "sion_seek( %d, %d, SION_CURRENT_CHUNK, SION_CURRENT_POS ) = %d\n",
                        sion_id,
                        globalranks[ l ],
                        err );
                if ( err != 1 )
                {
                    fprintf( stderr, "%d: error seeking for rank %d\n", rank, globalranks[ l ] );
                    ret = 1;
                    continue;
                }
            }

            /* read chunk */
            printf( "sion_fread( %p, %zu, 1, %d )\n",
                    chunks[ l ], ( size_t )chunksizes[ l ], sion_id );
            err = sion_fread( chunks[ l ], chunksizes[ l ], 1, sion_id );
            printf( "sion_fread( %p, %zu, 1, %d ) = %d\n",
                    chunks[ l ], ( size_t )chunksizes[ l ], sion_id, err );
            if ( err != 1 )
            {
                fprintf( stderr, "%d: error reading for rank %d\n", rank, globalranks[ l ] );
                ret = 1;
                continue;
            }

            /* check chunk */
            for ( int j = 0; j < elements_per_chunk; j++ )
            {
                if ( chunks[ l ][ j ] != ( numbers[ l ] * number_of_ranks_in_file + globalranks[ l ] ) )
                {
                    if ( chunks[ l ][ j ] / number_of_ranks_in_file != numbers[ l ] )
                    {
                        fprintf( stderr, "%d: sequence check error on rank %d\n", rank, globalranks[ l ] );
                    }
                    else
                    {
                        fprintf( stderr, "%d: rank check error on rank %d, got data from rank %d\n", rank, globalranks[ l ], chunks[ l ][ j ] % number_of_ranks_in_file );
                    }
                    ret = 1;
                }
                numbers[ l ]++;
            }
        }
    }
    for ( int l = 0; l < nlocaltasks; l++ )
    {
        free( chunks[ l ] );
    }
    free( numbers );
    free( chunks );

    if ( mapped_mode )
    {
        sion_generic_parclose_mapped( sion_id );
    }
    else
    {
        sion_generic_parclose( sion_id );
    }

    CTI_Finalize();

    return ret;
}
