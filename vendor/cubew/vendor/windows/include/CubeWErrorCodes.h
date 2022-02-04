/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef CUBEW_ERROR_CODES_H
#define CUBEW_ERROR_CODES_H


/**
 * @file            CUBEW_ErrorCodes.h
 * @ingroup         CUBEW_Exception_module
 *
 * @brief           Error codes and error handling.
 *
 */

#include <errno.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is the list of error codes for CUBEW.
 */
typedef enum
{
    /** Special marker for error messages which indicates a deprecation. */
    CUBEW_DEPRECATED    = -3,

    /** Special marker when the application will be aborted. */
    CUBEW_ABORT         = -2,

    /** Special marker for error messages which are only warnings. */
    CUBEW_WARNING       = -1,

    /** Operation successful */
    CUBEW_SUCCESS       = 0,

    /** Invalid error code
     *
     * Should only be used internally and not as an actual error code.
     */
    CUBEW_ERROR_INVALID = 1,

    /* These are the internal implementation of POSIX error codes. */
    /** The list of arguments is too long */
    CUBEW_ERROR_E2BIG,
    /** Not enough rights */
    CUBEW_ERROR_EACCES,
    /** Address is not available */
    CUBEW_ERROR_EADDRNOTAVAIL,
    /** Address family is not supported */
    CUBEW_ERROR_EAFNOSUPPORT,
    /** Resource temporarily not available */
    CUBEW_ERROR_EAGAIN,
    /** Connection is already processed */
    CUBEW_ERROR_EALREADY,
    /** Invalid file pointer */
    CUBEW_ERROR_EBADF,
    /** Invalid message */
    CUBEW_ERROR_EBADMSG,
    /** Resource or device is busy */
    CUBEW_ERROR_EBUSY,
    /** Operation was aborted */
    CUBEW_ERROR_ECANCELED,
    /** No child process available */
    CUBEW_ERROR_ECHILD,
    /** Connection was refused */
    CUBEW_ERROR_ECONNREFUSED,
    /** Connection was reset */
    CUBEW_ERROR_ECONNRESET,
    /** Resolved deadlock */
    CUBEW_ERROR_EDEADLK,
    /** Destination address was expected */
    CUBEW_ERROR_EDESTADDRREQ,
    /** Domain error */
    CUBEW_ERROR_EDOM,
    /** Reserved */
    CUBEW_ERROR_EDQUOT,
    /** File does already exist */
    CUBEW_ERROR_EEXIST,
    /** Invalid address */
    CUBEW_ERROR_EFAULT,
    /** File is too large */
    CUBEW_ERROR_EFBIG,
    /** Operation is work in progress */
    CUBEW_ERROR_EINPROGRESS,
    /** Interruption of an operating system call */
    CUBEW_ERROR_EINTR,
    /** Invalid argument */
    CUBEW_ERROR_EINVAL,
    /** Generic I/O error */
    CUBEW_ERROR_EIO,
    /** Socket is already connected */
    CUBEW_ERROR_EISCONN,
    /** Target is a directory */
    CUBEW_ERROR_EISDIR,
    /** Too many layers of symbolic links */
    CUBEW_ERROR_ELOOP,
    /** Too many opened files */
    CUBEW_ERROR_EMFILE,
    /** Too many links */
    CUBEW_ERROR_EMLINK,
    /** Message buffer is too small */
    CUBEW_ERROR_EMSGSIZE,
    /** Reserved */
    CUBEW_ERROR_EMULTIHOP,
    /** Filename is too long */
    CUBEW_ERROR_ENAMETOOLONG,
    /** Network is down */
    CUBEW_ERROR_ENETDOWN,
    /** Connection was reset from the network */
    CUBEW_ERROR_ENETRESET,
    /** Network is not reachable */
    CUBEW_ERROR_ENETUNREACH,
    /** Too many opened files */
    CUBEW_ERROR_ENFILE,
    /** No buffer space available */
    CUBEW_ERROR_ENOBUFS,
    /** No more data left in the queue */
    CUBEW_ERROR_ENODATA,
    /** This device does not support this function */
    CUBEW_ERROR_ENODEV,
    /** File or directory does not exist */
    CUBEW_ERROR_ENOENT,
    /** Cannot execute binary */
    CUBEW_ERROR_ENOEXEC,
    /** Locking failed */
    CUBEW_ERROR_ENOLCK,
    /** Reserved */
    CUBEW_ERROR_ENOLINK,
    /** Not enough main memory available */
    CUBEW_ERROR_ENOMEM,
    /** Message has not the expected type */
    CUBEW_ERROR_ENOMSG,
    /** This protocol is not available */
    CUBEW_ERROR_ENOPROTOOPT,
    /** No space left on device */
    CUBEW_ERROR_ENOSPC,
    /** No stream available */
    CUBEW_ERROR_ENOSR,
    /** This is not a stream */
    CUBEW_ERROR_ENOSTR,
    /** Requested function is not implemented */
    CUBEW_ERROR_ENOSYS,
    /** Socket is not connected */
    CUBEW_ERROR_ENOTCONN,
    /** This is not a directory */
    CUBEW_ERROR_ENOTDIR,
    /** This directory is not empty */
    CUBEW_ERROR_ENOTEMPTY,
    /** No socket */
    CUBEW_ERROR_ENOTSOCK,
    /** This operation is not supported */
    CUBEW_ERROR_ENOTSUP,
    /** This IOCTL is not supported by the device */
    CUBEW_ERROR_ENOTTY,
    /** Device is not yet configured */
    CUBEW_ERROR_ENXIO,
    /** Operation is not supported by this socket */
    CUBEW_ERROR_EOPNOTSUPP,
    /** Value is to long for the datatype */
    CUBEW_ERROR_EOVERFLOW,
    /** Operation is not permitted */
    CUBEW_ERROR_EPERM,
    /** Broken pipe */
    CUBEW_ERROR_EPIPE,
    /** Protocol error */
    CUBEW_ERROR_EPROTO,
    /** Protocol is not supported */
    CUBEW_ERROR_EPROTONOSUPPORT,
    /** Wrong protocol type for this socket */
    CUBEW_ERROR_EPROTOTYPE,
    /** Value is out of range */
    CUBEW_ERROR_ERANGE,
    /** Filesystem is read only */
    CUBEW_ERROR_EROFS,
    /** This seek is not allowed */
    CUBEW_ERROR_ESPIPE,
    /** No matching process found */
    CUBEW_ERROR_ESRCH,
    /** Reserved */
    CUBEW_ERROR_ESTALE,
    /** Timeout in file stream or IOCTL */
    CUBEW_ERROR_ETIME,
    /** Connection timed out */
    CUBEW_ERROR_ETIMEDOUT,
    /** File could not be executed while it is opened */
    CUBEW_ERROR_ETXTBSY,
    /** Operation would be blocking */
    CUBEW_ERROR_EWOULDBLOCK,
    /** Invalid link between devices */
    CUBEW_ERROR_EXDEV,

    /* These are the error codes specific to the CubeW package */

    /** Function call not allowed in current state */
    CUBEW_ERROR_INVALID_CALL,
    /** Parameter value out of range */
    CUBEW_ERROR_INVALID_ARGUMENT,
    /** Invalid or inconsistent record data */
    CUBEW_ERROR_INVALID_DATA,
    /** The given size cannot be used */
    CUBEW_ERROR_INVALID_SIZE_GIVEN,
    /** The given type is not known */
    CUBEW_ERROR_UNKNOWN_TYPE,
    /** This could not be done with the given memory */
    CUBEW_ERROR_MEM_FAULT,
    /** Memory allocation failed */
    CUBEW_ERROR_MEM_ALLOC_FAILED,
    /** Index out of bounds */
    CUBEW_ERROR_INDEX_OUT_OF_BOUNDS,
    /** Invalid source code line number */
    CUBEW_ERROR_INVALID_LINENO,
    /** End of buffer/file reached */
    CUBEW_ERROR_END_OF_BUFFER,
    /** Unable to open file */
    CUBEW_ERROR_FILE_CAN_NOT_OPEN,
    /** Missing library support for requested compression mode */
    CUBEW_ERROR_FILE_COMPRESSION_NOT_SUPPORTED,
    /** Invalid file operation */
    CUBEW_ERROR_FILE_INTERACTION
} CUBEW_ErrorCode;


/**
 * Returns the name of an error code.
 *
 * @param errorCode : Error Code
 *
 * @returns Returns the name of a known error code, and "INVALID_ERROR" for
 *          invalid or unknown error IDs.
 * @ingroup CUBEW_Exception_module
 */
const char*
CUBEW_Error_GetName( CUBEW_ErrorCode errorCode );


/**
 * Returns the description of an error code.
 *
 * @param errorCode : Error Code
 *
 * @returns Returns the description of a known error code.
 * @ingroup CUBEW_Exception_module
 */
const char*
CUBEW_Error_GetDescription( CUBEW_ErrorCode errorCode );


/**
 * Signature of error handler callback functions. The error handler can be set
 * with @ref CUBEW_Error_RegisterCallback.
 *
 * @param userData        : Data passed to this function as given by the registration call.
 * @param file            : Name of the source-code file where the error appeared
 * @param line            : Line number in the source code where the error appeared
 * @param function        : Name of the function where the error appeared
 * @param errorCode       : Error code
 * @param msgFormatString : Format string like it is used for the printf family.
 * @param va              : Variable argument list
 *
 * @returns Should return the errorCode
 */
typedef CUBEW_ErrorCode
( *CUBEW_ErrorCallback )( void*          userData,
                         const char*    file,
                         uint64_t       line,
                         const char*    function,
                         CUBEW_ErrorCode errorCode,
                         const char*    msgFormatString,
                         va_list        va );


/**
 * Register a programmers callback function for error handling.
 *
 * @param errorCallbackIn : Function which will be called instead of printing a
 *                          default message to standard error.
 * @param userData :        Data pointer passed to the callback.
 *
 * @returns Function pointer to the former error handling function.
 * @ingroup CUBEW_Exception_module
 *
 */
CUBEW_ErrorCallback
CUBEW_Error_RegisterCallback( CUBEW_ErrorCallback errorCallbackIn,
                             void*              userData );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CUBEW_ERROR_CODES_H */
