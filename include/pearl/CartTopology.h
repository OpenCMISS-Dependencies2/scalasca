/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2019                                                     **
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
 *  @brief   Declaration of the class CartTopology.
 *
 *  This header file provides the declaration of the class CartTopology and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_CARTTOPOLOGY_H
#define PEARL_CARTTOPOLOGY_H


#include <stdint.h>

#include <iosfwd>
#include <map>
#include <vector>

#include <pearl/Uncopyable.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class CartDimension;
class Communicator;
class String;


// --- Type definitions -----------------------------------------------------

/// Data type used to store a single Cartesian grid coordinate.
typedef std::vector< uint32_t > CartCoordinate;

/// Data type storing a list of Cartesian grid coordinates.
typedef std::vector< CartCoordinate > CartCoordList;


/*-------------------------------------------------------------------------*/
/**
 *  @class   CartTopology
 *  @ingroup PEARL_base
 *  @brief   Representation of a Cartesian grid topology definition.
 *
 *  The CartTopology class provides a representation of a Cartesian grid
 *  topology definition.
 **/
/*-------------------------------------------------------------------------*/

class CartTopology
    : private pearl::Uncopyable
{
    public:
        // --- Type definitions ----------------------------

        /// %CartTopology definition identifier type.
        typedef uint32_t IdType;

        /// Container type for topology dimensions.
        typedef std::vector< const CartDimension* > DimensionContainer;

        /// Container type to store rank -@> list of coordinates mapping.
        typedef std::map< uint32_t, CartCoordList > CoordinateContainer;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new instance with the given global identifier @p id,
        /// topology @p name, associated @p communicator, and list of
        /// @p dimensions.
        ///
        /// @param id
        ///     Global Cartesian grid topology identifier
        /// @param name
        ///     Name (label) of the topology
        /// @param communicator
        ///     Associated communicator
        /// @param dimensions
        ///     List of topology dimension definition objects
        ///
        /// @exception_strong
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        CartTopology(IdType                    id,
                     const String&             name,
                     const Communicator&       communicator,
                     const DimensionContainer& dimensions);

        /// @brief Destructor.
        ///
        /// Destroys the CartTopology instance.
        ///
        /// @exception_nothrow
        ///
        virtual
        ~CartTopology();

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the Cartesian grid topology
        /// definition.
        ///
        /// @returns
        ///     Global identifier
        ///
        /// @exception_nothrow
        ///
        IdType
        getId() const;

        /// @brief Get topology name.
        ///
        /// Returns the string definition storing the topology name.  If this
        /// information is not available, String::UNDEFINED is returned.
        ///
        /// @returns
        ///     Topology name
        ///
        /// @exception_nothrow
        ///
        const String&
        getName() const;

        /// @brief Get associated communicator.
        ///
        /// Returns the communicator definition associated to the topology.
        ///
        /// @returns
        ///     Associated communicator
        ///
        /// @exception_nothrow
        ///
        const Communicator&
        getCommunicator() const;

        /// @brief Get number of dimensions.
        ///
        /// Returns the number of Cartesian grid topology dimensions.
        ///
        /// @returns
        ///     Number of dimensions
        ///
        /// @exception_nothrow
        ///
        uint8_t
        numDimensions() const;

        /// @brief Get dimension information.
        ///
        /// Returns the Cartesian grid topology dimension definition storing
        /// information of the dimension with the given @p index.
        ///
        /// @param index
        ///     Dimension index in the range [0,@#dimensions-1]
        /// @returns
        ///     Requested dimension
        ///
        /// @exception_strong
        ///
        /// @throws RuntimeError
        ///     if @p index is out of bounds
        ///
        const CartDimension&
        getDimension(uint8_t index) const;

        /// @brief Get rank coordinates.
        ///
        /// Returns a (potentially empty) list of Cartesian coordinates
        /// assigned to the given @p rank.
        ///
        /// @param rank
        ///     Local rank in associated communicator
        /// @returns
        ///     List of assigned coordinates (may be empty)
        ///
        /// @exception_strong
        ///
        /// @throws RuntimeError
        ///     on @p rank &notin; [0,communicator.getSize()-1]
        ///
        const CartCoordList
        getRankCoordinates(uint32_t rank) const;

        /// @}


    protected:
        // --- Data members --------------------------------

        /// Topology coordinate map:
        /// rank in associated communicator -@> list of coordinates
        CoordinateContainer mCoordinates;

        /// Associated communicator
        const Communicator* mCommunicator;


    private:
        // --- Data members --------------------------------

        /// Topology dimensions
        DimensionContainer mDimensions;

        /// Topology name
        const String& mName;

        /// Global identifier
        IdType mIdentifier;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates CartTopology
///
/// Prints the contents of the given Cartesian grid topology definition
/// object @p item to an output @p stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     Cartesian grid topology definition object
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&       stream,
           const CartTopology& item);

/// @}
}    // namespace pearl


#endif    // !PEARL_CARTTOPOLOGY_H
