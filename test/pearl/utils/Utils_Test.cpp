/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include <pearl/Utils.h>

#include <limits>
#include <sstream>

#include <gtest/gtest.h>

using namespace std;
using namespace testing;
using namespace pearl;


// --- Helpers --------------------------------------------------------------

namespace
{
// Dummy test fixture needed for 'toStdString' typed tests
template< typename T >
class toStdStringT
    : public Test
{
};


// Instantiate test cases for types supported by 'toStdString'
typedef Types< int,
               long,
               long long,
               unsigned int,
               unsigned long,
               unsigned long long > toStdStringTypes;
TYPED_TEST_CASE(toStdStringT, toStdStringTypes);
}    // unnamed namespace


// --- toStdString tests ----------------------------------------------------

TYPED_TEST(toStdStringT,
           toStdString_min_returnsStringRep)
{
    const TypeParam value = numeric_limits< TypeParam >::min();

    ostringstream expected;
    expected << value;
    EXPECT_EQ(toStdString(value), expected.str());
}


TYPED_TEST(toStdStringT,
           toStdString_max_returnsStringRep)
{
    const TypeParam value = numeric_limits< TypeParam >::max();

    ostringstream expected;
    expected << value;
    EXPECT_EQ(toStdString(value), expected.str());
}


TYPED_TEST(toStdStringT,
           toStdString_value_returnsStringRep)
{
    const TypeParam value = 42;

    EXPECT_EQ(toStdString(value), string("42"));
}


// --- trim tests -----------------------------------------------------------

TEST(trimT,
     trim_emptyString_returnsEmptyString)
{
    string expected;

    EXPECT_EQ(trim(string()), expected);
}


TEST(trimT,
     trim_whitespaceOnly_returnsEmptyString)
{
    string expected;

    EXPECT_EQ(trim(string(" \f\n\r\t\v")), expected);
}


TEST(trimT,
     trim_leadingWhitespace_returnsTrailingText)
{
    string expected("Some Text.");

    EXPECT_EQ(trim(string(" \v\t\f Some Text.")), expected);
}


TEST(trimT,
     trim_trailingWhitespace_returnsLeadingText)
{
    string expected("Some Text.");

    EXPECT_EQ(trim(string("Some Text.\n \r ")), expected);
}


TEST(trimT,
     trim_leadingAndTrailingWhitespace_returnsText)
{
    string expected("Some Text.");

    EXPECT_EQ(trim(string(" \v\t\f Some Text.\n \r ")), expected);
}
