/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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

/** @internal
 *
 *  @file
 *
 *  @brief      This layer is completely internal to the OTF2 library and should
 *              not be exposed to the end user. Its purpose is to abstract the
 *              file interaction via the specific file substrate.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#define OTF2_DEBUG_MODULE_NAME SION
#include <UTILS_Debug.h>

#include <sion.h>

#include "otf2_internal.h"
#include "otf2_lock.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"

#include "OTF2_File.h"
#include "otf2_file_int.h"

#include "otf2_file_substrate_sion_int.h"


/** @brief File handle to interact with sion files.
 *  Please see otf2_file_sion_struct for a detailed description. */
typedef struct otf2_file_sion_struct otf2_file_sion;

/** @brief File handle to interact with Sion files. */
struct otf2_file_sion_struct
{
    OTF2_File super;

    /* ___ Information owned by the SION file substrate. ___ */

    /** SION ID. */
    int sion_id;

    /** The rank from what we want to read. -1 if not mapped.  */
    int rank;

    /** Check if key-value feature is enabled. */
    bool keyval;

    /** Check if SIONlib file handles are duplicated for each rank/key. */
    bool dup;
};

static OTF2_ErrorCode
otf2_file_sion_reset( OTF2_File* file );

static OTF2_ErrorCode
otf2_file_sion_write( OTF2_File*  file,
                      const void* buffer,
                      uint64_t    size );

static OTF2_ErrorCode
otf2_file_sion_read( OTF2_File* file,
                     void*      buffer,
                     uint64_t   size );

static OTF2_ErrorCode
otf2_file_sion_seek( OTF2_File* file,
                     int64_t    offset );

static OTF2_ErrorCode
otf2_file_sion_get_file_size( OTF2_File* file,
                              uint64_t*  size );

static OTF2_ErrorCode
otf2_file_sion_seek_current_rank( otf2_file_sion* sionFile );

static OTF2_ErrorCode
otf2_file_sion_seek_current_key( otf2_file_sion* sionFile );


/** @brief Register a handle for file substrate.
 *
 *  Allocates a handle, sets the according values and registers the function
 *  pointers to its file operations.
 *
 *  Please note: The actual sion file open is executed in otf2_file_sion_init
 *  since all callbacks and parameter must be set first.
 *
 *  @param fileName         Name of the file.
 *  @param mode             Method to interact with the file.
 *  @param substrate        File substrate.
 *
 *  @threadsafety           Caller holds the FileType lock.
 *
 *  @return                 Returns a handle to the file if successful, NULL
 *                          otherwise.
 */
OTF2_ErrorCode
otf2_file_sion_open( OTF2_FileSubstrateFileTypeData* fileTypeData,
                     int                             rank,
                     OTF2_LocationRef                locationId,
                     OTF2_File**                     file )
{
    otf2_file_sion* sion_file =
        ( otf2_file_sion* )calloc( 1, sizeof( otf2_file_sion ) );
    if ( !sion_file )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Could not allocate memory for file handle!" );
    }

    sion_file->sion_id = fileTypeData->sion_id;
    sion_file->rank    = rank;
    sion_file->keyval  = fileTypeData->keyval;
    sion_file->dup     = false;

    /* Duplicate sion handle. */
    if ( fileTypeData->dup )
    {
        sion_file->dup = true;
        int dup_mode = SION_DUP_RANK;
        if ( sion_file->keyval )
        {
            dup_mode = SION_DUP_RANK_KEY;
        }

        UTILS_DEBUG( "sion_dup( sid=%d, SION_DUP_%s, rank=%d, key=%" PRIu64 " )",
                     fileTypeData->sion_id,
                     dup_mode == SION_DUP_RANK ? "RANK" : "RANK_KEY",
                     rank, locationId );
        sion_file->sion_id = sion_dup( fileTypeData->sion_id, dup_mode, rank, locationId );
        UTILS_DEBUG( "sion_dup( sid=%d, SION_DUP_%s, rank=%d, key=%" PRIu64 " ) = %d",
                     fileTypeData->sion_id,
                     dup_mode == SION_DUP_RANK ? "RANK" : "RANK_KEY",
                     rank, locationId, sion_file->sion_id );
        if ( sion_file->sion_id < 0 )
        {
            free( sion_file );
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Can't duplicate SION id." );
        }
    }

    /* Register function pointers, for real SION files */
    sion_file->super.reset         = otf2_file_sion_reset;
    sion_file->super.write         = otf2_file_sion_write;
    sion_file->super.read          = otf2_file_sion_read;
    sion_file->super.seek          = otf2_file_sion_seek;
    sion_file->super.get_file_size = otf2_file_sion_get_file_size;

    *file = &sion_file->super;

    return OTF2_SUCCESS;
}


/** @name File operations for Sion file substrate.
 *  Please see the standard interface functions for a detailed description.
 *  @{ */
OTF2_ErrorCode
otf2_file_sion_close( OTF2_File* file )
{
    UTILS_ASSERT( file );

    /* Get the original substrate structure. */
    otf2_file_sion* sion_file = ( otf2_file_sion* )file;

    OTF2_ErrorCode status = OTF2_SUCCESS;

    UTILS_ASSERT( file->archive );
    OTF2_Archive* archive = file->archive;

    UTILS_ASSERT( archive->per_filetype_data[ file->file_type ] );
    OTF2_FileSubstrateFileTypeData* file_type_data =
        archive->per_filetype_data[ file->file_type ];

    if ( sion_file->dup )
    {
        OTF2_SION_FILE_LOCK( archive, file_type_data );

        UTILS_DEBUG( "sion_dedup( sid=%d )", sion_file->sion_id );
        int sion_status = sion_dedup( sion_file->sion_id );
        UTILS_DEBUG( "sion_dedup( sid=%d ) = %d", sion_file->sion_id, sion_status );
        if ( SION_SUCCESS != sion_status )
        {
            status = UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                  "Dedup of sion descriptor failed." );
        }

        OTF2_SION_FILE_UNLOCK( archive, file_type_data );
    }

    free( sion_file );

    return status;
}


OTF2_ErrorCode
otf2_file_sion_reset( OTF2_File* file )
{
    return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                        "Not supported!" );
}


OTF2_ErrorCode
otf2_file_sion_write( OTF2_File*  file,
                      const void* buffer,
                      uint64_t    size )
{
    UTILS_ASSERT( file );

    /* Get the original substrate structure. */
    otf2_file_sion* sion_file = ( otf2_file_sion* )file;

    uint64_t key = file->location_id;

    UTILS_ASSERT( file->archive );
    OTF2_Archive* archive = file->archive;

    UTILS_ASSERT( archive->per_filetype_data[ file->file_type ] );
    OTF2_FileSubstrateFileTypeData* file_type_data =
        archive->per_filetype_data[ file->file_type ];

    OTF2_SION_FILE_LOCK( archive, file_type_data );

    UTILS_DEBUG( "sion_fwrite_key( <buf>, key=%" PRIu64 ", 1, nitems=%zu, sid=%d )",
                 key, size, sion_file->sion_id );
    size_t status = sion_fwrite_key( buffer, key, 1, size, sion_file->sion_id );
    UTILS_DEBUG( "sion_fwrite_key( <buf>, key=%" PRIu64 ", 1, nitems=%zu, sid=%d ) = %zu",
                 key, size, sion_file->sion_id, status );

    OTF2_SION_FILE_UNLOCK( archive, file_type_data );

    if ( status != size )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Write to sion file failed." );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_file_sion_read( OTF2_File* file,
                     void*      buffer,
                     uint64_t   size )
{
    UTILS_ASSERT( file );

    /* Get the original substrate structure. */
    otf2_file_sion* sion_file = ( otf2_file_sion* )file;

    UTILS_ASSERT( file->archive );
    OTF2_Archive* archive = file->archive;

    UTILS_ASSERT( archive->per_filetype_data[ file->file_type ] );
    OTF2_FileSubstrateFileTypeData* file_type_data =
        archive->per_filetype_data[ file->file_type ];

    /* Seek to get correct sion rank and key if not in dup mode. */
    OTF2_ErrorCode status = OTF2_ERROR_INVALID;
    if ( !sion_file->dup )
    {
        OTF2_SION_FILE_LOCK( archive, file_type_data );

        status = otf2_file_sion_seek_current_rank( sion_file );
        if ( status != OTF2_SUCCESS )
        {
            UTILS_ERROR( status, "Seek in sion file failed." );
            goto out;
        }

        if ( sion_file->keyval )
        {
            status = otf2_file_sion_seek_current_key( sion_file );
            if ( status != OTF2_SUCCESS )
            {
                UTILS_ERROR( status, "Seek in sion file failed." );
                goto out;
            }
        }
    }

    size_t sion_status = 0; /* These are actually the read bytes. */

    /* The read operation reads n times 1 byte instead of 1 times n bytes to
     * identify the read of an empty or already finished file. */
    if ( sion_file->keyval )
    {
        uint64_t key = file->location_id;

        UTILS_DEBUG( "sion_fread_key( <buf>, key=%" PRIu64 ", 1, nitems=%zu, sid=%d )",
                     key, size, sion_file->sion_id );
        sion_status = sion_fread_key( buffer, key, 1, size, sion_file->sion_id );
        UTILS_DEBUG( "sion_fread_key( <buf>, key=%" PRIu64 ", 1, nitems=%zu, sid=%d ) = %zu",
                     key, size, sion_file->sion_id, sion_status );
    }
    else
    {
        UTILS_DEBUG( "sion_fread( <buf>, 1, %zu, %d )",
                     size, sion_file->sion_id );
        sion_status = sion_fread( buffer, 1, size, sion_file->sion_id );
        UTILS_DEBUG( "sion_fread( <buf>, 1, %zu, %d ) = %zu",
                     size, sion_file->sion_id, sion_status );
    }

    if ( sion_status == 0 )
    {
        status = UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                              "Found unexpected end of file." );
        goto out;
    }

    status = OTF2_SUCCESS;

out:
    if ( !sion_file->dup )
    {
        OTF2_SION_FILE_UNLOCK( archive, file_type_data );
    }

    return status;
}


OTF2_ErrorCode
otf2_file_sion_seek( OTF2_File* file,
                     int64_t    offset )
{
    UTILS_ASSERT( file );

    /* Get the original substrate structure. */
    otf2_file_sion* sion_file = ( otf2_file_sion* )file;

    UTILS_ASSERT( file->archive );
    OTF2_Archive* archive = file->archive;

    UTILS_ASSERT( archive->per_filetype_data[ file->file_type ] );
    OTF2_FileSubstrateFileTypeData* file_type_data =
        archive->per_filetype_data[ file->file_type ];

    if ( !sion_file->dup )
    {
        OTF2_SION_FILE_LOCK( archive, file_type_data );

        OTF2_ErrorCode status = otf2_file_sion_seek_current_rank( sion_file );
        if ( status != OTF2_SUCCESS )
        {
            UTILS_ERROR( status, "Seek in sion file failed." );
            goto out;
        }
    }

    OTF2_ErrorCode status      = OTF2_ERROR_INVALID;
    int            sion_status = SION_NOT_SUCCESS;
    if ( sion_file->keyval )
    {
        /* Seek to get correct sion rank if not in dup mode. */
        OTF2_ErrorCode status = OTF2_ERROR_INVALID;
        if ( !sion_file->dup )
        {
            status = otf2_file_sion_seek_current_rank( sion_file );
            if ( status != OTF2_SUCCESS )
            {
                UTILS_ERROR( status, "Seek in sion file failed." );
                goto out;
            }
        }

        uint64_t key = file->location_id;

        UTILS_DEBUG( "sion_seek_key( sid=%d, key=%" PRIu64 ", SION_ABSOLUTE_POS, pos=%" PRId64 " )",
                     sion_file->sion_id, key, offset );
        sion_status = sion_seek_key( sion_file->sion_id,
                                     key,
                                     SION_ABSOLUTE_POS,
                                     offset );
        UTILS_DEBUG( "sion_seek_key( sid=%d, key=%" PRIu64 ", SION_ABSOLUTE_POS, pos=%" PRId64 " ) = %d",
                     sion_file->sion_id, key, offset, sion_status );
    }
    else
    {
        UTILS_DEBUG( "sion_seek( sid=%d, rank=%d, SION_ABSOLUTE_POS, pos=" PRId64 " )",
                     sion_file->sion_id, sion_file->rank, offset );
        sion_status = sion_seek( sion_file->sion_id,
                                 sion_file->rank,
                                 SION_ABSOLUTE_POS,
                                 offset );
        UTILS_DEBUG( "sion_seek( sid=%d, rank=%d, SION_ABSOLUTE_POS, pos=" PRId64 " ) = %d",
                     sion_file->sion_id, sion_file->rank, offset, sion_status );
    }

    if ( sion_status != SION_SUCCESS )
    {
        status = UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                              "Seek in sion file failed." );
        goto out;
    }

    status = OTF2_SUCCESS;

out:
    if ( !sion_file->dup )
    {
        OTF2_SION_FILE_UNLOCK( archive, file_type_data );
    }

    return status;
}


OTF2_ErrorCode
otf2_file_sion_get_file_size( OTF2_File* file,
                              uint64_t*  size )
{
    UTILS_ASSERT( file );

    /* Get the original substrate structure. */
    otf2_file_sion* sion_file = ( otf2_file_sion* )file;

    UTILS_ASSERT( file->archive );
    OTF2_Archive* archive = file->archive;

    UTILS_ASSERT( archive->per_filetype_data[ file->file_type ] );
    OTF2_FileSubstrateFileTypeData* file_type_data =
        archive->per_filetype_data[ file->file_type ];

    OTF2_ErrorCode status = OTF2_ERROR_INVALID;

    /* Seek to get correct sion rank and key if not in dup mode. */
    if ( !sion_file->dup )
    {
        OTF2_SION_FILE_LOCK( archive, file_type_data );

        status = otf2_file_sion_seek_current_rank( sion_file );
        if ( status != OTF2_SUCCESS )
        {
            UTILS_ERROR( status, "Seek in sion file failed." );
            goto out;
        }
    }

    int sion_status = SION_NOT_SUCCESS;
    if ( sion_file->keyval )
    {
        /* Seek to get correct sion rank and key if not in dup mode. */
        if ( !sion_file->dup )
        {
            status = otf2_file_sion_seek_current_rank( sion_file );
            if ( status != OTF2_SUCCESS )
            {
                UTILS_ERROR( status, "Seek in sion file failed." );
                goto out;
            }
        }

        uint64_t key = file->location_id;

        sion_key_stat_t sion_stat;

        UTILS_DEBUG( "sion_key_get_stat( sid=%d, key=%" PRIu64 ", <stat-buf> )",
                     sion_file->sion_id, key );
        sion_status = sion_key_get_stat( sion_file->sion_id, key, &sion_stat );
        UTILS_DEBUG( "sion_key_get_stat( sid=%d, key=%" PRIu64 ", <stat-buf> ) = %d",
                     sion_file->sion_id, key, sion_status );

        if ( sion_status != SION_SUCCESS )
        {
            status = UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                  "Get size of sion file failed" );
            goto out;
        }

        *size = sion_stat.total_size;
    }
    else
    {
        int         current_chunk_num = 0;
        sion_int64  current_pos       = 0;
        int         max_chunks        = 0;
        sion_int64* chunk_sizes       = NULL;

        UTILS_DEBUG( "sion_get_current_location( sid=%d, ... )",
                     sion_file->sion_id );
        sion_status = sion_get_current_location( sion_file->sion_id,
                                                 &current_chunk_num,
                                                 &current_pos,
                                                 &max_chunks,
                                                 &chunk_sizes );
        UTILS_DEBUG( "sion_get_current_location( sid=%d, ... ) = %d",
                     sion_file->sion_id, sion_status );

        if ( sion_status != SION_SUCCESS )
        {
            status = UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                  "Get size of sion file failed" );
            goto out;
        }

        /* Sum up all chunk sizes to get total size. */
        uint64_t total_size = 0;
        for ( int i = 0; i < max_chunks; i++ )
        {
            total_size += chunk_sizes[ i ];
        }

        *size = total_size;
    }

    status = OTF2_SUCCESS;

out:
    if ( !sion_file->dup )
    {
        OTF2_SION_FILE_UNLOCK( archive, file_type_data );
    }
    return status;
}

/**
 *  @threadsafety Caller holds the FileType lock.
 */
OTF2_ErrorCode
otf2_file_sion_seek_current_rank( otf2_file_sion* sionFile )
{
    int status = SION_NOT_SUCCESS;
    if ( sionFile->rank >= 0 )
    {
        UTILS_DEBUG( "sion_seek( sid=%d, rank=%d, SION_CURRENT_CHUNK, SION_CURRENT_POS )",
                     sionFile->sion_id,
                     sionFile->rank );
        status = sion_seek( sionFile->sion_id,
                            sionFile->rank,
                            SION_CURRENT_BLK,
                            SION_CURRENT_POS );
        UTILS_DEBUG( "sion_seek( sid=%d, rank=%d, SION_CURRENT_CHUNK, SION_CURRENT_POS ) = %d",
                     sionFile->sion_id,
                     sionFile->rank,
                     status );
        if ( status != SION_SUCCESS )
        {
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Seek to current SIONlib rank failed." );
        }
    }

    return OTF2_SUCCESS;
}

/**
 *  @threadsafety Caller holds the FileType lock.
 */
OTF2_ErrorCode
otf2_file_sion_seek_current_key( otf2_file_sion* sionFile )
{
    int status = SION_NOT_SUCCESS;

    UTILS_DEBUG( "sion_seek_key( sid=%d, key=%" PRIu64 ", SION_CURRENT_CHUNK, SION_CURRENT_POS )",
                 sionFile->sion_id,
                 sionFile->super.location_id );
    status = sion_seek_key( sionFile->sion_id,
                            sionFile->super.location_id,
                            SION_CURRENT_CHUNK,
                            SION_CURRENT_POS );
    UTILS_DEBUG( "sion_seek_key( sid=%d, key=%" PRIu64 ", SION_CURRENT_CHUNK, SION_CURRENT_POS ) = %d",
                 sionFile->sion_id,
                 sionFile->super.location_id,
                 status );

    if ( status != SION_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Seek to current SIONlib key failed." );
    }

    return OTF2_SUCCESS;
}


/** @} */
