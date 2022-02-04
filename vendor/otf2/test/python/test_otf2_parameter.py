import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp
import time

import otf2


TIMER_GRANULARITY = 1000000


def t():
    return int(round(time.time() * TIMER_GRANULARITY))


class TestOTF2Program(unittest.TestCase):
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
            system_tree_node = trace.definitions.system_tree_node("Root", parent=None)
            location_group = trace.definitions.location_group("Process",
                                                              system_tree_parent=system_tree_node)

            region = trace.definitions.region("main")

            parameter_str = trace.definitions.parameter("Foo")
            parameter_uint64 = trace.definitions.parameter("Bar", otf2.ParameterType.UINT64)

            callpath = trace.definitions.callpath(None, region)
            trace.definitions.callpath_parameter(callpath, parameter_str, otf2.Type.UINT64, 42)
            self.assertEqual(callpath["Foo"], 42)

            writer = trace.event_writer("thread", group=location_group)

            writer.enter(t(), region)
            writer.parameter_unsigned_int(t(), parameter_uint64, 42)
            writer.parameter_string(t(), parameter_str, "Hello!")

            try:
                writer.parameter_string(t(), parameter_uint64, 23)
                self.fail('writing uint64 parameter with string parameter event')
            except TypeError:
                pass

            try:
                writer.parameter_unsigned_int(t(), parameter_uint64, "not an integral")
                self.fail('writing string parameter value with uint64 parameter')
            except TypeError:
                pass

            try:
                writer.parameter_unsigned_int(t(), parameter_uint64, -100)
                self.fail('writing negative parameter value with uint64 parameter')
            except TypeError:
                pass

            writer.leave(t(), region)

    def read_trace(self, archive_name):
        with otf2.reader.open(archive_name) as trace:
            callpath = next(cp for cp in trace.definitions.callpaths)
            self.assertEqual(callpath["Foo"], 42)

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
