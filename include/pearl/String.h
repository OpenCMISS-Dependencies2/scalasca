/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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


#ifndef PEARL_STRING_H
#define PEARL_STRING_H


#include <stdint.h>

#include <iosfwd>
#include <string>


/*-------------------------------------------------------------------------*/
/**
 *  @file    String.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class String.
 *
 *  This header file provides the declaration of the class String and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   String
 *  @ingroup PEARL_base
 *  @brief   Representation of a string definition.
 *
 *  Instances of the String class provide information about global string
 *  definitions. They are referenced by various other definitions, such as
 *  Region or Location, and associate a character string with a numerical
 *  identifier. String definitions are globally defined and continously
 *  enumerated, i.e., the ID is element of [0,@#strings-1].
 *
 *  @todo Find a way to get rid of MACHINE_ID, MACHINE, NODE_ID, and NODE.
 **/
/*-------------------------------------------------------------------------*/

class String
{
    public:
        // --- Type definitions ----------------------------

        /// %String definition identifier type.
        typedef uint32_t IdType;


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined string definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class String representing undefined string
        /// entries in other definitions.
        static const String UNDEFINED;

        /// Numerical identifier used for the String::MACHINE instance.
        static const IdType MACHINE_ID = NO_ID - 1;

        /// Static instance of class String providing the character string
        /// "machine" for the EPIK trace reader.
        static const String MACHINE;

        /// Numerical identifier used for the String::NODE instance.
        static const IdType NODE_ID = NO_ID - 2;

        /// Static instance of class String providing the character string
        /// "node" for the EPIK trace reader.
        static const String NODE;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new string definition instance using the given global
        /// identifier @a id and the character string @a str.
        ///
        /// @param  id   Global string identifier
        /// @param  str  Associated character string
        ///
        String(IdType             id,
               const std::string& str);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the string definition.
        ///
        /// @return Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get string data as C++ std::string.
        ///
        /// Returns the character string associated to the string definition
        /// as a C++ std::string object.
        ///
        /// @return C++ std::string object
        ///
        const std::string
        getString() const;

        /// @brief Get string data as C string.
        ///
        /// Returns the character string associated to the string definition
        /// as a null-terminated C string. For the returned pointer, the same
        /// restrictions apply as for the c_str() member function of the
        /// std::string class. That is, the returned array points to an
        /// internal location large enough to hold the character string plus
        /// its terminating null-character, but the values in this array
        /// should not be modified in the program.
        ///
        /// @return Null-terminated C string
        ///
        const char*
        getCString() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// Associated character string
        std::string mString;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates String
///
/// Prints the contents of the given string definition object @a item to an
/// output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    %String definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream& stream,
           const String& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates String
///
/// Compares two string definition objects @a lhs and @a rhs for equality. Two
/// string definitions are considered equal if both their global identifiers
/// and their associated characters strings are identical.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the string definitions are equal, false otherwise.
///
bool
operator==(const String& lhs,
           const String& rhs);

/// @brief   Inequality operator.
/// @relates String
///
/// Compares two string definition objects @a lhs and @a rhs for inequality.
/// Two string definitions are considered unequal if either their global
/// identifiers or their associated characters strings differ.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the string definitions differ, false otherwise.
///
bool
operator!=(const String& lhs,
           const String& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_STRING_H
