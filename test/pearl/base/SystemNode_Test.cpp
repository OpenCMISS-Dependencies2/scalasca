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

#include <pearl/SystemNode.h>

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
// Test fixture for SystemNode tests
class SystemNodeT
    : public Test
{
    public:
        SystemNodeT();


    protected:
        const String     mParentName;
        const String     mParentClass;
        SystemNode       mParent;
        const String     mName;
        const String     mClass;
        const SystemNode mItem;
};
}    // unnamed namespace


// --- SystemNode tests -----------------------------------------------------

TEST_F(SystemNodeT,
       operatorLeftShift_undefinedAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << SystemNode::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(SystemNodeT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(SystemNodeT,
       operatorLeftShift_undefinedAtAbbrvDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << SystemNode::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(SystemNodeT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    string expected("1");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(SystemNodeT,
       operatorLeftShift_undefinedAtCompactDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << SystemNode::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(SystemNodeT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    // System node w/o parent (root node)
    {
        string expected("SystemNode { id: 0, name: \"computer\", class: \"machine\", parent: <undef> }");

        ostringstream result;
        result << mParent;
        EXPECT_EQ(expected, result.str());
    }

    // System node w/ parent
    {
        string expected("SystemNode { id: 1, name: \"blade\", class: \"node\", parent: 0 }");

        ostringstream result;
        result << mItem;
        EXPECT_EQ(expected, result.str());
    }
}


TEST_F(SystemNodeT,
       operatorLeftShift_undefinedAtFullDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << SystemNode::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(SystemNodeT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    // System node w/o parent (root node)
    {
        string expected("SystemNode {\n"
                        "    id: 0\n"
                        "    name: String { id: 0, text: \"computer\" }\n"
                        "    class: String { id: 1, text: \"machine\" }\n"
                        "    parent: <undef>\n"
                        "}");

        ostringstream result;
        result << setDetail(DETAIL_FULL)
               << mParent;
        EXPECT_EQ(expected, result.str());
    }

    // System node w/ parent
    {
        string expected("SystemNode {\n"
                        "    id: 1\n"
                        "    name: String { id: 2, text: \"blade\" }\n"
                        "    class: String { id: 3, text: \"node\" }\n"
                        "    parent: SystemNode { id: 0, name: \"computer\", class: \"machine\", parent: <undef> }\n"
                        "}");

        ostringstream result;
        result << setDetail(DETAIL_FULL)
               << mItem;
        EXPECT_EQ(expected, result.str());
    }
}


// --- Helper ---------------------------------------------------------------

/// @todo[C++11] Use `nullptr` instead of `NULL`
SystemNodeT::SystemNodeT()
    : mParentName(0, "computer"),
      mParentClass(1, "machine"),
      mParent(0, mParentName, mParentClass, NULL),
      mName(2, "blade"),
      mClass(3, "node"),
      mItem(1, mName, mClass, &mParent)
{
}
