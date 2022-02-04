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

#include <pearl/Flags.h>

#include <gtest/gtest.h>

using namespace testing;
using namespace pearl;


// --- Helpers --------------------------------------------------------------

namespace
{
class MyClass
{
    public:
        enum FlagEnum
        {
            FLAG_NONE = 0x00,
            FLAG_A    = 0x01,
            FLAG_B    = 0x02,
            FLAG_C    = 0x04,
            FLAG_D    = 0x08,
            FLAG_ALL  = FLAG_A | FLAG_B | FLAG_C | FLAG_D
        };


        typedef pearl::Flags< FlagEnum > Flags;
};


PEARL_DEFINE_FLAGS_OPERATORS(MyClass, Flags)
}    // unnamed namespace


// --- Flags tests ----------------------------------------------------------

TEST(FlagsT,
     constructor_default_initializesFlagBitsToZero)
{
    const MyClass::Flags none;
    EXPECT_EQ(0x00, none.getValue());
}


TEST(FlagsT,
     constructor_flagEnum_initializesFlagBits)
{
    const MyClass::Flags a(MyClass::FLAG_A);
    EXPECT_EQ(0x01, a.getValue());

    const MyClass::Flags d(MyClass::FLAG_D);
    EXPECT_EQ(0x08, d.getValue());

    const MyClass::Flags all(MyClass::FLAG_ALL);
    EXPECT_EQ(0x0F, all.getValue());
}


TEST(FlagsT,
     operatorAndEq_flagEnum_keepsFlagBits)
{
    MyClass::Flags b(MyClass::FLAG_ALL);
    b &= MyClass::FLAG_B;
    EXPECT_EQ(0x02, b.getValue());

    MyClass::Flags d(MyClass::FLAG_ALL);
    d &= MyClass::FLAG_D;
    EXPECT_EQ(0x08, d.getValue());

    // Calls to &= with set flags should not affect value
    d &= MyClass::FLAG_D;
    EXPECT_EQ(0x08, d.getValue());

    MyClass::Flags c(MyClass::FLAG_ALL);
    (c &= MyClass::FLAG_C) &= MyClass::FLAG_C;
    EXPECT_EQ(0x04, c.getValue());

    MyClass::Flags none(MyClass::FLAG_ALL);
    (none &= MyClass::FLAG_A) &= MyClass::FLAG_C;
    EXPECT_EQ(0x00, none.getValue());
}


TEST(FlagsT,
     operatorAndEq_flagsItem_keepsFlagBits)
{
    const MyClass::Flags flagA(MyClass::FLAG_A);
    const MyClass::Flags flagB(MyClass::FLAG_B);
    const MyClass::Flags flagC(MyClass::FLAG_C);

    MyClass::Flags c(MyClass::FLAG_ALL);
    c &= flagC;
    EXPECT_EQ(0x04, c.getValue());

    MyClass::Flags b(MyClass::FLAG_ALL);
    b &= flagB;
    EXPECT_EQ(0x02, b.getValue());

    // Calls to &= with set flags should not affect value
    b &= b;
    EXPECT_EQ(0x02, b.getValue());

    MyClass::Flags ad(MyClass::FLAG_ALL);
    ad &= MyClass::FLAG_A | MyClass::FLAG_D;
    EXPECT_EQ(0x09, ad.getValue());

    MyClass::Flags a(MyClass::FLAG_ALL);
    (a &= MyClass::FLAG_A | MyClass::FLAG_C) &= flagA;
    EXPECT_EQ(0x01, a.getValue());
}


TEST(FlagsT,
     operatorOrEq_flagEnum_setsFlagBits)
{
    MyClass::Flags item;
    item |= MyClass::FLAG_B;
    EXPECT_EQ(0x02, item.getValue());

    item |= MyClass::FLAG_C;
    EXPECT_EQ(0x06, item.getValue());

    // Calls to |= with set flags should not affect value
    item |= MyClass::FLAG_C;
    EXPECT_EQ(0x06, item.getValue());

    (item |= MyClass::FLAG_A) |= MyClass::FLAG_D;
    EXPECT_EQ(0x0F, item.getValue());
}


TEST(FlagsT,
     operatorOrEq_flagsItem_setsFlagBits)
{
    const MyClass::Flags flagB(MyClass::FLAG_B);
    const MyClass::Flags flagC(MyClass::FLAG_C);
    const MyClass::Flags flagD(MyClass::FLAG_D);

    MyClass::Flags item;
    item |= flagC;
    EXPECT_EQ(0x04, item.getValue());

    item |= MyClass::FLAG_A | MyClass::FLAG_B;
    EXPECT_EQ(0x07, item.getValue());

    // Calls to |= with enabled flags should not affect value
    item |= flagC;
    EXPECT_EQ(0x07, item.getValue());

    (item |= flagB) |= flagD;
    EXPECT_EQ(0x0F, item.getValue());
}


TEST(FlagsT,
     set_flagEnum_setsFlagBits)
{
    MyClass::Flags item;
    item.set(MyClass::FLAG_A);
    EXPECT_EQ(0x01, item.getValue());

    item.set(MyClass::FLAG_C);
    EXPECT_EQ(0x05, item.getValue());

    // Calls to set() with set flags should not affect value
    item.set(MyClass::FLAG_C);
    EXPECT_EQ(0x05, item.getValue());

    item.set(MyClass::FLAG_B).set(MyClass::FLAG_D);
    EXPECT_EQ(0x0F, item.getValue());
}


TEST(FlagsT,
     set_flagsItem_setsFlagBits)
{
    const MyClass::Flags flagB(MyClass::FLAG_B);
    const MyClass::Flags flagD(MyClass::FLAG_D);

    MyClass::Flags item;
    item.set(flagB);
    EXPECT_EQ(0x02, item.getValue());

    item.set(MyClass::FLAG_A | MyClass::FLAG_C);
    EXPECT_EQ(0x07, item.getValue());

    // Calls to set() with set flags should not affect value
    item.set(flagB);
    EXPECT_EQ(0x07, item.getValue());

    item.set(flagB).set(flagD);
    EXPECT_EQ(0x0F, item.getValue());
}


TEST(FlagsT,
     unset_flagEnum_unsetsFlagBits)
{
    MyClass::Flags item(MyClass::FLAG_ALL);
    item.unset(MyClass::FLAG_A);
    EXPECT_EQ(0x0E, item.getValue());

    // Calls to unset() with unset flags should not affect value
    item.unset(MyClass::FLAG_A);
    EXPECT_EQ(0x0E, item.getValue());

    item.unset(MyClass::FLAG_D);
    EXPECT_EQ(0x06, item.getValue());

    item.unset(MyClass::FLAG_C).unset(MyClass::FLAG_B);
    EXPECT_EQ(0x00, item.getValue());
}


TEST(FlagsT,
     unset_flagsItem_unsetsFlagBits)
{
    const MyClass::Flags flagA(MyClass::FLAG_A);
    const MyClass::Flags flagB(MyClass::FLAG_B);
    const MyClass::Flags flagD(MyClass::FLAG_D);

    MyClass::Flags item(MyClass::FLAG_ALL);
    item.unset(flagA);
    EXPECT_EQ(0x0E, item.getValue());

    // Calls to unset() with unset flags should not affect value
    item.unset(flagA);
    EXPECT_EQ(0x0E, item.getValue());

    item.unset(MyClass::FLAG_C | MyClass::FLAG_B);
    EXPECT_EQ(0x08, item.getValue());

    item.unset(flagB).unset(flagD);
    EXPECT_EQ(0x00, item.getValue());
}


TEST(FlagsT,
     test_flagEnum_returnsStatus)
{
    MyClass::Flags item;
    EXPECT_FALSE(item.test(MyClass::FLAG_A));
    EXPECT_FALSE(item.test(MyClass::FLAG_B));
    EXPECT_FALSE(item.test(MyClass::FLAG_C));
    EXPECT_FALSE(item.test(MyClass::FLAG_D));
    EXPECT_TRUE(item.test(MyClass::FLAG_NONE));

    item.set(MyClass::FLAG_A);
    EXPECT_TRUE(item.test(MyClass::FLAG_A));
    EXPECT_FALSE(item.test(MyClass::FLAG_B));
    EXPECT_FALSE(item.test(MyClass::FLAG_C));
    EXPECT_FALSE(item.test(MyClass::FLAG_D));
    EXPECT_FALSE(item.test(MyClass::FLAG_NONE));

    item.set(MyClass::FLAG_C);
    EXPECT_TRUE(item.test(MyClass::FLAG_A));
    EXPECT_FALSE(item.test(MyClass::FLAG_B));
    EXPECT_TRUE(item.test(MyClass::FLAG_C));
    EXPECT_FALSE(item.test(MyClass::FLAG_D));
    EXPECT_FALSE(item.test(MyClass::FLAG_NONE));
}


TEST(FlagsT,
     test_flagsItem_returnsStatus)
{
    const MyClass::Flags none;
    const MyClass::Flags flagA(MyClass::FLAG_A);
    const MyClass::Flags flagB(MyClass::FLAG_B);
    const MyClass::Flags flagC(MyClass::FLAG_C);
    const MyClass::Flags flagD(MyClass::FLAG_D);

    MyClass::Flags item;
    EXPECT_FALSE(item.test(flagA));
    EXPECT_FALSE(item.test(flagB));
    EXPECT_FALSE(item.test(flagC));
    EXPECT_FALSE(item.test(flagD));
    EXPECT_TRUE(item.test(none));

    item.set(MyClass::FLAG_B);
    EXPECT_FALSE(item.test(flagA));
    EXPECT_TRUE(item.test(flagB));
    EXPECT_FALSE(item.test(flagC));
    EXPECT_FALSE(item.test(flagD));
    EXPECT_FALSE(item.test(none));

    item.set(MyClass::FLAG_D);
    EXPECT_FALSE(item.test(flagA));
    EXPECT_TRUE(item.test(flagB));
    EXPECT_FALSE(item.test(flagC));
    EXPECT_TRUE(item.test(flagD));
    EXPECT_TRUE(item.test(MyClass::FLAG_B | MyClass::FLAG_D));
    EXPECT_FALSE(item.test(none));
}


TEST(FlagsT,
     operatorEquals_equalItems_returnsTrue)
{
    const MyClass::Flags none;
    const MyClass::Flags flagC(MyClass::FLAG_C);

    MyClass::Flags item;
    EXPECT_TRUE(item == none);

    item.set(MyClass::FLAG_C);
    EXPECT_TRUE(item == flagC);

    item.set(MyClass::FLAG_A);
    EXPECT_TRUE(item == (MyClass::FLAG_A | MyClass::FLAG_C));
}


TEST(FlagsT,
     operatorEquals_unequalItems_returnsFalse)
{
    const MyClass::Flags flagA(MyClass::FLAG_A);
    const MyClass::Flags flagB(MyClass::FLAG_B);

    MyClass::Flags item;
    EXPECT_FALSE(item == flagA);

    item.set(MyClass::FLAG_C);
    EXPECT_FALSE(item == flagB);

    item.set(MyClass::FLAG_A);
    EXPECT_FALSE(item == (MyClass::FLAG_B | MyClass::FLAG_C));
}


TEST(FlagsT,
     operatorNotEquals_equalItems_returnsFalse)
{
    const MyClass::Flags none;
    const MyClass::Flags flagC(MyClass::FLAG_C);

    MyClass::Flags item;
    EXPECT_FALSE(item != none);

    item.set(MyClass::FLAG_C);
    EXPECT_FALSE(item != flagC);

    item.set(MyClass::FLAG_A);
    EXPECT_FALSE(item != (MyClass::FLAG_A | MyClass::FLAG_C));
}


TEST(FlagsT,
     operatorNotEquals_unequalItems_returnsTrue)
{
    const MyClass::Flags flagA(MyClass::FLAG_A);
    const MyClass::Flags flagB(MyClass::FLAG_B);

    MyClass::Flags item;
    EXPECT_TRUE(item != flagA);

    item.set(MyClass::FLAG_C);
    EXPECT_TRUE(item != flagB);

    item.set(MyClass::FLAG_A);
    EXPECT_TRUE(item != (MyClass::FLAG_B | MyClass::FLAG_C));
}


TEST(FlagsT,
     operatorAnd_flagsItemAndFlagEnum_returnsIntersection)
{
    const MyClass::Flags all(MyClass::FLAG_ALL);

    MyClass::Flags a;
    a = all & MyClass::FLAG_A;
    EXPECT_EQ(0x01, a.getValue());

    MyClass::Flags c;
    c = all & MyClass::FLAG_C;
    EXPECT_EQ(0x04, c.getValue());
}


TEST(FlagsT,
     operatorAnd_flagEnumAndFlagsItem_returnsIntersection)
{
    const MyClass::Flags all(MyClass::FLAG_ALL);

    MyClass::Flags b;
    b = MyClass::FLAG_B & all;
    EXPECT_EQ(0x02, b.getValue());

    MyClass::Flags d;
    d = MyClass::FLAG_D & all;
    EXPECT_EQ(0x08, d.getValue());
}


TEST(FlagsT,
     operatorAnd_twoFlagsItems_returnsIntersection)
{
    const MyClass::Flags all(MyClass::FLAG_ALL);
    const MyClass::Flags flagA(MyClass::FLAG_A);
    const MyClass::Flags flagC(MyClass::FLAG_C);

    MyClass::Flags a;
    a = flagA & all;
    EXPECT_EQ(0x01, a.getValue());

    MyClass::Flags c;
    c = flagC & all;
    EXPECT_EQ(0x04, c.getValue());

    MyClass::Flags bc;
    bc = (MyClass::FLAG_B | MyClass::FLAG_C) & all;
    EXPECT_EQ(0x06, bc.getValue());
}


TEST(FlagsT,
     operatorOr_twoFlagEnums_returnsUnion)
{
    MyClass::Flags item;
    item = MyClass::FLAG_B | MyClass::FLAG_C;
    EXPECT_EQ(0x06, item.getValue());
}


TEST(FlagsT,
     operatorOr_flagsItemAndFlagEnum_returnsUnion)
{
    const MyClass::Flags lhs(MyClass::FLAG_A);

    MyClass::Flags item;
    item = lhs | MyClass::FLAG_B;
    EXPECT_EQ(0x03, item.getValue());
}


TEST(FlagsT,
     operatorOr_flagEnumAndFlagsItem_returnsUnion)
{
    const MyClass::Flags rhs(MyClass::FLAG_D);

    MyClass::Flags item;
    item = MyClass::FLAG_B | rhs;
    EXPECT_EQ(0x0A, item.getValue());
}


TEST(FlagsT,
     operatorOr_twoFlagsItems_returnsUnion)
{
    const MyClass::Flags lhs(MyClass::FLAG_A);
    const MyClass::Flags rhs(MyClass::FLAG_C | MyClass::FLAG_B);

    MyClass::Flags item;
    item = lhs | rhs;
    EXPECT_EQ(0x07, item.getValue());
}
