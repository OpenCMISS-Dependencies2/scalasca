# coding=UTF-8
import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp
import time

import six

import otf2
from otf2.enums import Type



TIMER_GRANULARITY = 1000000


def t():
    return int(round(time.time() * TIMER_GRANULARITY))


class TestOTF2UserData(unittest.TestCase):

    num_locations = 4
    num_events = 200
    archive_name = "otf2_test_trace"
    locations = [x * x for x in range(num_locations)]

    def setUp(self):
        self.old_cwd = os.getcwd()
        self.tmp_dirname = mkdtemp(prefix=os.path.basename(os.path.abspath(__file__))[:-3] + '_tmp', dir=self.old_cwd)

        self.orig_trace = os.path.join(self.tmp_dirname, "orig")
        self.retrace = os.path.join(self.tmp_dirname, "retrc")

        os.mkdir(self.orig_trace)
        os.mkdir(self.retrace)

        os.chdir(self.tmp_dirname)

    def tearDown(self):
        os.chdir(self.old_cwd)
        if 'V' in os.environ and os.environ['V'] != '':
            print(self.tmp_dirname)
        else:
            rmtree(self.tmp_dirname)

    def generate_trace(self, archive_name):
        with otf2.writer.open(archive_name, timer_resolution=TIMER_GRANULARITY) as trace:

            self.definitions = trace.definitions

            function = trace.definitions.region(u"🔄 🐹 🌳 🔍")

            parent_node = trace.definitions.system_tree_node(u"💾 √🌳")
            system_tree_node = trace.definitions.system_tree_node(u"🖥", parent=parent_node)

            location_group = trace.definitions.location_group(u"🚗 Process",
                                                              system_tree_parent=system_tree_node)

            attr = trace.definitions.attribute(u"StringTest", u"A test attribute", Type.STRING)
            float_attr = trace.definitions.attribute(u"FloatTest", u"Another test attribute",
                                                     Type.DOUBLE)

            writer = trace.event_writer(u"💓〰️", group=location_group)
            writer2 = trace.event_writer(u"💕 〰️", group=location_group)

            function2 = trace.definitions.region(u"🙋 🌍")

            # Write enter and leave event
            writer.enter(t(), function, {attr: u"🙋 🌍"})
            writer2.enter(t(), function2)
            writer.enter(t(), function, {attr: u"🙋 🌍"})
            writer.enter(t(), function, {attr: u"🙋 🌍"})
            writer.enter(t(), function, {attr: u"🙋 🌍"})

            writer.leave(t(), function, attributes={float_attr: 42.0, attr: u"🍖?"})
            writer.leave(t(), function, attributes={float_attr: 42.0, attr: u"🍖?"})
            writer.leave(t(), function, attributes={float_attr: 42.0, attr: u"🍖?"})
            writer2.leave(t(), function2)

            writer.enter(t(), function, {attr: u"🙋 🌍"})
            writer.leave(t(), function, attributes={float_attr: 42.0, attr: u"🍖?"})

            writer.leave(t(), function, attributes={float_attr: 42.0, attr: u"🍖?"})

            # Get convenience metric object and write one metric event
            temperature = trace.definitions.metric(u"⌛️☕️", unit=u"min")
            writer.metric(t(), temperature, 72.0)

            # Get metric members
            temp_member = trace.definitions.metric_member(u"🌡", u"❄️",
                                                          otf2.MetricType.OTHER,
                                                          otf2.MetricMode.ABSOLUTE_POINT)
            power_member = trace.definitions.metric_member(u"🔋", u"🔥")
            # Add metric members to the metric class object
            mclass = trace.definitions.metric_class([temp_member, power_member])
            # Add metric object to the location object
            writer.metric(t(), mclass, [42.0, 12345.6])

    def read_trace(self, archive_name):
        with otf2.reader.open(archive_name) as trace:

            count = 0
            for location, event in trace.events:
                count += 1

            assert count == 14

            assert len(self.definitions.regions) == len(trace.definitions.regions)
            for written_region in self.definitions.regions:
                for read_region in trace.definitions.regions:
                    # there should be exactly one region
                    pass
                    # ert written_region.name == read_region.name

            assert len(self.definitions.strings) == len(trace.definitions.strings)
            for written_string in self.definitions.strings:
                written_string = written_string.name
                if six.PY2:
                    # strings from OTF2 in python2 are always of type str, not unicode
                    written_string = written_string.encode('utf-8')
                assert written_string in [read_string.name for read_string in
                                          trace.definitions.strings]

    def rewrite_trace(self, old_anchor_path, new_archive_path):
        with otf2.reader.open(old_anchor_path) as trace_reader:
            with otf2.writer.open(new_archive_path,
                                  definitions=trace_reader.definitions) as write_trace:
                for location, event in trace_reader.events:
                    writer = write_trace.event_writer_from_location(location)
                    writer(event)

    def test_rewrite(self):
        self.generate_trace(self.orig_trace)
        self.rewrite_trace(os.path.join(self.orig_trace, "traces.otf2"), self.retrace)
        self.read_trace(os.path.join(self.retrace, "traces.otf2"))


if __name__ == '__main__':
    unittest.main()
