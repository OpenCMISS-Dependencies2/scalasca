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
#if HAVE( SYS_STAT_H )
#include <sys/stat.h>
#endif

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#define OTF2_DEBUG_MODULE_NAME POSIX
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"

#include "OTF2_File.h"
#include "otf2_file_int.h"


/** @brief File handle to interact with posix files.
 *  Please see otf2_file_posix_struct for a detailed description. */
typedef struct otf2_file_posix_struct otf2_file_posix;

/** @brief File handle to interact with posix files. */
struct otf2_file_posix_struct
{
    OTF2_File super;

    /* ___ Information owned by the POSIX file substrate. ___ */

    /** Own copy of the filename. */
    const char* file_path;

    /** Actual OS file handle. It is NULL if file is currently closed. */
    FILE* file;
    /** Keep file pos when the real file is closed. Is undefined while file is
     *  open, equals 0 before opened for the first time. */
    uint64_t position;
};


OTF2_ErrorCode
otf2_file_posix_close( OTF2_File* file );


static OTF2_ErrorCode
otf2_file_posix_reset( OTF2_File* file );


static OTF2_ErrorCode
otf2_file_posix_write( OTF2_File*  file,
                       const void* buffer,
                       uint64_t    size );


static OTF2_ErrorCode
otf2_file_posix_read( OTF2_File* file,
                      void*      buffer,
                      uint64_t   size );


static OTF2_ErrorCode
otf2_file_posix_seek( OTF2_File* file,
                      int64_t    offset );

static OTF2_ErrorCode
otf2_file_posix_get_file_size( OTF2_File* file,
                               uint64_t*  size );


/** @brief Register a handle for file substrate.
 *
 *  Allocates a handle, sets the according values and registers the function
 *  pointers to its file operations.
 *
 *  @return                 Returns a handle to the file if successful, NULL
 *                          otherwise.
 */
OTF2_ErrorCode
otf2_file_posix_open( OTF2_Archive*    archive,
                      OTF2_FileMode    fileMode,
                      OTF2_FileType    fileType,
                      OTF2_LocationRef location,
                      OTF2_File**      file )
{
    char* file_path = otf2_archive_get_file_path( archive,
                                                  fileType,
                                                  location );
    if ( !file_path )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Can't determine file path!" );
    }
    UTILS_DEBUG( "File to open: '%s'", file_path );

    /* Initialize substrate specific data. */
    FILE* os_file = NULL;
    switch ( fileMode )
    {
        case OTF2_FILEMODE_WRITE:
            os_file = fopen( file_path, "wb" );
            break;
        case OTF2_FILEMODE_READ:
            os_file = fopen( file_path, "rb" );
            break;
        case OTF2_FILEMODE_MODIFY:
            os_file = fopen( file_path, "rb+" );
            break;
        default:
            os_file = fopen( file_path, "rb" );
            break;
    }

    if ( os_file == NULL )
    {
        OTF2_ErrorCode status = UTILS_ERROR_POSIX( "'%s'", file_path );
        free( file_path );
        return status;
    }

    otf2_file_posix* posix_file = ( otf2_file_posix* )calloc( 1, sizeof( *posix_file ) );
    if ( !posix_file )
    {
        fclose( os_file );
        free( file_path );
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Could not allocate POSIX file object." );
    }

    /* Register function pointers. */
    posix_file->super.reset         = otf2_file_posix_reset;
    posix_file->super.write         = otf2_file_posix_write;
    posix_file->super.read          = otf2_file_posix_read;
    posix_file->super.seek          = otf2_file_posix_seek;
    posix_file->super.get_file_size = otf2_file_posix_get_file_size;

    posix_file->file_path = file_path;
    posix_file->file      = os_file;
    posix_file->position  = 0;

    *file = &posix_file->super;

    return OTF2_SUCCESS;
}


/** @name File operations for Posix file substrate.
 *  Please see the standard interface functions for a detailed description.
 *  @{ */
OTF2_ErrorCode
otf2_file_posix_close( OTF2_File* file )
{
    /* Get the original substrate structure. */
    otf2_file_posix* posix_file = ( otf2_file_posix* )file;

    int status     = fclose( posix_file->file );
    int errno_save = errno;

    free( ( void* )posix_file->file_path );
    free( posix_file );

    if ( status == 0 )
    {
        return OTF2_SUCCESS;
    }

    errno = errno_save;
    return UTILS_ERROR_POSIX( "Posix call 'fclose()' failed!" );
}


OTF2_ErrorCode
otf2_file_posix_reset( OTF2_File* file )
{
    /* Get the original substrate structure. */
    otf2_file_posix* posix_file = ( otf2_file_posix* )file;

    /* Reset is done by closing the file and reopen it in writing mode. */
    int status = fclose( posix_file->file );
    if ( status != 0 )
    {
        return UTILS_ERROR_POSIX( "%s", posix_file->file_path );
    }

    posix_file->file = fopen( posix_file->file_path, "wb" );
    if ( posix_file->file == NULL )
    {
        return UTILS_ERROR_POSIX( "%s", posix_file->file_path );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_file_posix_write( OTF2_File*  file,
                       const void* buffer,
                       uint64_t    size )
{
    /* Get the original substrate structure. */
    otf2_file_posix* posix_file = ( otf2_file_posix* )file;

    size_t status = fwrite( buffer, size, 1, posix_file->file );
    if ( status != 1 )
    {
        return UTILS_ERROR_POSIX( "%s", posix_file->file_path );
    }

    posix_file->position += size;
    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_file_posix_read( OTF2_File* file,
                      void*      buffer,
                      uint64_t   size )
{
    /* Get the original substrate structure. */
    otf2_file_posix* posix_file = ( otf2_file_posix* )file;

    UTILS_DEBUG( "fread( %p, 1, %zu, %p )",
                 buffer, size, posix_file->file );
    size_t bytes = fread( buffer, 1, size, posix_file->file );
    UTILS_DEBUG( "fread( %p, 1, %zu, %p ) = %zu",
                 buffer, size, posix_file->file, bytes );
    if ( ferror( posix_file->file ) )
    {
        return UTILS_ERROR_POSIX( "%s", posix_file->file_path );
    }

    posix_file->position += bytes;
    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_file_posix_seek( OTF2_File* file,
                      int64_t    offset )
{
    /* Get the original substrate structure. */
    otf2_file_posix* posix_file = ( otf2_file_posix* )file;

    int status = fseek( posix_file->file, offset, SEEK_SET );
    if ( status != 0 )
    {
        return UTILS_ERROR_POSIX( "%s", posix_file->file_path );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_file_posix_get_file_size( OTF2_File* file,
                               uint64_t*  size )
{
    /* Get the original substrate structure. */
    otf2_file_posix* posix_file = ( otf2_file_posix* )file;

#if HAVE( STAT )
    struct stat file_stat;
    int         status = stat( posix_file->file_path, &file_stat );
    if ( 0 != status )
    {
        return UTILS_ERROR_POSIX( "%s", posix_file->file_path );
    }
    *size = ( uint64_t )file_stat.st_size;
#else
    FILE* file_object = fopen( posix_file->file_path, "r" );
    if ( !file_object )
    {
        return UTILS_ERROR_POSIX( "%s", posix_file->file_path );
    }
    int status = fseek( file_object, 0, SEEK_END );
    if ( 0 != status )
    {
        fclose( file_object );
        return UTILS_ERROR_POSIX( "%s", posix_file->file_path );
    }
    *size = ftell( file_object );
    fclose( file_object );
#endif

    return OTF2_SUCCESS;
}

/** @} */
