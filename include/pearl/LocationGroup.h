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
 *  @brief   Declaration of the class LocationGroup.
 *
 *  This header file provides the declaration of the class LocationGroup
 *  and related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_LOCATIONGROUP_H
#define PEARL_LOCATIONGROUP_H


#include <stdint.h>

#include <iosfwd>
#include <vector>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Location;
class String;
class SystemNode;


/*-------------------------------------------------------------------------*/
/**
 *  @class   LocationGroup
 *  @ingroup PEARL_base
 *  @brief   Interface of location group definition representations.
 *
 *  The abstract base class LocationGroup defines the interface of location
 *  group representations which provide information about logical (i.e.,
 *  software-related) nodes in the system hierarchy, typically representing
 *  individual UNIX/MPI processes. %Location groups are characterized by a
 *  name, a type, and a parent node (of type SystemNode) in the system
 *  hierarchy. In addition, one or more Location objects can be attached to a
 *  location group. The numerical identifiers of the individual location
 *  groups are globally defined and continuously enumerated, i.e., the ID is
 *  element of [0,@#location_groups-1].
 **/
/*-------------------------------------------------------------------------*/

class LocationGroup
{
    public:
        // --- Type definitions ----------------------------

        /// Location group definition identifier type.
        typedef uint32_t IdType;

        /// @brief %Location group types.
        ///
        /// Every location group has a type field associated to it which
        /// encodes more specific information about the group.
        ///
        enum Type
        {
            TYPE_UNKNOWN,    ///< Unknown type, used only if no other type fits

            TYPE_PROCESS,    ///< %Location group referring to an UNIX/MPI process

            NUMBER_OF_TYPES    ///< Not a valid type, for internal use only
        };


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined loction group definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Destructor.
        ///
        /// Releases all allocated resources and destroys the instance.
        ///
        virtual
        ~LocationGroup();

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the location group definition.
        ///
        /// @return Global identifier
        ///
        IdType
        getId() const;


        /// @brief Get location group name.
        ///
        /// Returns the string definition storing the location group's name. If
        /// this information is not available, String::UNDEFINED is returned.
        ///
        /// @return %Location group name
        ///
        const String&
        getName() const;

        /// @brief Get location group type.
        ///
        /// Returns the location group type, encoding more detailed information.
        /// See the documentation of LocationGroup::Type for details.
        ///
        /// @return %Location group type
        ///
        virtual Type
        getType() const = 0;

        /// @brief Get parent node.
        ///
        /// Returns a pointer to the parent node in the system tree.
        ///
        /// @return Parent node
        ///
        SystemNode*
        getParent() const;

        /// @brief Get number of locations.
        ///
        /// Returns the number of associated locations.
        ///
        /// @return  Number of locations
        ///
        uint32_t
        numLocations() const;

        /// @brief Get location.
        ///
        /// Returns the associated location stored at the given @a index in
        /// the range [0,@#stored_locations-1].
        ///
        /// @param  index  %Location index
        ///
        /// @return %Location
        ///
        const Location&
        getLocation(uint32_t index) const;

        /// @}


    protected:
        // --- Protected methods ---------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new location group definition instance and initializes
        /// it with the given values.
        ///
        /// @param  id      Global location group identifier
        /// @param  name    Group name
        /// @param  parent  Parent node in system tree (non-NULL)
        ///
        LocationGroup(IdType        id,
                      const String& name,
                      SystemNode*   parent);

        /// @}


    private:
        // --- Type definitions ----------------------------

        /// Container type for child locations
        typedef std::vector< Location* > ChildContainer;


        // --- Data members --------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// Location group name
        const String& mName;

        /// Parent node in system tree
        SystemNode* mParent;

        /// Associated locations
        ChildContainer mLocations;


        // --- Private methods & friends -------------------

        virtual std::ostream&
        output(std::ostream& stream) const = 0;

        void
        addLocation(Location* location);


        friend class Location;

        friend std::ostream&
        operator<<(std::ostream&        stream,
                   const LocationGroup& item);
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates LocationGroup
///
/// Prints the contents of the given location group definition object @a item
/// to an output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    %Location group definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&        stream,
           const LocationGroup& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates LocationGroup
///
/// Compares two location group definition objects @a lhs and @a rhs for
/// equality. Two groups are considered equal if their identifiers, names,
/// types, and parents are identical. The child nodes are not compared,
/// i.e., this operator performs a "shallow" comparison.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the location group definitions are equal, false otherwise.
///
bool
operator==(const LocationGroup& lhs,
           const LocationGroup& rhs);

/// @brief   Inequality operator.
/// @relates LocationGroup
///
/// Compares two location group definition objects @a lhs and @a rhs for
/// inequality. Two groups are considered unequal if either their IDs, names,
/// types, or parents differ. The child nodes are not compared, i.e., this
/// operator performs a "shallow" comparison.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the location group definitions differ, false otherwise.
///
bool
operator!=(const LocationGroup& lhs,
           const LocationGroup& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_LOCATIONGROUP_H
