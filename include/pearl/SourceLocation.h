/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015                                                     **
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
 *  @brief   Declaration of the class SourceLocation.
 *
 *  This header file provides the declaration of the class SourceLocation
 *  and related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_SOURCELOCATION_H
#define PEARL_SOURCELOCATION_H


#include <stdint.h>

#include <iosfwd>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   SourceLocation
 *  @ingroup PEARL_base
 *  @brief   Representation of a source-code location definition.
 *
 *  Instances of the SourceLocation class provide information about a
 *  particular location in the source code with line granularity.  Source
 *  locations are characterized by a file name and a line number within
 *  the corresponding file.  The numerical identifiers of the individual
 *  source-code locations are globally defined and continuously enumerated,
 *  i.e., the ID is element of [0,@#sourceLocations-1].
 **/
/*-------------------------------------------------------------------------*/

class SourceLocation
{
    public:
        // --- Type definitions ----------------------------

        /// Source-code location definition identifier type.
        typedef uint32_t IdType;


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined source-code location
        /// definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class SourceLocation representing undefined
        /// source-code location entries in other definitions.
        static const SourceLocation UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new source-code location definition instance and
        /// initializes it with the given values.
        ///
        /// @param id
        ///     Global source-code location identifier
        /// @param filename
        ///     Name of source file (String::UNDEFINED if unknown)
        /// @param line
        ///     Line number within @a filename (zero if unknown)
        ///
        SourceLocation(IdType         id,
                       const String&  filename,
                       const uint32_t line);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the source-code location
        /// definition.
        ///
        /// @returns
        ///     Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get filename.
        ///
        /// Returns the string definition storing the name of the source-code
        /// file.
        ///
        /// @returns
        ///     Filename if available, String::UNDEFINED otherwise
        ///
        const String&
        getFilename() const;

        /// @brief Get line number.
        ///
        /// Returns the line number within the source file.
        ///
        /// @returns
        ///     Line number if available, zero otherwise
        ///
        uint32_t
        getLine() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Name of the source file
        const String& mFilename;

        /// Global identifier
        IdType mIdentifier;

        /// Line number within the source file
        uint32_t mLine;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates SourceLocation
///
/// Prints the contents of the given source-code location definition object
/// @a item to an output @a stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     Source-code location definition object
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&         stream,
           const SourceLocation& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates SourceLocation
///
/// Compares two source-code location definition objects @a lhs and @a rhs
/// for equality.  Two source-code location definitions are considered equal
/// if all of their attributes are identical.
///
/// @param lhs
///     Left-hand operand
/// @param rhs
///     Right-hand operand
/// @returns
///     True if the source-code location definitions are equal, false otherwise
///
bool
operator==(const SourceLocation& lhs,
           const SourceLocation& rhs);

/// @brief   Inequality operator.
/// @relates SourceLocation
///
/// Compares two source-code location definition objects @a lhs and @a rhs
/// for inequality.  Two source-code location definitions are considered
/// unequal if any of their attributes differs.
///
/// @param lhs
///     Left-hand operand
/// @param rhs
///     Right-hand operand
/// @returns
///     True if the source-code location definitions differ, false otherwise
///
bool
operator!=(const SourceLocation& lhs,
           const SourceLocation& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_SOURCELOCATION_H
