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

#include <pearl/ScopedPtr.h>

#include <gtest/gtest.h>

using namespace testing;
using namespace pearl;


// --- Helpers --------------------------------------------------------------

namespace
{
// Global instance counter for Item objects
int itemCount = 0;

// Helper class modifying global instance counter on construction/destruction
class Item
{
    public:
        Item()
        {
            ++itemCount;
        }

        ~Item()
        {
            --itemCount;
        }
};


// Test fixture ensuring test isolation by resetting global instance counter
class ScopedPtrT
    : public Test
{
    public:
        ScopedPtrT()
        {
            itemCount = 0;
        }
};
}    // unnamed namespace


// --- ScopedPtr tests ------------------------------------------------------

TEST_F(ScopedPtrT,
       destructor_itemPtr_deletesPointee)
{
    {
        ScopedPtr< Item > ptr(new Item);
        EXPECT_EQ(1, itemCount);
    }
    EXPECT_EQ(0, itemCount);
}


TEST_F(ScopedPtrT,
       get_emptyPtr_returnsNullptr)
{
    ScopedPtr< int > null;
    EXPECT_TRUE(0 == null.get());
}


TEST_F(ScopedPtrT,
       get_itemPtr_returnsManagedPtr)
{
    int* item = new int(42);

    ScopedPtr< int > ptr(item);
    EXPECT_EQ(item, ptr.get());
}


TEST_F(ScopedPtrT,
       release_emptyPtr_returnsNullptr)
{
    ScopedPtr< int > null;
    EXPECT_TRUE(0 == null.release());
}


TEST_F(ScopedPtrT,
       release_itemPtr_returnsManagedPtr)
{
    int* item = new int(42);

    ScopedPtr< int > ptr(item);
    EXPECT_EQ(item, ptr.release());

    delete item;
}


TEST_F(ScopedPtrT,
       release_itemPtr_setsNullptr)
{
    int* item = new int(42);

    ScopedPtr< int > ptr(item);
    ptr.release();
    EXPECT_TRUE(0 == ptr.get());

    delete item;
}


TEST_F(ScopedPtrT,
       indirectionOp_itemPtr_returnsPointee)
{
    ScopedPtr< int > ptr(new int(42));
    EXPECT_EQ(42, *ptr);
}


TEST_F(ScopedPtrT,
       memberOfPtrOp_itemPtr_returnsManagedPtr)
{
    int* item = new int(42);

    ScopedPtr< int > ptr(item);
    EXPECT_EQ(item, ptr.operator->());
}


TEST_F(ScopedPtrT,
       reset_defaultPtr_setsNullptr)
{
    ScopedPtr< int > ptr(new int(42));
    ptr.reset();
    EXPECT_TRUE(0 == ptr.get());
}


TEST_F(ScopedPtrT,
       reset_defaultPtr_deletesPointee)
{
    ScopedPtr< Item > ptr(new Item);
    EXPECT_EQ(1, itemCount);
    ptr.reset();
    EXPECT_EQ(0, itemCount);
}


TEST_F(ScopedPtrT,
       reset_itemPtr_updatesManagedPtr)
{
    ScopedPtr< int > ptr(new int(42));

    int* item = new int(24);
    ptr.reset(item);
    EXPECT_EQ(item, ptr.get());
}


TEST_F(ScopedPtrT,
       reset_itemPtr_deletesPointee)
{
    ScopedPtr< Item > ptr(new Item);
    EXPECT_EQ(1, itemCount);
    Item* item = new Item;
    EXPECT_EQ(2, itemCount);
    ptr.reset(item);
    EXPECT_EQ(1, itemCount);
}


TEST_F(ScopedPtrT,
       swap_itemPtr_swapsManagedPtrs)
{
    int* aItem = new int(42);
    int* bItem = new int(24);

    ScopedPtr< int > a(aItem);
    ScopedPtr< int > b(bItem);
    a.swap(b);
    EXPECT_EQ(bItem, a.get());
    EXPECT_EQ(aItem, b.get());
}


TEST_F(ScopedPtrT,
       swap_twoScopedPtrs_swapsManagedPtrs)
{
    int* aItem = new int(42);
    int* bItem = new int(24);

    ScopedPtr< int > a(aItem);
    ScopedPtr< int > b(bItem);
    swap(a, b);
    EXPECT_EQ(bItem, a.get());
    EXPECT_EQ(aItem, b.get());
}


TEST_F(ScopedPtrT,
       operatorBoolType_emptyPtr_returnsFalse)
{
    ScopedPtr< int > null;
    ASSERT_FALSE(null);
}


TEST_F(ScopedPtrT,
       operatorBoolType_itemPtr_returnsTrue)
{
    ScopedPtr< int > ptr(new int(42));
    ASSERT_TRUE(ptr);
}
