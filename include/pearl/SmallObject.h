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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class pearl::SmallObject.
 *
 *  This header file provides the declaration of the class
 *  pearl::SmallObject.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_SMALLOBJECT_H
#define PEARL_SMALLOBJECT_H


#include <cstddef>


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   SmallObject
 *  @ingroup PEARL_base
 *  @brief   Provides a base class for small objects using a custom memory
 *           management.
 *
 *  Each dynamically allocated memory block usually requires some extra
 *  memory for bookkeeping purposes.  However, this can be quite space
 *  inefficient if a large number of small objects is allocated, as the
 *  per-object overhead is significant.
 *
 *  The `%SmallObject` class serves as a base class that can be used for
 *  such kinds of small objects, providing a customized memory management
 *  that allocates memory in larger chunks rather than individually for each
 *  object.  This specialized memory management is restricted to objects of
 *  not more than 64 bytes, however, if objects grow larger (e.g., objects
 *  of a derived class), the default memory allocation routines are used
 *  transparently.
 *
 *  @note
 *      Array allocations of small objects are always handled by the default
 *      allocator `operator new`.
 *
 *  @attention
 *      Small objects derived from this class have to be deleted on the
 *      same thread they were allocated on.  Otherwise the behavior is
 *      undefined.
 **/
/*-------------------------------------------------------------------------*/

class SmallObject
{
    public:
        // --- Public member functions ---------------------

        /// @name Memory allocation operators
        /// @{

        /// @brief Allocate storage space.
        ///
        /// Allocates @p objectSize bytes of storage space and returns a
        /// non-null pointer to the first byte of the block.  An optimized
        /// allocator is used for small objects less than 64 bytes, the
        /// default `operator new` otherwise.
        ///
        /// @param objectSize
        ///     Size of the requested memory block (in bytes)
        ///
        /// @exception_safety
        ///     If no (re-)allocations happen, the global state remains
        ///     unchanged in case of exceptions (strong guarantee).
        ///     Otherwise the global state is guaranteed to end in a valid
        ///     state (basic guarantee).
        ///
        /// @throws bad_alloc
        ///     on failure to allocate the object memory or any of the
        ///     required internal data structures
        ///
        static void*
        operator new(std::size_t objectSize);

        /// @brief Deallocate storage space.
        ///
        /// Deallocates the memory block of size @p objectSize pointed to by
        /// @p deadObject if it is not `nullptr`, otherwise this operator is
        /// a no-op.  The memory block pointed to by @p deadObject must have
        /// been returned by a previous call to SmallObject::operator new on
        /// the same thread, otherwise the behavior is undefined.
        ///
        /// @param deadObject
        ///     Pointer to the allocated memory
        /// @param objectSize
        ///     Size of the memory block (in bytes)
        ///
        /// @exception_nothrow
        ///
        static void
        operator delete(void*       deadObject,
                        std::size_t objectSize);

        /// @}


    protected:
        // --- Protected member functions ------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Destructor.
        ///
        /// Protected destructor destroying the instance.
        ///
        /// @exception_nothrow
        ///
        ~SmallObject();

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_SMALLOBJECT_H
