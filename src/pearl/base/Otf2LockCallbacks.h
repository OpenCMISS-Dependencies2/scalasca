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
 *  @brief   Declarations of OTF2 locking callback functions.
 *
 *  This header file provides the declarations of OTF2 callback functions
 *  and related data types implementing locking functionality, required to
 *  make the OTF2 reader thread-safe.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_OTF2LOCKCALLBACKS_H
#define PEARL_OTF2LOCKCALLBACKS_H


#include <otf2/OTF2_Callbacks.h>

#include "Threading.h"


/*-------------------------------------------------------------------------*/
/**
 *  @struct  OTF2_LockObject
 *  @ingroup PEARL_base
 *  @brief   OTF2 locking object
 *
 *  Locking data type that is used internally by the OTF2 library to ensure
 *  exclusive access to shared resources via corresponding callback functions.
 **/
/*-------------------------------------------------------------------------*/

struct OTF2_LockObject
{
    /// Underlying mutex variable
    pearl::Mutex mMutex;
};


namespace pearl
{
namespace detail
{
// --- Function prototypes --------------------------------------------------

/// @name OTF2: Locking callbacks
/// @{

/// @brief Lock creation callback
///
/// %Callback function that is triggered whenever the OTF2 library requires
/// a locking object to ensure exclusive access to shared resources.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param[out] lock
///     Pointer to a variable which should be initialized with a pointer to
///     a newly created instance of OTF2_LockObject
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_ERROR_LOCKING_CALLBACK
///     otherwise
///
extern "C" OTF2_CallbackCode
otf2LockCreate(void*      userData,
               OTF2_Lock* lock);

/// @brief Lock destruction callback
///
/// %Callback function that is triggered whenever the OTF2 library destroys
/// a locking object previously created by a call to the otf2LockCreate()
/// callback function.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param lock
///     Pointer to the corresponding OTF2_LockObject
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_ERROR_LOCKING_CALLBACK
///     otherwise
///
extern "C" OTF2_CallbackCode
otf2LockDestroy(void*     userData,
                OTF2_Lock lock);

/// @brief Lock acquire callback
///
/// %Callback function that is triggered whenever the OTF2 library is acquiring
/// access to a locking object.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param lock
///     Pointer to the corresponding OTF2_LockObject
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_ERROR_LOCKING_CALLBACK
///     otherwise
///
extern "C" OTF2_CallbackCode
otf2LockAcquire(void*     userData,
                OTF2_Lock lock);


/// @brief Lock release callback
///
/// %Callback function that is triggered whenever the OTF2 library is releasing
/// access to a locking object.
///
/// @param userData
///     Pointer to user-provided callback data
/// @param lock
///     Pointer to the corresponding OTF2_LockObject
/// @returns
///     OTF2_CALLBACK_SUCCESS if successful, OTF2_ERROR_LOCKING_CALLBACK
///     otherwise
///
extern "C" OTF2_CallbackCode
otf2LockRelease(void*     userData,
                OTF2_Lock lock);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_OTF2LOCKCALLBACKS_H
