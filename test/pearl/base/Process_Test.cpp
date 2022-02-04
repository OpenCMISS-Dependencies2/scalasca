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

#include "Process.h"

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
// Test fixture for Process tests
class ProcessT
    : public Test
{
    public:
        ProcessT();


    protected:
        const String mParentName;
        const String mParentClass;
        SystemNode   mParent;
        const String mName;
        Process      mItem;
};
}    // unnamed namespace


// --- Process tests --------------------------------------------------------

TEST_F(ProcessT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(ProcessT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    string expected("42");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(ProcessT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    string expected("Process { id: 42, name: \"MPI Rank 42\", rank: 42, parent: 0 }");

    ostringstream result;
    result << mItem;
    EXPECT_EQ(expected, result.str());
}


TEST_F(ProcessT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    string expected("Process {\n"
                    "    id: 42\n"
                    "    name: String { id: 2, text: \"MPI Rank 42\" }\n"
                    "    rank: 42\n"
                    "    parent: SystemNode { id: 0, name: \"blade\", class: \"node\", parent: <undef> }\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << mItem;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

/// @todo[C++11] Use `nullptr` instead of `NULL`
ProcessT::ProcessT()
    : mParentName(0, "blade"),
      mParentClass(1, "node"),
      mParent(0, mParentName, mParentClass, NULL),
      mName(2, "MPI Rank 42"),
      mItem(42, mName, &mParent)
{
    mItem.setRank(42);
}
