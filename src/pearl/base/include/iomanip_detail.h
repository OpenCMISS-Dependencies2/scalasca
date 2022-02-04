/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of PEARL-internal stream I/O functionality.
 *
 *  This header file provides the declaration of PEARL-internal stream
 *  I/O manipulators, format flag handlers, and related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_IOMANIP_DETAIL_H
#define PEARL_IOMANIP_DETAIL_H


#include <iosfwd>


namespace pearl
{
namespace detail
{
// --- Constants ------------------------------------------------------------

/// @name Common levels of detail
/// @{

/// Maximum level of detail at which output is suppressed
const long DETAIL_SKIP = -2;

/// Level of detail at which an abbreviated output is printed
const long DETAIL_ABBRV = -1;

/// Level of detail at which a single-line output is printed
const long DETAIL_COMPACT = 0;

/// Minimum level of detail at which full multi-line output is printed
const long DETAIL_FULL = 1;

/// @}


/*-------------------------------------------------------------------------*/
/**
 *  @struct  IomSetIndent
 *  @ingroup PEARL_base
 *  @brief   Helper struct to set the indentation level for PEARL object
 *           output.
 *
 *  This helper struct is used to temporarily store the indentation level
 *  that should be used when writing PEARL objects to an output stream.
 **/
/*-------------------------------------------------------------------------*/

struct IomSetIndent
{
    /// Requested indentation level
    long mLevel;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates IomSetIndent
///
/// Stream output operator for the IomSetIndent helper struct.  Sets the
/// requested indentation level provided in @p item that should be used
/// when writing PEARL objects to @p stream.
///
/// @param stream
///     Output stream
/// @param item
///     IomSetIndent helper struct
/// @returns
///     Output stream
/// @pre
///     @p level >= 0
///
std::ostream&
operator<<(std::ostream&       stream,
           const IomSetIndent& item);

/// @}
/// @name I/O manipulators
/// @{

/// @brief Manipulator to set indentation level for PEARL object output.
///
/// Sent to a stream object, sets the requested indentation level that should
/// be used when writing PEARL objects.  Valid values for the indentation level
/// are greater than or equal to zero.
///
/// @param level
///     Requested indentation level
/// @returns
///     Object of unspecified type that when written to an output stream sets
///     the given indentation level.
/// @pre
///     @p level >= 0
///
IomSetIndent
setIndent(long level);

/// @brief Marks the begin of an object output.
///
/// Outputs a character string to @p stream that marks the begin of an object
/// content description and modifies the stream's format flags accordingly.
/// Depending on the current level of detail, this includes decreasing the
/// level of detail used for child objects and increasing the indentation
/// level.
///
/// @param stream
///     Output stream
/// @returns
///     Output stream
/// @pre
///     level of detail >= DETAIL_COMPACT
///
std::ostream&
beginObject(std::ostream& stream);

/// @brief Marks the end of an object output.
///
/// Outputs a chatacter string to @p stream that marks the end of an object
/// content description and modifies the stream's format flags accordingly.
/// Depending on the level of detail, this includes resetting the level of
/// detail and decreasing the indentation level.
///
/// @param stream
///     Output stream
/// @returns
///     Output stream
/// @pre
///     level of detail >= DETAIL_ABBRV
///
std::ostream&
endObject(std::ostream& stream);

/// @brief Marks the begin of an array output.
///
/// Outputs a character string to @p stream that marks the begin of an array
/// as part of an object's content description and modifies the stream's
/// format flags accordingly.  Depending on the level of detail, this includes
/// decreasing the level of detail used for element objects and increasing the
/// indentation level.
///
/// @param stream
///     Output stream
/// @returns
///     Output stream
/// @pre
///     level of detail >= DETAIL_ABBRV
///
std::ostream&
beginArray(std::ostream& stream);

/// @brief Marks the end of an array output.
///
/// Outputs a chatacter string to @p stream that marks the end of an array
/// as part of an object's content description and modifies the stream's
/// format flags accordingly.  Depending on the level of detail, this includes
/// resetting the level of detail and decreasing the indentation level.
///
/// @param stream
///     Output stream
/// @returns
///     Output stream
/// @pre
///     level of detail >= DETAIL_SKIP
///
std::ostream&
endArray(std::ostream& stream);

/// @brief Marks the separation between object items.
///
/// Outputs a character string to @p stream that marks the separation between
/// two items of an array or object content description.  Note that it is not
/// recommended to call this manipulator after the last item of an array or
/// object, i.e., immediately before endArray() or endObject().
///
/// @param stream
///     Output stream
/// @returns
///     Output stream
/// @pre
///     level of detail >= DETAIL_ABBRV
///
std::ostream&
sep(std::ostream& stream);

/// @}
/// @name Query stream format flags
/// @{

/// @brief Query current indentation level for PEARL object output.
///
/// Queries the output @p stream for the current indentation level set for
/// writing PEARL objects.
///
/// @param stream
///     Output stream
/// @returns
///     Current indentation level
///
long
getIndent(std::ostream& stream);

/// @brief Query current level of detail for PEARL object output.
///
/// Queries the output @p stream for the current level of detail set for
/// writing PEARL objects.
///
/// @param stream
///     Output stream
/// @returns
///     Current level of detail
///
long
getDetail(std::ostream& stream);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


// --- Inline functions -----------------------------------------------------

#include "Inline/iomanip_detail-inl.h"


#endif    // !PEARL_IOMANIP_DETAIL_H
