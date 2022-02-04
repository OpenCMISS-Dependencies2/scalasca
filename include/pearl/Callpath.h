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


#ifndef PEARL_CALLPATH_H
#define PEARL_CALLPATH_H


#include <stdint.h>

#include <iosfwd>
#include <string>
#include <vector>

#include <pearl/pearlfwd_detail.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    Callpath.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Callpath.
 *
 *  This header file provides the declaration of the class Callpath and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Callsite;
class Region;


/*-------------------------------------------------------------------------*/
/**
 *  @class   Callpath
 *  @ingroup PEARL_base
 *  @brief   Representation of a callpath definition.
 *
 *  Instances of the Callpath class provide information about individual
 *  callpaths of the application execution. Callpaths are characterized by
 *  the associated source-code region, the parent callpath, and optionally
 *  a callsite. The numerical identifiers of the individual callpaths are
 *  globally defined and continuously enumerated, i.e., the ID is element
 *  of [0,@#callpaths-1].
 **/
/*-------------------------------------------------------------------------*/

class Callpath
{
    public:
        // --- Type definitions ----------------------------

        /// %Callpath definition identifier type.
        typedef uint32_t IdType;


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined callpath definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class Callpath representing undefined callpath
        /// entries in other definitions.
        static const Callpath UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new callpath definition instance and initializes it
        /// with the given values.
        ///
        /// @param  id        Global callpath identifier
        /// @param  region    Associated source-code region
        /// @param  callsite  Associated callsite (if applicable,
        ///                   Callsite::UNDEFINED otherwise)
        /// @param  parent    Parent callpath (NULL for root nodes)
        ///
        Callpath(IdType          id,
                 const Region&   region,
                 const Callsite& callsite,
                 Callpath*       parent);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the callpath definition.
        ///
        /// @return Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get associated region.
        ///
        /// Returns the region definition object related to the source-code
        /// region executed in this callpath.
        ///
        /// @return Executed region
        ///
        const Region&
        getRegion() const;

        /// @brief Get associated callsite.
        ///
        /// Returns the callsite definition object related to the callsite
        /// triggering the execution of the associated region. If this
        /// information is not available, Callsite::UNDEFINED is returned.
        ///
        /// @return Callsite
        ///
        const Callsite&
        getCallsite() const;

        /// @brief Get parent callpath.
        ///
        /// Returns a pointer to the parent callpath if applicable and NULL
        /// otherwise (i.e., for call tree root nodes).
        ///
        /// @return Parent callpath
        ///
        Callpath*
        getParent() const;

        /// @brief Get number of child callpaths.
        ///
        /// Returns the number of callpaths which were called from the current
        /// callpath during the application's execution.
        ///
        /// @return Number of child callpaths
        ///
        uint32_t
        numChildren() const;

        /// @brief Get child callpath.
        ///
        /// Returns the child callpath definition stored at the given @a index
        /// in the range of [0,@#children-1].
        ///
        /// @param  index  Child index
        ///
        /// @return Child callpath
        ///
        const Callpath&
        getChild(uint32_t index) const;

        /// @}


    private:
        // --- Type definitions ----------------------------

        /// Container type for child nodes
        typedef std::vector< Callpath* > ChildContainer;


        // --- Data members --------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// Associated source-code region
        const Region& mRegion;

        /// Associated callsite
        const Callsite& mCallsite;

        /// Parent node in call tree (NULL for root nodes)
        Callpath* mParent;

        /// Child nodes
        ChildContainer mChildren;


        // --- Private methods & friends -------------------

        std::ostream&
        print(std::ostream&      stream,
              const std::string& prefix,
              bool               isLast) const;

        Callpath*
        findChild(const Region&   region,
                  const Callsite& callsite) const;


        friend class detail::CallTree;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates Callpath
///
/// Prints the contents of the given callpath definition object @a item to
/// an output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    %Callpath definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&   stream,
           const Callpath& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates Callpath
///
/// Compares two callpath definition objects @a lhs and @a rhs for equality.
/// Two callpath definitions are considered equal if their identifiers,
/// associated regions and callsites, as well as their parents are identical.
/// The child callpaths are not compared, i.e., this operator performs a
/// "shallow" comparison.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the callpath definitions are equal, false otherwise.
///
bool
operator==(const Callpath& lhs,
           const Callpath& rhs);

/// @brief   Inequality operator.
/// @relates Callpath
///
/// Compares two callpath definition objects @a lhs and @a rhs for inequality.
/// Two callpath definitions are considered unequal if either their IDs, their
/// associated regions, callsites, or their parents differ. Child callpaths
/// are not compared, i.e., this operator performs a "shallow" comparison.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the callpath definitions differ, false otherwise.
///
bool
operator!=(const Callpath& lhs,
           const Callpath& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_CALLPATH_H
