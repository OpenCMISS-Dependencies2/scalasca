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

#include "DefinitionTree.h"

#include <sstream>
#include <string>

#include <gtest/gtest.h>

using namespace std;
using namespace testing;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Mocked global definition class implementing the minimal requirements for
// the DefinitionTree class template
class TestDef
{
    public:
        typedef unsigned int IdType;


        TestDef(const IdType   id,
                TestDef* const parent)
            : mId(id),
              mParent(parent)
        {
        }

        IdType
        getId() const
        {
            return mId;
        }

        TestDef*
        getParent() const
        {
            return mParent;
        }


    private:
        IdType   mId;
        TestDef* mParent;
};


ostream&
print(ostream&       stream,
      const TestDef& item,
      const bool     isLastChild)
{
    return stream << (isLastChild ? "   " : " + ")
                  << "Definition "
                  << item.getId()
                  << endl;
}
}    // unnamed namespace


// --- DefinitionTree tests -------------------------------------------------

//  NOTE:
//      Member functions that only delegate their work to the corresponding
//      members of the DefinitionContainer class template are not tested.
//      These should be sufficiently covered by the DefinitionContainer tests.


TEST(DefinitionTreeT,
     numRootItems_defaultTree_returnsZero)
{
    DefinitionTree< TestDef > tree;

    EXPECT_EQ(0, tree.numRootItems());
}


TEST(DefinitionTreeT,
     getRootItemByIndex_validIndex_returnsItem)
{
    DefinitionTree< TestDef > tree;

    TestDef* item0 = new TestDef(0, 0);
    TestDef* item1 = new TestDef(1, item0);
    TestDef* item2 = new TestDef(2, 0);
    tree.addItem(item0);
    tree.addItem(item1);
    tree.addItem(item2);

    EXPECT_EQ(item0, tree.getRootItemByIndex(0));
    EXPECT_EQ(item2, tree.getRootItemByIndex(1));
}


TEST(DefinitionTreeT,
     getRootItemByIndex_invalidIndex_throws)
{
    DefinitionTree< TestDef > tree;

    TestDef* item0 = new TestDef(0, 0);
    TestDef* item1 = new TestDef(1, item0);
    TestDef* item2 = new TestDef(2, 0);
    tree.addItem(item0);
    tree.addItem(item1);
    tree.addItem(item2);

    EXPECT_THROW(tree.getRootItemByIndex(2), std::out_of_range);
}


TEST(DefinitionTreeT,
     addItem_itemPtr_incrementsNumItems)
{
    DefinitionTree< TestDef > tree;

    TestDef* root = new TestDef(0, 0);
    tree.addItem(root);
    EXPECT_EQ(1, tree.numItems());

    tree.addItem(new TestDef(1, root));
    EXPECT_EQ(2, tree.numItems());

    tree.addItem(new TestDef(2, root));
    EXPECT_EQ(3, tree.numItems());
}


TEST(DefinitionTreeT,
     addItem_itemPtr_incrementsNumRootItems)
{
    DefinitionTree< TestDef > tree;

    TestDef* root = new TestDef(0, 0);
    tree.addItem(root);
    EXPECT_EQ(1, tree.numRootItems());

    tree.addItem(new TestDef(1, root));
    EXPECT_EQ(1, tree.numRootItems());

    tree.addItem(new TestDef(2, 0));
    EXPECT_EQ(2, tree.numRootItems());
}


TEST(DefinitionTreeT,
     addItem_nullptr_throws)
{
    DefinitionTree< TestDef > tree;

    EXPECT_THROW(tree.addItem(0), std::invalid_argument);
}


TEST(DefinitionTreeT,
     optimizeLookups_treeWithUnsortedIds_keepsRootItems)
{
    DefinitionTree< TestDef > tree;

    TestDef* root0 = new TestDef(42, 0);
    tree.addItem(root0);

    TestDef* root1 = new TestDef(0, 0);
    tree.addItem(root1);

    TestDef* root2 = new TestDef(24, 0);
    tree.addItem(root2);

    EXPECT_EQ(root0, tree.getRootItemByIndex(0));
    EXPECT_EQ(root1, tree.getRootItemByIndex(1));
    EXPECT_EQ(root2, tree.getRootItemByIndex(2));
    EXPECT_THROW(tree.getRootItemByIndex(3), std::out_of_range);
}


TEST(DefinitionTreeT,
     operatorLeftShift_defaultTreeToStream_printsNothing)
{
    DefinitionTree< TestDef > tree;

    string expected("");

    ostringstream result;
    result << tree;
    EXPECT_EQ(expected, result.str());
}


TEST(DefinitionTreeT,
     operatorLeftShift_nonEmptyTreeToStream_printsRootItems)
{
    DefinitionTree< TestDef > tree;

    TestDef* item0 = new TestDef(0, 0);
    tree.addItem(item0);

    TestDef* item1 = new TestDef(1, item0);
    tree.addItem(item1);

    TestDef* item2 = new TestDef(2, 0);
    tree.addItem(item2);

    TestDef* item3 = new TestDef(3, item2);
    tree.addItem(item3);

    TestDef* item4 = new TestDef(4, 0);
    tree.addItem(item4);

    TestDef* item5 = new TestDef(5, item4);
    tree.addItem(item5);


    string expected(" + Definition 0\n"
                    " + Definition 2\n"
                    "   Definition 4\n");

    ostringstream result;
    result << tree;
    EXPECT_EQ(expected, result.str());
}
