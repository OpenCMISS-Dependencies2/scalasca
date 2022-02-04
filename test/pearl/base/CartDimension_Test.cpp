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

#include <pearl/CartDimension.h>

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include <pearl/String.h>
#include <pearl/iomanip.h>

#include "iomanip_detail.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


// --- CartDimension tests --------------------------------------------------

TEST(CartDimensionT,
     getId_item_returnsIdentifier)
{
    CartDimension item0(0, String::UNDEFINED, 0, CartDimension::PERIODIC);
    EXPECT_EQ(0, item0.getId());

    CartDimension item1(1, String::UNDEFINED, 0, CartDimension::PERIODIC);
    EXPECT_EQ(1, item1.getId());

    CartDimension item42(42, String::UNDEFINED, 0, CartDimension::PERIODIC);
    EXPECT_EQ(42, item42.getId());
}


TEST(CartDimensionT,
     getName_item_returnsName)
{
    CartDimension item0(0, String::UNDEFINED, 0, CartDimension::PERIODIC);
    EXPECT_EQ(&String::UNDEFINED, &item0.getName());

    String        name1(0, "Foo");
    CartDimension item1(1, name1, 0, CartDimension::PERIODIC);
    EXPECT_EQ(&name1, &item1.getName());
}


TEST(CartDimensionT,
     getSize_item_returnsSize)
{
    CartDimension item0(0, String::UNDEFINED, 2, CartDimension::PERIODIC);
    EXPECT_EQ(2, item0.getSize());

    CartDimension item1(1, String::UNDEFINED, 3, CartDimension::PERIODIC);
    EXPECT_EQ(3, item1.getSize());

    CartDimension item2(2, String::UNDEFINED, 4, CartDimension::PERIODIC);
    EXPECT_EQ(4, item2.getSize());
}


TEST(CartDimensionT,
     isPeriodic_item_returnsPeriodicity)
{
    CartDimension periodic(0, String::UNDEFINED, 1, CartDimension::PERIODIC);
    EXPECT_TRUE(periodic.isPeriodic());

    CartDimension nonPeriodic(0, String::UNDEFINED, 1,
                              CartDimension::NON_PERIODIC);
    EXPECT_FALSE(nonPeriodic.isPeriodic());
}


TEST(CartDimensionT,
     operatorLeftShift_itemAtSkipDetail_printsNothing)
{
    String        name(24, "Periodic");
    CartDimension item(42, name, 2, CartDimension::PERIODIC);

    string expected;

    ostringstream result;
    result << setDetail(DETAIL_SKIP)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST(CartDimensionT,
     operatorLeftShift_itemAtAbbrevDetail_printsIdentifier)
{
    String        name(24, "Periodic");
    CartDimension item(42, name, 2, CartDimension::PERIODIC);

    string expected("42");

    ostringstream result;
    result << setDetail(DETAIL_ABBRV)
           << item;
    EXPECT_EQ(expected, result.str());
}


TEST(CartDimensionT,
     operatorLeftShift_itemAtCompactDetail_printsOneliner)
{
    {
        String        name(24, "Periodic");
        CartDimension item(42, name, 2, CartDimension::PERIODIC);

        string expected("CartDimension { id: 42, name: \"Periodic\", size: 2, periodic: true }");

        ostringstream result;
        result << item;
        EXPECT_EQ(expected, result.str());
    }

    {
        String        name(42, "Non-periodic");
        CartDimension item(24, name, 3, CartDimension::NON_PERIODIC);

        string expected("CartDimension { id: 24, name: \"Non-periodic\", size: 3, periodic: false }");

        ostringstream result;
        result << item;
        EXPECT_EQ(expected, result.str());
    }
}


TEST(CartDimensionT,
     operatorLeftShift_itemAtFullDetail_printsData)
{
    {
        String        name(24, "Periodic");
        CartDimension item(42, name, 2, CartDimension::PERIODIC);

        string expected("CartDimension {\n"
                        "    id: 42\n"
                        "    name: String { id: 24, text: \"Periodic\" }\n"
                        "    size: 2\n"
                        "    periodic: true\n"
                        "}");

        ostringstream result;
        result << setDetail(DETAIL_FULL)
               << item;
        EXPECT_EQ(expected, result.str());
    }

    {
        String        name(42, "Non-periodic");
        CartDimension item(24, name, 3, CartDimension::NON_PERIODIC);

        string expected("CartDimension {\n"
                        "    id: 24\n"
                        "    name: String { id: 42, text: \"Non-periodic\" }\n"
                        "    size: 3\n"
                        "    periodic: false\n"
                        "}");

        ostringstream result;
        result << setDetail(DETAIL_FULL)
               << item;
        EXPECT_EQ(expected, result.str());
    }
}
