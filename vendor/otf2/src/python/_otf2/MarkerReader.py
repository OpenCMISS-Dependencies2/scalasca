import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .MarkerReaderCallbacks import MarkerReaderCallbacks


class MarkerReader(ctypes.Structure):
    pass

def MarkerReader_ReadMarkers(reader, recordsToRead):
    c_ReadMarkers = conf.lib.OTF2_MarkerReader_ReadMarkers
    c_ReadMarkers.argtypes = [ ctypes.POINTER(MarkerReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadMarkers.restype = ErrorCode
    c_ReadMarkers.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadMarkers(reader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

def MarkerReader_SetCallbacks(reader, callbacks, userData):
    c_SetCallbacks = conf.lib.OTF2_MarkerReader_SetCallbacks
    c_SetCallbacks.argtypes = [ ctypes.POINTER(MarkerReader), ctypes.POINTER(MarkerReaderCallbacks), ctypes.py_object ]
    c_SetCallbacks.restype = ErrorCode
    c_SetCallbacks.errcheck = HandleErrorCode
    c_SetCallbacks(reader, callbacks, ctypes.py_object(userData))

__all__ = [
    'MarkerReader',
    'MarkerReader_ReadMarkers',
    'MarkerReader_SetCallbacks'
]
