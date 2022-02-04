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
 *  @brief   Declaration of the class LocationSet.
 *
 *  This header file provides the declaration of the class LocationSet.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_LOCATIONSET_H
#define PEARL_LOCATIONSET_H


#include <cstddef>
#include <vector>

#include <pearl/GroupingSet.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Location;


/*-------------------------------------------------------------------------*/
/**
 *  @class   LocationSet
 *  @ingroup PEARL_base
 *  @brief   Grouping set definition representation for a set of Location
 *           definition objects.
 *
 *  The LocationSet class provides a specialized grouping set definition
 *  representation for an ordered set of Location definition objects.  Note
 *  that a LocationSet does **not** take ownership of the provided Location
 *  definition objects.  Thus, care has to be taken with respect to the
 *  deletion and lifetime of the referenced Location objects.
 **/
/*-------------------------------------------------------------------------*/

class LocationSet
    : public pearl::GroupingSet
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Constructs a new instance with the given global identifier @p id,
        /// location set @p name, and a @p members vector providing an ordered
        /// list of Location definition objects defining the set.
        ///
        /// @param id
        ///     Global grouping set identifier
        /// @param name
        ///     %Location set name
        /// @param members
        ///     List of Location definition objects defining the set
        ///
        /// @exception_strong
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        LocationSet(IdType                                id,
                    const String&                         name,
                    const std::vector< const Location* >& members);

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Get grouping set type.
        ///
        /// Returns the type of the grouping set.
        ///
        /// @returns
        ///     Set type (i.e., GroupingSet::LOCATION_SET)
        ///
        /// @exception_nothrow
        ///
        virtual SetType
        getType() const;

        /// @brief Get number of locations.
        ///
        /// Returns the number of Location definitions stored in the location
        /// set instance.
        ///
        /// @returns
        ///     Number of Location definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numLocations() const;

        /// @brief Get location by rank.
        ///
        /// Returns a constant reference to the Location definition object with
        /// the given @p rank.
        ///
        /// @param rank
        ///     Local rank in the range [0,@#num_locations-1].
        /// @returns
        ///     Constant reference to the corresponding Location object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p rank is out of bounds
        ///
        const Location&
        getLocation(std::size_t rank) const;

        /// @}


    private:
        // --- Type definitions ----------------------------

        /// Internal container type
        typedef std::vector< const Location* > ContainerType;


        // --- Data members --------------------------------

        /// %Location set items
        ContainerType mLocations;


        // --- Private methods -----------------------------

        /// @name Stream I/O functions
        /// @{

        /// @brief Stream output helper function.
        ///
        /// Prints the location set's attributes to the given output @p stream.
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
}    // namespace pearl


#endif    // !PEARL_LOCATIONSET_H
