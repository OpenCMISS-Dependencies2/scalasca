/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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
 *  @brief   Declaration of helper functions related to system trees.
 *
 *  This header file provides the declaration of helper functions which are
 *  related to system trees.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_SYSTEMTREE_H
#define PEARL_SYSTEMTREE_H


#include <iosfwd>
#include <string>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Location;
class LocationGroup;
class SystemNode;


namespace detail
{
// --- Function prototypes --------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief Print location to output stream.
///
/// Prints the provided location @p item to the output @p stream in a
/// human-readable form.  The provided @p prefix is printed before each line
/// of output.
///
/// @param stream
///     Output stream
/// @param item
///     %Location to be printed
/// @param prefix
///     Prefix printed before each line of the location data
/// @returns
///     Output stream
///
std::ostream&
print(std::ostream&      stream,
      const Location&    item,
      const std::string& prefix = "");

/// @brief Print location group to output stream.
///
/// Prints the provided location group @p item to the output @p stream in a
/// human-readable form, before processing all child locations.  The provided
/// @p prefix is printed before each line of output and the @p isLastChild
/// parameter indicates whether @p item is the last child in the child list
/// of the parent system tree node.
///
/// @param stream
///     Output stream
/// @param item
///     %Location group to be printed
/// @param isLastChild
///     True if the location group is the last entry in the child list of
///     its parent system tree node, false otherwise
/// @param prefix
///     Prefix printed before each line of the location group data
/// @returns
///     Output stream
///
std::ostream&
print(std::ostream&        stream,
      const LocationGroup& item,
      bool                 isLastChild,
      const std::string&   prefix = "");

/// @brief Recursively print system tree node to output stream.
///
/// Prints the provided system tree node @p item to the output @p stream in a
/// human-readable form, before recursively processing all child system tree
/// nodes and location groups.  The provided @p prefix is printed before each
/// line of output and the @p isLastChild parameter indicates whether @p item
/// is the last child in the child list of the parent system tree node.
///
/// @param stream
///     Output stream
/// @param item
///     System tree node to be printed
/// @param isLastChild
///     True if the system tree node is the last entry in the child list of
///     its parent system tree node, false otherwise
/// @param prefix
///     Prefix printed before each line of the system tree node data
/// @returns
///     Output stream
///
std::ostream&
print(std::ostream&      stream,
      const SystemNode&  item,
      bool               isLastChild,
      const std::string& prefix = "");

/// @}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_SYSTEMTREE_H
