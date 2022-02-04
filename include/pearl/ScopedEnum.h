/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2014                                                     **
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
 *  @brief   Declaration of the ScopedEnum class template.
 *
 *  This header file provides the declaration (and inline implementation)
 *  of the ScopedEnum class template and related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_SCOPEDENUM_H
#define PEARL_SCOPEDENUM_H


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   ScopedEnum
 *  @ingroup PEARL_base
 *  @brief   Scoped enumeration class template.
 *
 *  The ScopedEnum class template can be used to define enumeration types
 *  which behave similar to C++11 scoped enums.  That is, enumerators are
 *  contained within the scope of the enumeration type and can only be
 *  accessed using the scope resolution operator.  Moreover, no implicit
 *  conversions to/from integral types are performed, which also prevents
 *  comparing values from different enumeration types.  However, since this
 *  class template uses only pre-C++11 language features, some limitations
 *  exist.  See the @b Limitations section below for details.
 *
 *  The ScopedEnum class template employs parameterized inheritance to
 *  derive the enumerators from a regular (unscoped) enum named @a type
 *  defined in a helper struct provided as the template parameter @a base.
 *  For example, to define a scoped enumeration type @c Color with the
 *  three enumerators @c RED, @c GREEN and @c BLUE, the following code
 *  snippet can be used:
 *  @code
 *      struct Color_enum
 *      {
 *          enum type
 *          {
 *              RED,
 *              GREEN,
 *              BLUE
 *          };
 *      };
 *
 *      typedef ScopedEnum< Color_enum > Color;
 *  @endcode
 *
 *  The newly defined enumeration type @c Color can then be used much like
 *  a C++11 scoped enum, except for some situations which are described in
 *  more detail below.
 *
 *  \par Limitations
 *      Scoped enums cannot be fully emulated using pre-C++11 language
 *      features.  That is, there are a few limitations that have to be
 *      considered when using enumeration types defined using this class
 *      template:
 *        - The defined type is not a C++ enum.  That is, its use may
 *          prevent certain compiler optimizations and therefore incur
 *          a performance penalty.
 *        - The emulated scoped enum can not be directly used in @c switch
 *          statements.  Instead, use the native_value() helper function:
 *          @code
 *              switch (native_value(color))
 *              {
 *                  case Color::RED:
 *                  // ...
 *              }
 *          @endcode
 *        - Likewise, direct casting to an integral type such as @c int is
 *          not possible.  Again, use the native_value() helper function
 *          instead.
 *        - The emulated scoped enum can not be directly used as a template
 *          parameter.
 *
 *  \note
 *      To convert code using emulated scoped enums based on this class
 *      template to C++11, the following actions need to be performed:
 *        - Adjust the enum type definition to use <tt>enum class</tt>
 *          instead of the helper struct and the @c typedef.
 *        - Find all uses of the native_value() helper function and either
 *          remove the function call (in @c switch statements) or replace it
 *          by a @c static_cast to the corresponding integral type (for
 *          explicit type conversions).
 **/
/*-------------------------------------------------------------------------*/

template< typename base >
class ScopedEnum
    : public base
{
    public:
        // --- Type definitions ----------------------------

        /// Enumeration type derived from @a base
        typedef typename base::type EnumType;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Default constructor.
        ///
        /// Creates an uninitialized scoped enum instance.
        ///
        ScopedEnum()
        {
        }

        /// @brief Constructor.
        ///
        /// Creates a scoped enum instance and initializes it with the given
        /// enumeration @a value.
        ///
        /// @param value
        ///     Initial enumeration value
        ///
        ScopedEnum(EnumType value)
            : mValue(value)
        {
        }

        /// @brief Converting constructor.
        ///
        /// Creates a scoped enum instance and initializes it with the given
        /// integer @a value (explicit type cast).
        ///
        /// @param value
        ///     Initial enumeration value
        ///
        explicit
        ScopedEnum(int value)
            : mValue(static_cast< EnumType >(value))
        {
        }

        /// @}


    private:
        // --- Data members --------------------------------

        /// Enumeration value
        EnumType mValue;


        // --- Private methods & friends -------------------

        /// @name Comparison operators
        /// @{

        /// @brief Less-than operator.
        ///
        /// Determines whether the enumeration value of @a lhs is less than
        /// the value of @a rhs.
        ///
        /// @param lhs
        ///     Left-hand operand
        /// @param rhs
        ///     Right-hand operand
        /// @returns
        ///     True if @a lhs is less than @a rhs, false otherwise.
        ///
        friend bool
        operator<(const ScopedEnum& lhs,
                  const ScopedEnum& rhs)
        {
            return (lhs.mValue < rhs.mValue);
        }

        /// @brief Less-than-or-equal operator.
        ///
        /// Determines whether the enumeration value of @a lhs is less than
        /// or equal to the value of @a rhs.
        ///
        /// @param lhs
        ///     Left-hand operand
        /// @param rhs
        ///     Right-hand operand
        /// @returns
        ///     True if @a lhs is less than or equal to @a rhs, false otherwise.
        ///
        friend bool
        operator<=(const ScopedEnum& lhs,
                   const ScopedEnum& rhs)
        {
            return (lhs.mValue <= rhs.mValue);
        }

        /// @brief Equal operator.
        ///
        /// Determines whether the enumeration value of @a lhs is equal to the
        /// value of @a rhs.
        ///
        /// @param lhs
        ///     Left-hand operand
        /// @param rhs
        ///     Right-hand operand
        /// @returns
        ///     True if @a lhs is equal to @a rhs, false otherwise.
        ///
        friend bool
        operator==(const ScopedEnum& lhs,
                   const ScopedEnum& rhs)
        {
            return (lhs.mValue == rhs.mValue);
        }

        /// @brief Not-equal operator.
        ///
        /// Determines whether the enumeration value of @a lhs is not equal to
        /// the value of @a rhs.
        ///
        /// @param lhs
        ///     Left-hand operand
        /// @param rhs
        ///     Right-hand operand
        /// @returns
        ///     True if @a lhs is not equal to @a rhs, false otherwise.
        ///
        friend bool
        operator!=(const ScopedEnum& lhs,
                   const ScopedEnum& rhs)
        {
            return (lhs.mValue != rhs.mValue);
        }

        /// @brief Greater-than-or-equal operator.
        ///
        /// Determines whether the enumeration value of @a lhs is greater than
        /// or equal to the value of @a rhs.
        ///
        /// @param lhs
        ///     Left-hand operand
        /// @param rhs
        ///     Right-hand operand
        /// @returns
        ///     True if @a lhs is greater than or equal to @a rhs, false
        ///     otherwise.
        ///
        friend bool
        operator>=(const ScopedEnum& lhs,
                   const ScopedEnum& rhs)
        {
            return (lhs.mValue >= rhs.mValue);
        }

        /// @brief Greater-than operator.
        ///
        /// Determines whether the enumeration value of @a lhs is greater than
        /// the value of @a rhs.
        ///
        /// @param lhs
        ///     Left-hand operand
        /// @param rhs
        ///     Right-hand operand
        /// @returns
        ///     True if @a lhs is greater than @a rhs, false otherwise.
        ///
        friend bool
        operator>(const ScopedEnum& lhs,
                  const ScopedEnum& rhs)
        {
            return (lhs.mValue > rhs.mValue);
        }

        /// @}
        /// @name Value access functions
        /// @{

        /// @brief Get enumeration value
        ///
        /// Returns the native enumeration value of the given scoped enum
        /// @a item. This helper function has to be used when using an
        /// emulated scoped enum in a @c switch statement or as a replacement
        /// for a @a static_cast to an integral type.
        ///
        /// @param item
        ///     Enumeration object
        /// @returns
        ///     Native enumeration value
        ///
        friend EnumType
        native_value(const ScopedEnum& item)
        {
            return item.mValue;
        }

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_SCOPEDENUM_H
