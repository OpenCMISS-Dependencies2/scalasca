/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_utils
 *  @brief   Declaration and implementation of general utility functions.
 *
 *  This header file provides declaration and implementation of a number
 *  of general utility functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_UTILS_H
#define PEARL_UTILS_H


#include <string>


namespace pearl
{
// --- Function prototypes --------------------------------------------------

/// @name Conversion to string
/// @{

/// @ingroup PEARL_utils
/// @brief   Convert `int` to `std::string`.
///
/// Returns an `std::string` with the decimal-base character representation
/// of @p value.
///
/// @param value
///     Numerical `int` value
/// @returns
///     Representation of @p value as a sequence of characters
///
/// @throws std::bad_alloc
///     on failure to allocate storage
///
std::string
toStdString(int value);

/// @ingroup PEARL_utils
/// @brief   Convert `long` to `std::string`.
///
/// Returns an `std::string` with the decimal-base character representation
/// of @p value.
///
/// @param value
///     Numerical `long` value
/// @returns
///     Representation of @p value as a sequence of characters
///
/// @throws std::bad_alloc
///     on failure to allocate storage
///
std::string
toStdString(long value);

/// @ingroup PEARL_utils
/// @brief   Convert `long long` to `std::string`.
///
/// Returns an `std::string` with the decimal-base character representation
/// of @p value.
///
/// @param value
///     Numerical `long long` value
/// @returns
///     Representation of @p value as a sequence of characters
///
/// @throws std::bad_alloc
///     on failure to allocate storage
///
std::string
toStdString(long long value);

/// @ingroup PEARL_utils
/// @brief   Convert `unsigned int` to `std::string`.
///
/// Returns an `std::string` with the decimal-base character representation
/// of @p value.
///
/// @param value
///     Numerical `unsigned int` value
/// @returns
///     Representation of @p value as a sequence of characters
///
/// @throws std::bad_alloc
///     on failure to allocate storage
///
std::string
toStdString(unsigned int value);

/// @ingroup PEARL_utils
/// @brief   Convert `unsigned long` to `std::string`.
///
/// Returns an `std::string` with the decimal-base character representation
/// of @p value.
///
/// @param value
///     Numerical `unsigned long` value
/// @returns
///     Representation of @p value as a sequence of characters
///
/// @throws std::bad_alloc
///     on failure to allocate storage
///
std::string
toStdString(unsigned long value);

/// @ingroup PEARL_utils
/// @brief   Convert `unsigned long long` to `std::string`.
///
/// Returns an `std::string` with the decimal-base character representation
/// of @p value.
///
/// @param value
///     Numerical `unsigned long long` value
/// @returns
///     Representation of @p value as a sequence of characters
///
/// @throws std::bad_alloc
///     on failure to allocate storage
///
std::string
toStdString(unsigned long long value);

/// @}
/// @name String manipulation
/// @{

/// @ingroup PEARL_utils
/// @brief   Strip off leading and trailing whitespace characters.
///
/// Returns an `std::string` with all leading and trailing whitespace characters
/// stripped off from @p value.
///
/// @param value
///     Input string
/// @returns
///     %String without leading/trailing whitespaces
///
std::string
trim(const std::string& value);

/// @}
}    // namespace pearl


// --- Inline function implementation ---------------------------------------

#include "Inline/Utils-inl.h"


#endif    // !PEARL_UTILS_H
