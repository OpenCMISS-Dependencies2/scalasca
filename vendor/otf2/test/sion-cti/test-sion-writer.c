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

    char* file_name      = argv[ 1 ];
    char* real_file_name = NULL;
    int   filenumber     = 0;
    int   numfiles       = 1;
    int   local_rank     = rank;
    int   local_size     = size;

    if ( argc > 2 )
    {
        numfiles = atoi( argv[ 2 ] );
        if ( numfiles < 1 )
        {
            numfiles = 1;
        }
        if ( numfiles > size )
        {
            numfiles = size;
        }

        filenumber = 0;
        int rem = size % numfiles;
        local_size = size / numfiles + !!rem;
        local_rank = 0;
        int local_root = 0;
        for ( int i = 0; i < rank; i++ )
        {
            local_rank++;
            if ( local_root + local_size == i + 1 )
            {
                local_root += local_size;
                filenumber++;
                local_size -= filenumber == rem;
                local_rank  = 0;
            }
        }
    }

    sion_int64 chunksize = 1024 * 1024;
    sion_int32 fsblksize = -1;
    printf( "sion_generic_paropen( aid=%d, fname=\"%s\", mode=\"bw\", chunksize=%zu, fsblksize=%d, global, grank=%u, gsize=%u, filenumber=%u, numfiles=%u, lrank=%u, lsize=%u, fp=NULL, newfname=NULL )\n",
            api_id,
            file_name,
            ( size_t )chunksize,
            fsblksize,
            rank,
            size,
            filenumber,
            numfiles,
            local_rank,
            local_size );
    int sion_id = sion_generic_paropen( api_id,
                                        file_name,
                                        "bw",
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

    printf( "sion_generic_paropen( ..., grank=%u, gsize=%u, ... ) = ( sid=%d )\n",
            rank,
            size,
            sion_id );

    if ( sion_id < 0 )
    {
        fprintf( stderr, "error opening SION file\n" );
        exit( 1 );
    }

    int* chunk              = malloc( chunksize );
    int  elements_per_chunk = chunksize / sizeof( *chunk );
    int  number             = 0;
    for ( int i = 0; i < 2; i++ )
    {
        /* fill chunk */
        for ( int j = 0; j < elements_per_chunk; j++ )
        {
            chunk[ j ] = number * size + rank;
            number++;
        }

        /* write chunk */
        printf( "sion_fwrite( %p, %zu, 1, %d )\n",
                chunk, ( size_t )chunksize, sion_id );
        int err = sion_fwrite( chunk, chunksize, 1, sion_id );
        printf( "sion_fwrite( %p, %zu, 1, %d ) = %d\n",
                chunk, ( size_t )chunksize, sion_id, err );
        if ( err != 1 )
        {
            fprintf( stderr, "%d: write error\n", rank );
            ret = 1;
        }
    }
    free( chunk );

    sion_generic_parclose( sion_id );

    CTI_Finalize();

    return ret;
}
