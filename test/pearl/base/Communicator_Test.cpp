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

#include <pearl/Communicator.h>

#include <iostream>

#include <gtest/gtest.h>

#include <pearl/CommLocationSet.h>
#include <pearl/CommSet.h>
#include <pearl/Location.h>
#include <pearl/ScopedPtr.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>
#include <pearl/iomanip.h>

#include "DefinitionContainer.h"
#include "Process.h"
#include "iomanip_detail.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Test fixture for Communicator tests
class CommunicatorT
    : public Test
{
    public:
        CommunicatorT();

        ~CommunicatorT();


    protected:
        ScopedPtr< SystemNode >       mSystemNode;
        ScopedPtr< LocationGroup >    mLocationGroup;
        DefinitionContainer< String > mStrings;
        vector< const Location* >     mLocations;
        ScopedPtr< CommLocationSet >  mCommLocationSet;
        ScopedPtr< CommSet >          mCommSetWorld;
        ScopedPtr< CommSet >          mCommSetOdd;
        ScopedPtr< Communicator >     mCommWorld;
        ScopedPtr< Communicator >     mCommOdd;
};
}    // unnamed namespace


// --- Communicator tests ---------------------------------------------------

TEST_F(CommunicatorT,
       operatorLeftShift_undefinedAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << Communicator::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommunicatorT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << *mCommOdd;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommunicatorT,
       operatorLeftShift_undefinedAtAbbrvDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << Communicator::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommunicatorT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    string expected("1");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << *mCommOdd;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommunicatorT,
       operatorLeftShift_undefinedAtCompactDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << Communicator::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommunicatorT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    // Communicator w/o parent (world)
    {
        string expected("Communicator { id: 0, name: \"world_comm\", comm set: 0, parent: <undef> }");

        ostringstream result;
        result << *mCommWorld;
        EXPECT_EQ(expected, result.str());
    }

    // Communicator w/ parent
    {
        string expected("Communicator { id: 1, name: \"odd_comm\", comm set: 1, parent: 0 }");

        ostringstream result;
        result << *mCommOdd;
        EXPECT_EQ(expected, result.str());
    }
}


TEST_F(CommunicatorT,
       operatorLeftShift_undefinedAtFullDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << Communicator::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommunicatorT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    // Communicator w/o parent (world)
    {
        string expected("Communicator {\n"
                        "    id: 0\n"
                        "    name: String { id: 18, text: \"world_comm\" }\n"
                        "    comm set: CommSet { id: 0, name: \"world_group\", props: 2 (world), paradigm: mpi, size: 16, members: [...] }\n"
                        "    parent: <undef>\n"
                        "}");

        ostringstream result;
        result << setDetail(DETAIL_FULL)
               << *mCommWorld;
        EXPECT_EQ(expected, result.str());
    }

    // Communicator w/ parent
    {
        string expected("Communicator {\n"
                        "    id: 1\n"
                        "    name: String { id: 19, text: \"odd_comm\" }\n"
                        "    comm set: CommSet { id: 1, name: \"odd_group\", props: 0 (NONE), paradigm: mpi, size: 8, members: [...] }\n"
                        "    parent: Communicator { id: 0, name: \"world_comm\", comm set: 0, parent: <undef> }\n"
                        "}");

        ostringstream result;
        result << setDetail(DETAIL_FULL)
               << *mCommOdd;
        EXPECT_EQ(expected, result.str());
    }
}


// --- Helper ---------------------------------------------------------------

CommunicatorT::CommunicatorT()
{
    // Create dummy SystemNode (used as parent)
    mSystemNode.reset(new SystemNode(0,
                                     String::UNDEFINED,
                                     String::UNDEFINED,
                                     0));

    // Create dummy LocationGroup (used as parent)
    mLocationGroup.reset(new Process(0,
                                     String::UNDEFINED,
                                     mSystemNode.get()));

    // Create locations
    for (uint32_t id = 0; id < 16; ++id)
    {
        ScopedPtr< String > name(new String(id, "Rank " + toStdString(id)));
        mStrings.addItem(name.get());
        name.release();

        ScopedPtr< Location > location(new Location(id,
                                                    *mStrings.getItemById(id),
                                                    Location::TYPE_CPU_THREAD,
                                                    0,
                                                    mLocationGroup.get()));
        mLocations.push_back(location.get());
        location.release();
    }

    // Create communication location set
    mCommLocationSet.reset(new CommLocationSet(0,
                                               String::UNDEFINED,
                                               Paradigm::MPI,
                                               mLocations));

    // Create "world" communication set
    vector< uint32_t > ranks;
    ranks.reserve(16);
    for (uint32_t rank = 0; rank < 16; ++rank)
    {
        ranks.push_back(rank);
    }
    ScopedPtr< String > name(new String(16, "world_group"));
    mStrings.addItem(name.get());
    mCommSetWorld.reset(new CommSet(0, *name, GroupingSet::PROPERTY_WORLD,
                                    *mCommLocationSet, ranks));
    name.release();

    // Create "odd" communication set
    ranks.clear();
    for (uint32_t rank = 1; rank < 16; rank += 2)
    {
        ranks.push_back(rank);
    }
    name.reset(new String(17, "odd_group"));
    mStrings.addItem(name.get());
    mCommSetOdd.reset(new CommSet(1, *name, GroupingSet::PROPERTY_NONE,
                                  *mCommLocationSet, ranks));
    name.release();

    // Create "world" communicator
    name.reset(new String(18, "world_comm"));
    mStrings.addItem(name.get());
    mCommWorld.reset(new Communicator(0, *name, *mCommSetWorld, 0));
    name.release();

    // Create "odd" communicator
    name.reset(new String(19, "odd_comm"));
    mStrings.addItem(name.get());
    mCommOdd.reset(new Communicator(1, *name, *mCommSetOdd, mCommWorld.get()));
    name.release();
}


CommunicatorT::~CommunicatorT()
{
    for_each(mLocations.begin(), mLocations.end(), delete_ptr< Location >());
}
