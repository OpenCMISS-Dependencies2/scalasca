import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GlobalDefReaderCallbacks import GlobalDefReaderCallbacks


class GlobalDefReader(ctypes.Structure):
    pass

def GlobalDefReader_SetCallbacks(reader, callbacks, userData):
    c_SetCallbacks = conf.lib.OTF2_GlobalDefReader_SetCallbacks
    c_SetCallbacks.argtypes = [ ctypes.POINTER(GlobalDefReader), ctypes.POINTER(GlobalDefReaderCallbacks), ctypes.py_object ]
    c_SetCallbacks.restype = ErrorCode
    c_SetCallbacks.errcheck = HandleErrorCode
    c_SetCallbacks(reader, callbacks, ctypes.py_object(userData))

def GlobalDefReader_ReadDefinitions(reader, recordsToRead):
    c_ReadDefinitions = conf.lib.OTF2_GlobalDefReader_ReadDefinitions
    c_ReadDefinitions.argtypes = [ ctypes.POINTER(GlobalDefReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadDefinitions.restype = ErrorCode
    c_ReadDefinitions.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadDefinitions(reader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

__all__ = [
    'GlobalDefReader',
    'GlobalDefReader_ReadDefinitions',
    'GlobalDefReader_SetCallbacks'
]
