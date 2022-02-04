#!/usr/bin/env python
"""
This unit test checks the meta/base class infrastructure for definition classes
This is NOT an instruction how to use the otf2 module.
"""
import unittest

from otf2.definitions import _NamedRefDefinition, no_default, String
from otf2.registry import _RefRegistry
from otf2.enums import Type


class Unicorn(_NamedRefDefinition):
    _fields = (('horn', no_default, String),
               ('description', ''),
               ('legs', 4),
               ('sex', no_default, String),
               ('type', Type.FLOAT))
    _ref_name = 'metric'


class MockRegistry(object):
    def _update(self, old_key, unicorn):
        self.last_updated = unicorn


class TestDefinitions(unittest.TestCase):
    def test_valid(self):
        registry = MockRegistry()
        corn_silky_pony = Unicorn(registry, 42, 'Corn Silky Pony', '>>>>>', sex='m')
        self.assertEqual(corn_silky_pony._ref, 42)
        self.assertEqual(corn_silky_pony.name, 'Corn Silky Pony')
        self.assertEqual(corn_silky_pony.horn, '>>>>>')
        self.assertEqual(corn_silky_pony.description, '')
        self.assertEqual(corn_silky_pony.legs, 4)
        self.assertEqual(corn_silky_pony.sex, 'm')
        self.assertEqual(corn_silky_pony.type, Type.FLOAT)

        ivy_pink_ears = Unicorn(registry, 0, 'Ivy Pink Eears', horn='>>>', sex='f')
        self.assertEqual(ivy_pink_ears.horn, '>>>')
        self.assertEqual(ivy_pink_ears.sex, 'f')

        marigold_dapple_damsel = Unicorn(registry, 1, sex='f', horn='<<<<')
        self.assertEqual(marigold_dapple_damsel.name, 'Unicorn #1')
        self.assertEqual(marigold_dapple_damsel.sex, 'f')
        self.assertEqual(marigold_dapple_damsel.horn, '<<<<')

        chestnut_celestial_cheeks = Unicorn(registry, 0, 'Chestnut Celestial Cheeks', '>>>',
                                            'sparkly', type=Type.INT8, legs=6, sex='m')
        self.assertEqual(chestnut_celestial_cheeks.horn, '>>>')
        self.assertEqual(chestnut_celestial_cheeks.description, 'sparkly')
        self.assertEqual(chestnut_celestial_cheeks.type, Type.INT8)
        self.assertEqual(chestnut_celestial_cheeks.legs, 6)
        self.assertEqual(chestnut_celestial_cheeks.sex, 'm')

        robin_velvet_foal = Unicorn(registry, 0, 'Robin Velvet Foal',  '****', 'shiny', 4, 'm',
                                    Type.FLOAT)
        self.assertEqual(robin_velvet_foal.name, 'Robin Velvet Foal')
        self.assertEqual(robin_velvet_foal.horn, '****')
        self.assertEqual(robin_velvet_foal.description, 'shiny')
        self.assertEqual(robin_velvet_foal.legs, 4)
        self.assertEqual(robin_velvet_foal.sex, 'm')
        self.assertEqual(robin_velvet_foal.type, Type.FLOAT)

        # A tragic accident :-(
        robin_velvet_foal.legs = 3
        self.assertEqual(robin_velvet_foal.legs, 3)
        self.assertEqual(registry.last_updated, robin_velvet_foal)

    def test_invalid(self):
        registry = MockRegistry()
        with self.assertRaises(TypeError):
            # Many missing
            Unicorn(registry, 0)

        with self.assertRaises(TypeError):
            # Missing horn
            Unicorn(registry, 0, 'Incomplete Indigo Ingo', sex='m')

        with self.assertRaises(TypeError):
            # Wrong type
            Unicorn(registry, 0, 'Thyme Beautiful Stallion', '>>>>>', sex=1)

        with self.assertRaises(TypeError):
            # Unused keyword argument
            Unicorn(registry, 0, horn='^', sex='f', extra=42)

        with self.assertRaises(TypeError):
            # Duplicate positional/keyword argument
            Unicorn(registry, 0, 'Parsley Velvet Trail', '^', sex='f', horn='>>')

        with self.assertRaises(TypeError):
            # Too many positional arguments
            Unicorn(registry, 0, 'Sage Beautiful Legs',  '****', 'shiny', 6, 'm', Type.FLOAT, 6)

        with self.assertRaises(TypeError):
            # Two horns not allowed
            Unicorn(registry, 0, "Parsley Jolly Legs", (">>>", ">>>>"), sex='m')

        corn_silky_pony = Unicorn(registry, 42, 'Corn Silky Pony', '>>>>>', sex='m')
        with self.assertRaises(AttributeError):
            corn_silky_pony.evil = True

        with self.assertRaises(TypeError):
            corn_silky_pony.legs = "4"

if __name__ == '__main__':
    unittest.main()
