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


#ifndef PEARL_CALLSITE_H
#define PEARL_CALLSITE_H


#include <stdint.h>

#include <iosfwd>


/*-------------------------------------------------------------------------*/
/**
 *  @file    Callsite.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Callsite.
 *
 *  This header file provides the declaration of the class Callsite and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Region;
class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   Callsite
 *  @ingroup PEARL_base
 *  @brief   Representation of a callsite definition.
 *
 *  Instances of the Callsite class provide information about source-code
 *  locations where the control flow moves from one region to another. This
 *  allows to distinguish between multiple entries of a region within the
 *  same enclosing source-code region. Callsites are therefore characterized
 *  by a file and a line number where the entry happens, as well as a
 *  reference to the region being entered. The numerical identifiers of the
 * individual callsites are globally defined and continuously enumerated,
 * i.e., the ID is element of [0,@#callsites-1].
 *
 *  @todo Add "caller" attribute
 **/
/*-------------------------------------------------------------------------*/

class Callsite
{
    public:
        // --- Type definitions ----------------------------

        /// %Callsite definition identifier type.
        typedef uint32_t IdType;


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined callsite definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class Callsite representing undefined callsite
        /// entries in other definitions.
        static const Callsite UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new callsite definition instance and initializes it
        /// with the given values.
        ///
        /// @param  id        Global callsite identifier
        /// @param  filename  Name of the file in which the callsite is
        ///                   located
        /// @param  line      Line number where the call happens
        /// @param  callee    %Region being entered
        ///
        Callsite(IdType        id,
                 const String& filename,
                 uint32_t      line,
                 const Region& callee);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the callsite definition.
        ///
        /// @return Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get filename.
        ///
        /// Returns the string definition storing the name of the file in which
        /// the callsite is located. If this information is not available,
        /// String::UNDEFINED is returned.
        ///
        /// @return Filename
        ///
        const String&
        getFilename() const;

        /// @brief Get line number.
        ///
        /// Returns the line number of the callsite in the associated source
        /// file. If this information is not available, zero is returned.
        ///
        /// @return Line number
        ///
        uint32_t
        getLine() const;

        /// @brief Get called region.
        ///
        /// Returns the region definition object related to the source-code
        /// region being entered from the callsite.
        ///
        /// @return Called region
        ///
        const Region&
        getCallee() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// Name of the file in which the callsite is located
        const String& mFilename;

        /// Line number where the call happens
        uint32_t mLine;

        /// Source-code region being entered
        const Region& mCallee;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates Callsite
///
/// Prints the contents of the given callsite definition object @a item to
/// an output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    %Callsite definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&   stream,
           const Callsite& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates Callsite
///
/// Compares two callsite definition objects @a lhs and @a rhs for equality.
/// Two callsite definitions are considered equal if all of their attributes
/// are identical.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the callsite definitions are equal, false otherwise.
///
bool
operator==(const Callsite& lhs,
           const Callsite& rhs);

/// @brief   Inequality operator.
/// @relates Callsite
///
/// Compares two callsite definition objects @a lhs and @a rhs for inequality.
/// Two callsite definitions are considered unequal if any of their attributes
/// differ.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the callsite definitions differ, false otherwise.
///
bool
operator!=(const Callsite& lhs,
           const Callsite& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_CALLSITE_H
