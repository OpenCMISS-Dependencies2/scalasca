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


/**
 *  @file
 *
 *  @brief      MPI collectives for OTF2.
 *
 *  See @ref usage_reading_mpi and @ref usage_writing_mpi for instruction
 *  how to use.
 *
 *  @addtogroup usage OTF2 usage examples
 *  @{
 *
 *  @defgroup usage_reading_mpi Usage in reading mode - MPI example
 *  @{
 *
 *  This is a example of how to use the OTF2 reading interface with MPI. It
 *  shows how to define and register callbacks and how to use the provided
 *  MPI collective callbacks to read all events of a given OTF2 archive in
 *  parallel. This example is available as source code in the file
 *  @refexample{otf2_mpi_reader_example,c}.
 *  @dontinclude otf2_mpi_reader_example.c
 *
 *  We start with inclusion of some standard headers.
 *  @skipline stdlib.h
 *  @until inttypes.h
 *
 *  And than include the MPI and OTF2 header.
 *  @line mpi.h
 *  @until otf2/otf2.h
 *
 *  Now prepare the inclusion of the <otf2/OTF2_MPI_Collectives.h> header. As
 *  it is a header-only interface, it needs some information about the used
 *  MPI environment. In particular the MPI datatypes which match the C99 types
 *  @c uint64_t and @c int64_t. In case you have a MPI 3.0 conforming MPI
 *  implementation you can skip this. If not, provide \#define's for the
 *  following macros prior the \#include statement. In this example, we assume
 *  a LP64 platform.
 *  @line MPI_VERSION
 *  @until endif
 *
 *  After this preparatory step, we can include the <otf2/OTF2_MPI_Collectives.h>
 *  header.
 *  @skipline otf2/OTF2_MPI_Collectives.h
 *
 *  The following section until describing @c main is the same as in the
 *  @ref usage_reading.
 *
 *  Define an event callback for entering and leaving a region.
 *  @line static OTF2_CallbackCode
 *  @until }
 *  @until }
 *
 *  The global definition file provides all location IDs that are included in
 *  the OTF2 trace archive. When reading the global definitions these location
 *  IDs must be collected and stored by the user. Probably, the easiest way to
 *  do that is to use a C++ container.
 *  @line vector
 *  @until }
 *  @until }
 *  @until }
 *
 *  Now everything is prepared to begin with the main program.
 *  @line int
 *  @until {
 *
 *  First initialize the MPI environment and query the size and rank.
 *  @line MPI_Init
 *  @until MPI_Comm_rank
 *
 *  Create a new reader handle. The path to the OTF2 anchor file must be
 *  provided as argument.
 *  @skipline OTF2_Reader_Open
 *
 *  Now we provide the OTF2 reader object the MPI collectives.
 *  @skipline SetCollectiveCallbacks
 *
 *  OTF2 provides an API to query the number of locations prior reading the
 *  global definitions. We use this to pre-allocate the storage for all
 *  locations.
 *  @line number_of_locations
 *  @until 0
 *
 *  All ranks need to read the global definitions to know the list of locations
 *  in the trace. Get a global definition reader with the above reader handle
 *  as argument.
 *  @skipline global_def_reader
 *
 *  Register the above defined global definition callbacks. All other definition
 *  callbacks will be deactivated. And instruct the reader to pass the
 *  @a locations object to each call of the callbacks.
 *  @line global_def_callbacks
 *  @until Delete
 *
 *  Read all global definitions. Every time a location definition is read, the
 *  previously registered callback is triggered.
 *  In @c definitions_read the number of read definitions is returned.
 *  @line definitions_read
 *  @until );
 *
 *  After reading all global definitions all location IDs are stored in the
 *  generic container @c ListOfLocations. After that, the locations that are
 *  supposed to be read are selected. We distribute the locations round-robin
 *  to all ranks in @c MPI_COMM_WORLD. We need also to remember, whether this
 *  rank actually reads any locations.
 *  @line number_of_locations_to_read
 *  @until }
 *  @until }
 *
 *  When the locations are selected the according event and definition files can
 *  be opened. Note that the local definition files are optional, thus we need
 *  to remember the success of this call.
 *  @line successful_open_def_files
 *  @until OpenEvtFiles
 *
 *  When the files are opened the event and definition reader handle can be
 *  requested. We distribute the locations round-robin to all ranks in
 *  @c MPI_COMM_WORLD.
 *  To apply mapping tables stored in the local definitions, the local
 *  definitions must be read. Though the existence of these files are optional.
 *  The call to @eref{OTF2_Reader_GetEvtReader} is mandatory, but the result
 *  is unused.
 *  @line for
 *  @until }
 *  @until }
 *  @until }
 *  @until }
 *
 *  The definition files can now be closed, if it was successfully opened in the
 *  first place.
 *  @line successful_open_def_files
 *  @until }
 *
 *  Only these ranks which actually read events for locations, can now open a
 *  new global event reader. This global reader automatically contains
 *  all previously opened local event readers.
 *  @line number_of_locations_to_read
 *  @until global_evt_reader
 *
 *  Register the above defined global event callbacks. All other global event
 *  callbacks will be deactivated.
 *  @line event_callbacks
 *  @until Delete
 *
 *  Read all events in the OTF2 archive. The events are automatically ordered by
 *  the time they occurred in the trace. Every time an enter or leave event is
 *  read, the previously registered callbacks are triggered.
 *  In @c events_read the number of read events is returned.
 *  @line events_read
 *  @until );
 *
 *  The global event reader can now be closed and the event files too.
 *  @skipline CloseGlobalEvtReader
 *
 *  As the call to @eref{OTF2_Reader_CloseEvtFiles} is a collective operation
 *  all ranks need to call this, not only those which read events.
 *  @line }
 *  @until CloseEvtFiles
 *
 *  At the end, close the reader and exit. All opened event and definition
 *  readers are closed automatically. Free resources and finalize the MPI
 *  environment.
 *  @line Close
 *  @until }
 *
 *  To compile your program use a command like the following. Note that we need
 *  to activate the C99 standard explicitly for GCC.
 *  @dontinclude Makefile
 *  @skip otf2_mpi_reader_example.c
 *  @line mpicc
 *  @until -o
 *
 *  Now you can link your program with:
 *  @line mpicc
 *  @until -o
 *
 *  @}
 *
 *  @defgroup usage_writing_mpi Usage in writing mode - MPI example
 *  @{
 *
 *  This is a short example of how to use the OTF2 writing interface with MPI.
 *  This example is available as source code in the file
 *  @refexample{otf2_mpi_writer_example,c}.
 *  @dontinclude otf2_mpi_writer_example.c
 *
 *  We start with inclusion of some standard headers.
 *  @skipline stdlib.h
 *  @until inttypes.h
 *
 *  And than include the MPI and OTF2 header.
 *  @line mpi.h
 *  @until otf2/otf2.h
 *
 *  Now prepare the inclusion of the <otf2/OTF2_MPI_Collectives.h> header. As
 *  it is a header-only interface, it needs some information about the used
 *  MPI environment. In particular the MPI datatypes which match the C99 types
 *  @c uint64_t and @c int64_t. In case you have a MPI 3.0 conforming MPI
 *  implementation you can skip this. If not, provide \#define's for the
 *  following macros prior the \#include statement. In this example, we assume
 *  a LP64 platform.
 *  @line MPI_VERSION
 *  @until endif
 *
 *  After this preparatory step, we can include the <otf2/OTF2_MPI_Collectives.h>
 *  header.
 *  @skipline otf2/OTF2_MPI_Collectives.h
 *
 *  We use @p MPI_Wtime to get timestamps for our events but need to convert
 *  the seconds to an integral value. We use a nano second resolution.
 *  @line OTF2_TimeStamp
 *  @until }
 *
 *  Define a pre and post flush callback.
 *  If no memory is left in OTF2's internal memory buffer or the writer handle
 *  is closed a memory buffer flushing routine is triggered.
 *  The pre flush callback is triggered right before a buffer flush. It needs to
 *  return either OTF2_FLUSH to flush the recorded data to a file or
 *  OTF2_NO_FLUSH to suppress flushing data to a file.
 *  The post flush callback is triggered right after a memory buffer flush. It
 *  has to return a current timestamp which is recorded to mark the time spend
 *  in a buffer flush.
 *  The callbacks are passed via a struct to OTF2.
 *  @line OTF2_FlushType
 *  @until };
 *
 *  Now everything is prepared to begin with the main program.
 *  @line int
 *  @until {
 *
 *  First initialize the MPI environment and query the size and rank.
 *  @line MPI_Init
 *  @until MPI_Comm_rank
 *
 *  Create new archive handle.
 *  @line Open
 *  @until );
 *
 *  Set the previously defined flush callbacks.
 *  @skipline SetFlushCallbacks
 *
 *  Now we provide the OTF2 archive object the MPI collectives. As all ranks
 *  in @c MPI_COMM_WORLD write into the archive, we use this communicator as
 *  the global one. We set the local communicator to @c MPI_COMM_NULL, as we
 *  don't care about file optimization here.
 *  @line SetCollectiveCallbacks
 *  @until );
 *
 *  Now we can create the event files. Though physical files aren't created yet.
 *  @skipline OpenEvtFiles
 *
 *  Each rank now requests an event writer with its rank number as the location
 *  id.
 *  @line GetEvtWriter
 *  @until );
 *
 *  We note the start time in each rank, this is later used to determine the
 *  global epoch.
 *  @skipline epoch_start
 *
 *  Write an enter and a leave record for region 0 to the local event writer.
 *  @line Enter
 *  @until );
 *
 *  We also record a @c MPI_Barrier in the trace. For this we generate an event
 *  before we do the MPI call.
 *  @line OTF2_EvtWriter_MpiCollectiveBegin
 *  @until );
 *
 *  Now we can do the @c MPI_Barrier call.
 *  @skipline MPI_Barrier
 *
 *  After we passed the @c MPI_Barrier. we can note the end of the collective
 *  operation inside the event stream.
 *  @line OTF2_EvtWriter_MpiCollectiveEnd
 *  @until );
 *
 *  Finally we leave the region again with the leave region.
 *  @line Leave
 *  @until );
 *
 *  The event recording is now done, note the end time in each rank.
 *  @skipline epoch_end
 *
 *  Now close the event writer, before closing the event files collectively.
 *  @skipline CloseEvtWriter
 *
 *  After we wrote all of the events we close the event files again.
 *  @skipline CloseEvtFiles
 *
 *  The per-rank definition files are optional, but create them nevertheless to
 *  please the reader. In a real application, this should be used to write
 *  @eref{ClockOffset} records for time synchronisations.
 *  @line OpenDefFiles
 *  @until CloseDefFiles
 *
 *  We now collect all of the @c epoch_start and @c epoch_end timestamps by
 *  calculating the minimum and maximize and provide these to the root rank.
 *  @line global_epoch_start
 *  @until );
 *  @until );
 *
 *  Only the root rank will write the global definitions, thus only he requests
 *  a writer object from the archive.
 *  @line rank
 *  @until global_def_writer
 *
 *  We need to define the clock used for this trace and the overall timestamp
 *  range.
 *  @line WriteClockProperties
 *  @until );
 *
 *  Now we can start writing the referenced definitions, starting with the strings.
 *  @line String
 *  @until 8
 *
 *  Write definition for the code region which was just entered and left to the
 *  global definition writer.
 *  @line Region
 *  @until );
 *
 *  Write the system tree to the global definition writer.
 *  @line SystemTreeNode
 *  @until );
 *
 *  For each rank we define a new location group and one location. We provide
 *  also a unique string for each location group.
 *  @line for
 *  @until }
 *
 *  The last step is to define the MPI communicator. This is a three-step process.
 *  First we define that this trace actually recorded in the MPI paradigm and
 *  enumerate all locations which participate in this paradigm. As we used
 *  the MPI ranks directly as the location id, the array with the locations is
 *  the identity.
 *  @line comm_locations
 *  @until );
 *
 *  Now we can define sub-groups of the previously defined list of communication.
 *  locations. For @c MPI_COMM_WORLD this is the whole group here. Note the these
 *  sub-groups are created by using indices into the list of communication
 *  locations, and not by enumerating location ids again. But in this example
 *  the sub-group is the identity again.
 *  @line OTF2_GlobalDefWriter_WriteGroup
 *  @until );
 *
 *  Finally we can write the definition of the @c MPI_COMM_WORLD communicator.
 *  This finalizes the writing of the global definitions and we can also close
 *  the writer object.
 *  @line OTF2_GlobalDefWriter_WriteComm
 *  @until }
 *
 *  All the other ranks wait inside this barrier so that root can write the
 *  global definitions.
 *  @skipline MPI_Barrier
 *
 *  At the end, close the archive, finalize the MPI environment, and exit.
 *  @line Close
 *  @until }
 *
 *  To compile your program use a command like the following. Note that we need
 *  to activate the C99 standard explicitly for GCC.
 *  @dontinclude Makefile
 *  @skip otf2_mpi_writer_example.c
 *  @line mpicc
 *  @until -o
 *
 *  Now you can link your program with:
 *  @line mpicc
 *  @until -o
 *
 *  @}
 *
 *  @}
 */


#ifndef OTF2_MPI_COLLECTIVES_H
#define OTF2_MPI_COLLECTIVES_H

#include <stdint.h>
#include <stdlib.h>

#include <otf2/otf2.h>


#include <mpi.h>


/** @brief Register a MPI collective context to an OTF2 archive.
 *
 *  @param archive     The archive handle.
 *  @param globalComm  The global communicator to use. Will be duplicated.
 *  @param localComm   The local communicator to use. Maybe MPI_COMM_NULL,
 *                     otherwise all @p localComm must be disjoint and join to
 *                     @p globalComm. Will be duplicated.
 *
 *  @return Success or error code.
 */
static OTF2_ErrorCode
OTF2_MPI_Archive_SetCollectiveCallbacks( OTF2_Archive* archive,
                                         MPI_Comm      globalComm,
                                         MPI_Comm      localComm );


/** @brief Register a MPI collective context to an OTF2 archive.
 *
 *  @param archive        The archive handle.
 *  @param globalComm     The global communicator to use. Will be duplicated.
 *  @param numberOfFiles  Splits the @p globalComm into @p numberOfFiles
 *                        disjoint sub-communicators and evenly distribute
 *                        the ranks among them.
 *
 *  @return Success or error code.
 */
static OTF2_ErrorCode
OTF2_MPI_Archive_SetCollectiveCallbacksSplit( OTF2_Archive* archive,
                                              MPI_Comm      globalComm,
                                              uint32_t      numberOfFiles );


/** @brief Register a MPI collective context to an OTF2 reader.
 *
 *  @param reader      The reader handle.
 *  @param globalComm  The global communicator to use. Will be duplicated.
 *
 *  @return Success or error code.
 */
static OTF2_ErrorCode
OTF2_MPI_Reader_SetCollectiveCallbacks( OTF2_Reader* reader,
                                        MPI_Comm     globalComm );


/** @def OTF2_MPI_USE_PMPI
 *  @brief If you want that the collectives call the PMPI interface, define
 *  this macro before including the header.
 *
 *  @internal
 *  The \#define OTF2_MPI_USE_PMPI below is only to please doxygen.
 *
 *  @def CALL_MPI
 *  @def OTF2_MPI_USE_PMPI_undef_me
 */
#ifdef OTF2_MPI_USE_PMPI
# define CALL_MPI( name ) P ## name
#else
# define CALL_MPI( name ) name
# define OTF2_MPI_USE_PMPI
# define OTF2_MPI_USE_PMPI_undef_me
#endif


/**
 *  @def OTF2_MPI_UINT8_T
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c uint8_t before including the header.
 *  @c MPI_UNSIGNED_CHAR or @c MPI_UINT8_T are used as proper default value.
 *  The latter in case of a MPI 3.0 conforming implementation.
 */
#ifndef OTF2_MPI_UINT8_T
# if MPI_VERSION >= 3
#  define OTF2_MPI_UINT8_T MPI_UINT8_T
# else
#  define OTF2_MPI_UINT8_T MPI_UNSIGNED_CHAR
# endif
#endif

/**
 *  @def OTF2_MPI_INT8_T
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c int8_t before including the header.
 *  @c MPI_CHAR or @c MPI_INT8_T are used as proper default value.
 *  The latter in case of a MPI 3.0 conforming implementation.
 */
#ifndef OTF2_MPI_INT8_T
# if MPI_VERSION >= 3
#  define OTF2_MPI_INT8_T MPI_INT8_T
# else
#  define OTF2_MPI_INT8_T MPI_CHAR
# endif
#endif


/**
 *  @def OTF2_MPI_UINT16_T
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c uint16_t before including the header.
 *  @c MPI_UNSIGNED_SHORT or @c MPI_UINT16_T are used as proper default value.
 *  The latter in case of a MPI 3.0 conforming implementation.
 */
#ifndef OTF2_MPI_UINT16_T
# if MPI_VERSION >= 3
#  define OTF2_MPI_UINT16_T MPI_UINT16_T
# else
#  define OTF2_MPI_UINT16_T MPI_UNSIGNED_SHORT
# endif
#endif

/**
 *  @def OTF2_MPI_INT16_T
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c int32_t before including the header.
 *  @c MPI_SHORT or @c MPI_INT32_T are used as proper default value.
 *  The latter in case of a MPI 3.0 conforming implementation.
 */
#ifndef OTF2_MPI_INT16_T
# if MPI_VERSION >= 3
#  define OTF2_MPI_INT16_T MPI_INT16_T
# else
#  define OTF2_MPI_INT16_T MPI_SHORT
# endif
#endif


/**
 *  @def OTF2_MPI_UINT32_T
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c uint32_t before including the header.
 *  @c MPI_UNSIGNED or @c MPI_UINT32_T are used as proper default value.
 *  The latter in case of a MPI 3.0 conforming implementation.
 */
#ifndef OTF2_MPI_UINT32_T
# if MPI_VERSION >= 3
#  define OTF2_MPI_UINT32_T MPI_UINT32_T
# else
#  define OTF2_MPI_UINT32_T MPI_UNSIGNED
# endif
#endif

/**
 *  @def OTF2_MPI_INT32_T
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c int32_t before including the header.
 *  @c MPI_INT or @c MPI_INT32_T are used as proper default value.
 *  The latter in case of a MPI 3.0 conforming implementation.
 */
#ifndef OTF2_MPI_INT32_T
# if MPI_VERSION >= 3
#  define OTF2_MPI_INT32_T MPI_INT32_T
# else
#  define OTF2_MPI_INT32_T MPI_INT
# endif
#endif


/**
 *  @def OTF2_MPI_UINT64_T
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c uint64_t before including the header.
 *  @c MPI_UINT64_T is used as proper default value in case of a MPI 3.0
 *  conforming implementation.
 */
#ifndef OTF2_MPI_UINT64_T
# define OTF2_MPI_UINT64_T MPI_UINT64_T
# if MPI_VERSION < 3
#  error Please define OTF2_MPI_UINT64_T to a suitable MPI datatype for uint64_t.
# endif
#endif

/**
 *  @def OTF2_MPI_INT64_T
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c int64_t before including the header.
 *  @c MPI_INT64_T is used as proper default value in case of a MPI 3.0
 *  conforming implementation.
 */
#ifndef OTF2_MPI_INT64_T
# define OTF2_MPI_INT64_T MPI_INT64_T
# if MPI_VERSION < 3
#  error Please define OTF2_MPI_INT64 to a suitable MPI datatype for int64_t.
# endif
#endif


/**
 *  @def OTF2_MPI_FLOAT
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c float before including the header.
 *  @c MPI_FLOAT is used as proper default value.
 */
#ifndef OTF2_MPI_FLOAT
# define OTF2_MPI_FLOAT MPI_FLOAT
#endif


/**
 *  @def OTF2_MPI_DOUBLE
 *  @brief Define this macro to a suitable MPI datatype to be used for
 *         @c double before including the header.
 *  @c MPI_DOUBLE is used as proper default value.
 */
#ifndef OTF2_MPI_DOUBLE
# define OTF2_MPI_DOUBLE MPI_DOUBLE
#endif


/**
 * @cond IMPLEMENTATION_OF_THE_CALLBACKS__PLEASE_IGNORE
 */


/** @brief Collective context which wraps a MPI communicator.
 */
struct OTF2_CollectiveContext
{
    MPI_Comm comm;
    int      size;
    int      rank;
    int      displacements[ 1 ];
};


/** @brief User data structure, which will be used by the MPI collectives.
 */
typedef struct OTF2_MPI_UserData
{
    OTF2_CollectiveCallbacks callbacks;
    OTF2_CollectiveContext*  global;
    OTF2_CollectiveContext*  local;
} OTF2_MPI_UserData;


static void
otf2_mpi_get_collectives( OTF2_CollectiveCallbacks* collectiveCallbacks );


static OTF2_CollectiveContext*
otf2_mpi_create_context( MPI_Comm comm,
                         bool     duplicate );


static void
otf2_mpi_destroy_context( OTF2_CollectiveContext* collectiveContext );


static OTF2_CollectiveContext*
otf2_mpi_split_context_by_number( OTF2_CollectiveContext* commContext,
                                  uint32_t                numberOfFiles );


static OTF2_ErrorCode
OTF2_MPI_Archive_SetCollectiveCallbacks( OTF2_Archive* archive,
                                         MPI_Comm      globalComm,
                                         MPI_Comm      localComm )
{
    OTF2_ErrorCode     status    = OTF2_SUCCESS;
    OTF2_MPI_UserData* user_data = NULL;

    ( void )OTF2_MPI_Archive_SetCollectiveCallbacksSplit;
    ( void )OTF2_MPI_Reader_SetCollectiveCallbacks;

    if ( !archive )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    if ( MPI_COMM_NULL == globalComm )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    user_data = ( OTF2_MPI_UserData* )calloc( 1, sizeof( *user_data ) );
    if ( !user_data )
    {
        return OTF2_ERROR_MEM_ALLOC_FAILED;
    }

    otf2_mpi_get_collectives( &user_data->callbacks );

    user_data->global = otf2_mpi_create_context( globalComm, true );
    if ( !user_data->global )
    {
        status = OTF2_ERROR_PROCESSED_WITH_FAULTS;
        goto out;
    }

    if ( MPI_COMM_NULL != localComm )
    {
        user_data->local = otf2_mpi_create_context( localComm, true );
        if ( !user_data->local )
        {
            status = OTF2_ERROR_PROCESSED_WITH_FAULTS;
            goto out;
        }
    }

    status = OTF2_Archive_SetCollectiveCallbacks( archive,
                                                  &user_data->callbacks,
                                                  user_data,
                                                  user_data->global,
                                                  user_data->local );

out:
    if ( OTF2_SUCCESS != status )
    {
        otf2_mpi_destroy_context( user_data->local );
        otf2_mpi_destroy_context( user_data->global );
        free( user_data );
    }

    return status;
}


static OTF2_ErrorCode
OTF2_MPI_Archive_SetCollectiveCallbacksSplit( OTF2_Archive* archive,
                                              MPI_Comm      globalComm,
                                              uint32_t      numberOfFiles )
{
    OTF2_ErrorCode     status    = OTF2_SUCCESS;
    OTF2_MPI_UserData* user_data = NULL;

    ( void )OTF2_MPI_Archive_SetCollectiveCallbacks;
    ( void )OTF2_MPI_Reader_SetCollectiveCallbacks;

    if ( !archive )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    if ( MPI_COMM_NULL == globalComm )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    user_data = ( OTF2_MPI_UserData* )calloc( 1, sizeof( *user_data ) );
    if ( !user_data )
    {
        return OTF2_ERROR_MEM_ALLOC_FAILED;
    }

    otf2_mpi_get_collectives( &user_data->callbacks );

    user_data->global = otf2_mpi_create_context( globalComm, true );
    if ( !user_data->global )
    {
        status = OTF2_ERROR_PROCESSED_WITH_FAULTS;
        goto out;
    }

    user_data->local = otf2_mpi_split_context_by_number( user_data->global,
                                                         numberOfFiles );
    if ( !user_data->local )
    {
        status = OTF2_ERROR_PROCESSED_WITH_FAULTS;
        goto out;
    }

    status = OTF2_Archive_SetCollectiveCallbacks( archive,
                                                  &user_data->callbacks,
                                                  user_data,
                                                  user_data->global,
                                                  user_data->local );

out:
    if ( OTF2_SUCCESS != status )
    {
        otf2_mpi_destroy_context( user_data->local );
        otf2_mpi_destroy_context( user_data->global );
        free( user_data );
    }

    return status;
}


static OTF2_ErrorCode
OTF2_MPI_Reader_SetCollectiveCallbacks( OTF2_Reader* reader,
                                        MPI_Comm     globalComm )
{
    OTF2_ErrorCode     status    = OTF2_SUCCESS;
    OTF2_MPI_UserData* user_data = NULL;

    ( void )OTF2_MPI_Archive_SetCollectiveCallbacks;
    ( void )OTF2_MPI_Archive_SetCollectiveCallbacksSplit;

    if ( !reader )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    if ( MPI_COMM_NULL == globalComm )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    user_data = ( OTF2_MPI_UserData* )calloc( 1, sizeof( *user_data ) );
    if ( !user_data )
    {
        return OTF2_ERROR_MEM_ALLOC_FAILED;
    }

    otf2_mpi_get_collectives( &user_data->callbacks );

    user_data->global = otf2_mpi_create_context( globalComm, true );
    if ( !user_data->global )
    {
        status = OTF2_ERROR_PROCESSED_WITH_FAULTS;
        goto out;
    }

    status = OTF2_Reader_SetCollectiveCallbacks( reader,
                                                 &user_data->callbacks,
                                                 user_data,
                                                 user_data->global,
                                                 NULL );

out:
    if ( OTF2_SUCCESS != status )
    {
        otf2_mpi_destroy_context( user_data->global );
        free( user_data );
    }

    return status;
}


static OTF2_CollectiveContext*
otf2_mpi_create_context( MPI_Comm comm,
                         bool     duplicate )
{
    int ret;
    int size;

    ret = CALL_MPI( MPI_Comm_size ) ( comm, &size );
    if ( MPI_SUCCESS != ret )
    {
        return NULL;
    }

    OTF2_CollectiveContext* new_context =
        ( OTF2_CollectiveContext* )malloc( sizeof( *new_context )
                                           + ( ( size - 1 ) * sizeof( int ) ) );
    if ( !new_context )
    {
        return NULL;
    }

    new_context->size = size;
    ret               = CALL_MPI( MPI_Comm_rank ) ( comm, &new_context->rank );
    if ( MPI_SUCCESS != ret )
    {
        free( new_context );
        return NULL;
    }

    if ( duplicate )
    {
        ret = CALL_MPI( MPI_Comm_dup ) ( comm, &new_context->comm );
        if ( MPI_SUCCESS != ret )
        {
            free( new_context );
            return NULL;
        }
    }
    else
    {
        new_context->comm = comm;
    }

    return new_context;
}


static void
otf2_mpi_destroy_context( OTF2_CollectiveContext* collectiveContext )
{
    if ( !collectiveContext )
    {
        return;
    }

    CALL_MPI( MPI_Comm_free ) ( &collectiveContext->comm );

    free( collectiveContext );
}


static OTF2_CollectiveContext*
otf2_mpi_split_context( OTF2_CollectiveContext* commContext,
                        int                     color,
                        int                     key )
{
    OTF2_CollectiveContext* new_context;
    MPI_Comm                new_comm;
    int                     ret;
    ret = CALL_MPI( MPI_Comm_split ) ( commContext->comm,
                                       color,
                                       key,
                                       &new_comm );
    if ( MPI_SUCCESS != ret )
    {
        return NULL;
    }

    new_context = otf2_mpi_create_context( new_comm, false );
    if ( !new_context )
    {
        CALL_MPI( MPI_Comm_free ) ( &new_comm );
        return NULL;
    }

    return new_context;
}


static OTF2_CollectiveContext*
otf2_mpi_split_context_by_number( OTF2_CollectiveContext* commContext,
                                  uint32_t                numberOfFiles )
{
    int file_number = 0;
    int rem         = commContext->size % numberOfFiles;
    int local_size  = commContext->size / numberOfFiles + !!rem;
    int local_rank  = 0;
    int local_root  = 0;
    int i;
    for ( i = 0; i < commContext->rank; i++ )
    {
        local_rank++;
        if ( local_root + local_size == i + 1 )
        {
            local_root += local_size;
            file_number++;
            local_size -= file_number == rem;
            local_rank  = 0;
        }
    }

    return otf2_mpi_split_context( commContext,
                                   file_number,
                                   local_rank );
}


static MPI_Datatype
otf2_mpi_get_type( OTF2_Type type )
{
#define case_return( TYPE, MPI_SUFFIX ) \
    case OTF2_TYPE_ ## TYPE: \
        return OTF2_MPI_ ## TYPE ## MPI_SUFFIX
    switch ( type )
    {
        case_return( UINT8,  _T );
        case_return( INT8,   _T );
        case_return( UINT16, _T );
        case_return( INT16,  _T );
        case_return( UINT32, _T );
        case_return( INT32,  _T );
        case_return( UINT64, _T );
        case_return( INT64,  _T );
        case_return( FLOAT,     );
        case_return( DOUBLE,    );
        default:
            return MPI_DATATYPE_NULL;
    }
#undef case_return
}


static void
otf2_mpi_collectives_release( void*                   userData,
                              OTF2_CollectiveContext* globalCommContext,
                              OTF2_CollectiveContext* localCommContext )
{
    OTF2_MPI_UserData* user_data = ( OTF2_MPI_UserData* )userData;

    ( void )globalCommContext;
    ( void )localCommContext;

    otf2_mpi_destroy_context( user_data->global );
    otf2_mpi_destroy_context( user_data->local );
    free( user_data );
}


static OTF2_CallbackCode
otf2_mpi_collectives_create_local_comm( void*                    userData,
                                        OTF2_CollectiveContext** localCommContextOut,
                                        OTF2_CollectiveContext*  globalCommContext,
                                        uint32_t                 globalRank,
                                        uint32_t                 globalSize,
                                        uint32_t                 localRank,
                                        uint32_t                 localSize,
                                        uint32_t                 fileNumber,
                                        uint32_t                 numberOfFiles )
{
    ( void )userData;
    ( void )globalRank;
    ( void )globalSize;
    ( void )localSize;
    ( void )numberOfFiles;

    *localCommContextOut = otf2_mpi_split_context( globalCommContext,
                                                   fileNumber,
                                                   localRank );

    return *localCommContextOut
           ? OTF2_CALLBACK_SUCCESS
           : OTF2_CALLBACK_ERROR;
}


static OTF2_CallbackCode
otf2_mpi_collectives_free_local_comm( void*                   userData,
                                      OTF2_CollectiveContext* localCommContext )
{
    ( void )userData;

    otf2_mpi_destroy_context( localCommContext );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_mpi_collectives_get_size( void*                   userData,
                               OTF2_CollectiveContext* commContext,
                               uint32_t*               size )
{
    ( void )userData;

    *size = commContext->size;

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_mpi_collectives_get_rank( void*                   userData,
                               OTF2_CollectiveContext* commContext,
                               uint32_t*               rank )
{
    ( void )userData;

    *rank = commContext->rank;

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_mpi_collectives_barrier( void*                   userData,
                              OTF2_CollectiveContext* commContext )
{
    int ret;

    ( void )userData;

    ret = CALL_MPI( MPI_Barrier ) ( commContext->comm );

    return MPI_SUCCESS == ret
           ? OTF2_CALLBACK_SUCCESS
           : OTF2_CALLBACK_ERROR;
}


static OTF2_CallbackCode
otf2_mpi_collectives_bcast( void*                   userData,
                            OTF2_CollectiveContext* commContext,
                            void*                   data,
                            uint32_t                numberElements,
                            OTF2_Type               type,
                            uint32_t                root )
{
    int ret;

    ( void )userData;

    ret = CALL_MPI( MPI_Bcast ) ( data,
                                  numberElements,
                                  otf2_mpi_get_type( type ),
                                  root,
                                  commContext->comm );

    return MPI_SUCCESS == ret
           ? OTF2_CALLBACK_SUCCESS
           : OTF2_CALLBACK_ERROR;
}


static OTF2_CallbackCode
otf2_mpi_collectives_gather( void*                   userData,
                             OTF2_CollectiveContext* commContext,
                             const void*             inData,
                             void*                   outData,
                             uint32_t                numberElements,
                             OTF2_Type               type,
                             uint32_t                root )
{
    int ret;

    ( void )userData;

    ret = CALL_MPI( MPI_Gather ) ( ( void* )inData,
                                   numberElements,
                                   otf2_mpi_get_type( type ),
                                   outData,
                                   numberElements,
                                   otf2_mpi_get_type( type ),
                                   root,
                                   commContext->comm );

    return MPI_SUCCESS == ret
           ? OTF2_CALLBACK_SUCCESS
           : OTF2_CALLBACK_ERROR;
}


static OTF2_CallbackCode
otf2_mpi_collectives_gatherv( void*                   userData,
                              OTF2_CollectiveContext* commContext,
                              const void*             inData,
                              uint32_t                inElements,
                              void*                   outData,
                              const uint32_t*         outElements,
                              OTF2_Type               type,
                              uint32_t                root )
{
    int  ret;
    int* displs = NULL;

    ( void )userData;

    if ( ( int )root == commContext->rank )
    {
        int i;
        int displ = 0;
        for ( i = 0; i < commContext->rank; ++i )
        {
            commContext->displacements[ i ] = displ;
            displ                          += outElements[ i ];
        }
        displs = commContext->displacements;
    }

    ret = CALL_MPI( MPI_Gatherv ) ( ( void* )inData,
                                    inElements,
                                    otf2_mpi_get_type( type ),
                                    outData,
                                    ( int* )outElements,
                                    displs,
                                    otf2_mpi_get_type( type ),
                                    root,
                                    commContext->comm );

    return MPI_SUCCESS == ret
           ? OTF2_CALLBACK_SUCCESS
           : OTF2_CALLBACK_ERROR;
}


static OTF2_CallbackCode
otf2_mpi_collectives_scatter( void*                   userData,
                              OTF2_CollectiveContext* commContext,
                              const void*             inData,
                              void*                   outData,
                              uint32_t                numberElements,
                              OTF2_Type               type,
                              uint32_t                root )
{
    ( void )userData;

    int ret = CALL_MPI( MPI_Scatter ) ( ( void* )inData,
                                        numberElements,
                                        otf2_mpi_get_type( type ),
                                        outData,
                                        numberElements,
                                        otf2_mpi_get_type( type ),
                                        root,
                                        commContext->comm );

    return MPI_SUCCESS == ret
           ? OTF2_CALLBACK_SUCCESS
           : OTF2_CALLBACK_ERROR;
}


static OTF2_CallbackCode
otf2_mpi_collectives_scatterv( void*                   userData,
                               OTF2_CollectiveContext* commContext,
                               const void*             inData,
                               const uint32_t*         inElements,
                               void*                   outData,
                               uint32_t                outElements,
                               OTF2_Type               type,
                               uint32_t                root )
{
    int* displs = NULL;

    ( void )userData;

    if ( ( int )root == commContext->rank )
    {
        int i;
        int displ = 0;
        for ( i = 0; i < commContext->rank; ++i )
        {
            commContext->displacements[ i ] = displ;
            displ                          += inElements[ i ];
        }
        displs = commContext->displacements;
    }

    int ret = CALL_MPI( MPI_Scatterv ) ( ( void* )inData,
                                         ( int* )inElements,
                                         displs,
                                         otf2_mpi_get_type( type ),
                                         outData,
                                         outElements,
                                         otf2_mpi_get_type( type ),
                                         root,
                                         commContext->comm );

    return MPI_SUCCESS == ret
           ? OTF2_CALLBACK_SUCCESS
           : OTF2_CALLBACK_ERROR;
}


static void
otf2_mpi_get_collectives( OTF2_CollectiveCallbacks* collectiveCallbacks )
{
    collectiveCallbacks->otf2_release           = otf2_mpi_collectives_release;
    collectiveCallbacks->otf2_get_size          = otf2_mpi_collectives_get_size;
    collectiveCallbacks->otf2_get_rank          = otf2_mpi_collectives_get_rank;
    collectiveCallbacks->otf2_create_local_comm = otf2_mpi_collectives_create_local_comm;
    collectiveCallbacks->otf2_free_local_comm   = otf2_mpi_collectives_free_local_comm;
    collectiveCallbacks->otf2_barrier           = otf2_mpi_collectives_barrier;
    collectiveCallbacks->otf2_bcast             = otf2_mpi_collectives_bcast;
    collectiveCallbacks->otf2_gather            = otf2_mpi_collectives_gather;
    collectiveCallbacks->otf2_gatherv           = otf2_mpi_collectives_gatherv;
    collectiveCallbacks->otf2_scatter           = otf2_mpi_collectives_scatter;
    collectiveCallbacks->otf2_scatterv          = otf2_mpi_collectives_scatterv;
}


#undef CALL_MPI
#ifdef OTF2_MPI_USE_PMPI_undef_me
#undef OTF2_MPI_USE_PMPI
#undef OTF2_MPI_USE_PMPI_undef_me
#endif


/**
 * @endcond
 */


#endif /* OTF2_MPI_COLLECTIVES_H */
