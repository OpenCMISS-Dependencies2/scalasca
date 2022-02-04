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
 *  @brief   Declaration of the class SystemNode.
 *
 *  This header file provides the declaration of the class SystemNode and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_SYSTEMNODE_H
#define PEARL_SYSTEMNODE_H


#include <stdint.h>

#include <iosfwd>
#include <vector>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class LocationGroup;
class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   SystemNode
 *  @ingroup PEARL_base
 *  @brief   Representation of a system node definition.
 *
 *  Instances of the SystemNode class provide information about nodes in the
 *  system (i.e., hardware) hierarchy of the computing platform. That is,
 *  system nodes represent entire machines, racks, midplanes, or individual
 *  cluster nodes or node boards and can be arbitrarily nested to reflect the
 *  hardware architecture of the computing system. System nodes are
 *  characterized by a name, a free-form description ("class name"), and a
 *  parent node in the system hierarchy. In addition, one or more
 *  LocationGroup objects can be attached to a system node. The numerical
 *  identifiers of the individual nodes are globally defined and continuously
 *  enumerated, i.e., the ID is element of [0,@#system_nodes-1].
 **/
/*-------------------------------------------------------------------------*/

class SystemNode
{
    public:
        // --- Type definitions ----------------------------

        /// System node definition identifier type.
        typedef uint32_t IdType;


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined system node definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class SystemNode representing undefined system
        /// node entries in other definitions.
        static const SystemNode UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new system node definition instance and initializes it
        /// with the given values.
        ///
        /// @param  id         Global system node identifier
        /// @param  name       Node name
        /// @param  className  Free-form class name (e.g., "machine")
        /// @param  parent     Parent node in system tree (NULL for root nodes)
        ///
        SystemNode(IdType        id,
                   const String& name,
                   const String& className,
                   SystemNode*   parent);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the system node definition.
        ///
        /// @return Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get node name.
        ///
        /// Returns the string definition storing the name of this system node.
        /// This is typically the name of the machine, rack, or cluster node.
        /// If this information is not available, String::UNDEFINED is returned.
        ///
        /// @return Node name
        ///
        const String&
        getName() const;

        /// @brief Get class name.
        ///
        /// Returns the string definition storing the free-form class name of
        /// this system node. This is typically a brief description of the node
        /// type, e.g., "machine", "rack", or "node". If this information is
        /// not available, String::UNDEFINED is returned.
        ///
        /// @return Class name
        ///
        const String&
        getClassName() const;

        /// @brief Get parent node.
        ///
        /// Returns a pointer to the parent node in the system tree if
        /// applicable. Returns NULL for root nodes.
        ///
        /// @return Parent node
        ///
        SystemNode*
        getParent() const;

        /// @brief Get number of child nodes.
        ///
        /// Returns the number of child nodes in the system tree.
        ///
        /// @return Number of child nodes
        ///
        uint32_t
        numChildren() const;

        /// @brief Get child node.
        ///
        /// Returns the child node stored at the given @a index in the range
        /// of [0,@#children-1].
        ///
        /// @param  index  Child index
        ///
        /// @return Child node
        ///
        const SystemNode&
        getChild(uint32_t index) const;

        /// @brief Get number of location groups.
        ///
        /// Returns the number of associated location groups.
        ///
        /// @return  Number of location groups
        ///
        uint32_t
        numLocationGroups() const;

        /// @brief Get location group.
        ///
        /// Returns the associated location group stored at the given @a index
        /// in the range [0,@#stored_groups-1].
        ///
        /// @param  index  %Location group index
        ///
        /// @return %Location group
        ///
        const LocationGroup&
        getLocationGroup(uint32_t index) const;

        /// @}


    private:
        // --- Type definitions ----------------------------

        /// Container type for child nodes
        typedef std::vector< SystemNode* > ChildContainer;

        /// Container type for associated location groups
        typedef std::vector< LocationGroup* > LocationGroupContainer;


        // --- Data members --------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// Name of the system node
        const String& mName;

        /// Free-form class name
        const String& mClassName;

        /// Parent node in system tree
        SystemNode* mParent;

        /// Child nodes
        ChildContainer mChildren;

        /// Associated location groups
        LocationGroupContainer mLocationGroups;


        // --- Private methods & friends -------------------

        void
        addLocationGroup(LocationGroup* group);


        friend class LocationGroup;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates SystemNode
///
/// Prints the contents of the given system node definition object @a item
/// to an output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    System node definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&     stream,
           const SystemNode& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates SystemNode
///
/// Compares two system node definition objects @a lhs and @a rhs for
/// equality. Two nodes are considered equal if their identifiers, names,
/// class names, as well as their parents are identical. The child nodes are
/// not compared, i.e., this operator performs a "shallow" comparison.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the system node definitions are equal, false otherwise.
///
bool
operator==(const SystemNode& lhs,
           const SystemNode& rhs);

/// @brief   Inequality operator.
/// @relates SystemNode
///
/// Compares two system node definition objects @a lhs and @a rhs for
/// inequality. Two nodes are considered unequal if either their IDs, their
/// names, class names, or their parents differ. The child nodes are not
/// compared, i.e., this operator performs a "shallow" comparison.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the system node definitions differ, false otherwise.
///
bool
operator!=(const SystemNode& lhs,
           const SystemNode& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_SYSTEMNODE_H
