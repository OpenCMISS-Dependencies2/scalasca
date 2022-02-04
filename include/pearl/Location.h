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
 *  @brief   Declaration of the class Location.
 *
 *  This header file provides the declaration of the class Location and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_LOCATION_H
#define PEARL_LOCATION_H


#include <stdint.h>

#include <iosfwd>

#include <pearl/SystemNode.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class LocationGroup;
class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   Location
 *  @ingroup PEARL_base
 *  @brief   Representation of a location definition.
 *
 *  Instances of the Location class provide information about the leaf nodes
 *  in the system hierarchy for which actual event trace data exists. A
 *  location therefore either represents a stream of execution on either a
 *  CPU or GPU, or a stream of metric values related to the system tree node
 *  to which the parent location group is attached. Locations are
 *  characterized by a name, a type, the number of stored events, and the
 *  parent location group. The numerical identifiers of the individual
 *  locations are globally defined, but not necessarily continuously
 *  enumerated.
 **/
/*-------------------------------------------------------------------------*/

class Location
{
    public:
        // --- Type definitions ----------------------------

        /// %Location definition identifier type.
        typedef uint64_t IdType;

        /// @brief %Location types.
        ///
        /// Every location has a type field associated to it which encodes
        /// more specific information about the location.
        ///
        enum Type
        {
            TYPE_UNKNOWN,    ///< Unknown type, used only if no other type fits

            TYPE_CPU_THREAD, ///< %Location corresponds to a CPU thread
            TYPE_GPU,        ///< %Location corresponds to a GPU thread
            TYPE_METRIC,     ///< %Location corresponds to non-thread level metric data

            NUMBER_OF_TYPES    ///< Not a valid type, for internal use only
        };


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined location definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class Location representing undefined location
        /// entries in other definitions.
        static const Location UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new location definition instance and initializes it
        /// with the given values.
        ///
        /// @param  id         Global location identifier
        /// @param  name       %Location name
        /// @param  type       %Location type
        /// @param  numEvents  Number of events stored
        /// @param  parent     Parent location group (non-NULL)
        ///
        Location(IdType         id,
                 const String&  name,
                 Type           type,
                 uint64_t       numEvents,
                 LocationGroup* parent);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the location definition.
        ///
        /// @return Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get location name.
        ///
        /// Returns the string definition storing the location's name. If this
        /// information is not available, String::UNDEFINED is returned.
        ///
        /// @return %Location name
        ///
        const String&
        getName() const;

        /// @brief Get location type.
        ///
        /// Returns the location type, encoding more detailed information. See
        /// the documentation of Location::Type for details.
        ///
        /// @return %Location type
        ///
        Type
        getType() const;

        /// @brief Get number of events.
        ///
        /// Returns the number of event records stored for this location.
        ///
        /// @return Number of events
        ///
        uint64_t
        getNumEvents() const;

        /// @brief Get parent node.
        ///
        /// Returns a pointer to the parent location group.
        ///
        /// @return Parent group
        ///
        LocationGroup*
        getParent() const;

        /// @}
        /// @name Access location properties
        /// @{

        /// @brief Get machine identifier.
        ///
        /// Returns the global system node identifier of the machine this
        /// location belongs to.
        ///
        /// @return Machine identifier
        ///
        SystemNode::IdType
        getMachineId() const;

        /// @brief Get node identifier.
        ///
        /// Returns the global system node identifier of the machine node
        /// this location belongs to.
        ///
        /// @return Node identifier
        ///
        SystemNode::IdType
        getNodeId() const;

        /// @brief Get process rank.
        ///
        /// Returns the MPI rank of the process. If this information is not
        /// available, -1 is returned.
        ///
        /// @return MPI rank
        ///
        int
        getRank() const;

        /// @brief Get thread identifier.
        ///
        /// Returns the unique thread identifier of this location within the
        /// corresponding process.
        ///
        /// @return Thread identifier
        ///
        uint32_t
        getThreadId() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// %Location name
        const String& mName;

        /// %Location type
        Type mType;

        /// Number of events stored for the location
        uint64_t mNumEvents;

        /// Parent location group in system tree
        LocationGroup* mParent;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates Location
///
/// Prints the contents of the given location definition object @a item to
/// an output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    %Location definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&   stream,
           const Location& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates Location
///
/// Compares two location definition objects @a lhs and @a rhs for equality.
/// Two location definitions are considered equal if their global identifiers,
/// names, types, and parents are identical. The number of events is not
/// considered.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the location definitions are equal, false otherwise.
///
bool
operator==(const Location& lhs,
           const Location& rhs);

/// @brief   Inequality operator.
/// @relates Location
///
/// Compares two location definition objects @a lhs and @a rhs for inequality.
/// Two location definitions are considered unequal if either their global
/// identifiers, names, types, or parents differ. The number of events is not
/// considered.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the location definitions differ, false otherwise.
///
bool
operator!=(const Location& lhs,
           const Location& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_LOCATION_H
