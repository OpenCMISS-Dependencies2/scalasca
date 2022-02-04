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

#include <pearl/Callsite.h>

#include <iostream>

#include <gtest/gtest.h>

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
// Test fixture for Callsite tests
class CallsiteT
    : public Test
{
    public:
        CallsiteT();


    protected:
        const String   mRegionCanonicalName;
        const String   mRegionDisplayName;
        const String   mRegionDescription;
        const String   mRegionFileName;
        const Region   mCallee;
        const String   mFileName;
        const Callsite mItem;
};
}    // unnamed namespace


// --- Callsite tests -------------------------------------------------------

TEST_F(CallsiteT,
       operatorLeftShift_undefinedAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << Callsite::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallsiteT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallsiteT,
       operatorLeftShift_undefinedAtAbbrvDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << Callsite::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallsiteT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    string expected("42");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallsiteT,
       operatorLeftShift_undefinedAtCompactDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << Callsite::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallsiteT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    string expected("Callsite { id: 42, file: \"bar.c\", line: 99, callee: \"void foo()\" }");

    ostringstream result;
    result << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallsiteT,
       operatorLeftShift_undefinedAtFullDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << Callsite::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CallsiteT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    string expected("Callsite {\n"
                    "    id: 42\n"
                    "    file: String { id: 4, text: \"bar.c\" }\n"
                    "    line: 99\n"
                    "    callee: Region { id: 24, name: \"void foo()\", desc: \"My code\", role: function, paradigm: compiler, file: \"foo.c\", begin: 17, end: 24 }\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

CallsiteT::CallsiteT()
    : mRegionCanonicalName(0, "foo"),
      mRegionDisplayName(1, "void foo()"),
      mRegionDescription(2, "My code"),
      mRegionFileName(3, "foo.c"),
      mCallee(24, mRegionCanonicalName, mRegionDisplayName, mRegionDescription,
              Region::ROLE_FUNCTION, Paradigm::COMPILER, mRegionFileName,
              17, 24),
      mFileName(4, "bar.c"),
      mItem(42, mFileName, 99, mCallee)
{
}
