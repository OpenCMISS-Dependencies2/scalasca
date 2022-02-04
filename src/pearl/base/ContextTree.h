/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2017                                                **
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
 *  @brief   Declaration of helper functions related to calling context
 *           trees.
 *
 *  This header file provides the declaration of helper functions which are
 *  related to calling context trees.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_CONTEXTTREE_H
#define PEARL_CONTEXTTREE_H


#include <iosfwd>
#include <string>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class CallingContext;


namespace detail
{
// --- Function prototypes --------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief Recursively print calling context to output stream.
///
/// Prints the provided calling context @p item to the output @p stream in a
/// human-readable form, before recursively processing all child contexts.
/// The provided @p prefix is printed before each line of output and the
/// @p isLastChild parameter indicates whether @p item is the last child in
/// the child list of the parent context.
///
/// @param stream
///     Output stream
/// @param item
///     Calling context to be printed
/// @param isLastChild
///     True if the calling context is the last entry in the child list of
///     its parent, false otherwise
/// @param prefix
///     Prefix printed before each line of the calling context data
/// @returns
///     Output stream
///
std::ostream&
print(std::ostream&         stream,
      const CallingContext& item,
      bool                  isLastChild,
      const std::string&    prefix = "");

/// @}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_CONTEXTTREE_H
