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

#include "cti.h"

static void
test_group( CTI_Group world,
            CTI_Group group )
{
    int world_rank, world_size;
    CTI_Group_size( world, &world_size );
    CTI_Group_rank( world, &world_rank );

    int rank, size;
    CTI_Group_size( group, &size );
    CTI_Group_rank( group, &rank );

    CTI_Barrier( group );

    int foo = -1;
    if ( 0 == rank )
    {
        foo = 42;
    }
    CTI_Bcast( group, &foo, sizeof( foo ), 0 );
    if ( foo != 42 )
    {
        fprintf( stderr, "%d/%d/%d/%d: expected 42, but foo == %d\n",
                 world_rank, world_size,
                 rank, size,
                 foo );
        exit( 1 );
    }

    foo = size - 1 - rank;
    int* all_foos = NULL;
    if ( 0 == rank )
    {
        all_foos = calloc( size, sizeof( *all_foos ) );
        memset( all_foos, 0xff, size * sizeof( *all_foos ) );
    }
    CTI_Gather( group, &foo, all_foos, sizeof( foo ), 0 );
    if ( 0 == rank )
    {
        for ( int r = 0; r < size; r++ )
        {
            if ( all_foos[ r ] != size - 1 - r )
            {
                fprintf( stderr, "%d/%d/%d/%d: expected %d, but all_foos[%d] == %d\n",
                         world_rank, world_size,
                         rank, size,
                         size - 1 - r, r, all_foos[ r ] );
                exit( 1 );
            }
            all_foos[ r ] = r;
        }
    }
    CTI_Scatter( group, all_foos, &foo, sizeof( foo ), 0 );
    if ( foo != rank )
    {
        fprintf( stderr, "%d/%d/%d/%d: expected %d, but foo == %d\n",
                 world_rank, world_size,
                 rank, size,
                 rank, foo );
        exit( 1 );
    }
    free( all_foos );

    int* send_buzs = calloc( rank + 1, sizeof( *send_buzs ) );
    for ( int i = 0; i <= rank; i++ )
    {
        send_buzs[ i ] = rank - i;
    }
    int* all_buzs = NULL;
    if ( 0 == rank )
    {
        all_buzs = calloc( ( size * ( size + 1 ) ) / 2, sizeof( *all_buzs ) );
        memset( all_buzs, 0xff, ( ( size * ( size + 1 ) ) / 2 ) * sizeof( *all_buzs ) );
    }
    CTI_Gatherv( group, send_buzs, all_buzs, ( rank + 1 ) * sizeof( *send_buzs ), 0 );
    if ( 0 == rank )
    {
        int o = 0;
        for ( int r = 0; r < size; r++ )
        {
            for ( int i = 0; i <= r; i++ )
            {
                if ( all_buzs[ o + i ] != r - i )
                {
                    fprintf( stderr, "%d/%d/%d/%d: expected %d, but all_buzs[%d][%d] == %d\n",
                             world_rank, world_size,
                             rank, size,
                             i, r, i, all_buzs[ o + i ] );
                    exit( 1 );
                }
            }
            o += r + 1;
        }
        o = 0;
        for ( int r = 0; r < size; r++ )
        {
            for ( int i = 0; i <= size - 1 - r; i++ )
            {
                all_buzs[ o + i ] = i;
            }
            o += size - r;
        }
    }
    CTI_Barrier( group );
    int* recv_buzs = calloc( size - rank, sizeof( *recv_buzs ) );
    memset( recv_buzs, 0xff, ( size - rank ) * sizeof( *recv_buzs ) );
    CTI_Scatterv( group, all_buzs, recv_buzs, ( size - rank ) * sizeof( *recv_buzs ), 0 );
    for ( int i = 0; i <= size - 1 - rank; i++ )
    {
        if ( recv_buzs[ i ] != i )
        {
            fprintf( stderr, "%d/%d/%d/%d: expected %d, but recv_buzs[%d] == %d\n",
                     world_rank, world_size,
                     rank, size,
                     i, i, recv_buzs[ i ] );
            exit( 1 );
        }
    }
    free( send_buzs );
    free( recv_buzs );
    free( all_buzs );
}

int
main( int ac, char* av[] )
{
    CTI_Init( &ac, &av );

    CTI_Group world = CTI_GROUP_WORLD;
    int       rank, size;
    CTI_Group_size( world, &size );
    CTI_Group_rank( world, &rank );

    for ( int r = 0; r < 100; r++ )
    {
        test_group( world, world );

        CTI_Group dup;
        CTI_Group_dup( world, &dup );
        test_group( world, dup );
        CTI_Group_free( &dup );

        CTI_Group sub;

        for ( int number_of_subs = 1; number_of_subs <= size; number_of_subs++ )
        {
            int color    = 0;
            int rem      = size % number_of_subs;
            int sub_size = size / number_of_subs + !!rem;
            int key      = 0;
            int sub_root = 0;
            for ( int i = 0; i < rank; i++ )
            {
                key++;
                if ( sub_root + sub_size == i + 1 )
                {
                    sub_root += sub_size;
                    color++;
                    sub_size -= color == rem;
                    key       = 0;
                }
            }

            CTI_Group_split( world, color, key, &sub );
            test_group( world, sub );
            CTI_Group_free( &sub );

            CTI_Group_split( world, color, sub_size - 1 - key, &sub );
            test_group( world, sub );
            CTI_Group_free( &sub );
        }
    }

    CTI_Finalize();

    return 0;
}
