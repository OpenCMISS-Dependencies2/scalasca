/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
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

#define OTF2_TEST_FLUSH_DEFAULT OTF2_FLUSH
#include <otf2-test/flush.h>

/* ___ Prototypes for static functions. _____________________________________ */


static OTF2_ErrorCode
create_archive( bool use_sion );

static OTF2_ErrorCode
check_archive( void );



/* ___ Prototypes for all callbacks. ________________________________________ */

static OTF2_CallbackCode
snapshot_start_cb( uint64_t            locationID,
                   uint64_t            time,
                   void*               userData,
                   OTF2_AttributeList* attributeList,
                   uint64_t            numberOfEventsInSnapshot );

static OTF2_CallbackCode
snapshot_end_cb( uint64_t            locationID,
                 uint64_t            time,
                 void*               userData,
                 OTF2_AttributeList* attributeList,
                 uint64_t            contReadPos );

static OTF2_CallbackCode
enter_snap_cb( OTF2_LocationRef    location,
               OTF2_TimeStamp      snapTime,
               void*               userData,
               OTF2_AttributeList* attributeList,
               OTF2_TimeStamp      origEventTime,
               OTF2_RegionRef      region );

/* ___ main _________________________________________________________________ */

static uint64_t number_of_snapshots            = 1000;
static uint64_t snapshot_tick_rate             = 10;
static uint64_t min_snapshot_events            = 300;
static uint64_t max_additional_snapshot_events = 100;

struct UD // user data
{
    uint64_t req_time;
    int      events_read;
    int      snapshot_start_events_read;
};

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
    OTF2_Archive* archive = OTF2_Archive_Open( "OTF2_Snapshot_Seek_trace",
                                               "TestTrace",
                                               OTF2_FILEMODE_WRITE,
                                               256 * 1024,
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
    status = OTF2_Archive_SetCreator( archive, "OTF2_Snapshot_Seek" );
    check_status( status, "Set creator." );

    status = OTF2_Archive_OpenSnapFiles( archive );
    check_status( status, "Open snapshot files for writing." );

    /* Write snapshots */
    OTF2_SnapWriter* snap_writer = OTF2_Archive_GetSnapWriter( archive, 0 );
    check_pointer( snap_writer, "Get snapshot writer." );

    OTF2_AttributeList* attr_list = OTF2_AttributeList_New();
    srand( 3 );
    for ( int i = 0; i < number_of_snapshots; i++ )
    {
        OTF2_TimeStamp snaphot_time = snapshot_tick_rate * ( i + 1 );

        uint64_t number_of_events_in_snapshot =
            min_snapshot_events + rand() % max_additional_snapshot_events;

        status = OTF2_SnapWriter_SnapshotStart( snap_writer,
                                                attr_list,
                                                snaphot_time,
                                                number_of_events_in_snapshot );

        for ( uint64_t j = 0; j < number_of_events_in_snapshot; j++ )
        {
            double r = rand() / ( double )RAND_MAX;
            if ( r <= .25 )
            {
                status = OTF2_AttributeList_AddUint64( attr_list, 0, j );
                check_status( status, "Add attribute." );
            }
            status = OTF2_SnapWriter_Enter(
                snap_writer,
                attr_list,
                snaphot_time,
                0,
                0 );
            check_status( status, "Write event." );
        }

        status = OTF2_SnapWriter_SnapshotEnd( snap_writer, attr_list, snaphot_time, 0 );
    }
    OTF2_AttributeList_Delete( attr_list );

    status = OTF2_Archive_CloseSnapWriter( archive, snap_writer );
    check_pointer( snap_writer, "Get snapshot writer." );

    status = OTF2_Archive_CloseSnapFiles( archive );
    check_status( status, "Close snapshot files for writing." );

    /* Write location definitions. */
    OTF2_GlobalDefWriter* global_def_writer =
        OTF2_Archive_GetGlobalDefWriter( archive );
    check_pointer( global_def_writer, "Get global definition writer" );

    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 0, "" );
    check_status( status, "Write string definition." );

    status = OTF2_GlobalDefWriter_WriteSystemTreeNode(
        global_def_writer, 0, 0, 0, OTF2_UNDEFINED_UINT32 );
    check_status( status, "Write system tree node." );

    status = OTF2_GlobalDefWriter_WriteLocationGroup(
        global_def_writer, 0, 0, OTF2_LOCATION_GROUP_TYPE_PROCESS, 0 );
    check_status( status, "Write location group." );

    status = OTF2_GlobalDefWriter_WriteLocation(
        global_def_writer, 0, 0,
        OTF2_LOCATION_TYPE_CPU_THREAD,
        0,
        0 );
    check_status( status, "Write location." );

    /* Write definitions. */
    status = OTF2_GlobalDefWriter_WriteRegion(
        global_def_writer, 0, 0, 0, 0,
        OTF2_REGION_ROLE_UNKNOWN, OTF2_PARADIGM_UNKNOWN, OTF2_REGION_FLAG_NONE,
        OTF2_UNDEFINED_UINT32, 0, 0 );
    check_status( status, "Write region" );

    status = OTF2_GlobalDefWriter_WriteAttribute(
        global_def_writer, 0, 0, 0, OTF2_TYPE_UINT64 );
    check_status( status, "Write attribute" );

    status = OTF2_Archive_CloseGlobalDefWriter( archive, global_def_writer );
    check_status( status, "Closing global definition writer." );

    /* write local definitions */
    status = OTF2_Archive_OpenDefFiles( archive );
    check_status( status, "Open local definitions files." );

    OTF2_DefWriter* def_writer = OTF2_Archive_GetDefWriter( archive, 0 );
    check_pointer( def_writer, "Get definition writer." );

    status = OTF2_Archive_CloseDefWriter( archive,
                                          def_writer );
    check_status( status, "Close local definitions writer." );

    status = OTF2_Archive_CloseDefFiles( archive );
    check_status( status, "Close local definitions files." );

    /* Finalize. */
    OTF2_Archive_Close( archive );

    return OTF2_SUCCESS;
}



/** @internal
 *  @brief Seek snaphots and check them.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
check_archive( void )
{
    OTF2_ErrorCode status;

    /* Get a reader handle. */
    OTF2_Reader* reader = OTF2_Reader_Open( "OTF2_Snapshot_Seek_trace/TestTrace.otf2" );
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
                                                     NULL );
    check_status( status, "Register global definition callbacks." );

    OTF2_GlobalDefReaderCallbacks_Delete( check_defs );

    /* Read definitions. */
    uint64_t definitions_read = 0;
    status = OTF2_Reader_ReadGlobalDefinitions( reader, global_def_reader,
                                                OTF2_UNDEFINED_UINT64,
                                                &definitions_read );
    check_status( status, "Read global definitions." );

    status = OTF2_Reader_SelectLocation( reader, 0 );
    check_status( status, "Select location to read." );

    /* Read local definitions. */
    status = OTF2_Reader_OpenDefFiles( reader );
    check_status( status, "Open local def files for reading." );

    /* Open a definition reader and read all local definitions. */
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

    /* seek snaphots */
    status = OTF2_Reader_OpenSnapFiles( reader );
    check_status( status, "Open snapshot files for reading." );

    /* Open snap reader. */
    OTF2_SnapReader* snap_reader = OTF2_Reader_GetSnapReader( reader, 0 );
    check_pointer( snap_reader, "Create local event reader." );

    bool found;

    /* test1: look for a snapshot that doesn't exist */

    uint64_t req_time = 0;

    status = OTF2_SnapReader_Seek( snap_reader, req_time, &found );

    if ( found == true )
    {
        exit( EXIT_FAILURE );
    }

    /* test2: look for a snapshot that starts in the chunk that
     * OTF2_Buffer_ReadSeekSnapshotChunk returns */

    struct UD ud;

    ud.req_time                   = 42;
    ud.events_read                = 0;
    ud.snapshot_start_events_read = 0;

    status = OTF2_SnapReader_Seek( snap_reader, ud.req_time, &found );

    if ( !found )
    {
        exit( EXIT_FAILURE );
    }

    /* read snapshot: set & register event callbacks...  */
    OTF2_SnapReaderCallbacks* snapReaderCallbacks = OTF2_SnapReaderCallbacks_New();

    OTF2_SnapReaderCallbacks_SetSnapshotStartCallback(
        snapReaderCallbacks,
        snapshot_start_cb );

    OTF2_SnapReaderCallbacks_SetEnterCallback(
        snapReaderCallbacks,
        enter_snap_cb );

    OTF2_SnapReaderCallbacks_SetSnapshotEndCallback(
        snapReaderCallbacks,
        snapshot_end_cb );

    status = OTF2_Reader_RegisterSnapCallbacks( reader,
                                                snap_reader,
                                                snapReaderCallbacks,
                                                &ud );

    check_status( status, "Register event callbacks." );

    OTF2_SnapReaderCallbacks_Delete( snapReaderCallbacks );


    uint64_t events_read;
    while ( ud.snapshot_start_events_read < 2 )
    {
        status = OTF2_Reader_ReadLocalSnapshots( reader,
                                                 snap_reader,
                                                 1,
                                                 &events_read );
    }

    /* test3: look for a snapshot that doesn't start in the chunk that
     * OTF2_Buffer_ReadSeekSnapshotChunk returns ( need to read previous chunk ) */

    ud.req_time                   = 35;
    ud.events_read                = 0;
    ud.snapshot_start_events_read = 0;

    status = OTF2_SnapReader_Seek( snap_reader, ud.req_time, &found );

    if ( !found )
    {
        exit( EXIT_FAILURE );
    }

    while ( ud.snapshot_start_events_read < 2 )
    {
        status = OTF2_Reader_ReadLocalSnapshots( reader,
                                                 snap_reader,
                                                 1,
                                                 &events_read );
    }

    status = OTF2_Reader_CloseSnapReader( reader, snap_reader );
    check_status( status, "Close snapshot reader." );

    status = OTF2_Reader_CloseSnapFiles( reader );
    check_status( status, "Close snapshot files for reading." );

    /* Close reader */
    status = OTF2_Reader_Close( reader );
    check_status( status, "Close reader." );

    return OTF2_SUCCESS;
}


/* ___ Implementation of callbacks __________________________________________ */


OTF2_CallbackCode
snapshot_start_cb( uint64_t            locationID,
                   uint64_t            time,
                   void*               userData,
                   OTF2_AttributeList* attributeList,
                   uint64_t            numberOfEventsInSnapshot )
{
    struct UD* ud = ( struct UD* )userData;

    /* check first read snapshot start event */
    check_condition( ud->snapshot_start_events_read != 0 || ( ud->events_read == 0 && time <= ud->req_time ),
                     "Got first sanpshot start record." );

    /* check second read snapshot start event */
    check_condition( ud->snapshot_start_events_read == 0 || time > ud->req_time,
                     "Got second sanpshot start record." );

    ud->events_read++;

    ud->snapshot_start_events_read++;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
snapshot_end_cb( uint64_t            locationID,
                 uint64_t            time,
                 void*               userData,
                 OTF2_AttributeList* attributeList,
                 uint64_t            contReadPos )
{
    struct UD* ud = ( struct UD* )userData;

    ud->events_read++;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
enter_snap_cb( OTF2_LocationRef    location,
               OTF2_TimeStamp      snapTime,
               void*               userData,
               OTF2_AttributeList* attributeList,
               OTF2_TimeStamp      origEventTime,
               OTF2_RegionRef      region )
{
    struct UD* ud = ( struct UD* )userData;

    ud->events_read++;

    return OTF2_CALLBACK_SUCCESS;
}
