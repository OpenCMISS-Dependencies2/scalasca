/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @internal
 *
 *  @file
 *
 *  @brief This module is an internal representation of the opened
 *         archive. It is mainly used to organize the archive related
 *         meta-data.
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>

#include "otf2_internal.h"
#include "otf2_lock.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"
#include "otf2_archive_location.h"

#include "otf2_attic_types.h"

#include "otf2_attribute_value.h"
#include "otf2_attribute_list.h"
#include "otf2_id_map.h"

#include "OTF2_File.h"
#include "otf2_file_substrate.h"

#include "otf2_local_definitions.h"
#include "otf2_global_definitions.h"
#include "otf2_events_int.h"
#include "otf2_snaps.h"

#include "otf2_def_reader.h"
#include "otf2_def_writer.h"
#include "otf2_evt_reader.h"
#include "otf2_evt_writer.h"
#include "otf2_snap_reader.h"
#include "otf2_snap_writer.h"

#include "otf2_global_def_reader.h"
#include "otf2_global_def_writer.h"
#include "otf2_global_evt_reader.h"
#include "otf2_global_snap_reader.h"

#include "otf2_thumbnail.h"

#include "otf2_marker_int.h"

#include "otf2_collectives.h"

#define VALID_PROPERTY_NAME_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_"

#define OTF2_ARCHIVE_LOCATION_SIZE 64

/*__SET_FUNCTIONS____________________________________________*/


/** @brief Create a new internal archive.
 *
 *  This function generates the basic datastructure for an internal archive representation,
 *  which is mainly used to hold the metadata on runtime. Every member variable can be set
 *  or get with related get and set routines.
 *
 *  @param archivePath      Path to the archive i.e. the directory where the
 *                          anchor file is located.
 *  @param archiveName      Name of the archive. It is used to generate sub
 *                          pathes e.g. 'archiveName.otf2'.
 *  @param substrate        Determines which file substrate should be used in
 *                          writing mode. In reading mode this value is ignored
 *                          because the correct file substrate is extracted
 *                          from the anchor file.
 *  @param chunksize        Requested size of the internal chunks in writing
 *                          mode. In reading mode this value is ignored because
 *                          the correct chunk size is extracted from the
 *                          anchor file.
 *  @param file_mode        Determines if in reading, writing or modify mode.
 *
 *  @return                 Returns an archive handle if successful, NULL
 *                          otherwise.
 */

OTF2_Archive*
otf2_archive_open( const char*        archivePath,
                   const char*        archiveName,
                   OTF2_FileSubstrate substrate,
                   OTF2_Compression   compression,
                   uint64_t           chunkSizeEvents,
                   uint64_t           chunkSizeDefs,
                   OTF2_FileMode      fileMode )
{
    OTF2_ErrorCode ret_error = OTF2_ERROR_INVALID;

    UTILS_ASSERT( archivePath );
    UTILS_ASSERT( archiveName );

    /* Generate the initial structure and do some controling */
    OTF2_Archive* ret = calloc( 1, sizeof( *ret ) );

    if ( !ret )
    {
        UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Alloc failed!" );
        return NULL;
    }

    /* Set initial values */
    ret->file_mode         = OTF2_UNDEFINED_TYPE;
    ret->prev_file_mode    = OTF2_UNDEFINED_TYPE;
    ret->substrate         = OTF2_SUBSTRATE_UNDEFINED;
    ret->compression       = OTF2_COMPRESSION_UNDEFINED;
    ret->chunk_size_events = OTF2_UNDEFINED_UINT64;
    ret->chunk_size_defs   = OTF2_UNDEFINED_UINT64;

    ret->number_of_locations   = OTF2_UNDEFINED_UINT64;
    ret->number_of_global_defs = OTF2_UNDEFINED_UINT64;

    ret->number_of_thumbnails = 0;
    ret->number_of_snapshots  = 0;

    /* Set default values for hints */
    ret->hint_global_reader_locked = false;
    ret->hint_global_reader        = OTF2_FALSE;

    /* Initialize mapping to 0. */
    ret->calling_context_to_region_map          = 0;
    ret->calling_context_to_region_map_size     = 0;
    ret->calling_context_to_region_map_capacity = 0;
    ret->calling_context_to_region_index_map    = 0;

    /* Already known values */

    if ( chunkSizeEvents != OTF2_UNDEFINED_UINT64 )
    {
        ret_error = otf2_archive_set_event_chunksize( ret, chunkSizeEvents );

        if ( ret_error != OTF2_SUCCESS )
        {
            otf2_archive_close( ret );
            UTILS_ERROR( ret_error, "Set of event chunk sizes failed!" );
            return NULL;
        }
    }

    if ( chunkSizeDefs != OTF2_UNDEFINED_UINT64 )
    {
        ret_error = otf2_archive_set_def_chunksize( ret, chunkSizeDefs );

        if ( ret_error != OTF2_SUCCESS )
        {
            otf2_archive_close( ret );
            UTILS_ERROR( ret_error, "Set of def chunk sizes failed!" );
            return NULL;
        }
    }

    ret_error = otf2_archive_set_file_mode( ret, fileMode );
    if ( ret_error != OTF2_SUCCESS )
    {
        otf2_archive_close( ret );
        UTILS_ERROR( ret_error, "Set failed!" );
        return NULL;
    }

    if ( substrate != OTF2_SUBSTRATE_UNDEFINED )
    {
        ret_error = otf2_archive_set_file_substrate( ret, substrate );

        if ( ret_error != OTF2_SUCCESS )
        {
            otf2_archive_close( ret );
            UTILS_ERROR( ret_error, "Set failed!" );
            return NULL;
        }
    }

    if ( compression != OTF2_COMPRESSION_UNDEFINED )
    {
        ret_error = otf2_archive_set_compression( ret, compression );

        if ( ret_error != OTF2_SUCCESS )
        {
            otf2_archive_close( ret );
            UTILS_ERROR( ret_error, "Set failed!" );
            return NULL;
        }
    }

    /* Set the archive path */
    ret_error = otf2_archive_set_archive_path( ret, archivePath );
    if ( ret_error != OTF2_SUCCESS )
    {
        otf2_archive_close( ret );
        UTILS_ERROR( ret_error, "Set failed!" );
        return NULL;
    }


    /* Set the archive name */
    ret_error = otf2_archive_set_archive_name( ret, archiveName );
    if ( ret_error != OTF2_SUCCESS )
    {
        otf2_archive_close( ret );
        UTILS_ERROR( ret_error, "Set failed!" );
        return NULL;
    }

    ret->collective_callbacks = NULL;
    ret->collective_data      = NULL;
    ret->global_comm_context  = NULL;
    ret->local_comm_context   = NULL;

    ret->locking_callbacks = NULL;
    ret->locking_data      = NULL;
    ret->lock              = NULL;

    /* Set the OTF2 and trace format versions */
    if ( fileMode != OTF2_FILEMODE_READ )
    {
        /* We count the number of definitions and locations, thus start with 0 */
        ret->number_of_locations   = 0;
        ret->number_of_global_defs = 0;

        ret_error = otf2_archive_set_version( ret,
                                              OTF2_VERSION_MAJOR,
                                              OTF2_VERSION_MINOR,
                                              OTF2_VERSION_BUGFIX );
        if ( ret_error != OTF2_SUCCESS )
        {
            otf2_archive_close( ret );
            UTILS_ERROR( ret_error, "Set failed!" );
            return NULL;
        }

        ret_error = otf2_archive_set_trace_format( ret, OTF2_TRACE_FORMAT );
        if ( ret_error != OTF2_SUCCESS )
        {
            otf2_archive_close( ret );
            UTILS_ERROR( ret_error, "Set failed!" );
            return NULL;
        }
    }

    /* Initialize the lists where already allocated readers/writers are stored */
    ret->local_evt_readers = NULL;
    ret->local_def_readers = NULL;
    ret->local_evt_writers = NULL;
    ret->local_def_writers = NULL;

    ret->local_snap_readers = NULL;
    ret->local_snap_writers = NULL;

    ret->thumb_writers = NULL;
    ret->thumb_readers = NULL;

    ret->marker_reader = NULL;
    ret->marker_writer = NULL;

    /* Initialize locations vector.
     * The vector is initialized with a size of 64 locations and can be adapted
     * on demand. */
    ret->locations_number = 0;
    ret->locations        = ( otf2_archive_location* )calloc( OTF2_ARCHIVE_LOCATION_SIZE, sizeof( otf2_archive_location ) );
    if ( ret->locations == NULL )
    {
        otf2_archive_close( ret );
        UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                     "Allocation of locations vector failed!" );
        return NULL;
    }

    return ret;
}


/** @brief Deconstruct a processed archive.
 *
 *  This is more a finalization step. If this function is called, the archive will be
 *  finalized and completely written to disk. If this function is successful, a valid
 *  OTF2 archive is written to disk. All included reader and writer objects will be
 *  finalized and destroid. Their data is automatically written to disk.
 *
 *  @param archive          Archive handle.
 *
 *  @return                 Returns a writer object or NULL in case of an error.
 */

OTF2_ErrorCode
otf2_archive_close( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    OTF2_ErrorCode ret;

    if ( archive->collective_callbacks )
    {
        ret = otf2_file_substrate_close( archive );
        if ( ret != OTF2_SUCCESS )
        {
            UTILS_ERROR( ret, "Couldn't close the substrate." );
        }
    }

    /* All writers must be closed etc. */
    while ( archive->local_evt_writers )
    {
        OTF2_EvtWriter* next = archive->local_evt_writers->next;
        otf2_evt_writer_delete( archive->local_evt_writers );
        archive->local_evt_writers = next;
    }

    while ( archive->local_def_writers )
    {
        OTF2_DefWriter* next = archive->local_def_writers->next;
        otf2_def_writer_delete( archive->local_def_writers );
        archive->local_def_writers = next;
    }

    if ( archive->global_def_writer )
    {
        ret = otf2_global_def_writer_delete( archive->global_def_writer );
        if ( ret != OTF2_SUCCESS )
        {
            UTILS_ERROR( ret, "Free of global definition writer failed!" );
        }
    }

    while ( archive->local_snap_writers )
    {
        OTF2_SnapWriter* next = archive->local_snap_writers->next;
        otf2_snap_writer_delete( archive->local_snap_writers );
        archive->local_snap_writers = next;
    }

    while ( archive->thumb_writers )
    {
        OTF2_ThumbWriter* next = archive->thumb_writers->next;
        otf2_thumb_writer_delete( archive->thumb_writers );
        archive->thumb_writers = next;
    }

    if ( archive->marker_writer )
    {
        ret = otf2_marker_writer_delete( archive->marker_writer );
        if ( ret != OTF2_SUCCESS )
        {
            UTILS_ERROR( ret, "Free of marker writer failed!" );
        }
    }

    /* Readers must be freed also */
    if ( archive->global_evt_reader )
    {
        ret = otf2_global_evt_reader_delete( archive->global_evt_reader, false );
        if ( ret != OTF2_SUCCESS )
        {
            UTILS_ERROR( ret, "Free of global event reader failed!" );
        }
    }

    while ( archive->local_evt_readers )
    {
        OTF2_EvtReader* next = archive->local_evt_readers->next;
        otf2_evt_reader_delete( archive->local_evt_readers );
        archive->local_evt_readers = next;
    }

    while ( archive->local_def_readers )
    {
        OTF2_DefReader* next = archive->local_def_readers->next;
        otf2_def_reader_delete( archive->local_def_readers );
        archive->local_def_readers = next;
    }

    if ( archive->global_def_reader )
    {
        ret = otf2_global_def_reader_delete( archive->global_def_reader );
        if ( ret != OTF2_SUCCESS )
        {
            UTILS_ERROR( ret, "Free of global definition reader failed!" );
        }
    }

    if ( archive->global_snap_reader )
    {
        ret = otf2_global_snap_reader_delete( archive->global_snap_reader, false );
        if ( ret != OTF2_SUCCESS )
        {
            UTILS_ERROR( ret, "Free of global snapshot reader failed!" );
        }
    }

    while ( archive->local_snap_readers )
    {
        OTF2_SnapReader* next = archive->local_snap_readers->next;
        otf2_snap_reader_delete( archive->local_snap_readers );
        archive->local_snap_readers = next;
    }

    while ( archive->thumb_readers )
    {
        OTF2_ThumbReader* next = archive->thumb_readers->next;
        otf2_thumb_reader_delete( archive->thumb_readers );
        archive->thumb_readers = next;
    }

    if ( archive->marker_reader )
    {
        ret = otf2_marker_reader_delete( archive->marker_reader );
        if ( ret != OTF2_SUCCESS )
        {
            UTILS_ERROR( ret, "Free of marker reader failed!" );
        }
    }

    /* Free properties */
    otf2_archive_property* current_property = archive->properties;
    otf2_archive_property* next_property;
    while ( current_property != NULL )
    {
        next_property = current_property->next;
        free( current_property->name );
        free( current_property->value );
        free( current_property );
        current_property = next_property;
    }

    for ( uint32_t i = 0; i < archive->locations_number; i++ )
    {
        otf2_archive_location_finalize( &archive->locations[ i ] );
    }
    free( archive->locations );

    /* finalize the file substrate */
    if ( archive->substrate != OTF2_SUBSTRATE_UNDEFINED )
    {
        otf2_file_substrate_finalize( archive, archive->substrate );
    }

    /* call the release callback for the collective callback, if provided */
    if ( archive->collective_callbacks &&
         archive->collective_callbacks->otf2_release )
    {
        archive->collective_callbacks->otf2_release(
            archive->collective_data,
            archive->global_comm_context,
            archive->local_comm_context );
    }

    ret = otf2_lock_destroy( archive, archive->lock );
    if ( OTF2_SUCCESS != ret )
    {
        UTILS_ERROR( ret, "Couldn't destroy archive lock." );
    }

    /* call the release callback for the locking callbacks, if provided */
    if ( archive->locking_callbacks &&
         archive->locking_callbacks->otf2_release )
    {
        archive->locking_callbacks->otf2_release( archive->locking_data );
    }

    free( archive->calling_context_to_region_map );
    OTF2_IdMap_Free( archive->calling_context_to_region_index_map );

    /* Free all path buffers */
    free( archive->archive_path );
    free( archive->archive_name );
    free( archive->machine_name );
    free( archive->description );
    free( archive->creator );
    free( archive );

    return OTF2_SUCCESS;
}

/*_set_functions_____________________________________________________________*/


/** @brief Set the path to the folder where the archive is stored.
 *
 *
 *  @param archive      Archive handle.
 *  @param archivePath  Path of the archives folder.
 *
 *  @threadsafety       Only called at archive open time.
 *
 *  @return             OTF2_SUCCESS, or error.
 */

OTF2_ErrorCode
otf2_archive_set_archive_path( OTF2_Archive* archive,
                               const char*   archivePath )
{
    /* Be sure that valid pointers where passed */
    if ( !archive || !archivePath )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT, "No valid pointer!" );
    }

    /* File path can never be changed and set only one time */
    if ( archive->archive_path )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT, "Archive path already set!" );
    }

    /* Get enough space for the path */
    archive->archive_path = UTILS_CStr_dup( archivePath );
    if ( !archive->archive_path )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Can't allocate filepath!" );
    }

    return OTF2_SUCCESS;
}


/** @brief Set the archive name which is partly used to generate the file path
 *         to the folder where the archive is stored.
 *
 *
 *  @param archive      Archive handle.
 *  @param archiveName  Archive name, which is partly used to generate sub path.
 *
 *  @threadsafety       Only called at archive open time.
 *
 *  @return             Returns OTF2_SUCCESS if successful, an error code
 *                      if an error occurs.
 */

OTF2_ErrorCode
otf2_archive_set_archive_name( OTF2_Archive* archive,
                               const char*   archiveName )
{
    /* Test for valid pointers */
    UTILS_ASSERT( archive );
    UTILS_ASSERT( archiveName );

    /* Test if there is already a path set */
    if ( archive->archive_name )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Archive name is already set." );
    }

    /* The input string must be checked for '/' */
    if ( strchr( archiveName, '/' ) )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Archive name must not contain '/'!" );
    }

    /* Make a copy of the archive name */
    archive->archive_name = UTILS_CStr_dup( archiveName );
    if ( !archive->archive_name )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Memory allocation failed!" );
    }

    return OTF2_SUCCESS;
}


/** @brief Set the OTF2 version
 *
 *  @param archive  Archive handle.
 *  @param major    OTF2 version identifier
 *  @param minor    OTF2 version identifier
 *  @param bugfix   OTF2 version identifier
 *
 *  @threadsafety   Only called at archive open time.
 *
 *  @return         Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_version( OTF2_Archive* archive,
                          uint8_t       major,
                          uint8_t       minor,
                          uint8_t       bugfix )
{
    UTILS_ASSERT( archive );

    archive->version[ 0 ] = major;
    archive->version[ 1 ] = minor;
    archive->version[ 2 ] = bugfix;

    return OTF2_SUCCESS;
}


/** @brief Set the trace format version
 *         The trace format version is stored to the anchor file, to make format
 *         incompatibilities detectable.
 *
 *  @param archive  Archive handle.
 *  @param version  Trace format version identifier
 *
 *  @threadsafety   Only called at archive open time.
 *
 *  @return         Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_trace_format( OTF2_Archive* archive,
                               uint8_t       version )
{
    UTILS_ASSERT( archive );

    archive->trace_format = version;

    return OTF2_SUCCESS;
}


/** @brief Set the chunksize.
 *         The chunksize for written event traces must be set globally. Therefore
 *         this size must be saved to the archive management.
 *
 *  @param archive    Archive handle.
 *  @param chunkSize  Chunksize
 *
 *  @threadsafety     Only called at archive open time.
 *
 *  @return           Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_event_chunksize( OTF2_Archive* archive,
                                  uint64_t      chunkSize )
{
    UTILS_ASSERT( archive );

    if ( chunkSize == OTF2_UNDEFINED_UINT64 )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Can't allocate chunks with undefined length!" );
    }
    if ( archive->chunk_size_events != OTF2_UNDEFINED_UINT64 )
    {
        return UTILS_ERROR( OTF2_ERROR_PROCESSED_WITH_FAULTS,
                            "Set failed, because value is already set!" );
    }

    archive->chunk_size_events = chunkSize;

    return OTF2_SUCCESS;
}


/** @brief Set the chunksize.
 *         The chunksize for written event traces must be set globally. Therefore
 *         this size must be saved to the archive management.
 *
 *  @param archive    Archive handle.
 *  @param chunkSize  Chunksize
 *
 *  @threadsafety     Locks the archive.
 *
 *  @return           Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_def_chunksize( OTF2_Archive* archive,
                                uint64_t      chunkSize )
{
    UTILS_ASSERT( archive );

    if ( chunkSize == OTF2_UNDEFINED_UINT64 )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Can't allocate chunks with undefined length!" );
    }
    if ( archive->chunk_size_defs != OTF2_UNDEFINED_UINT64 )
    {
        return UTILS_ERROR( OTF2_ERROR_PROCESSED_WITH_FAULTS,
                            "Set failed, because value is already set!" );
    }

    archive->chunk_size_defs = chunkSize;

    return OTF2_SUCCESS;
}


/** @brief Set the name of the machine where the trace was recorded. This is optional!
 *
 *  @param archive      Archive handle.
 *  @param machineName  Machine name.
 *
 *  @threadsafety       Locks the archive.
 *
 *  @return             Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_machine_name( OTF2_Archive* archive,
                               const char*   machineName )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( machineName );

    OTF2_ARCHIVE_LOCK( archive );

    /* Release previous machine name */
    if ( archive->machine_name )
    {
        free( archive->machine_name );
    }

    /* Make a copy of the string */
    archive->machine_name = UTILS_CStr_dup( machineName );
    if ( !archive->machine_name )
    {
        OTF2_ARCHIVE_UNLOCK( archive );
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Allocation failed!" );
    }

    OTF2_ARCHIVE_UNLOCK( archive );
    return OTF2_SUCCESS;
}


/** @brief Set the description of the trace. This is optional!
 *
 *  @param archive      Archive handle.
 *  @param description  Description of the performance experiment which is stored in this trace.
 *
 *  @threadsafety       Locks the archive.
 *
 *  @return             Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_description( OTF2_Archive* archive,
                              const char*   description )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( description );

    OTF2_ARCHIVE_LOCK( archive );

    /* Release previous description */
    if ( archive->description )
    {
        free( archive->description );
    }

    /* Make a copy of the string */
    archive->description = UTILS_CStr_dup( description );
    if ( !archive->description )
    {
        OTF2_ARCHIVE_UNLOCK( archive );
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Allocation failed!" );
    }

    OTF2_ARCHIVE_UNLOCK( archive );
    return OTF2_SUCCESS;
}


/** @brief To determine who has recorded the trace. This is optional!
 *
 *  @param archive  Archive handle.
 *  @param creator  Creator of the OTF2 trace archive.
 *
 *  @threadsafety       Locks the archive.
 *
 *  @return         Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_creator( OTF2_Archive* archive,
                          const char*   creator )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( creator );

    OTF2_ARCHIVE_LOCK( archive );

    /* Release previous creator */
    if ( archive->creator )
    {
        free( archive->creator );
    }

    /* Make a copy of the string */
    archive->creator = UTILS_CStr_dup( creator );
    if ( !archive->creator )
    {
        OTF2_ARCHIVE_UNLOCK( archive );
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Allocation failed!" );
    }

    OTF2_ARCHIVE_UNLOCK( archive );
    return OTF2_SUCCESS;
}


/** @brief Set the number locations.
 *
 *  @param archive         Archive handle.
 *  @param locationNumber  Number of locations.
 *
 *  @threadsafety          Only called at archive open time.
 *
 *  @return                Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_number_of_locations( OTF2_Archive* archive,
                                      uint64_t      numberOfLocations )
{
    UTILS_ASSERT( archive );

    archive->number_of_locations = numberOfLocations;

    return OTF2_SUCCESS;
}


/** @brief Set the number of global definition records, which are stored in this trace.
 *         This can later be used for progress calculation.
 *
 *  @param archive                 Archive handle.
 *  @param globalDefRecordsNumber  Number of global definition records.
 *
 *  @threadsafety                  Only called at archive open time.
 *
 *  @return                        Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_number_of_global_defs( OTF2_Archive* archive,
                                        uint64_t      numberOfGlobalDefs )
{
    UTILS_ASSERT( archive );

    archive->number_of_global_defs = numberOfGlobalDefs;

    return OTF2_SUCCESS;
}


/** @brief Set the interaction mode (reading, writing, modification)
 *
 *  @param archive   Archive handle.
 *  @param fileMode  Reading, writing or modification mode?
 *
 *  @threadsafety    Only called at archive open time.
 *
 *  @return          Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_file_mode( OTF2_Archive* archive,
                            OTF2_FileMode fileMode )
{
    UTILS_ASSERT( archive );

    /* Check that a possible value is passed as argument */
    if ( !( ( fileMode == OTF2_FILEMODE_READ ) ||
            ( fileMode == OTF2_FILEMODE_WRITE ) ||
            ( fileMode == OTF2_FILEMODE_MODIFY ) )
         )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Filemode argument is not valid!" );
    }

    if ( archive->file_mode != OTF2_UNDEFINED_TYPE )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Filemode is already set!" );
    }

    archive->file_mode = fileMode;

    return OTF2_SUCCESS;
}


/** @brief Set the file substrate (posix, sion, none)
 *
 *  @param archive    Archive handle.
 *  @param substrate  Used file substrate: OTF2_SUBSTRATE_POSIX,
 *                    OTF2_SUBSTRATE_SION, or OTF2_SUBSTRATE_NONE
 *
 *  @threadsafety     Only called at archive open time.
 *
 *  @return           Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_set_file_substrate( OTF2_Archive*      archive,
                                 OTF2_FileSubstrate substrate )
{
    UTILS_ASSERT( archive );


    /* File substrate can always given only once */
    if ( archive->substrate != OTF2_SUBSTRATE_UNDEFINED )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Substrate is already set!" );
    }

    /* Check that a possible value is passed as argument */
    if ( !( ( substrate == OTF2_SUBSTRATE_POSIX ) ||
            ( substrate == OTF2_SUBSTRATE_SION ) ||
            ( substrate == OTF2_SUBSTRATE_NONE ) )
         )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "The substrate is not correct!" );
    }

    OTF2_ErrorCode ret = otf2_file_substrate_initialize( archive, substrate );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not initialize file substrate" );
    }

    archive->substrate = substrate;

    return OTF2_SUCCESS;
}


/** @brief Set the compression (none, zlib)
 *
 *  @param archive      Archive handle.
 *  @param compression  Compression mode: OTF2_COMPRESSION_NONE or
 *                      OTF2_COMPRESSION_ZLIB.
 *
 *  @threadsafety       Only called at archive open time.
 *
 *  @return             Returns an error code if something was going wrong.
 */

OTF2_ErrorCode
otf2_archive_set_compression( OTF2_Archive*    archive,
                              OTF2_Compression compression )
{
    UTILS_ASSERT( archive );

    /* Compression can always given only once */
    if ( archive->compression != OTF2_COMPRESSION_UNDEFINED )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Compression is already set!" );
    }

    /* Check that a possible value is passed as argument */
    if ( !( ( compression == OTF2_COMPRESSION_NONE ) ||
            ( compression == OTF2_COMPRESSION_ZLIB ) )
         )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid compression mode!" );
    }

    archive->compression = compression;

    return OTF2_SUCCESS;
}


/**
 *  @threadsafety  Locks the archive.
 */
void
otf2_archive_set_flush_callbacks( OTF2_Archive*              archive,
                                  const OTF2_FlushCallbacks* flushCallbacks,
                                  void*                      flushData )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( flushCallbacks );
    UTILS_ASSERT( archive->file_mode == OTF2_FILEMODE_WRITE );

    OTF2_ARCHIVE_LOCK( archive );

    archive->flush_callbacks = flushCallbacks;
    archive->flush_data      = flushData;

    OTF2_ARCHIVE_UNLOCK( archive );
}


/**
 *  @threadsafety  Locks the archive.
 */
void
otf2_archive_set_memory_callbacks( OTF2_Archive*               archive,
                                   const OTF2_MemoryCallbacks* memoryCallbacks,
                                   void*                       memoryData )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( memoryCallbacks );
    UTILS_ASSERT( memoryCallbacks->otf2_allocate );
    UTILS_ASSERT( memoryCallbacks->otf2_free_all );
    UTILS_ASSERT( archive->file_mode == OTF2_FILEMODE_WRITE );

    OTF2_ARCHIVE_LOCK( archive );
    archive->allocator_callbacks = memoryCallbacks;
    archive->allocator_data      = memoryData;
    OTF2_ARCHIVE_UNLOCK( archive );
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_set_collective_callbacks( OTF2_Archive*                   archive,
                                       const OTF2_CollectiveCallbacks* collectiveCallbacks,
                                       void*                           collectiveData,
                                       OTF2_CollectiveContext*         globalCommContext,
                                       OTF2_CollectiveContext*         localCommContext )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( collectiveCallbacks );

    if ( archive->collective_callbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Setting the collective context is only allowed once." );
    }

    OTF2_ARCHIVE_LOCK( archive );

    archive->collective_callbacks = collectiveCallbacks;
    archive->collective_data      = collectiveData;
    archive->global_comm_context  = globalCommContext;
    archive->local_comm_context   = localCommContext;

    OTF2_ErrorCode status;

    /* It is time to create the directories by the root rank. */
    if ( archive->file_mode == OTF2_FILEMODE_WRITE )
    {
        if ( otf2_archive_is_master( archive ) )
        {
            status = otf2_archive_create_directory( archive );
        }
        OTF2_CallbackCode callback_ret =
            otf2_collectives_bcast_error( archive,
                                          archive->global_comm_context,
                                          &status,
                                          OTF2_COLLECTIVES_ROOT );
        if ( OTF2_CALLBACK_SUCCESS != callback_ret )
        {
            status = UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                                  "Can't broadcast failed for result of creating the directories." );
            goto out;
        }
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status, "Couldn't create directories on root." );
            goto out;
        }
    }

    /* Now it is time to actually open the archive from the substrate perspective. */
    status = otf2_file_substrate_open( archive,
                                       archive->file_mode );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


OTF2_ErrorCode
otf2_archive_set_locking_callbacks( OTF2_Archive*                archive,
                                    const OTF2_LockingCallbacks* lockingCallbacks,
                                    void*                        lockingData )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( lockingCallbacks );

    if ( archive->locking_callbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Setting the collective context is only allowed once." );
    }

    archive->locking_callbacks = lockingCallbacks;
    archive->locking_data      = lockingData;

    OTF2_ErrorCode status = otf2_lock_create( archive, &archive->lock );
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status, "Can't create archive lock." );
    }

    return OTF2_SUCCESS;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_set_property( OTF2_Archive* archive,
                           const char*   name,
                           const char*   value,
                           bool          overwrite )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( name );
    UTILS_ASSERT( value );

    OTF2_ARCHIVE_LOCK( archive );

    OTF2_ErrorCode status;

    bool remove_property = false;

    /* Validate property name syntax */
    const char* name_check_pos = name;
    const char* name_sep       = "";
    do
    {
        name_sep = strstr( name_check_pos, "::" );
        if ( !name_sep )
        {
            /* If this was the first iteration and we did not found a namespace
             * separator, than we fail */
            if ( name_check_pos == name )
            {
                status = UTILS_ERROR( OTF2_ERROR_PROPERTY_NAME_INVALID,
                                      "Property name does not contain a namespace "
                                      "separator." );
                goto out;
            }
            /* Last name component expands till the end of the string */
            name_sep = name_check_pos + strlen( name_check_pos );
        }

        /* Check if the name component has a none-zero length */
        size_t name_len = name_sep - name_check_pos;
        if ( name_len == 0 )
        {
            status = UTILS_ERROR( OTF2_ERROR_PROPERTY_NAME_INVALID,
                                  "Namespace component of property name is empty." );
            goto out;
        }

        /* Check for valid characters in the name component */
        if ( strspn( name_check_pos, VALID_PROPERTY_NAME_CHARS ) != name_len )
        {
            status = UTILS_ERROR( OTF2_ERROR_PROPERTY_NAME_INVALID,
                                  "Property name contains invalid characters. "
                                  "Please use only [A-Z0-9_]: '%.*s'",
                                  ( int )name_len, name_check_pos );
            goto out;
        }

        /* Skip :: and advance check position */
        name_check_pos = name_sep + 2;
    }
    while ( *name_sep );

    /* Convert property name to upper case */
    char* property_name = UTILS_CStr_dup( name );
    for ( char* ptr = property_name; *ptr; ptr++ )
    {
        *ptr = toupper( *ptr );
    }

    /* Removing a trace file property is done by passing "" in the
    * value parameter. The overwrite parameter is ignored than. */
    if ( strlen( value ) == 0 )
    {
        remove_property = true;
    }

    if ( archive->properties == NULL )
    {
        /* Set head element of list */

        if ( remove_property )
        {
            free( property_name );
            status = OTF2_SUCCESS;
            goto out;
        }

        archive->properties = malloc( sizeof( otf2_archive_property ) );
        if ( !archive->properties )
        {
            free( property_name );
            status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
            goto out;
        }

        archive->properties->name  = property_name;
        archive->properties->value = UTILS_CStr_dup( value );
        archive->properties->next  = archive->properties->previous  = NULL;
        archive->number_of_properties++;
    }
    else
    {
        /* At least one element in list */

        otf2_archive_property* current_property = archive->properties;
        while ( true )
        {
            /* Look for an already existing element with the same name */
            if ( strcmp( current_property->name, property_name ) == 0 )
            {
                /* Already existing element with same name found */
                if ( remove_property )
                {
                    /* Free memory for strings */
                    free( current_property->name );
                    free( current_property->value );

                    if ( current_property == archive->properties )
                    {
                        /* Remove first element in list */
                        if ( current_property->next == NULL )
                        {
                            /* It's the one and only element in list */
                            archive->properties = NULL;
                        }
                        else
                        {
                            archive->properties           = current_property->next;
                            archive->properties->previous = NULL;
                        }
                    }
                    else if ( current_property->next == NULL )
                    {
                        /* Remove last element in list */
                        current_property->previous->next = NULL;
                    }
                    else
                    {
                        /* Free element in the middle of the list */
                        current_property->previous->next = current_property->next;
                        current_property->next->previous = current_property->previous;
                    }
                    free( current_property );
                    free( property_name );
                    archive->number_of_properties--;
                    status = OTF2_SUCCESS;
                    goto out;
                }

                /* Property is already available */
                if ( overwrite )
                {
                    /* Delete old value */
                    free( current_property->value );
                    /* Write new value */
                    current_property->value = UTILS_CStr_dup( value );

                    free( property_name );
                    status = OTF2_SUCCESS;
                    goto out;
                }

                /* Return with error code if property is already available but should not be overwritten */
                free( property_name );
                status = OTF2_ERROR_PROPERTY_EXISTS;
                goto out;
            }

            /* Continue with next element in list */
            if ( current_property->next == NULL )
            {
                break;
            }
            else
            {
                current_property = current_property->next;
            }
        }

        if ( remove_property )
        {
            /* Property requested to remove but not found. */
            free( property_name );
            status = OTF2_ERROR_PROPERTY_NOT_FOUND;
            goto out;
        }

        /* Property is not available in list yet, add new one */
        current_property->next = malloc( sizeof( otf2_archive_property ) );
        if ( !current_property->next )
        {
            status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
            goto out;
        }

        current_property->next->name     = property_name;
        current_property->next->value    = UTILS_CStr_dup( value );
        current_property->next->next     = NULL;
        current_property->next->previous = current_property;
        archive->number_of_properties++;
    }

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Set the identifier of the trace file.
 *
 *  @param archive  Archive handle.
 *  @param id       Trace identifier.
 *
 *  @threadsafety   Only called at archive open time.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_set_trace_id( OTF2_Archive* archive,
                           uint64_t      id )
{
    UTILS_ASSERT( archive );

    archive->trace_id = id;

    return OTF2_SUCCESS;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_set_number_of_snapshots( OTF2_Archive* archive,
                                      uint32_t      number )
{
    UTILS_ASSERT( archive );

    OTF2_ARCHIVE_LOCK( archive );
    archive->number_of_snapshots = number;
    OTF2_ARCHIVE_UNLOCK( archive );

    return OTF2_SUCCESS;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_set_number_of_thumbnails( OTF2_Archive* archive,
                                       uint32_t      numberOfThumbnails )
{
    UTILS_ASSERT( archive );

    OTF2_ARCHIVE_LOCK( archive );
    archive->number_of_thumbnails = numberOfThumbnails;
    OTF2_ARCHIVE_UNLOCK( archive );

    return OTF2_SUCCESS;
}


/*__get_functions__________________________________________________*/


/** @brief Get the OTF2 version. This was hopefully set by
 *         otf2_archive_set_version.
 *
 *  @param archive  Archive handle.
 *  @param major    OTF2 version identifier
 *  @param minor    OTF2 version identifier
 *  @param bugfix   OTF2 version identifier
 *
 *  @threadsafety   Constant since archive open time.
 *
 *  @return         Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_version( OTF2_Archive* archive,
                          uint8_t*      major,
                          uint8_t*      minor,
                          uint8_t*      bugfix )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( major );
    UTILS_ASSERT( minor );
    UTILS_ASSERT( bugfix );

    /* Can't be set by user, thus no race when reading. */
    *major  = archive->version[ 0 ];
    *minor  = archive->version[ 1 ];
    *bugfix = archive->version[ 2 ];

    return OTF2_SUCCESS;
}


/** @brief Get the trace format version. This was hopefully set by
 *         otf2_archive_set_trace_format.
 *
 *  @param archive  Archive handle.
 *  @param version  Trace format version identifier
 *
 *  @threadsafety   Constant since archive open time.
 *
 *  @return         Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_trace_format( OTF2_Archive* archive,
                               uint8_t*      version )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( version );

    /* Can't be set by user, thus no race when reading. */
    *version = archive->trace_format;

    return OTF2_SUCCESS;
}


/** @brief Get the chunksize.
 *         The chunksize for written event traces must be set globally. Therefore
 *         this size must be saved to the archive management.
 *
 *  @param archive    Archive handle.
 *  @param chunksize  Chunksize
 *
 *  @threadsafety     Constant since archive open time.
 *
 *  @return           Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_event_chunksize( OTF2_Archive* archive,
                                  uint64_t*     chunkSize )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( chunkSize );

    /* Check if this flag was already set. If not return an error */
    if ( archive->chunk_size_events == OTF2_UNDEFINED_UINT64 )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Event chunk size not yet   set!" );
    }

    /* Can't be set by user after open, thus no race when reading. */
    *chunkSize = archive->chunk_size_events;

    return OTF2_SUCCESS;
}


/** @brief Get the chunksize.
 *         The chunksize for written event traces must be set globally. Therefore
 *         this size must be saved to the archive management.
 *
 *  @param archive    Archive handle.
 *  @param chunksize  Chunksize
 *
 *  @threadsafety     Caller holds the archive lock.
 *
 *  @return           Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_def_chunksize( OTF2_Archive* archive,
                                uint64_t*     chunkSize )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( chunkSize );

    /* Check if this flag was already set. If not return an error */
    if ( archive->chunk_size_defs == OTF2_UNDEFINED_UINT64 )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Definition chunk size not yet set!" );
    }

    *chunkSize = archive->chunk_size_defs;

    return OTF2_SUCCESS;
}


/** @brief Get the name of the machine where the trace was recorded. This is optional!
 *
 *  @param archive      Archive handle.
 *  @param machineName  Name of the machine the trace was created.
 *
 *  @threadsafety       Locks the archive.
 *
 *  @return             Returns an error code if something was going wrong,
 *                      or there was no name set before.
 */

OTF2_ErrorCode
otf2_archive_get_machine_name( OTF2_Archive* archive,
                               char**        machineName )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( machineName );

    /* Check if there is no name set, return an error */
    if ( !archive->machine_name )
    {
        return OTF2_ERROR_MEM_FAULT;
    }

    /* Make a copy of the string */
    OTF2_ARCHIVE_LOCK( archive );
    *machineName = UTILS_CStr_dup( archive->machine_name );
    if ( !*machineName )
    {
        OTF2_ARCHIVE_UNLOCK( archive );
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
    }

    OTF2_ARCHIVE_UNLOCK( archive );
    return OTF2_SUCCESS;
}


/** @brief Get the description of the trace. This is optional!
 *
 *  @param archive      Archive handle.
 *  @param description  Description of the performance experiment which is stored in this trace.
 *
 *  @threadsafety       Locks the archive.
 *
 *  @return             Returns an error code if something was going wrong,
 *                      or there was no name set before.
 */

OTF2_ErrorCode
otf2_archive_get_description( OTF2_Archive* archive,
                              char**        description )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( description );

    /* Check if there is no description set, return an error */
    if ( !archive->description )
    {
        return OTF2_ERROR_MEM_FAULT;
    }

    /* Make a copy of the string */
    OTF2_ARCHIVE_LOCK( archive );
    *description = UTILS_CStr_dup( archive->description );
    if ( !*description )
    {
        OTF2_ARCHIVE_UNLOCK( archive );
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
    }

    OTF2_ARCHIVE_UNLOCK( archive );
    return OTF2_SUCCESS;
}


/** @brief Get the name of the creator to determine who has recorded the trace.
 *         This is optional!
 *
 *  @param archive  Archive handle.
 *  @param creator  Place for the name of the author of the trace
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns an error code if something was going wrong,
 *                  or there was no name set before.
 */

OTF2_ErrorCode
otf2_archive_get_creator( OTF2_Archive* archive,
                          char**        creator )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( creator );

    /* Check if there is no name set, return an error */
    if ( !archive->creator )
    {
        return OTF2_ERROR_MEM_FAULT;
    }

    /* Make a copy of the string */
    OTF2_ARCHIVE_LOCK( archive );
    *creator = UTILS_CStr_dup( archive->creator );
    if ( !*creator )
    {
        OTF2_ARCHIVE_UNLOCK( archive );
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
    }

    OTF2_ARCHIVE_UNLOCK( archive );
    return OTF2_SUCCESS;
}


/** @brief Get the number of locations.
 *
 *  @param archive         Archive handle.
 *  @param locationNumber  Number of global definition records.
 *
 *  @threadsafety          Locks the archive.
 *
 *  @return                Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_number_of_locations( OTF2_Archive* archive,
                                      uint64_t*     numberOfLocations )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( numberOfLocations );

    OTF2_ARCHIVE_LOCK( archive );
    *numberOfLocations = archive->number_of_locations;
    OTF2_ARCHIVE_UNLOCK( archive );

    return OTF2_SUCCESS;
}


/** @brief Get the number of global definition records, which are stored in this trace.
 *         This can be used for progress calculation.
 *
 *  @param archive                 Archive handle.
 *  @param globalDefRecordsNumber  Number of global definition records.
 *
 *  @threadsafety                  Locks the archive.
 *
 *  @return                        Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_number_of_global_defs( OTF2_Archive* archive,
                                        uint64_t*     numberOfGlobalDefs )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( numberOfGlobalDefs );

    OTF2_ARCHIVE_LOCK( archive );
    *numberOfGlobalDefs = archive->number_of_global_defs;
    OTF2_ARCHIVE_UNLOCK( archive );

    return OTF2_SUCCESS;
}


/** @brief Get the file substrate (posix, sion, none)
 *
 *  @param archive    Archive handle.
 *  @param substrate  Return pointer to file substrate:
 *                    OTF2_SUBSTRATE_POSIX, OTF2_SUBSTRATE_SION,
 *                    or OTF2_SUBSTRATE_NONE
 *
 *  @threadsafety     Constant since archive open time.
 *
 *  @return           Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_file_substrate( OTF2_Archive*       archive,
                                 OTF2_FileSubstrate* substrate )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( substrate );

    /* Check if this flag was already set. If not return an error */
    if ( archive->substrate == OTF2_SUBSTRATE_UNDEFINED )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Value is not set!" );
    }

    /* Check if the state of this flag has a valid state */
    if ( !( ( archive->substrate == OTF2_SUBSTRATE_POSIX ) ||
            ( archive->substrate == OTF2_SUBSTRATE_SION ) ||
            ( archive->substrate == OTF2_SUBSTRATE_NONE ) )
         )
    {
        return UTILS_ERROR( OTF2_ERROR_INDEX_OUT_OF_BOUNDS,
                            "State is not valid!" );
    }

    /* Copy the actual state */
    *substrate = archive->substrate;

    return OTF2_SUCCESS;
}


/** @brief Get compression mode (none or zlib)
 *
 *  @param archive      Archive handle.
 *  @param compression  Return pointer to compression mode.
 *
 *  @threadsafety       Constant since archive open time.
 *
 *  @return             Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_compression( OTF2_Archive*     archive,
                              OTF2_Compression* compression )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( compression );

    /* Check if this flag was already set. If not return an error */
    if ( archive->compression == OTF2_COMPRESSION_UNDEFINED )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Value is not set!" );
    }

    /* Check if the state of this flag has a valid state */
    if ( !( ( archive->compression == OTF2_COMPRESSION_NONE ) ||
            ( archive->compression == OTF2_COMPRESSION_ZLIB ) )
         )
    {
        return UTILS_ERROR( OTF2_ERROR_INDEX_OUT_OF_BOUNDS,
                            "State is not valid!" );
    }

    /* Copy the actual state */
    *compression = archive->compression;

    return OTF2_SUCCESS;
}


/** @brief Get the interaction mode (reading, writing, modification)
 *
 *  @param archive    Archive handle.
 *  @param fileMode   Reading, writing or modification mode?
 *
 *  @threadsafety     Constant since archive open time.
 *
 *  @return           Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_file_mode( OTF2_Archive*  archive,
                            OTF2_FileMode* fileMode )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( fileMode );

    /* Check if this flag was already set. If not return an error */
    if ( archive->file_mode == OTF2_UNDEFINED_TYPE )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Value is not set!" );
    }

    /* Check if the state of this flag has a valid state */
    if ( !( ( archive->file_mode == OTF2_FILEMODE_READ ) ||
            ( archive->file_mode == OTF2_FILEMODE_WRITE ) ||
            ( archive->file_mode == OTF2_FILEMODE_MODIFY ) )
         )
    {
        return UTILS_ERROR( OTF2_ERROR_INDEX_OUT_OF_BOUNDS,
                            "State is not valid!" );
    }

    /* Copy the actual state */
    *fileMode = archive->file_mode;

    return OTF2_SUCCESS;
}


/** @brief Get the archive path.
 *
 *  @param archive      Archive handle.
 *  @param archivePath  Path to the archive
 *
 *  @threadsafety       Constant since archive open time.
 *
 *  @return             Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_archive_path( OTF2_Archive* archive,
                               char**        archivePath )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( archivePath );

    /* Copy archive name */
    *archivePath = UTILS_CStr_dup( archive->archive_path );
    if ( !*archivePath )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
    }

    return OTF2_SUCCESS;
}


/** @brief Get the path to the trace files (ie. per location files).
 *
 *  @param archive    Archive handle.
 *  @param tracePath  Path to the trace files
 *
 *  @threadsafety     Constant since archive open time.
 *
 *  @return           Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_trace_path( OTF2_Archive* archive,
                             char**        tracePath )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( tracePath );

    /* join the archive name and the file path to get the full path */
    *tracePath = UTILS_IO_JoinPath( 2,
                                    archive->archive_path,
                                    archive->archive_name );
    if ( !*tracePath )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
    }

    return OTF2_SUCCESS;
}


/** @brief Get the names of all trace file properties.
 *
 *  @param archive             Archive handle.
 *  @param numberOfProperties  Number of properties.
 *  @param names               Pointer to a string where the property name will
 *                             be stored, will be malloc'ed. Just pass *names to
 *                             free.
 *
 *  @threadsafety              Locks the archive.
 *
 *  @return                    Returns OTF2_SUCCESS if successful, an error code
 *                             if an error occurs.
 */

OTF2_ErrorCode
otf2_archive_get_property_names( OTF2_Archive* archive,
                                 uint32_t*     numberOfProperties,
                                 char***       names )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( numberOfProperties );
    UTILS_ASSERT( names );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    uint32_t               string_length_sum = 0;
    uint32_t               current_index     = 0;
    otf2_archive_property* property          = archive->properties;

    *numberOfProperties = archive->number_of_properties;
    if ( *numberOfProperties == 0 )
    {
        ( *names ) = NULL;
        status     = OTF2_SUCCESS;
        goto out;
    }

    while ( property != NULL )
    {
        if ( current_index >= *numberOfProperties )
        {
            status = OTF2_ERROR_INDEX_OUT_OF_BOUNDS;
            goto out;
        }

        string_length_sum += strlen( property->name ) + 1;

        property = property->next;
        current_index++;
    }

    /* Array of pointer to immediately following strings */
    char** name_array = malloc( *numberOfProperties * sizeof( char* ) + string_length_sum * sizeof( char ) );
    if ( name_array == NULL )
    {
        status = OTF2_ERROR_MEM_FAULT;
        goto out;
    }

    current_index = 0;
    property      = archive->properties;
    uint32_t string_start = *numberOfProperties * sizeof( char* );
    while ( property != NULL )
    {
        name_array[ current_index ] = ( char* )name_array + string_start;
        strcpy( ( char* )name_array + string_start, property->name );
        string_start += strlen( property->name ) + 1;

        property = property->next;
        current_index++;
    }

    *names = name_array;
    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Get the value of the named trace file property.
 *
 *  @param archive  Archive handle.
 *  @param name     Name of the property.
 *  @param value    Pointer to a string where the value will be stored, will be malloc'ed.
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */

OTF2_ErrorCode
otf2_archive_get_property( OTF2_Archive* archive,
                           const char*   name,
                           char**        value )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( name );
    UTILS_ASSERT( value );

    OTF2_ARCHIVE_LOCK( archive );

    otf2_archive_property* property = archive->properties;
    while ( property != NULL )
    {
        if ( strcmp( property->name, name ) == 0 )
        {
            *value = UTILS_CStr_dup( property->value );
            OTF2_ARCHIVE_UNLOCK( archive );
            return OTF2_SUCCESS;
        }

        property = property->next;
    }

    *value = NULL;

    OTF2_ARCHIVE_UNLOCK( archive );
    return OTF2_ERROR_PROPERTY_NOT_FOUND;
}


/** @brief Get the identifier of the trace file.
 *
 *  @note This call is only allowed when the archive was opened with mode
 *        OTF2_FILEMODE_READ.
 *
 *  @param archive  Archive handle.
 *  @param id       Trace identifier.
 *
 *  @threadsafety   Constant since archive open time or modified at close time.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_get_trace_id( OTF2_Archive* archive,
                           uint64_t*     id )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( id );

    *id = archive->trace_id;
    return OTF2_SUCCESS;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_get_number_of_snapshots( OTF2_Archive* archive,
                                      uint32_t*     number )
{
    UTILS_ASSERT( archive );

    OTF2_ARCHIVE_LOCK( archive );
    *number = archive->number_of_snapshots;
    OTF2_ARCHIVE_UNLOCK( archive );

    return OTF2_SUCCESS;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_get_number_of_thumbnails( OTF2_Archive* archive,
                                       uint32_t*     numberOfThumbnails )
{
    UTILS_ASSERT( archive );

    OTF2_ARCHIVE_LOCK( archive );
    *numberOfThumbnails = archive->number_of_thumbnails;
    OTF2_ARCHIVE_UNLOCK( archive );

    return OTF2_SUCCESS;
}


/** @brief The get_local_event_writer interface does internally lookup for
 *         already existing event writer instances (local_evt_writers)
 *         or if the requested writer was not requested before, it generates a
 *         new one for the requested location and puts it into the internal
 *         vector.
 *
 *  @param archive     Archive handle.
 *  @param writer      Returned local event writer object.
 *  @param locationID  Location id for which the writer is requested.
 *
 *  @threadsafety      Locks the archive.
 *
 *  @return            Returns OTF2_SUCCESS if successful, an error code
 *                     if an error occurs.
 */

OTF2_ErrorCode
otf2_archive_get_evt_writer( OTF2_Archive*    archive,
                             OTF2_LocationRef location,
                             OTF2_EvtWriter** writer )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( writer );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search writer in existing list */
    if ( location != OTF2_UNDEFINED_LOCATION )
    {
        for ( *writer = archive->local_evt_writers;
              *writer;
              *writer = ( *writer )->next )
        {
            if ( ( *writer )->location_id == location )
            {
                status = OTF2_SUCCESS;
                goto out;
            }
        }
    }

    /* Create new writer */
    *writer = otf2_evt_writer_new( archive, location );
    if ( !*writer )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create event writer!" );
        goto out;
    }

    /* Put new writer into the list of open writers */
    ( *writer )->next          = archive->local_evt_writers;
    archive->local_evt_writers = *writer;

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief  This interface gets the global definition writer, if the archive handle
 *          is in master mode.
 *
 *  @param archive  Archive handle.
 *  @param writer   Returned global definition writer object.
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */

OTF2_ErrorCode
otf2_archive_get_global_def_writer( OTF2_Archive*          archive,
                                    OTF2_GlobalDefWriter** writer )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( writer );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( !otf2_archive_is_master( archive ) )
    {
        status = OTF2_ERROR_INVALID_CALL;
        goto out;
    }

    if ( archive->global_def_writer )
    {
        *writer = archive->global_def_writer;
        status  = OTF2_SUCCESS;
        goto out;
    }

    archive->global_def_writer =
        otf2_global_def_writer_new( archive );

    if ( !archive->global_def_writer )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create global definition writer!" );
        goto out;
    }

    *writer = archive->global_def_writer;

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief The get_local_definition_writer interface does internally lookup for
 *         already existing definition writer instances (local_def_writers)
 *         or if the requested writer was not requested before, it generates a
 *         new one for the requested location and puts it into the internal
 *         vector.
 *
 *  @param archive   Archive handle.
 *  @param location  Location id for which the writer is requested.
 *  @param writer    Returned local definition writer object.
 *
 *  @threadsafety    Locks the archive.
 *
 *  @return          Returns OTF2_SUCCESS if successful, an error code
 *                   if an error occurs.
 */

OTF2_ErrorCode
otf2_archive_get_def_writer( OTF2_Archive*    archive,
                             OTF2_LocationRef location,
                             OTF2_DefWriter** writer )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( location != OTF2_UNDEFINED_LOCATION );
    UTILS_ASSERT( writer );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    *writer = NULL;

    /* Search writer in existing list */
    *writer = archive->local_def_writers;
    while ( *writer )
    {
        if ( ( *writer )->location_id == location )
        {
            status = OTF2_SUCCESS;
            goto out;
        }
        *writer = ( *writer )->next;
    }

    status = otf2_archive_add_location( archive, location );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not add location to archive!" );
        goto out;
    }

    /* Create new writer */
    *writer = otf2_def_writer_new( archive, location );
    if ( !*writer )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create definition writer!" );
        goto out;
    }

    /* Put new writer into the list of open writers */
    ( *writer )->next          = archive->local_def_writers;
    archive->local_def_writers = *writer;

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_get_snap_writer( OTF2_Archive*     archive,
                              OTF2_LocationRef  location,
                              OTF2_SnapWriter** writer )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( location != OTF2_UNDEFINED_LOCATION );
    UTILS_ASSERT( writer );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search writer in existing list */
    for ( *writer = archive->local_snap_writers;
          *writer;
          *writer = ( *writer )->next )
    {
        if ( ( *writer )->location_id == location )
        {
            status = OTF2_SUCCESS;
            goto out;
        }
    }

    status = otf2_archive_add_location( archive, location );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not add location to archive!" );
        goto out;
    }

    /* Create new writer */
    *writer = otf2_snap_writer_new( archive, location );

    if ( !*writer )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create snap writer!" );
        goto out;
    }

    /* Put new writer into the list of open writers */
    ( *writer )->next           = archive->local_snap_writers;
    archive->local_snap_writers = *writer;

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_get_thumb_writer( OTF2_Archive*      archive,
                               OTF2_ThumbWriter** writer,
                               const char*        name,
                               const char*        description,
                               OTF2_ThumbnailType type,
                               uint32_t           numberOfSamples,
                               uint32_t           numberOfMetrics,
                               const uint64_t*    refsToDefs )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( writer );
    UTILS_ASSERT( name );
    UTILS_ASSERT( description );
    UTILS_ASSERT( numberOfSamples > 0 );
    UTILS_ASSERT( numberOfMetrics > 0 );
    UTILS_ASSERT( refsToDefs );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    *writer = NULL;

    /* Create new writer */
    *writer = otf2_thumb_writer_new( archive, archive->number_of_thumbnails );
    if ( !*writer )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create definition writer!" );
        goto out;
    }

    /* write thumb header */
    status = otf2_thumb_writer_write_header( *writer,
                                             name,
                                             description,
                                             type,
                                             numberOfSamples,
                                             numberOfMetrics,
                                             refsToDefs );
    if ( status != OTF2_SUCCESS )
    {
        otf2_thumb_writer_delete( *writer );
        UTILS_ERROR( status, "Can't write thumbnail header." );
        goto out;
    }

    /* Put new writer into the list of open writers */
    ( *writer )->next      = archive->thumb_writers;
    archive->thumb_writers = *writer;

    archive->number_of_thumbnails++;

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief  This interface gets the marker writer, if the archive handle
 *          is in master mode.
 *
 *  @param archive  Archive handle.
 *  @param writer   Returned marker writer object.
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_get_marker_writer( OTF2_Archive*       archive,
                                OTF2_MarkerWriter** writer )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( writer );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( !archive->marker_writer )
    {
        archive->marker_writer = otf2_marker_writer_new( archive );
    }

    if ( !archive->marker_writer )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Marker writer creation failed!" );
        goto out;
    }

    *writer = archive->marker_writer;

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief The get_local_event_reader interface does internally lookup for
 *         already existing reader instances (local_evt_readers)
 *         or if the requested reader was not requested before, it generates a
 *         new one for the requested location and puts it into the internal
 *         vector.
 *
 *  @param archive    Archive handle.
 *  @param location   Location id for which the reader is requested.
 *  @param reader     Returned local event reader object.
 *
 *  @threadsafety     Locks the archive.
 *
 *  @return Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_evt_reader( OTF2_Archive*    archive,
                             OTF2_LocationRef location,
                             OTF2_EvtReader** reader )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( location != OTF2_UNDEFINED_LOCATION );
    UTILS_ASSERT( reader );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search reader in existing list */
    *reader = archive->local_evt_readers;
    while ( *reader )
    {
        if ( ( *reader )->location_id == location )
        {
            status = OTF2_SUCCESS;
            goto out;
        }
        *reader = ( *reader )->next;
    }

    /* OTF2 1.2 -> 1.3 transitional fallback for pure POSIX reader */
    if ( archive->substrate == OTF2_SUBSTRATE_POSIX )
    {
        status = otf2_archive_add_location( archive, location );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status, "Can't auto-select location!" );
            goto out;
        }
    }

    /* Create new reader */
    *reader = otf2_evt_reader_new( archive, location );
    if ( !*reader )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create event reader!" );
        goto out;
    }

    /* Put new reader into the list of open readers */
    ( *reader )->next          = archive->local_evt_readers;
    archive->local_evt_readers = *reader;
    archive->number_of_evt_readers++;

    OTF2_ARCHIVE_UNLOCK( archive );

    /* We open the file outside of the archive lock to be able to parallel this. */
    return otf2_evt_reader_open_file( *reader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_get_global_evt_reader( OTF2_Archive*          archive,
                                    OTF2_GlobalEvtReader** reader )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( reader );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( !archive->global_evt_reader )
    {
        archive->global_evt_reader = otf2_global_evt_reader_new( archive );
    }

    if ( !archive->global_evt_reader )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create global event reader!" );
        goto out;
    }

    *reader = archive->global_evt_reader;

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief The get_local_definition_reader interface does internally lookup for
 *         already existing reader instances (local_def_readers)
 *         or if the requested reader was not requested before, it generates a
 *         new one for the requested location and puts it into the internal
 *         vector.
 *
 *  @param archive   Archive handle.
 *  @param location  Location id for which the reader is requested.
 *  @param reader    Returned local definition reader object.
 *
 *  @threadsafety    Locks the archive.
 *
 *  @return Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_def_reader( OTF2_Archive*    archive,
                             OTF2_LocationRef location,
                             OTF2_DefReader** reader )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( location != OTF2_UNDEFINED_LOCATION );
    UTILS_ASSERT( reader );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search reader in existing list */
    *reader = archive->local_def_readers;
    while ( *reader )
    {
        if ( ( *reader )->location_id == location )
        {
            status = OTF2_SUCCESS;
            goto out;
        }
        *reader = ( *reader )->next;
    }

    /* OTF2 1.2 -> 1.3 transitional fallback for pure POSIX reader */
    if ( archive->substrate == OTF2_SUBSTRATE_POSIX )
    {
        status = otf2_archive_add_location( archive, location );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status, "Can't auto-select location!" );
            goto out;
        }
    }

    /* Create new reader */
    *reader = otf2_def_reader_new( archive, location );
    if ( !*reader )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create definition reader!" );
        goto out;
    }

    /* Put new reader into the list of open readers */
    ( *reader )->next          = archive->local_def_readers;
    archive->local_def_readers = *reader;

    OTF2_ARCHIVE_UNLOCK( archive );

    /* We open the file outside of the archive lock to be able to parallel this. */
    return otf2_def_reader_open_file( *reader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_get_global_def_reader( OTF2_Archive*          archive,
                                    OTF2_GlobalDefReader** reader )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( reader );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( archive->global_def_reader )
    {
        *reader = archive->global_def_reader;
        status  = OTF2_SUCCESS;
        goto out;
    }

    /* Need to create the global def reader */

    archive->global_def_reader = otf2_global_def_reader_new( archive );
    if ( !archive->global_def_reader )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Global definition reader creation failed!" );
        goto out;
    }

    *reader = archive->global_def_reader;

    OTF2_ARCHIVE_UNLOCK( archive );

    /* We open the file outside of the archive lock to be able to parallel this. */
    return otf2_global_def_reader_open_file( *reader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief The get_local_event_reader interface does internally lookup for
 *         already existing reader instances (local_evt_readers)
 *         or if the requested reader was not requested before, it generates a
 *         new one for the requested location and puts it into the internal
 *         vector.
 *
 *  @param archive   Archive handle.
 *  @param location  Location id for which the reader is requested.
 *  @param reader    Returned local event reader object.
 *
 *  @threadsafety    Locks the archive.
 *
 *  @return          Returns an error code if something was going wrong
 */

OTF2_ErrorCode
otf2_archive_get_snap_reader( OTF2_Archive*     archive,
                              OTF2_LocationRef  location,
                              OTF2_SnapReader** reader )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( location != OTF2_UNDEFINED_LOCATION );
    UTILS_ASSERT( reader );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search reader in existing list */
    *reader = archive->local_snap_readers;
    while ( *reader )
    {
        if ( ( *reader )->location_id == location )
        {
            status = OTF2_SUCCESS;
            goto out;
        }
        *reader = ( *reader )->next;
    }

    /* Create new reader */
    *reader = otf2_snap_reader_new( archive, location );
    if ( !*reader )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create snap reader!" );
        goto out;
    }

    /* Put new reader into the list of open readers */
    ( *reader )->next           = archive->local_snap_readers;
    archive->local_snap_readers = *reader;
    archive->number_of_snap_readers++;

    OTF2_ARCHIVE_UNLOCK( archive );

    /* We open the file outside of the archive lock to be able to parallel this. */
    return otf2_snap_reader_open_file( *reader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_get_global_snap_reader( OTF2_Archive*           archive,
                                     OTF2_GlobalSnapReader** reader )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( reader );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( !archive->global_snap_reader )
    {
        archive->global_snap_reader = otf2_global_snap_reader_new( archive );
    }

    if ( !archive->global_snap_reader )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create global snap reader!" );
        goto out;
    }

    *reader = archive->global_snap_reader;

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_get_thumb_reader( OTF2_Archive*      archive,
                               uint32_t           thumbID,
                               OTF2_ThumbReader** reader )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( reader );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search reader in existing list */
    *reader = archive->thumb_readers;
    while ( *reader )
    {
        if ( ( *reader )->thumb_id == thumbID )
        {
            status = OTF2_SUCCESS;
            goto out;
        }
        *reader = ( *reader )->next;
    }

    /* Create new reader */
    *reader = otf2_thumb_reader_new( archive, thumbID );
    if ( !*reader )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Can't create thumb reader!" );
        goto out;
    }

    /* Put new reader into the list of open readers */
    ( *reader )->next      = archive->thumb_readers;
    archive->thumb_readers = *reader;

    OTF2_ARCHIVE_UNLOCK( archive );

    /* We open the file outside of the archive lock to be able to parallel this. */
    return otf2_thumb_reader_open_file( *reader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_get_marker_reader( OTF2_Archive*       archive,
                                OTF2_MarkerReader** reader )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( reader );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( archive->marker_reader )
    {
        *reader = archive->marker_reader;
        status  = OTF2_SUCCESS;
        goto out;
    }

    /* Need to create the marker reader. */

    archive->marker_reader = otf2_marker_reader_new( archive );
    if ( !archive->marker_reader )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                              "Marker reader creation failed!" );
        goto out;
    }

    *reader = archive->marker_reader;

    OTF2_ARCHIVE_UNLOCK( archive );

    /* We open the file outside of the archive lock to be able to parallel this. */
    return otf2_marker_reader_open_file( *reader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/* ___ Close functions for local reader and writer. _________________________ */


/** @brief Close an opened local event writer.
 *
 *  @param archive  Archive handle.
 *  @param writer   Writer handle to be closed.
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_evt_writer( OTF2_Archive*   archive,
                               OTF2_EvtWriter* writer )
{
    UTILS_ASSERT( archive );

    if ( writer == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search the writer in the list of open writers. */
    OTF2_EvtWriter** writer_it;
    for ( writer_it = &archive->local_evt_writers;
          *writer_it;
          writer_it = &( *writer_it )->next )
    {
        if ( *writer_it == writer )
        {
            break;
        }
    }
    if ( !*writer_it )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Can't find event writer." );
        goto out;
    }
    /* Remove the writer from the list */
    *writer_it = writer->next;

    /* Close event writer. */
    status = otf2_evt_writer_delete( writer );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Close an opened local definition writer.
 *
 *  @param archive  Archive handle.
 *  @param writer   Writer handle to be closed.
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_def_writer( OTF2_Archive*   archive,
                               OTF2_DefWriter* writer )
{
    UTILS_ASSERT( archive );

    if ( writer == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search the writer in the list of open writers. */
    OTF2_DefWriter** writer_it;
    for ( writer_it = &archive->local_def_writers;
          *writer_it;
          writer_it = &( *writer_it )->next )
    {
        if ( *writer_it == writer )
        {
            break;
        }
    }
    if ( !*writer_it )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Can't find definition writer." );
        goto out;
    }
    /* Remove the writer from the list */
    *writer_it = writer->next;

    /* Close definition writer. */
    status = otf2_def_writer_delete( writer );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Close an opened global definition writer.
 *
 *  @param archive  Archive handle.
 *  @param writer   Writer handle to be closed.
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_global_def_writer( OTF2_Archive*         archive,
                                      OTF2_GlobalDefWriter* writer )
{
    UTILS_ASSERT( archive );

    if ( writer == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( archive->file_mode != OTF2_FILEMODE_WRITE ||
         !otf2_archive_is_master( archive ) )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                              "Archive is not the master." );
        goto out;
    }

    if ( archive->global_def_writer != writer )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "The writer object does not match with that of this archive." );
        goto out;
    }

    archive->global_def_writer = NULL;

    /* Close global definition writer. */
    status = otf2_global_def_writer_delete( writer );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Close an opened local snap writer.
 *
 *  @param archive  Archive handle.
 *  @param writer   Writer handle to be closed.
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_snap_writer( OTF2_Archive*    archive,
                                OTF2_SnapWriter* writer )
{
    UTILS_ASSERT( archive );

    if ( writer == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search the writer in the list of open writers. */
    OTF2_SnapWriter** writer_it;
    for ( writer_it = &archive->local_snap_writers;
          *writer_it;
          writer_it = &( *writer_it )->next )
    {
        if ( *writer_it == writer )
        {
            break;
        }
    }
    if ( !*writer_it )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Can't find snap writer." );
        goto out;
    }
    /* Remove the writer from the list */
    *writer_it = writer->next;

    /* Close event writer. */
    status = otf2_snap_writer_delete( writer );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Closes the marker writer.
 *
 *  @param archive       Archive handle.
 *  @param markerWriter  Writer handle to be closed.
 *
 *  @threadsafety        Locks the archive.
 *
 *  @return              Returns OTF2_SUCCESS if successful, an error code
 *                       if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_marker_writer( OTF2_Archive*      archive,
                                  OTF2_MarkerWriter* markerWriter )
{
    UTILS_ASSERT( archive );

    if ( markerWriter == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( archive->marker_writer != markerWriter )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Marker writer does not match with that of the archive." );
        goto out;
    }
    archive->marker_writer = NULL;

    /* Delete the writer object. */
    status = otf2_marker_writer_delete( markerWriter );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Close an opened local event reader.
 *
 *  @param archive  Archive handle.
 *  @param reader   Reader handle to be closed.
 *  @param locked   Archive already locked?
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_evt_reader( OTF2_Archive*   archive,
                               OTF2_EvtReader* reader,
                               bool            locked )
{
    UTILS_ASSERT( archive );

    if ( reader == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    if ( !locked )
    {
        OTF2_ARCHIVE_LOCK( archive );
    }

    /* Search the reader in the list of open readers. */
    OTF2_EvtReader** reader_it;
    for ( reader_it = &archive->local_evt_readers;
          *reader_it;
          reader_it = &( *reader_it )->next )
    {
        if ( *reader_it == reader )
        {
            break;
        }
    }
    if ( !*reader_it )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Can't find event reader." );
        goto out;
    }
    /* Remove the reader from the list */
    *reader_it = reader->next;
    archive->number_of_evt_readers--;

    /* Close event reader. */
    status = otf2_evt_reader_delete( reader );

out:
    if ( !locked )
    {
        OTF2_ARCHIVE_UNLOCK( archive );
    }
    return status;
}


/** @brief Closes the global event reader.
 *
 *  @param archive          Archive handle.
 *  @param globalEvtReader  Reader handle to be closed.
 *
 *  @threadsafety           Locks the archive.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_global_evt_reader( OTF2_Archive*         archive,
                                      OTF2_GlobalEvtReader* globalEvtReader )
{
    UTILS_ASSERT( archive );

    if ( globalEvtReader == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( archive->global_evt_reader != globalEvtReader )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Global event reader does not match with that of "
                              "the archive." );
        goto out;
    }
    archive->global_evt_reader = NULL;

    /* Delete the reader object. */
    status = otf2_global_evt_reader_delete( globalEvtReader, true );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Close an opened local definition reader.
 *
 *  @param archive  Archive handle.
 *  @param reader   Reader handle to be closed.
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_def_reader( OTF2_Archive*   archive,
                               OTF2_DefReader* reader )
{
    UTILS_ASSERT( archive );

    if ( reader == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search the reader in the list of open readers. */
    OTF2_DefReader** reader_it;
    for ( reader_it = &archive->local_def_readers;
          *reader_it;
          reader_it = &( *reader_it )->next )
    {
        if ( *reader_it == reader )
        {
            break;
        }
    }
    if ( !*reader_it )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Can't find definition reader." );
        goto out;
    }
    /* Remove the reader from the list */
    *reader_it = reader->next;

    /* Close definition reader. */
    status = otf2_def_reader_delete( reader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Closes the global definition reader.
 *
 *  @param archive          Archive handle.
 *  @param globalDefReader  Reader handle to be closed.
 *
 *  @threadsafety           Locks the archive.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_global_def_reader( OTF2_Archive*         archive,
                                      OTF2_GlobalDefReader* globalDefReader )
{
    UTILS_ASSERT( archive );

    if ( globalDefReader == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( archive->global_def_reader != globalDefReader )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Global definition reader does not match with that "
                              "of the archive." );
        goto out;
    }
    archive->global_def_reader = NULL;

    /* Delete the reader object. */
    status = otf2_global_def_reader_delete( globalDefReader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Close an opened local snap reader.
 *
 *  @param archive  Archive handle.
 *  @param reader   Reader handle to be closed.
 *  @param locked   Archive already locked?
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_snap_reader( OTF2_Archive*    archive,
                                OTF2_SnapReader* reader,
                                bool             locked )
{
    UTILS_ASSERT( archive );

    if ( reader == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    if ( !locked )
    {
        OTF2_ARCHIVE_LOCK( archive );
    }

    /* Search the reader in the list of open readers. */
    OTF2_SnapReader** reader_it;
    for ( reader_it = &archive->local_snap_readers;
          *reader_it;
          reader_it = &( *reader_it )->next )
    {
        if ( *reader_it == reader )
        {
            break;
        }
    }
    if ( !*reader_it )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Can't find event reader." );
        goto out;
    }
    /* Remove the reader from the list */
    *reader_it = reader->next;
    archive->number_of_snap_readers--;

    /* Close event reader. */
    status = otf2_snap_reader_delete( reader );

out:
    if ( !locked )
    {
        OTF2_ARCHIVE_UNLOCK( archive );
    }
    return status;
}


/** @brief Closes the global snapshot reader.
 *
 *  @param archive          Archive handle.
 *  @param globalSnapReader Reader handle to be closed.
 *
 *  @threadsafety           Locks the archive.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_global_snap_reader( OTF2_Archive*          archive,
                                       OTF2_GlobalSnapReader* globalSnapReader )
{
    UTILS_ASSERT( archive );

    if ( globalSnapReader == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( archive->global_snap_reader != globalSnapReader )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Global snapshot reader does not match with that of "
                              "the archive." );
        goto out;
    }
    archive->global_snap_reader = NULL;

    /* Delete the reader object. */
    status = otf2_global_snap_reader_delete( globalSnapReader, true );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Close an opened thumb reader.
 *
 *  @param archive  Archive handle.
 *  @param reader   Reader handle to be closed.
 *
 *  @threadsafety   Locks the archive.
 *
 *  @return         Returns OTF2_SUCCESS if successful, an error code
 *                  if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_thumb_reader( OTF2_Archive*     archive,
                                 OTF2_ThumbReader* reader )
{
    UTILS_ASSERT( archive );

    if ( reader == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    /* Search the reader in the list of open readers. */
    OTF2_ThumbReader** reader_it;
    for ( reader_it = &archive->thumb_readers;
          *reader_it;
          reader_it = &( *reader_it )->next )
    {
        if ( *reader_it == reader )
        {
            break;
        }
    }
    if ( !*reader_it )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Can't find thumbnail reader." );
        goto out;
    }
    /* Remove the reader from the list */
    *reader_it = reader->next;

    /* Close thumb reader. */
    status = otf2_thumb_reader_delete( reader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/** @brief Closes the marker reader.
 *
 *  @param archive       Archive handle.
 *  @param markerReader  Reader handle to be closed.
 *
 *  @threadsafety        Locks the archive.
 *
 *  @return              Returns OTF2_SUCCESS if successful, an error code
 *                       if an error occurs.
 */
OTF2_ErrorCode
otf2_archive_close_marker_reader( OTF2_Archive*      archive,
                                  OTF2_MarkerReader* markerReader )
{
    UTILS_ASSERT( archive );

    if ( markerReader == NULL )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    if ( archive->marker_reader != markerReader )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                              "Marker reader does not match with that of the archive." );
        goto out;
    }
    archive->marker_reader = NULL;

    /* Delete the reader object. */
    status = otf2_marker_reader_delete( markerReader );

out:
    OTF2_ARCHIVE_UNLOCK( archive );
    return status;
}


/* ___ Register function pointers. __________________________________________ */

static const otf2_reader_archive_impl otf2_archive_reader_impl =
{
    .delete_archive                  = ( OTF2_FP_Archive                     )OTF2_Archive_Close,
    .set_collective_callbacks        = ( OTF2_FP_SetCollectiveCallbacks      )OTF2_Archive_SetCollectiveCallbacks,
    .set_serial_collective_callbacks = ( OTF2_FP_Archive                     )OTF2_Archive_SetSerialCollectiveCallbacks,
    .set_locking_callbacks           = ( OTF2_FP_SetLockingCallbacks         )OTF2_Archive_SetLockingCallbacks,
    .register_evt_callbacks          = ( OTF2_FP_RegisterEvtCallbacks        )OTF2_EvtReader_SetCallbacks,
    .register_global_evt_callbacks   = ( OTF2_FP_RegisterGlobalEvtCallbacks  )OTF2_GlobalEvtReader_SetCallbacks,
    .register_def_callbacks          = ( OTF2_FP_RegisterDefCallbacks        )OTF2_DefReader_SetCallbacks,
    .register_global_def_callbacks   = ( OTF2_FP_RegisterGlobalDefCallbacks  )OTF2_GlobalDefReader_SetCallbacks,
    .register_snap_callbacks         = ( OTF2_FP_RegisterSnapCallbacks       )OTF2_SnapReader_SetCallbacks,
    .register_global_snap_callbacks  = ( OTF2_FP_RegisterGlobalSnapCallbacks )OTF2_GlobalSnapReader_SetCallbacks,
    .register_marker_callbacks       = ( OTF2_FP_RegisterMarkerCallbacks     )OTF2_MarkerReader_SetCallbacks,
    .read_local_events               = ( OTF2_FP_ReadLocalEvents             )OTF2_EvtReader_ReadEvents,
    .read_global_event               = ( OTF2_FP_ReadGlobalEvent             )OTF2_GlobalEvtReader_ReadEvent,
    .has_global_event                = ( OTF2_FP_HasGlobalEvent              )OTF2_GlobalEvtReader_HasEvent,
    .read_global_events              = ( OTF2_FP_ReadGlobalEvents            )OTF2_GlobalEvtReader_ReadEvents,
    .read_local_events_backward      = ( OTF2_FP_ReadLocalEvents             )OTF2_EvtReader_ReadEventsBackward,
    .read_local_definitions          = ( OTF2_FP_ReadLocalDefinitions        )OTF2_DefReader_ReadDefinitions,
    .read_global_definitions         = ( OTF2_FP_ReadGlobalDefinitions       )OTF2_GlobalDefReader_ReadDefinitions,
    .read_local_snapshots            = ( OTF2_FP_ReadLocalSnapshots          )OTF2_SnapReader_ReadSnapshots,
    .read_global_snapshots           = ( OTF2_FP_ReadGlobalSnapshots         )OTF2_GlobalSnapReader_ReadSnapshots,
    .read_markers                    = ( OTF2_FP_ReadMarkers                 )OTF2_MarkerReader_ReadMarkers,
    .get_evt_reader                  = ( OTF2_FP_GetEvtReader                )OTF2_Archive_GetEvtReader,
    .get_snap_reader                 = ( OTF2_FP_GetSnapReader               )OTF2_Archive_GetSnapReader,
    .get_def_reader                  = ( OTF2_FP_GetDefReader                )OTF2_Archive_GetDefReader,
    .get_global_evt_reader           = ( OTF2_FP_GetGlobalEvtReader          )OTF2_Archive_GetGlobalEvtReader,
    .get_global_snap_reader          = ( OTF2_FP_GetGlobalSnapReader         )OTF2_Archive_GetGlobalSnapReader,
    .get_global_def_reader           = ( OTF2_FP_GetGlobalDefReader          )OTF2_Archive_GetGlobalDefReader,
    .get_thumb_reader                = ( OTF2_FP_GetThumbReader              )OTF2_Archive_GetThumbReader,
    .get_marker_reader               = ( OTF2_FP_GetMarkerReader             )OTF2_Archive_GetMarkerReader,
    .get_marker_writer               = ( OTF2_FP_GetMarkerWriter             )OTF2_Archive_GetMarkerWriter,
    .get_version                     = ( OTF2_FP_GetVersion                  )OTF2_Archive_GetVersion,
    .get_chunk_size                  = ( OTF2_FP_GetChunkSize                )OTF2_Archive_GetChunkSize,
    .get_file_substrate              = ( OTF2_FP_GetFileSubstrate            )OTF2_Archive_GetFileSubstrate,
    .get_compression                 = ( OTF2_FP_GetCompression              )OTF2_Archive_GetCompression,
    .get_number_of_locations         = ( OTF2_FP_GetNumLocations             )OTF2_Archive_GetNumberOfLocations,
    .get_number_of_global_defs       = ( OTF2_FP_GetNumGlobalDefs            )OTF2_Archive_GetNumberOfGlobalDefinitions,
    .get_machine_name                = ( OTF2_FP_GetMachineName              )OTF2_Archive_GetMachineName,
    .get_creator                     = ( OTF2_FP_GetCreator                  )OTF2_Archive_GetCreator,
    .get_description                 = ( OTF2_FP_GetDescription              )OTF2_Archive_GetDescription,
    .get_property_names              = ( OTF2_FP_GetPropertyNames            )OTF2_Archive_GetPropertyNames,
    .get_property                    = ( OTF2_FP_GetProperty                 )OTF2_Archive_GetProperty,
    .get_trace_id                    = ( OTF2_FP_GetTraceId                  )OTF2_Archive_GetTraceId,
    .get_number_of_snapshots         = ( OTF2_FP_GetNumberOfSnapshots        )OTF2_Archive_GetNumberOfSnapshots,
    .get_number_of_thumbnails        = ( OTF2_FP_GetNumberOfThumbnails       )OTF2_Archive_GetNumberOfThumbnails,
    .close_evt_reader                = ( OTF2_FP_CloseEvtReader              )OTF2_Archive_CloseEvtReader,
    .close_global_evt_reader         = ( OTF2_FP_CloseGlobalEvtReader        )OTF2_Archive_CloseGlobalEvtReader,
    .close_def_reader                = ( OTF2_FP_CloseDefReader              )OTF2_Archive_CloseDefReader,
    .close_global_def_reader         = ( OTF2_FP_CloseGlobalDefReader        )OTF2_Archive_CloseGlobalDefReader,
    .close_snap_reader               = ( OTF2_FP_CloseSnapReader             )OTF2_Archive_CloseSnapReader,
    .close_global_snap_reader        = ( OTF2_FP_CloseGlobalSnapReader       )OTF2_Archive_CloseGlobalSnapReader,
    .close_thumb_reader              = ( OTF2_FP_CloseThumbReader            )OTF2_Archive_CloseThumbReader,
    .close_marker_reader             = ( OTF2_FP_CloseMarkerReader           )OTF2_Archive_CloseMarkerReader,
    .close_marker_writer             = ( OTF2_FP_CloseMarkerWriter           )OTF2_Archive_CloseMarkerWriter,
    .open_evt_files                  = ( OTF2_FP_Archive                     )OTF2_Archive_OpenEvtFiles,
    .close_evt_files                 = ( OTF2_FP_Archive                     )OTF2_Archive_CloseEvtFiles,
    .open_local_def_files            = ( OTF2_FP_Archive                     )OTF2_Archive_OpenDefFiles,
    .close_local_def_files           = ( OTF2_FP_Archive                     )OTF2_Archive_CloseDefFiles,
    .open_snap_files                 = ( OTF2_FP_Archive                     )OTF2_Archive_OpenSnapFiles,
    .close_snap_files                = ( OTF2_FP_Archive                     )OTF2_Archive_CloseSnapFiles,
    .select_location                 = ( OTF2_FP_SelectLocation              )OTF2_Archive_SelectLocation,
    .set_hint                        = ( OTF2_FP_SetHint                     )OTF2_Archive_SetHint,
    .is_master                       = ( OTF2_FP_IsMaster                    )OTF2_Archive_IsMaster
};


const otf2_reader_archive_impl*
otf2_archive_get_reader_impl( void )
{
    return &otf2_archive_reader_impl;
}


/* ___ Static helper callbacks. __________________________________________ */


static char*
otf2_archive_get_file_name( const char*   base,
                            OTF2_FileType fileType )
{
    const char* suffix = "";
    switch ( fileType )
    {
        case OTF2_FILETYPE_ANCHOR:
            suffix = "otf2";
            break;

        case OTF2_FILETYPE_GLOBAL_DEFS:
            suffix = "def";
            break;

        case OTF2_FILETYPE_LOCAL_DEFS:
            suffix = "def";
            break;

        case OTF2_FILETYPE_EVENTS:
            suffix = "evt";
            break;

        case OTF2_FILETYPE_SNAPSHOTS:
            suffix = "snap";
            break;

        case OTF2_FILETYPE_THUMBNAIL:
            suffix = "thumb";
            break;

        case OTF2_FILETYPE_MARKER:
            suffix = "marker";
            break;

        case OTF2_FILETYPE_SIONRANKMAP:
            suffix = "srm";
            break;
    }

    char* file_name = malloc( strlen( base ) + 1 + strlen( suffix ) + 1 );
    if ( !file_name )
    {
        UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
        return NULL;
    }

    /* Build-up the name */
    sprintf( file_name, "%s.%s", base, suffix );

    return file_name;
}

char*
otf2_archive_get_file_path( OTF2_Archive*    archive,
                            OTF2_FileType    fileType,
                            OTF2_LocationRef location )
{
    UTILS_ASSERT( archive );

    char  location_name[ 32 ];
    char* name_prefix = NULL;

    /* pre-location files have the location as name_prefix */
    if ( otf2_file_type_needs_location_id( fileType ) ||
         fileType == OTF2_FILETYPE_THUMBNAIL )
    {
        UTILS_ASSERT( OTF2_UNDEFINED_LOCATION != location );

        sprintf( location_name, "%" PRIu64, location );
        name_prefix = location_name;
    }
    else
    {
        UTILS_ASSERT( OTF2_UNDEFINED_LOCATION == location );
    }

    return otf2_archive_get_file_path_with_name_prefix( archive,
                                                        fileType,
                                                        name_prefix );
}

char*
otf2_archive_get_file_path_with_name_prefix( OTF2_Archive* archive,
                                             OTF2_FileType fileType,
                                             const char*   name_prefix )
{
    UTILS_ASSERT( archive );
    //UTILS_ASSERT( ( otf2_file_type_needs_location_id( fileType ) && name_prefix )
    //              || ( !otf2_file_type_needs_location_id( fileType ) && !name_prefix ) );

    char* path_component_1;
    char* path_component_2;
    char* to_free = NULL;

    if ( otf2_file_type_needs_location_id( fileType ) )
    {
        /* per-location files reside in the archive_name directory */

        path_component_1 = archive->archive_name;
        path_component_2 = otf2_archive_get_file_name( name_prefix,
                                                       fileType );
        if ( !path_component_2 )
        {
            UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
            return NULL;
        }
        to_free = path_component_2;
    }
    else if ( fileType == OTF2_FILETYPE_THUMBNAIL )
    {
        char* thumb_name_base = malloc( strlen( archive->archive_name ) + 1
                                        + strlen( name_prefix ) + 1 );

        sprintf( thumb_name_base, "%s.%s", archive->archive_name, name_prefix );

        path_component_1 = otf2_archive_get_file_name( thumb_name_base,
                                                       fileType );
        free( thumb_name_base );
        path_component_2 = "";
    }
    else
    {
        /*
         * global files have the archive_name as prefix and get an appropriate
         * suffix.
         */

        path_component_1 = otf2_archive_get_file_name( archive->archive_name,
                                                       fileType );
        if ( !path_component_1 )
        {
            UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
            return NULL;
        }
        to_free = path_component_1;

        /*
         * UTILS_IO_JoinPath ignores "" path components
         */
        path_component_2 = "";
    }

    char* file_path = UTILS_IO_JoinPath( 3,
                                         archive->archive_path,
                                         path_component_1,
                                         path_component_2 );
    free( to_free );
    if ( !file_path )
    {
        UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Allocation failed!" );
        return NULL;
    }

    return file_path;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_open_evt_files( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    status = otf2_file_substrate_open_file_type( archive,
                                                 archive->file_mode,
                                                 OTF2_FILETYPE_EVENTS );

    OTF2_ARCHIVE_UNLOCK( archive );

    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_close_evt_files( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    status = otf2_file_substrate_close_file_type( archive,
                                                  OTF2_FILETYPE_EVENTS );

    OTF2_ARCHIVE_UNLOCK( archive );

    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_open_def_files( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    status = otf2_file_substrate_open_file_type( archive,
                                                 archive->file_mode,
                                                 OTF2_FILETYPE_LOCAL_DEFS );

    OTF2_ARCHIVE_UNLOCK( archive );

    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_close_def_files( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    status = otf2_file_substrate_close_file_type( archive,
                                                  OTF2_FILETYPE_LOCAL_DEFS );

    OTF2_ARCHIVE_UNLOCK( archive );

    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_open_snap_files( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    status = otf2_file_substrate_open_file_type( archive,
                                                 archive->file_mode,
                                                 OTF2_FILETYPE_SNAPSHOTS );

    OTF2_ARCHIVE_UNLOCK( archive );

    return status;
}


/**
 *  @threadsafety  Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_close_snap_files( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    status = otf2_file_substrate_close_file_type( archive,
                                                  OTF2_FILETYPE_SNAPSHOTS );

    OTF2_ARCHIVE_UNLOCK( archive );

    return status;
}


bool
otf2_archive_is_master( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    uint32_t rank;
    otf2_collectives_get_rank( archive,
                               archive->global_comm_context,
                               &rank );

    return rank == 0;
}


/**
 *  @threadsafety  Only called from otf2_archive_create_directory() which
 *                 already holds the lock.
 */
OTF2_ErrorCode
otf2_archive_create_directory( OTF2_Archive* archive )
{
    /* Get archive path and archive name. */
    char* archive_path = NULL;
    char* archive_name = NULL;

    OTF2_ErrorCode status = otf2_archive_get_archive_path( archive, &archive_path );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not get archive name!" );
    }
    status = otf2_archive_get_trace_path( archive, &archive_name );
    if ( status != OTF2_SUCCESS )
    {
        free( archive_path );

        return UTILS_ERROR( status, "Could not get archive name!" );
    }

    /* Create archive main directory and archive trace directory. Ignore error
       if an archive main directory already exists because it is free to the
       user to create it by himself. */
    status = OTF2_File_CreateDirectory( archive, archive_path, false );
    if ( status != OTF2_SUCCESS )
    {
        free( archive_path );
        free( archive_name );

        return UTILS_ERROR( status, "Could not create archive main directory!" );
    }

    /* Please note: It is wanted that the call fails, if the directory already
     * exists. It is to prevent overwriting previous OTF2 archives. */
    status = OTF2_File_CreateDirectory( archive, archive_name, true );
    if ( status != OTF2_SUCCESS )
    {
        free( archive_path );
        free( archive_name );

        return UTILS_ERROR( status, "Could not create archive trace directory!" );
    }

    free( archive_path );
    free( archive_name );

    return OTF2_SUCCESS;
}


/** @brief Select location to location vector.
 *
 *  @param archive   Archive handle.
 *  @param location  Location ID to add to vector.
 *
 *  @threadsafety    Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_select_location( OTF2_Archive*    archive,
                              OTF2_LocationRef location )
{
    UTILS_ASSERT( archive );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    status = otf2_archive_add_location( archive, location );

    OTF2_ARCHIVE_UNLOCK( archive );

    return status;
}


/** @brief Add location to location vector.
 *
 *  @param archive   Archive handle.
 *  @param location  Location ID to add to vector.
 *
 *  @threadsafety    Caller must hold the archive lock.
 */
OTF2_ErrorCode
otf2_archive_add_location( OTF2_Archive*    archive,
                           OTF2_LocationRef location )
{
    UTILS_ASSERT( archive );

    /* Check if location is already in the vector. */
    for ( uint32_t i = 0; i < archive->locations_number; i++ )
    {
        if ( archive->locations[ i ].location_id == location )
        {
            return OTF2_SUCCESS;
        }
    }

    /* Check size of vector and let it grow if necessary. */
    if ( ( ( archive->locations_number % OTF2_ARCHIVE_LOCATION_SIZE ) == 0 ) &&
         ( archive->locations_number > 0 ) )
    {
        void* new_locations = NULL;
        new_locations = realloc( archive->locations,
                                 ( archive->locations_number + OTF2_ARCHIVE_LOCATION_SIZE )
                                 * sizeof( otf2_archive_location ) );
        if ( new_locations == NULL )
        {
            return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                                "Failed to reallocate locations vector!" );
        }
        archive->locations = ( otf2_archive_location* )new_locations;
    }

    otf2_archive_location_initialize( &archive->locations[ archive->locations_number ],
                                      location );
    archive->locations_number++;

    return OTF2_SUCCESS;
}


/**
 *  @threadsafety    Caller must hold the archive lock.
 */
OTF2_ErrorCode
otf2_archive_find_location( OTF2_Archive*    archive,
                            OTF2_LocationRef location,
                            uint32_t*        archiveLocationIndex )
{
    UTILS_ASSERT( archive );

    /* Check if location is already in the vector. */
    for ( uint32_t i = 0; i < archive->locations_number; i++ )
    {
        if ( archive->locations[ i ].location_id == location )
        {
            if ( archiveLocationIndex )
            {
                *archiveLocationIndex = i;
            }
            return OTF2_SUCCESS;
        }
    }

    return OTF2_ERROR_INDEX_OUT_OF_BOUNDS;
}


/**
 *  @threadsafety    Caller must hold the archive lock.
 */
OTF2_ErrorCode
otf2_archive_get_location( OTF2_Archive*           archive,
                           uint32_t                archiveLocationIndex,
                           otf2_archive_location** archiveLocation )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( archiveLocation );

    if ( archiveLocationIndex < archive->locations_number )
    {
        *archiveLocation = &archive->locations[ archiveLocationIndex ];

        return OTF2_SUCCESS;
    }

    return OTF2_ERROR_INDEX_OUT_OF_BOUNDS;
}


/** @threadsafety    Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_set_hint( OTF2_Archive* archive,
                       OTF2_Hint     hint,
                       void*         value )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( value );

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( archive );

    switch ( hint )
    {
        case OTF2_HINT_GLOBAL_READER:
            if ( archive->file_mode != OTF2_FILEMODE_READ )
            {
                status = UTILS_ERROR( OTF2_ERROR_HINT_INVALID,
                                      "Archive is not in reader mode for global-reader hint." );
                goto out;
            }
            if ( archive->hint_global_reader_locked )
            {
                status = UTILS_ERROR( OTF2_ERROR_HINT_LOCKED,
                                      "The global-reader hint is already locked." );
                goto out;
            }
            archive->hint_global_reader_locked = true;
            archive->hint_global_reader        = *( OTF2_Boolean* )value;
            if ( archive->hint_global_reader != OTF2_TRUE &&
                 archive->hint_global_reader != OTF2_FALSE )
            {
                status = UTILS_ERROR( OTF2_ERROR_HINT_INVALID_VALUE,
                                      "Invalid value for global-reader hint: %u",
                                      archive->hint_global_reader );
                goto out;
            }
            break;

        default:
            status = UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                                  "Unknown hint: %u", hint );
            goto out;
    }

    status = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );

    return status;
}


/**
 *  @threadsafety Caller must hold the archive lock.
 */
OTF2_ErrorCode
otf2_archive_query_hint( OTF2_Archive* archive,
                         OTF2_Hint     hint,
                         void*         value )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( value );

    switch ( hint )
    {
        case OTF2_HINT_GLOBAL_READER:
            if ( archive->file_mode != OTF2_FILEMODE_READ )
            {
                return UTILS_ERROR( OTF2_ERROR_HINT_INVALID,
                                    "Archive is not in reader mode for global-reader hint." );
            }
            archive->hint_global_reader_locked = true;
            *( OTF2_Boolean* )value            = archive->hint_global_reader;
            break;

        default:
            /* This should never happen, as only OTF2 internally can query hints. */
            return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                                "Unknown hint: %u", hint );
    }

    return OTF2_SUCCESS;
}

/** @threadsafety    Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_add_calling_context( OTF2_Archive*              archive,
                                  OTF2_CallingContextRef     callingContext,
                                  OTF2_RegionRef             region,
                                  OTF2_SourceCodeLocationRef sourceCodeLocation,
                                  OTF2_CallingContextRef     parent )
{
    UTILS_ASSERT( archive );

    OTF2_ARCHIVE_LOCK( archive );

    OTF2_ErrorCode ret;

    if ( archive->calling_context_to_region_map_size
         == archive->calling_context_to_region_map_capacity )
    {
        uint32_t        new_capacity = archive->calling_context_to_region_map_capacity + 128;
        OTF2_RegionRef* new_mapping  =
            realloc( archive->calling_context_to_region_map,
                     new_capacity * sizeof( *new_mapping ) );
        if ( !new_mapping )
        {
            ret = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                               "Couldn't increase CallingContext->Region mapping." );
            goto out;
        }
        archive->calling_context_to_region_map          = new_mapping;
        archive->calling_context_to_region_map_capacity = new_capacity;
    }

    if ( callingContext != archive->calling_context_to_region_map_size )
    {
        /* We got a callingContext out of order. The user gets what he deserves. */

        if ( !archive->calling_context_to_region_index_map )
        {
            archive->calling_context_to_region_index_map =
                OTF2_IdMap_Create( OTF2_ID_MAP_SPARSE, 16 );
            if ( !archive->calling_context_to_region_index_map )
            {
                ret = UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                                   "Couldn't creating CallingContext->Region index mapping." );
                goto out;
            }
        }

        ret = OTF2_IdMap_AddIdPair( archive->calling_context_to_region_index_map,
                                    callingContext,
                                    archive->calling_context_to_region_map_size );
        if ( OTF2_SUCCESS != ret )
        {
            UTILS_ERROR( ret, "Couldn't add new CallingContext index mapping entry." );
            goto out;
        }
    }

    archive->calling_context_to_region_map[ archive->calling_context_to_region_map_size++ ] =
        region;

    ret = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );

    return ret;
}

/** @threadsafety    Locks the archive.
 */
OTF2_ErrorCode
otf2_archive_map_calling_context_to_region( OTF2_Archive*          archive,
                                            OTF2_CallingContextRef callingContext,
                                            OTF2_RegionRef*        region )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( region );

    OTF2_ARCHIVE_LOCK( archive );

    OTF2_ErrorCode ret;

    uint64_t index = callingContext;
    if ( archive->calling_context_to_region_index_map )
    {
        /* This maps entries not in the mapping to itself. */
        ret = OTF2_IdMap_GetGlobalId( archive->calling_context_to_region_index_map,
                                      index,
                                      &index );
        if ( OTF2_SUCCESS != ret )
        {
            goto out;
        }
    }

    if ( index >= archive->calling_context_to_region_map_size )
    {
        ret = OTF2_ERROR_INDEX_OUT_OF_BOUNDS;
        goto out;
    }

    *region = archive->calling_context_to_region_map[ index ];
    ret     = OTF2_SUCCESS;

out:
    OTF2_ARCHIVE_UNLOCK( archive );

    return ret;
}
