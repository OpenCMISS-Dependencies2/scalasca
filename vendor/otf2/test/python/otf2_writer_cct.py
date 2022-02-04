#!/usr/bin/env python

import otf2
from otf2.enums import Type
import time
from tempfile import mkdtemp
import os

TIMER_GRANULARITY = 1000000


def t():
    return int(round(time.time() * TIMER_GRANULARITY))


def generate_trace(archive_name="TestArchive", num_locations = 1):
    with otf2.writer.open(archive_name, timer_resolution=TIMER_GRANULARITY) as trace:

        function = trace.definitions.region("My Function")

        parent_node = trace.definitions.system_tree_node("root node")
        system_tree_node = trace.definitions.system_tree_node("myHost", parent=parent_node)
        trace.definitions.system_tree_node_property(system_tree_node, "color", value="black")
        trace.definitions.system_tree_node_property(system_tree_node, "rack #", value=42)

        location_group = trace.definitions.location_group("Master Process",
                                                          system_tree_parent=system_tree_node)

        attr = trace.definitions.attribute("StringTest", "A test attribute", Type.STRING)
        float_attr = trace.definitions.attribute("FloatTest", "Another test attribute",
                                                 Type.DOUBLE)

        # Get convenience metric object and write one metric event
        temperature = trace.definitions.metric("Time since last coffee", unit="min")

        # Get metric members
        temp_member = trace.definitions.metric_member("Instructions", "C", otf2.MetricType.OTHER,
                                                      otf2.MetricMode.ABSOLUTE_POINT)
        power_member = trace.definitions.metric_member("Mem", "W")

        # Add metric members to the metric class object
        mclass = trace.definitions.metric_class([temp_member, power_member])

        calling_context0 = trace.definitions.calling_context(function, parent=None)
        calling_context_ = trace.definitions.calling_context(function, parent=calling_context0)
        calling_context1 = trace.definitions.calling_context(function, parent=calling_context0)
        trace.definitions.calling_context_property(calling_context1, "instruction",
                                                   value="mov R1, R2")

        irg = trace.definitions.interrupt_generator("INSTRUCTIONS", period=1337)

        for location in range(num_locations):
            writer = trace.event_writer("Main Thread {}".format(location), group=location_group)

            # Write enter event
            writer.calling_context_enter(t(), calling_context_, unwind_distance=2, attributes={attr: "Hello World"})

            writer.metric(t(), temperature, 72.0)

            # Add metric object to the location object
            writer.metric(t(), mclass, [42.0, 12345.6])

            writer.calling_context_sample(t(), calling_context1, unwind_distance=0,
                                          interrupt_generator=irg)

            # Write leave event
            writer.calling_context_leave(t(), calling_context_, attributes={float_attr: 42.0, attr: "Wurst?"})

        return trace.definitions

if __name__ == '__main__':
    trace_dir = mkdtemp(prefix="trace_test.", dir=os.getcwd())
    os.rmdir(trace_dir)
    generate_trace(trace_dir)
    print("Created trace in {}".format(trace_dir))
