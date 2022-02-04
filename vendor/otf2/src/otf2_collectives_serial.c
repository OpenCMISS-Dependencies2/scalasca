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
#include <UTILS_Debug.h>

static inline size_t
otf2_type_to_size( OTF2_Type type )
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


static OTF2_CallbackCode
otf2_collectives_serial_get_size( void*                   userData,
                                  OTF2_CollectiveContext* commContext,
                                  uint32_t*               size )
{
    *size = 1;
    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_collectives_serial_get_rank( void*                   userData,
                                  OTF2_CollectiveContext* commContext,
                                  uint32_t*               rank )
{
    *rank = 0;
    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_collectives_serial_barrier( void*                   userData,
                                 OTF2_CollectiveContext* commContext )
{
    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_collectives_serial_bcast( void*                   userData,
                               OTF2_CollectiveContext* commContext,
                               void*                   data,
                               uint32_t                numberElements,
                               OTF2_Type               type,
                               uint32_t                root )
{
    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_collectives_serial_gather( void*                   userData,
                                OTF2_CollectiveContext* commContext,
                                const void*             inData,
                                void*                   outData,
                                uint32_t                numberElements,
                                OTF2_Type               type,
                                uint32_t                root )
{
    memcpy( outData, inData, otf2_type_to_size( type ) * numberElements );
    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_collectives_serial_gatherv( void*                   userData,
                                 OTF2_CollectiveContext* commContext,
                                 const void*             inData,
                                 uint32_t                inElements,
                                 void*                   outData,
                                 const uint32_t*         outElements,
                                 OTF2_Type               type,
                                 uint32_t                root )
{
    memcpy( outData, inData, otf2_type_to_size( type ) * inElements );
    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_collectives_serial_scatter( void*                   userData,
                                 OTF2_CollectiveContext* commContext,
                                 const void*             inData,
                                 void*                   outData,
                                 uint32_t                numberElements,
                                 OTF2_Type               type,
                                 uint32_t                root )
{
    memcpy( outData, inData, otf2_type_to_size( type ) * numberElements );
    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_collectives_serial_scatterv( void*                   userData,
                                  OTF2_CollectiveContext* commContext,
                                  const void*             inData,
                                  const uint32_t*         inElements,
                                  void*                   outData,
                                  uint32_t                outElements,
                                  OTF2_Type               type,
                                  uint32_t                root )
{
    memcpy( outData, inData, otf2_type_to_size( type ) * outElements );
    return OTF2_CALLBACK_SUCCESS;
}


const OTF2_CollectiveCallbacks otf2_collectives_serial =
{
    .otf2_release           = NULL,
    .otf2_get_size          = otf2_collectives_serial_get_size,
    .otf2_get_rank          = otf2_collectives_serial_get_rank,
    .otf2_create_local_comm = NULL,
    .otf2_free_local_comm   = NULL,
    .otf2_barrier           = otf2_collectives_serial_barrier,
    .otf2_bcast             = otf2_collectives_serial_bcast,
    .otf2_gather            = otf2_collectives_serial_gather,
    .otf2_gatherv           = otf2_collectives_serial_gatherv,
    .otf2_scatter           = otf2_collectives_serial_scatter,
    .otf2_scatterv          = otf2_collectives_serial_scatterv
};
