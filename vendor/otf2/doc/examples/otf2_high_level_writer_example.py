#!/usr/bin/env python
#
# This file is part of the Score-P software (http://www.score-p.org)
#
# Copyright (c) 2015-2016,
# Technische Universitaet Dresden, Germany
#
# This software may be modified and distributed under the terms of
# a BSD-style license.  See the COPYING file in the package base
# directory for details.
#

import otf2
from otf2.enums import Type
import time


TIMER_GRANULARITY = 1000000


def t():
    return int(round(time.time() * TIMER_GRANULARITY))


with otf2.writer.open("TestArchive", timer_resolution=TIMER_GRANULARITY) as trace:

    function = trace.definitions.region("My Function")

    parent_node = trace.definitions.system_tree_node("node")
    system_tree_node = trace.definitions.system_tree_node("myHost", parent=parent_node)

    trace.definitions.system_tree_node_property(system_tree_node, "color", value="black")
    trace.definitions.system_tree_node_property(system_tree_node, "rack #", value=42)

    location_group = trace.definitions.location_group("Master Process",
                                                      system_tree_parent=system_tree_node)

    attr = trace.definitions.attribute("StringTest", "A test attribute", Type.STRING)
    float_attr = trace.definitions.attribute("FloatTest", "Another test attribute",
                                             Type.DOUBLE)

    writer = trace.event_writer("Main Thread", group=location_group)

    # Write enter and leave event
    writer.enter(t(), function, {attr: "Hello World"})
    writer.leave(t(), function, attributes={float_attr: 42.0, attr: "Wurst?"})

    # Get convenience metric object and write one metric event
    temperature = trace.definitions.metric("Time since last coffee", unit="min")
    writer.metric(t(), temperature, 72.0)

    # Get metric members
    temp_member = trace.definitions.metric_member("Temperature", "C", otf2.MetricType.OTHER,
                                                  otf2.MetricMode.ABSOLUTE_POINT)
    power_member = trace.definitions.metric_member("Power", "W")
    # Add metric members to the metric class object
    mclass = trace.definitions.metric_class([temp_member, power_member])
    # Add metric object to the location object
    writer.metric(t(), mclass, [42.0, 12345.6])

