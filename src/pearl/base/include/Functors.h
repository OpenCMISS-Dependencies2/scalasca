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
 *  @brief   Declaration of various functor classes.
 *
 *  This header file provides the declaration of various functor classes.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_FUNCTORS_H
#define PEARL_FUNCTORS_H


#include <functional>


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @struct  delete_ptr
 *  @ingroup PEARL_base
 *  @brief   Functor class deleting its pointer argument.
 *
 *  The delete_ptr<> functor class calls the delete operator on the pointer
 *  argument passed to the function call operator.  It can be used in
 *  combination with the std::for_each algorithm to delete items of pointer
 *  type stored in an STL container.
 *
 *  @par Example
 *  @code
 *      #include <algorithm>
 *      #include <vector>
 *
 *      #include "Functors.h"
 *
 *      using namespace std;
 *      using namespace pearl::detail;
 *
 *      int main()
 *      {
 *          vector< int* > data;
 *
 *          for (int i = 0; i < 10; ++i)
 *          {
 *              data.push_back(new int(i));
 *          }
 *
 *          for_each(data.begin(), data.end(), delete_ptr< int >());
 *
 *          return 0;
 *      }
 *  @endcode
 *
 *  @tparam ptrT
 *      @parblock
 *      Base type of the pointer to be deleted.
 *
 *      The behavior of this functor is undefined if the destructor or the
 *      `delete` operator for objects of type @p ptrT throw exceptions.
 *      @endparblock
 **/
/*-------------------------------------------------------------------------*/

template< typename ptrT >
struct delete_ptr
    : public std::unary_function< ptrT*, void >
{
    // --- Public methods -------------------------------

    /// @brief Function call operator.
    ///
    /// Deletes the operand @a item by calling the delete operator.
    ///
    /// @param item
    ///     Pointer to be deleted
    ///
    /// @exception_nothrow
    ///     Depends on the requirement that the destructor and the `delete`
    ///     operator for objects of type @p ptrT do not throw exceptions.
    ///
    void
    operator()(ptrT* item)
    {
        delete item;
    }

    /// @brief Function call operator (`const` pointer version).
    ///
    /// Deletes the operand @a item by calling the delete operator.
    ///
    /// @param item
    ///     Pointer to be deleted
    ///
    /// @exception_nothrow
    ///     Depends on the requirement that the destructor and the `delete`
    ///     operator for objects of type @p ptrT do not throw exceptions.
    ///
    void
    operator()(const ptrT* item)
    {
        delete item;
    }
};


/*-------------------------------------------------------------------------*/
/**
 *  @struct  less_ptr
 *  @ingroup PEARL_base
 *  @brief   Dereferencing comparison functor class for operator<().
 *
 *  The less_ptr<> functor class is a comparison functor similar to the
 *  predefined STL function object std::less<>, but instead of comparing
 *  the two operands directly using operator <(), the arguments are
 *  dereferenced beforehand.  This is useful for applying STL algorithms
 *  to containers of pointer types.
 *
 *  @par Example
 *  @code
 *      #include <algorithm>
 *      #include <cstdlib>
 *      #include <iostream>
 *      #include <vector>
 *
 *      #include "Functors.h"
 *
 *      using namespace std;
 *      using namespace pearl::detail;
 *
 *      int main()
 *      {
 *          vector< int* > data;
 *
 *          for (int i = 0; i < 10; ++i)
 *          {
 *              data.push_back(new int(rand()));
 *          }
 *
 *          sort(data.begin(), data.end(), less_ptr< int >());
 *
 *          for (int i = 0; i < 10; ++i)
 *          {
 *              cout << *data[i] << endl;
 *              delete data[i];
 *          }
 *
 *          return 0;
 *      }
 *  @endcode
 *
 *  @tparam ptrT
 *      Base type of the pointers to compare.
 **/
/*-------------------------------------------------------------------------*/

template< typename ptrT >
struct less_ptr
    : public std::binary_function< ptrT*, ptrT*, bool >
{
    // --- Public methods -------------------------------

    /// @brief Function call operator.
    ///
    /// Dereferences the operands and compares the resulting values using
    /// the "less than" operator.
    ///
    /// @param lhs
    ///     Left-hand operand
    /// @param rhs
    ///     Right-hand operand
    /// @returns
    ///     True if *@a lhs is less than *@a rhs, false otherwise
    ///
    bool
    operator()(ptrT* lhs,
               ptrT* rhs) const
    {
        return (*lhs < *rhs);
    }
};
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_FUNCTORS_H
