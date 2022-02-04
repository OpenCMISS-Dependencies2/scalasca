/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
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
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <sion.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#define OTF2_DEBUG_MODULE_NAME SION
#include <UTILS_Debug.h>

#include <UTILS_CStr.h>

#include "otf2_internal.h"
#include "otf2_lock.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"
#include "otf2_archive_location.h"

#include "OTF2_File.h"
#include "otf2_file_int.h"
#include "otf2_file_sion.h"
#include "otf2_file_substrate.h"
#include "otf2_file_substrate_sion.h"
#include "otf2_file_substrate_posix.h"

#include "otf2_file_substrate_sion_int.h"

#include "otf2_collectives.h"


static OTF2_ErrorCode
otf2_sion_open_write( OTF2_Archive* archive,
                      OTF2_FileMode fileMode,
                      OTF2_FileType fileType );


static OTF2_ErrorCode
otf2_sion_open_read( OTF2_Archive* archive,
                     OTF2_FileMode fileMode,
                     OTF2_FileType fileType );


static char*
otf2_sion_get_multi_filename_cb( const char* filePath,
                                 int         fileNumber )
{
    UTILS_DEBUG_ENTRY( "%s, %d", filePath, fileNumber );

    if ( fileNumber <= 0 )
    {
        UTILS_DEBUG_EXIT( "%s", filePath );
        return UTILS_CStr_dup( filePath );
    }

    /* we need at most 16 bytes to print an int. */
    char* new_file_path = malloc( strlen( filePath ) + 16 + 1 );
    if ( !new_file_path )
    {
        return NULL;
    }

    char* zero = strrchr( filePath, '0' );
    if ( !zero )
    {
        free( new_file_path );
        return NULL;
    }
    /* copy all upto, but not including the '0' to the new path. */
    memcpy( new_file_path, filePath, zero - filePath );
    sprintf( new_file_path + ( zero - filePath ), "%d%s", fileNumber, zero + 1 );

    UTILS_DEBUG_EXIT( "%s", new_file_path );

    return new_file_path;
}


/**
 * @brief Initializes the substrate.
 *
 * Called once for an archive. Substrate can use @a *substrateData, for
 * substrate specific data.
 */
OTF2_ErrorCode
otf2_file_substrate_sion_initialize( OTF2_Archive* archive )
{
    UTILS_DEBUG_ENTRY();

    UTILS_ASSERT( archive );

    UTILS_DEBUG( "SIONlib is thread-safe: %d", sion_is_thread_safe() );

    OTF2_ErrorCode status = otf2_file_substrate_posix_initialize( archive );
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status, "Could not initialize the POSIX substrate." );
    }

    int api_id = sion_generic_create_api( "OTF2_SION_API" );
    UTILS_DEBUG( "API id: %d", api_id );
    otf2_file_substrate_sion_register_callbacks( api_id );

    sion_generic_register_get_multi_filename_cb(
        api_id,
        otf2_sion_get_multi_filename_cb );

    OTF2_FileSubstrateData* data = calloc( 1, sizeof( *data ) );
    if ( !data )
    {
        sion_generic_free_api( api_id );
        return UTILS_ERROR( OTF2_ERROR_MEM_ALLOC_FAILED,
                            "Allocation of global file substrate data failed" );
    }

    data->api_id = api_id;

    data->file_mode = OTF2_UNDEFINED_TYPE;

    data->global_comm_proxy.archive = archive;
    data->global_comm_proxy.type    = OTF2_SION_PROXY_TYPE_GLOBAL;

    data->local_comm_proxy.archive = archive;
    data->local_comm_proxy.type    = OTF2_SION_PROXY_TYPE_LOCAL;

    archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] = data;

    return OTF2_SUCCESS;
}


/**
 * @brief Finalizes the substrate.
 *
 * Called once for an archive. Substrate should release @a substrateData, if
 * it was set in @a otf2_file_substrate_initialize.
 */
OTF2_ErrorCode
otf2_file_substrate_sion_finalize( OTF2_Archive* archive )
{
    UTILS_DEBUG_ENTRY();

    UTILS_ASSERT( archive );
    if ( !archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Substrate not initialized!" );
    }

    OTF2_FileSubstrateData* data = archive->per_substrate_data[ OTF2_SUBSTRATE_SION ];
    sion_generic_free_api( data->api_id );

    /* Call also the POSIX finalization function. */
    OTF2_ErrorCode status = otf2_file_substrate_posix_finalize( archive );

    free( data );
    archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] = NULL;

    return status;
}


/**
 * @brief Called when the collective context was set by the user.
 *
 * @threadsafety Caller holds the archive lock.
 */
OTF2_ErrorCode
otf2_file_substrate_sion_open( OTF2_Archive* archive,
                               OTF2_FileMode fileMode )
{
    UTILS_DEBUG_ENTRY( "%d", fileMode );

    UTILS_ASSERT( archive );
    if ( !archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Substrate not initialized!" );
    }

    OTF2_FileSubstrateData* data = archive->per_substrate_data[ OTF2_SUBSTRATE_SION ];

    data->file_mode = fileMode;

    if ( OTF2_FILEMODE_READ == fileMode )
    {
        /* read SION rank-map file */
        OTF2_ErrorCode status = otf2_sion_read_rank_map( archive,
                                                         &data->number_of_files,
                                                         &data->number_of_ranks,
                                                         &data->file_number_per_rank,
                                                         &data->file_rank_per_rank,
                                                         &data->location_to_rank_map );
        if ( OTF2_SUCCESS != status )
        {
            return UTILS_ERROR( status, "Could not load SION rank-map file." );
        }
    }
    else
    {
        uint32_t          global_rank  = 0;
        uint32_t          global_size  = 0;
        OTF2_CallbackCode callback_ret = OTF2_CALLBACK_SUCCESS;

        /* Get global rank and size. */
        callback_ret = otf2_collectives_get_rank( archive,
                                                  archive->global_comm_context,
                                                  &global_rank );
        if ( callback_ret != OTF2_CALLBACK_SUCCESS )
        {
            return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                                "Collective get rank failed." );
        }

        callback_ret = otf2_collectives_get_size( archive,
                                                  archive->global_comm_context,
                                                  &global_size );
        if ( callback_ret != OTF2_CALLBACK_SUCCESS )
        {
            return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                                "Collective get size failed." );
        }

        /* Determine in how many files we write */
        if ( !archive->local_comm_context )
        {
            data->file_number             = 0;
            data->number_of_files         = 1;
            data->file_rank               = global_rank;
            data->number_of_ranks_in_file = global_size;
        }
        else
        {
            callback_ret = otf2_collectives_get_rank( archive,
                                                      archive->local_comm_context,
                                                      &data->file_rank );
            if ( callback_ret != OTF2_CALLBACK_SUCCESS )
            {
                return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                                    "Collective get rank failed." );
            }

            callback_ret = otf2_collectives_get_size( archive,
                                                      archive->local_comm_context,
                                                      &data->number_of_ranks_in_file );
            if ( callback_ret != OTF2_CALLBACK_SUCCESS )
            {
                return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                                    "Collective get size failed." );
            }

            /* Calculate number of files and file number using local and global
             * communication context. */
            uint32_t* all_local_ranks = NULL;
            uint32_t* file_numbers    = NULL;
            if ( OTF2_COLLECTIVES_ROOT == global_rank )
            {
                all_local_ranks = calloc( global_size, sizeof( *all_local_ranks ) );
                file_numbers    = calloc( global_size, sizeof( *file_numbers ) );
            }

            /* Collect all local ranks. */
            callback_ret = otf2_collectives_gather( archive,
                                                    archive->global_comm_context,
                                                    &data->file_rank,
                                                    all_local_ranks,
                                                    1,
                                                    OTF2_TYPE_UINT32,
                                                    OTF2_COLLECTIVES_ROOT );
            if ( callback_ret != OTF2_CALLBACK_SUCCESS )
            {
                free( all_local_ranks );
                free( file_numbers );
                return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                                    "Collective gather failed." );
            }

            /* Count all local roots to get number of files.
             * Give every local root a file number. */
            if ( OTF2_COLLECTIVES_ROOT == global_rank )
            {
                data->number_of_files = 0;
                for ( int i = 0; i < global_size; i++ )
                {
                    if ( all_local_ranks[ i ] == 0 )
                    {
                        file_numbers[ i ] = data->number_of_files;
                        data->number_of_files++;
                    }
                }
            }
            free( all_local_ranks );

            /* Broadcast number of files to all. */
            callback_ret = otf2_collectives_bcast( archive,
                                                   archive->global_comm_context,
                                                   &data->number_of_files,
                                                   1,
                                                   OTF2_TYPE_UINT32,
                                                   OTF2_COLLECTIVES_ROOT );
            if ( callback_ret != OTF2_CALLBACK_SUCCESS )
            {
                return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                                    "Collective bcast failed." );
            }

            /* Scatter file number to all. */
            callback_ret = otf2_collectives_scatter( archive,
                                                     archive->global_comm_context,
                                                     file_numbers,
                                                     &data->file_number,
                                                     1,
                                                     OTF2_TYPE_UINT32,
                                                     OTF2_COLLECTIVES_ROOT );
            free( file_numbers );
            if ( callback_ret != OTF2_CALLBACK_SUCCESS )
            {
                return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                                    "Collective scatter failed." );
            }

            /* Broadcast file number within local communication context. */
            callback_ret = otf2_collectives_bcast( archive,
                                                   archive->local_comm_context,
                                                   &data->file_number,
                                                   1,
                                                   OTF2_TYPE_UINT32,
                                                   OTF2_COLLECTIVES_ROOT );
            if ( callback_ret != OTF2_CALLBACK_SUCCESS )
            {
                return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                                    "Collective bcast failed." );
            }
        }
    }

    return OTF2_SUCCESS;
}


/**
 * @brief Called before closing the archive.
 *
 * @threadsafety Caller holds the archive lock.
 */
OTF2_ErrorCode
otf2_file_substrate_sion_close( OTF2_Archive* archive )
{
    UTILS_DEBUG_ENTRY();

    UTILS_ASSERT( archive );
    if ( !archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Substrate not initialized!" );
    }

    OTF2_FileSubstrateData* data = archive->per_substrate_data[ OTF2_SUBSTRATE_SION ];

    if ( data->file_mode == OTF2_FILEMODE_WRITE )
    {
        OTF2_ErrorCode status = otf2_sion_write_rank_map( archive );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status, "Couldn't write rank-map file." );
        }
    }
    else
    {
        free( data->file_number_per_rank );
        data->file_number_per_rank = NULL;
        free( data->file_rank_per_rank );
        data->file_rank_per_rank = NULL;
        OTF2_IdMap_Free( data->location_to_rank_map );
        data->location_to_rank_map = NULL;
    }

    return OTF2_SUCCESS;
}


/**
 * @brief
 *
 * @threadsafety Caller holds the archive lock.
 */
OTF2_ErrorCode
otf2_file_substrate_sion_open_file_type( OTF2_Archive* archive,
                                         OTF2_FileMode fileMode,
                                         OTF2_FileType fileType )
{
    UTILS_DEBUG_ENTRY( "%d, %d", fileMode, fileType );

    UTILS_ASSERT( archive );
    if ( !archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Substrate not initialized!" );
    }

    OTF2_FileSubstrateData* data = archive->per_substrate_data[ OTF2_SUBSTRATE_SION ];
    if ( data->file_mode == OTF2_UNDEFINED_TYPE )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Collective context not yet given!" );
    }

    if ( !otf2_file_type_needs_location_id( fileType ) )
    {
        return otf2_file_substrate_posix_open_file_type( archive,
                                                         fileMode,
                                                         fileType );
    }

    char* file_path = otf2_archive_get_file_path_with_name_prefix( archive,
                                                                   fileType,
                                                                   "0" );
    if ( !file_path )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Can't determine file path!" );
    }

    OTF2_FileSubstrateFileTypeData* file_type_data = calloc( 1, sizeof( *file_type_data ) );
    if ( !file_type_data )
    {
        free( file_path );
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Can't allocate file type data!" );
    }
    archive->per_filetype_data[ fileType ] = file_type_data;
    file_type_data->symbolic_file_path     = file_path;
    file_type_data->proxy.archive          = archive;
    file_type_data->proxy.type             = OTF2_SION_PROXY_TYPE_LOCAL_FILE_TYPE;

    otf2_lock_create( archive, &file_type_data->lock );

    /* Determine the file mode. */
    OTF2_ErrorCode ret = OTF2_SUCCESS;
    file_type_data->file_mode = fileMode;
    switch ( fileMode )
    {
        case OTF2_FILEMODE_WRITE:
            ret = otf2_sion_open_write( archive, fileMode, fileType );
            break;
        case OTF2_FILEMODE_READ:
            ret = otf2_sion_open_read( archive, fileMode, fileType );
            break;
        case OTF2_FILEMODE_MODIFY:
            ret = UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                               "SION does not support the modify modus" );
            break;
        default:
            ret = UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                               "Unknown file mode" );
            break;
    }

    if ( OTF2_SUCCESS != ret )
    {
        otf2_lock_destroy( archive, file_type_data->lock );
        free( file_type_data->symbolic_file_path );
        free( file_type_data );
        archive->per_filetype_data[ fileType ] = NULL;
    }

    return ret;
}


OTF2_ErrorCode
otf2_sion_open_write( OTF2_Archive* archive,
                      OTF2_FileMode fileMode,
                      OTF2_FileType fileType )
{
    /* Open the SION file handle */
    sion_int64 chunksize   = OTF2_FILE_BUFFER_SIZE;
    sion_int32 fsblksize   = -1; /* Unknown value, let SIONlib decide. */
    uint32_t   global_rank = 0;
    uint32_t   global_size = 0;

    OTF2_FileSubstrateData*         data           = archive->per_substrate_data[ OTF2_SUBSTRATE_SION ];
    OTF2_FileSubstrateFileTypeData* file_type_data = archive->per_filetype_data[ fileType ];

    OTF2_CallbackCode callback_ret = OTF2_CALLBACK_SUCCESS;

    /* dup is not allowed for writing. */
    file_type_data->dup = false;

    /* we always use the keyval API for writing. */
    file_type_data->keyval = true;

    /* Get global/local rank and size. */
    callback_ret = otf2_collectives_get_rank( archive,
                                              archive->global_comm_context,
                                              &global_rank );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective get rank failed." );
    }

    callback_ret = otf2_collectives_get_size( archive,
                                              archive->global_comm_context,
                                              &global_size );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective get size failed." );
    }

    /* Barrier before open. */
    callback_ret = otf2_collectives_barrier( archive,
                                             archive->global_comm_context );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective barrier failed." );
    }

    /* save some state in data, so that the create_local_commgroup_cb
     * knows what todo
     */
    data->fileModeForLocalCommCreate = fileMode;
    data->fileTypeForLocalCommCreate = fileType;
    int filenumber = data->file_number;
    int numfiles   = data->number_of_files;
    int local_rank = data->file_rank;
    int local_size = data->number_of_ranks_in_file;
    UTILS_DEBUG( "sion_generic_paropen( aid=%d, fname=\"%s\", mode=\"bw,keyval=inline\", chunksize=%zu, fsblksize=%d, global, grank=%u, gsize=%u, filenumber=%d, numfiles=%d, lrank=%d, lsize=%d, fp=NULL, newfname=NULL )",
                 data->api_id,
                 file_type_data->symbolic_file_path,
                 ( size_t )chunksize,
                 fsblksize,
                 global_rank,
                 global_size,
                 filenumber,
                 numfiles,
                 local_rank,
                 local_size );
    file_type_data->sion_id = sion_generic_paropen(
        data->api_id,
        file_type_data->symbolic_file_path,
        "bw,keyval=inline",
        &chunksize, /* w: in, r: out, in Byte. */
        &fsblksize, /* w: in | -1 unknown, r: out */
        &data->global_comm_proxy,
        global_rank,
        global_size,
        &filenumber, /* w: in, r: out */
        &numfiles,   /* w: in, r: out */
        &local_rank, /* w: in, r: out */
        &local_size, /* w: in, r: out */
        NULL,        /* file pointer, NULL, when using sion_write etc. */
        &file_type_data->file_path /* Actual file name, NULL optional. */ );

    UTILS_DEBUG( "sion_generic_paropen( ..., grank=%u, gsize=%u, ... ) = ( sid=%d )",
                 global_rank,
                 global_size,
                 file_type_data->sion_id );

    if ( file_type_data->sion_id < 0 )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Can't open SION file: %s",
                            file_type_data->symbolic_file_path );
    }

    return OTF2_SUCCESS;
}

OTF2_ErrorCode
otf2_sion_open_read( OTF2_Archive* archive,
                     OTF2_FileMode fileMode,
                     OTF2_FileType fileType )
{
    /* Open the SION file handle */
    uint32_t global_rank = 0;
    uint32_t global_size = 0;

    OTF2_FileSubstrateData*         data           = archive->per_substrate_data[ OTF2_SUBSTRATE_SION ];
    OTF2_FileSubstrateFileTypeData* file_type_data = archive->per_filetype_data[ fileType ];

    OTF2_ErrorCode    status       = OTF2_SUCCESS;
    OTF2_CallbackCode callback_ret = OTF2_CALLBACK_SUCCESS;

    /* Query the global-reader hint early, to catch any errors early too. */
    OTF2_Boolean global_reader_hint;
    status = otf2_archive_query_hint( archive,
                                      OTF2_HINT_GLOBAL_READER,
                                      &global_reader_hint );
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status, "Can't query global-reader hint." );
    }

    /* Get global/local rank and size. */
    callback_ret = otf2_collectives_get_rank( archive,
                                              archive->global_comm_context,
                                              &global_rank );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective get rank failed." );
    }

    callback_ret = otf2_collectives_get_size( archive,
                                              archive->global_comm_context,
                                              &global_size );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective get size failed." );
    }

    /* Check if the master file actually exists. If it doesn't the sion paropen
     * call for non-root ranks will block in an bcast collective.
     */
    if ( OTF2_COLLECTIVES_ROOT == global_rank )
    {
        errno = 0;
        FILE* file_exists = fopen( file_type_data->symbolic_file_path, "r" );
        status = UTILS_Error_FromPosix( errno );
        if ( file_exists )
        {
            fclose( file_exists );
        }
    }
    callback_ret = otf2_collectives_bcast_error( archive,
                                                 archive->global_comm_context,
                                                 &status,
                                                 OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status,
                            "Master SION file does not exists: %s",
                            file_type_data->symbolic_file_path );
    }

    /* Collect the ranks to open. */
    uint8_t* ranks_to_open = calloc( data->number_of_ranks,
                                     sizeof( *ranks_to_open ) );
    uint32_t number_of_ranks_to_open = 0;
    for ( uint32_t i = 0; i < archive->locations_number; i++ )
    {
        UTILS_DEBUG_PRINTF( OTF2_DEBUG_SION_RANK_MAP,
                            "Checking location %" PRIu64 " in rank-map",
                            archive->locations[ i ].location_id );
        uint64_t global_id;
        if ( OTF2_SUCCESS != OTF2_IdMap_GetGlobalIdSave( data->location_to_rank_map,
                                                         archive->locations[ i ].location_id,
                                                         &global_id ) )
        {
            continue;
        }

        if ( global_id >= data->number_of_ranks )
        {
            continue;
        }

        if ( !ranks_to_open[ global_id ] )
        {
            number_of_ranks_to_open++;
        }
        UTILS_DEBUG_PRINTF( OTF2_DEBUG_SION_RANK_MAP,
                            "Selecting SION rank %d because of location %" PRIu64,
                            global_id,
                            archive->locations[ i ].location_id );
        ranks_to_open[ global_id ] = 1;
    }
    UTILS_DEBUG_PRINTF( OTF2_DEBUG_SION_RANK_MAP,
                        "Total number of SION ranks to open %u for %u locations",
                        number_of_ranks_to_open,
                        archive->locations_number );

    /* Create the rank array for SION */
    int* global_ranks_to_open = calloc( number_of_ranks_to_open,
                                        sizeof( *global_ranks_to_open ) );
    number_of_ranks_to_open = 0;
    for ( uint32_t i = 0; i < data->number_of_ranks; i++ )
    {
        if ( !ranks_to_open[ i ] )
        {
            continue;
        }

        global_ranks_to_open[ number_of_ranks_to_open++ ] = i;
    }
    free( ranks_to_open );

    /* Check that all ranks don't use the mapped open call */
    uint8_t dont_use_mapped_open = ( data->number_of_ranks == global_size &&
                                     number_of_ranks_to_open == 1 &&
                                     global_ranks_to_open[ 0 ] == global_rank &&
                                     archive->collective_callbacks->otf2_create_local_comm );
    UTILS_DEBUG( "can normal paropen: %s", dont_use_mapped_open ? "yes" : "no" );

    uint8_t* all_dont_use_mapped_open = NULL;
    if ( OTF2_COLLECTIVES_ROOT == global_rank )
    {
        all_dont_use_mapped_open =
            calloc( global_size, sizeof( *all_dont_use_mapped_open ) );
    }

    /* Allreduce op=AND */
    callback_ret = otf2_collectives_gather( archive,
                                            archive->global_comm_context,
                                            &dont_use_mapped_open,
                                            all_dont_use_mapped_open,
                                            1,
                                            OTF2_TYPE_UINT8,
                                            OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective gather failed." );
    }
    if ( OTF2_COLLECTIVES_ROOT == global_rank )
    {
        for ( uint32_t i = 0; i < global_size; i++ )
        {
            dont_use_mapped_open = dont_use_mapped_open && all_dont_use_mapped_open[ i ];
        }
    }
    free( all_dont_use_mapped_open );
    callback_ret = otf2_collectives_bcast( archive,
                                           archive->global_comm_context,
                                           &dont_use_mapped_open,
                                           1,
                                           OTF2_TYPE_UINT8,
                                           OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }

    /* save some state in data, so that the create_local_commgroup_cb
     * knows what todo
     */
    data->fileModeForLocalCommCreate = fileMode;
    data->fileTypeForLocalCommCreate = fileType;
    UTILS_DEBUG( "paropen mode: %s", dont_use_mapped_open ? "normal" : "mapped" );
    if ( dont_use_mapped_open )
    {
        sion_int64 chunksize  = -1;
        sion_int32 fsblksize  = -1;
        int        numfiles   = -1;
        int        filenumber = -1;
        int        lrank      = -1;
        int        lsize      = -1;

        UTILS_DEBUG( "sion_generic_paropen( aid=%d, fname=\"%s\", mode=\"br,keyval=unknown\", chunksize=&, fsblksize=&, global, grank=%u, gsize=%u, filenumber=&, numfiles=&, lrank=&, lsize=&, fp=NULL, newfname=NULL )",
                     data->api_id,
                     file_type_data->symbolic_file_path,
                     global_rank,
                     global_size );

        file_type_data->sion_id = sion_generic_paropen(
            data->api_id,
            file_type_data->symbolic_file_path,
            "br,keyval=unknown",
            &chunksize, /* w: in, r: out, in Byte. */
            &fsblksize, /* w: in | -1 unknown, r: out */
            &data->global_comm_proxy,
            global_rank,
            global_size,
            &filenumber, /* w: in, r: out */
            &numfiles,   /* w: in, r: out */
            &lrank,      /* w: in, r: out */
            &lsize,      /* w: in, r: out */
            NULL,        /* file pointer, NULL, when using sion_write etc. */
            &file_type_data->file_path /* Actual file name, NULL optional. */ );

        UTILS_DEBUG( "sion_generic_paropen( ..., grank=%u, gsize=%u, ... ) = ( sid=%d, chunksize=%zu, fsblksize=%d, filenumber=%d, numfiles=%d, lrank=%d, lsize=%d )",
                     global_rank,
                     global_size,
                     file_type_data->sion_id,
                     ( size_t )chunksize, fsblksize,
                     filenumber, numfiles,
                     lrank, lsize );
    }
    else
    {
        file_type_data->mapped_open = true;

        sion_int64* chunksizes       = NULL;
        sion_int32  fsblksize        = 0;
        int         numfiles         = 0;
        int*        mapping_filenrs  = NULL;
        int*        mapping_lranks   = NULL;
        int         sion_nlocaltasks = number_of_ranks_to_open;

        UTILS_DEBUG( "sion_generic_paropen_mapped( aid=%d, fname=\"%s\", mode=\"br,keyval=unknown\", numfiles=&, global, grank=%u, gsize=%u, nlocaltasks=%d, globalranks=[], chunksizes=&[], mapping_filenrs=&[], mapping_lranks=&[], fsblksize=&, fp=NULL )",
                     data->api_id,
                     file_type_data->symbolic_file_path,
                     global_rank,
                     global_size,
                     sion_nlocaltasks );

        for ( int i = 0; i < sion_nlocaltasks; i++ )
        {
            UTILS_DEBUG( "sion_generic_paropen_mapped( ..., grank=%u, gsize=%u, ..., globalranks[%d]=%d, ... )",
                         global_rank, global_size,
                         i, global_ranks_to_open[ i ] );
        }

        file_type_data->sion_id = sion_generic_paropen_mapped(
            data->api_id,
            file_type_data->symbolic_file_path,
            "br,keyval=unknown",      /* input */
            &numfiles,                /* output */
            &data->global_comm_proxy, /* input */
            global_rank,              /* input */
            global_size,              /* input */
            &sion_nlocaltasks,        /* input */
            &global_ranks_to_open,    /* input */
            &chunksizes,              /* output */
            &mapping_filenrs,         /* output */
            &mapping_lranks,          /* output */
            &fsblksize,               /* output */
            NULL );

        UTILS_DEBUG( "sion_generic_paropen_mapped( ..., grank=%u, gsize=%u, ... ) = ( sid=%d, numfiles=%d, fsblksize=%d )",
                     global_rank, global_size,
                     file_type_data->sion_id,
                     numfiles,
                     fsblksize );

        if ( 0 <= file_type_data->sion_id )
        {
            for ( int i = 0; i < sion_nlocaltasks; i++ )
            {
                UTILS_DEBUG( "sion_generic_paropen_mapped( ..., grank=%u, gsize=%u, ... ) = ( chunksizes[%d]=%zu, mapping_filenrs[%d]=%d, mapping_lranks[%d]=%d )",
                             global_rank, global_size,
                             i, ( size_t )chunksizes[ i ],
                             i, mapping_filenrs[ i ],
                             i, mapping_lranks[ i ] );
            }
        }

        free( mapping_filenrs );
        free( mapping_lranks );
        free( chunksizes );
    }
    free( global_ranks_to_open );

    UTILS_DEBUG( "SION id: %d", file_type_data->sion_id );
    if ( file_type_data->sion_id < 0 )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Can't open SION file: %s",
                            file_type_data->symbolic_file_path );
    }

    /* Determine whether we use the keyval API. */
    file_type_data->keyval = false;
    int keyval_mode = sion_get_keyval_mode( file_type_data->sion_id );
    UTILS_DEBUG( "sion_get_keyval_mode( sid=%d ) = %d", file_type_data->sion_id, keyval_mode );
    if ( keyval_mode == SION_KEYVAL_UNKNOWN )
    {
        UTILS_WARNING( "SION id %d: unknown keyval mode", file_type_data->sion_id );
    }
    else if ( keyval_mode != SION_KEYVAL_NONE )
    {
        file_type_data->keyval = true;
        if ( 1 <= number_of_ranks_to_open )
        {
            UTILS_DEBUG( "sion_key_full_scan( sid=%d )", file_type_data->sion_id );
            int sion_status = sion_key_full_scan( file_type_data->sion_id );
            UTILS_DEBUG( "sion_key_full_scan( sid=%d ) = %d", file_type_data->sion_id, sion_status );
            if ( sion_status != SION_SUCCESS )
            {
                UTILS_WARNING( "SION id %d: can't do a full key scan: %d",
                               file_type_data->sion_id );
            }
        }
    }

    /* Determine whether we duplicate sids for each locations. */
    if ( global_reader_hint )
    {
        file_type_data->dup = false;
    }
    else
    {
        file_type_data->dup = true;
    }


    return OTF2_SUCCESS;
}

/**
 * @brief
 *
 * @threadsafety  Caller holds the archive lock.
 */
OTF2_ErrorCode
otf2_file_substrate_sion_close_file_type( OTF2_Archive* archive,
                                          OTF2_FileType fileType )
{
    UTILS_DEBUG_ENTRY( "%d", fileType );

    UTILS_ASSERT( archive );
    if ( !archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Substrate not initialized!" );
    }

    if ( !otf2_file_type_needs_location_id( fileType ) )
    {
        return otf2_file_substrate_posix_close_file_type( archive,
                                                          fileType );
    }

    if ( !archive->per_filetype_data[ fileType ] )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "File type was not opened yet: %hhu", fileType );
    }

    OTF2_FileSubstrateFileTypeData* file_type_data = archive->per_filetype_data[ fileType ];

    int ret;
    switch ( file_type_data->file_mode )
    {
        case OTF2_FILEMODE_WRITE:
            UTILS_DEBUG( "sion_generic_parclose( sid=%d )", file_type_data->sion_id );
            ret = sion_generic_parclose( file_type_data->sion_id );
            break;
        case OTF2_FILEMODE_READ:
            if ( file_type_data->mapped_open )
            {
                UTILS_DEBUG( "sion_generic_parclose_mapped( sid=%d )", file_type_data->sion_id );
                ret = sion_generic_parclose_mapped( file_type_data->sion_id );
            }
            else
            {
                UTILS_DEBUG( "sion_generic_parclose( sid=%d )", file_type_data->sion_id );
                ret = sion_generic_parclose( file_type_data->sion_id );
            }
            break;
        default:
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Unknown file mode." );
            break;
    }
    OTF2_ErrorCode status = OTF2_SUCCESS;
    /* SION returns 1 if close was correctly executed. */
    if ( ret != 1 )
    {
        status = UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                              "Couldn't close SION file: %d", ret );
    }

    otf2_lock_destroy( archive, file_type_data->lock );
    free( file_type_data->file_path );
    free( file_type_data->symbolic_file_path );
    free( file_type_data );
    archive->per_filetype_data[ fileType ] = NULL;

    return status;
}


/**
 * @brief
 *
 * @threadsafety Locks the FileType.
 */
OTF2_ErrorCode
otf2_file_substrate_sion_open_file( OTF2_Archive*    archive,
                                    OTF2_FileMode    fileMode,
                                    OTF2_FileType    fileType,
                                    OTF2_LocationRef locationId,
                                    OTF2_File**      file )
{
    UTILS_DEBUG_ENTRY( "%d, %d, %" PRIu64, fileMode, fileType, locationId );

    UTILS_ASSERT( archive );
    UTILS_ASSERT( file );

    if ( !archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Substrate not initialized!" );
    }

    if ( !otf2_file_type_needs_location_id( fileType ) )
    {
        return otf2_file_substrate_posix_open_file( archive,
                                                    fileMode,
                                                    fileType,
                                                    locationId,
                                                    file );
    }

    OTF2_FileSubstrateData* data = archive->per_substrate_data[ OTF2_SUBSTRATE_SION ];

    UTILS_ASSERT( archive->per_filetype_data[ fileType ] );

    OTF2_FileSubstrateFileTypeData* file_type_data =
        archive->per_filetype_data[ fileType ];

    OTF2_ErrorCode status;

    OTF2_SION_FILE_LOCK( archive, file_type_data );

    int rank = -1;
    /* Get rank for this location from the rank-map */
    if ( fileMode == OTF2_FILEMODE_READ && file_type_data->mapped_open )
    {
        uint64_t global_id;
        if ( OTF2_IdMap_GetGlobalIdSave( data->location_to_rank_map,
                                         locationId,
                                         &global_id ) == OTF2_SUCCESS )
        {
            rank = global_id;
        }
    }

    status = otf2_file_sion_open( file_type_data,
                                  rank,
                                  locationId,
                                  file );

    OTF2_SION_FILE_UNLOCK( archive, file_type_data );

    return status;
}


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_sion_close_file( OTF2_File* file )
{
    UTILS_DEBUG_ENTRY( "%d, %" PRIu64, file->file_type, file->location_id );

    UTILS_ASSERT( file );

    if ( !otf2_file_type_needs_location_id( file->file_type ) )
    {
        return otf2_file_substrate_posix_close_file( file );
    }

    /** @threadsafety FileType lock will be taken inside otf2_file_sion_close,
     *  as we don't have the otf2_file_sion type available.
     */
    return otf2_file_sion_close( file );
}
