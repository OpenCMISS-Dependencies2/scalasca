/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2019                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_IOMANIP_H
#define PEARL_IOMANIP_H


#include <iosfwd>


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of PEARL-specific stream I/O functionality.
 *
 *  This header file provides the declaration of PEARL-specific stream
 *  I/O manipulators.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @struct  IomSetDetail
 *  @ingroup PEARL_base
 *  @brief   Helper struct to set the level of detail for PEARL object output.
 *
 *  This helper struct is used to temporarily store the level of detail that
 *  should be used when writing PEARL objects to an output stream.
 **/
/*-------------------------------------------------------------------------*/

struct IomSetDetail
{
    /// Requested level of detail
    long mLevel;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates IomSetDetail
///
/// Stream output operator for the IomSetDetail helper struct.  Sets the
/// requested level of detail provided in @p item that should be used when
/// writing PEARL objects to @p stream.
///
/// @param stream
///     Output stream
/// @param item
///     IomSetDetail helper struct
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&       stream,
           const IomSetDetail& item);

/// @}
/// @name I/O manipulators
/// @{

/// @brief Manipulator to set level of detail for PEARL object output.
///
/// Sent to a stream object, sets the requested @p level of detail that should
/// be used when writing PEARL objects.  Valid values for the level of detail
/// are greater than or equal to zero, negative values are reserved for
/// internal use.
///
/// @param level
///     Requested level of detail
/// @returns
///     Object of unspecified type that when written to an output stream sets
///     the given level of detail.
///
IomSetDetail
setDetail(long level);

/// @}
}    // namespace pearl


// --- Inline functions -----------------------------------------------------

#include "Inline/iomanip-inl.h"


#endif    // !PEARL_IOMANIP_H
