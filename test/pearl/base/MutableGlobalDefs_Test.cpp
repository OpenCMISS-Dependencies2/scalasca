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

#include "MutableGlobalDefs.h"

#include <vector>

#include <gtest/gtest.h>

#include <pearl/CallingContext.h>
#include <pearl/Callsite.h>
#include <pearl/CartDimension.h>
#include <pearl/CartTopology.h>
#include <pearl/CommLocationSet.h>
#include <pearl/CommSet.h>
#include <pearl/Communicator.h>
#include <pearl/Error.h>
#include <pearl/Location.h>
#include <pearl/LocationGroup.h>
#include <pearl/LocationSet.h>
#include <pearl/Metric.h>
#include <pearl/Region.h>
#include <pearl/RmaWindow.h>
#include <pearl/SourceLocation.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>

#include "Process.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Dummy test fixture needed for MutableGlobalDefs typed tests
template< typename DefClassT >
class MutableGlobalDefsTypedT
    : public Test
{
};


// Instantiate test cases for definition types supported by MutableGlobalDefs
typedef Types< CallingContext,
               Callsite,
               CartDimension,
               CartTopology,
               Communicator,
               GroupingSet,
               Location,
               LocationGroup,
               Metric,
               Region,
               RmaWindow,
               SourceLocation,
               String,
               SystemNode > DefinitionTypes;
TYPED_TEST_CASE(MutableGlobalDefsTypedT, DefinitionTypes);


// createItem function template and specializations for supported definitions
template< typename DefClassT >
DefClassT*
createItem(const typename DefClassT::IdType id)
{
    return 0;
}


template< >
CallingContext*
createItem(const CallingContext::IdType id)
{
    return new CallingContext(id,
                              Region::UNDEFINED,
                              SourceLocation::UNDEFINED,
                              0);
}


template< >
Callsite*
createItem(const Callsite::IdType id)
{
    return new Callsite(id, String::UNDEFINED, 0, Region::UNDEFINED);
}


template< >
CartDimension*
createItem(const CartDimension::IdType id)
{
    return new CartDimension(id, String::UNDEFINED, 0, CartDimension::PERIODIC);
}


template< >
CartTopology*
createItem(const CartTopology::IdType id)
{
    CartTopology::DimensionContainer dimensions;

    return new CartTopology(id,
                            String::UNDEFINED,
                            Communicator::UNDEFINED,
                            dimensions);
}


template< >
Communicator*
createItem(const Communicator::IdType id)
{
    return new Communicator(id, String::UNDEFINED, CommSet::UNDEFINED, 0);
}


template< >
GroupingSet*
createItem(const GroupingSet::IdType id)
{
    vector< const Location* > members;

    return new LocationSet(id, String::UNDEFINED, members);
}


template< >
Location*
createItem(const Location::IdType id)
{
    Process process(0,
                    String::UNDEFINED,
                    const_cast< SystemNode* >(&SystemNode::UNDEFINED));

    return new Location(id,
                        String::UNDEFINED,
                        Location::TYPE_CPU_THREAD,
                        0,
                        &process);
}


template< >
LocationGroup*
createItem(const LocationGroup::IdType id)
{
    return new Process(id,
                       String::UNDEFINED,
                       const_cast< SystemNode* >(&SystemNode::UNDEFINED));
}


template< >
Metric*
createItem(const Metric::IdType id)
{
    return new Metric(id, "", "", "INTEGER", "COUNTER", "START");
}


template< >
Region*
createItem(const Region::IdType id)
{
    return new Region(id,
                      String::UNDEFINED,
                      String::UNDEFINED,
                      String::UNDEFINED,
                      Region::ROLE_UNKNOWN,
                      Paradigm::UNKNOWN,
                      String::UNDEFINED,
                      0,
                      0);
}


template< >
RmaWindow*
createItem(const RmaWindow::IdType id)
{
    return new RmaWindow(id,
                         const_cast< Communicator* >(&Communicator::UNDEFINED));
}


template< >
SourceLocation*
createItem(const SourceLocation::IdType id)
{
    return new SourceLocation(id, String::UNDEFINED, 0);
}


template< >
String*
createItem(const String::IdType id)
{
    return new String(id, "");
}


template< >
SystemNode*
createItem(const SystemNode::IdType id)
{
    return new SystemNode(id, String::UNDEFINED, String::UNDEFINED, 0);
}


// addItem function template and specializations for supported definitions
template< typename DefClassT >
void
addItem(MutableGlobalDefs& defs,
        DefClassT*         item)
{
}


#define ADDITEM_FUNC(DefClassT)       \
    template< >                       \
    void                              \
    addItem(MutableGlobalDefs & defs, \
            DefClassT * item)         \
    {                                 \
        defs.add ## DefClassT(item);  \
    }

ADDITEM_FUNC(CallingContext)
ADDITEM_FUNC(Callsite)
ADDITEM_FUNC(CartDimension)
ADDITEM_FUNC(CartTopology)
ADDITEM_FUNC(Communicator)
ADDITEM_FUNC(GroupingSet)
ADDITEM_FUNC(Location)
ADDITEM_FUNC(LocationGroup)
ADDITEM_FUNC(Metric)
ADDITEM_FUNC(Region)
ADDITEM_FUNC(RmaWindow)
ADDITEM_FUNC(SourceLocation)
ADDITEM_FUNC(String)
ADDITEM_FUNC(SystemNode)

#undef ADDITEM_FUNC
}    // unnamed namespace


// --- MutableGlobalDefs tests ----------------------------------------------

TEST(MutableGlobalDefsT,
     assignCartCoordinate_validCartTopoId_addsCoordinate)
{
    MutableGlobalDefs defs;

    addItem< Location >(defs, createItem< Location >(0));

    vector< const Location* > locations;
    locations.push_back(&defs.getLocation(0));
    addItem< GroupingSet >(defs, new CommLocationSet(0,
                                                     String::UNDEFINED,
                                                     Paradigm::USER,
                                                     locations));

    vector< uint32_t > ranks(1, 0);
    addItem< GroupingSet >(defs, new CommSet(1,
                                             String::UNDEFINED,
                                             GroupingSet::PROPERTY_NONE,
                                             defs.getCommLocationSet(Paradigm::USER),
                                             ranks));

    const GroupingSet& groupingSet = defs.getGroupingSet(1);
    const CommSet&     commSet     = static_cast< const CommSet& >(groupingSet);
    addItem< Communicator >(defs, new Communicator(0,
                                                   String::UNDEFINED,
                                                   commSet,
                                                   0));

    addItem< CartDimension >(defs, new CartDimension(0,
                                                     String::UNDEFINED,
                                                     3,
                                                     CartDimension::PERIODIC));
    addItem< CartDimension >(defs, new CartDimension(1,
                                                     String::UNDEFINED,
                                                     3,
                                                     CartDimension::PERIODIC));

    CartTopology::DimensionContainer dimensions;
    dimensions.push_back(&defs.getCartDimension(0));
    dimensions.push_back(&defs.getCartDimension(1));
    addItem< CartTopology >(defs, new CartTopology(0,
                                                   String::UNDEFINED,
                                                   defs.getCommunicator(0),
                                                   dimensions));

    CartCoordinate coordinate;
    coordinate.push_back(1);
    coordinate.push_back(2);
    defs.assignCartCoordinate(0, 0, coordinate);

    const CartCoordList coords = defs.getCartTopology(0).getRankCoordinates(0);
    ASSERT_EQ(1, coords.size());
    EXPECT_EQ(coordinate, coords[0]);
}


TEST(MutableGlobalDefsT,
     assignCartCoordinate_invalidCartTopoId_throws)
{
    CartCoordinate coordinate;

    MutableGlobalDefs defs;
    EXPECT_THROW(defs.assignCartCoordinate(0, 0, coordinate), RuntimeError);
}


TEST(MutableGlobalDefsT,
     assignCartCommunicator_validCartTopoId_setsCommunicator)
{
    MutableGlobalDefs defs;

    addItem< CartTopology >(defs, createItem< CartTopology >(0));

    addItem< Communicator >(defs, createItem< Communicator >(0));
    defs.assignCartCommunicator(0, defs.getCommunicator(0));

    const CartTopology& topology = defs.getCartTopology(0);
    EXPECT_EQ(&defs.getCommunicator(0), &topology.getCommunicator());
}


TEST(MutableGlobalDefsT,
     assignCartCommunicator_invalidCartTopoId_throws)
{
    CartCoordinate coordinate;

    MutableGlobalDefs defs;
    EXPECT_THROW(defs.assignCartCommunicator(0, Communicator::UNDEFINED),
                 RuntimeError);
}


TEST(MutableGlobalDefsT,
     getCommLocationSet_validParadigm_returnsSet)
{
    vector< const Location* > locations;

    CommLocationSet* comm = new CommLocationSet(0,
                                                String::UNDEFINED,
                                                Paradigm::MPI,
                                                locations);

    MutableGlobalDefs defs;
    defs.addGroupingSet(comm);

    EXPECT_EQ(comm, &defs.getCommLocationSet(Paradigm::MPI));
}


TEST(MutableGlobalDefsT,
     getCommLocationSet_invalidParadigm_throws)
{
    MutableGlobalDefs defs;

    EXPECT_THROW(defs.getCommLocationSet(Paradigm::MPI), RuntimeError);
}


TYPED_TEST(MutableGlobalDefsTypedT,
           validate_defsWithUniqueIds_doesNotThrow)
{
    MutableGlobalDefs defs;

    addItem< TypeParam >(defs, createItem< TypeParam >(7));
    addItem< TypeParam >(defs, createItem< TypeParam >(2));
    addItem< TypeParam >(defs, createItem< TypeParam >(5));
    addItem< TypeParam >(defs, createItem< TypeParam >(3));

    EXPECT_NO_THROW(defs.validate());
}


TYPED_TEST(MutableGlobalDefsTypedT,
           validate_defsWithDuplicateIds_throws)
{
    MutableGlobalDefs defs;

    addItem< TypeParam >(defs, createItem< TypeParam >(7));
    addItem< TypeParam >(defs, createItem< TypeParam >(2));
    addItem< TypeParam >(defs, createItem< TypeParam >(5));
    addItem< TypeParam >(defs, createItem< TypeParam >(2));

    EXPECT_THROW(defs.validate(), RuntimeError);
}


TEST(MutableGlobalDefsT,
     validate_defsWithUniqueCommLocationSets_doesNotThrow)
{
    vector< const Location* > locations;

    CommLocationSet* comm = new CommLocationSet(0,
                                                String::UNDEFINED,
                                                Paradigm::MPI,
                                                locations);

    MutableGlobalDefs defs;
    defs.addGroupingSet(comm);

    EXPECT_NO_THROW(defs.validate());
}


TEST(MutableGlobalDefsT,
     validate_defsWithDuplicateCommLocationSets_throws)
{
    vector< const Location* > locations;

    CommLocationSet* comm0 = new CommLocationSet(0,
                                                 String::UNDEFINED,
                                                 Paradigm::MPI,
                                                 locations);
    CommLocationSet* comm1 = new CommLocationSet(1,
                                                 String::UNDEFINED,
                                                 Paradigm::MPI,
                                                 locations);

    MutableGlobalDefs defs;
    defs.addGroupingSet(comm0);
    defs.addGroupingSet(comm1);

    EXPECT_THROW(defs.validate(), RuntimeError);
}
