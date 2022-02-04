import ctypes

import six

from .Config import conf, str_errcheck_nullable, EnumBase


class ErrorCode(EnumBase, ctypes.c_int):
    pass

DEPRECATED = ErrorCode(-3)
ABORT = ErrorCode(-2)
WARNING = ErrorCode(-1)
SUCCESS = ErrorCode(0)
ERROR_INVALID = ErrorCode(1)
ERROR_E2BIG = ErrorCode(2)
ERROR_EACCES = ErrorCode(3)
ERROR_EADDRNOTAVAIL = ErrorCode(4)
ERROR_EAFNOSUPPORT = ErrorCode(5)
ERROR_EAGAIN = ErrorCode(6)
ERROR_EALREADY = ErrorCode(7)
ERROR_EBADF = ErrorCode(8)
ERROR_EBADMSG = ErrorCode(9)
ERROR_EBUSY = ErrorCode(10)
ERROR_ECANCELED = ErrorCode(11)
ERROR_ECHILD = ErrorCode(12)
ERROR_ECONNREFUSED = ErrorCode(13)
ERROR_ECONNRESET = ErrorCode(14)
ERROR_EDEADLK = ErrorCode(15)
ERROR_EDESTADDRREQ = ErrorCode(16)
ERROR_EDOM = ErrorCode(17)
ERROR_EDQUOT = ErrorCode(18)
ERROR_EEXIST = ErrorCode(19)
ERROR_EFAULT = ErrorCode(20)
ERROR_EFBIG = ErrorCode(21)
ERROR_EINPROGRESS = ErrorCode(22)
ERROR_EINTR = ErrorCode(23)
ERROR_EINVAL = ErrorCode(24)
ERROR_EIO = ErrorCode(25)
ERROR_EISCONN = ErrorCode(26)
ERROR_EISDIR = ErrorCode(27)
ERROR_ELOOP = ErrorCode(28)
ERROR_EMFILE = ErrorCode(29)
ERROR_EMLINK = ErrorCode(30)
ERROR_EMSGSIZE = ErrorCode(31)
ERROR_EMULTIHOP = ErrorCode(32)
ERROR_ENAMETOOLONG = ErrorCode(33)
ERROR_ENETDOWN = ErrorCode(34)
ERROR_ENETRESET = ErrorCode(35)
ERROR_ENETUNREACH = ErrorCode(36)
ERROR_ENFILE = ErrorCode(37)
ERROR_ENOBUFS = ErrorCode(38)
ERROR_ENODATA = ErrorCode(39)
ERROR_ENODEV = ErrorCode(40)
ERROR_ENOENT = ErrorCode(41)
ERROR_ENOEXEC = ErrorCode(42)
ERROR_ENOLCK = ErrorCode(43)
ERROR_ENOLINK = ErrorCode(44)
ERROR_ENOMEM = ErrorCode(45)
ERROR_ENOMSG = ErrorCode(46)
ERROR_ENOPROTOOPT = ErrorCode(47)
ERROR_ENOSPC = ErrorCode(48)
ERROR_ENOSR = ErrorCode(49)
ERROR_ENOSTR = ErrorCode(50)
ERROR_ENOSYS = ErrorCode(51)
ERROR_ENOTCONN = ErrorCode(52)
ERROR_ENOTDIR = ErrorCode(53)
ERROR_ENOTEMPTY = ErrorCode(54)
ERROR_ENOTSOCK = ErrorCode(55)
ERROR_ENOTSUP = ErrorCode(56)
ERROR_ENOTTY = ErrorCode(57)
ERROR_ENXIO = ErrorCode(58)
ERROR_EOPNOTSUPP = ErrorCode(59)
ERROR_EOVERFLOW = ErrorCode(60)
ERROR_EPERM = ErrorCode(61)
ERROR_EPIPE = ErrorCode(62)
ERROR_EPROTO = ErrorCode(63)
ERROR_EPROTONOSUPPORT = ErrorCode(64)
ERROR_EPROTOTYPE = ErrorCode(65)
ERROR_ERANGE = ErrorCode(66)
ERROR_EROFS = ErrorCode(67)
ERROR_ESPIPE = ErrorCode(68)
ERROR_ESRCH = ErrorCode(69)
ERROR_ESTALE = ErrorCode(70)
ERROR_ETIME = ErrorCode(71)
ERROR_ETIMEDOUT = ErrorCode(72)
ERROR_ETXTBSY = ErrorCode(73)
ERROR_EWOULDBLOCK = ErrorCode(74)
ERROR_EXDEV = ErrorCode(75)
ERROR_END_OF_FUNCTION = ErrorCode(76)
ERROR_INVALID_CALL = ErrorCode(77)
ERROR_INVALID_ARGUMENT = ErrorCode(78)
ERROR_INVALID_RECORD = ErrorCode(79)
ERROR_INVALID_DATA = ErrorCode(80)
ERROR_INVALID_SIZE_GIVEN = ErrorCode(81)
ERROR_UNKNOWN_TYPE = ErrorCode(82)
ERROR_INTEGRITY_FAULT = ErrorCode(83)
ERROR_MEM_FAULT = ErrorCode(84)
ERROR_MEM_ALLOC_FAILED = ErrorCode(85)
ERROR_PROCESSED_WITH_FAULTS = ErrorCode(86)
ERROR_INDEX_OUT_OF_BOUNDS = ErrorCode(87)
ERROR_INVALID_LINENO = ErrorCode(88)
ERROR_END_OF_BUFFER = ErrorCode(89)
ERROR_FILE_INTERACTION = ErrorCode(90)
ERROR_FILE_CAN_NOT_OPEN = ErrorCode(91)
ERROR_INTERRUPTED_BY_CALLBACK = ErrorCode(92)
ERROR_PROPERTY_NAME_INVALID = ErrorCode(93)
ERROR_PROPERTY_EXISTS = ErrorCode(94)
ERROR_PROPERTY_NOT_FOUND = ErrorCode(95)
ERROR_PROPERTY_VALUE_INVALID = ErrorCode(96)
ERROR_FILE_COMPRESSION_NOT_SUPPORTED = ErrorCode(97)
ERROR_DUPLICATE_MAPPING_TABLE = ErrorCode(98)
ERROR_INVALID_FILE_MODE_TRANSITION = ErrorCode(99)
ERROR_COLLECTIVE_CALLBACK = ErrorCode(100)
ERROR_FILE_SUBSTRATE_NOT_SUPPORTED = ErrorCode(101)


def Error_GetName(errorCode):
    c_GetName = conf.lib.OTF2_Error_GetName
    c_GetName.argtypes = [ ErrorCode ]
    c_GetName.restype = ctypes.c_char_p
    c_GetName.errcheck = str_errcheck_nullable
    return c_GetName(errorCode)


def Error_GetDescription(errorCode):
    c_GetDescription = conf.lib.OTF2_Error_GetDescription
    c_GetDescription.argtypes = [ ErrorCode ]
    c_GetDescription.restype = ctypes.c_char_p
    c_GetDescription.errcheck = str_errcheck_nullable
    return c_GetDescription(errorCode)


# This needs ErrorCallback, but ErrorCallback needs variadic arguments which
# cannot be expressed with ctypes ffi.
# def Error_RegisterCallback(errorCallbackIn, userData):
    # c_RegisterCallback = conf.lib.OTF2_Error_RegisterCallback
    # c_RegisterCallback.argtypes = [ ErrorCallback, ctypes.py_object ]
    # c_RegisterCallback.restype = ErrorCallback
    # return c_RegisterCallback(errorCallbackIn, ctypes.py_object(userData))


class Error(Exception):
    '''Represents an generic error that occurred when using OTF2 library

    This is raised in the case where a C library function returns a value
    unlike SUCCESS

    FIXME: Just one generic exception class is not much helpful.
    Get error description from C library.
    '''
    def __init__(self, code):
        self.code = code
        self.name = Error_GetName(code)
        self.description = Error_GetDescription(code)
        self.message = "{} ({}): {}".format(self.name, code.value, self.description)
        Exception.__init__(self, self.message)


def HandleErrorCode(errcode, *args):
    ''' This functions is used for the errcheck attribute of the foreign
        functions. To automatically raise an exception if the ErrorCode
        returned by the C function is not SUCCESS.
    '''
    if errcode != SUCCESS:
        raise Error(errcode)


def errcheck_not_null(value, *_):
    if not value:
        raise Error(ERROR_INVALID)
    return value

# No fun to write all the error codes in here.
# __all__ = ['Error', 'ErrorCode', 'Error_GetDescription', 'HandleErrorCode']
