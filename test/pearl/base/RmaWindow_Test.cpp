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

#include <pearl/RmaWindow.h>

#include <iostream>

#include <gtest/gtest.h>

#include <pearl/CommLocationSet.h>
#include <pearl/CommSet.h>
#include <pearl/Communicator.h>
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
// Test fixture for RmaWindow tests
class RmaWindowT
    : public Test
{
    public:
        RmaWindowT();

        ~RmaWindowT();


    protected:
        ScopedPtr< SystemNode >       mSystemNode;
        ScopedPtr< LocationGroup >    mLocationGroup;
        DefinitionContainer< String > mStrings;
        vector< const Location* >     mLocations;
        ScopedPtr< CommLocationSet >  mCommLocationSet;
        ScopedPtr< CommSet >          mCommSet;
        ScopedPtr< Communicator >     mComm;
        ScopedPtr< RmaWindow >        mWindow;
};
}    // unnamed namespace


// --- RmaWindow tests ------------------------------------------------------

TEST_F(RmaWindowT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << *mWindow;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RmaWindowT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    string expected("42");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << *mWindow;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RmaWindowT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    string expected("RmaWindow { id: 42, comm: 0 }");

    ostringstream result;
    result << *mWindow;
    EXPECT_EQ(expected, result.str());
}


TEST_F(RmaWindowT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    string expected("RmaWindow {\n"
                    "    id: 42\n"
                    "    comm: Communicator { id: 0, name: \"world_comm\", comm set: 0, parent: <undef> }\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << *mWindow;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

RmaWindowT::RmaWindowT()
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
    mCommSet.reset(new CommSet(0, *name, GroupingSet::PROPERTY_WORLD,
                               *mCommLocationSet, ranks));
    name.release();

    // Create "world" communicator
    name.reset(new String(18, "world_comm"));
    mStrings.addItem(name.get());
    mComm.reset(new Communicator(0, *name, *mCommSet, 0));
    name.release();

    // Create RMA window
    mWindow.reset(new RmaWindow(42, mComm.get()));
}


RmaWindowT::~RmaWindowT()
{
    for_each(mLocations.begin(), mLocations.end(), delete_ptr< Location >());
}
