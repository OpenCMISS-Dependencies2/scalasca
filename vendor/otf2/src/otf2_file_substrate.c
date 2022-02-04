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
 *  @brief      File substrate layer.
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
#include "otf2_file_int.h"
#include "otf2_file_substrate.h"

#include "otf2_file_substrate_posix.h"
#if HAVE( SIONLIB_SUPPORT )
#include "otf2_file_substrate_sion.h"
#endif
#include "otf2_file_substrate_none.h"

/**
 * @brief Initializes the substrate.
 *
 * Called once for an archive. Substrate can use @a *substrateData, for
 * substrate specific data.
 */
OTF2_ErrorCode
otf2_file_substrate_initialize( OTF2_Archive*      archive,
                                OTF2_FileSubstrate substrate )
{
    UTILS_ASSERT( archive );

    switch ( substrate )
    {
        case OTF2_SUBSTRATE_POSIX:
            return otf2_file_substrate_posix_initialize( archive );

        case OTF2_SUBSTRATE_SION:
            #if HAVE( SIONLIB_SUPPORT )
            return otf2_file_substrate_sion_initialize( archive );
            #else
            return OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED;
            #endif

        case OTF2_SUBSTRATE_NONE:
            return otf2_file_substrate_none_initialize( archive );

        default:
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Unknown file substrate." );
    }
}


/**
 * @brief Finalizes the substrate.
 *
 * Called once for an archive. Substrate should release @a substrateData, if
 * it was set in @a otf2_file_substrate_initialize.
 */
OTF2_ErrorCode
otf2_file_substrate_finalize( OTF2_Archive*      archive,
                              OTF2_FileSubstrate substrate )
{
    UTILS_ASSERT( archive );

    switch ( substrate )
    {
        case OTF2_SUBSTRATE_POSIX:
            return otf2_file_substrate_posix_finalize( archive );

        case OTF2_SUBSTRATE_SION:
            #if HAVE( SIONLIB_SUPPORT )
            return otf2_file_substrate_sion_finalize( archive );
            #else
            return OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED;
            #endif

        case OTF2_SUBSTRATE_NONE:
            return otf2_file_substrate_none_finalize( archive );

        default:
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Unknown file substrate." );
    }
}


/**
 * @brief Called when the collective context was set by the user.
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_open( OTF2_Archive* archive,
                          OTF2_FileMode fileMode )
{
    UTILS_ASSERT( archive );

    switch ( archive->substrate )
    {
        case OTF2_SUBSTRATE_POSIX:
            return otf2_file_substrate_posix_open( archive,
                                                   fileMode );

        case OTF2_SUBSTRATE_SION:
            #if HAVE( SIONLIB_SUPPORT )
            return otf2_file_substrate_sion_open( archive,
                                                  fileMode );
            #else
            return OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED;
            #endif

        case OTF2_SUBSTRATE_NONE:
            return otf2_file_substrate_none_open( archive,
                                                  fileMode );

        default:
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Unknown file substrate." );
    }
}


/**
 * @brief Called before closing the archive.
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_close( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    switch ( archive->substrate )
    {
        case OTF2_SUBSTRATE_POSIX:
            return otf2_file_substrate_posix_close( archive );

        case OTF2_SUBSTRATE_SION:
            #if HAVE( SIONLIB_SUPPORT )
            return otf2_file_substrate_sion_close( archive );
            #else
            return OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED;
            #endif

        case OTF2_SUBSTRATE_NONE:
            return otf2_file_substrate_none_close( archive );

        default:
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Unknown file substrate." );
    }
}


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_open_file_type( OTF2_Archive* archive,
                                    OTF2_FileMode fileMode,
                                    OTF2_FileType fileType )
{
    UTILS_ASSERT( archive );

    switch ( archive->substrate )
    {
        case OTF2_SUBSTRATE_POSIX:
            return otf2_file_substrate_posix_open_file_type( archive,
                                                             fileMode,
                                                             fileType );

        case OTF2_SUBSTRATE_SION:
            #if HAVE( SIONLIB_SUPPORT )
            return otf2_file_substrate_sion_open_file_type( archive,
                                                            fileMode,
                                                            fileType );
            #else
            return OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED;
            #endif

        case OTF2_SUBSTRATE_NONE:
            return otf2_file_substrate_none_open_file_type( archive,
                                                            fileMode,
                                                            fileType );

        default:
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Unknown file substrate." );
    }
}


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_close_file_type( OTF2_Archive* archive,
                                     OTF2_FileType fileType )
{
    UTILS_ASSERT( archive );

    switch ( archive->substrate )
    {
        case OTF2_SUBSTRATE_POSIX:
            return otf2_file_substrate_posix_close_file_type( archive,
                                                              fileType );

        case OTF2_SUBSTRATE_SION:
            #if HAVE( SIONLIB_SUPPORT )
            return otf2_file_substrate_sion_close_file_type( archive,
                                                             fileType );
            #else
            return OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED;
            #endif

        case OTF2_SUBSTRATE_NONE:
            return otf2_file_substrate_none_close_file_type( archive,
                                                             fileType );

        default:
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Unknown file substrate." );
    }
}


/** @brief Open a file.
 *
 *  Open a new file with the given 'fileName'. The passed 'mode' defines whether
 *  the file is opened for write-only access 'OTF2_FILEMODE_WRITE', for
 *  read-only access 'OTF2_FILEMODE_READ', or for read-write access
 *  'OTF2_FILEMODE_MODIFY'. The passed 'substrate' defines the used file
 *  substrate to open and interact with the file.
 *
 *  Calls the specific file open routine defined by the file substrate.
 *
 *  @param fileName         Name of the file.
 *  @param mode             Method to interact with the file. Available values
 *                          are: Available values are: 'OTF2_FILEMODE_WRITE',
 *                          'OTF2_FILEMODE_READ', and 'OTF2_FILEMODE_MODIFY'.
 *  @param substrate        File substrate. Available values at the moment are:
 *                          'OTF2_SUBSTRATE_POSIX' and 'OTF2_SUBSTRATE_NONE'.
 *
 *  @return                 Returns a handle to the file if successful, NULL
 *                          otherwise.
 */
OTF2_ErrorCode
otf2_file_substrate_open_file( OTF2_Archive*    archive,
                               OTF2_FileMode    fileMode,
                               OTF2_FileType    fileType,
                               OTF2_LocationRef location,
                               OTF2_File**      file )
{
    UTILS_ASSERT( archive );

    OTF2_FileSubstrate substrate = archive->substrate;

    /* Validate arguments.
     * If the none substrate is selected, we need a location id for the local
     * defs and event file types. */
    if ( substrate != OTF2_SUBSTRATE_NONE
         && otf2_file_type_needs_location_id( fileType )
         && location == OTF2_UNDEFINED_LOCATION )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid file name!" );
    }

    /* Special case the ANCHOR file when reading,  */
    if ( fileType == OTF2_FILETYPE_ANCHOR
         && substrate == OTF2_SUBSTRATE_UNDEFINED )
    {
        substrate = OTF2_SUBSTRATE_POSIX;
    }

    OTF2_ErrorCode status;
    switch ( substrate )
    {
        case OTF2_SUBSTRATE_POSIX:
            status = otf2_file_substrate_posix_open_file( archive,
                                                          fileMode,
                                                          fileType,
                                                          location,
                                                          file );
            break;

        case OTF2_SUBSTRATE_SION:
            #if HAVE( SIONLIB_SUPPORT )
            status = otf2_file_substrate_sion_open_file( archive,
                                                         fileMode,
                                                         fileType,
                                                         location,
                                                         file );
            break;
            #else
            return UTILS_ERROR( OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED,
                                "Could not find SIONlib installation!" );
            #endif

        case OTF2_SUBSTRATE_NONE:
            status = otf2_file_substrate_none_open_file( archive,
                                                         fileMode,
                                                         fileType,
                                                         location,
                                                         file );
            break;

        /* If substrate does not match any of the above, return NULL. */
        default:
            return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                                "This is no valid file substrate!" );
    }

    if ( OTF2_SUCCESS == status )
    {
        /* This does not fail. */
        otf2_file_initialize( archive,
                              *file,
                              fileType,
                              location );
    }

    return status;
}


/** @brief Close a file.
 *
 *  Calls the specific file close routine defined by the file substrate.
 *
 *  @param file             OTF2 file handle.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
otf2_file_substrate_close_file( OTF2_File* file )
{
    /* Validate arguments. */
    UTILS_ASSERT( file );

    OTF2_ErrorCode status = otf2_file_finalize( file );
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status, "Finalization failed." );
    }

    switch ( file->archive->substrate )
    {
        case OTF2_SUBSTRATE_POSIX:
            return otf2_file_substrate_posix_close_file( file );

        #if HAVE( SIONLIB_SUPPORT )
        case OTF2_SUBSTRATE_SION:
            return otf2_file_substrate_sion_close_file( file );
        #endif

        case OTF2_SUBSTRATE_NONE:
            return otf2_file_substrate_none_close_file( file );

        default:
            /* Should not happen. */
            return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                                "Got an open file, for what we don't have support." );
    }
}
