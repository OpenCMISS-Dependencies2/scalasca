/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
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
 *  @brief      This tool generates a basic CallingContext OTF2 archive.
 */

#include <config.h>

#include <otf2/otf2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>



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

/*
    the source:

     1	void bar( void )
     2	{
     3	    @fma
     4	}
     5
     6	void foo( void )
     7	{
     8	    @fma
     9	    @call bar()
    10	    @fma
    11	}
    12
    13	void baz( void )
    14	{
    15	    @fma
    16	    @call foo()
    17	    @fma
    18	}
    19
    20	int main( void )
    21	{
    22	    @fma
    23	    @call foo()
    24	    @fma
    25	    @call baz()
    26	    @fma
    27	}
 */

/* empty string definition */
enum
{
    STRING_EMPTY
};

/* attribute definitions */
enum
{
    ATTRIBUTE_ENTER,
    ATTRIBUTE_LEAVE,
    ATTRIBUTE_SAMPLE
};

/* definition IDs for regions */
enum
{
    REGION_MAIN,
    REGION_FOO,
    REGION_BAR,
    REGION_BAZ  /* instrumented */
};


/* region begin/end line numbers */
enum
{
    LNO_MAIN_BEGIN = 21, LNO_MAIN_END = 27,
    LNO_FOO_BEGIN  = 7, LNO_FOO_END = 11,
    LNO_BAR_BEGIN  = 2, LNO_BAR_END = 4,
    LNO_BAZ_BEGIN  = 14, LNO_BAZ_END = 18
};

/* scls, numbers in symbol are the offsets to the start of funciton */
enum
{
    // main
    SCL_MAIN_1, SCL_MAIN_2, SCL_MAIN_3, SCL_MAIN_4, SCL_MAIN_5,
    // foo
    SCL_FOO_1, SCL_FOO_2, SCL_FOO_3,
    // bar
    SCL_BAR_1,
    // baz
    SCL_BAZ_0, SCL_BAZ_1, SCL_BAZ_2, SCL_BAZ_3, SCL_BAZ_4
};

/*
 * definition IDs for the CCT
 * MAIN -> FOO -> BAR
 * MAIN -> BAZ -> FOO -> BAR
 */
enum
{
    CCT_MAIN0,
    CCT_MAINC_FOO,
    CCT_MAINC_FOO0,
    CCT_MAINC_FOOC_BAR,
    CCT_MAINC_FOOC_BAR0,
    CCT_MAINC_FOO1,
    CCT_MAIN1,
    CCT_MAINC_BAZ,
    CCT_MAINC_BAZE,
    CCT_MAINC_BAZ0,
    CCT_MAINC_BAZC_FOO,
    CCT_MAINC_BAZC_FOO0,
    CCT_MAINC_BAZC_FOOC_BAR,
    CCT_MAINC_BAZC_FOOC_BAR0,
    CCT_MAINC_BAZC_FOO1,
    CCT_MAINC_BAZ1,
    CCT_MAINC_BAZL,
    CCT_MAIN2
};

/* interrupt generator */
enum
{
    DUMMY_INTERRRUPT_GENERATOR
};

/* ___ main _________________________________________________________________ */


/** Use trampolines to calculate unwind distance. */
static bool otf2_TRAMPOLINES;

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
              "otf2_trace_gen_trace_cct%s",
              otf2_TRAMPOLINES ? "_trampolines" : "" );

    /* Print initial information. */
    printf( "\nGenerating an OTF2 test CCT archive\n" );
    printf( "to %s/%s.otf2\n\n", archive_path, archive_name );

    OTF2_AttributeValue attribute_value;
    uint32_t            attribute_enter_value  = 0;
    uint32_t            attribute_leave_value  = 0;
    uint32_t            attribute_sample_value = 0;
    OTF2_AttributeList* attributes             = OTF2_AttributeList_New();
    check_pointer( attributes, "Create attribute list." );

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
    status = OTF2_Archive_SetCreator( archive, "otf2-trace-gen-cct" );
    check_status( status, "Set creator." );

    status = OTF2_Archive_OpenEvtFiles( archive );
    check_status( status, "Open event files for writing." );

    uint64_t t;

    /* Create local event writer. */
    OTF2_EvtWriter* evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
    check_pointer( evt_writer, "Get event writer." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAIN0,
                                                  OTF2_UNDEFINED_UINT32,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAINC_FOO0,
                                                  otf2_TRAMPOLINES ? 2 : OTF2_UNDEFINED_UINT32,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAINC_FOOC_BAR0,
                                                  otf2_TRAMPOLINES ? 2 : OTF2_UNDEFINED_UINT32,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAINC_FOO1,
                                                  otf2_TRAMPOLINES ? 1 : OTF2_UNDEFINED_UINT32,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAIN1,
                                                  otf2_TRAMPOLINES ? 1 : OTF2_UNDEFINED_UINT32,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_enter_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_ENTER,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextEnter( evt_writer, attributes,
                                                 get_time(),
                                                 CCT_MAINC_BAZE,
                                                 2 );
    check_status( status, "Write calling context enter." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAINC_BAZ0,
                                                  otf2_TRAMPOLINES ? 1 : 1,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAINC_BAZC_FOO0,
                                                  otf2_TRAMPOLINES ? 2 : 2,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAINC_BAZC_FOOC_BAR0,
                                                  otf2_TRAMPOLINES ? 2 : 3,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAINC_BAZC_FOO1,
                                                  otf2_TRAMPOLINES ? 1 : 2,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAINC_BAZ1,
                                                  otf2_TRAMPOLINES ? 1 : 1,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    attribute_value.uint32 = attribute_leave_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_LEAVE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextLeave( evt_writer, attributes,
                                                 get_time(),
                                                 CCT_MAINC_BAZL );
    check_status( status, "Write calling context leave." );

    attribute_value.uint32 = attribute_sample_value++;
    status                 = OTF2_AttributeList_AddAttribute( attributes,
                                                              ATTRIBUTE_SAMPLE,
                                                              OTF2_TYPE_UINT32,
                                                              attribute_value );
    check_status( status, "Adding attribute" );
    status = OTF2_EvtWriter_CallingContextSample( evt_writer, attributes,
                                                  get_time(),
                                                  CCT_MAIN2,
                                                  otf2_TRAMPOLINES ? 1 : OTF2_UNDEFINED_UINT32,
                                                  DUMMY_INTERRRUPT_GENERATOR );
    check_status( status, "Write calling context sampling." );

    /* last timestamp */
    t = get_time();

    uint64_t number_of_events;
    status = OTF2_EvtWriter_GetNumberOfEvents( evt_writer,
                                               &number_of_events );
    check_status( status, "Get the numberof written events." );

    status = OTF2_Archive_CloseEvtFiles( archive );
    check_status( status, "Close event files for reading." );

    /* Write the global definitions. */
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
    OTF2_StringRef string = STRING_EMPTY + 1;

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

    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "Main Thread" );
    check_status( status, "Write string definition." );

    status = OTF2_GlobalDefWriter_WriteLocation( glob_def_writer,
                                                 0,
                                                 string,
                                                 OTF2_LOCATION_TYPE_CPU_THREAD,
                                                 number_of_events,
                                                 0 );
    check_status( status, "Write location definition." );
    string++;

    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "enter attribute" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteAttribute( glob_def_writer,
                                                  ATTRIBUTE_ENTER,
                                                  string,
                                                  STRING_EMPTY,
                                                  OTF2_TYPE_UINT32 );
    check_status( status, " Write attribute definition." );
    string++;

    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "leave attribute" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteAttribute( glob_def_writer,
                                                  ATTRIBUTE_LEAVE,
                                                  string,
                                                  STRING_EMPTY,
                                                  OTF2_TYPE_UINT32 );
    check_status( status, "Write attribute definition." );
    string++;

    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "sample attribute" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteAttribute( glob_def_writer,
                                                  ATTRIBUTE_SAMPLE,
                                                  string,
                                                  STRING_EMPTY,
                                                  OTF2_TYPE_UINT32 );
    check_status( status, "Write attribute definition." );
    string++;

    OTF2_StringRef source_file = string++;
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               source_file,
                                               "test.c" );

    /* Write region definition for "main". */
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "main" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteRegion( glob_def_writer,
                                               REGION_MAIN,
                                               string,
                                               string,
                                               STRING_EMPTY,
                                               OTF2_REGION_ROLE_FUNCTION,
                                               OTF2_PARADIGM_SAMPLING,
                                               OTF2_REGION_FLAG_NONE,
                                               source_file,
                                               LNO_MAIN_BEGIN,
                                               LNO_MAIN_END );
    check_status( status, "Write region definition" );
    string++;

    /* Write region definition for "foo". */
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "foo" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteRegion( glob_def_writer,
                                               REGION_FOO,
                                               string,
                                               string,
                                               STRING_EMPTY,
                                               OTF2_REGION_ROLE_FUNCTION,
                                               OTF2_PARADIGM_SAMPLING,
                                               OTF2_REGION_FLAG_NONE,
                                               source_file,
                                               LNO_FOO_BEGIN,
                                               LNO_FOO_END );
    check_status( status, "Write region definition" );
    string++;

    /* Write region definition for "bar". */
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "bar" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteRegion( glob_def_writer,
                                               REGION_BAR,
                                               string,
                                               string,
                                               STRING_EMPTY,
                                               OTF2_REGION_ROLE_FUNCTION,
                                               OTF2_PARADIGM_SAMPLING,
                                               OTF2_REGION_FLAG_NONE,
                                               source_file,
                                               LNO_BAR_BEGIN,
                                               LNO_BAR_END );
    check_status( status, "Write region definition" );
    string++;

    /* Write region definition for "baz". */
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "baz" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteRegion( glob_def_writer,
                                               REGION_BAZ,
                                               string,
                                               string,
                                               STRING_EMPTY,
                                               OTF2_REGION_ROLE_FUNCTION,
                                               OTF2_PARADIGM_USER,
                                               OTF2_REGION_FLAG_NONE,
                                               source_file,
                                               LNO_BAZ_BEGIN,
                                               LNO_BAZ_END );
    check_status( status, "Write region definition" );
    string++;

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_MAIN_1,
                                                           source_file,
                                                           LNO_MAIN_BEGIN + 1 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_MAIN_2,
                                                           source_file,
                                                           LNO_MAIN_BEGIN + 2 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_MAIN_3,
                                                           source_file,
                                                           LNO_MAIN_BEGIN + 3 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_MAIN_4,
                                                           source_file,
                                                           LNO_MAIN_BEGIN + 4 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_MAIN_5,
                                                           source_file,
                                                           LNO_MAIN_BEGIN + 5 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_FOO_1,
                                                           source_file,
                                                           LNO_FOO_BEGIN + 1 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_FOO_2,
                                                           source_file,
                                                           LNO_FOO_BEGIN + 2 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_FOO_3,
                                                           source_file,
                                                           LNO_FOO_BEGIN + 3 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_BAR_1,
                                                           source_file,
                                                           LNO_BAR_BEGIN + 1 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_BAZ_0,
                                                           source_file,
                                                           LNO_BAZ_BEGIN + 0 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_BAZ_1,
                                                           source_file,
                                                           LNO_BAZ_BEGIN + 1 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_BAZ_2,
                                                           source_file,
                                                           LNO_BAZ_BEGIN + 2 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_BAZ_3,
                                                           source_file,
                                                           LNO_BAZ_BEGIN + 3 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteSourceCodeLocation( glob_def_writer,
                                                           SCL_BAZ_4,
                                                           source_file,
                                                           LNO_BAZ_BEGIN + 4 );
    check_status( status, "Write interrupt generator definition" );

    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               string,
                                               "dummy" );
    check_status( status, "Write string definition" );
    status = OTF2_GlobalDefWriter_WriteInterruptGenerator( glob_def_writer,
                                                           DUMMY_INTERRRUPT_GENERATOR,
                                                           string,
                                                           OTF2_INTERRUPT_GENERATOR_MODE_COUNT,
                                                           OTF2_BASE_DECIMAL,
                                                           0,
                                                           1 );
    check_status( status, "Write interrupt generator definition" );
    string++;

    OTF2_StringRef property_inst = string++;
    status = OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                               property_inst,
                                               "Instr." );
    check_status( status, "Write string definition" );
    OTF2_AttributeValue property_inst_fma_value;
    property_inst_fma_value.stringRef = string++;
    status                            =
        OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                          property_inst_fma_value.stringRef,
                                          "fma" );
    check_status( status, "Write string definition" );
    #define property_inst_fma OTF2_TYPE_STRING, property_inst_fma_value

    OTF2_AttributeValue property_inst_call_value;
    property_inst_call_value.stringRef = string++;
    status                             =
        OTF2_GlobalDefWriter_WriteString( glob_def_writer,
                                          property_inst_call_value.stringRef,
                                          "call" );
    check_status( status, "Write string definition" );
    #define property_inst_call OTF2_TYPE_STRING, property_inst_call_value

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAIN0,
                                                       REGION_MAIN,
                                                       SCL_MAIN_1,
                                                       OTF2_UNDEFINED_CALLING_CONTEXT );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAIN0,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_FOO,
                                                       REGION_MAIN,
                                                       SCL_MAIN_2,
                                                       OTF2_UNDEFINED_CALLING_CONTEXT );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_FOO,
                                                               property_inst,
                                                               property_inst_call );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_FOO0,
                                                       REGION_FOO,
                                                       SCL_FOO_1,
                                                       CCT_MAINC_FOO );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_FOO0,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_FOOC_BAR,
                                                       REGION_FOO,
                                                       SCL_FOO_2,
                                                       CCT_MAINC_FOO );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_FOOC_BAR,
                                                               property_inst,
                                                               property_inst_call );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_FOOC_BAR0,
                                                       REGION_BAR,
                                                       SCL_BAR_1,
                                                       CCT_MAINC_FOOC_BAR );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_FOOC_BAR0,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_FOO1,
                                                       REGION_FOO,
                                                       SCL_FOO_3,
                                                       CCT_MAINC_FOO );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_FOO1,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAIN1,
                                                       REGION_MAIN,
                                                       SCL_MAIN_3,
                                                       OTF2_UNDEFINED_CALLING_CONTEXT );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAIN1,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZ,
                                                       REGION_MAIN,
                                                       SCL_MAIN_4,
                                                       OTF2_UNDEFINED_CALLING_CONTEXT );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_BAZ,
                                                               property_inst,
                                                               property_inst_call );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZE,
                                                       REGION_BAZ,
                                                       SCL_BAZ_0,
                                                       CCT_MAINC_BAZ );
    check_status( status, "Write calling context definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZ0,
                                                       REGION_BAZ,
                                                       SCL_BAZ_1,
                                                       CCT_MAINC_BAZ );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_BAZ0,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZC_FOO,
                                                       REGION_BAZ,
                                                       SCL_BAZ_2,
                                                       CCT_MAINC_BAZ );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_BAZC_FOO,
                                                               property_inst,
                                                               property_inst_call );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZC_FOO0,
                                                       REGION_FOO,
                                                       SCL_FOO_1,
                                                       CCT_MAINC_BAZC_FOO );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_BAZC_FOO0,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZC_FOOC_BAR,
                                                       REGION_FOO,
                                                       SCL_FOO_2,
                                                       CCT_MAINC_BAZC_FOO );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_BAZC_FOOC_BAR,
                                                               property_inst,
                                                               property_inst_call );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZC_FOOC_BAR0,
                                                       REGION_BAR,
                                                       SCL_BAR_1,
                                                       CCT_MAINC_BAZC_FOOC_BAR );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_BAZC_FOOC_BAR0,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZC_FOO1,
                                                       REGION_FOO,
                                                       SCL_FOO_3,
                                                       CCT_MAINC_BAZC_FOO );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_BAZC_FOO1,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZ1,
                                                       REGION_BAZ,
                                                       SCL_BAZ_3,
                                                       CCT_MAINC_BAZ );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAINC_BAZ1,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAINC_BAZL,
                                                       REGION_BAZ,
                                                       SCL_BAZ_4,
                                                       CCT_MAINC_BAZ );
    check_status( status, "Write calling context definition" );

    status = OTF2_GlobalDefWriter_WriteCallingContext( glob_def_writer,
                                                       CCT_MAIN2,
                                                       REGION_MAIN,
                                                       SCL_MAIN_5,
                                                       OTF2_UNDEFINED_CALLING_CONTEXT );
    check_status( status, "Write calling context definition" );
    status = OTF2_GlobalDefWriter_WriteCallingContextProperty( glob_def_writer,
                                                               CCT_MAIN2,
                                                               property_inst,
                                                               property_inst_fma );
    check_status( status, "Write calling context property definition" );

    status = OTF2_Archive_CloseGlobalDefWriter( archive, glob_def_writer );
    check_status( status, "Closing global definition writer." );

    status = OTF2_Archive_OpenDefFiles( archive );
    check_status( status, "Open local definition files for writing." );

    /* Open a definition writer, so the file is created. */
    OTF2_DefWriter* def_writer = OTF2_Archive_GetDefWriter( archive, 0 );
    check_pointer( def_writer, "Get definition writer." );

    status = OTF2_Archive_CloseDefWriter( archive, def_writer );
    check_status( status, "Close local definition writer." );

    status = OTF2_Archive_CloseDefFiles( archive );
    check_status( status, "Close local definition files for writing." );

    /* Finalize. */
    status = OTF2_Archive_Close( archive );
    check_status( status, "Close archive." );

    OTF2_AttributeList_Delete( attributes );

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
        printf( "Usage: otf2-trace-gen-cct [OPTION]... \n" );
        printf( "Generate a basic unified OTF2 archive.\n" );
        printf( "\n" );
        printf( "  -t, --trampolines      Calculate unwind distance with the help of trampolines.\n" );
        printf( "  -V, --version          Print version information.\n" );
        printf( "  -h, --help             Print this help information.\n" );
        printf( "\n" );
        printf( "Report bugs to <%s>\n", PACKAGE_BUGREPORT );
        exit( EXIT_SUCCESS );
    }

    /* Check for passed '--version' or '-V' and print version information. */
    if ( !strcmp( argv[ 1 ], "--version" ) || !strcmp( argv[ 1 ], "-V" ) )
    {
        printf( "otf2-trace-gen-cct: version %s\n", OTF2_VERSION );
        exit( EXIT_SUCCESS );
    }

    for ( uint8_t i = 1; i < argc; i++ )
    {
        if ( !strcmp( argv[ i ], "--trampolines" ) )
        {
            otf2_TRAMPOLINES = true;
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
