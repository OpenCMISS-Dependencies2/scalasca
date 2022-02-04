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


#ifndef PEARL_UTILS_H
    #error "Do not include this file!  Use <pearl/Utils.h> instead."
#endif


#include <cstdio>


namespace pearl
{
// --- Conversion to string -------------------------------------------------

inline std::string
toStdString(const int value)
{
    // Conservative upper bound for length of character representation
    const unsigned int maxLength = 4 * sizeof(int);

    char      result[maxLength];
    const int length = std::snprintf(result, maxLength, "%d", value);

    return std::string(result, result + length);
}


inline std::string
toStdString(const long value)
{
    // Conservative upper bound for length of character representation
    const unsigned int maxLength = 4 * sizeof(long);

    char      result[maxLength];
    const int length = std::snprintf(result, maxLength, "%ld", value);

    return std::string(result, result + length);
}


inline std::string
toStdString(const long long value)
{
    // Conservative upper bound for length of character representation
    const unsigned int maxLength = 4 * sizeof(long long);

    char      result[maxLength];
    const int length = std::snprintf(result, maxLength, "%lld", value);

    return std::string(result, result + length);
}


inline std::string
toStdString(const unsigned int value)
{
    // Conservative upper bound for length of character representation
    const unsigned int maxLength = 4 * sizeof(unsigned int);

    char      result[maxLength];
    const int length = std::snprintf(result, maxLength, "%u", value);

    return std::string(result, result + length);
}


inline std::string
toStdString(const unsigned long value)
{
    // Conservative upper bound for length of character representation
    const unsigned int maxLength = 4 * sizeof(unsigned long);

    char      result[maxLength];
    const int length = std::snprintf(result, maxLength, "%lu", value);

    return std::string(result, result + length);
}


inline std::string
toStdString(const unsigned long long value)
{
    // Conservative upper bound for length of character representation
    const unsigned int maxLength = 4 * sizeof(unsigned long long);

    char      result[maxLength];
    const int length = std::snprintf(result, maxLength, "%llu", value);

    return std::string(result, result + length);
}


// --- String manipulation --------------------------------------------------

inline std::string
trim(const std::string& value)
{
    const char* const WHITESPACE = " \f\n\r\t\v";

    std::string::size_type startpos = value.find_first_not_of(WHITESPACE);
    if (startpos == std::string::npos)
    {
        return std::string();
    }
    std::string::size_type endpos = value.find_last_not_of(WHITESPACE);

    return std::string(value, startpos, endpos - startpos + 1);
}
}    // namespace pearl
