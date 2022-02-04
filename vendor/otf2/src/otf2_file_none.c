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
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"

#include "OTF2_File.h"
#include "otf2_file_int.h"


/** @brief File handle for no interaction with files.
 *  Please see otf2_file_none_struct for a detailed description. */

typedef struct otf2_file_none_struct otf2_file_none;


/** @brief File handle for no interaction with files. */
struct otf2_file_none_struct
{
    OTF2_File super;
};


/** @name File operations for none file substrate.
 *  Please see the standard interface functions for a detailed description.
 *  @{ */
static OTF2_ErrorCode
otf2_file_none_reset( OTF2_File* file )
{
    /* Dummy to suppress compiler warnings. Compiler should remove this. */
    ( void )file;

    return OTF2_SUCCESS;
}

static OTF2_ErrorCode
otf2_file_none_write( OTF2_File*  file,
                      const void* buffer,
                      uint64_t    size )
{
    /* Dummy to suppress compiler warnings. Compiler should remove this. */
    ( void )file;
    ( void )buffer;
    ( void )size;

    return OTF2_SUCCESS;
}


static OTF2_ErrorCode
otf2_file_none_read( OTF2_File* file,
                     void*      buffer,
                     uint64_t   size )
{
    /* Dummy to suppress compiler warnings. Compiler should remove this. */
    ( void )file;
    ( void )buffer;
    ( void )size;

    return OTF2_SUCCESS;
}


static OTF2_ErrorCode
otf2_file_none_seek( OTF2_File* file,
                     int64_t    offset )
{
    /* Dummy to suppress compiler warnings. Compiler should remove this. */
    ( void )file;
    ( void )offset;

    return OTF2_SUCCESS;
}


static OTF2_ErrorCode
otf2_file_none_get_file_size( OTF2_File* file,
                              uint64_t*  size )
{
    /* Dummy to suppress compiler warnings. Compiler should remove this. */
    ( void )file;
    ( void )size;

    return OTF2_SUCCESS;
}
/** @} */


/** @brief Register a handle for file substrate.
 *
 *  Allocates a handle, sets the according values and registers the function
 *  pointers to its file operations.
 *
 *  @param fileName         Name of the file.
 *  @param mode             Method to interact with the file.
 *  @param substrate        File substrate.
 *
 *  @return                 Returns a handle to the file if successful, NULL
 *                          otherwise.
 */
OTF2_ErrorCode
otf2_file_none_open( OTF2_Archive*    archive,
                     OTF2_FileMode    fileMode,
                     OTF2_FileType    fileType,
                     OTF2_LocationRef location,
                     OTF2_File**      file )
{
    ( void )archive;
    ( void )fileMode;
    ( void )fileType;
    ( void )location;

    otf2_file_none* none_file = ( otf2_file_none* )calloc( 1, sizeof( *none_file ) );
    if ( none_file == NULL )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Could not allocate memory for file handle!" );
    }

    /* Register function pointers. */
    none_file->super.reset         = otf2_file_none_reset;
    none_file->super.write         = otf2_file_none_write;
    none_file->super.read          = otf2_file_none_read;
    none_file->super.seek          = otf2_file_none_seek;
    none_file->super.get_file_size = otf2_file_none_get_file_size;

    *file = &none_file->super;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_file_none_close( OTF2_File* file )
{
    free( file );

    return OTF2_SUCCESS;
}
