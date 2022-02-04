/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/** @internal
 *
 *  @file
 *
 *  @brief      This test generates an OTF2 archive with a defined data set.
 *              Afterwards it reads this archive back in and compares the data
 *              sets.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdarg.h>

#include <otf2/otf2.h>

#if HAVE( PTHREAD )
#include <otf2/OTF2_Pthread_Locks.h>
#endif

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <otf2-test/check.h>

#include <otf2-test/gettime.h>

#define OTF2_TEST_FLUSH_DEFAULT OTF2_FLUSH
#define OTF2_TEST_FLUSH_GETTIME otf2_test_gettime
#include <otf2-test/flush.h>


/* ___ Global defines _______________________________________________________ */


/** @brief Number of locations. Should be small. */
#define NUM_LOCATIONS 4

/** @brief Number of events per location. */
#define NUMBER_OF_EVENTS 20000

/** @brief Archive path */
#define OTF2_ARCHIVE_PATH "OTF2_Integrity_trace"

/** @brief Trace name */
#define OTF2_TRACE_NAME "TestTrace"


/* ___ Global variables. ____________________________________________________ */


/** @internal
 *  @brief Global time counter. */
static uint64_t otf2_Time;


/** @internal
 *  @brief Global region counter. */
static uint64_t otf2_Region;


/* ___ Prototypes for static functions. _____________________________________ */


static OTF2_ErrorCode
create_archive( bool use_sion );

static OTF2_ErrorCode
check_archive( const char* trace );


/* ___ Prototypes for all callbacks. ________________________________________ */


static OTF2_CallbackCode
global_def_location_cb( void*             userData,
                        uint64_t          locationIdentifier,
                        uint32_t          stringID,
                        OTF2_LocationType locationType,
                        uint64_t          numberOfEvents,
                        uint32_t          locationGroup );


static OTF2_CallbackCode
enter_cb( uint64_t            locationID,
          uint64_t            time,
          void*               userData,
          OTF2_AttributeList* attributes,
          uint32_t            regionID );


static OTF2_CallbackCode
leave_cb( uint64_t            locationID,
          uint64_t            time,
          void*               userData,
          OTF2_AttributeList* attributes,
          uint32_t            regionID );


static OTF2_CallbackCode
mpi_send_cb( uint64_t            locationID,
             uint64_t            time,
             void*               userData,
             OTF2_AttributeList* attributes,
             uint32_t            receiver,
             uint32_t            communicator,
             uint32_t            msgTag,
             uint64_t            msgLength );


static OTF2_CallbackCode
mpi_recv_cb( uint64_t            locationID,
             uint64_t            time,
             void*               userData,
             OTF2_AttributeList* attributes,
             uint32_t            sender,
             uint32_t            communicator,
             uint32_t            msgTag,
             uint64_t            msgLength );


/* ___ main _________________________________________________________________ */


/** Main function
 *
 *  This test generates an OTF2 archive with a defined data set. Afterwards it
 *  reads this archive back in and compares the data sets.
 *
 *  @return                 Returns EXIT_SUCCESS if successful, EXIT_FAILURE
 *                          if an error occures.
 */
int
main( int   argc,
      char* argv[] )
{
    OTF2_ErrorCode status = OTF2_ERROR_INVALID;

    otf2_DEBUG = !!getenv( "OTF2_DEBUG_TESTS" );

    if ( argc == 1 )
    {
        status = create_archive( !!getenv( "OTF2_TEST_USE_SION" ) );
        check_status( status, "Create Archive." );

        status = check_archive( OTF2_ARCHIVE_PATH "/" OTF2_TRACE_NAME ".otf2" );
        check_status( status, "Check Archive." );
    }
    else
    {
        status = check_archive( argv[ 1 ] );
        check_status( status, "Check Archive." );
    }


    return EXIT_SUCCESS;
}


/** @internal
 *  @brief Create an OTF2 archive.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
create_archive( bool use_sion )
{
    /* Create new archive handle. */
    OTF2_Archive* archive = OTF2_Archive_Open( OTF2_ARCHIVE_PATH,
                                               OTF2_TRACE_NAME,
                                               OTF2_FILEMODE_WRITE,
                                               256 * 1024,
                                               OTF2_UNDEFINED_UINT64,
                                               use_sion
                                               ? OTF2_SUBSTRATE_SION
                                               : OTF2_SUBSTRATE_POSIX,
                                               OTF2_COMPRESSION_NONE );
    check_pointer( archive, "Create archive" );


    /* Set description, and creator. */
    OTF2_ErrorCode status = OTF2_ERROR_INVALID;
    status = OTF2_Archive_SetFlushCallbacks( archive, &otf2_test_flush_callbacks, NULL );
    check_status( status, "Set flush callbacks." );
    status = OTF2_Archive_SetSerialCollectiveCallbacks( archive );
    check_status( status, "Set serial mode." );
#if HAVE( PTHREAD )
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
#if HAVE( PTHREAD_MUTEX_ERRORCHECK )
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
#endif
    status = OTF2_Pthread_Archive_SetLockingCallbacks( archive, &attr );
    check_status( status, "Set Pthread locking callbacks." );
#endif
    status = OTF2_Archive_SetDescription( archive, "OTF2 trace for integrity test." );
    check_status( status, "Set description." );
    status = OTF2_Archive_SetCreator( archive, "OTF2_Integrity" );
    check_status( status, "Set creator." );


    /* Generate location IDs. Just to have non-consecutive location IDs. */
    uint64_t locations[ NUM_LOCATIONS ];
    for ( uint64_t i = 0; i < NUM_LOCATIONS; ++i )
    {
        locations[ i ] = i * i;
    }


    status = OTF2_Archive_SetDefChunkSize( archive,
                                           4 * 1024 * 1024 );
    check_status( status, "Setting definition chunk size." );

    /* Write location definitions. */
    OTF2_GlobalDefWriter* global_def_writer = NULL;
    global_def_writer = OTF2_Archive_GetGlobalDefWriter( archive );
    check_pointer( global_def_writer, "Get global definition writer" );

    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 0, "Process" );
    check_status( status, "Write string definition." );

    for ( uint64_t i = 0; i < NUM_LOCATIONS; ++i )
    {
        status = OTF2_GlobalDefWriter_WriteLocation( global_def_writer,
                                                     locations[ i ],
                                                     0,
                                                     OTF2_LOCATION_TYPE_CPU_THREAD,
                                                     NUMBER_OF_EVENTS * 6,
                                                     0 );
        check_status( status, "Write location definition." );
    }

    /* Write definitions. */
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 1, "MPI_Send" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 2, "Send an MPI message" );
    check_status( status, "Write string definition" );

    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 3, "MPI_Recv" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 4, "Receive an MPI message" );
    check_status( status, "Write string definition" );

    status = OTF2_GlobalDefWriter_WriteRegion( global_def_writer, 0, 1, 1, 2, OTF2_REGION_ROLE_UNKNOWN, OTF2_PARADIGM_UNKNOWN, OTF2_REGION_FLAG_NONE, 0, 0, 0 );
    check_status( status, "Write region definition" );

    status = OTF2_GlobalDefWriter_WriteRegion( global_def_writer, 1, 3, 3, 4, OTF2_REGION_ROLE_UNKNOWN, OTF2_PARADIGM_UNKNOWN, OTF2_REGION_FLAG_NONE, 0, 0, 0 );
    check_status( status, "Write region definition" );

    status = OTF2_Archive_CloseGlobalDefWriter( archive, global_def_writer );
    check_status( status, "Closing global definition writer." );

    /* Define local event and definition writer. */
    OTF2_EvtWriter* evt_writer = NULL;
    OTF2_DefWriter* def_writer = NULL;


    status = OTF2_Archive_OpenDefFiles( archive );
    check_status( status, "Open local definitions files." );
    for ( uint64_t i = 0; i < NUM_LOCATIONS; ++i )
    {
        /* Just open a definition writer, so the file is created. */
        def_writer = OTF2_Archive_GetDefWriter( archive, locations[ i ] );
        check_pointer( def_writer, "Get definition writer." );

        status = OTF2_Archive_CloseDefWriter( archive, def_writer );
        check_status( status, "Close definition writer." );
    }
    status = OTF2_Archive_CloseDefFiles( archive );
    check_status( status, "Close local definitions files." );

    status = OTF2_Archive_OpenEvtFiles( archive );
    check_status( status, "Open event files." );
    for ( uint64_t j = 0; j < NUMBER_OF_EVENTS; j++ )
    {
        for ( uint64_t i = 0; i < NUM_LOCATIONS; ++i )
        {
            evt_writer = OTF2_Archive_GetEvtWriter( archive, locations[ i ] );
            check_pointer( evt_writer, "Get event writer." );

            /* Write enter, mpi_send, leave and region definition. */
            uint64_t ts = otf2_test_gettime();
            status = OTF2_EvtWriter_Enter( evt_writer, NULL, ts, 0 );
            check_status( status, "Write Enter." );
            status = OTF2_EvtWriter_MpiSend( evt_writer, NULL, ts, locations[ i ], 0, 42, 1024 );
            check_status( status, "Write MPI_Send." );
            status = OTF2_EvtWriter_Leave( evt_writer, NULL, otf2_test_gettime(), 0 );
            check_status( status, "Write Leave." );

            /* Write enter, mpi_recv, leave and region definition. */
            status = OTF2_EvtWriter_Enter( evt_writer, NULL, otf2_test_gettime(), 1 );
            check_status( status, "Write Enter." );
            ts     = otf2_test_gettime();
            status = OTF2_EvtWriter_MpiRecv( evt_writer, NULL, ts, locations[ i ], 0, 42, 1024 );
            check_status( status, "Write MPI_Send." );
            status = OTF2_EvtWriter_Leave( evt_writer, NULL, ts, 1 );
            check_status( status, "Write Leave." );
        }
    }
    for ( uint64_t i = 0; i < NUM_LOCATIONS; ++i )
    {
        evt_writer = OTF2_Archive_GetEvtWriter( archive, locations[ i ] );
        check_pointer( evt_writer, "Get event writer." );

        status = OTF2_Archive_CloseEvtWriter( archive, evt_writer );
        check_status( status, "Close event writer." );
    }
    status = OTF2_Archive_CloseEvtFiles( archive );
    check_status( status, "Close event files." );


    /* Finalize. */
    OTF2_Archive_Close( archive );


    return OTF2_SUCCESS;
}


/** @internal
 *  @brief Read an OTF2 archive an compare values.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
check_archive( const char* trace )
{
    OTF2_ErrorCode status;

    /* Get a reader handle. */
    OTF2_Reader* reader = OTF2_Reader_Open( trace );
    check_pointer( reader, "Create new reader handle." );
    status = OTF2_Reader_SetSerialCollectiveCallbacks( reader );
    check_status( status, "Set serial mode." );

    /* Get number of locations from the anchor file. */
    uint64_t num_locations = 0;
    status = OTF2_Reader_GetNumberOfLocations( reader, &num_locations );
    check_status( status, "Get number of locations. Number of locations: %" PRIu64,
                  num_locations );
    OTF2_LocationRef locations_to_read[ num_locations + 1 ];
    locations_to_read[ 0 ] = 1;

    /* Get global definition reader. */
    OTF2_GlobalDefReader* global_def_reader = OTF2_Reader_GetGlobalDefReader( reader );
    check_pointer( global_def_reader, "Create global definition reader handle." );

    /* Define and register definition callbacks. */
    OTF2_GlobalDefReaderCallbacks* check_defs = OTF2_GlobalDefReaderCallbacks_New();
    check_pointer( check_defs, "Create global definition callback handle." );
    OTF2_GlobalDefReaderCallbacks_SetLocationCallback( check_defs, global_def_location_cb );

    status = OTF2_Reader_RegisterGlobalDefCallbacks( reader, global_def_reader,
                                                     check_defs,
                                                     locations_to_read );
    check_status( status, "Register global definition callbacks." );

    OTF2_GlobalDefReaderCallbacks_Delete( check_defs );

    /* Read definitions. */
    uint64_t definitions_read = 0;
    status = OTF2_Reader_ReadGlobalDefinitions( reader, global_def_reader,
                                                OTF2_UNDEFINED_UINT64,
                                                &definitions_read );
    check_status( status, "Read global definitions. Number of definitions: %" PRIu64,
                  definitions_read );

    OTF2_Reader_CloseGlobalDefReader( reader,
                                      global_def_reader );

    /* Check if number of global definitions read matches the number of global
     * definitions defined in the anchor file. */
    uint64_t defs_anchor = 0;
    status = OTF2_Reader_GetNumberOfGlobalDefinitions( reader, &defs_anchor );
    check_status( status, "Get number of global definitions: %" PRIu64,
                  defs_anchor );

    check_condition( defs_anchor == definitions_read,
                     "Number of global definitions read and defined in anchor file do not match!" );

    for ( uint64_t location_index = 1; location_index <= num_locations; location_index++ )
    {
        status = OTF2_Reader_SelectLocation( reader, locations_to_read[ location_index ] );
        check_status( status, "Select location to read." );
    }

    /* Also open a definition reader and read all local definitions. */
    status = OTF2_Reader_OpenDefFiles( reader );
    check_status( status, "Open local def files for reading." );
    status = OTF2_Reader_OpenEvtFiles( reader );
    check_status( status, "Open event files for reading." );
    for ( uint64_t location_index = 1; location_index <= num_locations; location_index++ )
    {
        OTF2_DefReader* def_reader = OTF2_Reader_GetDefReader( reader,
                                                               locations_to_read[ location_index ] );
        check_pointer( def_reader, "Create local definition reader for #%" PRIu64, locations_to_read[ location_index ] );

        status = OTF2_Reader_ReadAllLocalDefinitions( reader, def_reader,
                                                      &definitions_read );
        check_status( status, "Read local definitions." );

        /* Close def reader, it is no longer useful and occupies memory */
        status = OTF2_Reader_CloseDefReader( reader, def_reader );
        check_status( status, "Close local definition reader." );

        OTF2_EvtReader* evt_reader = OTF2_Reader_GetEvtReader( reader,
                                                               locations_to_read[ location_index ] );
        check_pointer( evt_reader, "Create local event reader." );
    }
    status = OTF2_Reader_CloseDefFiles( reader );
    check_status( status, "Close local def files for reading." );

    /* Get global event reader. */
    OTF2_GlobalEvtReader* global_evt_reader = OTF2_Reader_GetGlobalEvtReader( reader );
    check_pointer( global_evt_reader, "Create global event reader." );

    /* Define and register event callbacks. */
    OTF2_GlobalEvtReaderCallbacks* check_evts = OTF2_GlobalEvtReaderCallbacks_New();
    check_pointer( check_evts, "Create global event callbacks." );

    OTF2_GlobalEvtReaderCallbacks_SetEnterCallback( check_evts, enter_cb );
    OTF2_GlobalEvtReaderCallbacks_SetLeaveCallback( check_evts, leave_cb );
    OTF2_GlobalEvtReaderCallbacks_SetMpiSendCallback( check_evts, mpi_send_cb );
    OTF2_GlobalEvtReaderCallbacks_SetMpiRecvCallback( check_evts, mpi_recv_cb );

    status = OTF2_Reader_RegisterGlobalEvtCallbacks( reader, global_evt_reader,
                                                     check_evts,
                                                     NULL );
    check_status( status, "Register global event callbacks." );

    OTF2_GlobalEvtReaderCallbacks_Delete( check_evts );

    /* Read until an error occures. */
    uint64_t events_read = 0;
    status = OTF2_Reader_ReadGlobalEvents( reader, global_evt_reader, OTF2_UNDEFINED_UINT64, &events_read );
    check_status( status, "Read %" PRIu64 " events.", events_read );

    if ( events_read != ( 6 * NUM_LOCATIONS * NUMBER_OF_EVENTS ) )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT,
                      "Number of events read does not match!" );
    }

    status = OTF2_Reader_CloseGlobalEvtReader( reader, global_evt_reader );
    check_status( status, "Close global event reader." );

    status = OTF2_Reader_CloseEvtFiles( reader );
    check_status( status, "Close event files for reading." );

    /* Close reader */
    status = OTF2_Reader_Close( reader );
    check_status( status, "Close reader." );

    return OTF2_SUCCESS;
}



/* ___ Implementation of callbacks __________________________________________ */



/** @internal
 *  @brief Callbacks for location definition.
 *
 *  @param userData             Optional user data.
 *  @param locationIdentifier   Location ID.
 *  @param stringID             String ID for the Location description.
 *  @param locationType         Type of the location.
 *  @param numberOfEvents       Number of events for this location.
 *
 *  @return                     Returns OTF2_SUCCESS if successful, an error
 *                              code if an error occures.
 */
OTF2_CallbackCode
global_def_location_cb( void*             userData,
                        uint64_t          locationIdentifier,
                        uint32_t          stringID,
                        OTF2_LocationType locationType,
                        uint64_t          numberOfEvents,
                        uint32_t          locationGroup )
{
    /* Check values. */
    check_condition( numberOfEvents == ( NUMBER_OF_EVENTS * 6 ),
                     "Check Location Definition." );

    OTF2_LocationRef* locations_to_read = userData;
    locations_to_read[ locations_to_read[ 0 ]++ ] = locationIdentifier;

    return OTF2_CALLBACK_SUCCESS;
}


/** @internal
 *  @name Callbacks for events.
 *
 *  @param locationID       Location ID.
 *  @param time             Timestamp of the event.
 *  @param userData         Optional user data.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occures.
 *  @{
 */
OTF2_CallbackCode
enter_cb( uint64_t            locationID,
          uint64_t            time,
          void*               userData,
          OTF2_AttributeList* attributes,
          uint32_t            regionID )
{
    if ( time < otf2_Time )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Timestamp failure." );
    }
    otf2_Time = time;

    if ( regionID != otf2_Region )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Region ID failure." );
    }

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
leave_cb( uint64_t            locationID,
          uint64_t            time,
          void*               userData,
          OTF2_AttributeList* attributes,
          uint32_t            regionID )
{
    if ( time < otf2_Time )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Timestamp failure." );
    }
    otf2_Time = time;

    if ( regionID != otf2_Region )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Region ID failure." );
    }
    otf2_Region = ( otf2_Region + 1 ) % 2;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
mpi_send_cb( uint64_t            locationID,
             uint64_t            time,
             void*               userData,
             OTF2_AttributeList* attributes,
             uint32_t            receiver,
             uint32_t            communicator,
             uint32_t            msgTag,
             uint64_t            msgLength )
{
    if ( time < otf2_Time )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Timestamp failure." );
    }
    otf2_Time = time;

    if ( locationID != receiver )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Message receiver failure." );
    }

    if ( communicator != 0 )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Communicator failure." );
    }

    if ( msgTag != 42 )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Message tag failure." );
    }

    if ( msgLength != 1024 )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Message length failure." );
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
mpi_recv_cb( uint64_t            locationID,
             uint64_t            time,
             void*               userData,
             OTF2_AttributeList* attributes,
             uint32_t            sender,
             uint32_t            communicator,
             uint32_t            msgTag,
             uint64_t            msgLength )
{
    if ( time < otf2_Time )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Timestamp failure." );
    }
    otf2_Time = time;

    if ( locationID != sender )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Message sender failure." );
    }

    if ( communicator != 0 )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Communicator failure." );
    }

    if ( msgTag != 42 )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Message tag failure." );
    }

    if ( msgLength != 1024 )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Message length failure." );
    }

    return OTF2_CALLBACK_SUCCESS;
}

/** @} */
