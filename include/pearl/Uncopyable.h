/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_utils
 *  @brief   Declaration and implementation of the class Uncopyable.
 *
 *  This header file provides the declaration and implementation of the class
 *  Uncopyable.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_UNCOPYABLE_H
#define PEARL_UNCOPYABLE_H


namespace pearl
{
namespace uncopyable
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   Uncopyable
 *  @ingroup PEARL_utils
 *  @brief   Policy class to ensure that derived classes cannot be copied.
 *
 *  Uncopyable provides a base class which can be used to ensure that derived
 *  classes cannot be copied.  This is accomplished by defining both the copy
 *  constructor and assignment operator as (unimplemented) private member
 *  functions.
 *
 *  @note
 *      This class should be used via the pearl::Uncopyable typedef.  See
 *      the corresponding description for details.
 */
/*-------------------------------------------------------------------------*/

class Uncopyable
{
    protected:
        // --- Protected methods ---------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Default constructor.
        ///
        /// Protected default constructor.  Prevents the explicit creation of
        /// Uncopyable instances.
        ///
        /// @exception_nothrow
        ///
        Uncopyable();

        /// @brief Destructor.
        ///
        /// Protected destructor.  Prevents deletion of instances through
        /// pointers to Uncopyable objects.
        ///
        /// @exception_nothrow
        ///
        ~Uncopyable();

        /// @}


    private:
        // --- Private methods -----------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Copy constructor (not implemented).
        ///
        /// Unimplemented private copy constructor.  Prevents copying of
        /// derived classes.
        ///
        Uncopyable(const Uncopyable& rhs);

        /// @}
        /// @name Assignment operator
        /// @{

        /// @brief Assignment operator (not implemented).
        ///
        /// Unimplemented private assignment operator.  Prevents copying of
        /// derived classes.
        ///
        Uncopyable&
        operator=(const Uncopyable& rhs);

        /// @}
};
}    // namespace pearl::uncopyable


// --- Type definitions -----------------------------------------------------

/// @ingroup PEARL_utils
/// @brief   Alias for the pearl::uncopyable::Uncopyable policy class.
///
/// This type definition makes the pearl::uncopyable::Uncopyable policy class
/// available in the @ref pearl namespace.  This class can be used to ensure
/// that derived classes cannot be copied.  It is intended to be used via
/// private inheritance.
///
/// @par Example:
///     @code
///         #include <pearl/Uncopyable.h>
///
///         class Foo
///             : private pearl::Uncopyable
///         {
///             // ...
///         };
///     @endcode
///
typedef pearl::uncopyable::Uncopyable Uncopyable;
}    // namespace pearl


// --- Inline method implementation -----------------------------------------

#include "Inline/Uncopyable-inl.h"


#endif    // !PEARL_UNCOPYABLE_H
