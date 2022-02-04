/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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
 *  @brief      Sets defined information in the archive and writes an anchor
 *              file. Then reads the anchor file back in and compares the read
 *              information with the defined information.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>

#include <otf2/otf2.h>

#if HAVE( PTHREAD )
#include <otf2/OTF2_Pthread_Locks.h>
#endif

#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <UTILS_IO.h>
#include <UTILS_CStr.h>

#include <otf2_internal.h>
#include <otf2_file_types.h>
#include <otf2_reader.h>
#include <otf2_archive.h>

#include <otf2-test/check.h>

#define OTF2_TEST_CHUNK_POOL_CHUNK_SIZE ( 256 * 1024 )
#define OTF2_TEST_CHUNK_POOL_NUMBER_OF_CHUNKS 4
#include <otf2-test/chunk-pool.h>

#include <otf2-test/gettime.h>

#define OTF2_TEST_FLUSH_DEFAULT OTF2_FLUSH
#define OTF2_TEST_FLUSH_GETTIME otf2_test_gettime
#include <otf2-test/flush.h>


/* ___ main _________________________________________________________________ */



int
main( void )
{
    char archive_path[] = "OTF2_AnchorFile_trace/trace";
    char archive_name[] = "archive.otf2";

    /* Information stored in the trace. */
    uint64_t           chunk_size_events = 1024 * 1024;
    uint64_t           chunk_size_defs   = 4 * 1024 * 1024;
    OTF2_FileSubstrate substrate         = OTF2_SUBSTRATE_POSIX;
    OTF2_Compression   compression       = OTF2_COMPRESSION_NONE;
    uint64_t           num_locations     = 123;
    uint64_t           num_definitions   = 456;
    char*              machine_name      = "TestMachine";
    char*              creator           = "TestCreator";
    char*              description       = "TestDescription";
    /* We will write only two properties. The third one will be deleted,
     * that means it won't be written to anchor file. */
    const char* const invalid_property_names[] =
    {
        "",              // empty
        "foo",           // no "::" at all
        "::foo",         // empty first component 1
        "::foo::bar",    // empty first component 2
        "foo::::",       // empty middle component 1
        "::::foo",       // empty middle component 2
        "foo::",         // empty last component 1
        "foo::bar::",    // empty last component 2
        "foo::ba ::baz", // invalid character 1
        "foo::bar::ba ", // invalid character 2
        NULL
    };
    char*    string_property_names[]     = { "NamespaceA::Property1", "NamespaceA::Property2", "NamespaceA::Property3" };
    char*    string_property_values[]    = { "Valid", "OK", "true" };
    char*    bool_property_names[]       = { "NamespaceB::BoolProperty1", "NamespaceB::BoolProperty2" };
    bool     bool_property_values[]      = { true, false };
    uint32_t number_of_string_properties = sizeof( string_property_names ) / sizeof( *string_property_names );
    uint32_t number_of_bool_properties   = sizeof( bool_property_names ) / sizeof( *bool_property_names );
    uint32_t number_of_properties        = number_of_string_properties + number_of_bool_properties;
    char*    property_name_to_delete     = "Whatever::Property";
    char*    arbitrary_property_value    = "Whatever::Value";
    char*    value_to_delete_property    = "";

    /* Print out info. */
    if ( otf2_DEBUG )
    {
        printf( "\n=== OTF2_ANCHORFILE_TEST [DEBUG MODE] ===\n\n" );
    }
    else
    {
        printf( "\n=== OTF2_ANCHORFILE_TEST ===\n\n" );
    }

    char* extension_dot = strrchr( archive_name, '.' );

    /* cut the extension from the archive_name when writing */
    *extension_dot = '\0';

    /* Create new archive handle. */
    OTF2_Archive* archive = OTF2_Archive_Open( archive_path,
                                               archive_name,
                                               OTF2_FILEMODE_WRITE,
                                               chunk_size_events,
                                               chunk_size_defs,
                                               substrate,
                                               compression );
    check_pointer( archive, "Open new archive." );


    OTF2_ErrorCode status = OTF2_Archive_SetFlushCallbacks( archive, &otf2_test_flush_callbacks, NULL );
    check_status( status, "Set flush callbacks." );

#if HAVE( PTHREAD )
    pthread_mutexattr_t mutex_attr_archive;
    pthread_mutexattr_init( &mutex_attr_archive );
#if HAVE( PTHREAD_MUTEX_ERRORCHECK )
    pthread_mutexattr_settype( &mutex_attr_archive, PTHREAD_MUTEX_ERRORCHECK );
#endif
    status = OTF2_Pthread_Archive_SetLockingCallbacks( archive, &mutex_attr_archive );
    check_status( status, "Set Pthread locking callbacks." );
#endif

    /* Set number of locations. */
    status = otf2_archive_set_number_of_locations( archive, num_locations );
    check_status( status, "Set number of locations." );

    /* Set number of global definitions. */
    status = otf2_archive_set_number_of_global_defs( archive, num_definitions );
    check_status( status, "Set number of definitions." );

    /* Set machine name, creator and description (optional) */
    status = OTF2_Archive_SetMachineName( archive, machine_name );
    check_status( status, "Set machine name." );
    status = OTF2_Archive_SetCreator( archive, creator );
    check_status( status, "Set creator." );
    status = OTF2_Archive_SetDescription( archive, description );
    check_status( status, "Set description." );

    status = OTF2_Archive_SetSerialCollectiveCallbacks( archive );
    check_status( status, "Set serial mode." );

    /* Check for some invalid property names */
    for ( const char* const* invalid_property_name_it = invalid_property_names;
          *invalid_property_name_it;
          invalid_property_name_it++ )
    {
        status = OTF2_Archive_SetProperty( archive,
                                           *invalid_property_name_it,
                                           "value",
                                           true );
        if ( status == OTF2_SUCCESS )
        {
            status = OTF2_ERROR_PROPERTY_NAME_INVALID;
        }
        else if ( status == OTF2_ERROR_PROPERTY_NAME_INVALID )
        {
            status = OTF2_SUCCESS;
        }
        check_status( status, "Test invalid property names failed for %s.", *invalid_property_name_it );
    }

    /* Write some additional meta-information to anchor file. */
    status = OTF2_Archive_SetProperty( archive,
                                       property_name_to_delete,
                                       value_to_delete_property,
                                       false );
    check_status( status, "Set empty string as value of a non-existing property should have no effect." );

    status = OTF2_Archive_SetProperty( archive,
                                       property_name_to_delete,
                                       arbitrary_property_value,
                                       false );
    check_status( status, "Write arbitrary property that will be deleted later on." );
    status = OTF2_Archive_SetProperty( archive,
                                       property_name_to_delete,
                                       value_to_delete_property,
                                       true );
    check_status( status, "Delete property at the front of the list." );

    status = OTF2_Archive_SetProperty( archive,
                                       string_property_names[ 0 ],
                                       arbitrary_property_value,
                                       false );
    check_status( status, "Write first property." );

    status = OTF2_Archive_SetProperty( archive,
                                       string_property_names[ 1 ],
                                       string_property_values[ 1 ],
                                       false );
    check_status( status, "Write second property." );

    status = OTF2_Archive_SetProperty( archive,
                                       string_property_names[ 0 ],
                                       string_property_values[ 0 ],
                                       true );
    check_status( status, "Re-write first property." );

    status = OTF2_Archive_SetProperty( archive,
                                       property_name_to_delete,
                                       arbitrary_property_value,
                                       false );
    check_status( status, "Write arbitrary property that will be deleted later on." );

    status = OTF2_Archive_SetProperty( archive,
                                       string_property_names[ 2 ],
                                       string_property_values[ 2 ],
                                       false );
    check_status( status, "Write third property." );

    status = OTF2_Archive_SetProperty( archive,
                                       property_name_to_delete,
                                       value_to_delete_property,
                                       true );
    check_status( status, "Delete property somewhere in the middle of the list." );

    status = OTF2_Archive_SetProperty( archive,
                                       property_name_to_delete,
                                       arbitrary_property_value,
                                       false );
    check_status( status, "Write arbitrary property that will be deleted later on." );
    status = OTF2_Archive_SetProperty( archive,
                                       property_name_to_delete,
                                       value_to_delete_property,
                                       true );
    check_status( status, "Delete   property at the end of the list." );

    status = OTF2_Archive_SetBoolProperty( archive,
                                           bool_property_names[ 0 ],
                                           bool_property_values[ 0 ],
                                           false );
    check_status( status, "Write fourth property (first one of type boolean)." );

    status = OTF2_Archive_SetBoolProperty( archive,
                                           bool_property_names[ 1 ],
                                           bool_property_values[ 1 ],
                                           false );
    check_status( status, "Write fifth property (second one of type boolean)." );

    /* Close archive. */
    status = OTF2_Archive_Close( archive );
    check_status( status, "Close archive." );




    /* Open new archive. */
    /* add the extension back to the archive_name when reading */
    *extension_dot = '.';

    char* full_path = UTILS_IO_JoinPath( 2, archive_path, archive_name );
    check_pointer( full_path, "Could not create full path to anchor file." );

    OTF2_Reader* reader = OTF2_Reader_Open( full_path );
    check_pointer( reader, "Open new reader for reading." );
    status = OTF2_Reader_SetSerialCollectiveCallbacks( reader );
    check_status( status, "Set serial mode." );
#if HAVE( PTHREAD )
    pthread_mutexattr_t mutex_attr_reader;
    pthread_mutexattr_init( &mutex_attr_reader );
#if HAVE( PTHREAD_MUTEX_ERRORCHECK )
    pthread_mutexattr_settype( &mutex_attr_reader, PTHREAD_MUTEX_ERRORCHECK );
#endif
    status = OTF2_Pthread_Reader_SetLockingCallbacks( reader, &mutex_attr_reader );
    check_status( status, "Set Pthread locking callbacks." );
#endif

    free( full_path );

    /* Information read from the trace. */
    uint8_t            version_read[ 3 ];
    uint64_t           num_locations_read;
    uint64_t           num_definitions_read;
    uint64_t           chunk_size_events_read;
    uint64_t           chunk_size_defs_read;
    OTF2_FileSubstrate substrate_read;
    OTF2_Compression   compression_read;
    char*              machine_name_read;
    char*              creator_read;
    char*              description_read;
    char**             property_names_read;
    char*              string_property_value_read;
    bool               bool_property_value_read;
    uint32_t           number_of_properties_read;



    /* Get version. */
    status = OTF2_Reader_GetVersion( reader,
                                     &version_read[ 0 ],
                                     &version_read[ 1 ],
                                     &version_read[ 2 ] );
    check_status( status, "Get version." );
    check_condition( OTF2_VERSION_MAJOR == version_read[ 0 ],
                     "Major version does not match: "
                     "Expected %u. Got %u.",
                     OTF2_VERSION_MAJOR, version_read[ 0 ] );
    check_condition( OTF2_VERSION_MINOR == version_read[ 1 ],
                     "Minor version does not match: "
                     "Expected %u. Got %u.",
                     OTF2_VERSION_MINOR, version_read[ 1 ] );
    check_condition( OTF2_VERSION_BUGFIX == version_read[ 2 ],
                     "Bugfix version does not match: "
                     "Expected %u. Got %u.",
                     OTF2_VERSION_BUGFIX, version_read[ 2 ] );

    /* Get number of locations. */
    status = OTF2_Reader_GetNumberOfLocations( reader,
                                               &num_locations_read );
    check_status( status, "Get number of locations." );
    check_condition( num_locations == num_locations_read,
                     "Number of locations does not match: "
                     "Expected %" PRIu64 ". Got %" PRIu64 ".",
                     num_locations, num_locations_read );

    /* Get number of definitions. */
    status = OTF2_Reader_GetNumberOfGlobalDefinitions( reader,
                                                       &num_definitions_read );
    check_status( status, "Get number of definitions." );
    check_condition( num_definitions == num_definitions_read,
                     "Number of definitions does not match: "
                     "Expected %" PRIu64 ". Got %" PRIu64 ".",
                     num_definitions, num_definitions_read );

    /* Get chunk sizes. */
    status = OTF2_Reader_GetChunkSize( reader,
                                       &chunk_size_events_read,
                                       &chunk_size_defs_read );
    check_status( status, "Get chuck sizes." );
    check_condition( chunk_size_events == chunk_size_events_read,
                     "Event chunk size does not match: "
                     "Expected %" PRIu64 ". Got %" PRIu64 ".",
                     chunk_size_events, chunk_size_events_read );
    check_condition( chunk_size_defs == chunk_size_defs_read,
                     "Definition chunk size does not match: "
                     "Expected %" PRIu64 ". Got %" PRIu64 ".",
                     chunk_size_defs, chunk_size_defs_read );

    /* Get substrate. */
    status = OTF2_Reader_GetFileSubstrate( reader,
                                           &substrate_read );
    check_status( status, "Get file substrate." );
    check_condition( substrate == substrate_read,
                     "File subtrate does not match: Expected %u. Got %u.",
                     substrate, substrate_read );

    /* Get compression. */
    status = OTF2_Reader_GetCompression( reader,
                                         &compression_read );
    check_status( status, "Get compression." );
    check_condition( compression == compression_read,
                     "Compression does not match: Expected %u. Got %u.",
                     compression, compression_read );

    /* Get machine name */
    status = OTF2_Reader_GetMachineName( reader, &machine_name_read );
    check_status( status, "Get machine name." );
    check_condition( 0 == strcmp( machine_name, machine_name_read ),
                     "Machine name does not match: Expected %s. Got %s.",
                     machine_name, machine_name_read );
    free( machine_name_read );

    status = OTF2_Reader_GetCreator( reader, &creator_read );
    check_status( status, "Get creator." );
    check_condition( 0 == strcmp( creator, creator_read ),
                     "Creator does not match: Expected %s. Got %s.",
                     creator, creator_read );
    free( creator_read );

    status = OTF2_Reader_GetDescription( reader, &description_read );
    check_status( status, "Get description." );
    check_condition( 0 == strcmp( description, description_read ),
                     "Description does not match: Expected %s. Got %s.",
                     description, description_read );
    free( description_read );

    /* Get additional meta-information (a.k.a. properties) */
    status = OTF2_Reader_GetPropertyNames( reader, &number_of_properties_read, &property_names_read );
    check_status( status, "Get number of properties." );

    /* Check number of properties */
    check_condition( number_of_properties == number_of_properties_read,
                     "Number of properties does not match: Expected %u. Got %u.",
                     number_of_properties, number_of_properties_read );

    char* converted_name;
    for ( uint32_t i = 0; i < number_of_properties_read; i++ )
    {
        /*
         * First, this test writes #number_of_string_properties properties of type
         * string to the anchor file. Afterwards, #number_of_bool_properties are
         * written to the anchor file.
         */

        /* Property names are store in upper case. If everything works correctly,
         * OTF2_Reader_GetProperty will return our property names converted from
         * mixed case to upper case. Therefore, we also have to convert property
         * names to upper case. */
        if ( i < number_of_string_properties )
        {
            converted_name = UTILS_CStr_dup( string_property_names[ i ] );
        }
        else
        {
            converted_name = UTILS_CStr_dup( bool_property_names[ i - number_of_string_properties ] );
        }
        for ( char* ptr = converted_name; *ptr; ptr++ )
        {
            *ptr = toupper( *ptr );
        }

        /* Check name of property */
        check_condition( 0 == strcmp( converted_name, property_names_read[ i ] ),
                         "Property name does not match: Expected %s. Got %s.",
                         converted_name, property_names_read[ i ] );
        free( converted_name );

        if ( i < number_of_string_properties )
        {
            /* Check property value (value is of type string) */
            status = OTF2_Reader_GetProperty( reader, property_names_read[ i ], &string_property_value_read );
            check_status( status, "Get property value." );
            check_condition( 0 == strcmp( string_property_values[ i ], string_property_value_read ),
                             "Property value does not match: Expected %s. Got %s.",
                             string_property_values[ i ], string_property_value_read );
            free( string_property_value_read );
        }
        else
        {
            /* Check property value (value is of type boolean) */
            status = OTF2_Reader_GetBoolProperty( reader,
                                                  property_names_read[ i ],
                                                  &bool_property_value_read );
            check_status( status, "Get boolean property value." );
            check_condition( bool_property_values[ i - number_of_string_properties ] == bool_property_value_read,
                             "Property value does not match: Expected %s. Got %s.",
                             ( bool_property_values[ i - number_of_string_properties ] ) ? "true" : "false",
                             ( bool_property_value_read ) ? "true" : "false" );
        }
    }
    free( property_names_read );

    /* Close reader. */
    status = OTF2_Reader_Close( reader );
    check_status( status, "Close reader." );

    return EXIT_SUCCESS;
}
