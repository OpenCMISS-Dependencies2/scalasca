import ctypes
import os.path
from os import environ

from collections import namedtuple

import six

Conf = namedtuple('Conf', 'lib')


def is_sphinx_run():
    try:
        return environ["OTF2_PYTHON_SPHINX_GENERATOR_RUN"] == "yes"
    except:
        return False


def lib_path():
    libname = '_otf2.so'
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), libname)


def lib():
    if is_sphinx_run():
        return None
    libhandle = ctypes.cdll.LoadLibrary(lib_path())
    return Conf(libhandle)


conf = lib()

# Performance note:
# str.encode('utf-8'), seems to be slightly faster then Codec.encode()[0]
# str.encode() using the default argument 'utf-8' may be slightly faster
# but is more confusing for encoding, there might be optimization potential
# by avoiding the intermediate byte object and see if a sub-class of
# c_char_p could be a bytes-like object itself
encoding = 'utf-8'

if six.PY2:
    class StrParam(object):
        """
        argtype for ctypes to be used for string pointers
        Python2 variant, allows both str a.k.a. bytes and unicode and returns str
        """
        @classmethod
        def from_param(cls, value):
            if value is None:
                return None
            if isinstance(value, str):
                return value
            if isinstance(value, unicode):
                return value.encode(encoding)
            raise TypeError("String parameters must be str or unicode, actual {}.".
                            format(type(value)))


    def str_errcheck_nullable(result, func, arguments):
        """
        ctypes errcheck method for strings.
        NULL / None is ok.
        Use in combination with restype = ctypes.c_char_p
        Python2 variant, does nothing
        Note: We don't have a non-global way of the user telling us whether or not he wants str or
              unicode objects
        """
        return result

    def str_decode(otf2_str):
        """
        Decode a string from otf2 / ctypes.
        Asserts the input is a valid string or NULL.
        Python2 version, does nothing
        """
        if otf2_str is None:
            return None
        assert isinstance(otf2_str, str)
        return otf2_str
else:
    class StrParam(object):
        """
        argtype for ctypes to be used for string pointers
        Python3 variant, encodes the unicode to binary
        """
        @classmethod
        def from_param(cls, value):
            if value is None:
                return None
            assert isinstance(value, str)
            return value.encode(encoding)

    def str_errcheck_nullable(result, func, arguments):
        """
        ctypes errcheck method for strings.
        NULL / None is ok.
        Use in combination with restype = ctypes.c_char_p
        Python3 variant, decodes the binary string to unicode
        """
        if result is None:
            return None
        assert isinstance(result, bytes)
        return result.decode(encoding)

    def str_decode(otf2_str):
        """
        Decode a string from otf2 / ctypes.
        Asserts the input is a valid string or NULL.
        Python3 variant, decodes the binary string to unicode
        """
        if otf2_str is None:
            return None
        assert isinstance(otf2_str, bytes)
        return otf2_str.decode(encoding)


class EnumBase(object):
    def __eq__(self, other):
        if type(self) != type(other):
            raise TypeError("Trying to compare different enums.")
        return self.value == other.value

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        try:
            return "{}.{}".format(type(self).__name__, type(self)._names[self.value])
        except AttributeError:
            return "{}({})".format(type(self).__name__, self.value)

    def __repr__(self):
        return str(self)

    def __hash__(self):
        return hash((type(self), self.value))

    @classmethod
    def _construct(cls, value, name):
        assert type(name) is str
        assert name != ""
        if not hasattr(cls, "_names"):
            cls._names = dict()
        cls._names[value] = name
        obj = cls(value)
        setattr(cls, name, obj)
        return obj


def _same_type(a, b):
    assert type(a) == type(b)
    return type(a)


class EnumBitset(EnumBase):
    def __or__(self, other):
        return _same_type(self, other)(self.value | other.value)

    def __and__(self, other):
        return _same_type(self, other)(self.value & other.value)

    def __xor__(self, other):
        return _same_type(self, other)(self.value ^ other.value)

    def __invert__(self):
        return type(self)(~self.value)


def errcheck_null_to_none(result, func, arguments):
    if result:
        return result
    return None


def free(value):
    c_free = conf.lib.otf2_python_free
    c_free.argtypes = [ ctypes.c_void_p ]
    c_free.restype = None
    c_free(value)
