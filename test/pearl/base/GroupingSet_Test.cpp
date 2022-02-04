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

#include <pearl/GroupingSet.h>

#include <gtest/gtest.h>

using namespace testing;
using namespace pearl;


// --- GroupingSet tests ----------------------------------------------------

TEST(GroupingSetT,
     toStdString_knownSetTypes_returnsStringRep)
{
    EXPECT_EQ("Location set", toStdString(GroupingSet::LOCATION_SET));
    EXPECT_EQ("Region set", toStdString(GroupingSet::REGION_SET));
    EXPECT_EQ("Metric set", toStdString(GroupingSet::METRIC_SET));
    EXPECT_EQ("Comm location set", toStdString(GroupingSet::COMM_LOCATION_SET));
    EXPECT_EQ("Comm set", toStdString(GroupingSet::COMM_SET));
}


TEST(GroupingSetT,
     toStdString_properties_returnsStringRep)
{
    GroupingSet::Properties properties;

    EXPECT_EQ("0 (NONE)", toStdString(properties));

    properties |= GroupingSet::PROPERTY_SELF;
    EXPECT_EQ("1 (self)", toStdString(properties));

    properties |= GroupingSet::PROPERTY_GLOBAL_RANKS;
    EXPECT_EQ("5 (self,global ranks)", toStdString(properties));

    properties |= GroupingSet::PROPERTY_WORLD;
    EXPECT_EQ("7 (self,world,global ranks)", toStdString(properties));
}
