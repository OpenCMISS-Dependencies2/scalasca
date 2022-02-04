/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
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
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"

#include "OTF2_File.h"
#include "otf2_file_posix.h"
#include "otf2_file_substrate.h"


/** @brief Implementation of per archive file substrate data.
 */
struct OTF2_FileSubstrateData
{
    int dummy;
};


/** @brief Implementation of file type substrate data.
 */
struct OTF2_FileSubstrateFileTypeData
{
    int dummy;
};


/**
 * @brief Initializes the substrate.
 *
 * Called once for an archive. Substrate can use @a *substrateData, for
 * substrate specific data.
 */
OTF2_ErrorCode
otf2_file_substrate_posix_initialize( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    OTF2_FileSubstrateData* data = malloc( sizeof( *data ) );
    if ( !data )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_ALLOC_FAILED,
                            "Allocation of global file substrate data failed" );
    }

    archive->per_substrate_data[ OTF2_SUBSTRATE_POSIX ] = data;

    return OTF2_SUCCESS;
}


/**
 * @brief Finalizes the substrate.
 *
 * Called once for an archive. Substrate should release @a substrateData, if
 * it was set in @a otf2_file_substrate_initialize.
 */
OTF2_ErrorCode
otf2_file_substrate_posix_finalize( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );
    if ( !archive->per_substrate_data[ OTF2_SUBSTRATE_POSIX ] )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Substrate not initialized!" );
    }

    OTF2_FileSubstrateData* data = archive->per_substrate_data[ OTF2_SUBSTRATE_POSIX ];
    free( data );
    archive->per_substrate_data[ OTF2_SUBSTRATE_POSIX ] = NULL;

    return OTF2_SUCCESS;
}


/**
 * @brief Called when the collective context was set by the user.
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_open( OTF2_Archive* archive,
                                OTF2_FileMode fileMode )
{
    UTILS_ASSERT( archive );

    return OTF2_SUCCESS;
}


/**
 * @brief Called before closing the archive.
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_close( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    return OTF2_SUCCESS;
}


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_open_file_type( OTF2_Archive* archive,
                                          OTF2_FileMode fileMode,
                                          OTF2_FileType fileType )
{
    UTILS_ASSERT( archive );

    return OTF2_SUCCESS;
}


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_close_file_type( OTF2_Archive* archive,
                                           OTF2_FileType fileType )
{
    UTILS_ASSERT( archive );

    return OTF2_SUCCESS;
}


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_open_file( OTF2_Archive*    archive,
                                     OTF2_FileMode    fileMode,
                                     OTF2_FileType    fileType,
                                     OTF2_LocationRef locationId,
                                     OTF2_File**      file )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( file );

    return otf2_file_posix_open( archive,
                                 fileMode,
                                 fileType,
                                 locationId,
                                 file );
}


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_close_file( OTF2_File* file )
{
    UTILS_ASSERT( file );

    return otf2_file_posix_close( file );
}
