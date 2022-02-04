/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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
 *  @brief   Declaration of the class Communicator.
 *
 *  This header file provides the declaration of the class Communicator and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_COMMUNICATOR_H
#define PEARL_COMMUNICATOR_H


#include <stdint.h>

#include <iosfwd>

#include <pearl/Paradigm.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class CommSet;
class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   Communicator
 *  @ingroup PEARL_base
 *  @brief   Representation of a communicator definition.
 *
 *  Instances of the Communicator definition class provide information about
 *  communication contexts.  Communicators are characterized by an identifier,
 *  a name, a parent communicator, and an associated group of locations (the
 *  communication set).
 **/
/*-------------------------------------------------------------------------*/

class Communicator
{
    public:
        // --- Type definitions ----------------------------

        /// Communicator definition identifier type.
        typedef uint32_t IdType;


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined communicator definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class Communicator representing undefined
        /// communicator entries in other definitions.
        static const Communicator UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new instance with the given global identifier @p id,
        /// communicator @p name, associated communication set @p commSet,
        /// and a @p parent communicator.
        ///
        /// @param id
        ///     Global communicator identifier
        /// @param name
        ///     Communicator name
        /// @param commSet
        ///     Associated communication set
        /// @param parent
        ///     Parent communicator (`nullptr` for root communicators or when
        ///     this information is not available)
        ///
        /// @exception_nothrow
        ///
        Communicator(IdType              id,
                     const String&       name,
                     const CommSet&      commSet,
                     const Communicator* parent);

        /// @brief Destructor.
        ///
        /// Destroys the communicator instance.
        ///
        /// @exception_nothrow
        ///
        virtual
        ~Communicator();

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the communicator definition.
        ///
        /// @returns
        ///     Global identifier
        ///
        /// @exception_nothrow
        ///
        IdType
        getId() const;

        /// @brief Get communicator name.
        ///
        /// Returns a constant reference to the string definition storing the
        /// communicators's name.  If this information is not available,
        /// String::UNDEFINED is returned.
        ///
        /// @returns
        ///     Communicator name
        ///
        /// @exception_nothrow
        ///
        const String&
        getName() const;

        /// @brief Get associated communication set.
        ///
        /// Returns a constant reference to the associated communication set.
        ///
        /// @returns
        ///     Associated communication set
        ///
        /// @exception_nothrow
        ///
        const CommSet&
        getCommSet() const;

        /// @brief Get parent communicator.
        ///
        /// Returns a pointer to the parent communicator if applicable.
        /// Returns `nullptr` for root communicators or when this information
        /// is not available.
        ///
        /// @returns
        ///     Parent communicator
        ///
        /// @exception_nothrow
        ///
        const Communicator*
        getParent() const;

        /// @brief Get size.
        ///
        /// Returns the size of this communicator (i.e., the number of ranks
        /// in the associated communication set).
        ///
        /// @returns
        ///     %Communicator size
        ///
        /// @exception_nothrow
        ///
        uint32_t
        getSize() const;

        /// @brief Get paradigm.
        ///
        /// Returns the paradigm associated to this communicator.
        ///
        /// @returns
        ///     Communication paradigm
        ///
        /// @exception_nothrow
        ///
        Paradigm
        getParadigm() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// Communicator name
        const String& mName;

        /// Associated communication set
        const CommSet& mCommSet;

        /// Parent communicator
        const Communicator* mParent;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates Communicator
///
/// Prints the contents of the given communicator definition object @a item
/// to an output @a stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     Communicator definition object
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&       stream,
           const Communicator& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates Communicator
///
/// Compares two communicator definition objects @a lhs and @a rhs for
/// equality.  Two communicators are considered equal if their identifiers,
/// names, associated process groups, and parents are identical.
///
/// @param lhs
///     Left-hand operand
/// @param rhs
///     Right-hand operand
/// @returns
///     True if the communicator definitions are equal, false otherwise.
///
bool
operator==(const Communicator& lhs,
           const Communicator& rhs);

/// @brief   Inequality operator.
/// @relates Communicator
///
/// Compares two communicator definition objects @a lhs and @a rhs for
/// inequality.  Two communicators are considered unequal if either their
/// IDs, names, associated process groups, or parents differ.
///
/// @param lhs
///     Left-hand operand
/// @param rhs
///     Right-hand operand
/// @returns
///     True if the communicator definitions differ, false otherwise.
///
bool
operator!=(const Communicator& lhs,
           const Communicator& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_COMMUNICATOR_H
