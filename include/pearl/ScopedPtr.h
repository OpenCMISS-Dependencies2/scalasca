/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2017                                                **
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
 *  @brief   Declaration and implementation of the class template ScopedPtr.
 *
 *  This header file provides declaration and implementation of the class
 *  template ScopedPtr.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_SCOPEDPTR_H
#define PEARL_SCOPEDPTR_H


#include <pearl/Uncopyable.h>


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   ScopedPtr
 *  @ingroup PEARL_utils
 *  @brief   Class template providing a very basic "resource acquisition is
 *           initialization" facility.
 *
 *  The class template ScopedPtr provides a very basic and lightweight smart
 *  pointer to implement the "resource acquisition is initialization" idiom.
 *  A ScopedPtr instance manages a pointer to a dynamically allocated object
 *  of type @p BaseT.  It guarantees that the object is deleted by calling
 *  its `delete` operator when the ScopedPtr instance goes out of scope.
 *
 *  Using a ScopedPtr incurs no space and typically also no time overhead
 *  compared to using a plain pointer (assuming a decent optimizing compiler).
 *  In contrast to `std::auto_ptr` (deprecated), ScopedPtr instances cannot
 *  be copied, and thus only allow the transfer of ownership by explicitly
 *  releasing the managed pointer from the source instance.  For example:
 *  @code
 *      destination.reset(source.release());
 *  @endcode
 *  To entirely prohibit transfer of ownership, a `const ScopedPtr` can be
 *  used.
 *
 *  A ScopedPtr instance may be constructed from an incomplete type @p BaseT.
 *  @p BaseT must be complete at the point where its destructor is invoked,
 *  which happens in the destructor and the reset() member function.
 *
 *  @tparam BaseT
 *      Base type of the managed pointer, i.e., the type of the pointee.
 *      The behavior of this class is undefined if either the destructor
 *      or the `delete` operator for objects of type @p BaseT throws
 *      exceptions.
 */
/*-------------------------------------------------------------------------*/

template< typename BaseT >
class ScopedPtr
    : private pearl::Uncopyable
{
    private:
        // --- Type definitions ----------------------------

        /// Helper struct to implement the "safe bool" idiom.
        struct SafeBoolStruct
        {
            int dummy;
        };


        /// Pointer-to-int-data-member of private SafeBoolStruct.  Can only
        /// be converted to `bool` in client code.
        typedef int SafeBoolStruct::* BoolType;


    public:
        // --- Type definitions ----------------------------

        /// Base type of the managed pointer.
        typedef BaseT ElementType;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructs a scoped pointer.
        ///
        /// Constructs a new instance which takes ownership of the provided
        /// pointer @p ptr.  @p ptr must have been allocated via a C++ `new`
        /// expression or be `nullptr`.
        ///
        /// @param ptr
        ///     Pointer to be managed
        ///
        /// @exception_nothrow
        ///
        explicit
        ScopedPtr(BaseT* ptr = 0);

        /// @brief Destructor.
        ///
        /// Deletes the object pointed to by the managed pointer by calling
        /// its `delete` operator.
        ///
        /// @exception_nothrow
        ///     Depends on the requirement that the destructor and the `delete`
        ///     operator for objects of type @p BaseT do not throw exceptions.
        ///
        ~ScopedPtr();

        /// @}
        /// @name Value access
        /// @{

        /// @brief Returns the managed pointer.
        ///
        /// Returns the managed pointer without releasing ownership.
        ///
        /// @returns
        ///     Managed pointer
        ///
        /// @exception_nothrow
        ///
        BaseT*
        get() const;

        /// @brief Releases ownership of the managed pointer.
        ///
        /// Returns the managed pointer and releases ownership.  After the
        /// call, the instance holds `nullptr`.
        ///
        /// @returns
        ///     Managed pointer
        ///
        /// @exception_nothrow
        ///
        BaseT*
        release();

        /// @brief Indirection operator.
        ///
        /// Returns a reference to the object pointed to by the managed
        /// pointer.  The behavior of this operator is undefined if the
        /// managed pointer is `nullptr`.
        ///
        /// @returns
        ///     Reference to pointee
        ///
        /// @exception_nothrow
        ///
        BaseT&
        operator*() const;

        /// @brief Member-of-pointer operator.
        ///
        /// Returns the managed pointer.  The behavior of this operator is
        /// undefined if the managed pointer is `nullptr`.
        ///
        /// @returns
        ///     Managed pointer
        ///
        /// @exception_nothrow
        ///
        BaseT*
        operator->() const;

        /// @}
        /// @name Value modification
        /// @{

        /// @brief Replaces the managed pointer.
        ///
        /// Deletes the object pointed to by the current managed pointer by
        /// calling its `delete` operator, and takes ownership of the provided
        /// pointer @p ptr.  @p ptr must have been allocated via a C++ `new`
        /// expression or be `nullptr`.
        ///
        /// @param ptr
        ///     New pointer to be managed
        ///
        /// @exception_nothrow
        ///     Depends on the requirement that the destructor and the `delete`
        ///     operator for objects of type @p BaseT do not throw exceptions.
        ///
        void
        reset(BaseT* ptr = 0);

        /// @brief Swaps the managed pointers.
        ///
        /// Exchanges the managed pointer of the instance with the managed
        /// pointer of @p rhs, transferring ownership between the two ScopedPtr
        /// instances without destroying either pointee.
        ///
        /// @param rhs
        ///     Another ScopedPtr of the same type
        ///
        /// @exception_nothrow
        ///
        void
        swap(ScopedPtr& rhs);

        /// @}
        /// @name Type conversion
        /// @{

        /// @brief Checks if not empty.
        ///
        /// Conversion to a value of unspecified type which, when used in a
        /// boolean context, indicates whether the instance currently manages
        /// a pointer (i.e., if the stored pointer is not `nullptr`).
        ///
        /// @returns
        ///     Value which implicitly converts to `bool`; true if not empty,
        ///     false otherwise
        ///
        /// @exception_nothrow
        ///
        operator BoolType() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Managed pointer
        BaseT* mManagedPtr;


        // --- Private methods -----------------------------

        /// @name Comparison operators
        /// @{

        /// @brief Equal-to operator (not implemented).
        ///
        /// Unimplemented equal-to operator.  Prevents comparing for equality,
        /// since no pointer should ever be managed by two instances at the
        /// same time due to ScopedPtr's ownership semantics.
        ///
        bool
        operator==(const ScopedPtr& rhs);

        /// @brief Not-equal-to operator (not implemented).
        ///
        /// Unimplemented not-equal-to operator.  Prevents comparing for
        /// inequality, since no pointer should ever be managed by two
        /// instances at the same time due to ScopedPtr's ownership semantics.
        ///
        bool
        operator!=(const ScopedPtr& rhs);

        /// @}
};


// --- Related functions ----------------------------------------------------

/// @name Swap algorithm overload
/// @{

/// @brief   Overloads the `std::swap` algorithm for ScopedPtr instances.
/// @relates ScopedPtr
///
/// Overloads the `std::swap` algorithm for ScopedPtr instances.  Exchanges
/// the managed pointers of the two instances, transferring ownership between
/// them without destroying either pointee.
///
/// @tparam BaseT
///     Base type of the pointers managed by @p lhs and @p rhs
///
/// @param lhs
///     Left-hand side: Scoped pointer whose contents to swap
/// @param rhs
///     Right-hand side: Scoped pointer whose contents to swap
///
/// @exception_nothrow
///
template< typename BaseT >
void
swap(ScopedPtr< BaseT >& lhs,
     ScopedPtr< BaseT >& rhs);

/// @}
}    // namespace pearl


// --- Class template & related functions implementation --------------------

#include "Inline/ScopedPtr-inl.h"


#endif    // !PEARL_SCOPEDPTR_H
