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


#ifndef PEARL_UNCOPYABLE_H
    #error "Do not include this file!  Use <pearl/Uncopyable.h> instead."
#endif


namespace pearl
{
namespace uncopyable
{
// --- Constructors & destructor --------------------------------------------

inline
Uncopyable::Uncopyable()
{
}


inline
Uncopyable::~Uncopyable()
{
}
}    // namespace pearl::uncopyable
}    // namespace pearl
