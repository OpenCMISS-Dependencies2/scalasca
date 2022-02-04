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


/** @brief Number of events per location. */
#define NUMBER_OF_EVENTS 10

/** @brief Archive path */
#define OTF2_ARCHIVE_PATH "OTF2_CallbackInterrupt_trace"

/** @brief Trace name */
#define OTF2_TRACE_NAME "TestTrace"


/* ___ Global variables. ____________________________________________________ */


/** @internal
 *  @brief Global time counter. */
static uint64_t otf2_Time;


/** @internal
 *  @brief Global region counter. */
static uint64_t otf2_Region;


static OTF2_ErrorCode
create_archive( bool use_sion );


static OTF2_ErrorCode
check_archive( const char* trace );


/* ___ Prototypes for all callbacks. ________________________________________ */


static OTF2_CallbackCode
enter_cb( uint64_t            locationID,
          uint64_t            time,
          uint64_t            eventPosition,
          void*               userData,
          OTF2_AttributeList* attributeList,
          OTF2_RegionRef      region );


static OTF2_CallbackCode
leave_cb( uint64_t            locationID,
          uint64_t            time,
          uint64_t            eventPosition,
          void*               userData,
          OTF2_AttributeList* attributeList,
          OTF2_RegionRef      region );


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
                                               1024 * 1024,
                                               4 * 1024 * 1024,
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
    status = OTF2_Archive_SetDescription( archive, "OTF2 trace for callback interrupt test." );
    check_status( status, "Set description." );
    status = OTF2_Archive_SetCreator( archive, "OTF2_CallbackInterrupt" );
    check_status( status, "Set creator." );

    /* Write location definitions. */
    OTF2_GlobalDefWriter* global_def_writer = NULL;
    global_def_writer = OTF2_Archive_GetGlobalDefWriter( archive );
    check_pointer( global_def_writer, "Get global definition writer" );

    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 0, "" );
    check_status( status, "Write string definition." );
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 1, "machine" );
    check_status( status, "Write string definition." );
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 2, "test" );
    check_status( status, "Write string definition." );
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 3, "process" );
    check_status( status, "Write string definition." );
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 4, "thread" );
    check_status( status, "Write string definition." );
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 5, "foo" );
    check_status( status, "Write string definition." );
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 6, "bar" );
    check_status( status, "Write string definition." );

    status = OTF2_GlobalDefWriter_WriteSystemTreeNode( global_def_writer,
                                                       0,
                                                       2,
                                                       1,
                                                       OTF2_UNDEFINED_SYSTEM_TREE_NODE );
    check_status( status, "Write system tree node definition." );

    status = OTF2_GlobalDefWriter_WriteLocationGroup( global_def_writer,
                                                      0,
                                                      3,
                                                      OTF2_LOCATION_GROUP_TYPE_PROCESS,
                                                      0 );
    check_status( status, "Write location group definition." );

    status = OTF2_GlobalDefWriter_WriteLocation( global_def_writer,
                                                 0,
                                                 4,
                                                 OTF2_LOCATION_TYPE_CPU_THREAD,
                                                 2 * NUMBER_OF_EVENTS,
                                                 0 );
    check_status( status, "Write location definition." );

    /* Write definitions. */
    status = OTF2_GlobalDefWriter_WriteRegion( global_def_writer,
                                               0,
                                               5, 6, 0,
                                               OTF2_REGION_ROLE_FUNCTION,
                                               OTF2_PARADIGM_USER,
                                               OTF2_REGION_FLAG_NONE,
                                               0, 0, 0 );
    check_status( status, "Write region definition" );

    status = OTF2_Archive_CloseGlobalDefWriter( archive, global_def_writer );
    check_status( status, "Closing global definition writer." );

    /* Define local event and definition writer. */
    OTF2_EvtWriter* evt_writer = NULL;

    status = OTF2_Archive_OpenEvtFiles( archive );
    check_status( status, "Open event files." );

    evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
    for ( uint64_t j = 0; j < NUMBER_OF_EVENTS; j++ )
    {
        /* Write enter. */
        uint64_t ts = otf2_test_gettime();
        status = OTF2_EvtWriter_Enter( evt_writer, NULL, ts, 0 );
        check_status( status, "Write Enter." );
    }
    for ( uint64_t j = 0; j < NUMBER_OF_EVENTS; j++ )
    {
        /* Write leave. */
        uint64_t ts = otf2_test_gettime();
        status = OTF2_EvtWriter_Leave( evt_writer, NULL, ts, 0 );
        check_status( status, "Write Leave." );
    }
    status = OTF2_Archive_CloseEvtWriter( archive, evt_writer );
    check_status( status, "Close event writer." );

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
#if HAVE( PTHREAD )
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
#if HAVE( PTHREAD_MUTEX_ERRORCHECK )
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
#endif
    status = OTF2_Pthread_Reader_SetLockingCallbacks( reader, &attr );
    check_status( status, "Set Pthread locking callbacks." );
#endif

    status = OTF2_Reader_SelectLocation( reader, 0 );
    check_status( status, "Select location to read." );

    status = OTF2_Reader_OpenEvtFiles( reader );
    check_status( status, "Open event files for reading." );

    /* Get global event reader. */
    OTF2_EvtReader* evt_reader = OTF2_Reader_GetEvtReader( reader, 0 );
    check_pointer( evt_reader, "Create event reader." );

    /* Define and register event callbacks. */
    OTF2_EvtReaderCallbacks* check_evts = OTF2_EvtReaderCallbacks_New();
    check_pointer( check_evts, "Create event callbacks." );

    OTF2_EvtReaderCallbacks_SetEnterCallback( check_evts, enter_cb );
    OTF2_EvtReaderCallbacks_SetLeaveCallback( check_evts, leave_cb );

    uint32_t counter = 0;
    status = OTF2_Reader_RegisterEvtCallbacks( reader,
                                               evt_reader,
                                               check_evts,
                                               &counter );
    check_status( status, "Register global event callbacks." );

    OTF2_EvtReaderCallbacks_Delete( check_evts );

    /* Read the first NUMBER_OF_EVENTS records (the enters) */
    uint64_t events_read = 0;
    status = OTF2_Reader_ReadLocalEvents( reader,
                                          evt_reader,
                                          OTF2_UNDEFINED_UINT64,
                                          &events_read );
    check_condition( status == OTF2_ERROR_INTERRUPTED_BY_CALLBACK,
                     "Expected OTF2_ERROR_INTERRUPTED_BY_CALLBACK." );
    check_condition( events_read == NUMBER_OF_EVENTS,
                     "Didn't read %d events.", NUMBER_OF_EVENTS );

    /* Read the second NUMBER_OF_EVENTS records (the leaves) */
    events_read = 0;
    status      = OTF2_Reader_ReadLocalEvents( reader,
                                               evt_reader,
                                               OTF2_UNDEFINED_UINT64,
                                               &events_read );
    check_status( status, "Expected OTF2_SUCCESS." );
    check_condition( events_read == NUMBER_OF_EVENTS,
                     "Didn't read %d events.", NUMBER_OF_EVENTS );

    status = OTF2_Reader_CloseEvtReader( reader, evt_reader );
    check_status( status, "Close event reader." );

    status = OTF2_Reader_CloseEvtFiles( reader );
    check_status( status, "Close event files for reading." );

    /* Close reader */
    status = OTF2_Reader_Close( reader );
    check_status( status, "Close reader." );

    return OTF2_SUCCESS;
}



/* ___ Implementation of callbacks __________________________________________ */


OTF2_CallbackCode
enter_cb( uint64_t            locationID,
          uint64_t            time,
          uint64_t            eventPosition,
          void*               userData,
          OTF2_AttributeList* attributeList,
          OTF2_RegionRef      region )
{
    uint32_t* counter = userData;

    ( *counter )++;

    if ( ( *counter ) == NUMBER_OF_EVENTS )
    {
        return OTF2_CALLBACK_INTERRUPT;
    }

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
leave_cb( uint64_t            locationID,
          uint64_t            time,
          uint64_t            eventPosition,
          void*               userData,
          OTF2_AttributeList* attributeList,
          OTF2_RegionRef      region )
{
    uint32_t* counter = userData;

    ( *counter )++;

    return OTF2_CALLBACK_SUCCESS;
}

/** @} */
