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

#include <config.h>

#include <stdlib.h>

#include <otf2/otf2.h>

#include <cti.h>

#include "otf2-cti.h"


struct OTF2_CollectiveContext
{
    CTI_Group cti_group;
};


typedef struct OTF2_CTI_UserData
{
    OTF2_CollectiveCallbacks callbacks;
    OTF2_CollectiveContext   global;
    OTF2_CollectiveContext   local;
    bool                     local_created;
} OTF2_CTI_UserData;


static inline size_t
otf2_cti_type_to_size( OTF2_Type type )
{
    switch ( type )
    {
        case OTF2_TYPE_UINT8:
        case OTF2_TYPE_INT8:
            return 1;
        case OTF2_TYPE_UINT16:
        case OTF2_TYPE_INT16:
            return 2;
        case OTF2_TYPE_UINT32:
        case OTF2_TYPE_INT32:
        case OTF2_TYPE_FLOAT:
            return 4;
        case OTF2_TYPE_UINT64:
        case OTF2_TYPE_INT64:
        case OTF2_TYPE_DOUBLE:
            return 8;
        default:
            return 0;
    }
}


static void
otf2_cti_collectives_release( void*                   userData,
                              OTF2_CollectiveContext* globalCommContext,
                              OTF2_CollectiveContext* localCommContext )
{
    OTF2_CTI_UserData* user_data = userData;
    if ( user_data->local_created )
    {
        CTI_Group_free( &localCommContext->cti_group );
    }
    free( user_data );
}


static OTF2_CallbackCode
otf2_cti_collectives_create_local_comm( void*                    userData,
                                        OTF2_CollectiveContext** localCommContextOut,
                                        OTF2_CollectiveContext*  globalCommContext,
                                        uint32_t                 globalRank,
                                        uint32_t                 globalSize,
                                        uint32_t                 localRank,
                                        uint32_t                 localSize,
                                        uint32_t                 fileNumber,
                                        uint32_t                 numberOfFiles )
{
    OTF2_CollectiveContext* new_comm_context = malloc( sizeof( *new_comm_context ) );
    CTI_Group_split( globalCommContext->cti_group,
                     fileNumber,
                     localRank,
                     &new_comm_context->cti_group );
    *localCommContextOut = new_comm_context;

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_cti_collectives_free_local_comm( void*                   userData,
                                      OTF2_CollectiveContext* localCommContext )
{
    CTI_Group_free( &localCommContext->cti_group );
    free( localCommContext );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_cti_collectives_get_size( void*                   userData,
                               OTF2_CollectiveContext* commContext,
                               uint32_t*               size )
{
    int group_size;
    CTI_Group_size( commContext->cti_group, &group_size );
    *size = group_size;

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_cti_collectives_get_rank( void*                   userData,
                               OTF2_CollectiveContext* commContext,
                               uint32_t*               rank )
{
    int group_rank;
    CTI_Group_rank( commContext->cti_group, &group_rank );
    *rank = group_rank;

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_cti_collectives_barrier( void*                   userData,
                              OTF2_CollectiveContext* commContext )
{
    CTI_Barrier( commContext->cti_group );
    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_cti_collectives_bcast( void*                   userData,
                            OTF2_CollectiveContext* commContext,
                            void*                   data,
                            uint32_t                numberElements,
                            OTF2_Type               type,
                            uint32_t                root )
{
    size_t size = numberElements * otf2_cti_type_to_size( type );

    CTI_Bcast( commContext->cti_group,
               data,
               size,
               root );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_cti_collectives_gather( void*                   userData,
                             OTF2_CollectiveContext* commContext,
                             const void*             inData,
                             void*                   outData,
                             uint32_t                numberElements,
                             OTF2_Type               type,
                             uint32_t                root )
{
    size_t size = numberElements * otf2_cti_type_to_size( type );

    CTI_Gather( commContext->cti_group,
                inData,
                outData,
                size,
                root );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_cti_collectives_gatherv( void*                   userData,
                              OTF2_CollectiveContext* commContext,
                              const void*             inData,
                              uint32_t                inElements,
                              void*                   outData,
                              const uint32_t*         outElements,
                              OTF2_Type               type,
                              uint32_t                root )
{
    size_t size = inElements * otf2_cti_type_to_size( type );

    CTI_Gatherv( commContext->cti_group,
                 inData,
                 outData,
                 size,
                 root );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_cti_collectives_scatter( void*                   userData,
                              OTF2_CollectiveContext* commContext,
                              const void*             inData,
                              void*                   outData,
                              uint32_t                numberElements,
                              OTF2_Type               type,
                              uint32_t                root )
{
    size_t size = numberElements * otf2_cti_type_to_size( type );

    CTI_Scatter( commContext->cti_group,
                 inData,
                 outData,
                 size,
                 root );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_cti_collectives_scatterv( void*                   userData,
                               OTF2_CollectiveContext* commContext,
                               const void*             inData,
                               const uint32_t*         inElements,
                               void*                   outData,
                               uint32_t                outElements,
                               OTF2_Type               type,
                               uint32_t                root )
{
    size_t size = outElements * otf2_cti_type_to_size( type );

    CTI_Scatterv( commContext->cti_group,
                  inData,
                  outData,
                  size,
                  root );

    return OTF2_CALLBACK_SUCCESS;
}


static const OTF2_CollectiveCallbacks otf2_cti_collectives =
{
    .otf2_release           = otf2_cti_collectives_release,
    .otf2_get_size          = otf2_cti_collectives_get_size,
    .otf2_get_rank          = otf2_cti_collectives_get_rank,
    .otf2_create_local_comm = otf2_cti_collectives_create_local_comm,
    .otf2_free_local_comm   = otf2_cti_collectives_free_local_comm,
    .otf2_barrier           = otf2_cti_collectives_barrier,
    .otf2_bcast             = otf2_cti_collectives_bcast,
    .otf2_gather            = otf2_cti_collectives_gather,
    .otf2_gatherv           = otf2_cti_collectives_gatherv,
    .otf2_scatter           = otf2_cti_collectives_scatter,
    .otf2_scatterv          = otf2_cti_collectives_scatterv
};


OTF2_ErrorCode
OTF2_CTI_Archive_SetCollectiveCallbacks( OTF2_Archive* archive,
                                         CTI_Group     globalGroup,
                                         CTI_Group     localGroup )
{
    if ( !archive )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    if ( CTI_GROUP_NULL == globalGroup )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    OTF2_CTI_UserData* user_data = malloc( sizeof( *user_data ) );
    if ( !user_data )
    {
        return OTF2_ERROR_MEM_ALLOC_FAILED;
    }

    user_data->callbacks = otf2_cti_collectives;

    OTF2_CollectiveContext* global = &user_data->global;
    global->cti_group = globalGroup;

    OTF2_CollectiveContext* local = NULL;
    user_data->local_created = false;
    if ( CTI_GROUP_NULL != localGroup )
    {
        local            = &user_data->local;
        local->cti_group = localGroup;
    }

    return OTF2_Archive_SetCollectiveCallbacks( archive,
                                                &user_data->callbacks,
                                                user_data,
                                                global,
                                                local );
}


OTF2_ErrorCode
OTF2_CTI_Archive_SetCollectiveCallbacksSplit( OTF2_Archive* archive,
                                              CTI_Group     globalGroup,
                                              uint32_t      numberOfFiles )
{
    if ( !archive )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    if ( CTI_GROUP_NULL == globalGroup )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    OTF2_CTI_UserData* user_data = malloc( sizeof( *user_data ) );
    if ( !user_data )
    {
        return OTF2_ERROR_MEM_ALLOC_FAILED;
    }

    user_data->callbacks = otf2_cti_collectives;

    OTF2_CollectiveContext* global = &user_data->global;
    global->cti_group = globalGroup;

    int size;
    CTI_Group_size( globalGroup, &size );
    int rank;
    CTI_Group_rank( globalGroup, &rank );

    OTF2_CollectiveContext* local = &user_data->local;
    user_data->local_created = true;

    int file_number = 0;
    int rem         = size % numberOfFiles;
    int local_size  = size / numberOfFiles + !!rem;
    int local_rank  = 0;
    int local_root  = 0;
    for ( int i = 0; i < rank; i++ )
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

    CTI_Group_split( globalGroup,
                     file_number,
                     local_rank,
                     &local->cti_group );

    return OTF2_Archive_SetCollectiveCallbacks( archive,
                                                &user_data->callbacks,
                                                user_data,
                                                global,
                                                local );
}


OTF2_ErrorCode
OTF2_CTI_Reader_SetCollectiveCallbacks( OTF2_Reader* reader,
                                        CTI_Group    globalGroup,
                                        bool         allowLocalGroup )
{
    if ( !reader )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    if ( CTI_GROUP_NULL == globalGroup )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    OTF2_CTI_UserData* user_data = malloc( sizeof( *user_data ) );
    if ( !user_data )
    {
        return OTF2_ERROR_MEM_ALLOC_FAILED;
    }

    user_data->callbacks = otf2_cti_collectives;

    OTF2_CollectiveContext* global = &user_data->global;
    global->cti_group = globalGroup;

    if ( !allowLocalGroup )
    {
        user_data->callbacks.otf2_create_local_comm = NULL;
        user_data->callbacks.otf2_free_local_comm   = NULL;
    }
    user_data->local_created = false;

    return OTF2_Reader_SetCollectiveCallbacks( reader,
                                               &user_data->callbacks,
                                               user_data,
                                               global,
                                               NULL );
}
