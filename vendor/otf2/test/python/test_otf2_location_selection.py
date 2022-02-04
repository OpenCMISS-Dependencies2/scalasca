import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp
import time

import otf2

from otf2_writer import generate_trace

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

    def read_trace(self, archive_name):
        count_selected = 0
        with otf2.reader.open(archive_name) as trace:
            selected_location = trace.definitions.locations[1]
            for location, event in trace.events(selected_location):
                self.assertEqual(location, selected_location)
                count_selected += 1

            with self.assertRaises(RuntimeError):
                trace.events(selected_location)

        count_reference = 0
        with otf2.reader.open(archive_name) as trace:
            selected_location = trace.definitions.locations[1]
            for location, event in trace.events:
                if location == selected_location:
                    count_reference += 1

            with self.assertRaises(EOFError):
                for _ in trace.events:
                    pass

        self.assertEqual(count_selected, count_reference)

    def test_integrity(self):
        self.definitions = generate_trace(self.tmp_dirname, 2)
        self.read_trace(os.path.join(self.tmp_dirname, "traces.otf2"))


if __name__ == '__main__':
    unittest.main()
