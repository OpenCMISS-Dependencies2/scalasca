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
 *  @brief   Declaration of the class MutableCartTopology.
 *
 *  This header file provides the declaration of the class MutableCartTopology.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_MUTABLECARTTOPOLOGY_H
#define PEARL_MUTABLECARTTOPOLOGY_H


#include <pearl/CartTopology.h>


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MutableCartTopology
 *  @ingroup PEARL_base
 *  @brief   Mutable Cartesian grid topology definition.
 *
 *  The MutableCartTopology class provides the PEARL-internal representation
 *  of a Cartesian grid topology definition, allowing the modification or
 *  addition of certain attributes, respectively.
 **/
/*-------------------------------------------------------------------------*/

class MutableCartTopology
    : public CartTopology
{
    public:
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
        MutableCartTopology(IdType                    id,
                            const String&             name,
                            const Communicator&       communicator,
                            const DimensionContainer& dimensions);

        /// @}
        /// @name Set definition data
        /// @{

        /// @brief Set associated communicator.
        ///
        /// Associates a new @p communicator definition to the topology.
        ///
        /// @param communicator
        ///     New associated communicator
        ///
        /// @exception_nothrow
        ///
        /// @todo
        ///     Remove this method once EPIK support has been dropped.  Also,
        ///     mark mCommunicator as `private` data of CartTopology.
        ///
        void
        setCommunicator(const Communicator& communicator);

        /// @brief Add a rank coordinate.
        ///
        /// Adds the given @p coordinate to the list of Cartesian coordinates
        /// for a particular @p rank.
        ///
        /// @param rank
        ///     Local rank in associated communicator
        /// @param coordinate
        ///     Cartesian coordinate to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws RuntimeError
        ///     on communicator &ne; Communicator::UNDEFINED and
        ///         @p rank &notin; [0,communicator.getSize()-1]<br/>
        ///     on invalid @#dimensions<br/>
        ///     on coordinate value out of range
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        /// @todo
        ///     Always perform rank check once EPIK support has been dropped.
        ///
        void
        addRankCoordinate(uint32_t              rank,
                          const CartCoordinate& coordinate);

        /// @}
};
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_MUTABLECARTTOPOLOGY_H
