#!/usr/bin/env python

import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp

import _otf2_combined_writer


class TestOTF2Combined(unittest.TestCase):
    def setUp(self):
        self.tmp_dirname = mkdtemp(prefix=os.path.basename(os.path.abspath(__file__))[:-3] + '_tmp', dir=os.getcwd())

    def tearDown(self):
        if 'V' in os.environ and os.environ['V'] != '':
            print(self.tmp_dirname)
        else:
            rmtree(self.tmp_dirname)

    def test_combined(self):
        _otf2_combined_writer.write_archive(
            os.path.join(self.tmp_dirname, "test_trace"), "traces")


if __name__ == '__main__':
    unittest.main()
