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

#include <pearl/CartTopology.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <pearl/CartDimension.h>
#include <pearl/CommLocationSet.h>
#include <pearl/CommSet.h>
#include <pearl/Communicator.h>
#include <pearl/Error.h>
#include <pearl/Location.h>
#include <pearl/ScopedPtr.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>
#include <pearl/iomanip.h>

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
// Test fixture for Cartesian topology tests
class CartTopologyT
    : public Test
{
    public:
        ~CartTopologyT();

        void
        createCommunicator(uint32_t size);


    protected:
        CartTopology::DimensionContainer mDimensions;
        ScopedPtr< Communicator >        mCommunicator;
        ScopedPtr< CommSet >             mCommSet;
        ScopedPtr< CommLocationSet >     mCommLocationSet;
        vector< const Location* >        mLocations;
        ScopedPtr< LocationGroup >       mLocationGroup;
        ScopedPtr< SystemNode >          mSystemNode;
};
}    // unnamed namespace


// --- CartTopology tests ---------------------------------------------------

TEST_F(CartTopologyT,
       getId_item_returnsIdentifier)
{
    CartTopology item0(0, String::UNDEFINED, Communicator::UNDEFINED,
                       mDimensions);
    EXPECT_EQ(0, item0.getId());

    CartTopology item1(1, String::UNDEFINED, Communicator::UNDEFINED,
                       mDimensions);
    EXPECT_EQ(1, item1.getId());

    CartTopology item42(42, String::UNDEFINED, Communicator::UNDEFINED,
                        mDimensions);
    EXPECT_EQ(42, item42.getId());
}


TEST_F(CartTopologyT,
       getName_item_returnsName)
{
    CartTopology item0(0, String::UNDEFINED, Communicator::UNDEFINED,
                       mDimensions);
    EXPECT_EQ(&String::UNDEFINED, &item0.getName());

    String       name1(0, "Foo");
    CartTopology item1(1, name1, Communicator::UNDEFINED, mDimensions);
    EXPECT_EQ(&name1, &item1.getName());
}


TEST_F(CartTopologyT,
       getCommunicator_item_returnsCommunicator)
{
    CartTopology item0(0, String::UNDEFINED, Communicator::UNDEFINED,
                       mDimensions);
    EXPECT_EQ(&Communicator::UNDEFINED, &item0.getCommunicator());

    createCommunicator(16);
    CartTopology item1(1, String::UNDEFINED, *mCommunicator, mDimensions);
    EXPECT_EQ(mCommunicator.get(), &item1.getCommunicator());
}


TEST_F(CartTopologyT,
       numDimensions_item_returnsDimensionCount)
{
    CartTopology item0(0, String::UNDEFINED, Communicator::UNDEFINED,
                       mDimensions);
    EXPECT_EQ(0, item0.numDimensions());

    CartDimension dim0(0, String::UNDEFINED, 0, CartDimension::PERIODIC);
    mDimensions.push_back(&dim0);

    CartTopology item1(1, String::UNDEFINED, Communicator::UNDEFINED,
                       mDimensions);
    EXPECT_EQ(1, item1.numDimensions());

    CartDimension dim1(1, String::UNDEFINED, 0, CartDimension::PERIODIC);
    mDimensions.push_back(&dim1);

    CartTopology item2(2, String::UNDEFINED, Communicator::UNDEFINED,
                       mDimensions);
    EXPECT_EQ(2, item2.numDimensions());
}


TEST_F(CartTopologyT,
       getDimension_itemAndValidIndex_returnsDimension)
{
    CartDimension dim0(0, String::UNDEFINED, 0, CartDimension::PERIODIC);
    mDimensions.push_back(&dim0);
    CartDimension dim1(1, String::UNDEFINED, 0, CartDimension::PERIODIC);
    mDimensions.push_back(&dim1);

    CartTopology item(0, String::UNDEFINED, Communicator::UNDEFINED,
                      mDimensions);
    EXPECT_EQ(&dim0, &item.getDimension(0));
    EXPECT_EQ(&dim1, &item.getDimension(1));
}


TEST_F(CartTopologyT,
       getDimension_itemAndInvalidIndex_throws)
{
    CartTopology item(0, String::UNDEFINED, Communicator::UNDEFINED,
                      mDimensions);
    EXPECT_THROW(item.getDimension(0), pearl::RuntimeError);
}


TEST_F(CartTopologyT,
       getRankCoordinates_item_returnsEmptyList)
{
    createCommunicator(8);
    CartTopology item(0, String::UNDEFINED, *mCommunicator, mDimensions);
    EXPECT_EQ(0, item.getRankCoordinates(0).size());
}


TEST_F(CartTopologyT,
       getRankCoordinates_itemAndInvalidRank_throws)
{
    CartTopology item(0, String::UNDEFINED, Communicator::UNDEFINED,
                      mDimensions);
    EXPECT_THROW(item.getRankCoordinates(0), pearl::RuntimeError);
}


TEST_F(CartTopologyT,
       operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    String        name0(24, "Periodic");
    CartDimension dim0(42, name0, 2, CartDimension::PERIODIC);
    mDimensions.push_back(&dim0);

    String        name1(25, "Non-periodic");
    CartDimension dim1(43, name1, 3, CartDimension::NON_PERIODIC);
    mDimensions.push_back(&dim1);

    String       name2(4, "My topology");
    CartTopology item(17, name2, Communicator::UNDEFINED, mDimensions);

    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CartTopologyT,
       operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    String        name0(24, "Periodic");
    CartDimension dim0(42, name0, 2, CartDimension::PERIODIC);
    mDimensions.push_back(&dim0);

    String        name1(25, "Non-periodic");
    CartDimension dim1(43, name1, 3, CartDimension::NON_PERIODIC);
    mDimensions.push_back(&dim1);

    String       name2(4, "My topology");
    CartTopology item(17, name2, Communicator::UNDEFINED, mDimensions);

    string expected("17");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CartTopologyT,
       operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    String        name0(24, "Periodic");
    CartDimension dim0(42, name0, 2, CartDimension::PERIODIC);
    mDimensions.push_back(&dim0);

    String        name1(25, "Non-periodic");
    CartDimension dim1(43, name1, 3, CartDimension::NON_PERIODIC);
    mDimensions.push_back(&dim1);

    String       name2(4, "My topology");
    CartTopology item(17, name2, Communicator::UNDEFINED, mDimensions);

    string expected("CartTopology { id: 17, name: \"My topology\", comm: <undef>, #dims: 2, dims: [...], coords: [...] }");

    ostringstream result;
    result << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CartTopologyT,
       operatorLeftShift_itemAtFullDetail_printsData)
{
    String        name0(24, "Periodic");
    CartDimension dim0(42, name0, 2, CartDimension::PERIODIC);
    mDimensions.push_back(&dim0);

    String        name1(25, "Non-periodic");
    CartDimension dim1(43, name1, 3, CartDimension::NON_PERIODIC);
    mDimensions.push_back(&dim1);

    String       name2(4, "My topology");
    CartTopology item(17, name2, Communicator::UNDEFINED, mDimensions);

    string expected("CartTopology {\n"
                    "    id: 17\n"
                    "    name: String { id: 4, text: \"My topology\" }\n"
                    "    comm: <undef>\n"
                    "    #dims: 2\n"
                    "    dims: [ 42, 43 ]\n"
                    "    coords: [  ]\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST_F(CartTopologyT,
       operatorLeftShift_itemAtIncreasedDetail_printsMembers)
{
    String        name0(24, "Periodic");
    CartDimension dim0(42, name0, 2, CartDimension::PERIODIC);
    mDimensions.push_back(&dim0);

    String        name1(25, "Non-periodic");
    CartDimension dim1(43, name1, 3, CartDimension::NON_PERIODIC);
    mDimensions.push_back(&dim1);

    String       name2(4, "My topology");
    CartTopology item(17, name2, Communicator::UNDEFINED, mDimensions);

    string expected("CartTopology {\n"
                    "    id: 17\n"
                    "    name: String {\n"
                    "        id: 4\n"
                    "        text: \"My topology\"\n"
                    "    }\n"
                    "    comm: <undef>\n"
                    "    #dims: 2\n"
                    "    dims: [\n"
                    "        CartDimension { id: 42, name: \"Periodic\", size: 2, periodic: true }\n"
                    "        CartDimension { id: 43, name: \"Non-periodic\", size: 3, periodic: false }\n"
                    "    ]\n"
                    "    coords: [\n"
                    "        \n"
                    "    ]\n"
                    "}");

    ostringstream result;
    result << setDetail(DETAIL_FULL + 1)
           << item;
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

CartTopologyT::~CartTopologyT()
{
    for_each(mLocations.begin(), mLocations.end(), delete_ptr< Location >());
}


/// @todo [C++11] Pass `nullptr` to comm constructor for `parent`
void
CartTopologyT::createCommunicator(const uint32_t size)
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
    vector< uint32_t > ranks;
    ranks.reserve(size);
    mLocations.reserve(size);
    for (uint32_t rank = 0; rank < size; ++rank)
    {
        ranks.push_back(rank);

        ScopedPtr< Location > location(new Location(rank,
                                                    String::UNDEFINED,
                                                    Location::TYPE_CPU_THREAD,
                                                    0,
                                                    mLocationGroup.get()));
        mLocations.push_back(location.get());
        location.release();
    }

    // Create CommLocationSet
    mCommLocationSet.reset(new CommLocationSet(0,
                                               String::UNDEFINED,
                                               Paradigm::USER,
                                               mLocations));

    // Create CommSet
    mCommSet.reset(new CommSet(0,
                               String::UNDEFINED,
                               GroupingSet::PROPERTY_NONE,
                               *mCommLocationSet,
                               ranks));

    // Create Communicator
    mCommunicator.reset(new Communicator(0,
                                         String::UNDEFINED, *mCommSet, 0));
}
