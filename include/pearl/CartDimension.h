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
 *  @brief   Declaration of the class CartDimension.
 *
 *  This header file provides the declaration of the class CartDimension and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_CARTDIMENSION_H
#define PEARL_CARTDIMENSION_H


#include <stdint.h>

#include <iosfwd>

#include <pearl/Uncopyable.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   CartDimension
 *  @ingroup PEARL_base
 *  @brief   Representation of a Cartesian grid topology dimension definition.
 *
 *  The CartDimension class provides a representation of a Cartesian grid
 *  topology dimension definition.  It stores information about a single
 *  dimension of a Cartesian grid topology, such as its name (i.e., label),
 *  size (i.e., allowed coordinate range), and periodicity.
 **/
/*-------------------------------------------------------------------------*/

class CartDimension
    : private pearl::Uncopyable
{
    public:
        // --- Type definitions ----------------------------

        /// %CartDimension definition identifier type.
        typedef uint32_t IdType;


        /// @brief Dimension periodicity type.
        ///
        /// Symbolic names specifying whether the Cartesian grid topology
        /// dimension is periodic or not.
        ///
        enum Periodicity
        {
            NON_PERIODIC, ///< Dimension is not periodic
            PERIODIC,     ///< Dimension is periodic
        };


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new instance with the given global identifier @p id,
        /// dimension @p name, dimension @p size, and @p periodicity.
        ///
        /// @param id
        ///     Global Cartesian grid topology dimension identifier
        /// @param name
        ///     Name (label) of the dimension
        /// @param size
        ///     Size of the dimension
        /// @param periodicity
        ///     Defines whether the dimension is periodic or not
        ///
        /// @exception_nothrow
        ///
        CartDimension(IdType        id,
                      const String& name,
                      uint32_t      size,
                      Periodicity   periodicity);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the Cartesian grid topology
        /// dimension definition.
        ///
        /// @returns
        ///     Global identifier
        ///
        /// @exception_nothrow
        ///
        IdType
        getId() const;

        /// @brief Get dimension name.
        ///
        /// Returns the string definition storing the dimension name.  If this
        /// information is not available, String::UNDEFINED is returned.
        ///
        /// @returns
        ///     Dimension name
        ///
        /// @exception_nothrow
        ///
        const String&
        getName() const;

        /// @brief Get dimension size.
        ///
        /// Returns the size of the Cartesian grid topology dimension.
        ///
        /// @returns
        ///     Dimension size
        ///
        /// @exception_nothrow
        ///
        uint32_t
        getSize() const;

        /// @brief Get periodicity.
        ///
        /// Returns whether the Cartesian grid topology dimension is periodic
        /// or not.
        ///
        /// @returns
        ///     @c true if the dimension is periodic, @c false otherwise
        ///
        /// @exception_nothrow
        ///
        bool
        isPeriodic() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Dimension name
        const String& mName;

        /// Global identifier
        IdType mIdentifier;

        /// Dimension size
        uint32_t mSize;

        /// Dimension periodicity
        Periodicity mPeriodicity;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates CartDimension
///
/// Prints the contents of the given Cartesian grid topology dimension
/// definition object @p item to an output @p stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     Cartesian grid topology dimension definition object
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&        stream,
           const CartDimension& item);

/// @}
}    // namespace pearl


#endif    // !PEARL_CARTDIMENSION_H
