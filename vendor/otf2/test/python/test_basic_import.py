import unittest


class BasicImportTest(unittest.TestCase):
    @staticmethod
    def test_import_ll():
        import _otf2

    @staticmethod
    def test_import_hl():
        import otf2


if __name__ == '__main__':
    unittest.main()
