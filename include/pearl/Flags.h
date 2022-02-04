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
 *  @ingroup PEARL_utils
 *  @brief   Declaration and implementation of the class template Flags.
 *
 *  This header file provides the declaration and implementation of the
 *  class template Flags and related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_FLAGS_H
#define PEARL_FLAGS_H


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   Flags
 *  @ingroup PEARL_utils
 *  @brief   Class template for type-safe enum flags.
 *
 *  The Flags class template provides a type-safe way to store and manipulate
 *  combinations of flags provided via an @c enum type.  The associated flag
 *  enumeration @p EnumT should provide symbolic names for the individual
 *  flags using powers of two as values.  (While non-power-of-two values are
 *  not prohibited, they are strongly discouraged.  If used nevertheless, the
 *  enumerator name should at least properly reflect that it represents a
 *  combination of flags.)
 *
 *  The underlying storage type for the flag bit mask can be selected via the
 *  @p StorageT template parameter (<c>unsigned int</c> by default), with the
 *  restriction that it has to be large enough to hold a bit mask resulting
 *  from a binary OR of all individual flag values.
 *
 *  To define a flag type for a class @c MyClass, provide an @c enum type
 *  defining an enumerator for each flag with a value of the corresponding
 *  bit position.  Then create a convenience @c typedef for the Flags class
 *  template instantiated with the @c enum type.  To allow for binary logic
 *  operations on such flag types, the corresponding operators need to be
 *  defined as free functions (due to C++'s name resolution mechanism).  This
 *  can be accomplished using the PEARL_DEFINE_FLAGS_OPERATORS(classT,flagsT)
 *  convenience macro.
 *
 *  @par Example:
 *  @code
 *      #include <pearl/Flags.h>
 *
 *      class MyClass
 *      {
 *          public:
 *              enum PropertyEnum
 *              {
 *                  PROPERTY_NONE = 0x00,
 *                  PROPERTY_A    = 0x01,
 *                  PROPERTY_B    = 0x02,
 *                  PROPERTY_C    = 0x04
 *              };
 *
 *              typedef pearl::Flags< PropertyEnum > PropertyFlags;
 *      };
 *
 *      PEARL_DEFINE_FLAGS_OPERATORS(MyClass, PropertyFlags)
 *
 *      // ...
 *
 *      MyClass::PropertyFlags flags(MyClass::PROPERTY_C);
 *      flags.set(MyClass::PROPERTY_A | MyClass::PROPERTY_B);
 *  @endcode
 *
 *  @tparam EnumT
 *      Associated flag enumeration type
 *  @tparam StorageT
 *      Underlying storage type for the bit mask
 **/
/*-------------------------------------------------------------------------*/

template< typename EnumT,
          typename StorageT = unsigned int >
class Flags
{
    public:
        // --- Type definitions ----------------------------

        /// Base type of the associated flag enumeration.
        typedef EnumT EnumType;

        /// Base type used for storing the bit mask.
        typedef StorageT ValueType;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Default constructor.
        ///
        /// Constructs a new instance with no flags set.
        ///
        /// @exception_nothrow
        ///
        Flags();

        /// @brief Constructor.
        ///
        /// Constructs a new instance initialized with the given @p flag.
        ///
        /// @param flag
        ///     Initial flag
        ///
        /// @exception_nothrow
        ///
        Flags(EnumT flag);


        /// @}
        /// @name Manipulating operations
        /// @{

        /// @brief Binary AND.
        ///
        /// Performs a binary AND on corresponding pairs of flags with @p rhs
        /// and stores the result in this instance.
        ///
        /// @param rhs
        ///     Right-hand side operand
        /// @returns
        ///     Reference to instance
        ///
        /// @exception_nothrow
        ///
        Flags&
        operator&=(EnumT rhs);

        /// @brief Binary AND.
        ///
        /// Performs a binary AND on corresponding pairs of flags with @p rhs
        /// and stores the result in this instance.
        ///
        /// @param rhs
        ///     Right-hand side operand
        /// @returns
        ///     Reference to instance
        ///
        /// @exception_nothrow
        ///
        Flags&
        operator&=(const Flags& rhs);

        /// @brief Binary OR.
        ///
        /// Performs a binary OR on corresponding pairs of flags with @p rhs
        /// and stores the result in this instance.
        ///
        /// @param rhs
        ///     Right-hand side operand
        /// @returns
        ///     Reference to instance
        ///
        /// @exception_nothrow
        ///
        Flags&
        operator|=(EnumT rhs);

        /// @brief Binary OR.
        ///
        /// Performs a binary OR on corresponding pairs of flags with @p rhs
        /// and stores the result in this instance.
        ///
        /// @param rhs
        ///     Right-hand side operand
        /// @returns
        ///     Reference to instance
        ///
        /// @exception_nothrow
        ///
        Flags&
        operator|=(const Flags& rhs);

        /// @brief Set flag.
        ///
        /// Sets the given @p flag in the instance.
        ///
        /// @param flag
        ///     Flag to be set
        /// @returns
        ///     Reference to instance
        ///
        /// @exception_nothrow
        ///
        Flags&
        set(EnumT flag);

        /// @brief Set flags.
        ///
        /// Sets the given @p flags in the instance.
        ///
        /// @param flags
        ///     Flags to be set
        /// @returns
        ///     Reference to instance
        ///
        /// @exception_nothrow
        ///
        Flags&
        set(const Flags& flags);

        /// @brief Unset flag.
        ///
        /// Unsets the given @p flag in the instance.
        ///
        /// @param flag
        ///     Flag to be unset
        /// @returns
        ///     Reference to instance
        ///
        /// @exception_nothrow
        ///
        Flags&
        unset(EnumT flag);

        /// @brief Unset flags.
        ///
        /// Unsets the given @p flags in the instance.
        ///
        /// @param flags
        ///     Flags to be unset
        /// @returns
        ///     Reference to instance
        ///
        /// @exception_nothrow
        ///
        Flags&
        unset(const Flags& flags);

        /// @}
        /// @name Value access
        /// @{

        /// @brief Test flag.
        ///
        /// Tests whether the given @p flag is set in the instance.
        ///
        /// @param flag
        ///     Flag to be tested
        /// @returns
        ///     True if flag is set, false otherwise
        ///
        /// @exception_nothrow
        ///
        bool
        test(EnumT flag) const;

        /// @brief Test flags.
        ///
        /// Tests whether the given @p flags are set in the instance.
        ///
        /// @param flags
        ///     Flags to be tested
        /// @returns
        ///     True if flags are set, false otherwise
        ///
        /// @exception_nothrow
        ///
        bool
        test(const Flags& flags) const;

        /// @brief Get integer representation of flags.
        ///
        /// Returns the flags stored in the instance as a bit mask of type
        /// @p StorageT.
        ///
        /// @exception_nothrow
        ///
        StorageT
        getValue() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Flags represented as a bit mask of type @p StorageT.
        StorageT mValue;
};


// --- Related functions ----------------------------------------------------

/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates Flags
///
/// Tests whether the flags stored in @p lhs and @p rhs are equal.
///
/// @param lhs
///     Left-hand side operand
/// @param rhs
///     Right-hand side operand
/// @returns
///     True if the operands are equal, false otherwise.
///
/// @exception_nothrow
///
template< typename EnumT,
          typename StorageT >
bool
operator==(const Flags< EnumT, StorageT >& lhs,
           const Flags< EnumT, StorageT >& rhs);

/// @brief   Inequality operator.
/// @relates Flags
///
/// Tests whether the flags stored in @p lhs and @p rhs are unequal.
///
/// @param lhs
///     Left-hand side operand
/// @param rhs
///     Right-hand side operand
/// @returns
///     True if the operands are not equal, false otherwise.
///
/// @exception_nothrow
///
template< typename EnumT,
          typename StorageT >
bool
operator!=(const Flags< EnumT, StorageT >& lhs,
           const Flags< EnumT, StorageT >& rhs);

/// @}
}    // namespace pearl


// --- Convenience macros ---------------------------------------------------

/// @brief Convenience macro to define binary logic operators.
///
/// This preprocessor macro defines binary logic operators for a flag type
/// definition @p flagsT scoped in class @p classT, including corresponding
/// Doxygen commentary.  See the pearl::Flags class template documentation
/// for more details.
///
/// @param classT
///     Class scope of the flag type definition
/// @param flagsT
///     Name of the flag enumeration type definition within classT
///
#define PEARL_DEFINE_FLAGS_OPERATORS(classT, flagsT)                    \
    /** @name Binary logic operations                                */ \
    /** @{                                                           */ \
                                                                        \
    /** @brief   Binary AND.                                         */ \
    /** @relates classT                                              */ \
    /**                                                              */ \
    /** Returns a classT##::##flagsT object containing the result of */ \
    /** a binary AND on corresponding pairs of flags of @p rhs and   */ \
    /** @p rhs.                                                      */ \
    /**                                                              */ \
    /** @param lhs                                                   */ \
    /**     Left-hand side operand                                   */ \
    /** @param rhs                                                   */ \
    /**     Right-hand side operand                                  */ \
    /**  @returns                                                    */ \
    /**     Intersection of flags                                    */ \
    /**                                                              */ \
    /**  @exception_nothrow                                          */ \
    /**                                                              */ \
    inline classT::flagsT                                               \
    operator&(const classT::flagsT&          lhs,                       \
              const classT::flagsT::EnumType rhs)                       \
    {                                                                   \
        return (classT::flagsT(lhs) &= rhs);                            \
    }                                                                   \
                                                                        \
    /** @brief   Binary AND.                                         */ \
    /** @relates classT                                              */ \
    /**                                                              */ \
    /** Returns a classT##::##flagsT object containing the result of */ \
    /** a binary AND on corresponding pairs of flags of @p rhs and   */ \
    /** @p rhs.                                                      */ \
    /**                                                              */ \
    /** @param lhs                                                   */ \
    /**     Left-hand side operand                                   */ \
    /** @param rhs                                                   */ \
    /**     Right-hand side operand                                  */ \
    /**  @returns                                                    */ \
    /**     Intersection of flags                                    */ \
    /**                                                              */ \
    /**  @exception_nothrow                                          */ \
    /**                                                              */ \
    inline classT::flagsT                                               \
    operator&(const classT::flagsT::EnumType lhs,                       \
              const classT::flagsT&          rhs)                       \
    {                                                                   \
        return (classT::flagsT(lhs) &= rhs);                            \
    }                                                                   \
                                                                        \
    /** @brief   Binary AND.                                         */ \
    /** @relates classT                                              */ \
    /**                                                              */ \
    /** Returns a classT##::##flagsT object containing the result of */ \
    /** a binary AND on corresponding pairs of flags of @p rhs and   */ \
    /** @p rhs.                                                      */ \
    /**                                                              */ \
    /** @param lhs                                                   */ \
    /**     Left-hand side operand                                   */ \
    /** @param rhs                                                   */ \
    /**     Right-hand side operand                                  */ \
    /**  @returns                                                    */ \
    /**     Intersection of flags                                    */ \
    /**                                                              */ \
    /**  @exception_nothrow                                          */ \
    /**                                                              */ \
    inline classT::flagsT                                               \
    operator&(const classT::flagsT& lhs,                                \
              const classT::flagsT& rhs)                                \
    {                                                                   \
        return (classT::flagsT(lhs) &= rhs);                            \
    }                                                                   \
                                                                        \
    /** @brief   Binary OR.                                          */ \
    /** @relates classT                                              */ \
    /**                                                              */ \
    /** Returns a classT##::##flagsT object containing the result of */ \
    /** a binary OR on corresponding pairs of flags of @p rhs and    */ \
    /** @p rhs.                                                      */ \
    /**                                                              */ \
    /** @param lhs                                                   */ \
    /**     Left-hand side operand                                   */ \
    /** @param rhs                                                   */ \
    /**     Right-hand side operand                                  */ \
    /**  @returns                                                    */ \
    /**     Union of flags                                           */ \
    /**                                                              */ \
    /**  @exception_nothrow                                          */ \
    /**                                                              */ \
    inline classT::flagsT                                               \
    operator|(const classT::flagsT::EnumType lhs,                       \
              const classT::flagsT::EnumType rhs)                       \
    {                                                                   \
        return (classT::flagsT(lhs) |= rhs);                            \
    }                                                                   \
                                                                        \
    /** @brief   Binary OR.                                          */ \
    /** @relates classT                                              */ \
    /**                                                              */ \
    /** Returns a classT##::##flagsT object containing the result of */ \
    /** a binary OR on corresponding pairs of flags of @p rhs and    */ \
    /** @p rhs.                                                      */ \
    /**                                                              */ \
    /** @param lhs                                                   */ \
    /**     Left-hand side operand                                   */ \
    /** @param rhs                                                   */ \
    /**     Right-hand side operand                                  */ \
    /**  @returns                                                    */ \
    /**     Union of flags                                           */ \
    /**                                                              */ \
    /**  @exception_nothrow                                          */ \
    /**                                                              */ \
    inline classT::flagsT                                               \
    operator|(const classT::flagsT&          lhs,                       \
              const classT::flagsT::EnumType rhs)                       \
    {                                                                   \
        return (classT::flagsT(lhs) |= rhs);                            \
    }                                                                   \
                                                                        \
    /** @brief   Binary OR.                                          */ \
    /** @relates classT                                              */ \
    /**                                                              */ \
    /** Returns a classT##::##flagsT object containing the result of */ \
    /** a binary OR on corresponding pairs of flags of @p rhs and    */ \
    /** @p rhs.                                                      */ \
    /**                                                              */ \
    /** @param lhs                                                   */ \
    /**     Left-hand side operand                                   */ \
    /** @param rhs                                                   */ \
    /**     Right-hand side operand                                  */ \
    /**  @returns                                                    */ \
    /**     Union of flags                                           */ \
    /**                                                              */ \
    /**  @exception_nothrow                                          */ \
    /**                                                              */ \
    inline classT::flagsT                                               \
    operator|(const classT::flagsT::EnumType lhs,                       \
              const classT::flagsT&          rhs)                       \
    {                                                                   \
        return (classT::flagsT(lhs) |= rhs);                            \
    }                                                                   \
                                                                        \
    /** @brief   Binary OR.                                          */ \
    /** @relates classT                                              */ \
    /**                                                              */ \
    /** Returns a classT##::##flagsT object containing the result of */ \
    /** a binary OR on corresponding pairs of flags of @p rhs and    */ \
    /** @p rhs.                                                      */ \
    /**                                                              */ \
    /** @param lhs                                                   */ \
    /**     Left-hand side operand                                   */ \
    /** @param rhs                                                   */ \
    /**     Right-hand side operand                                  */ \
    /**  @returns                                                    */ \
    /**     Union of flags                                           */ \
    /**                                                              */ \
    /**  @exception_nothrow                                          */ \
    /**                                                              */ \
    inline classT::flagsT                                               \
    operator|(const classT::flagsT& lhs,                                \
              const classT::flagsT& rhs)                                \
    {                                                                   \
        return (classT::flagsT(lhs) |= rhs);                            \
    }                                                                   \
                                                                        \
    /** @}                                                           */


// --- Class template implementation ----------------------------------------

#include "Inline/Flags-inl.h"


#endif    // !PEARL_FLAGS_H
