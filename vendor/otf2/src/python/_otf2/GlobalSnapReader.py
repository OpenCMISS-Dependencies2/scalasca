import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GlobalSnapReaderCallbacks import GlobalSnapReaderCallbacks


class GlobalSnapReader(ctypes.Structure):
    pass

def GlobalSnapReader_SetCallbacks(reader, callbacks, userData):
    c_SetCallbacks = conf.lib.OTF2_GlobalSnapReader_SetCallbacks
    c_SetCallbacks.argtypes = [ ctypes.POINTER(GlobalSnapReader), ctypes.POINTER(GlobalSnapReaderCallbacks), ctypes.py_object ]
    c_SetCallbacks.restype = ErrorCode
    c_SetCallbacks.errcheck = HandleErrorCode
    c_SetCallbacks(reader, callbacks, ctypes.py_object(userData))


def GlobalSnapReader_ReadSnapshots(reader, recordsToRead):
    c_ReadSnapshots = conf.lib.OTF2_GlobalSnapReader_ReadSnapshots
    c_ReadSnapshots.argtypes = [ ctypes.POINTER(GlobalSnapReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadSnapshots.restype = ErrorCode
    c_ReadSnapshots.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadSnapshots(reader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

__all__ = [
    'GlobalSnapReader',
    'GlobalSnapReader_ReadSnapshots',
    'GlobalSnapReader_SetCallbacks'
]
