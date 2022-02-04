/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "SystemTree.h"

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include <pearl/Location.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>

#include "DefinitionContainer.h"
#include "Process.h"
#include "UnknownLocationGroup.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Test fixture for system tree tests
class SystemTreeT
    : public Test
{
    public:
        const SystemNode&
        createSystemTree();


    protected:
        DefinitionContainer< String >        mStrings;
        DefinitionContainer< Location >      mLocations;
        DefinitionContainer< LocationGroup > mLocationGroups;
        DefinitionContainer< SystemNode >    mSystemNodes;
};
}    // unnamed namespace


// --- print tests (system trees) -------------------------------------------

TEST_F(SystemTreeT,
       print_undefinedLocation_printsUndefined)
{
    string expected(" + (undefined)\n");

    ostringstream result;
    print(result, Location::UNDEFINED);
    EXPECT_EQ(expected, result.str());

    expected = "# + (undefined)\n";

    result.str("");
    print(result, Location::UNDEFINED, "#");
    EXPECT_EQ(expected, result.str());
}


TEST_F(SystemTreeT,
       print_systemTree_printsTree)
{
    const SystemNode& tree = createSystemTree();

    string expected(" + Machine  <sn:0>\n"
                    "    + Node 0  <sn:1>\n"
                    "    |  + Rank 0  <lg:0>\n"
                    "    |     + Master thread  <l:0>\n"
                    "    |     + GPU stream  <l:1>\n"
                    "    + Switch  <sn:2>\n"
                    "       + Node 1  <sn:3>\n"
                    "       |  + Rank 1  <lg:2>\n"
                    "       |     + Master thread  <l:3>\n"
                    "       + Switch metric  <lg:1>\n"
                    "          + Metric  <l:2>\n");

    ostringstream result;
    print(result, tree, true);
    EXPECT_EQ(expected, result.str());
}


TEST_F(SystemTreeT,
       print_systemTreeLastChild_printsTreeWithPrefix)
{
    const SystemNode& tree = createSystemTree();

    string expected(" + Machine  <sn:0>\n"
                    " |  + Node 0  <sn:1>\n"
                    " |  |  + Rank 0  <lg:0>\n"
                    " |  |     + Master thread  <l:0>\n"
                    " |  |     + GPU stream  <l:1>\n"
                    " |  + Switch  <sn:2>\n"
                    " |     + Node 1  <sn:3>\n"
                    " |     |  + Rank 1  <lg:2>\n"
                    " |     |     + Master thread  <l:3>\n"
                    " |     + Switch metric  <lg:1>\n"
                    " |        + Metric  <l:2>\n");

    ostringstream result;
    print(result, tree, false);
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

// Creates a system tree covering all possible situations w.r.t. the possible
// node types:
//
//    + Machine  <sn:0>
//       + Node 0  <sn:1>
//       |  + Rank 0  <lg:0>            # LG w/ different types of L
//       |     + Master thread  <l:0>
//       |     + GPU stream  <l:1>
//       + Switch  <sn:2>               # STN w/ child STN and LG
//          + Node 1  <sn:3>
//          |  + Rank 1  <lg:2>
//          |     + Master thread  <l:3>
//          + Switch metric  <lg:1>
//             + Metric  <l:2>
//
const SystemNode&
SystemTreeT::createSystemTree()
{
    // Create String definitions
    String* strMachine = new String(0, "Machine");
    mStrings.addItem(strMachine);

    String* strNode0 = new String(1, "Node 0");
    mStrings.addItem(strNode0);

    String* strNode1 = new String(2, "Node 1");
    mStrings.addItem(strNode1);

    String* strSwitch = new String(3, "Switch");
    mStrings.addItem(strSwitch);

    String* strSwitchMetric = new String(4, "Switch metric");
    mStrings.addItem(strSwitchMetric);

    String* strRank0 = new String(5, "Rank 0");
    mStrings.addItem(strRank0);

    String* strRank1 = new String(6, "Rank 1");
    mStrings.addItem(strRank1);

    String* strThread = new String(7, "Master thread");
    mStrings.addItem(strThread);

    String* strStream = new String(8, "GPU stream");
    mStrings.addItem(strStream);

    String* strMetric = new String(9, "Metric");
    mStrings.addItem(strMetric);

    // Create SystemNode definitions
    SystemNode* snMachine =
        new SystemNode(0,
                       *strMachine,
                       String::UNDEFINED,
                       0);
    mSystemNodes.addItem(snMachine);

    SystemNode* snNode0 =
        new SystemNode(1,
                       *strNode0,
                       String::UNDEFINED,
                       snMachine);
    mSystemNodes.addItem(snNode0);

    SystemNode* snSwitch =
        new SystemNode(2,
                       *strSwitch,
                       String::UNDEFINED,
                       snMachine);
    mSystemNodes.addItem(snSwitch);

    SystemNode* snNode1 =
        new SystemNode(3,
                       *strNode1,
                       String::UNDEFINED,
                       snSwitch);
    mSystemNodes.addItem(snNode1);

    // Create LocationGroup definitions
    LocationGroup* lgRank0 =
        new Process(0,
                    *strRank0,
                    snNode0);
    mLocationGroups.addItem(lgRank0);

    LocationGroup* lgSwitchMetric =
        new UnknownLocationGroup(1,
                                 *strSwitchMetric,
                                 snSwitch);
    mLocationGroups.addItem(lgSwitchMetric);

    LocationGroup* lgRank1 =
        new Process(2,
                    *strRank1,
                    snNode1);
    mLocationGroups.addItem(lgRank1);

    // Create Location definitions
    Location* locThread0 =
        new Location(0,
                     *strThread,
                     Location::TYPE_CPU_THREAD,
                     0,
                     lgRank0);
    mLocations.addItem(locThread0);

    Location* locStream0 =
        new Location(1,
                     *strStream,
                     Location::TYPE_GPU,
                     0,
                     lgRank0);
    mLocations.addItem(locStream0);

    Location* locMetric =
        new Location(2,
                     *strMetric,
                     Location::TYPE_METRIC,
                     0,
                     lgSwitchMetric);
    mLocations.addItem(locMetric);

    Location* locThread1 =
        new Location(3,
                     *strThread,
                     Location::TYPE_CPU_THREAD,
                     0,
                     lgRank1);
    mLocations.addItem(locThread1);

    return *snMachine;
}
