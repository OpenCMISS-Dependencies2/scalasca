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
 *  @brief   Declaration of the class CommSet.
 *
 *  This header file provides the declaration of the class CommSet and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_COMMSET_H
#define PEARL_COMMSET_H


#include <vector>

#include <pearl/GroupingSet.h>
#include <pearl/Paradigm.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class CommLocationSet;
class Location;


/*-------------------------------------------------------------------------*/
/**
 *  @class   CommSet
 *  @ingroup PEARL_base
 *  @brief   Grouping set definition representation for defining a subgroup
 *           of Location definition objects from a communication location set.
 *
 *  The CommSet class provides a specialized grouping set definition
 *  representation for defining a subgroup of Location definition objects from
 *  the associated communication location set.  The group is defined through
 *  an ordered set of identifiers (global ranks), which correspond to the
 *  indices of the Location definition objects in the associated communication
 *  location set.  This also implicitly assigns a local rank in the range
 *  [0,@#members-1] with respect to this communication set to each member
 *  Location.
 *
 *  CommSet instances define subgroups of locations involved in some form
 *  of communication or synchronization.  Examples of such subgroups are
 *  MPI groups or OpenMP thread teams.
 **/
/*-------------------------------------------------------------------------*/

class CommSet
    : public pearl::GroupingSet
{
    public:
        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined communication set
        /// definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class CommSet representing undefined
        /// communication set entries in other definitions.
        static const CommSet UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new instance with the given global identifier @p id,
        /// communication set @p name, @p properties, the associated
        /// communication location set @p commLocationSet, and a @p ranks
        /// vector providing a list of global ranks defining the subset.
        ///
        /// @note
        ///     @p ranks should be empty if the GroupingSet::PROPERTY_WORLD
        ///     property flag is set.
        ///
        /// @param id
        ///     Global grouping set identifier
        /// @param name
        ///     Communication set group name
        /// @param properties
        ///     Property flags
        /// @param commLocationSet
        ///     Associated communication location set
        /// @param ranks
        ///     List of ranks (i.e., indices in @p commLocationSet) defining
        ///     the subset
        ///
        /// @exception_strong
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        CommSet(IdType                         id,
                const String&                  name,
                const Properties&              properties,
                const CommLocationSet&         commLocationSet,
                const std::vector< uint32_t >& ranks);

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Get grouping set type.
        ///
        /// Returns the type of the grouping set.
        ///
        /// @returns
        ///     Set type (i.e., GroupingSet::COMM_SET)
        ///
        /// @exception_nothrow
        ///
        virtual SetType
        getType() const;

        /// @brief Get grouping set properties.
        ///
        /// Returns the property flags of this communication set.
        ///
        /// @returns
        ///     Property flags
        ///
        /// @exception_nothrow
        ///
        Properties
        getProperties() const;

        /// @brief Get communication location set.
        ///
        /// Returns a constant reference to the associated communication
        /// location set.
        ///
        /// @returns
        ///     Communciaton location set
        ///
        /// @exception_nothrow
        ///
        const CommLocationSet&
        getCommLocationSet() const;

        /// @brief Get paradigm.
        ///
        /// Returns the paradigm associated to this communication set.
        ///
        /// @returns
        ///     Communication paradigm
        ///
        /// @exception_nothrow
        ///
        Paradigm
        getParadigm() const;

        /// @brief Get number of ranks.
        ///
        /// Returns the number of ranks in this communication set.
        ///
        /// @returns
        ///     Number of ranks
        ///
        /// @exception_nothrow
        ///
        uint32_t
        numRanks() const;

        /// @brief Convert global to local rank.
        ///
        /// Returns the local rank of the subset member with the given global
        /// rank @p globalRank with respect to this communication set.
        ///
        /// @param globalRank
        ///     Global rank identifier
        /// @returns
        ///     Local rank with respect to this communciation set
        ///
        /// @exception_strong
        ///
        /// @throws RuntimeError
        ///     on failure to find @a globalRank in this communication set
        ///
        uint32_t
        getLocalRank(uint32_t globalRank) const;

        /// @brief Convert local to global rank.
        ///
        /// Returns the global rank of the subset member with the given local
        /// rank @p localRank with respect to this communication set.
        ///
        /// @param localRank
        ///     Local rank with respect to this communciation set
        /// @returns
        ///     Global rank identifier
        ///
        /// @exception_strong
        ///
        /// @throws RuntimeError
        ///     on @p localRank &notin; [0,@#num_ranks-1]
        ///
        uint32_t
        getGlobalRank(uint32_t localRank) const;

        /// @brief Get location by rank.
        ///
        /// Returns a constant reference to the Location definition object with
        /// the given @p localRank.
        ///
        /// @param localRank
        ///     Local rank with respect to this communication set.
        /// @returns
        ///     Constant reference to the corresponding Location object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on @p localRank &notin; [0,@#num_ranks-1]
        ///
        const Location&
        getLocation(uint32_t localRank) const;

        /// @}


    private:
        // --- Type definitions ----------------------------

        /// Internal container type
        typedef std::vector< uint32_t > RankContainer;


        // --- Data members --------------------------------

        /// List of global rank identifiers (i.e., indices in
        /// @ref mCommLocationSet); empty if GroupingSet::PROPERTY_WORLD
        /// flag is set.
        RankContainer mRanks;

        /// Associated communication location set
        const CommLocationSet& mCommLocationSet;

        /// Property flags (SELF, WORLD, etc.)
        Properties mProperties;


        // --- Private methods -----------------------------

        /// @name Stream I/O functions
        /// @{

        /// @brief Stream output helper function.
        ///
        /// Prints the communication set's attributes to the given output
        /// @p stream.
        ///
        /// @param stream
        ///     Output stream
        /// @returns
        ///     Output stream
        ///
        virtual std::ostream&
        output(std::ostream& stream) const;

        /// @}
};


// --- Related functions ----------------------------------------------------

/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates CommSet
///
/// Compares two communication set definition objects @p lhs and @p rhs for
/// equality.  Two communication sets are considered equal if their identifiers,
/// names, property flags, and the sizes of the rank lists are identical.
///
/// @param lhs
///     Left-hand operand
/// @param rhs
///     Right-hand operand
/// @returns
///     True if the communication set definitions are equal, false otherwise.
///
/// @exception_nothrow
///
bool
operator==(const CommSet& lhs,
           const CommSet& rhs);

/// @brief   Inequality operator.
/// @relates CommSet
///
/// Compares two communication set definition objects @p lhs and @p rhs for
/// inequality.  Two communication sets are considered unequal if either their
/// identifiers, names, property flags, or the sizes of the rank lists differ.
///
/// @param lhs
///     Left-hand operand
/// @param rhs
///     Right-hand operand
/// @returns
///     True if the communication set definitions differ, false otherwise.
///
/// @exception_nothrow
///
bool
operator!=(const CommSet& lhs,
           const CommSet& rhs);

/// @}
}    // namespace pearl


#endif    // !PEARL_COMMSET_H
