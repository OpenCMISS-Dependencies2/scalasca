/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_SCOPEDPTR_H
    #error "Do not include this file!  Use <pearl/ScopedPtr.h> instead."
#endif


namespace pearl
{
// --- Constructors & destructor --------------------------------------------

template< typename BaseT >
ScopedPtr< BaseT >::ScopedPtr(BaseT* const ptr)
    : mManagedPtr(ptr)
{
}


template< typename BaseT >
ScopedPtr< BaseT >::~ScopedPtr()
{
    delete mManagedPtr;
}


// --- Value access ---------------------------------------------------------

template< typename BaseT >
BaseT*
ScopedPtr< BaseT >::get() const
{
    return mManagedPtr;
}


template< typename BaseT >
BaseT*
ScopedPtr< BaseT >::release()
{
    BaseT* tmp = mManagedPtr;

    mManagedPtr = 0;

    return tmp;
}


template< typename BaseT >
BaseT&
ScopedPtr< BaseT >::operator*() const
{
    return *mManagedPtr;
}


template< typename BaseT >
BaseT*
ScopedPtr< BaseT >::operator->() const
{
    return mManagedPtr;
}


// --- Value modification ---------------------------------------------------

template< typename BaseT >
void
ScopedPtr< BaseT >::reset(BaseT* const ptr)
{
    // See H. Sutter, "Exceptional C++" (1999), Item 13
    ScopedPtr< BaseT > tmp(ptr);
    swap(tmp);
}


template< typename BaseT >
void
ScopedPtr< BaseT >::swap(ScopedPtr& rhs)
{
    BaseT* tmp = rhs.mManagedPtr;

    rhs.mManagedPtr = mManagedPtr;
    mManagedPtr     = tmp;
}


template< typename BaseT >
ScopedPtr< BaseT >::operator BoolType() const
{
    return (mManagedPtr ? &SafeBoolStruct::dummy : 0);
}


// --- Swap algorithm overload ----------------------------------------------

template< typename BaseT >
void
swap(ScopedPtr< BaseT >& lhs,
     ScopedPtr< BaseT >& rhs)
{
    lhs.swap(rhs);
}
}    // namespace pearl
