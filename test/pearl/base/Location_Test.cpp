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

#include <pearl/Location.h>

#include <iostream>

#include <gtest/gtest.h>

#include <pearl/String.h>
#include <pearl/SystemNode.h>
#include <pearl/iomanip.h>

#include "Process.h"
#include "iomanip_detail.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Test fixture for Location tests
class LocationT
    : public Test
{
    public:
        LocationT();


    protected:
        const String   mSysNodeName;
        const String   mSysNodeClass;
        SystemNode     mSysNode;
        const String   mProcessName;
        Process        mProcess;
        const String   mName;
        const Location mItem;
};
}    // unnamed namespace


// --- Location tests -------------------------------------------------------

TEST_F(LocationT,
       operatorLeftShift_undefinedAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << Location::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationT,
       operatorLeftShift_undefinedAtAbbrvDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << Location::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    string expected("42");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationT,
       operatorLeftShift_undefinedAtCompactDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << Location::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    string expected("Location { id: 42, name: \"Master thread\", type: CPU thread, #events: 17, parent: 24 }");

    ostringstream result;
    result << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationT,
       operatorLeftShift_undefinedAtFullDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << Location::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    string expected("Location {\n"
                    "    id: 42\n"
                    "    name: String { id: 3, text: \"Master thread\" }\n"
                    "    type: CPU thread\n"
                    "    #events: 17\n"
                    "    parent: Process { id: 24, name: \"MPI Rank 24\", rank: 24, parent: 0 }\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

LocationT::LocationT()
    : mSysNodeName(0, "blade"),
      mSysNodeClass(1, "node"),
      mSysNode(0, mSysNodeName, mSysNodeClass, NULL),
      mProcessName(2, "MPI Rank 24"),
      mProcess(24, mProcessName, &mSysNode),
      mName(3, "Master thread"),
      mItem(42, mName, Location::TYPE_CPU_THREAD, 17, &mProcess)
{
    mProcess.setRank(24);
}
