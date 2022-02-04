#!/usr/bin/env python

import unittest

import _otf2


class TestOTF2AttributeList(unittest.TestCase):
    def test_1_new_delete(self):
        attr_list = _otf2.AttributeList_New()
        self.assertIsNotNone(attr_list)
        _otf2.AttributeList_Delete(attr_list)

    def test_2_add_get_pop(self):
        attr_list = _otf2.AttributeList_New()
        self.assertIsNotNone(attr_list)
        attr = _otf2.AttributeValue()
        attr.uint32 = 1
        _otf2.AttributeList_AddAttribute(attr_list, 42, _otf2.TYPE_UINT32, attr)

        res = _otf2.AttributeList_TestAttributeByID(attr_list, 42)
        self.assertTrue(res, "test id")

        number_of_elements = _otf2.AttributeList_GetNumberOfElements(attr_list)
        self.assertEqual(number_of_elements, 1, "incorrect number of elements")

        # C-style
        type_ = _otf2.Type()
        id_ = _otf2.AttributeRef()
        _otf2.AttributeList_GetAttributeByID(attr_list, 42, type_, attr)
        self.assertEqual(type_, _otf2.TYPE_UINT32, "incorrect type")
        self.assertNotEqual(type_, _otf2.TYPE_INT32, "incorrect type matches")
        self.assertEqual(attr.uint32, 1, "incorrect value")

        _otf2.AttributeList_GetAttributeByIndex(attr_list, 0, id_, type_, attr)
        self.assertEqual(id_.value, 42, "incorrect id")
        self.assertEqual(type_, _otf2.TYPE_UINT32, "incorrect type")
        self.assertEqual(attr.uint32, 1, "incorrect value")

        # Python-style
        (type_, attr) = _otf2.AttributeList_GetAttributeByID(attr_list, 42)
        self.assertEqual(type_, _otf2.TYPE_UINT32, "incorrect type")
        self.assertNotEqual(type_, _otf2.TYPE_INT32, "incorrect type matches")
        self.assertEqual(attr.uint32, 1, "incorrect value")

        (id_, type_, attr) = _otf2.AttributeList_GetAttributeByIndex(attr_list, 0)
        self.assertEqual(id_, 42, "incorrect id")
        self.assertEqual(type_, _otf2.TYPE_UINT32, "incorrect type")
        self.assertEqual(attr.uint32, 1, "incorrect value")

        (id, type, attr) = _otf2.AttributeList_PopAttribute(attr_list)
        self.assertEqual(id_, 42, "incorrect id")
        self.assertEqual(type_, _otf2.TYPE_UINT32, "incorrect type")
        self.assertEqual(attr.uint32, 1, "incorrect value")

        number_of_elements = _otf2.AttributeList_GetNumberOfElements(attr_list)
        self.assertEqual(number_of_elements, 0, "incorrect number of elements")

        _otf2.AttributeList_Delete(attr_list)

    def test_3_random_remove(self):
        attr_list = _otf2.AttributeList_New()
        self.assertIsNotNone(attr_list)

        ids_to_add = [4, 8, 15, 16, 23, 42]
        # from the end, from the front, from the middle, as index into ids_to_add
        ids_to_remove = [5, 0, 2, 1, 4, 3]

        attr = _otf2.AttributeValue()

        for (i, id_to_add) in enumerate(ids_to_add):
            attr.uint32 = i
            _otf2.AttributeList_AddAttribute(attr_list, id_to_add, _otf2.TYPE_UINT32, attr)

            res = _otf2.AttributeList_TestAttributeByID(attr_list, id_to_add);
            self.assertTrue(res, "failed test id")

            number_of_elements = _otf2.AttributeList_GetNumberOfElements(attr_list)
            self.assertEqual(number_of_elements, i + 1, "incorrect number of elements")

        for (i, id_to_remove) in enumerate(ids_to_remove):
            id_to_add = ids_to_add[id_to_remove]
            (type_, attr) = _otf2.AttributeList_GetAttributeByID(attr_list, id_to_add)
            self.assertEqual(type_, _otf2.TYPE_UINT32)
            self.assertEqual(attr.uint32, id_to_remove)

            _otf2.AttributeList_RemoveAttribute(attr_list, id_to_add)

            ret = _otf2.AttributeList_TestAttributeByID(attr_list, id_to_add)
            self.assertFalse(ret, "found removed test id")

            number_of_elements = _otf2.AttributeList_GetNumberOfElements(attr_list)
            self.assertEqual(number_of_elements, len(ids_to_add) - i - 1, "incorrect number of elements")

        _otf2.AttributeList_Delete(attr_list)


if __name__ == '__main__':
    unittest.main()
