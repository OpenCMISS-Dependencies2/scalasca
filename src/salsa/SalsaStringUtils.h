/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SALSA_SALSASTRINGUTILS_H
#define SALSA_SALSASTRINGUTILS_H


#include <sstream>
#include <string>


// object<->string conversion routines
template< typename T >
std::string
val2string(const T& i)
{
    std::ostringstream num;
    num << i;

    return num.str();
}


template< typename T >
bool
string2val(const std::string& str,
           T&                 t)
{
    std::istringstream buf(str);
    buf >> t;

    return buf.eof();
}


#endif    // !SALSA_SALSASTRINGUTILS_H
