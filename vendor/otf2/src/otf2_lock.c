/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
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
 *  @brief      Lock object handling.
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
#define OTF2_DEBUG_MODULE_NAME LOCKS
#include <UTILS_Debug.h>


#include "otf2_internal.h"
#include "otf2_lock.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"


OTF2_ErrorCode
otf2_lock_create( OTF2_Archive* archive,
                  OTF2_Lock*    lock )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( lock );

    if ( !archive->locking_callbacks )
    {
        *lock = NULL;
        return OTF2_SUCCESS;
    }

    OTF2_CallbackCode callback_ret =
        archive->locking_callbacks->otf2_create( archive->locking_data,
                                                 lock );
    if ( OTF2_CALLBACK_SUCCESS != callback_ret )
    {
        return UTILS_ERROR( OTF2_ERROR_LOCKING_CALLBACK,
                            "Can't create lock." );
    }

    UTILS_DEBUG( "Created lock %p", *lock );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_lock_destroy( OTF2_Archive* archive,
                   OTF2_Lock     lock )
{
    UTILS_ASSERT( archive );

    if ( !archive->locking_callbacks )
    {
        return OTF2_SUCCESS;
    }

    UTILS_DEBUG( "Destroy lock %p", lock );

    OTF2_CallbackCode callback_ret =
        archive->locking_callbacks->otf2_destroy( archive->locking_data,
                                                  lock );
    if ( OTF2_CALLBACK_SUCCESS != callback_ret )
    {
        return UTILS_ERROR( OTF2_ERROR_LOCKING_CALLBACK,
                            "Can't destroy lock %p.", lock );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_lock_lock( OTF2_Archive* archive,
                OTF2_Lock     lock )
{
    UTILS_ASSERT( archive );

    if ( !archive->locking_callbacks )
    {
        return OTF2_SUCCESS;
    }

    UTILS_DEBUG( "Lock lock %p", lock );

    OTF2_CallbackCode callback_ret =
        archive->locking_callbacks->otf2_lock( archive->locking_data, lock );
    if ( OTF2_CALLBACK_SUCCESS != callback_ret )
    {
        return UTILS_ERROR( OTF2_ERROR_LOCKING_CALLBACK,
                            "Can't lock %p.", lock );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_lock_unlock( OTF2_Archive* archive,
                  OTF2_Lock     lock )
{
    UTILS_ASSERT( archive );

    if ( !archive->locking_callbacks )
    {
        return OTF2_SUCCESS;
    }

    UTILS_DEBUG( "Unlock lock %p", lock );

    OTF2_CallbackCode callback_ret =
        archive->locking_callbacks->otf2_unlock( archive->locking_data, lock );
    if ( OTF2_CALLBACK_SUCCESS != callback_ret )
    {
        return UTILS_ERROR( OTF2_ERROR_LOCKING_CALLBACK,
                            "Can't unlock %p.", lock );
    }

    return OTF2_SUCCESS;
}
