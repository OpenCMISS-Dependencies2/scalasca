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


/**
 *  @internal
 *
 *  @file
 *
 *  @brief      Callbacks for the serial case.
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
#define OTF2_DEBUG_MODULE_NAME COLLECTIVES
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"
#include "otf2_archive_location.h"

#include "otf2_collectives.h"


static inline const char*
otf2_print_get_type( OTF2_Type type )
{
    switch ( type )
    {
        case OTF2_TYPE_UINT8:
            return "UINT8";
        case OTF2_TYPE_UINT16:
            return "UINT16";
        case OTF2_TYPE_UINT32:
            return "UINT32";
        case OTF2_TYPE_UINT64:
            return "UINT64";
        case OTF2_TYPE_INT8:
            return "INT8";
        case OTF2_TYPE_INT16:
            return "INT16";
        case OTF2_TYPE_INT32:
            return "INT32";
        case OTF2_TYPE_INT64:
            return "INT64";
        case OTF2_TYPE_FLOAT:
            return "FLOAT";
        case OTF2_TYPE_DOUBLE:
            return "DOUBLE";

        default:
            UTILS_BUG( "Invalid type for collective operations: %hhu", type );
            return "<INVALID>";
    }
}


OTF2_CallbackCode
otf2_collectives_get_size( OTF2_Archive*           archive,
                           OTF2_CollectiveContext* commContext,
                           uint32_t*               size )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_get_size,
                  "collective callback get_size unset" );
    return archive->collective_callbacks->
           otf2_get_size( archive->collective_data,
                          commContext,
                          size );
}


OTF2_CallbackCode
otf2_collectives_get_rank( OTF2_Archive*           archive,
                           OTF2_CollectiveContext* commContext,
                           uint32_t*               rank )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_get_rank,
                  "collective callback get_rank unset" );

    return archive->collective_callbacks->
           otf2_get_rank( archive->collective_data,
                          commContext,
                          rank );
}


OTF2_CallbackCode
otf2_collectives_create_local_comm( OTF2_Archive*            archive,
                                    OTF2_CollectiveContext** localCommContext,
                                    OTF2_CollectiveContext*  globalCommContext,
                                    uint32_t                 globalRank,
                                    uint32_t                 globalSize,
                                    uint32_t                 localRank,
                                    uint32_t                 localSize,
                                    uint32_t                 fileNumber,
                                    uint32_t                 numberOfFiles )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_create_local_comm,
                  "collective callback create_local_comm unset" );

    return archive->collective_callbacks->
           otf2_create_local_comm( archive->collective_data,
                                   localCommContext,
                                   globalCommContext,
                                   globalRank,
                                   globalSize,
                                   localRank,
                                   localSize,
                                   fileNumber,
                                   numberOfFiles );
}


OTF2_CallbackCode
otf2_collectives_free_local_comm( OTF2_Archive*           archive,
                                  OTF2_CollectiveContext* localCommContext )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_free_local_comm,
                  "collective callback free_local_comm unset" );

    return archive->collective_callbacks->
           otf2_free_local_comm( archive->collective_data,
                                 localCommContext );
}


OTF2_CallbackCode
otf2_collectives_barrier( OTF2_Archive*           archive,
                          OTF2_CollectiveContext* commContext )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_barrier,
                  "collective callback barrier unset" );

    return archive->collective_callbacks->
           otf2_barrier( archive->collective_data,
                         commContext );
}


OTF2_CallbackCode
otf2_collectives_bcast( OTF2_Archive*           archive,
                        OTF2_CollectiveContext* commContext,
                        void*                   data,
                        uint32_t                numberElements,
                        OTF2_Type               type,
                        uint32_t                root )
{
    UTILS_DEBUG_ENTRY( "%p, %p, %u, %s, %u",
                       commContext,
                       data,
                       numberElements,
                       otf2_print_get_type( type ),
                       root );
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_bcast,
                  "collective callback bcast unset" );

    return archive->collective_callbacks->
           otf2_bcast( archive->collective_data,
                       commContext,
                       data,
                       numberElements,
                       type,
                       root );
}


OTF2_CallbackCode
otf2_collectives_gather( OTF2_Archive*           archive,
                         OTF2_CollectiveContext* commContext,
                         const void*             inData,
                         void*                   outData,
                         uint32_t                numberElements,
                         OTF2_Type               type,
                         uint32_t                root )
{
    UTILS_DEBUG_ENTRY( "%p, %p, %p, %u, %s, %u",
                       commContext,
                       inData,
                       outData,
                       numberElements,
                       otf2_print_get_type( type ),
                       root );
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_gather,
                  "collective callback gather unset" );

    return archive->collective_callbacks->
           otf2_gather( archive->collective_data,
                        commContext,
                        inData,
                        outData,
                        numberElements,
                        type,
                        root );
}


OTF2_CallbackCode
otf2_collectives_gatherv( OTF2_Archive*           archive,
                          OTF2_CollectiveContext* commContext,
                          const void*             inData,
                          uint32_t                inElements,
                          void*                   outData,
                          const uint32_t*         outElements,
                          OTF2_Type               type,
                          uint32_t                root )
{
    UTILS_DEBUG_ENTRY( "%p, %p, %u, %p, %p, %s, %u",
                       commContext,
                       inData,
                       inElements,
                       outData,
                       outElements,
                       otf2_print_get_type( type ),
                       root );
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_gatherv,
                  "collective callback gatherv unset" );

    return archive->collective_callbacks->
           otf2_gatherv( archive->collective_data,
                         commContext,
                         inData,
                         inElements,
                         outData,
                         outElements,
                         type,
                         root );
}


OTF2_CallbackCode
otf2_collectives_scatter( OTF2_Archive*           archive,
                          OTF2_CollectiveContext* commContext,
                          const void*             inData,
                          void*                   outData,
                          uint32_t                numberElements,
                          OTF2_Type               type,
                          uint32_t                root )
{
    UTILS_DEBUG_ENTRY( "%p, %p, %p, %u, %s, %u",
                       commContext,
                       inData,
                       outData,
                       numberElements,
                       otf2_print_get_type( type ),
                       root );
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_scatter,
                  "collective callback scatter unset" );

    return archive->collective_callbacks->
           otf2_scatter( archive->collective_data,
                         commContext,
                         inData,
                         outData,
                         numberElements,
                         type,
                         root );
}


OTF2_CallbackCode
otf2_collectives_scatterv( OTF2_Archive*           archive,
                           OTF2_CollectiveContext* commContext,
                           const void*             inData,
                           const uint32_t*         inElements,
                           void*                   outData,
                           uint32_t                outElements,
                           OTF2_Type               type,
                           uint32_t                root )
{
    UTILS_DEBUG_ENTRY( "%p, %p, %p, %p, %u, %s, %u",
                       commContext,
                       inData,
                       inElements,
                       outData,
                       outElements,
                       otf2_print_get_type( type ),
                       root );
    UTILS_BUG_ON( !archive->collective_callbacks, "collective callbacks unset" );
    UTILS_BUG_ON( !archive->collective_callbacks->otf2_scatterv,
                  "collective callback scatterv unset" );

    return archive->collective_callbacks->
           otf2_scatterv( archive->collective_data,
                          commContext,
                          inData,
                          inElements,
                          outData,
                          outElements,
                          type,
                          root );
}
