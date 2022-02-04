/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2014                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of OTF2 locking callback functions.
 *
 *  This file provides the implementation of OTF2 callback functions
 *  providing locking functionality, required to make the OTF2 reader
 *  thread-safe.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "Otf2LockCallbacks.h"

#include <exception>


// --- OTF2: Locking callbacks ----------------------------------------------

OTF2_CallbackCode
pearl::detail::otf2LockCreate(void*      userData,
                              OTF2_Lock* lock)
{
    if (!lock)
    {
        return OTF2_CALLBACK_ERROR;
    }

    try
    {
        *lock = new OTF2_LockObject;
    }
    catch (std::exception& ex)
    {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2LockDestroy(void*     userData,
                               OTF2_Lock lock)
{
    if (!lock)
    {
        return OTF2_CALLBACK_ERROR;
    }

    delete lock;

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2LockAcquire(void*     userData,
                               OTF2_Lock lock)
{
    if (!lock)
    {
        return OTF2_CALLBACK_ERROR;
    }

    lock->mMutex.lock();

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2LockRelease(void*     userData,
                               OTF2_Lock lock)
{
    if (!lock)
    {
        return OTF2_CALLBACK_ERROR;
    }

    lock->mMutex.unlock();

    return OTF2_CALLBACK_SUCCESS;
}
