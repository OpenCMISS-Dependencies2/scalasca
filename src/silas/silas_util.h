/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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


#ifndef SILAS_SILAS_UTIL_H
#define SILAS_SILAS_UTIL_H


#include <algorithm>
#include <string>


void
log(const char* fmt,
    ...);


namespace silas
{
/**
 * @brief helper function to convert a string to all lowercase * letters
 * @param str input string
 * @return converted string
 */
inline std::string
lowercase(const std::string& str)
{
    std::string result(str);
    std::transform(str.begin(), str.end(), result.begin(), (int (*)(int))tolower);

    return result;
}
}    // namespace silas


#endif    // !SILAS_SILAS_UTIL_H
