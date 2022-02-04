/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "DefinitionContainer.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace pearl::detail;


// --- Helper ---------------------------------------------------------------

namespace
{
// Mocked global definition class implementing the minimal requirements for
// the DefinitionContainer class template
class TestDef
{
    public:
        typedef unsigned int IdType;

        TestDef(IdType id)
            : mId(id)
        {
        }

        IdType
        getId() const
        {
            return mId;
        }


    private:
        IdType mId;
};
}    // unnamed namespace


// --- DefinitionContainer tests --------------------------------------------

TEST(DefinitionContainerT,
     isEmpty_defaultContainer_returnsTrue)
{
    DefinitionContainer< TestDef > container;

    EXPECT_TRUE(container.isEmpty());
}


TEST(DefinitionContainerT,
     isEmpty_nonEmptyContainer_returnsFalse)
{
    DefinitionContainer< TestDef > container;

    container.addItem(new TestDef(0));
    EXPECT_FALSE(container.isEmpty());
}


TEST(DefinitionContainerT,
     numItems_defaultContainer_returnsZero)
{
    DefinitionContainer< TestDef > container;

    EXPECT_EQ(0, container.numItems());
}


TEST(DefinitionContainerT,
     getItemById_compactContainerAndValidId_returnsItem)
{
    DefinitionContainer< TestDef > compact;

    TestDef* item0 = new TestDef(0);
    TestDef* item1 = new TestDef(1);
    TestDef* item2 = new TestDef(2);
    compact.addItem(item0);
    compact.addItem(item1);
    compact.addItem(item2);

    EXPECT_EQ(item0, compact.getItemById(0));
    EXPECT_EQ(item1, compact.getItemById(1));
    EXPECT_EQ(item2, compact.getItemById(2));
}


TEST(DefinitionContainerT,
     getItemById_compactContainerAndInvalidId_throws)
{
    DefinitionContainer< TestDef > compact;

    compact.addItem(new TestDef(0));
    compact.addItem(new TestDef(1));
    compact.addItem(new TestDef(2));

    EXPECT_THROW(compact.getItemById(3), std::invalid_argument);
}


TEST(DefinitionContainerT,
     getItemById_sortedContainerAndValidId_returnsItem)
{
    DefinitionContainer< TestDef > sorted;

    TestDef* item0 = new TestDef(0);
    TestDef* item1 = new TestDef(2);
    TestDef* item2 = new TestDef(4);
    sorted.addItem(item0);
    sorted.addItem(item1);
    sorted.addItem(item2);

    EXPECT_EQ(item0, sorted.getItemById(0));
    EXPECT_EQ(item1, sorted.getItemById(2));
    EXPECT_EQ(item2, sorted.getItemById(4));
}


TEST(DefinitionContainerT,
     getItemById_sortedContainerAndInvalidId_throws)
{
    DefinitionContainer< TestDef > sorted;

    sorted.addItem(new TestDef(0));
    sorted.addItem(new TestDef(2));
    sorted.addItem(new TestDef(4));

    EXPECT_THROW(sorted.getItemById(3), std::invalid_argument);
}


TEST(DefinitionContainerT,
     getItemById_unsortedContainerAndValidId_returnsItem)
{
    DefinitionContainer< TestDef > unsorted;

    TestDef* item0 = new TestDef(3);
    TestDef* item1 = new TestDef(1);
    TestDef* item2 = new TestDef(2);
    unsorted.addItem(item0);
    unsorted.addItem(item1);
    unsorted.addItem(item2);

    EXPECT_EQ(item0, unsorted.getItemById(3));
    EXPECT_EQ(item1, unsorted.getItemById(1));
    EXPECT_EQ(item2, unsorted.getItemById(2));
}


TEST(DefinitionContainerT,
     getItemById_unsortedContainerAndInvalidId_throws)
{
    DefinitionContainer< TestDef > unsorted;

    unsorted.addItem(new TestDef(3));
    unsorted.addItem(new TestDef(1));
    unsorted.addItem(new TestDef(2));

    EXPECT_THROW(unsorted.getItemById(0), std::invalid_argument);
}


TEST(DefinitionContainerT,
     getItemByIndex_validIndex_returnsItem)
{
    DefinitionContainer< TestDef > compact;

    TestDef* item0 = new TestDef(0);
    TestDef* item1 = new TestDef(1);
    TestDef* item2 = new TestDef(2);
    compact.addItem(item0);
    compact.addItem(item1);
    compact.addItem(item2);

    EXPECT_EQ(item0, compact.getItemByIndex(0));
    EXPECT_EQ(item1, compact.getItemByIndex(1));
    EXPECT_EQ(item2, compact.getItemByIndex(2));
}


TEST(DefinitionContainerT,
     getItemByIndex_invalidIndex_throws)
{
    DefinitionContainer< TestDef > sorted;

    sorted.addItem(new TestDef(0));
    sorted.addItem(new TestDef(2));
    sorted.addItem(new TestDef(4));

    EXPECT_THROW(sorted.getItemByIndex(3), std::out_of_range);
}


TEST(DefinitionContainerT,
     addItem_itemPtr_incrementsNumItems)
{
    DefinitionContainer< TestDef > container;

    container.addItem(new TestDef(0));
    EXPECT_EQ(1, container.numItems());

    container.addItem(new TestDef(1));
    EXPECT_EQ(2, container.numItems());

    container.addItem(new TestDef(2));
    EXPECT_EQ(3, container.numItems());
}


TEST(DefinitionContainerT,
     addItem_nullptr_throws)
{
    DefinitionContainer< TestDef > container;

    EXPECT_THROW(container.addItem(0), std::invalid_argument);
}


TEST(DefinitionContainerT,
     optimizeLookups_unsortedContainer_sortsItems)
{
    DefinitionContainer< TestDef > unsorted;

    TestDef* item0 = new TestDef(3);
    TestDef* item1 = new TestDef(1);
    TestDef* item2 = new TestDef(2);
    unsorted.addItem(item0);
    unsorted.addItem(item1);
    unsorted.addItem(item2);
    unsorted.optimizeLookups();

    EXPECT_EQ(item1, unsorted.getItemByIndex(0));
    EXPECT_EQ(item2, unsorted.getItemByIndex(1));
    EXPECT_EQ(item0, unsorted.getItemByIndex(2));
    EXPECT_THROW(unsorted.getItemByIndex(3), std::out_of_range);
}
