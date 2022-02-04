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

#include <pearl/Region.h>

#include <iostream>

#include <gtest/gtest.h>

#include <pearl/String.h>
#include <pearl/iomanip.h>

#include "iomanip_detail.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Test fixture for Region tests
class RegionT
    : public Test
{
    public:
        RegionT();


    protected:
        const String mCanonicalName;
        const String mDisplayName;
        const String mDescription;
        const String mFileName;
        const Region mItem;
};
}    // unnamed namespace


// --- Region tests ---------------------------------------------------------

TEST_F(RegionT,
       operatorLeftShift_undefinedAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << Region::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RegionT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RegionT,
       operatorLeftShift_undefinedAtAbbrvDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << Region::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RegionT,
       operatorLeftShift_itemAtAbbrevDetail_printsQuotedDisplayName)
{
    string expected("\"void foo()\"");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RegionT,
       operatorLeftShift_undefinedAtCompactDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << Region::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RegionT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    string expected("Region { id: 42, name: \"void foo()\", desc: \"My code\", role: function, paradigm: compiler, file: \"foo.c\", begin: 17, end: 24 }");

    ostringstream result;
    result << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RegionT,
       operatorLeftShift_undefinedAtFullDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << Region::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RegionT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    string expected("Region {\n"
                    "    id: 42\n"
                    "    name: String { id: 1, text: \"void foo()\" }\n"
                    "    desc: String { id: 2, text: \"My code\" }\n"
                    "    role: function\n"
                    "    paradigm: compiler\n"
                    "    file: String { id: 3, text: \"foo.c\" }\n"
                    "    begin: 17\n"
                    "    end: 24\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

RegionT::RegionT()
    : mCanonicalName(0, "foo"),
      mDisplayName(1, "void foo()"),
      mDescription(2, "My code"),
      mFileName(3, "foo.c"),
      mItem(42, mCanonicalName, mDisplayName, mDescription,
            Region::ROLE_FUNCTION, Paradigm::COMPILER, mFileName, 17, 24)
{
}
