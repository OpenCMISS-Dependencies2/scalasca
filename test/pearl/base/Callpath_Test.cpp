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

#include <pearl/Callpath.h>

#include <iostream>

#include <gtest/gtest.h>

#include <pearl/Callsite.h>
#include <pearl/Region.h>
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
// Test fixture for String tests
class CallpathT
    : public Test
{
    public:
        CallpathT();


    protected:
        const String   mRegionCanonicalName;
        const String   mRegionDisplayName;
        const String   mRegionDescription;
        const String   mRegionFileName;
        const Region   mRegion;
        const Callpath mItem;
};
}    // unnamed namespace


// --- Callpath tests -------------------------------------------------------

TEST_F(CallpathT,
       operatorLeftShift_undefinedAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << Callpath::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallpathT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallpathT,
       operatorLeftShift_undefinedAtAbbrvDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << Callpath::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallpathT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    string expected("42");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallpathT,
       operatorLeftShift_undefinedAtCompactDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << Callpath::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallpathT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    string expected("Callpath { id: 42, region: \"void foo()\", callsite: <undef>, parent: <undef> }");

    ostringstream result;
    result << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallpathT,
       operatorLeftShift_undefinedAtFullDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << Callpath::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallpathT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    string expected("Callpath {\n"
                    "    id: 42\n"
                    "    region: Region { id: 24, name: \"void foo()\", desc: \"My code\", role: function, paradigm: compiler, file: \"foo.c\", begin: 17, end: 24 }\n"
                    "    callsite: <undef>\n"
                    "    parent: <undef>\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

/// @todo[C++] Replace NULL with nullptr
CallpathT::CallpathT()
    : mRegionCanonicalName(0, "foo"),
      mRegionDisplayName(1, "void foo()"),
      mRegionDescription(2, "My code"),
      mRegionFileName(3, "foo.c"),
      mRegion(24, mRegionCanonicalName, mRegionDisplayName, mRegionDescription,
              Region::ROLE_FUNCTION, Paradigm::COMPILER, mRegionFileName,
              17, 24),
      mItem(42, mRegion, Callsite::UNDEFINED, NULL)
{
}
