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

#include <pearl/CommLocationSet.h>

#include <algorithm>
#include <limits>
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
// Test fixture for communication location set tests
class CommLocationSetT
    : public Test
{
    public:
        virtual
        ~CommLocationSetT()
        {
            for_each(mLocations.begin(), mLocations.end(),
                     delete_ptr< Location >());
        }

        void
        createLocations(const Location::IdType maxId);


    protected:
        vector< const Location* >     mLocations;
        ScopedPtr< Process >          mLocationGroup;
        ScopedPtr< SystemNode >       mSystemNode;
        DefinitionContainer< String > mStrings;
};
}    // unnamed namespace


// --- CommLocationSet tests ------------------------------------------------


TEST_F(CommLocationSetT,
       getId_undefined_returnsMaxId)
{
    EXPECT_EQ(numeric_limits< CommLocationSet::IdType >::max(),
              CommLocationSet::UNDEFINED.getId());
}


TEST_F(CommLocationSetT,
       getId_item_returnsIdentifier)
{
    CommLocationSet item0(0, String::UNDEFINED, Paradigm::UNKNOWN, mLocations);
    EXPECT_EQ(0, item0.getId());

    CommLocationSet item1(1, String::UNDEFINED, Paradigm::UNKNOWN, mLocations);
    EXPECT_EQ(1, item1.getId());

    CommLocationSet item42(42, String::UNDEFINED, Paradigm::UNKNOWN,
                           mLocations);
    EXPECT_EQ(42, item42.getId());
}


TEST_F(CommLocationSetT,
       getName_undefined_returnsUndefined)
{
    EXPECT_EQ(String::UNDEFINED, CommLocationSet::UNDEFINED.getName());
}


TEST_F(CommLocationSetT,
       getName_item_returnsName)
{
    CommLocationSet item0(0, String::UNDEFINED, Paradigm::UNKNOWN, mLocations);
    EXPECT_EQ(&String::UNDEFINED, &item0.getName());

    String          name1(0, "Foo");
    CommLocationSet item1(1, name1, Paradigm::UNKNOWN, mLocations);
    EXPECT_EQ(&name1, &item1.getName());
}


TEST_F(CommLocationSetT,
       getType_undefined_returnsType)
{
    EXPECT_EQ(GroupingSet::COMM_LOCATION_SET,
              CommLocationSet::UNDEFINED.getType());
}


TEST_F(CommLocationSetT,
       getType_item_returnsType)
{
    CommLocationSet item(0, String::UNDEFINED, Paradigm::UNKNOWN, mLocations);
    EXPECT_EQ(GroupingSet::COMM_LOCATION_SET, item.getType());
}


TEST_F(CommLocationSetT,
       numLocations_undefined_returnsZero)
{
    EXPECT_EQ(0, CommLocationSet::UNDEFINED.numLocations());
}


TEST_F(CommLocationSetT,
       numLocations_emptyItem_returnsZero)
{
    CommLocationSet item(0, String::UNDEFINED, Paradigm::UNKNOWN, mLocations);
    EXPECT_EQ(0, item.numLocations());
}


TEST_F(CommLocationSetT,
       numLocations_item_returnsMemberCount)
{
    createLocations(5);

    CommLocationSet item(0, String::UNDEFINED, Paradigm::UNKNOWN, mLocations);
    EXPECT_EQ(5, item.numLocations());
}


TEST_F(CommLocationSetT,
       getLocation_itemAndValidRank_returnsMember)
{
    createLocations(3);

    CommLocationSet item(0, String::UNDEFINED, Paradigm::UNKNOWN, mLocations);
    EXPECT_EQ(mLocations[0], &item.getLocation(0));
    EXPECT_EQ(mLocations[1], &item.getLocation(1));
    EXPECT_EQ(mLocations[2], &item.getLocation(2));
}


TEST_F(CommLocationSetT,
       getLocation_itemAndInvalidRank_throws)
{
    createLocations(3);

    CommLocationSet item(0, String::UNDEFINED, Paradigm::UNKNOWN, mLocations);
    EXPECT_THROW(item.getLocation(3), pearl::RuntimeError);
}


TEST_F(CommLocationSetT,
       getParadigm_undefined_returnsUnknown)
{
    EXPECT_EQ(Paradigm::UNKNOWN, CommLocationSet::UNDEFINED.getParadigm());
}


TEST_F(CommLocationSetT,
       getParadigm_item_returnsParadigm)
{
    CommLocationSet item0(0, String::UNDEFINED, Paradigm::UNKNOWN, mLocations);
    EXPECT_EQ(Paradigm::UNKNOWN, item0.getParadigm());

    CommLocationSet item1(1, String::UNDEFINED, Paradigm::MPI, mLocations);
    EXPECT_EQ(Paradigm::MPI, item1.getParadigm());
}


TEST_F(CommLocationSetT,
       operatorLeftShift_undefinedAtSkipDetail_printsNothing)
{
    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << CommLocationSet::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommLocationSetT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    createLocations(2);

    String          name(24, "Foo");
    CommLocationSet item(42, name, Paradigm::MPI, mLocations);

    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommLocationSetT,
       operatorLeftShift_undefinedAtAbbrevDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << CommLocationSet::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommLocationSetT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    createLocations(2);

    String          name(24, "Foo");
    CommLocationSet item(42, name, Paradigm::MPI, mLocations);

    string expected("42");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommLocationSetT,
       operatorLeftShift_undefinedAtCompactDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << CommLocationSet::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommLocationSetT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    createLocations(2);

    String          name(24, "Foo");
    CommLocationSet item(42, name, Paradigm::MPI, mLocations);

    string expected("CommLocationSet { id: 42, name: \"Foo\", paradigm: mpi, size: 2, members: [...] }");

    ostringstream result;
    result << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommLocationSetT,
       operatorLeftShift_undefinedAtFullDetail_printsUndef)
{
    string expected("<undef>");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << CommLocationSet::UNDEFINED;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommLocationSetT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    createLocations(2);

    String          name(24, "Foo");
    CommLocationSet item(42, name, Paradigm::MPI, mLocations);

    string expected("CommLocationSet {\n"
                    "    id: 42\n"
                    "    name: String { id: 24, text: \"Foo\" }\n"
                    "    paradigm: mpi\n"
                    "    size: 2\n"
                    "    members: [ 0, 1 ]\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CommLocationSetT,
       operatorLeftShift_itemAtIncreasedDetail_printsMembers)
{
    createLocations(2);

    String          name(24, "Foo");
    CommLocationSet item(42, name, Paradigm::MPI, mLocations);

    string expected("CommLocationSet {\n"
                    "    id: 42\n"
                    "    name: String {\n"
                    "        id: 24\n"
                    "        text: \"Foo\"\n"
                    "    }\n"
                    "    paradigm: mpi\n"
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
CommLocationSetT::createLocations(const Location::IdType maxId)
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
