/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include <pearl/LocationSet.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <pearl/Error.h>
#include <pearl/Location.h>
#include <pearl/ScopedPtr.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>
#include <pearl/Utils.h>
#include <pearl/iomanip.h>

#include "DefinitionContainer.h"
#include "Functors.h"
#include "Process.h"
#include "iomanip_detail.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Test fixture for location set tests
class LocationSetT
    : public Test
{
    public:
        virtual
        ~LocationSetT()
        {
            for_each(mLocations.begin(), mLocations.end(),
                     delete_ptr< Location >());
        }

        void
        createLocations(const Location::IdType maxId);


    protected:
        vector< const Location* >     mLocations;
        ScopedPtr< LocationGroup >    mLocationGroup;
        ScopedPtr< SystemNode >       mSystemNode;
        DefinitionContainer< String > mStrings;
};
}    // unnamed namespace


// --- LocationSet tests ----------------------------------------------------

TEST_F(LocationSetT,
       getId_item_returnsIdentifier)
{
    LocationSet item0(0, String::UNDEFINED, mLocations);
    EXPECT_EQ(0, item0.getId());

    LocationSet item1(1, String::UNDEFINED, mLocations);
    EXPECT_EQ(1, item1.getId());

    LocationSet item42(42, String::UNDEFINED, mLocations);
    EXPECT_EQ(42, item42.getId());
}


TEST_F(LocationSetT,
       getName_item_returnsName)
{
    LocationSet item0(0, String::UNDEFINED, mLocations);
    EXPECT_EQ(&String::UNDEFINED, &item0.getName());

    String      name1(0, "Foo");
    LocationSet item1(1, name1, mLocations);
    EXPECT_EQ(&name1, &item1.getName());
}


TEST_F(LocationSetT,
       getType_item_returnsType)
{
    LocationSet item(0, String::UNDEFINED, mLocations);
    EXPECT_EQ(GroupingSet::LOCATION_SET, item.getType());
}


TEST_F(LocationSetT,
       numLocations_emptyItem_returnsZero)
{
    LocationSet item(0, String::UNDEFINED, mLocations);
    EXPECT_EQ(0, item.numLocations());
}


TEST_F(LocationSetT,
       numLocations_item_returnsMemberCount)
{
    createLocations(5);

    LocationSet item(0, String::UNDEFINED, mLocations);
    EXPECT_EQ(5, item.numLocations());
}


TEST_F(LocationSetT,
       getLocation_itemAndValidRank_returnsMember)
{
    createLocations(3);

    LocationSet item(0, String::UNDEFINED, mLocations);
    EXPECT_EQ(mLocations[0], &item.getLocation(0));
    EXPECT_EQ(mLocations[1], &item.getLocation(1));
    EXPECT_EQ(mLocations[2], &item.getLocation(2));
}


TEST_F(LocationSetT,
       getLocation_itemAndInvalidRank_throws)
{
    createLocations(3);

    LocationSet item(0, String::UNDEFINED, mLocations);
    EXPECT_THROW(item.getLocation(3), pearl::RuntimeError);
}


TEST_F(LocationSetT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    createLocations(2);

    String      name(24, "Foo");
    LocationSet item(42, name, mLocations);

    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationSetT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    createLocations(2);

    String      name(24, "Foo");
    LocationSet item(42, name, mLocations);

    string expected("42");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationSetT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    createLocations(2);

    String      name(24, "Foo");
    LocationSet item(42, name, mLocations);

    string expected("LocationSet { id: 42, name: \"Foo\", size: 2, members: [...] }");

    ostringstream result;
    result << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationSetT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    createLocations(2);

    String      name(24, "Foo");
    LocationSet item(42, name, mLocations);

    string expected("LocationSet {\n"
                    "    id: 42\n"
                    "    name: String { id: 24, text: \"Foo\" }\n"
                    "    size: 2\n"
                    "    members: [ 0, 1 ]\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(LocationSetT,
       operatorLeftShift_itemAtIncreasedDetail_printsMembers)
{
    createLocations(2);

    String      name(24, "Foo");
    LocationSet item(42, name, mLocations);

    string expected("LocationSet {\n"
                    "    id: 42\n"
                    "    name: String {\n"
                    "        id: 24\n"
                    "        text: \"Foo\"\n"
                    "    }\n"
                    "    size: 2\n"
                    "    members: [\n"
                    "        Location { id: 0, name: \"Thread 0\", type: CPU thread, #events: 0, parent: 0 }\n"
                    "        Location { id: 1, name: \"Thread 1\", type: CPU thread, #events: 0, parent: 0 }\n"
                    "    ]\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL + 1)
           << item;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

void
LocationSetT::createLocations(const Location::IdType maxId)
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
    for (Location::IdType id = 0; id < maxId; ++id)
    {
        ScopedPtr< String > name(new String(id, "Thread " + toStdString(id)));
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
}
