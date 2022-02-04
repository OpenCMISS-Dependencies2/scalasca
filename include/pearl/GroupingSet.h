/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017                                                     **
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
 *  @brief   Declaration of the class GroupingSet.
 *
 *  This header file provides the declaration of the class GroupingSet and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_GROUPINGSET_H
#define PEARL_GROUPINGSET_H


#include <stdint.h>

#include <iosfwd>
#include <string>

#include <pearl/Flags.h>
#include <pearl/Uncopyable.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   GroupingSet
 *  @ingroup PEARL_base
 *  @brief   Abstract base class of grouping set definition representations.
 *
 *  GroupingSet is an abstract base class that defines the common interface
 *  of grouping set definition representations.  Grouping sets are ordered
 *  sets and can be defined over different types of items, for example,
 *  Location, Region, or Metric definition objects, as well as numeric
 *  identifiers (ranks) defining subgroups of communication location set
 *  items.  Each of these grouping set types is implemented by a concrete
 *  derived class, due to slightly different characteristics.
 **/
/*-------------------------------------------------------------------------*/

class GroupingSet
    : private pearl::Uncopyable
{
    public:
        // --- Type definitions ----------------------------

        /// Grouping set definition identifier type.
        typedef uint32_t IdType;


        /// @brief Grouping set types.
        ///
        /// Symbolic names for different types of grouping sets (i.e., sets
        /// defined over different types of items).
        ///
        /// @see
        ///     getType()
        ///
        enum SetType
        {
            LOCATION_SET,      ///< Set of Location definition objects
            REGION_SET,        ///< Set of Region definition objects
            METRIC_SET,        ///< Set of Metric definition objects
            COMM_LOCATION_SET, ///< Set of Location definition objects which
                               ///  participate in a particular Paradigm
            COMM_SET,          ///< Set of identifiers (ranks) defining a
                               ///  subgroup of the COMM_LOCATION_SET of the
                               ///  corresponding Paradigm
        };


        /// @brief Grouping set properties.
        ///
        /// Symbolic names for flags indicating certain properties of specific
        /// grouping sets.  Property flags are only relevant for communication
        /// sets (@see CommSet).
        ///
        enum Property
        {
            /// Communication set without special properties
            PROPERTY_NONE = 0,

            /// The communication set refers to 'SELF' (i.e., always the
            /// current process)
            PROPERTY_SELF = (1 << 0),

            /// The communication set refers to 'WORLD' (i.e., all processes of
            /// a multi-process execution)
            PROPERTY_WORLD = (1 << 1),

            /// Rank identifiers in events referencing a communication set with
            /// this flag (usually indirectly via a communicator or RMA window)
            /// were converted to local ranks during reading, although the
            /// communication paradigm natively only supports global ranks.
            PROPERTY_GLOBAL_RANKS = (1 << 2)
        };


        /// Grouping set property flags type.
        typedef Flags< Property, uint8_t > Properties;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Destructor.
        ///
        /// Destroys the grouping set instance.
        ///
        /// @exception_nothrow
        ///
        virtual
        ~GroupingSet();

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the grouping set definition.
        ///
        /// @returns
        ///     Global identifier
        ///
        /// @exception_nothrow
        ///
        IdType
        getId() const;

        /// @brief Get grouping set name.
        ///
        /// Returns the string definition storing the grouping set's name.  If
        /// this information is not available, String::UNDEFINED is returned.
        ///
        /// @returns
        ///     Grouping set name
        ///
        /// @exception_nothrow
        ///
        const String&
        getName() const;

        /// @brief Get grouping set type.
        ///
        /// Pure virtual member function that has to be overwritten in derived
        /// classes to return the corresponding type of the grouping set.
        ///
        /// @returns
        ///     Set type
        ///
        /// @exception_nothrow
        ///
        virtual SetType
        getType() const = 0;

        /// @}


    protected:
        // --- Protected methods ---------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Constructs a new instance with the given global identifier @p id
        /// and grouping set @p name.
        ///
        /// @param id
        ///     Global grouping set identifier
        /// @param name
        ///     Grouping set name
        ///
        /// @exception_nothrow
        ///
        GroupingSet(IdType        id,
                    const String& name);

        /// @}


    private:
        // --- Data members --------------------------------

        /// Grouping set name
        const String& mName;

        /// Global identifier
        IdType mIdentifier;


        // --- Private methods -----------------------------

        /// @name Stream I/O functions
        /// @{

        /// @brief Stream output helper function.
        ///
        /// Pure virtual member function that has to be overwritten in derived
        /// classes to print the corresponding grouping set's attributes to the
        /// given output @p stream.
        ///
        /// @param stream
        ///     Output stream
        /// @returns
        ///     Output stream
        ///
        virtual std::ostream&
        output(std::ostream& stream) const = 0;

        /// @}


        // --- Friend declarations -------------------------

        friend std::ostream&
        operator<<(std::ostream&      stream,
                   const GroupingSet& item);
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates GroupingSet
///
/// Prints the contents of the given grouping set definition object @p item to
/// an output @p stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     Grouping set definition object
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&      stream,
           const GroupingSet& item);

/// @}
/// @name Conversion functions
/// @{

/// @brief   Convert GroupingSet::SetType to `std::string`.
/// @relates GroupingSet
///
/// Returns an `std::string` with a human-readable representation of the given
/// grouping set @p type.
///
/// @param type
///     Grouping set type
/// @returns
///     Human-readable representation of @p type as a sequence of characters
///
/// @exception_strong
///
/// @throws std::bad_alloc
///     on failure to allocate storage
///
std::string
toStdString(GroupingSet::SetType type);

/// @brief   Convert GroupingSet::Properties to `std::string`
/// @relates GroupingSet
///
/// Returns an `std::string` with a human-readable representation of the given
/// grouping set @p properties.
///
/// @param properties
///     Grouping set property flags
/// @returns
///     Human-readable representation of @p properties as a sequence of
///     characters
///
/// @exception_strong
///
/// @throws std::bad_alloc
///     on failure to allocate storage
///
std::string
toStdString(const GroupingSet::Properties& properties);

/// @}


PEARL_DEFINE_FLAGS_OPERATORS(GroupingSet, Properties)
}    // namespace pearl


#endif    // !PEARL_GROUPINGSET_H
