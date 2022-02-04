/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "ContextTree.h"

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include <pearl/CallingContext.h>
#include <pearl/Region.h>
#include <pearl/SourceLocation.h>
#include <pearl/String.h>

#include "DefinitionContainer.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Test fixture for calling context tree tests
class ContextTreeT
    : public Test
{
    public:
        const CallingContext&
        createContextTree();


    protected:
        DefinitionContainer< String >         mStrings;
        DefinitionContainer< Region >         mRegions;
        DefinitionContainer< SourceLocation > mSourceLocs;
        DefinitionContainer< CallingContext > mContexts;
};
}    // unnamed namespace


// --- print tests (context trees) ------------------------------------------

TEST_F(ContextTreeT,
       print_undefinedContext_printsUndefined)
{
    string expected(" + (undefined)\n");

    ostringstream result;
    print(result, CallingContext::UNDEFINED, true);
    EXPECT_EQ(expected, result.str());

    expected = "# + (undefined)\n";

    result.str("");
    print(result, CallingContext::UNDEFINED, true, "#");
    EXPECT_EQ(expected, result.str());
}


TEST_F(ContextTreeT,
       print_contextTreeLastChild_printsTree)
{
    const CallingContext& tree = createContextTree();

    string expected(" + main @ test.c:24  <0>\n"
                    "    + foo @ test.c:19  <1>\n"
                    "    |  + bar @ test.c:14  <2>\n"
                    "    + foo @ test.c  <3>\n"
                    "       + bar  <4>\n");

    ostringstream result;
    print(result, tree, true);
    EXPECT_EQ(expected, result.str());
}


TEST_F(ContextTreeT,
       print_contextTree_printsTreeWithPrefix)
{
    const CallingContext& tree = createContextTree();

    string expected(" + main @ test.c:24  <0>\n"
                    " |  + foo @ test.c:19  <1>\n"
                    " |  |  + bar @ test.c:14  <2>\n"
                    " |  + foo @ test.c  <3>\n"
                    " |     + bar  <4>\n");

    ostringstream result;
    print(result, tree, false);
    EXPECT_EQ(expected, result.str());
}


// --- Helper ---------------------------------------------------------------

// Creates a calling context tree covering all possible situations w.r.t.
// the available information:
//
//    + main @ test.c:24  <0>           # Full info
//       + foo @ test.c:19  <1>         # Full info
//       |  + bar @ test.c:14  <2>      # No file info (region)
//       + foo @ test.c:19  <3>         # No source info (calling context)
//          + bar  <4>                  # Neither file info (region) nor source
//                                      # info (calling context)
//
const CallingContext&
ContextTreeT::createContextTree()
{
    // Create String definitions
    String* strEmpty = new String(0, "");
    mStrings.addItem(strEmpty);

    String* strSource = new String(1, "test.c");
    mStrings.addItem(strSource);

    String* strMain = new String(2, "main");
    mStrings.addItem(strMain);

    String* strFoo = new String(3, "foo");
    mStrings.addItem(strFoo);

    String* strBar = new String(4, "bar");
    mStrings.addItem(strBar);

    // Create Region definitions
    Region* regMain = new Region(0,
                                 *strMain,
                                 *strMain,
                                 *strEmpty,
                                 Region::ROLE_FUNCTION,
                                 Paradigm::SAMPLING,
                                 *strSource,
                                 23, 25);
    mRegions.addItem(regMain);

    Region* regFoo = new Region(1,
                                *strFoo,
                                *strFoo,
                                *strEmpty,
                                Region::ROLE_FUNCTION,
                                Paradigm::SAMPLING,
                                *strSource,
                                18, 20);
    mRegions.addItem(regFoo);

    // No file info
    Region* regBar = new Region(2,
                                *strBar,
                                *strBar,
                                *strEmpty,
                                Region::ROLE_FUNCTION,
                                Paradigm::SAMPLING,
                                String::UNDEFINED,
                                13, 15);
    mRegions.addItem(regBar);

    // Create SourceLocation definitions
    SourceLocation* sclMain = new SourceLocation(0, *strSource, 24);
    mSourceLocs.addItem(sclMain);

    SourceLocation* sclFoo = new SourceLocation(1, *strSource, 19);
    mSourceLocs.addItem(sclFoo);

    SourceLocation* sclBar = new SourceLocation(2, *strSource, 14);
    mSourceLocs.addItem(sclBar);

    // Create CallingContext definitions
    CallingContext* ccMain = new CallingContext(0,
                                                *regMain,
                                                *sclMain,
                                                0);
    mContexts.addItem(ccMain);

    CallingContext* ccMainFoo0 = new CallingContext(1,
                                                    *regFoo,
                                                    *sclFoo,
                                                    ccMain);
    mContexts.addItem(ccMainFoo0);

    // No file info
    CallingContext* ccMainFoo0Bar = new CallingContext(2,
                                                       *regBar,
                                                       *sclBar,
                                                       ccMainFoo0);
    mContexts.addItem(ccMainFoo0Bar);

    // No source location info
    CallingContext* ccMainFoo1 = new CallingContext(3,
                                                    *regFoo,
                                                    SourceLocation::UNDEFINED,
                                                    ccMain);
    mContexts.addItem(ccMainFoo1);

    // No file & no source location info
    CallingContext* ccMainFoo1Bar = new CallingContext(4,
                                                       *regBar,
                                                       SourceLocation::UNDEFINED,
                                                       ccMainFoo1);
    mContexts.addItem(ccMainFoo1Bar);

    return *ccMain;
}
