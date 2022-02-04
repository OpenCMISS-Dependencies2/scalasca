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

#include <otf2/otf2.h>

#include <otf2/OTF2_Pthread_Locks.h>

#include <cti.h>

#include <otf2-cti.h>

#include <otf2-test/check.h>

#define OTF2_TEST_FLUSH_DEFAULT OTF2_FLUSH
#include <otf2-test/flush.h>

#define OTF2_TEST_CHUNK_POOL_CHUNK_SIZE OTF2_CHUNK_SIZE_MIN
#define OTF2_TEST_CHUNK_POOL_NUMBER_OF_CHUNKS ( ( 4 * 1024 * 1024 ) / OTF2_TEST_CHUNK_POOL_CHUNK_SIZE )
#include <otf2-test/chunk-pool.h>

struct otf2_writer_location
{
    OTF2_EvtWriter* evt_writer;
    uint64_t        timer_source;
};

static void
write_events( struct otf2_writer_location* location,
              OTF2_RegionRef               region )
{
    /* timestamp + record-id + region-id + 1 */
    uint32_t number_of_event_pairs = OTF2_TEST_CHUNK_POOL_CHUNK_SIZE / ( 2 * ( 9 + 1 + 1 + 1 ) );
    for ( uint32_t i = 0; i < number_of_event_pairs; i++ )
    {
        OTF2_EvtWriter_Enter( location->evt_writer,
                              NULL,
                              location->timer_source++,
                              region );
        OTF2_EvtWriter_Leave( location->evt_writer,
                              NULL,
                              location->timer_source++,
                              region );
    }
}

int
main( int argc, char* argv[] )
{
    CTI_Init( &argc, &argv );

    int size;
    CTI_Group_size( CTI_GROUP_WORLD, &size );
    int rank;
    CTI_Group_rank( CTI_GROUP_WORLD, &rank );

    char*    trace_name        = argv[ 1 ];
    bool     write_local_defs  = !!atoi( argv[ 2 ] );
    uint32_t number_of_threads = 1;
    if ( argc > 3 )
    {
        number_of_threads = strtoul( argv[ 3 ], NULL, 10 );
    }
    bool empty_threads = false;
    if ( argc > 4 )
    {
        empty_threads = !!atoi( argv[ 4 ] );
    }
    uint32_t number_of_files = 0;
    bool     use_sion        = false;
    if ( argc > 5 )
    {
        number_of_files = strtoul( argv[ 5 ], NULL, 10 );
        use_sion        = true;
    }

    OTF2_ErrorCode status;

    /* Create new archive handle. */
    OTF2_Archive* archive = OTF2_Archive_Open( ".",
                                               trace_name,
                                               OTF2_FILEMODE_WRITE,
                                               OTF2_TEST_CHUNK_POOL_CHUNK_SIZE,
                                               OTF2_UNDEFINED_UINT64,
                                               use_sion
                                               ? OTF2_SUBSTRATE_SION
                                               : OTF2_SUBSTRATE_POSIX,
                                               OTF2_COMPRESSION_NONE );
    check_pointer( archive, "Create archive" );


    status = OTF2_Archive_SetFlushCallbacks( archive,
                                             &otf2_test_flush_callbacks,
                                             NULL );
    check_status( status, "Set flush callbacks." );
    status = OTF2_Archive_SetMemoryCallbacks( archive,
                                              &otf2_test_chunk_pool_memory_callbacks,
                                              NULL );
    check_status( status, "Set memory callbacks." );

    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
#if HAVE( PTHREAD_MUTEX_ERRORCHECK )
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
#endif
    status = OTF2_Pthread_Archive_SetLockingCallbacks( archive, &attr );
    check_status( status, "Set Pthread locking callbacks." );

    status = OTF2_Archive_SetDescription( archive, "" );
    check_status( status, "Set description." );
    status = OTF2_Archive_SetCreator( archive, PACKAGE_STRING " test writer" );
    check_status( status, "Set creator." );

    /* simulate a late MPP init */
    struct otf2_writer_location* locations =
        calloc( number_of_threads, sizeof( *locations ) );
    locations[ 0 ].evt_writer = OTF2_Archive_GetEvtWriter( archive, OTF2_UNDEFINED_LOCATION );
    check_pointer( locations[ 0 ].evt_writer, "Get event writer." );

    /* Each location has its own region which global id match the location id,
       when using local definitions we use 0 as the id and map it to the global id */
    #define region_id( i ) ( ( ( i ) * size + rank ) * write_local_defs )

    /* write first events before MPP init */
    write_events( &locations[ 0 ], region_id( 0 ) );

    /* init MPP */
    if ( 0 == number_of_files )
    {
        status = OTF2_CTI_Archive_SetCollectiveCallbacks( archive,
                                                          CTI_GROUP_WORLD,
                                                          CTI_GROUP_NULL );
    }
    else
    {
        status = OTF2_CTI_Archive_SetCollectiveCallbacksSplit( archive,
                                                               CTI_GROUP_WORLD,
                                                               number_of_files );
    }
    check_status( status, "Set collectives mode." );

    status = OTF2_Archive_OpenEvtFiles( archive );
    check_status( status, "Open event files." );

    /* Set the location ID for the master threads. */
    status = OTF2_EvtWriter_SetLocationID( locations[ 0 ].evt_writer, rank );
    check_status( status, "Set location id." );

    /* Open event writers for the threads */
    for ( int i = 1; i < number_of_threads; i++ )
    {
        locations[ i ].evt_writer =
            OTF2_Archive_GetEvtWriter( archive, i * size + rank );
        check_pointer( locations[ i ].evt_writer, "Get event writer." );
    }

    if ( !empty_threads )
    {
        /* trigger buffer flushes and also file buffer flushes (4MB) */
        for ( int j = 1; j < 2 * OTF2_TEST_CHUNK_POOL_NUMBER_OF_CHUNKS; j++ )
        {
            for ( int i = 0; i < number_of_threads; i++ )
            {
                write_events( &locations[ i ], region_id( i ) );
            }
        }
    }

    /* The master thread also does some more events at the end */
    write_events( &locations[ 0 ], region_id( 0 ) );

    uint64_t* number_of_events =
        calloc( number_of_threads, sizeof( *number_of_events ) );
    for ( int i = 0; i < number_of_threads; i++ )
    {
        OTF2_EvtWriter_GetNumberOfEvents( locations[ i ].evt_writer,
                                          &number_of_events[ i ] );
        status = OTF2_Archive_CloseEvtWriter( archive,
                                              locations[ i ].evt_writer );
        check_status( status, "Close event writer." );
    }

    status = OTF2_Archive_CloseEvtFiles( archive );
    check_status( status, "Close event files." );

    status = OTF2_Archive_SetDefChunkSize( archive,
                                           1024 * 1024 );
    check_status( status, "Setting definition chunk size." );

    /* write local definitions */
    if ( write_local_defs )
    {
        status = OTF2_Archive_OpenDefFiles( archive );
        check_status( status, "Open local definitions files." );

        OTF2_IdMap* region_map = OTF2_IdMap_Create( OTF2_ID_MAP_SPARSE, 1 );
        check_pointer( region_map, "Create region id map." );

        for ( int i = 0; i < number_of_threads; i++ )
        {
            OTF2_DefWriter* def_writer =
                OTF2_Archive_GetDefWriter( archive, i * size + rank );
            check_pointer( def_writer, "Get local definitions writer." );

            status = OTF2_IdMap_AddIdPair( region_map,
                                           i * size + rank,
                                           0 );
            check_status( status, "Add the mapping entry." );

            status = OTF2_DefWriter_WriteMappingTable( def_writer,
                                                       OTF2_MAPPING_REGION,
                                                       region_map );
            check_status( status, "Write the mapping." );

            status = OTF2_IdMap_Clear( region_map );
            check_status( status, "Clear ID map." );

            status = OTF2_Archive_CloseDefWriter( archive,
                                                  def_writer );
            check_status( status, "Close local definitions writer." );
        }

        OTF2_IdMap_Free( region_map );

        status = OTF2_Archive_CloseDefFiles( archive );
        check_status( status, "Close local definitions files." );
    }

    uint64_t* number_of_events_per_locations = NULL;
    if ( 0 == rank )
    {
        number_of_events_per_locations =
            calloc( size * number_of_threads,
                    sizeof( *number_of_events_per_locations ) );
    }
    CTI_Gather( CTI_GROUP_WORLD,
                number_of_events,
                number_of_events_per_locations,
                number_of_threads * sizeof( *number_of_events_per_locations ),
                0 );
    free( number_of_events );

    uint64_t last_timestamp = 0;
    for ( int i = 0; i < number_of_threads; i++ )
    {
        if ( last_timestamp < locations[ i ].timer_source )
        {
            last_timestamp = locations[ i ].timer_source;
        }
    }
    free( locations );

    uint64_t* last_timestamp_per_locations = NULL;
    if ( 0 == rank )
    {
        last_timestamp_per_locations =
            calloc( size, sizeof( *last_timestamp_per_locations ) );
    }
    CTI_Gather( CTI_GROUP_WORLD,
                &last_timestamp,
                last_timestamp_per_locations,
                sizeof( last_timestamp ),
                0 );

    if ( 0 == rank )
    {
        for ( int i = 1; i < size; i++ )
        {
            if ( last_timestamp_per_locations[ i ] > last_timestamp )
            {
                last_timestamp = last_timestamp_per_locations[ i ];
            }
        }
        free( last_timestamp_per_locations );

        OTF2_GlobalDefWriter* global_def_writer =
            OTF2_Archive_GetGlobalDefWriter( archive );
        check_pointer( global_def_writer, "Get global definitions writer." );

        OTF2_GlobalDefWriter_WriteClockProperties( global_def_writer,
                                                   1,
                                                   0,
                                                   last_timestamp );

        OTF2_GlobalDefWriter_WriteString( global_def_writer,
                                          0,
                                          "" );


        OTF2_GlobalDefWriter_WriteRegion( global_def_writer,
                                          0,
                                          0,
                                          0,
                                          0,
                                          OTF2_REGION_ROLE_FUNCTION,
                                          OTF2_PARADIGM_USER,
                                          OTF2_REGION_FLAG_NONE,
                                          0,
                                          0,
                                          0 );

        OTF2_GlobalDefWriter_WriteSystemTreeNode(
            global_def_writer,
            0,
            0,
            0,
            OTF2_UNDEFINED_SYSTEM_TREE_NODE );

        for ( int i = 0; i < size; i++ )
        {
            OTF2_GlobalDefWriter_WriteLocationGroup(
                global_def_writer,
                i,
                0,
                OTF2_LOCATION_GROUP_TYPE_PROCESS,
                0 );

            for ( int j = 0; j < number_of_threads; j++ )
            {
                OTF2_GlobalDefWriter_WriteLocation(
                    global_def_writer,
                    j * size + i,
                    0,
                    OTF2_LOCATION_TYPE_CPU_THREAD,
                    number_of_events_per_locations[ j * size + i ],
                    i );
            }
        }

        free( number_of_events_per_locations );

        status = OTF2_Archive_CloseGlobalDefWriter( archive, global_def_writer );
        check_status( status, "Closing global definition writer." );
    }
    CTI_Barrier( CTI_GROUP_WORLD );

    status = OTF2_Archive_Close( archive );
    check_status( status, "Close archive." );

    CTI_Finalize();

    return 0;
}
