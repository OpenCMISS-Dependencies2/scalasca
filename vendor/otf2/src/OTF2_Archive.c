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
 *  @brief      Writing interface for OTF2 archives.
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
#define OTF2_DEBUG_MODULE_NAME ARCHIVE
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_lock.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"

#include "otf2_collectives.h"
#include "otf2_collectives_serial.h"

#include "OTF2_File.h"
#include "OTF2_AnchorFile.h"

#include "OTF2_Compression_Zlib.h"

/* ___ Prototypes for internal functions. ___________________________________ */



static inline OTF2_ErrorCode
otf2_anchorfile_version_check( OTF2_Archive* archive );



/* ___ Interactions with the archive handle. ________________________________ */



OTF2_Archive*
OTF2_Archive_Open( const char*              archivePath,
                   const char*              archiveName,
                   const OTF2_FileMode      fileMode,
                   const uint64_t           chunkSizeEvents,
                   const uint64_t           chunkSizeDefs,
                   const OTF2_FileSubstrate fileSubstrate,
                   const OTF2_Compression   compression )
{
    UTILS_DEBUG_ENTRY( "%s/%s", archivePath, archiveName );

    OTF2_Archive*  archive = NULL;
    OTF2_ErrorCode status  = OTF2_SUCCESS;

    /* Validate arguments. */
    if ( NULL == archiveName )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive name!" );
        return NULL;
    }
    if ( NULL == archivePath )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive path!" );
        return NULL;
    }

    /* Check if passed enum value for file mode is valid. */
    if ( fileMode != OTF2_FILEMODE_WRITE &&
         fileMode != OTF2_FILEMODE_READ &&
         fileMode != OTF2_FILEMODE_MODIFY )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid value for the file mode!" );
        return NULL;
    }

    /* Check if passed enum value for file substrate is valid. */
    if ( fileSubstrate != OTF2_SUBSTRATE_POSIX &&
         fileSubstrate != OTF2_SUBSTRATE_SION &&
         fileSubstrate != OTF2_SUBSTRATE_NONE &&
         fileMode == OTF2_FILEMODE_WRITE )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid value for the file substrate!" );
        return NULL;
    }

    /* Get archive handle from internal archive. */
    if ( fileMode == OTF2_FILEMODE_WRITE )
    {
        /* Check the range of chunk sizes. */
        if ( chunkSizeEvents == OTF2_UNDEFINED_UINT64 ||
             chunkSizeEvents < OTF2_CHUNK_SIZE_MIN ||
             chunkSizeEvents > OTF2_CHUNK_SIZE_MAX )
        {
            UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                         "This is no valid event chunk size!" );
            return NULL;
        }

        if ( chunkSizeDefs != OTF2_UNDEFINED_UINT64 &&
             ( chunkSizeDefs < OTF2_CHUNK_SIZE_MIN || chunkSizeDefs > OTF2_CHUNK_SIZE_MAX ) )
        {
            UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                         "This is no valid definition chunk size!" );
            return NULL;
        }

        /* Check compression. */
        if ( compression != OTF2_COMPRESSION_NONE &&
             compression != OTF2_COMPRESSION_ZLIB )
        {
            UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                         "Unhandled file compression!" );
            return NULL;
        }
        if ( !OTF2_Compression_Zlib_Available() && compression == OTF2_COMPRESSION_ZLIB )
        {
            UTILS_ERROR( OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED,
                         "No compression library found!" );
            return NULL;
        }

        archive = otf2_archive_open( archivePath, archiveName,
                                     fileSubstrate, compression,
                                     chunkSizeEvents, chunkSizeDefs,
                                     fileMode );
    }
    else
    {
        archive = otf2_archive_open( archivePath, archiveName,
                                     OTF2_SUBSTRATE_UNDEFINED,
                                     OTF2_COMPRESSION_UNDEFINED,
                                     OTF2_UNDEFINED_UINT64,
                                     OTF2_UNDEFINED_UINT64,
                                     fileMode );
    }

    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                     "Could not get new archive handle!" );
        return NULL;
    }

    /* If in reading mode, set archive path and master.
     * And read the anchor file. */
    if ( fileMode == OTF2_FILEMODE_READ )
    {
        status = OTF2_AnchorFile_Load( archive );
        if ( status != OTF2_SUCCESS )
        {
            UTILS_ERROR( status,
                         "Could not read data from anchor file!" );

            return NULL;
        }

        status = otf2_anchorfile_version_check( archive );
        if ( status != OTF2_SUCCESS )
        {
            UTILS_ERROR( status,
                         "The trace version conflicts with current reader version!" );
            otf2_archive_close( archive );
            return NULL;
        }
    }

    return archive;
}



OTF2_ErrorCode
OTF2_Archive_Close( OTF2_Archive* archive )
{
    OTF2_FileMode  file_mode = OTF2_FILEMODE_READ;
    OTF2_ErrorCode status    = OTF2_ERROR_INVALID;

    /* Return success if NULL is passed. */
    if ( NULL == archive )
    {
        return OTF2_SUCCESS;
    }

    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode information!" );
    }

    /* If master and in writing mode, write a anchor file. */
    if ( file_mode == OTF2_FILEMODE_WRITE &&
         otf2_archive_is_master( archive ) )
    {
        status = OTF2_AnchorFile_Save( archive );

        /* Print a warning if writing to the anchor file fails but continue
           anyway. */
        if ( status != OTF2_SUCCESS )
        {
            UTILS_ERROR( status, "Could not write data from anchor file!" );
        }
    }

    return otf2_archive_close( archive );
}


/* ___ Set additional information. __________________________________________ */



OTF2_ErrorCode
OTF2_Archive_SetDefChunkSize( OTF2_Archive* archive,
                              uint64_t      chunkSize )
{
    OTF2_ErrorCode status = OTF2_ERROR_INVALID;

    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }

    OTF2_CallbackCode callback_ret =
        otf2_collectives_bcast( archive,
                                archive->global_comm_context,
                                &chunkSize,
                                1,
                                OTF2_TYPE_UINT64,
                                OTF2_COLLECTIVES_ROOT );
    if ( OTF2_CALLBACK_SUCCESS != callback_ret )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Can't broadcast chunk size." );
    }

    if ( chunkSize == OTF2_UNDEFINED_UINT64 ||
         ( chunkSize < OTF2_CHUNK_SIZE_MIN || chunkSize > OTF2_CHUNK_SIZE_MAX ) )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid chunk size given as argument to the master archive!" );
    }

    OTF2_ARCHIVE_LOCK( archive );

    status = otf2_archive_set_def_chunksize( archive, chunkSize );

    OTF2_ARCHIVE_UNLOCK( archive );

    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not set definition chunk size!" );
    }

    return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_Archive_SetMachineName( OTF2_Archive* archive,
                             const char*   machineName )
{
    OTF2_ErrorCode status = OTF2_ERROR_INVALID;

    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( NULL == machineName )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid machine name!" );
    }

    status = otf2_archive_set_machine_name( archive, machineName );

    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not set machine name!" );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_Archive_SetDescription( OTF2_Archive* archive,
                             const char*   description )
{
    OTF2_ErrorCode status = OTF2_ERROR_INVALID;

    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( NULL == description )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid description!" );
    }

    status = otf2_archive_set_description( archive, description );

    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not set description!" );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_Archive_SetCreator( OTF2_Archive* archive,
                         const char*   creator )
{
    OTF2_ErrorCode status = OTF2_ERROR_INVALID;

    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( NULL == creator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid creator!" );
    }

    status = otf2_archive_set_creator( archive, creator );

    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not set creator!" );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_Archive_SetFlushCallbacks( OTF2_Archive*              archive,
                                const OTF2_FlushCallbacks* flushCallbacks,
                                void*                      flushData )
{
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }
    if ( !flushCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for memoryCallbacks parameter!" );
    }
    if ( archive->file_mode == OTF2_FILEMODE_READ )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Memory callbacks use in reading mode!" );
    }

    /* guaranteed to process without fault */
    otf2_archive_set_flush_callbacks( archive,
                                      flushCallbacks,
                                      flushData );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_Archive_SetMemoryCallbacks( OTF2_Archive*               archive,
                                 const OTF2_MemoryCallbacks* memoryCallbacks,
                                 void*                       memoryData )
{
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }
    if ( !memoryCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for memoryCallbacks parameter!" );
    }

    if ( !memoryCallbacks->otf2_allocate || !memoryCallbacks->otf2_free_all )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Missing function pointers in memoryCallbacks!" );
    }

    if ( archive->file_mode == OTF2_FILEMODE_READ )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Memory callbacks used in reading mode!" );
    }

    /* guaranteed to process without fault */
    otf2_archive_set_memory_callbacks( archive,
                                       memoryCallbacks,
                                       memoryData );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_Archive_SetCollectiveCallbacks( OTF2_Archive*                   archive,
                                     const OTF2_CollectiveCallbacks* collectiveCallbacks,
                                     void*                           collectiveData,
                                     OTF2_CollectiveContext*         globalCommContext,
                                     OTF2_CollectiveContext*         localCommContext )
{
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }

    if ( !collectiveCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for collectiveCallbacks parameter!" );
    }

    if ( !collectiveCallbacks->otf2_get_size ||
         !collectiveCallbacks->otf2_get_rank ||
         !collectiveCallbacks->otf2_barrier ||
         !collectiveCallbacks->otf2_bcast ||
         !collectiveCallbacks->otf2_gather ||
         !collectiveCallbacks->otf2_gatherv ||
         !collectiveCallbacks->otf2_scatter ||
         !collectiveCallbacks->otf2_scatterv )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Missing callbacks in collective callback structure." );
    }

    if ( collectiveCallbacks->otf2_create_local_comm &&
         !collectiveCallbacks->otf2_free_local_comm )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Missing free_local_comm callback in collective callback structure." );
    }

    return otf2_archive_set_collective_callbacks( archive,
                                                  collectiveCallbacks,
                                                  collectiveData,
                                                  globalCommContext,
                                                  localCommContext );
}


OTF2_ErrorCode
OTF2_Archive_SetSerialCollectiveCallbacks( OTF2_Archive* archive )
{
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }

    return otf2_archive_set_collective_callbacks( archive,
                                                  &otf2_collectives_serial,
                                                  NULL,
                                                  NULL,
                                                  NULL );
}


OTF2_ErrorCode
OTF2_Archive_IsMaster( OTF2_Archive* archive, bool* result )
{
    if ( !archive || !result )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }

    if ( !archive->collective_callbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Collective context not yet set!" );
    }

    *result = otf2_archive_is_master( archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_Archive_SetLockingCallbacks( OTF2_Archive*                archive,
                                  const OTF2_LockingCallbacks* lockingCallbacks,
                                  void*                        lockingData )
{
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }

    if ( !lockingCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for lockingCallbacks parameter!" );
    }

    if ( !lockingCallbacks->otf2_create ||
         !lockingCallbacks->otf2_destroy ||
         !lockingCallbacks->otf2_lock ||
         !lockingCallbacks->otf2_unlock )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Missing callbacks in locking callback structure." );
    }

    return otf2_archive_set_locking_callbacks( archive,
                                               lockingCallbacks,
                                               lockingData );
}


OTF2_ErrorCode
OTF2_Archive_SetHint( OTF2_Archive* archive,
                      OTF2_Hint     hint,
                      void*         value )
{
    /* Validate arguments. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }

    /* Validate arguments. */
    if ( !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid reference value parameter!" );
    }

    return otf2_archive_set_hint( archive, hint, value );
}


OTF2_ErrorCode
OTF2_Archive_SetProperty( OTF2_Archive* archive,
                          const char*   name,
                          const char*   value,
                          bool          overwrite )
{
    /* Validate arguments. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }
    if ( !name )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for property name parameter!" );
    }
    if ( !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for property value parameter!" );
    }
    if ( archive->file_mode != OTF2_FILEMODE_WRITE )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "This is call is not allowed in reading mode!" );
    }

    return otf2_archive_set_property( archive, name, value, overwrite );
}

OTF2_ErrorCode
OTF2_Archive_SetBoolProperty( OTF2_Archive* archive,
                              const char*   name,
                              bool          value,
                              bool          overwrite )
{
    /* Validate arguments. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }
    if ( !name )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for property name parameter!" );
    }
    if ( archive->file_mode != OTF2_FILEMODE_WRITE )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "This is call is not allowed in reading mode!" );
    }

    return otf2_archive_set_property( archive,
                                      name,
                                      value ? "true" : "false",
                                      overwrite );
}

OTF2_ErrorCode
OTF2_Archive_GetPropertyNames( OTF2_Archive* archive,
                               uint32_t*     numberOfProperties,
                               char***       names )
{
    /* Validate arguments. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }
    if ( !numberOfProperties )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for number of properties parameter!" );
    }
    if ( !names )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for property names parameter!" );
    }

    return otf2_archive_get_property_names( archive, numberOfProperties, names );
}

OTF2_ErrorCode
OTF2_Archive_GetProperty( OTF2_Archive* archive,
                          const char*   name,
                          char**        value )
{
    /* Validate arguments. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }
    if ( !name )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for property name parameter!" );
    }

    return otf2_archive_get_property( archive, name, value );
}

OTF2_ErrorCode
OTF2_Archive_GetTraceId( OTF2_Archive* archive,
                         uint64_t*     id )
{
    /* Validate arguments. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for archive parameter!" );
    }
    if ( !id )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid argument for id parameter!" );
    }
    if ( archive->file_mode != OTF2_FILEMODE_READ )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "This is call is allowed in reading mode only!" );
    }

    return otf2_archive_get_trace_id( archive, id );
}

OTF2_ErrorCode
OTF2_Archive_GetNumberOfLocations( OTF2_Archive* archive,
                                   uint64_t*     numberOfLocations )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !numberOfLocations )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid numberOfLocations argument!" );
    }

    return otf2_archive_get_number_of_locations( archive,
                                                 numberOfLocations );
}


OTF2_ErrorCode
OTF2_Archive_GetNumberOfGlobalDefinitions( OTF2_Archive* archive,
                                           uint64_t*     numberOfGlobalDefinitions )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !numberOfGlobalDefinitions )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid numberOfGlobalDefinitions argument!" );
    }

    return otf2_archive_get_number_of_global_defs( archive,
                                                   numberOfGlobalDefinitions );
}


OTF2_ErrorCode
OTF2_Archive_GetMachineName( OTF2_Archive* archive,
                             char**        machineName )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !machineName )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid machineName argument!" );
    }

    return otf2_archive_get_machine_name( archive, machineName );
}


OTF2_ErrorCode
OTF2_Archive_GetDescription( OTF2_Archive* archive,
                             char**        description )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !description )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid description argument!" );
    }

    return otf2_archive_get_description( archive, description );
}


OTF2_ErrorCode
OTF2_Archive_GetCreator( OTF2_Archive* archive,
                         char**        creator )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !creator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid creator argument!" );
    }

    return otf2_archive_get_creator( archive, creator );
}


OTF2_ErrorCode
OTF2_Archive_GetVersion( OTF2_Archive* archive,
                         uint8_t*      major,
                         uint8_t*      minor,
                         uint8_t*      bugfix )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !major || !minor || !bugfix )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments!" );
    }

    return otf2_archive_get_version( archive, major, minor, bugfix );
}



OTF2_ErrorCode
OTF2_Archive_GetChunkSize( OTF2_Archive* archive,
                           uint64_t*     chunkSizeEvents,
                           uint64_t*     chunkSizeDefs )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !chunkSizeEvents || !chunkSizeDefs )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments!" );
    }

    OTF2_ErrorCode ret = otf2_archive_get_event_chunksize( archive, chunkSizeEvents );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    OTF2_ARCHIVE_LOCK( archive );
    ret = otf2_archive_get_def_chunksize( archive, chunkSizeDefs );
    OTF2_ARCHIVE_UNLOCK( archive );

    return ret;
}


OTF2_ErrorCode
OTF2_Archive_GetFileSubstrate( OTF2_Archive*       archive,
                               OTF2_FileSubstrate* substrate )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !substrate )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid substrate argument!" );
    }

    return otf2_archive_get_file_substrate( archive, substrate );
}


OTF2_ErrorCode
OTF2_Archive_GetCompression( OTF2_Archive*     archive,
                             OTF2_Compression* compression )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !compression )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid compression argument!" );
    }

    return otf2_archive_get_compression( archive, compression );
}


OTF2_EvtWriter*
OTF2_Archive_GetEvtWriter( OTF2_Archive*    archive,
                           OTF2_LocationRef location )
{
    OTF2_EvtWriter* writer = NULL;
    OTF2_ErrorCode  status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode == OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a event writer in read mode!" );
        return NULL;
    }

    /* Flush callbacks are mandatory for event writers */
    if ( !archive->flush_callbacks )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a event writer without flush callbacks!" );
        return NULL;
    }

    OTF2_FileSubstrate substrate;
    status = otf2_archive_get_file_substrate( archive, &substrate );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file substrate!" );
        return NULL;
    }

    /* location == OTF2_UNDEFINED_LOCATION is valid */

    status = otf2_archive_get_evt_writer( archive, location, &writer );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get local event writer" );

        return NULL;
    }

    return writer;
}

OTF2_DefWriter*
OTF2_Archive_GetDefWriter( OTF2_Archive*    archive,
                           OTF2_LocationRef location )
{
    OTF2_DefWriter* writer = NULL;
    OTF2_ErrorCode  status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    if ( location == OTF2_UNDEFINED_LOCATION )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid location ID given!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode == OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a definition writer in read mode!" );
        return NULL;
    }

    /* A pre_flush callback is mandatory for definition writers */
    if ( !archive->flush_callbacks )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a definition writer without flush callback!" );
        return NULL;
    }

    OTF2_FileSubstrate substrate;
    status = otf2_archive_get_file_substrate( archive, &substrate );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file substrate!" );
        return NULL;
    }

    status = otf2_archive_get_def_writer( archive, location, &writer );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get local definition writer" );
        return NULL;
    }

    return writer;
}


OTF2_GlobalDefWriter*
OTF2_Archive_GetGlobalDefWriter( OTF2_Archive* archive )
{
    OTF2_GlobalDefWriter* writer = NULL;
    OTF2_ErrorCode        status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode == OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a global definition writer in read mode!" );
        return NULL;
    }

    /* A pre_flush callback is mandatory for global definition writers */
    if ( !archive->flush_callbacks )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a global definition writer without flush callback!" );
        return NULL;
    }

    OTF2_FileSubstrate substrate;
    status = otf2_archive_get_file_substrate( archive, &substrate );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file substrate!" );
        return NULL;
    }

    status = otf2_archive_get_global_def_writer( archive, &writer );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get global definition writer" );
        return NULL;
    }

    return writer;
}


OTF2_SnapWriter*
OTF2_Archive_GetSnapWriter( OTF2_Archive*    archive,
                            OTF2_LocationRef location )
{
    OTF2_SnapWriter* writer = NULL;
    OTF2_ErrorCode   status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    if ( location == OTF2_UNDEFINED_LOCATION )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid location ID given!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode == OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a event writer in read mode!" );
        return NULL;
    }

    /* Flush callbacks are mandatory for event writers */
    if ( !archive->flush_callbacks )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a snapshot writer without flush callbacks!" );
        return NULL;
    }

    OTF2_FileSubstrate substrate;
    status = otf2_archive_get_file_substrate( archive, &substrate );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file substrate!" );
        return NULL;
    }

    status = otf2_archive_get_snap_writer( archive, location, &writer );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get local event writer" );

        return NULL;
    }

    return writer;
}


OTF2_ThumbWriter*
OTF2_Archive_GetThumbWriter( OTF2_Archive*      archive,
                             const char*        name,
                             const char*        description,
                             OTF2_ThumbnailType type,
                             uint32_t           numberOfSamples,
                             uint32_t           numberOfMetrics,
                             const uint64_t*    refsToDefs )
{
    OTF2_ThumbWriter* writer = NULL;
    OTF2_ErrorCode    status;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid archive handle!" );
        return NULL;
    }
    if ( !name || !description
         || 0 == numberOfSamples || 0 == numberOfMetrics
         || !refsToDefs )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid arguments!" );
        return NULL;
    }
    if ( type != OTF2_THUMBNAIL_TYPE_REGION
         && type != OTF2_THUMBNAIL_TYPE_METRIC
         && type != OTF2_THUMBNAIL_TYPE_ATTRIBUTES )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid thumbnail type!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode == OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a thumbnail writer in read mode!" );
        return NULL;
    }

    status = otf2_archive_get_thumb_writer( archive,
                                            &writer,
                                            name,
                                            description,
                                            type,
                                            numberOfSamples,
                                            numberOfMetrics,
                                            refsToDefs );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get local event writer" );

        return NULL;
    }

    return writer;
}


OTF2_MarkerWriter*
OTF2_Archive_GetMarkerWriter( OTF2_Archive* archive )
{
    OTF2_MarkerWriter* writer = NULL;
    OTF2_ErrorCode     status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    if ( !otf2_archive_is_master( archive ) )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a marker writer in slave mode!" );
        return NULL;
    }

    status = otf2_archive_get_marker_writer( archive, &writer );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get marker writer" );
        return NULL;
    }

    return writer;
}


OTF2_EvtReader*
OTF2_Archive_GetEvtReader( OTF2_Archive*    archive,
                           OTF2_LocationRef location )
{
    OTF2_EvtReader* reader = NULL;
    OTF2_ErrorCode  status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    if ( location == OTF2_UNDEFINED_LOCATION )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid location ID given!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode != OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a event reader in writing mode!" );
        return NULL;
    }

    OTF2_FileSubstrate substrate;
    status = otf2_archive_get_file_substrate( archive, &substrate );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file substrate!" );
        return NULL;
    }

    status = otf2_archive_get_evt_reader( archive, location, &reader );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get local event reader" );

        return NULL;
    }

    return reader;
}


/** @brief Get a global event reader.
 *
 *  @param archive          Archive handle.
 *
 *  @return                 Returns a global event reader handle if successful,
 *                          NULL if an error occurs.
 */
OTF2_GlobalEvtReader*
OTF2_Archive_GetGlobalEvtReader( OTF2_Archive* archive )
{
    OTF2_GlobalEvtReader* reader = NULL;
    OTF2_ErrorCode        status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    /* Validate argument. */
    if ( NULL == archive->local_evt_readers )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "No event reader selected!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode != OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a global event reader in writing mode!" );
        return NULL;
    }

    status = otf2_archive_get_global_evt_reader( archive, &reader );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get global event reader" );

        return NULL;
    }

    return reader;
}


OTF2_DefReader*
OTF2_Archive_GetDefReader( OTF2_Archive*    archive,
                           OTF2_LocationRef location )
{
    OTF2_DefReader* reader = NULL;
    OTF2_ErrorCode  status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    if ( location == OTF2_UNDEFINED_LOCATION )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid location ID given!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode != OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a definition reader in writing mode!" );
        return NULL;
    }

    OTF2_FileSubstrate substrate;
    status = otf2_archive_get_file_substrate( archive, &substrate );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file substrate!" );
        return NULL;
    }

    status = otf2_archive_get_def_reader( archive, location, &reader );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get local definition reader" );
        return NULL;
    }

    return reader;
}


OTF2_GlobalDefReader*
OTF2_Archive_GetGlobalDefReader( OTF2_Archive* archive )
{
    OTF2_GlobalDefReader* reader = NULL;
    OTF2_ErrorCode        status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode != OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a global definition reader in writing mode!" );
        return NULL;
    }

    OTF2_FileSubstrate substrate;
    status = otf2_archive_get_file_substrate( archive, &substrate );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file substrate!" );
        return NULL;
    }

    status = otf2_archive_get_global_def_reader( archive, &reader );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get global definition reader" );

        return NULL;
    }

    return reader;
}


OTF2_SnapReader*
OTF2_Archive_GetSnapReader( OTF2_Archive*    archive,
                            OTF2_LocationRef location )
{
    OTF2_SnapReader* reader = NULL;
    OTF2_ErrorCode   status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    if ( location == OTF2_UNDEFINED_LOCATION )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid location ID given!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode != OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a snap reader in writing mode!" );
        return NULL;
    }

    OTF2_FileSubstrate substrate;
    status = otf2_archive_get_file_substrate( archive, &substrate );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file substrate!" );
        return NULL;
    }

    status = otf2_archive_get_snap_reader( archive, location, &reader );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get local snap reader" );

        return NULL;
    }

    return reader;
}


/** @brief Get a global snap reader.
 *
 *  @param archive          Archive handle.
 *
 *  @return                 Returns a global snap reader handle if successful,
 *                          NULL if an error occurs.
 */
OTF2_GlobalSnapReader*
OTF2_Archive_GetGlobalSnapReader( OTF2_Archive* archive )
{
    OTF2_GlobalSnapReader* reader = NULL;
    OTF2_ErrorCode         status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    /* Validate argument. */
    if ( NULL == archive->local_snap_readers )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "No snap reader selected!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode != OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a global snap reader in writing mode!" );
        return NULL;
    }

    status = otf2_archive_get_global_snap_reader( archive, &reader );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get global event reader" );

        return NULL;
    }

    return reader;
}


OTF2_ThumbReader*
OTF2_Archive_GetThumbReader( OTF2_Archive* archive,
                             uint32_t      thumbID )
{
    OTF2_ThumbReader* reader = NULL;
    OTF2_ErrorCode    status = OTF2_ERROR_INVALID;

    /* Validate arguments. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    if ( !( thumbID < archive->number_of_thumbnails ) )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid thumb ID given!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode != OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a event reader in writing mode!" );
        return NULL;
    }

    OTF2_FileSubstrate substrate;
    status = otf2_archive_get_file_substrate( archive, &substrate );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file substrate!" );
        return NULL;
    }

    status = otf2_archive_get_thumb_reader( archive, thumbID, &reader );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get local event reader" );

        return NULL;
    }

    return reader;
}


OTF2_MarkerReader*
OTF2_Archive_GetMarkerReader( OTF2_Archive* archive )
{
    OTF2_MarkerReader* reader = NULL;
    OTF2_ErrorCode     status = OTF2_ERROR_INVALID;

    /* Validate argument. */
    if ( NULL == archive )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid archive handle!" );
        return NULL;
    }

    OTF2_FileMode file_mode;
    status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get file mode!" );
        return NULL;
    }

    if ( file_mode != OTF2_FILEMODE_READ )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                     "Requesting a marker reader in writing mode!" );
        return NULL;
    }

    status = otf2_archive_get_marker_reader( archive, &reader );

    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status, "Could not get marker reader" );

        return NULL;
    }

    return reader;
}


OTF2_ErrorCode
OTF2_Archive_CloseEvtWriter( OTF2_Archive*   archive,
                             OTF2_EvtWriter* writer )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }

    if ( writer == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_evt_writer( archive, writer );
}


OTF2_ErrorCode
OTF2_Archive_CloseDefWriter( OTF2_Archive*   archive,
                             OTF2_DefWriter* writer )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }

    if ( writer == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_def_writer( archive, writer );
}


OTF2_ErrorCode
OTF2_Archive_CloseGlobalDefWriter( OTF2_Archive*         archive,
                                   OTF2_GlobalDefWriter* writer )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    if ( writer == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_global_def_writer( archive, writer );
}


OTF2_ErrorCode
OTF2_Archive_CloseSnapWriter( OTF2_Archive*    archive,
                              OTF2_SnapWriter* writer )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }

    if ( writer == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_snap_writer( archive, writer );
}


OTF2_ErrorCode
OTF2_Archive_CloseMarkerWriter( OTF2_Archive*      archive,
                                OTF2_MarkerWriter* writer )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }

    if ( writer == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_marker_writer( archive, writer );
}


OTF2_ErrorCode
OTF2_Archive_CloseEvtReader( OTF2_Archive*   archive,
                             OTF2_EvtReader* reader )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }

    if ( reader == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_evt_reader( archive, reader, false );
}


OTF2_ErrorCode
OTF2_Archive_CloseThumbReader( OTF2_Archive*     archive,
                               OTF2_ThumbReader* reader )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }

    if ( reader == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_thumb_reader( archive, reader );
}

OTF2_ErrorCode
OTF2_Archive_CloseGlobalEvtReader( OTF2_Archive*         archive,
                                   OTF2_GlobalEvtReader* globalEvtReader )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    if ( globalEvtReader == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_global_evt_reader( archive, globalEvtReader );
}


OTF2_ErrorCode
OTF2_Archive_CloseDefReader( OTF2_Archive*   archive,
                             OTF2_DefReader* reader )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }

    if ( reader == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_def_reader( archive, reader );
}


OTF2_ErrorCode
OTF2_Archive_CloseGlobalDefReader( OTF2_Archive*         archive,
                                   OTF2_GlobalDefReader* globalDefReader )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    if ( globalDefReader == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_global_def_reader( archive, globalDefReader );
}


OTF2_ErrorCode
OTF2_Archive_CloseSnapReader( OTF2_Archive*    archive,
                              OTF2_SnapReader* reader )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }

    if ( reader == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_snap_reader( archive, reader, false );
}


OTF2_ErrorCode
OTF2_Archive_CloseGlobalSnapReader( OTF2_Archive*          archive,
                                    OTF2_GlobalSnapReader* globalSnapReader )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    if ( globalSnapReader == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_global_snap_reader( archive, globalSnapReader );
}


OTF2_ErrorCode
OTF2_Archive_CloseMarkerReader( OTF2_Archive*      archive,
                                OTF2_MarkerReader* markerReader )
{
    /* Validate argument. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    if ( markerReader == NULL )
    {
        return OTF2_SUCCESS;
    }

    return otf2_archive_close_marker_reader( archive, markerReader );
}


/** @brief Check version of trace archive.
 *
 *  Checks if the version of the trace archive works with the version of the
 *  current reader.
 *
 *  @param archive          Archive handle.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
static inline OTF2_ErrorCode
otf2_anchorfile_version_check( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    uint8_t        version = 0;
    OTF2_ErrorCode status  = otf2_archive_get_trace_format( archive, &version );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Unable to get trace format version!" );
    }

    /*
     * We are currently only able to read version 1 and 2 of the trace format.
     */
    if ( version <= OTF2_TRACE_FORMAT )
    {
        return OTF2_SUCCESS;
    }

    return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                        "The trace version conflicts with current reader version!" );
}


/** @brief Switch from read to write mode.
 *
 *  @param archive          Archive handle.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_Archive_SwitchFileMode( OTF2_Archive* archive,
                             OTF2_FileMode newFileMode )
{
    /* validate arguments */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    /* Check if passed enum value for file mode is valid. */
    if ( newFileMode != OTF2_FILEMODE_WRITE
         && newFileMode != OTF2_FILEMODE_READ
         && newFileMode != OTF2_FILEMODE_MODIFY )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid value for the file mode!" );
    }

    if ( !( archive->file_mode == OTF2_FILEMODE_READ
            && newFileMode == OTF2_FILEMODE_WRITE
            && archive->substrate == OTF2_SUBSTRATE_POSIX ) )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_FILE_MODE_TRANSITION,
                            "File mode transition not permitted!" );
    }

    archive->prev_file_mode = archive->file_mode;
    archive->file_mode      = newFileMode;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_Archive_GetNumberOfThumbnails( OTF2_Archive* archive,
                                    uint32_t*     number )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !number )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid 'number' argument!" );
    }

    return otf2_archive_get_number_of_thumbnails( archive, number );
}


OTF2_ErrorCode
OTF2_Archive_SetNumberOfSnapshots( OTF2_Archive* archive,
                                   uint32_t      number )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    return otf2_archive_set_number_of_snapshots( archive, number );
}


OTF2_ErrorCode
OTF2_Archive_GetNumberOfSnapshots( OTF2_Archive* archive,
                                   uint32_t*     number )
{
    /* Validate arguments. */
    if ( NULL == archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid archive handle!" );
    }
    if ( !number )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid 'number' argument!" );
    }
    return otf2_archive_get_number_of_snapshots( archive, number );
}


OTF2_ErrorCode
OTF2_Archive_OpenEvtFiles( OTF2_Archive* archive )
{
    /* Validate argument. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    OTF2_FileMode  file_mode;
    OTF2_ErrorCode status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not get file mode!" );
    }

    if ( file_mode != OTF2_FILEMODE_READ &&
         file_mode != OTF2_FILEMODE_WRITE )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Invalid file mode to operate on." );
    }

    return otf2_archive_open_evt_files( archive );
}


OTF2_ErrorCode
OTF2_Archive_CloseEvtFiles( OTF2_Archive* archive )
{
    /* Validate argument. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    OTF2_FileMode  file_mode;
    OTF2_ErrorCode status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not get file mode!" );
    }

    if ( file_mode != OTF2_FILEMODE_READ &&
         file_mode != OTF2_FILEMODE_WRITE )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Invalid file mode to operate on." );
    }

    return otf2_archive_close_evt_files( archive );
}


OTF2_ErrorCode
OTF2_Archive_OpenDefFiles( OTF2_Archive* archive )
{
    /* Validate argument. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    OTF2_FileMode  file_mode;
    OTF2_ErrorCode status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not get file mode!" );
    }

    if ( file_mode != OTF2_FILEMODE_READ &&
         file_mode != OTF2_FILEMODE_WRITE )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Invalid file mode to operate on." );
    }

    return otf2_archive_open_def_files( archive );
}


OTF2_ErrorCode
OTF2_Archive_CloseDefFiles( OTF2_Archive* archive )
{
    /* Validate argument. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    OTF2_FileMode  file_mode;
    OTF2_ErrorCode status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not get file mode!" );
    }

    if ( file_mode != OTF2_FILEMODE_READ &&
         file_mode != OTF2_FILEMODE_WRITE )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Invalid file mode to operate on." );
    }

    return otf2_archive_close_def_files( archive );
}


OTF2_ErrorCode
OTF2_Archive_OpenSnapFiles( OTF2_Archive* archive )
{
    /* Validate argument. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    OTF2_FileMode  file_mode;
    OTF2_ErrorCode status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not get file mode!" );
    }

    if ( file_mode != OTF2_FILEMODE_READ &&
         file_mode != OTF2_FILEMODE_WRITE )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Invalid file mode to operate on." );
    }

    return otf2_archive_open_snap_files( archive );
}


OTF2_ErrorCode
OTF2_Archive_CloseSnapFiles( OTF2_Archive* archive )
{
    /* Validate argument. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    OTF2_FileMode  file_mode;
    OTF2_ErrorCode status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not get file mode!" );
    }

    if ( file_mode != OTF2_FILEMODE_READ &&
         file_mode != OTF2_FILEMODE_WRITE )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Invalid file mode to operate on." );
    }

    return otf2_archive_close_snap_files( archive );
}


OTF2_ErrorCode
OTF2_Archive_SelectLocation( OTF2_Archive* archive, OTF2_LocationRef location )
{
    /* Validate argument. */
    if ( !archive )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid archive handle!" );
    }

    OTF2_FileMode  file_mode;
    OTF2_ErrorCode status = otf2_archive_get_file_mode( archive, &file_mode );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Could not get file mode!" );
    }

    if ( file_mode != OTF2_FILEMODE_READ )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_CALL,
                            "Call only allowed in read mode." );
    }

    return otf2_archive_select_location( archive, location );
}
