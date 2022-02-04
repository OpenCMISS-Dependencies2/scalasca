import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp
import time

import otf2

from otf2_writer import generate_trace
from otf2_writer_cct import generate_trace as generate_trace_cct

TIMER_GRANULARITY = 1000000


def t():
    return int(round(time.time() * TIMER_GRANULARITY))


class TestOTF2UserData(unittest.TestCase):

    archive_name = "otf2_test_trace"

    def setUp(self):
        self.old_cwd = os.getcwd()
        self.tmp_dirname = mkdtemp(prefix=os.path.basename(os.path.abspath(__file__))[:-3] + '_tmp', dir=self.old_cwd)
        os.chdir(self.tmp_dirname)

    def tearDown(self):
        os.chdir(self.old_cwd)
        if 'V' in os.environ and os.environ['V'] != '':
            print(self.tmp_dirname)
        else:
            rmtree(self.tmp_dirname)

    def read_trace(self, archive_name, expected_number_of_events):
        with otf2.reader.open(archive_name) as trace:
            count = 0
            for location, event in trace.events:
                count += 1

            self.assertEqual(count, expected_number_of_events)

            self.assertEqual(len(self.definitions.regions), len(trace.definitions.regions))
            for written_region in self.definitions.regions:
                for read_region in trace.definitions.regions:
                    # there should be exactly one region
                    self.assertEqual(written_region.name, read_region.name)

            self.assertEqual(len(self.definitions.strings), len(trace.definitions.strings))
            for written_string in self.definitions.strings:
                self.assertTrue(written_string in trace.definitions.strings)

    def test_integrity(self):
        self.definitions = generate_trace(self.tmp_dirname)
        self.read_trace(os.path.join(self.tmp_dirname, "traces.otf2"), 4)


    def test_integrity_cct(self):
        self.definitions = generate_trace_cct(self.tmp_dirname)
        self.read_trace(os.path.join(self.tmp_dirname, "traces.otf2"), 5)


if __name__ == '__main__':
    unittest.main()
