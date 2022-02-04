/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2020                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_GENERATOR_HELPER_H
#define SCOUT_GENERATOR_HELPER_H


#include <string>


// --- Type definitions -----------------------------------------------------

/// @brief Commentary styles.
///
/// Symbolic names to specify whether C- or C++-style comments are to be
/// used.
///
enum CommentStyle
{
    C_STYLE,    ///< Use C-style comments
    CPP_STYLE   ///< Use C++-style comments
};


// --- Generic helper functions ---------------------------------------------

/// @brief Returns a string representation of a separator comment.
///
/// Returns a string representation of a separator comment in either C or C++
/// @p style using the given @p text.  The comment will fill a full commentary
/// line (77 characters) and take the provided @p indent level into account.
///
/// @param indent
///     Current indentation level
/// @param style
///     Comment style to use (C or C++)
/// @param text
///     Separator text
/// @returns
///     String representation of the separator comment
///
std::string
separatorComment(int                indent,
                 CommentStyle       style,
                 const std::string& text = "");


#endif    // !SCOUT_GENERATOR_HELPER_H
