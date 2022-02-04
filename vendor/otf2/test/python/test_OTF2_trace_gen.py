#!/usr/bin/env python

import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp

from _otf2_trace_gen import main, otf2_set_parameters


class TestOTF2TraceGen(unittest.TestCase):
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

    def test_otf2_trace_gen_mpi(self):
        otf2_set_parameters(paradigm='mpi', locations=4, attributes=True, events=10)
        main()

    def test_otf2_trace_gen_openmp(self):
        otf2_set_parameters(paradigm='openmp', locations=6, attributes=False, events=20)
        main()

    def test_otf2_trace_gen_hybrid(self):
        otf2_set_parameters(paradigm='hybrid', locations=12, attributes=True, events=7)
        main()


if __name__ == '__main__':
    unittest.main()
