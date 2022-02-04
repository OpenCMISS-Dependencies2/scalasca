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
 *              Afterwards it reads this archive back in and check if the timer
 *              synchronization works.
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

/* ___ Prototypes for static functions. _____________________________________ */

static OTF2_ErrorCode
create_archive( bool use_sion );

static OTF2_ErrorCode
check_archive( void );



/* ___ Prototypes for all callbacks. ________________________________________ */

static OTF2_CallbackCode
enter_cb( uint64_t            locationID,
          uint64_t            time,
          void*               userData,
          OTF2_AttributeList* attributes,
          uint32_t            regionID );

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
main( int    argc,
      char** argv )
{
    OTF2_ErrorCode status;

    status = create_archive( argc > 1 );
    check_status( status, "Create Archive." );

    status = check_archive();
    check_status( status, "Check Archive." );

    return EXIT_SUCCESS;
}



/* ___ Test data ____________________________________________________________ */

typedef struct otf2_event_timestamp_struct
{
    uint64_t local_timestamp;
    uint64_t expected_global_timestamp;
} otf2_event_timestamp;
static const otf2_event_timestamp event_timestamps[] = {
    { .local_timestamp =  0, .expected_global_timestamp =  0 },
    { .local_timestamp =  1, .expected_global_timestamp =  2 },
    { .local_timestamp =  2, .expected_global_timestamp =  4 },
    { .local_timestamp =  3, .expected_global_timestamp =  6 },
    { .local_timestamp =  4, .expected_global_timestamp =  8 },
    { .local_timestamp =  5, .expected_global_timestamp = 10 },
    { .local_timestamp =  6, .expected_global_timestamp = 11 },
    { .local_timestamp =  7, .expected_global_timestamp = 12 },
    { .local_timestamp =  8, .expected_global_timestamp = 13 },
    { .local_timestamp =  9, .expected_global_timestamp = 14 },
    { .local_timestamp = 10, .expected_global_timestamp = 15 },
    { .local_timestamp = 11, .expected_global_timestamp = 16 },
};
#define number_of_events \
    ( sizeof( event_timestamps ) / sizeof( event_timestamps[ 0 ] ) )

typedef struct otf2_clock_offset_struct
{
    uint64_t time;
    int64_t  offset;
} otf2_clock_offset;
static const otf2_clock_offset clock_offsets[] = {
    { .time =  0, .offset = 0 },
    { .time =  5, .offset = 5 },
    { .time = 10, .offset = 5 },
};
#define number_of_clock_offsets \
    ( sizeof( clock_offsets ) / sizeof( clock_offsets[ 0 ] ) )

/** @internal
 *  @brief Create an OTF2 archive.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
create_archive( bool use_sion )
{
    OTF2_ErrorCode status;

    /* Create new archive handle. */
    OTF2_Archive* archive = OTF2_Archive_Open( "OTF2_Timer_sync_trace",
                                               "TestTrace",
                                               OTF2_FILEMODE_WRITE,
                                               1024 * 1024,
                                               4 * 1024 * 1024,
                                               use_sion
                                               ? OTF2_SUBSTRATE_SION
                                               : OTF2_SUBSTRATE_POSIX,
                                               OTF2_COMPRESSION_NONE );
    check_pointer( archive, "Create archive" );

    status = OTF2_Archive_SetFlushCallbacks( archive, &otf2_test_flush_callbacks, NULL );
    check_status( status, "Set flush callbacks." );
    /* Set description, and creator. */
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
    status = OTF2_Archive_SetDescription( archive, "OTF2 trace for timer synchronization test." );
    check_status( status, "Set description." );
    status = OTF2_Archive_SetCreator( archive, "OTF2_Timer_sync" );
    check_status( status, "Set creator." );

    /* Write location definitions. */
    OTF2_GlobalDefWriter* global_def_writer =
        OTF2_Archive_GetGlobalDefWriter( archive );
    check_pointer( global_def_writer, "Get global definition writer" );

    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 0, "" );
    check_status( status, "Write string definition." );

    status = OTF2_GlobalDefWriter_WriteSystemTreeNode(
        global_def_writer, 0, 0, 0, OTF2_UNDEFINED_SYSTEM_TREE_NODE );
    check_status( status, "Write system tree node." );

    status = OTF2_GlobalDefWriter_WriteLocationGroup(
        global_def_writer, 0, 0, OTF2_LOCATION_GROUP_TYPE_PROCESS, 0 );
    check_status( status, "Write location group." );

    status = OTF2_GlobalDefWriter_WriteLocation(
        global_def_writer, 0, 0,
        OTF2_LOCATION_TYPE_CPU_THREAD,
        number_of_events,
        0 );
    check_status( status, "Write location." );

    /* Write definitions. */
    status = OTF2_GlobalDefWriter_WriteRegion(
        global_def_writer, 0, 0, 0, 0,
        OTF2_REGION_ROLE_UNKNOWN, OTF2_PARADIGM_UNKNOWN, OTF2_REGION_FLAG_NONE,
        OTF2_UNDEFINED_STRING, 0, 0 );
    check_status( status, "Write region" );

    status = OTF2_Archive_CloseGlobalDefWriter( archive, global_def_writer );
    check_status( status, "Closing global definition writer." );

    status = OTF2_Archive_OpenEvtFiles( archive );
    check_status( status, "Open event files." );

    OTF2_EvtWriter* evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
    check_pointer( evt_writer, "Get event writer." );

    for ( uint32_t j = 0; j < number_of_events; j++ )
    {
        status = OTF2_EvtWriter_Enter(
            evt_writer, NULL,
            event_timestamps[ j ].local_timestamp,
            0 );
        check_status( status, "Write event." );
    }

    status = OTF2_Archive_CloseEvtWriter( archive, evt_writer );
    check_status( status, "Close event writer." );

    status = OTF2_Archive_CloseEvtFiles( archive );
    check_status( status, "Close event files." );

    status = OTF2_Archive_OpenDefFiles( archive );
    check_status( status, "Open local definitions files." );

    OTF2_DefWriter* def_writer = OTF2_Archive_GetDefWriter( archive, 0 );
    check_pointer( def_writer, "Get definition writer." );

    for ( uint32_t i = 0; i < number_of_clock_offsets; i++ )
    {
        status = OTF2_DefWriter_WriteClockOffset(
            def_writer,
            clock_offsets[ i ].time,
            clock_offsets[ i ].offset,
            0.0 );
        check_status( status, "Write timer sync." );
    }

    status = OTF2_Archive_CloseDefWriter( archive,
                                          def_writer );
    check_status( status, "Close local definitions writer." );

    status = OTF2_Archive_CloseDefFiles( archive );
    check_status( status, "Close local definitions files." );

    /* Finalize. */
    status = OTF2_Archive_Close( archive );
    check_status( status, "Close archive." );

    return OTF2_SUCCESS;
}


/** @internal
 *  @brief Read an OTF2 archive an compare values.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
check_archive( void )
{
    OTF2_ErrorCode status;

    /* Get a reader handle. */
    OTF2_Reader* reader = OTF2_Reader_Open( "OTF2_Timer_sync_trace/TestTrace.otf2" );
    check_pointer( reader, "Create new reader handle." );
    status = OTF2_Reader_SetSerialCollectiveCallbacks( reader );
    check_status( status, "Set serial mode." );
#if HAVE( PTHREAD )
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
#if HAVE( PTHREAD_MUTEX_ERRORCHECK )
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
#endif
    status = OTF2_Pthread_Reader_SetLockingCallbacks( reader, &attr );
    check_status( status, "Set Pthread locking callbacks." );
#endif

    /* Get global definition reader. */
    OTF2_GlobalDefReader* global_def_reader = OTF2_Reader_GetGlobalDefReader( reader );
    check_pointer( global_def_reader, "Create global definition reader handle." );

    /* Define and register definition callbacks. */
    OTF2_GlobalDefReaderCallbacks* check_defs = OTF2_GlobalDefReaderCallbacks_New();
    check_pointer( check_defs, "Create global definition callback handle." );

    status = OTF2_Reader_RegisterGlobalDefCallbacks( reader, global_def_reader,
                                                     check_defs,
                                                     reader );
    check_status( status, "Register global definition callbacks." );

    OTF2_GlobalDefReaderCallbacks_Delete( check_defs );

    /* Read definitions. */
    uint64_t definitions_read = 0;
    status = OTF2_Reader_ReadGlobalDefinitions( reader, global_def_reader,
                                                OTF2_UNDEFINED_UINT64,
                                                &definitions_read );
    check_status( status, "Read global definitions." );

    OTF2_Reader_CloseGlobalDefReader( reader,
                                      global_def_reader );

    status = OTF2_Reader_SelectLocation( reader, 0 );
    check_status( status, "Select location to read." );

    /* Also open a definition reader and read all local definitions. */
    status = OTF2_Reader_OpenDefFiles( reader );
    check_status( status, "Open local def files for reading." );

    OTF2_DefReader* def_reader = OTF2_Reader_GetDefReader( reader, 0 );
    check_pointer( def_reader, "Create local definition reader." );

    status = OTF2_Reader_ReadAllLocalDefinitions( reader, def_reader,
                                                  &definitions_read );
    check_status( status, "Read local definitions." );

    /* Close def reader, it is no longer useful and occupies memory */
    status = OTF2_Reader_CloseDefReader( reader, def_reader );
    check_status( status, "Close local definition reader." );

    status = OTF2_Reader_CloseDefFiles( reader );
    check_status( status, "Close local def files for reading." );

    /* Open and read event files. */
    status = OTF2_Reader_OpenEvtFiles( reader );
    check_status( status, "Open event files for reading." );

    /* Open the local event reader. */
    OTF2_EvtReader* evt_reader = OTF2_Reader_GetEvtReader( reader, 0 );
    check_pointer( evt_reader, "Create local event reader." );

    /* Get global event reader. */
    OTF2_GlobalEvtReader* global_evt_reader = OTF2_Reader_GetGlobalEvtReader( reader );
    check_pointer( global_evt_reader, "Create global event reader." );

    /* Define and register event callbacks. */
    OTF2_GlobalEvtReaderCallbacks* check_evts = OTF2_GlobalEvtReaderCallbacks_New();
    check_pointer( check_evts, "Create global event callbacks." );

    OTF2_GlobalEvtReaderCallbacks_SetEnterCallback( check_evts, enter_cb );

    uint32_t timestamp_position = 0;
    status = OTF2_Reader_RegisterGlobalEvtCallbacks( reader, global_evt_reader,
                                                     check_evts,
                                                     &timestamp_position );
    check_status( status, "Register global event callbacks." );

    OTF2_GlobalEvtReaderCallbacks_Delete( check_evts );

    /* Read until an error occures. */
    uint64_t events_read = 0;
    status = OTF2_Reader_ReadGlobalEvents( reader, global_evt_reader, OTF2_UNDEFINED_UINT64, &events_read );
    check_status( status, "Read %" PRIu64 " events.", events_read );

    status = OTF2_Reader_CloseGlobalEvtReader( reader, global_evt_reader );
    check_status( status, "Close global event reader." );

    status = OTF2_Reader_CloseEvtFiles( reader );
    check_status( status, "Close event files for reading." );

    /* Close reader. */
    status = OTF2_Reader_Close( reader );
    check_status( status, "Close reader." );

    return OTF2_SUCCESS;
}



/* ___ Implementation of callbacks __________________________________________ */



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
    uint32_t timestamp_position = *( uint32_t* )userData;

    check_condition( timestamp_position < number_of_events,
                     "More events read than writen." );

    check_condition( event_timestamps[ timestamp_position ].expected_global_timestamp == time,
                     "Timer synchronization mismatch for event %u: expected %" PRIu64 ", got %" PRIu64,
                     timestamp_position,
                     event_timestamps[ timestamp_position ].expected_global_timestamp,
                     time );

    /* increment position for next event */
    *( uint32_t* )userData = timestamp_position + 1;

    return OTF2_CALLBACK_SUCCESS;
}


/** @} */
