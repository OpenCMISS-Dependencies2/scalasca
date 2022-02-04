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

#include <sion.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#define OTF2_DEBUG_MODULE_NAME SION_COLLECTIVES
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"
#include "otf2_archive_location.h"

#include "OTF2_File.h"
#include "otf2_file_int.h"
#include "otf2_file_sion.h"
#include "otf2_file_substrate.h"
#include "otf2_file_substrate_sion.h"

#include "otf2_file_substrate_sion_int.h"

#include "otf2_collectives.h"


static OTF2_CollectiveContext*
otf2_sion_resolve_comm_proxy( struct otf2_sion_comm_proxy* proxy )
{
    OTF2_Archive* archive = proxy->archive;

    switch ( proxy->type )
    {
        case OTF2_SION_PROXY_TYPE_GLOBAL:
            return archive->global_comm_context;
        case OTF2_SION_PROXY_TYPE_LOCAL:
            return archive->local_comm_context;
        case OTF2_SION_PROXY_TYPE_LOCAL_FILE_TYPE:
        {
            OTF2_FileSubstrateFileTypeData* filetype_data =
                ( void* )( ( char* )proxy - offsetof( OTF2_FileSubstrateFileTypeData, proxy ) );
            return filetype_data->local_comm;
        }
        default:
            return NULL;
    }
}


/** @brief Translate SIONlib's data types to OTF2 data types.
 *
 *  @param type             SIONlib data type.
 *
 *  @return                 Returns OTF2 data type.
 */
static OTF2_Type
otf2_get_otf2_type_from_sion_type( int type )
{
    switch ( type )
    {
        case _SION_INT32:
            return OTF2_TYPE_INT32;
        case _SION_INT64:
            return OTF2_TYPE_INT64;
        case _SION_CHAR:
            return OTF2_TYPE_UINT8;
        default:
            UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                         "Invalid SIONlib type: %d", type );
    }

    return OTF2_TYPE_NONE;
}


static int
otf2_sion_collectives_create_local_comm_cb( void** localCommContext,
                                            void*  globalCommContext,
                                            int    globalRank,
                                            int    globalSize,
                                            int    localRank,
                                            int    localSize,
                                            int    fileNumber,
                                            int    numberOfFiles );


static int
otf2_sion_collectives_free_local_comm_cb( void* localCommContext );


static int
otf2_sion_collectives_barrier_cb( void* commContext );


static int
otf2_sion_collectives_bcast_cb( void* data,
                                void* commContext,
                                int   type,
                                int   nelem,
                                int   root );


static int
otf2_sion_collectives_gather_cb( void* inData,
                                 void* outData,
                                 void* commContext,
                                 int   type,
                                 int   nelem,
                                 int   root );


static int
otf2_sion_collectives_gatherv_cb( void* inData,
                                  void* outData,
                                  void* commContext,
                                  int   type,
                                  int*  counts,
                                  int   nelem,
                                  int   root );


static int
otf2_sion_collectives_scatter_cb( void* inData,
                                  void* outData,
                                  void* commContext,
                                  int   type,
                                  int   nelem,
                                  int   root );


static int
otf2_sion_collectives_scatterv_cb( void* inData,
                                   void* outData,
                                   void* commContext,
                                   int   type,
                                   int*  counts,
                                   int   nelem,
                                   int   root );


void
otf2_file_substrate_sion_register_callbacks( int api_id )
{
    sion_generic_register_create_local_commgroup_cb(
        api_id,
        otf2_sion_collectives_create_local_comm_cb );
    sion_generic_register_free_local_commgroup_cb(
        api_id,
        otf2_sion_collectives_free_local_comm_cb );
    sion_generic_register_barrier_cb(
        api_id,
        otf2_sion_collectives_barrier_cb );
    sion_generic_register_bcastr_cb(
        api_id,
        otf2_sion_collectives_bcast_cb );
    sion_generic_register_gatherr_cb(
        api_id,
        otf2_sion_collectives_gather_cb );
    sion_generic_register_gathervr_cb(
        api_id,
        otf2_sion_collectives_gatherv_cb );
    sion_generic_register_scatterr_cb(
        api_id,
        otf2_sion_collectives_scatter_cb );
    sion_generic_register_scattervr_cb(
        api_id,
        otf2_sion_collectives_scatterv_cb );
}


/** @name Wrappers of collective callbacks for SIONlib.
 *  @{
 */

int
otf2_sion_collectives_create_local_comm_cb( void** localCommContext,
                                            void*  globalCommContext,
                                            int    globalRank,
                                            int    globalSize,
                                            int    localRank,
                                            int    localSize,
                                            int    fileNumber,
                                            int    numberOfFiles )
{
    UTILS_DEBUG_ENTRY();

    struct otf2_sion_comm_proxy* global_proxy =
        globalCommContext;
    OTF2_Archive* archive =
        global_proxy->archive;
    OTF2_CollectiveContext* global_comm =
        otf2_sion_resolve_comm_proxy( global_proxy );

    OTF2_FileSubstrateData* data = archive->per_substrate_data[ OTF2_SUBSTRATE_SION ];

    OTF2_FileMode file_mode = data->fileModeForLocalCommCreate;
    OTF2_FileType file_type = data->fileTypeForLocalCommCreate;

    UTILS_DEBUG( "%d, %d", file_mode, file_type );

    /* at writing take the local comm, if not present, take the global comm */
    if ( file_mode == OTF2_FILEMODE_WRITE )
    {
        if ( archive->local_comm_context )
        {
            *localCommContext = &data->local_comm_proxy;
        }
        else
        {
            *localCommContext = &data->global_comm_proxy;
        }
        return OTF2_SION_SUCCESS;
    }

    /* for reading call otf2_create_local_comm if present, or take the global
     * comm
     */
    if ( !archive->collective_callbacks->otf2_create_local_comm )
    {
        /* @todo this should not happen, we only call _paropen when we have a
           otf2_create_local_comm callback, the _paropen_mapped should never
           call this callback */
        *localCommContext = &data->global_comm_proxy;
        return OTF2_SION_SUCCESS;
    }

    /* call the OTF2 callback to create the local comm and pass the
     * per-file-type proxy back to SIONlib
     */
    OTF2_FileSubstrateFileTypeData* filetype_data = archive->per_filetype_data[ file_type ];
    *localCommContext = &filetype_data->proxy;
    OTF2_CallbackCode status = otf2_collectives_create_local_comm( archive,
                                                                   &filetype_data->local_comm,
                                                                   global_comm,
                                                                   globalRank,
                                                                   globalSize,
                                                                   localRank,
                                                                   localSize,
                                                                   fileNumber,
                                                                   numberOfFiles );

#if HAVE( UTILS_DEBUG )
    uint32_t global_rank;
    uint32_t global_size;
    uint32_t local_rank;
    uint32_t local_size;
    otf2_collectives_get_rank( archive,
                               archive->global_comm_context,
                               &global_rank );
    otf2_collectives_get_size( archive,
                               archive->global_comm_context,
                               &global_size );
    otf2_collectives_get_rank( archive,
                               filetype_data->local_comm,
                               &local_rank );
    otf2_collectives_get_size( archive,
                               filetype_data->local_comm,
                               &local_size );
    UTILS_DEBUG( "file %d/%d; requested: %u/%u -> %u/%u; created: %u/%u -> %u/%u",
                 fileNumber, numberOfFiles,
                 globalRank, globalSize,
                 localRank, localSize,
                 global_rank, global_size,
                 local_rank, local_size );
#endif

    return OTF2_SION_SUCCESS;
}


int
otf2_sion_collectives_free_local_comm_cb( void* localCommContext )
{
    UTILS_DEBUG_ENTRY();

    struct otf2_sion_comm_proxy* proxy   = localCommContext;
    OTF2_Archive*                archive = proxy->archive;
    OTF2_CollectiveContext*      comm    =
        otf2_sion_resolve_comm_proxy( proxy );

    if ( proxy->type != OTF2_SION_PROXY_TYPE_LOCAL_FILE_TYPE )
    {
        /* don't call free_local_comm on non-created local comms */
        return OTF2_SION_SUCCESS;
    }

    OTF2_CallbackCode status = otf2_collectives_free_local_comm( proxy->archive,
                                                                 comm );

    return OTF2_SION_SUCCESS;
}


int
otf2_sion_collectives_barrier_cb( void* commContext )
{
    UTILS_DEBUG_ENTRY();

    struct otf2_sion_comm_proxy* proxy = commContext;

    OTF2_CallbackCode status = OTF2_CALLBACK_SUCCESS;

    status = otf2_collectives_barrier( proxy->archive,
                                       otf2_sion_resolve_comm_proxy( proxy ) );

    return OTF2_SION_SUCCESS;
}


int
otf2_sion_collectives_bcast_cb( void* data,
                                void* commContext,
                                int   type,
                                int   nelem,
                                int   root )
{
    UTILS_DEBUG_ENTRY();

    struct otf2_sion_comm_proxy* proxy = commContext;

    OTF2_CallbackCode status = OTF2_CALLBACK_SUCCESS;

    OTF2_Type otf2_type = otf2_get_otf2_type_from_sion_type( type );

    status = otf2_collectives_bcast( proxy->archive,
                                     otf2_sion_resolve_comm_proxy( proxy ),
                                     data,
                                     nelem,
                                     otf2_type,
                                     root );

    return OTF2_SION_SUCCESS;
}


int
otf2_sion_collectives_gather_cb( void* inData,
                                 void* outData,
                                 void* commContext,
                                 int   type,
                                 int   nelem,
                                 int   root )
{
    UTILS_DEBUG_ENTRY();

    struct otf2_sion_comm_proxy* proxy = commContext;

    OTF2_CallbackCode status = OTF2_CALLBACK_SUCCESS;

    OTF2_Type otf2_type = otf2_get_otf2_type_from_sion_type( type );

    status = otf2_collectives_gather( proxy->archive,
                                      otf2_sion_resolve_comm_proxy( proxy ),
                                      inData,
                                      outData,
                                      nelem,
                                      otf2_type,
                                      root );

    return OTF2_SION_SUCCESS;
}


int
otf2_sion_collectives_gatherv_cb( void* inData,
                                  void* outData,
                                  void* commContext,
                                  int   type,
                                  int*  counts,
                                  int   nelem,
                                  int   root )
{
    UTILS_DEBUG_ENTRY();

    struct otf2_sion_comm_proxy* proxy = commContext;
    OTF2_CollectiveContext*      comm  = otf2_sion_resolve_comm_proxy( proxy );

    OTF2_CallbackCode status = OTF2_CALLBACK_SUCCESS;

    OTF2_Type otf2_type = otf2_get_otf2_type_from_sion_type( type );
    status = otf2_collectives_gatherv( proxy->archive,
                                       comm,
                                       inData,
                                       nelem,
                                       outData,
                                       ( const uint32_t* )counts,
                                       otf2_type,
                                       root );

    return OTF2_SION_SUCCESS;
}


int
otf2_sion_collectives_scatter_cb( void* inData,
                                  void* outData,
                                  void* commContext,
                                  int   type,
                                  int   nelem,
                                  int   root )
{
    UTILS_DEBUG_ENTRY();

    struct otf2_sion_comm_proxy* proxy = commContext;

    OTF2_CallbackCode status = OTF2_CALLBACK_SUCCESS;

    OTF2_Type otf2_type = otf2_get_otf2_type_from_sion_type( type );

    status = otf2_collectives_scatter( proxy->archive,
                                       otf2_sion_resolve_comm_proxy( proxy ),
                                       inData,
                                       outData,
                                       nelem,
                                       otf2_type,
                                       root );

    return OTF2_SION_SUCCESS;
}


int
otf2_sion_collectives_scatterv_cb( void* inData,
                                   void* outData,
                                   void* commContext,
                                   int   type,
                                   int*  counts,
                                   int   nelem,
                                   int   root )
{
    UTILS_DEBUG_ENTRY();

    struct otf2_sion_comm_proxy* proxy = commContext;
    OTF2_CollectiveContext*      comm  = otf2_sion_resolve_comm_proxy( proxy );

    OTF2_CallbackCode status = OTF2_CALLBACK_SUCCESS;

    OTF2_Type otf2_type = otf2_get_otf2_type_from_sion_type( type );

    status = otf2_collectives_scatterv( proxy->archive,
                                        comm,
                                        inData,
                                        ( const uint32_t* )counts,
                                        outData,
                                        nelem,
                                        otf2_type,
                                        root );

    return OTF2_SION_SUCCESS;
}

/**
 * @}
 */
