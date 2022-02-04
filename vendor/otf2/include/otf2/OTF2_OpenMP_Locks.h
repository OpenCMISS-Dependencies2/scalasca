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
 *  @file
 *
 *  @brief      OpenMP locks for OTF2.
 *
 */


#ifndef OTF2_OPENMP_LOCKS_H
#define OTF2_OPENMP_LOCKS_H


#include <otf2/otf2.h>


#include <omp.h>


/** @brief Register callbacks to use OpenMP locks for a OTF2 archive.
 *
 *  @param archive  The archive handle.
 *
 *  @since Version 1.5
 *
 *  @return Success or error code.
 */
static OTF2_ErrorCode
OTF2_OpenMP_Archive_SetLockingCallbacks( OTF2_Archive* archive );


/** @brief Register callbacks to use OpenMP locks for a OTF2 reader.
 *
 *  @param reader  The reader handle.
 *
 *  @since Version 1.5
 *
 *  @return Success or error code.
 */
static OTF2_ErrorCode
OTF2_OpenMP_Reader_SetLockingCallbacks( OTF2_Reader* reader );


/**
 * @cond IMPLEMENTATION_OF_THE_CALLBACKS__PLEASE_IGNORE
 */


/** @brief The OpenMP locking object type.
 */
struct OTF2_LockObject
{
    omp_lock_t lock;
};


static OTF2_CallbackCode
otf2_openmp_lock_create( void*      userData,
                         OTF2_Lock* lock )
{
    if ( !lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    *lock = (  OTF2_Lock )malloc( sizeof( **lock ) );
    if ( !*lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    omp_init_lock( &( *lock )->lock );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_openmp_lock_destroy( void*     userData,
                          OTF2_Lock lock )
{
    if ( !lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    omp_destroy_lock( &lock->lock );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_openmp_lock_lock( void*     userData,
                       OTF2_Lock lock )
{
    if ( !lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    omp_set_lock( &lock->lock );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_openmp_lock_unlock( void*     userData,
                         OTF2_Lock lock )
{
    if ( !lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    omp_unset_lock( &lock->lock );

    return OTF2_CALLBACK_SUCCESS;
}


static const OTF2_LockingCallbacks otf2_openmp_locking_callbacks =
{
    NULL,
    otf2_openmp_lock_create,
    otf2_openmp_lock_destroy,
    otf2_openmp_lock_lock,
    otf2_openmp_lock_unlock
};


static OTF2_ErrorCode
OTF2_OpenMP_Archive_SetLockingCallbacks( OTF2_Archive* archive )
{
    ( void )OTF2_OpenMP_Reader_SetLockingCallbacks;

    if ( !archive )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    return OTF2_Archive_SetLockingCallbacks( archive,
                                             &otf2_openmp_locking_callbacks,
                                             NULL );
}


static OTF2_ErrorCode
OTF2_OpenMP_Reader_SetLockingCallbacks( OTF2_Reader* reader )
{
    ( void )OTF2_OpenMP_Archive_SetLockingCallbacks;

    if ( !reader )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    return OTF2_Reader_SetLockingCallbacks( reader,
                                            &otf2_openmp_locking_callbacks,
                                            NULL );
}


/**
 * @endcond
 */


#endif /* OTF2_OPENMP_LOCKS_H */
