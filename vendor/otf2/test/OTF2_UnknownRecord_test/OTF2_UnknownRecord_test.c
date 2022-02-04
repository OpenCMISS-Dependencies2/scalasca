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
 *  @brief      Generates a event sequence including unknown records of unknown type.
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

#include <otf2_internal.h>
#include <otf2_file_types.h>
#include <otf2_reader.h>
#include <otf2_archive.h>

#include <otf2_attribute_value.h>
#include <otf2_attribute_list.h>
#include <otf2_id_map.h>

#include <OTF2_File.h>
#include <OTF2_Buffer.h>

#include <otf2_events_int.h>
#include <otf2_evt_writer.h>

#include <otf2-test/check.h>

#include <otf2-test/gettime.h>

#define OTF2_TEST_FLUSH_DEFAULT OTF2_FLUSH
#define OTF2_TEST_FLUSH_GETTIME otf2_test_gettime
#include <otf2-test/flush.h>


#define OTF2_UNKNOWN_RECORD_TEST_CHUNK_SIZE ( 1024 * 1024 )
#define OTF2_UNKNOWN_RECORD_TEST_NUMBER_OF_EVENTS 6
#define OTF2_UNKNOWN_RECORD_TEST_NUMBER_OF_UNKNOWN_RECORDS 4
#define OTF2_UNKNOWN_RECORD_TEST_TRACE_DIR "./unknown_record_test_trace"
#define OTF2_UNKNOWN_RECORD_TEST_TRACE_NAME "traces"


/* ___ Global variables. ____________________________________________________ */


/** @internal
 *  @brief Global time counter. */
static uint64_t expected_time = 0;


/** @internal
 *  @brief Global region counter. */
static uint64_t expected_region = 0;


/** @internal
 *  @brief Global counter of unknown records. */
static uint64_t unknown_records_read = 0;


/* ___ Function prototypes. _________________________________________________ */


static OTF2_ErrorCode
create_archive( bool use_sion );

static OTF2_ErrorCode
check_archive( void );

static void
write_definitions( OTF2_Archive* archive );

static void
write_events( OTF2_Archive* archive );

static OTF2_CallbackCode
def_location_cb( void*             userData,
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
unknown_cb( uint64_t            locationID,
            uint64_t            time,
            void*               userData,
            OTF2_AttributeList* attributes );

int
main( int    argc,
      char** argv )
{
    OTF2_ErrorCode status;

    status = create_archive( argc > 1 );
    check_status( status, "Create archive." );

    check_archive();
    check_status( status, "Check archive." );

    return 0;
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

    OTF2_Archive* archive = OTF2_Archive_Open( OTF2_UNKNOWN_RECORD_TEST_TRACE_DIR,
                                               OTF2_UNKNOWN_RECORD_TEST_TRACE_NAME,
                                               OTF2_FILEMODE_WRITE,
                                               OTF2_UNKNOWN_RECORD_TEST_CHUNK_SIZE,
                                               4 * 1024 * 1024,
                                               use_sion
                                               ? OTF2_SUBSTRATE_SION
                                               : OTF2_SUBSTRATE_POSIX,
                                               OTF2_COMPRESSION_NONE );
    check_pointer( archive, "Create writer archive handle" );

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

    /* Write definitions */
    write_definitions( archive );

    /* Write events */
    write_events( archive );

    status = OTF2_Archive_Close( archive );
    check_status( status, "Close archive." );

    return status;
}

void
write_definitions( OTF2_Archive* archive )
{
    OTF2_ErrorCode status;

    /* Write location definitions. */
    OTF2_GlobalDefWriter* global_def_writer = NULL;
    global_def_writer = OTF2_Archive_GetGlobalDefWriter( archive );
    check_pointer( global_def_writer, "Get global definition writer" );

    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 0, "Process" );
    check_status( status, "Write string definition." );

    status = OTF2_GlobalDefWriter_WriteLocation( global_def_writer,
                                                 0,  // location ID
                                                 0,
                                                 OTF2_LOCATION_TYPE_CPU_THREAD,
                                                 OTF2_UNKNOWN_RECORD_TEST_NUMBER_OF_EVENTS + OTF2_UNKNOWN_RECORD_TEST_NUMBER_OF_UNKNOWN_RECORDS,
                                                 0 );
    check_status( status, "Write location definition." );

    /* Write definitions. */
    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 1, "simple_example.c" );
    check_status( status, "Write string definition" );

    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 2, "main" );
    check_status( status, "Write string definition" );

    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 3, "foo" );
    check_status( status, "Write string definition" );

    status = OTF2_GlobalDefWriter_WriteString( global_def_writer, 4, "bar" );
    check_status( status, "Write string definition" );

    status = OTF2_GlobalDefWriter_WriteRegion( global_def_writer, 0, 2, 2, 0, OTF2_REGION_ROLE_UNKNOWN, OTF2_PARADIGM_UNKNOWN, OTF2_REGION_FLAG_NONE, 1, 0, 0 );
    check_status( status, "Write region definition" );

    status = OTF2_GlobalDefWriter_WriteRegion( global_def_writer, 1, 3, 3, 0, OTF2_REGION_ROLE_UNKNOWN, OTF2_PARADIGM_UNKNOWN, OTF2_REGION_FLAG_NONE, 1, 0, 0 );
    check_status( status, "Write region definition" );

    status = OTF2_GlobalDefWriter_WriteRegion( global_def_writer, 2, 4, 4, 0, OTF2_REGION_ROLE_UNKNOWN, OTF2_PARADIGM_UNKNOWN, OTF2_REGION_FLAG_NONE, 1, 0, 0 );
    check_status( status, "Write region definition" );

    status = OTF2_Archive_CloseGlobalDefWriter( archive, global_def_writer );
    check_status( status, "Closing global definition writer." );

    /* write local definitions */
    status = OTF2_Archive_OpenDefFiles( archive );
    check_status( status, "Open local definitions files." );

    /* Just open a definition writer, so the file is created. */
    OTF2_DefWriter* def_writer = OTF2_Archive_GetDefWriter( archive, 0 );
    check_pointer( def_writer, "Get definition writer." );

    status = OTF2_Archive_CloseDefWriter( archive,
                                          def_writer );
    check_status( status, "Close local definitions writer." );

    status = OTF2_Archive_CloseDefFiles( archive );
    check_status( status, "Close local definitions files." );
}

void
write_events( OTF2_Archive* archive )
{
    OTF2_ErrorCode status;

    status = OTF2_Archive_OpenEvtFiles( archive );
    check_status( status, "Open event files." );

    /* Open a new buffer handle. */
    OTF2_EvtWriter* evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
    check_pointer( evt_writer, "Get event writer." );

    OTF2_Buffer* buffer = evt_writer->buffer;

    /* Maximum record length of an enter/leave record is the record id byte
     * plus the individual size of the attributes (uint32_t region,
     * excluding timestamp). */
    size_t record_length = 6;
    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    OTF2_Buffer_WriteUint8( buffer, OTF2_EVENT_ENTER );
    OTF2_Buffer_WriteUint32( buffer, 0 );

    /* Write record of unknown type */
    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    /* Write unknown record type */
    OTF2_Buffer_WriteUint8( buffer, 0xFF );
    /* Write record length byte */
    OTF2_Buffer_WriteUint8( buffer, 1 );
    /* Write dummy data */
    OTF2_Buffer_WriteUint8( buffer, 0 );

    /* Write record of unknown type */
    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    /* Write unknown record type */
    OTF2_Buffer_WriteUint8( buffer, 0xFF );
    /* Write length for a large record, that means first
     * record length is set to UINT8_MAX and length information
     * is encoded in additional 8 bytes */
    OTF2_Buffer_WriteUint8( buffer, UINT8_MAX );
    /* Write length in additional 8 bytes */
    OTF2_Buffer_WriteUint64Full( buffer, 2 );
    /* Write dummy data */
    OTF2_Buffer_WriteUint16( buffer, 0 );

    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    OTF2_Buffer_WriteUint8( buffer, OTF2_EVENT_ENTER );
    OTF2_Buffer_WriteUint32( buffer, 1 );


    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    OTF2_Buffer_WriteUint8( buffer, OTF2_EVENT_ENTER );
    OTF2_Buffer_WriteUint32( buffer, 2 );

    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    OTF2_Buffer_WriteUint8( buffer, OTF2_EVENT_LEAVE );
    OTF2_Buffer_WriteUint32( buffer, 2 );

    /* Write record of unknown type */
    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    /* Write unknown record type */
    OTF2_Buffer_WriteUint8( buffer, 0xFF );
    /* Write record length byte */
    OTF2_Buffer_WriteUint8( buffer, 9 );
    /* Write dummy data */
    OTF2_Buffer_WriteUint64Full( buffer, 0 );
    OTF2_Buffer_WriteUint8( buffer, 0 );


    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    OTF2_Buffer_WriteUint8( buffer, OTF2_EVENT_LEAVE );
    OTF2_Buffer_WriteUint32( buffer, 1 );

    /* Write record of unknown type */
    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    /* Write unknown record type */
    OTF2_Buffer_WriteUint8( buffer, 0xFF );
    /* Write record length byte */
    OTF2_Buffer_WriteUint8( buffer, 0 );
    /* No dummy data needed */


    status = OTF2_Buffer_WriteTimeStamp( buffer,
                                         otf2_test_gettime(),
                                         record_length );
    check_status( status, "Write timestamp" );
    OTF2_Buffer_WriteUint8( buffer, OTF2_EVENT_LEAVE );
    OTF2_Buffer_WriteUint32( buffer, 0 );

    status = OTF2_Archive_CloseEvtWriter( archive, evt_writer );
    check_status( status, "Close event writer." );

    status = OTF2_Archive_CloseEvtFiles( archive );
    check_status( status, "Close event files." );
}

OTF2_ErrorCode
check_archive( void )
{
    OTF2_ErrorCode status;

    /* Get string length of trace file path:
     * length of path to directory
     * + one additional character for directory separator
     * + length of trace file name
     * + five additional for '.otf2' file extension */
    size_t string_length = strlen( OTF2_UNKNOWN_RECORD_TEST_TRACE_DIR ) + strlen( OTF2_UNKNOWN_RECORD_TEST_TRACE_DIR ) + 6;
    char*  trace_name    = malloc( ( string_length + 1 ) * sizeof( char ) );
    if ( trace_name == NULL )
    {
        return OTF2_ERROR_ENOMEM;
    }
    snprintf( trace_name, string_length, "%s/%s.otf2", OTF2_UNKNOWN_RECORD_TEST_TRACE_DIR, OTF2_UNKNOWN_RECORD_TEST_TRACE_NAME );

    /* Open reader. */
    OTF2_Reader* reader = OTF2_Reader_Open( trace_name );
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

    /* Get number of locations from the anchor file. */
    uint64_t num_locations = 0;
    status = OTF2_Reader_GetNumberOfLocations( reader, &num_locations );
    check_status( status, "Get number of locations. Number of locations: %" PRIu64,
                  num_locations );
    check_condition( num_locations == 1, "Expecting one location." );

    /* Get global definition reader. */
    OTF2_GlobalDefReader* global_def_reader = OTF2_Reader_GetGlobalDefReader( reader );
    check_pointer( global_def_reader, "Create global definition reader handle." );

    /* Define and register definition callbacks. */
    OTF2_GlobalDefReaderCallbacks* check_defs = OTF2_GlobalDefReaderCallbacks_New();
    check_pointer( check_defs, "Create global definition callback handle." );
    OTF2_GlobalDefReaderCallbacks_SetLocationCallback( check_defs, def_location_cb );

    OTF2_LocationRef location_to_read;
    status = OTF2_Reader_RegisterGlobalDefCallbacks( reader, global_def_reader,
                                                     check_defs,
                                                     &location_to_read );
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

    printf( "Read %" PRIu64 " global definitions\n", definitions_read );

    status = OTF2_Reader_SelectLocation( reader, location_to_read );
    check_status( status, "Select location to read." );

    /* Read local definitions. */
    status = OTF2_Reader_OpenDefFiles( reader );
    check_status( status, "Open local def files for reading." );

    /* Also open a definition reader and read all local definitions. */
    OTF2_DefReader* def_reader = OTF2_Reader_GetDefReader( reader,
                                                           location_to_read );
    check_pointer( def_reader, "Create local definition reader for location %" PRIu64 ".",
                   location_to_read );

    definitions_read = 0;
    status           = OTF2_Reader_ReadAllLocalDefinitions( reader, def_reader, &definitions_read );
    check_status( status, "Read %" PRIu64 " definitions for location %" PRIu64,
                  definitions_read, location_to_read );

    /* Close def reader, it is no longer useful and occupies memory */
    status = OTF2_Reader_CloseDefReader( reader, def_reader );
    check_status( status, "Close local definition reader." );

    status = OTF2_Reader_CloseDefFiles( reader );
    check_status( status, "Close local def files for reading." );

    /* Open and read event files. */
    status = OTF2_Reader_OpenEvtFiles( reader );
    check_status( status, "Open event files for reading." );

    /* Open a new local event reader for each found location ID. */
    OTF2_EvtReader* evt_reader = OTF2_Reader_GetEvtReader( reader,
                                                           location_to_read );
    check_pointer( evt_reader, "Create local event reader for location %" PRIu64 ".",
                   location_to_read );

    /* Get global event reader. */
    OTF2_GlobalEvtReader* global_evt_reader = OTF2_Reader_GetGlobalEvtReader( reader );
    check_pointer( global_evt_reader, "Create global event reader." );

    /* Define and register event callbacks. */
    OTF2_GlobalEvtReaderCallbacks* check_evts = OTF2_GlobalEvtReaderCallbacks_New();
    check_pointer( check_evts, "Create global event callbacks." );

    OTF2_GlobalEvtReaderCallbacks_SetEnterCallback( check_evts, enter_cb );
    OTF2_GlobalEvtReaderCallbacks_SetLeaveCallback( check_evts, leave_cb );
    OTF2_GlobalEvtReaderCallbacks_SetUnknownCallback( check_evts, unknown_cb );

    status = OTF2_Reader_RegisterGlobalEvtCallbacks( reader, global_evt_reader,
                                                     check_evts,
                                                     NULL );
    check_status( status, "Register global event callbacks." );

    OTF2_GlobalEvtReaderCallbacks_Delete( check_evts );

    /* Read until an error occures. */
    uint64_t events_read = 0;
    status = OTF2_Reader_ReadGlobalEvents( reader, global_evt_reader, OTF2_UNDEFINED_UINT64, &events_read );
    check_status( status, "Read %" PRIu64 " events.", events_read );

    check_condition( events_read == ( OTF2_UNKNOWN_RECORD_TEST_NUMBER_OF_EVENTS + OTF2_UNKNOWN_RECORD_TEST_NUMBER_OF_UNKNOWN_RECORDS ),
                     "Number of events read does not match!" );
    check_condition( unknown_records_read == OTF2_UNKNOWN_RECORD_TEST_NUMBER_OF_UNKNOWN_RECORDS,
                     "Number of unknown records read does not match!" );

    status = OTF2_Reader_CloseGlobalEvtReader( reader, global_evt_reader );
    check_status( status, "Close global event reader." );

    status = OTF2_Reader_CloseEvtFiles( reader );
    check_status( status, "Close event files for reading." );


    printf( "Read %" PRIu64 " events\n", events_read );
    printf( "Read %" PRIu64 " unknown records\n", unknown_records_read );

    /* Close reader. */
    status = OTF2_Reader_Close( reader );
    check_status( status, "Close Reader." );

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
def_location_cb( void*             userData,
                 OTF2_LocationRef  locationIdentifier,
                 uint32_t          stringID,
                 OTF2_LocationType locationType,
                 uint64_t          numberOfEvents,
                 uint32_t          locationGroup )
{
    /* Check values. */
    check_condition( numberOfEvents == OTF2_UNKNOWN_RECORD_TEST_NUMBER_OF_EVENTS + OTF2_UNKNOWN_RECORD_TEST_NUMBER_OF_UNKNOWN_RECORDS,
                     "Check Location Definition." );

    OTF2_LocationRef* location_to_read = userData;
    *location_to_read = locationIdentifier;

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
    if ( time != expected_time )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Timestamp failure." );
    }
    expected_time++;

    if ( regionID != expected_region )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Region ID failure." );
    }
    expected_region++;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
leave_cb( uint64_t            locationID,
          uint64_t            time,
          void*               userData,
          OTF2_AttributeList* attributes,
          uint32_t            regionID )
{
    if ( time != expected_time )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Timestamp failure." );
    }
    expected_time++;

    expected_region--;
    if ( regionID != expected_region )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Region ID failure." );
    }

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
unknown_cb( uint64_t            locationID,
            uint64_t            time,
            void*               userData,
            OTF2_AttributeList* attributes )
{
    if ( time != expected_time )
    {
        check_status( OTF2_ERROR_INTEGRITY_FAULT, "Timestamp failure." );
    }
    expected_time++;

    unknown_records_read++;

    return OTF2_CALLBACK_SUCCESS;
}
/** @} */
