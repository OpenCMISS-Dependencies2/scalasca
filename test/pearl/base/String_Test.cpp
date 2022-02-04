/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2019                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include <pearl/String.h>

#include <iostream>

#include <gtest/gtest.h>

#include <pearl/iomanip.h>

#include "iomanip_detail.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Test fixture for String tests
class StringT
    : public Test
{
    public:
        StringT();


    protected:
        const String mItem;
};
}    // unnamed namespace


// --- String tests ---------------------------------------------------------

TEST_F(StringT,
       operatorLeftShift_undefinedAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << String::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(StringT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(StringT,
       operatorLeftShift_undefinedAtAbbrvDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << String::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(StringT,
       operatorLeftShift_itemAtAbbrevDetail_printsQuotedText)
{
    string expected("\"foo\"");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(StringT,
       operatorLeftShift_undefinedAtCompactDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << String::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(StringT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    string expected("String { id: 42, text: \"foo\" }");

    ostringstream result;
    result << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(StringT,
       operatorLeftShift_undefinedAtFullDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << String::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(StringT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    string expected("String {\n"
                    "    id: 42\n"
                    "    text: \"foo\"\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

StringT::StringT()
    : mItem(42, "foo")
{
}
