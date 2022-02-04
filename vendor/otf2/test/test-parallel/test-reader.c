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
#include <stdbool.h>
#include <inttypes.h>

#include <otf2/otf2.h>

#include <otf2/OTF2_Pthread_Locks.h>

#include <cti.h>

#include <otf2-cti.h>

#include <otf2-test/check.h>

struct otf2_reader_location
{
    bool             do_read;
    OTF2_LocationRef location_id;
    OTF2_EvtReader*  evt_reader;
    uint64_t         number_of_events;
    uint64_t         number_of_events_read;
};

struct otf2_reader_defs
{
    struct otf2_reader_location* locations;
    uint32_t                     locations_index;
    uint32_t                     number_of_locations;
};

static OTF2_CallbackCode
location_cb( void*                 userData,
             OTF2_LocationRef      self,
             OTF2_StringRef        name,
             OTF2_LocationType     locationType,
             uint64_t              numberOfEvents,
             OTF2_LocationGroupRef locationGroup )
{
    struct otf2_reader_defs*     global_defs = userData;
    struct otf2_reader_location* location    =
        &global_defs->locations[ global_defs->locations_index++ ];

    if ( global_defs->locations_index > global_defs->number_of_locations )
    {
        return OTF2_CALLBACK_INTERRUPT;
    }

    location->do_read          = false;
    location->location_id      = self;
    location->number_of_events = numberOfEvents;

    return OTF2_CALLBACK_SUCCESS;
}

static OTF2_CallbackCode
global_enter_cb( OTF2_LocationRef    location,
                 OTF2_TimeStamp      time,
                 void*               userData,
                 OTF2_AttributeList* attributeList,
                 OTF2_RegionRef      region )
{
    struct otf2_reader_location* location_data = userData;
    while ( location_data->location_id != location )
    {
        location_data++;
    }

    if ( location_data->location_id != location )
    {
        fprintf( stderr, "location id missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }

    if ( location_data->number_of_events_read != time )
    {
        fprintf( stderr, "timestamp missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }
    location_data->number_of_events_read++;

    if ( region != 0 )
    {
        fprintf( stderr, "region missmatch at %" PRIu64 ": %u\n", time, region );
        return OTF2_CALLBACK_INTERRUPT;
    }

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
global_leave_cb( OTF2_LocationRef    location,
                 OTF2_TimeStamp      time,
                 void*               userData,
                 OTF2_AttributeList* attributeList,
                 OTF2_RegionRef      region )
{
    struct otf2_reader_location* location_data = userData;
    while ( location_data->location_id != location )
    {
        location_data++;
    }

    if ( location_data->location_id != location )
    {
        fprintf( stderr, "location id missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }

    if ( location_data->number_of_events_read != time )
    {
        fprintf( stderr, "timestamp missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }
    location_data->number_of_events_read++;

    if ( region != 0 )
    {
        fprintf( stderr, "region missmatch at %" PRIu64 ": %u\n", time, region );
        return OTF2_CALLBACK_INTERRUPT;
    }

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
local_enter_cb( OTF2_LocationRef    location,
                OTF2_TimeStamp      time,
                uint64_t            eventPosition,
                void*               userData,
                OTF2_AttributeList* attributeList,
                OTF2_RegionRef      region )
{
    struct otf2_reader_location* location_data = userData;

    if ( location_data->location_id != location )
    {
        fprintf( stderr, "location id missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }

    if ( location_data->number_of_events_read != time )
    {
        fprintf( stderr, "timestamp missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }
    location_data->number_of_events_read++;

    if ( location_data->number_of_events_read != eventPosition )
    {
        fprintf( stderr, "event position missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }

    if ( region != 0 )
    {
        fprintf( stderr, "region missmatch at %" PRIu64 ": %u\n", time, region );
        return OTF2_CALLBACK_INTERRUPT;
    }

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
local_leave_cb( OTF2_LocationRef    location,
                OTF2_TimeStamp      time,
                uint64_t            eventPosition,
                void*               userData,
                OTF2_AttributeList* attributeList,
                OTF2_RegionRef      region )
{
    struct otf2_reader_location* location_data = userData;

    if ( location_data->location_id != location )
    {
        fprintf( stderr, "location id missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }

    if ( location_data->number_of_events_read != time )
    {
        fprintf( stderr, "timestamp missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }
    location_data->number_of_events_read++;

    if ( location_data->number_of_events_read != eventPosition )
    {
        fprintf( stderr, "event position missmatch\n" );
        return OTF2_CALLBACK_INTERRUPT;
    }

    if ( region != 0 )
    {
        fprintf( stderr, "region missmatch at %" PRIu64 ": %u\n", time, region );
        return OTF2_CALLBACK_INTERRUPT;
    }

    return OTF2_CALLBACK_SUCCESS;
}

enum
{
    GLOBAL_HINT   = 1,
    GLOBAL_READER = 2
};

int
main( int argc, char* argv[] )
{
    CTI_Init( &argc, &argv );

    int size;
    CTI_Group_size( CTI_GROUP_WORLD, &size );
    int rank;
    CTI_Group_rank( CTI_GROUP_WORLD, &rank );

    check_condition( argc > 3, "At least three arguments." );

    char*    trace_name         = argv[ 1 ];
    int      collective_mode    = atoi( argv[ 2 ] );
    unsigned global_reader_mode = atoi( argv[ 3 ] );

    uint32_t number_of_locations_to_read = argc - 4;
    uint32_t locations_to_read[ number_of_locations_to_read ];
    for ( uint32_t i = 0; i < number_of_locations_to_read; i++ )
    {
        locations_to_read[ i ] = atoi( argv[ 4 + i ] );
    }

    OTF2_ErrorCode status;

    /* Create new archive handle. */
    OTF2_Reader* reader = OTF2_Reader_Open( trace_name );
    check_pointer( reader, "Open reader." );

    if ( 2 == collective_mode )
    {
        status = OTF2_Reader_SetSerialCollectiveCallbacks( reader );
    }
    else
    {
        status = OTF2_CTI_Reader_SetCollectiveCallbacks( reader,
                                                         CTI_GROUP_WORLD,
                                                         collective_mode == 1 );
    }
    check_status( status, "Set collective callbacks." );

    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
#if HAVE( PTHREAD_MUTEX_ERRORCHECK )
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
#endif
    status = OTF2_Pthread_Reader_SetLockingCallbacks( reader, &attr );
    check_status( status, "Set Pthread locking callbacks." );

    OTF2_Boolean global_reader_hint = OTF2_FALSE;
    if ( global_reader_mode & GLOBAL_HINT )
    {
        global_reader_hint = OTF2_TRUE;
    }
    status = OTF2_Reader_SetHint( reader,
                                  OTF2_HINT_GLOBAL_READER,
                                  &global_reader_hint );
    check_status( status, "Setting global-reader hint." );

    uint64_t number_of_locations;
    status = OTF2_Reader_GetNumberOfLocations( reader,
                                               &number_of_locations );

    struct otf2_reader_location* all_locations = calloc( number_of_locations,
                                                         sizeof( *all_locations ) );
    check_pointer( all_locations, "Allocation location storage." );

    OTF2_GlobalDefReaderCallbacks* def_callbacks;

    def_callbacks = OTF2_GlobalDefReaderCallbacks_New();
    check_pointer( def_callbacks, "Create global definition callbacks." );

    status = OTF2_GlobalDefReaderCallbacks_SetLocationCallback( def_callbacks,
                                                                location_cb );
    check_status( status, "Add Location definition callback." );

    OTF2_GlobalDefReader* global_def_reader =
        OTF2_Reader_GetGlobalDefReader( reader );
    check_pointer( global_def_reader, "Create global definition reader." );

    struct otf2_reader_defs global_defs;
    global_defs.locations           = all_locations;
    global_defs.number_of_locations = number_of_locations;
    global_defs.locations_index     = 0;

    status = OTF2_Reader_RegisterGlobalDefCallbacks( reader,
                                                     global_def_reader,
                                                     def_callbacks,
                                                     &global_defs );
    check_status( status, "Register global definition callbacks." );
    OTF2_GlobalDefReaderCallbacks_Delete( def_callbacks );

    uint64_t definitions_read;
    status = OTF2_Reader_ReadGlobalDefinitions( reader, global_def_reader,
                                                OTF2_UNDEFINED_UINT64,
                                                &definitions_read );
    check_status( status, "Read global definitions. Number of definitions: %" PRIu64,
                  definitions_read );

    check_condition( global_defs.locations_index == global_defs.number_of_locations,
                     "Announced number of locations match." );

    OTF2_Reader_CloseGlobalDefReader( reader,
                                      global_def_reader );

    /* Select locations to read */
    uint32_t worker_number_of_locations_to_read = 0;
    for ( uint64_t i = 0; i < number_of_locations; i++ )
    {
        struct otf2_reader_location* location = &all_locations[ i ];

        if ( location->location_id % size != rank )
        {
            continue;
        }

        for ( uint32_t j = 0; j < number_of_locations_to_read; j++ )
        {
            if ( locations_to_read[ j ] == location->location_id )
            {
                location->do_read = true;
                break;
            }
        }

        if ( location->do_read )
        {
            status = OTF2_Reader_SelectLocation( reader,
                                                 location->location_id );
            check_status( status,
                          "Select location %" PRIu64 " for reading.",
                          location->location_id );

            worker_number_of_locations_to_read++;
        }
    }

    status = OTF2_Reader_OpenDefFiles( reader );
    bool local_def_files_exists = OTF2_SUCCESS == status;
    if ( OTF2_ERROR_ENOENT == status )
    {
        /* Ignore ENOENT in the next check */
        status = OTF2_SUCCESS;
    }
    check_status( status, "Open local def files for reading." );

    /* Read local definitions for all locations. */
    if ( local_def_files_exists )
    {
        for ( uint64_t i = 0; i < number_of_locations; i++ )
        {
            struct otf2_reader_location* location = &all_locations[ i ];

            if ( !location->do_read )
            {
                continue;
            }

            OTF2_DefReader* def_reader =
                OTF2_Reader_GetDefReader( reader,
                                          location->location_id );
            /* a local def file is not mandatory */
            if ( !def_reader )
            {
                continue;
            }

            definitions_read = 0;
            status           = OTF2_SUCCESS;
            do
            {
                uint64_t def_reads = 0;
                status = OTF2_Reader_ReadAllLocalDefinitions( reader,
                                                              def_reader,
                                                              &def_reads );
                definitions_read += def_reads;

                /* continue reading, if we have an duplicate mapping table */
                if ( OTF2_ERROR_DUPLICATE_MAPPING_TABLE != status )
                {
                    break;
                }
            }
            while ( true );
            check_status( status,
                          "Read %" PRIu64 " definitions for location %" PRIu64,
                          definitions_read,
                          location->location_id );

            /* Close def reader, it is no longer useful and occupies memory */
            status = OTF2_Reader_CloseDefReader( reader, def_reader );
            check_status( status, "Close local definition reader." );
        }
        CTI_Barrier( CTI_GROUP_WORLD );
        status = OTF2_Reader_CloseDefFiles( reader );
        check_status( status, "Close local def files for reading." );
    }

    /* Open and read event files. */
    status = OTF2_Reader_OpenEvtFiles( reader );
    check_status( status, "Open event files for reading." );

    OTF2_EvtReaderCallbacks* evt_callbacks;
    if ( ~global_reader_mode & GLOBAL_READER )
    {
        evt_callbacks = OTF2_EvtReaderCallbacks_New();
        check_pointer( evt_callbacks, "Create event callbacks." );

        OTF2_EvtReaderCallbacks_SetEnterCallback( evt_callbacks,
                                                  local_enter_cb );

        OTF2_EvtReaderCallbacks_SetLeaveCallback( evt_callbacks,
                                                  local_leave_cb );
    }

    for ( uint64_t i = 0; i < number_of_locations; i++ )
    {
        struct otf2_reader_location* location = &all_locations[ i ];

        if ( !location->do_read )
        {
            continue;
        }

        location->evt_reader = OTF2_Reader_GetEvtReader( reader,
                                                         location->location_id );
        check_pointer( location->evt_reader, "Get event reader." );

        if ( ~global_reader_mode & GLOBAL_READER )
        {
            status = OTF2_Reader_RegisterEvtCallbacks( reader,
                                                       location->evt_reader,
                                                       evt_callbacks,
                                                       location );
            check_status( status, "Register event callbacks." );

            uint64_t events_read;
            status = OTF2_Reader_ReadAllLocalEvents( reader,
                                                     location->evt_reader,
                                                     &events_read );
            check_status( status, "Reading all events." );

            status = OTF2_Reader_CloseEvtReader( reader,
                                                 location->evt_reader );
            check_status( status, "Closing event reader." );
        }
    }

    if ( global_reader_mode & GLOBAL_READER && worker_number_of_locations_to_read > 0 )
    {
        OTF2_GlobalEvtReaderCallbacks* global_evt_callbacks = OTF2_GlobalEvtReaderCallbacks_New();
        check_pointer( global_evt_callbacks, "Create global event callbacks." );

        OTF2_GlobalEvtReaderCallbacks_SetEnterCallback( global_evt_callbacks,
                                                        global_enter_cb );

        OTF2_GlobalEvtReaderCallbacks_SetLeaveCallback( global_evt_callbacks,
                                                        global_leave_cb );

        OTF2_GlobalEvtReader* global_evt_reader =
            OTF2_Reader_GetGlobalEvtReader( reader );
        check_pointer( global_evt_reader, "Get global event reader." );

        status = OTF2_Reader_RegisterGlobalEvtCallbacks( reader,
                                                         global_evt_reader,
                                                         global_evt_callbacks,
                                                         all_locations );
        check_status( status, "Register global event callbacks." );

        uint64_t events_read;
        status = OTF2_Reader_ReadAllGlobalEvents( reader,
                                                  global_evt_reader,
                                                  &events_read );
        check_status( status, "Reading all global events." );

        status = OTF2_Reader_CloseGlobalEvtReader( reader,
                                                   global_evt_reader );
        check_status( status, "Closing global event reader." );

        OTF2_GlobalEvtReaderCallbacks_Delete( global_evt_callbacks );
    }
    else if ( ~global_reader_mode & GLOBAL_READER )
    {
        OTF2_EvtReaderCallbacks_Delete( evt_callbacks );
    }

    status = OTF2_Reader_CloseEvtFiles( reader );
    check_status( status, "Close event files for reading." );

    status = OTF2_Reader_Close( reader );
    check_status( status, "Close reader." );

    free( all_locations );

    CTI_Finalize();

    return 0;
}
