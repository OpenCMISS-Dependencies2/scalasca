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
 *  @brief      Pthread locks for OTF2.
 *
 */


#ifndef OTF2_PTHREAD_LOCKS_H
#define OTF2_PTHREAD_LOCKS_H


#include <otf2/otf2.h>


#include <pthread.h>


/** @brief Register callbacks to use Pthread mutexes for a OTF2 archive.
 *
 *  @param archive        The archive handle.
 *  @param mutexAttribute A possible @a pthread_mutexattr_t which will be used in
 *                        all @a pthread_mutex_init calls. A corresponding
 *                        @a pthread_mutexattr_destroy call is done
 *                        when the archive will be closed. But no such call will
 *                        be issued, if a call to this function will fail.
 *
 *  @since Version 1.5
 *
 *  @return Success or error code.
 */
static OTF2_ErrorCode
OTF2_Pthread_Archive_SetLockingCallbacks( OTF2_Archive*              archive,
                                          const pthread_mutexattr_t* mutexAttribute );


/** @brief Register callbacks to use Pthread mutexes for a OTF2 reader.
 *
 *  @param reader         The reader handle.
 *  @param mutexAttribute A possible @a pthread_mutexattr_t which will e used in
 *                        all @a pthread_mutex_init calls. A corresponding
 *                        @a pthread_mutexattr_destroy call is done
 *                        when the reader will be closed. But no such call will
 *                        be issued, if a call to this function will fail.
 *
 *  @since Version 1.5
 *
 *  @return Success or error code.
 */
static OTF2_ErrorCode
OTF2_Pthread_Reader_SetLockingCallbacks( OTF2_Reader*               reader,
                                         const pthread_mutexattr_t* mutexAttribute );


/**
 * @cond IMPLEMENTATION_OF_THE_CALLBACKS__PLEASE_IGNORE
 */


/** @brief The Pthread locking object type.
 */
struct OTF2_LockObject
{
    pthread_mutex_t mutex;
};


/** @brief User data structure, which will be used by the Pthread locks.
 */
typedef struct OTF2_Pthread_UserData
{
    const pthread_mutexattr_t* mutex_attribute;
} OTF2_Pthread_UserData;


static void
otf2_pthread_lock_release( void* userData )
{
    OTF2_Pthread_UserData* user_data = ( OTF2_Pthread_UserData* )userData;

    if ( user_data->mutex_attribute )
    {
        /* Ignore errors */
        pthread_mutexattr_destroy( ( pthread_mutexattr_t* )user_data->mutex_attribute );
    }

    free( user_data );
}


static OTF2_CallbackCode
otf2_pthread_lock_create( void*      userData,
                          OTF2_Lock* lock )
{
    OTF2_Pthread_UserData* user_data = ( OTF2_Pthread_UserData* )userData;
    int                    err;

    if ( !lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    *lock = (  OTF2_Lock )malloc( sizeof( **lock ) );
    if ( !*lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    err = pthread_mutex_init( &( *lock )->mutex, user_data->mutex_attribute );
    if ( 0 != err )
    {
        free( *lock );
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
otf2_pthread_lock_destroy( void*     userData,
                           OTF2_Lock lock )
{
    int err;

    ( void )userData;

    if ( !lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    err = pthread_mutex_destroy( &lock->mutex );
    free( lock );

    return 0 == err ? OTF2_CALLBACK_SUCCESS : OTF2_CALLBACK_ERROR;
}


static OTF2_CallbackCode
otf2_pthread_lock_lock( void*     userData,
                        OTF2_Lock lock )
{
    int err;

    ( void )userData;

    if ( !lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    err = pthread_mutex_lock( &lock->mutex );

    return 0 == err ? OTF2_CALLBACK_SUCCESS : OTF2_CALLBACK_ERROR;
}


static OTF2_CallbackCode
otf2_pthread_lock_unlock( void*     userData,
                          OTF2_Lock lock )
{
    int err;

    ( void )userData;

    if ( !lock )
    {
        return OTF2_CALLBACK_ERROR;
    }

    err = pthread_mutex_unlock( &lock->mutex );

    return 0 == err ? OTF2_CALLBACK_SUCCESS : OTF2_CALLBACK_ERROR;
}


static const OTF2_LockingCallbacks otf2_pthread_locking_callbacks =
{
    otf2_pthread_lock_release,
    otf2_pthread_lock_create,
    otf2_pthread_lock_destroy,
    otf2_pthread_lock_lock,
    otf2_pthread_lock_unlock
};


static OTF2_ErrorCode
OTF2_Pthread_Archive_SetLockingCallbacks( OTF2_Archive*              archive,
                                          const pthread_mutexattr_t* mutexAttribute )
{
    OTF2_ErrorCode         ret;
    OTF2_Pthread_UserData* user_data = NULL;

    ( void )OTF2_Pthread_Reader_SetLockingCallbacks;

    if ( !archive )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    user_data = ( OTF2_Pthread_UserData* )calloc( 1, sizeof( *user_data ) );
    if ( !user_data )
    {
        return OTF2_ERROR_MEM_ALLOC_FAILED;
    }

    user_data->mutex_attribute = mutexAttribute;

    ret = OTF2_Archive_SetLockingCallbacks( archive,
                                            &otf2_pthread_locking_callbacks,
                                            user_data );
    if ( OTF2_SUCCESS != ret )
    {
        free( user_data );
    }
    return ret;
}


static OTF2_ErrorCode
OTF2_Pthread_Reader_SetLockingCallbacks( OTF2_Reader*               reader,
                                         const pthread_mutexattr_t* mutexAttribute )
{
    OTF2_ErrorCode         ret;
    OTF2_Pthread_UserData* user_data = NULL;

    ( void )OTF2_Pthread_Archive_SetLockingCallbacks;

    if ( !reader )
    {
        return OTF2_ERROR_INVALID_ARGUMENT;
    }

    user_data = ( OTF2_Pthread_UserData* )calloc( 1, sizeof( *user_data ) );
    if ( !user_data )
    {
        return OTF2_ERROR_MEM_ALLOC_FAILED;
    }

    user_data->mutex_attribute = mutexAttribute;

    ret = OTF2_Reader_SetLockingCallbacks( reader,
                                           &otf2_pthread_locking_callbacks,
                                           user_data );
    if ( OTF2_SUCCESS != ret )
    {
        free( user_data );
    }
    return ret;
}


/**
 * @endcond
 */


#endif /* OTF2_PTHREAD_LOCKS_H */
