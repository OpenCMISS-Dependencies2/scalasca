/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_FLAGS_H
    #error "Do not include this file!  Use <pearl/Flags.h> instead."
#endif


namespace pearl
{
// --- Constructors & destructor --------------------------------------------

template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >::Flags()
    : mValue(0)
{
}


template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >::Flags(const EnumT flag)
    : mValue(static_cast< StorageT >(flag))
{
}


// --- Modifying operations -------------------------------------------------

template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >&
Flags< EnumT, StorageT >::operator&=(const EnumT rhs)
{
    mValue &= static_cast< StorageT >(rhs);

    return *this;
}


template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >&
Flags< EnumT, StorageT >::operator&=(const Flags& rhs)
{
    mValue &= rhs.mValue;

    return *this;
}


template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >&
Flags< EnumT, StorageT >::operator|=(const EnumT rhs)
{
    mValue |= static_cast< StorageT >(rhs);

    return *this;
}


template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >&
Flags< EnumT, StorageT >::operator|=(const Flags& rhs)
{
    mValue |= rhs.mValue;

    return *this;
}


template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >&
Flags< EnumT, StorageT >::set(const EnumT flag)
{
    return (*this |= flag);
}


template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >&
Flags< EnumT, StorageT >::set(const Flags& flags)
{
    return (*this |= flags);
}


template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >&
Flags< EnumT, StorageT >::unset(const EnumT flag)
{
    mValue &= ~static_cast< StorageT >(flag);

    return *this;
}


template< typename EnumT,
          typename StorageT >
Flags< EnumT, StorageT >&
Flags< EnumT, StorageT >::unset(const Flags& flags)
{
    mValue &= ~flags.mValue;

    return *this;
}


// --- Value access ---------------------------------------------------------

template< typename EnumT,
          typename StorageT >
bool
Flags< EnumT, StorageT >::test(const EnumT flag) const
{
    const StorageT mask = static_cast< StorageT >(flag);

    return (  ((mValue & mask) == mask)
           && ((mask != 0) || (mValue == mask)));
}


template< typename EnumT,
          typename StorageT >
bool
Flags< EnumT, StorageT >::test(const Flags& flags) const
{
    const StorageT mask = flags.mValue;

    return (  ((mValue & mask) == mask)
           && ((mask != 0) || (mValue == mask)));
}


template< typename EnumT,
          typename StorageT >
StorageT
Flags< EnumT, StorageT >::getValue() const
{
    return mValue;
}


// --- Related functions ----------------------------------------------------

template< typename EnumT,
          typename StorageT >
bool
operator==(const Flags< EnumT, StorageT >& lhs,
           const Flags< EnumT, StorageT >& rhs)
{
    return (lhs.getValue() == rhs.getValue());
}


template< typename EnumT,
          typename StorageT >
bool
operator!=(const Flags< EnumT, StorageT >& lhs,
           const Flags< EnumT, StorageT >& rhs)
{
    return (lhs.getValue() != rhs.getValue());
}
}    // namespace pearl
