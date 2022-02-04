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
 *  @brief      This tool generates a basic OTF2 archive to test the reader
 *              components and tools.
 */

#include <config.h>

#include <otf2/otf2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>



/* ___ Global variables. ____________________________________________________ */



/** @internal
 *  @brief Defines the number of locations to generate. */
static uint64_t otf2_NUM_THREADS = 4;

/* ___ Prototypes for static functions. _____________________________________ */



static void
otf2_get_parameters( int    argc,
                     char** argv );

static void
check_pointer( void* pointer,
               char* description );

static void
check_status( OTF2_ErrorCode status,
              char*          description );

static OTF2_FlushType
pre_flush( void*         userData,
           OTF2_FileType fileType,
           uint64_t      locationId,
           void*         callerData,
           bool          final );

static OTF2_TimeStamp
post_flush( void*         userData,
            OTF2_FileType fileType,
            uint64_t      locationId );

static OTF2_FlushCallbacks flush_callbacks =
{
    .otf2_pre_flush  = pre_flush,
    .otf2_post_flush = post_flush
};

static uint64_t
get_time( void );

/* empty string definition */
enum
{
    STRING_EMPTY
};

/* definition IDs for regions */
enum
{
    REGION_MAIN,
    REGION_PTHREAD_CREATE,
    REGION_PTHREAD_JOIN,
    REGION_DGEMM
};


/* definition IDs for Groups */
enum
{
    GROUP_PTHREAD_LOCATIONS,
    GROUP_PTHREAD_POOL
};

/* definition IDs for comms */
enum
{
    COMM_PTHREAD_POOL
};

/* ___ main _________________________________________________________________ */



/** Main function
 *
 *  This tool generates a basic unified OTF2 archive to test the reader
 *  components and tools.
 *
 *  @return                 Returns EXIT_SUCCESS if successful, EXIT_FAILURE
 *                          if an error occures.
 */
int
main( int    argc,
      char** argv )
{
    otf2_get_parameters( argc, argv );

    char  archive_path[ 128 ];
    char* archive_name = "TestTrace";

    snprintf( archive_path, sizeof( archive_path ),
              "otf2_trace_gen_trace_pthread_%" PRIu64,
              otf2_NUM_THREADS );

    /* Print initial information. */
    printf( "\nGenerating an OTF2 test PTHREAD archive\n" );
    printf( "with %" PRIu64 " thread(s)\n",
            otf2_NUM_THREADS );
    printf( "to %s/%s.otf2\n\n", archive_path, archive_name );


    /* Create new archive handle. */
    OTF2_Archive* archive = OTF2_Archive_Open( archive_path,
                                               archive_name,
                                               OTF2_FILEMODE_WRITE,
                                               1024 * 1024,
                                               4 * 1024 * 1024,
                                               OTF2_SUBSTRATE_POSIX,
                                               OTF2_COMPRESSION_NONE );
    check_pointer( archive, "Create archive" );

    OTF2_ErrorCode status;
    status = OTF2_Archive_SetFlushCallbacks( archive, &flush_callbacks, NULL );
    check_status( status, "Set flush callbacks." );

    /* Set description, and creator. */
    status = OTF2_Archive_SetSerialCollectiveCallbacks( archive );
    check_status( status, "Set serial mode." );
    status = OTF2_Archive_SetDescription( archive, "Simple OTF2 trace archive to valid reader components" );
    check_status( status, "Set description." );
    status = OTF2_Archive_SetCreator( archive, "otf2_trace_gen" );
    check_status( status, "Set creator." );


    /* Generate location IDs. Just to have non-consecutive location IDs. */
    uint64_t threads[ otf2_NUM_THREADS ];

    for ( uint64_t thread = 0; thread < otf2_NUM_THREADS; thread++ )
    {
        threads[ thread ] = thread;
    }

    status = OTF2_Archive_OpenEvtFiles( archive );
    check_status( status, "Open event files for writing." );

    /* Create local event writer. */
    OTF2_EvtWriter* evt_writer = NULL;

    uint64_t t = get_time();
    /* Enter main. */
    evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
    check_pointer( evt_writer, "Get event writer." );
    status = OTF2_EvtWriter_Enter( evt_writer, NULL, t, REGION_MAIN );
    check_status( status, "Write Enter." );

    for ( uint32_t thread = 1; thread < otf2_NUM_THREADS; thread++ )
    {
        /* enter pthread_create. */
        t          = get_time();
        evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
        check_pointer( evt_writer, "Get event writer." );
        status = OTF2_EvtWriter_Enter( evt_writer, NULL, t, REGION_PTHREAD_CREATE );
        check_status( status, "Write Enter." );

        /* thread begin on thread. */
        t          = get_time();
        evt_writer = OTF2_Archive_GetEvtWriter( archive, thread );
        check_pointer( evt_writer, "Get event writer." );
        status = OTF2_EvtWriter_ThreadBegin( evt_writer, NULL, t, COMM_PTHREAD_POOL, thread );
        check_status( status, "Write ThreadBegin." );
        status = OTF2_EvtWriter_Enter( evt_writer, NULL, t, REGION_DGEMM );
        check_status( status, "Write Enter." );

        /* leave pthread_create */
        t          = get_time();
        evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
        check_pointer( evt_writer, "Get event writer." );
        status = OTF2_EvtWriter_ThreadCreate( evt_writer, NULL, t, COMM_PTHREAD_POOL, thread );
        check_status( status, "Write ThreadCreate." );
        status = OTF2_EvtWriter_Leave( evt_writer, NULL, t, REGION_PTHREAD_CREATE );
        check_status( status, "Write Leave." );
    }

    /* dgemm on master. */
    t          = get_time();
    evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
    check_pointer( evt_writer, "Get event writer." );
    status = OTF2_EvtWriter_Enter( evt_writer, NULL, t, REGION_DGEMM );
    check_status( status, "Write Enter." );
    t      = get_time();
    status = OTF2_EvtWriter_Leave( evt_writer, NULL, t, REGION_DGEMM );
    check_status( status, "Write Leave." );

    for ( uint32_t thread = 1; thread < otf2_NUM_THREADS; thread++ )
    {
        /* enter pthread_join on master. */
        t          = get_time();
        evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
        check_pointer( evt_writer, "Get event writer." );
        status = OTF2_EvtWriter_Enter( evt_writer, NULL, t, REGION_PTHREAD_JOIN );
        check_status( status, "Write Enter." );

        /* leave dgemm on thread. */
        t          = get_time();
        evt_writer = OTF2_Archive_GetEvtWriter( archive, thread );
        check_pointer( evt_writer, "Get event writer." );
        status = OTF2_EvtWriter_Leave( evt_writer, NULL, t, REGION_DGEMM );
        check_status( status, "Write Leave." );
        status = OTF2_EvtWriter_ThreadEnd( evt_writer, NULL, t, COMM_PTHREAD_POOL, thread + otf2_NUM_THREADS );
        check_status( status, "Write ThreadEnd." );

        t          = get_time();
        evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
        check_pointer( evt_writer, "Get event writer." );
        status = OTF2_EvtWriter_ThreadWait( evt_writer, NULL, t, COMM_PTHREAD_POOL, thread + otf2_NUM_THREADS );
        check_status( status, "Write ThreadWait." );
        status = OTF2_EvtWriter_Leave( evt_writer, NULL, t, REGION_PTHREAD_JOIN );
        check_status( status, "Write Leave." );
    }

    /* leave main on master. */
    t          = get_time();
    evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
    check_pointer( evt_writer, "Get event writer." );
    status = OTF2_EvtWriter_Leave( evt_writer, NULL, t, REGION_MAIN );
    check_status( status, "Write Leave." );

    /* last timestamp */
    t = get_time();

    uint64_t number_of_events_per_location[ otf2_NUM_THREADS ];
    for ( uint64_t thread = 0; thread < otf2_NUM_THREADS; ++thread )
    {
        evt_writer = OTF2_Archive_GetEvtWriter( archive, thread );
        check_pointer( evt_writer, "Get event writer." );

        status = OTF2_EvtWriter_GetNumberOfEvents( evt_writer,
                                                   &number_of_events_per_location[ thread ] );
        check_status( status, "Get the numberof written events." );

        status = OTF2_Archive_CloseEvtWriter( archive, evt_writer );
        check_status( status, "Close event writer." );
    }
    status = OTF2_Archive_CloseEvtFiles( archive );
    check_status( status, "Close event files for reading." );

    /* Write the global and local definitions. */
    OTF2_GlobalDefWriter* glob_def_writer = NULL;
    glob_def_writer = OTF2_Archive_GetGlobalDefWriter( archive );
    check_pointer( glob_def_writer, "Get global definition writer" );

    /* Write the timerange of the trace */
    status = OTF2_GlobalDefWriter_WriteClockProperties( glob_def_writer,
                                                        1, 0, t );
    check_status( status, "Write time range definition." );

    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               STRING_EMPTY,
                                               "" );
    check_status( status, "Write string definition." );
    uint32_t string = STRING_EMPTY + 1;

    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer, string, "Pthread" );
    check_status( status, "Write string definition." );

    /* Write paradigm definitions */
    status = OTF2_GlobalDefWriter_WriteParadigm( glob_def_writer,
                                                 OTF2_PARADIGM_PTHREAD,
                                                 string,
                                                 OTF2_PARADIGM_CLASS_THREAD_CREATE_WAIT );
    check_status( status, "Write paradigm definition." );
    string++;

    /* Write system tree definitions. */
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer, string, "machine" );
    check_status( status, "Write string definition." );
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer, string + 1, "fake" );
    check_status( status, "Write string definition." );

    status = OTF2_GlobalDefWriter_WriteSystemTreeNode( glob_def_writer, 0,
                                                       string + 1,
                                                       string,
                                                       OTF2_UNDEFINED_SYSTEM_TREE_NODE );
    check_status( status, "Write system tree node definition." );
    string += 2;

    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "Process" );
    check_status( status, "Write string definition." );
    status = OTF2_GlobalDefWriter_WriteLocationGroup( glob_def_writer,
                                                      0,
                                                      string,
                                                      OTF2_LOCATION_GROUP_TYPE_PROCESS,
                                                      0 );
    string++;

    /* Write location group and location definitions. */
    for ( uint64_t thread = 0; thread < otf2_NUM_THREADS; ++thread )
    {
        char name_buffer[ 64 ];

        sprintf( name_buffer, "Thread %" PRIu64, thread );
        status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                                   string,
                                                   name_buffer );
        check_status( status, "Write string definition." );

        status = OTF2_GlobalDefWriter_WriteLocation( glob_def_writer,
                                                     thread,
                                                     string,
                                                     OTF2_LOCATION_TYPE_CPU_THREAD,
                                                     number_of_events_per_location[ thread ],
                                                     0 );
        check_status( status, "Write location definition." );
        string++;
    }

    /* Write region definition for "main". */
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "main (demangled name)" );
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string + 1,
                                               "main (mangled name)" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteRegion( glob_def_writer,
                                               REGION_MAIN,
                                               string,
                                               string + 1,
                                               STRING_EMPTY,
                                               OTF2_REGION_ROLE_FUNCTION,
                                               OTF2_PARADIGM_COMPILER,
                                               OTF2_REGION_FLAG_NONE,
                                               STRING_EMPTY,
                                               0,
                                               0 );
    check_status( status, "Write region definition" );
    string += 2;

    /* Write region definition for "pthrad_create". */
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "pthread_create" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteRegion( glob_def_writer,
                                               REGION_PTHREAD_CREATE,
                                               string,
                                               string,
                                               STRING_EMPTY,
                                               OTF2_REGION_ROLE_THREAD_CREATE,
                                               OTF2_PARADIGM_PTHREAD,
                                               OTF2_REGION_FLAG_NONE,
                                               STRING_EMPTY,
                                               0,
                                               0 );
    check_status( status, "Write region definition" );
    string++;

    /* Write region definition for "pthrad_join". */
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "pthread_join" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteRegion( glob_def_writer,
                                               REGION_PTHREAD_JOIN,
                                               string,
                                               string,
                                               STRING_EMPTY,
                                               OTF2_REGION_ROLE_THREAD_WAIT,
                                               OTF2_PARADIGM_PTHREAD,
                                               OTF2_REGION_FLAG_NONE,
                                               STRING_EMPTY,
                                               0,
                                               0 );
    check_status( status, "Write region definition" );
    string++;

    /* Write region definition for "dgemm". */
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "dgemm" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteRegion( glob_def_writer,
                                               REGION_DGEMM,
                                               string,
                                               string,
                                               STRING_EMPTY,
                                               OTF2_REGION_ROLE_FUNCTION,
                                               OTF2_PARADIGM_COMPILER,
                                               OTF2_REGION_FLAG_NONE,
                                               STRING_EMPTY,
                                               0,
                                               0 );
    check_status( status, "Write region definition" );
    string++;

    status = OTF2_GlobalDefWriter_WriteGroup( glob_def_writer,
                                              GROUP_PTHREAD_LOCATIONS,
                                              STRING_EMPTY,
                                              OTF2_GROUP_TYPE_COMM_LOCATIONS,
                                              OTF2_PARADIGM_PTHREAD,
                                              OTF2_GROUP_FLAG_NONE,
                                              otf2_NUM_THREADS,
                                              threads );
    check_status( status, "Write Group definition." );

    status = OTF2_GlobalDefWriter_WriteGroup( glob_def_writer,
                                              GROUP_PTHREAD_POOL,
                                              STRING_EMPTY,
                                              OTF2_GROUP_TYPE_COMM_GROUP,
                                              OTF2_PARADIGM_PTHREAD,
                                              OTF2_GROUP_FLAG_NONE,
                                              otf2_NUM_THREADS,
                                              threads );
    check_status( status, "Write Group definition." );

    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "POSIX Threads pool" );
    status = OTF2_GlobalDefWriter_WriteComm( glob_def_writer,
                                             COMM_PTHREAD_POOL,
                                             string,
                                             GROUP_PTHREAD_POOL,
                                             OTF2_UNDEFINED_COMM );
    check_status( status, "Write Comm definition." );
    string++;

    status = OTF2_Archive_CloseGlobalDefWriter( archive, glob_def_writer );
    check_status( status, "Closing global definition writer." );

    status = OTF2_Archive_OpenDefFiles( archive );
    check_status( status, "Open local definition files for writing." );

    for ( uint64_t thread = 0; thread < otf2_NUM_THREADS; thread++ )
    {
        /* Open a definition writer, so the file is created. */
        OTF2_DefWriter* def_writer = OTF2_Archive_GetDefWriter( archive, thread );
        check_pointer( def_writer, "Get definition writer." );

        status = OTF2_Archive_CloseDefWriter( archive, def_writer );
        check_status( status, "Close local definition writer." );
    }

    status = OTF2_Archive_CloseDefFiles( archive );
    check_status( status, "Close local definition files for writing." );

    /* Finalize. */
    status = OTF2_Archive_Close( archive );
    check_status( status, "Close archive." );


    printf( "Done\n\n" );


    return EXIT_SUCCESS;
}



/* ___ Implementation of static functions ___________________________________ */


/** @internal
 *  @brief Parse a number from the command line.
 */
uint64_t
otf2_parse_number_argument( const char* option,
                            const char* argument )
{
    uint64_t number = 0;
    for ( uint8_t j = 0; argument[ j ]; j++ )
    {
        if ( ( argument[ j ] < '0' ) || ( argument[ j ] > '9' ) )
        {
            printf( "ERROR: Invalid number argument for %s: %s\n", option, argument );
            printf( "Try 'otf2-trace-gen --help' for information on usage.\n\n" );
            exit( EXIT_FAILURE );
        }
        uint64_t new_number = number * 10 + argument[ j ] - '0';
        if ( new_number < number )
        {
            printf( "ERROR: Number argument to large for %s: %s\n", option, argument );
            printf( "Try 'otf2-trace-gen --help' for information on usage.\n\n" );
            exit( EXIT_FAILURE );
        }
        number = new_number;
    }

    return number;
}

/** @internal
 *  @brief Get command line parameters.
 *
 *  Parses command line parameters and checks for their existence.
 *  Prints help for parameters '-h' or '--help'.
 *
 *  @param argc             Programs argument counter.
 *  @param argv             Programs argument values.
 */
void
otf2_get_parameters( int    argc,
                     char** argv )
{
    /* Check if there is at least one command line parameter. */
    if ( argc < 2 )
    {
        return;
    }

    /* Check for passed '--help' or '-h' and print help. */
    if ( !strcmp( argv[ 1 ], "--help" ) || !strcmp( argv[ 1 ], "-h" ) )
    {
        printf( "Usage: otf2-trace-gen-pthread [OPTION]... \n" );
        printf( "Generate a basic unified OTF2 archive.\n" );
        printf( "\n" );
        printf( "  -nt, --threads        Number of locations to generate (Should be small).\n" );
        printf( "  -V, --version          Print version information.\n" );
        printf( "  -h, --help             Print this help information.\n" );
        printf( "\n" );
        printf( "Report bugs to <%s>\n", PACKAGE_BUGREPORT );
        exit( EXIT_SUCCESS );
    }

    /* Check for passed '--version' or '-V' and print version information. */
    if ( !strcmp( argv[ 1 ], "--version" ) || !strcmp( argv[ 1 ], "-V" ) )
    {
        printf( "otf2-trace-gen: version %s\n", OTF2_VERSION );
        exit( EXIT_SUCCESS );
    }

    for ( uint8_t i = 1; i < argc; i++ )
    {
        if ( !strcmp( argv[ i ], "--threads" ) || !strcmp( argv[ i ], "-nt" ) )
        {
            if ( i + 1 == argc )
            {
                printf( "ERROR: Missing argument for %s.\n", argv[ i ] );
                printf( "Try 'otf2-trace-gen --help' for information on usage.\n\n" );
                exit( EXIT_FAILURE );
            }

            otf2_NUM_THREADS = otf2_parse_number_argument( argv[ i ], argv[ i + 1 ] );
            i++;
        }

        else
        {
            printf( "WARNING: Skipped unknown control option %s.\n", argv[ i ] );
        }
    }

    return;
}


void
check_pointer( void* pointer,
               char* description )
{
    if ( pointer == NULL )
    {
        printf( "\nERROR: %s\n\n", description );
        exit( EXIT_FAILURE );
    }
}



void
check_status( OTF2_ErrorCode status,
              char*          description )
{
    if ( status != OTF2_SUCCESS )
    {
        printf( "\nERROR: %s\n\n", description );
        exit( EXIT_FAILURE );
    }
}

static OTF2_FlushType
pre_flush( void*         userData,
           OTF2_FileType fileType,
           uint64_t      locationId,
           void*         callerData,
           bool          final )
{
    return OTF2_FLUSH;
}

static OTF2_TimeStamp
post_flush( void*         userData,
            OTF2_FileType fileType,
            uint64_t      locationId )
{
    return get_time();
}

static uint64_t
get_time( void )
{
    static uint64_t sequence;
    return sequence++;
}
