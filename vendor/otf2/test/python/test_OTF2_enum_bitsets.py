#!/usr/bin/env python

import unittest

from _otf2.Definitions import GROUP_FLAG_GLOBAL_MEMBERS, GROUP_FLAG_NONE
from _otf2.Events import RMA_SYNC_LEVEL_MEMORY, RMA_SYNC_LEVEL_NONE, RMA_SYNC_LEVEL_PROCESS
from _otf2.Definitions import REGION_FLAG_DYNAMIC, REGION_FLAG_NONE, REGION_FLAG_PHASE


class TestSymbolsComplete(unittest.TestCase):
    def test_group_flag(self):
        self.assertEqual(GROUP_FLAG_GLOBAL_MEMBERS | GROUP_FLAG_GLOBAL_MEMBERS,
                         GROUP_FLAG_GLOBAL_MEMBERS)
        self.assertEqual(GROUP_FLAG_GLOBAL_MEMBERS & GROUP_FLAG_GLOBAL_MEMBERS,
                         GROUP_FLAG_GLOBAL_MEMBERS)
        self.assertEqual(GROUP_FLAG_GLOBAL_MEMBERS ^ GROUP_FLAG_GLOBAL_MEMBERS, GROUP_FLAG_NONE)

    def test_rma_sync_level(self):
        self.assertNotEqual(~RMA_SYNC_LEVEL_PROCESS, RMA_SYNC_LEVEL_PROCESS)
        self.assertEqual(RMA_SYNC_LEVEL_MEMORY & RMA_SYNC_LEVEL_NONE, RMA_SYNC_LEVEL_NONE)
        self.assertEqual((RMA_SYNC_LEVEL_MEMORY | RMA_SYNC_LEVEL_NONE | RMA_SYNC_LEVEL_PROCESS) &
                         RMA_SYNC_LEVEL_PROCESS, RMA_SYNC_LEVEL_PROCESS)

    def test_region_flag(self):
        self.assertNotEqual(REGION_FLAG_DYNAMIC| REGION_FLAG_PHASE,  REGION_FLAG_NONE)
        self.assertEqual(REGION_FLAG_DYNAMIC | REGION_FLAG_PHASE,
                         REGION_FLAG_PHASE | REGION_FLAG_DYNAMIC)

if __name__ == '__main__':
    unittest.main()
