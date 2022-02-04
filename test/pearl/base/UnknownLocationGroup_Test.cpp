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

#include "UnknownLocationGroup.h"

#include <iostream>

#include <gtest/gtest.h>

#include <pearl/String.h>
#include <pearl/SystemNode.h>
#include <pearl/iomanip.h>

#include "iomanip_detail.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Test fixture for UnknownLocationGroup tests
class UnknownLocationGroupT
    : public Test
{
    public:
        UnknownLocationGroupT();


    protected:
        const String         mParentName;
        const String         mParentClass;
        SystemNode           mParent;
        const String         mName;
        UnknownLocationGroup mItem;
};
}    // unnamed namespace


// --- UnknownLocationGroup tests -------------------------------------------

TEST_F(UnknownLocationGroupT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(UnknownLocationGroupT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    string expected("42");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(UnknownLocationGroupT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    string expected("UnknownLocationGroup { id: 42, name: \"Group 42\", parent: 0 }");

    ostringstream result;
    result << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(UnknownLocationGroupT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    string expected("UnknownLocationGroup {\n"
                    "    id: 42\n"
                    "    name: String { id: 2, text: \"Group 42\" }\n"
                    "    parent: SystemNode { id: 0, name: \"blade\", class: \"node\", parent: <undef> }\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

/// @todo[C++11] Use `nullptr` instead of `NULL`
UnknownLocationGroupT::UnknownLocationGroupT()
    : mParentName(0, "blade"),
      mParentClass(1, "node"),
      mParent(0, mParentName, mParentClass, NULL),
      mName(2, "Group 42"),
      mItem(42, mName, &mParent)
{
}
