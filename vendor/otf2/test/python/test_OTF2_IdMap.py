#!/usr/bin/env python

import unittest

import _otf2


def traverse(local_id, global_id, user_data):
    user_data[global_id] = local_id


class TestOTF2AnchorFile(unittest.TestCase):
    def test_1(self):
        id_map = _otf2.IdMap_Create(_otf2.ID_MAP_SPARSE, 10)
        self.assertIsNotNone(id_map)
        _otf2.IdMap_Free(id_map)

    def test_2(self):
        id_map = _otf2.IdMap_Create(_otf2.ID_MAP_SPARSE, 100)
        self.assertIsNotNone(id_map)
        _otf2.IdMap_Free(id_map)

    def test_3(self):
        id_map = _otf2.IdMap_Create(_otf2.ID_MAP_SPARSE, 6)
        self.assertIsNotNone(id_map)

        _otf2.IdMap_AddIdPair(id_map, 42, 0)
        _otf2.IdMap_AddIdPair(id_map, 13, 1)
        _otf2.IdMap_AddIdPair(id_map, 0, 2)

        array = [0, 0, 0]
        _otf2.IdMap_Traverse(id_map, traverse, array)

        self.assertEqual(42, array[0])
        self.assertEqual(13, array[1])
        self.assertEqual(0, array[2])

        _otf2.IdMap_Free(id_map)

    def test_4(self):
        mappings = [0, 5, 2, 1, 4]

        id_map = _otf2.IdMap_CreateFromUint64Array(mappings, True)
        self.assertIsNotNone(id_map)

        mode = _otf2.IdMap_GetMode(id_map)
        self.assertEqual(mode, _otf2.ID_MAP_SPARSE)

        for i in range(5):
            global_id = _otf2.IdMap_GetGlobalId(id_map, i)
            self.assertEqual(mappings[i], global_id)
        _otf2.IdMap_Free(id_map)

    def test_5(self):
        mappings = [0, 5, 2, 1, 3]

        id_map = _otf2.IdMap_CreateFromUint64Array(mappings, True)
        self.assertIsNotNone(id_map)

        mode = _otf2.IdMap_GetMode(id_map)
        self.assertEqual(mode, _otf2.ID_MAP_DENSE)

        for i in range(5):
            global_id = _otf2.IdMap_GetGlobalId(id_map, i)
            self.assertEqual(mappings[i], global_id)

        _otf2.IdMap_Free(id_map)

    def test_6(self):
        mappings = [0, 1, 2, 3, 4]

        id_map = _otf2.IdMap_CreateFromUint64Array(mappings, True)
        self.assertEqual(id_map, None)

        id_map = _otf2.IdMap_CreateFromUint64Array([], True)
        self.assertEqual(id_map, None)

    def test_7(self):
        ids = [4, 0, 2, 1, 3]

        id_map = _otf2.IdMap_Create(_otf2.ID_MAP_SPARSE, 5)
        self.assertIsNotNone(id_map)

        for i in range(5):
            _otf2.IdMap_AddIdPair(id_map, ids[i], ids[i])

        for i in range(5):
            global_id = _otf2.IdMap_GetGlobalId(id_map, i)
            self.assertEqual(i, global_id)

        _otf2.IdMap_Free(id_map)


if __name__ == '__main__':
    unittest.main()
