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
 *  @brief   Declaration of the class CallingContext.
 *
 *  This header file provides the declaration of the class CallingContext
 *  and related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_CALLINGCONTEXT_H
#define PEARL_CALLINGCONTEXT_H


#include <stdint.h>

#include <iosfwd>
#include <vector>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Region;
class SourceLocation;


/*-------------------------------------------------------------------------*/
/**
 *  @class   CallingContext
 *  @ingroup PEARL_base
 *  @brief   Representation of a calling context definition.
 *
 *  Instances of the CallingContext class provide information about a node
 *  in the calling context tree.  A calling context describes the sequence
 *  of call sites leading to a particular source-code location where a thread
 *  was interrupted by a CallingContextSample, CallingContextEnter, or
 *  CallingContextLeave event.  Calling contexts are characterized by the
 *  associated source-code region, a specific source-code location, and a
 *  parent calling context.  The numerical identifiers of the individual
 *  calling contexts are globally defined and continuously enumerated, i.e.,
 *  the ID is element of [0,@#callingContexts-1].
 **/
/*-------------------------------------------------------------------------*/

class CallingContext
{
    public:
        // --- Type definitions ----------------------------

        /// Calling context definition identifier type.
        typedef uint32_t IdType;


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined calling context
        /// definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class CallingContext representing undefined
        /// calling context entries.
        static const CallingContext UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new calling context definition instance and initializes
        /// it with the given values.
        ///
        /// @param id
        ///     Global calling context identifier
        /// @param region
        ///     Associated region (Region::UNDEFINED if unknown)
        /// @param scl
        ///     Source-code location (SourceLocation::UNDEFINED if unknown)
        /// @param parent
        ///     Parent calling context (`nullptr` for root nodes)
        ///
        CallingContext(IdType                id,
                       const Region&         region,
                       const SourceLocation& scl,
                       CallingContext*       parent);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the calling context definition.
        ///
        /// @returns
        ///     Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get current region.
        ///
        /// Returns the region definition associated to the calling context.
        ///
        /// @returns
        ///     %Region if available, Region::UNDEFINED otherwise
        ///
        const Region&
        getRegion() const;

        /// @brief Get source-code location.
        ///
        /// Returns the source-code location definition associated to the
        /// calling context.
        ///
        /// @returns
        ///     Source-code location if available, SourceLocation::UNDEFINED
        ///     otherwise
        ///
        const SourceLocation&
        getSourceLocation() const;

        /// @brief Get parent context.
        ///
        /// Returns a pointer to the parent calling context.
        ///
        /// @returns
        ///     Parent calling context if available, `nullptr` otherwise
        ///
        CallingContext*
        getParent() const;


        /// @brief Get number of child contexts.
        ///
        /// Returns the number of calling contexts which were spawned from the
        /// current context during the application's execution.
        ///
        /// @returns
        ///     Number of child contexts
        ///
        uint32_t
        numChildren() const;

        /// @brief Get child context.
        ///
        /// Returns the child calling context definition stored at the given
        /// @a index in the range of [0,@#children-1].
        ///
        /// @param index
        ///     Child index
        /// @returns
        ///     Child context
        ///
        const CallingContext&
        getChild(uint32_t index) const;

        /// @}


    private:
        // --- Type definitions ----------------------------

        /// Container type for child nodes
        typedef std::vector< CallingContext* > ChildContainer;


        // --- Data members --------------------------------

        /// Child nodes
        ChildContainer mChildren;

        /// Current region
        const Region& mRegion;

        /// Associated source-code location
        const SourceLocation& mSourceLocation;

        /// Parent node in the calling context tree (`nullptr` for root nodes)
        CallingContext* mParent;

        /// Global identifier
        IdType mIdentifier;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates CallingContext
///
/// Prints the contents of the given calling context definition object @a item
/// to an output @a stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     Calling context definition object
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&         stream,
           const CallingContext& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates CallingContext
///
/// Compares two calling context definition objects @a lhs and @a rhs for
/// equality.  Two calling context definitions are considered equal if their
/// identifiers, associated regions and source-code locations, as well as
/// their parents are identical.  The child contexts are not compared, i.e.,
/// this operator performs a "shallow" comparison.
///
/// @param lhs
///     Left-hand operand
/// @param rhs
///     Right-hand operand
/// @returns
///     True if the calling context definitions are equal, false otherwise
///
bool
operator==(const CallingContext& lhs,
           const CallingContext& rhs);

/// @brief   Inequality operator.
/// @relates CallingContext
///
/// Compares two calling context definition objects @a lhs and @a rhs for
/// inequality.  Two calling context definitions are considered unequal if
/// either their identifiers, associated regions, source-code locations, or
/// their parent differ.  Child contexts are not compared, i.e., this
/// operator performs a "shallow" comparison.
///
/// @param lhs
///     Left-hand operand
/// @param rhs
///     Right-hand operand
/// @returns
///     True if the calling context definitions differ, false otherwise
///
bool
operator!=(const CallingContext& lhs,
           const CallingContext& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_CALLINGCONTEXT_H
