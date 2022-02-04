import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GlobalEvtReaderCallbacks import GlobalEvtReaderCallbacks


class GlobalEvtReader(ctypes.Structure):
    pass

def GlobalEvtReader_SetCallbacks(reader, callbacks, userData):
    c_SetCallbacks = conf.lib.OTF2_GlobalEvtReader_SetCallbacks
    c_SetCallbacks.argtypes = [ ctypes.POINTER(GlobalEvtReader), ctypes.POINTER(GlobalEvtReaderCallbacks), ctypes.py_object ]
    c_SetCallbacks.restype = ErrorCode
    c_SetCallbacks.errcheck = HandleErrorCode
    c_SetCallbacks(reader, callbacks, ctypes.py_object(userData))

def GlobalEvtReader_ReadEvent(reader):
    c_ReadEvent = conf.lib.OTF2_GlobalEvtReader_ReadEvent
    c_ReadEvent.argtypes = [ ctypes.POINTER(GlobalEvtReader) ]
    c_ReadEvent.restype = ErrorCode
    c_ReadEvent.errcheck = HandleErrorCode
    c_ReadEvent(reader)

def GlobalEvtReader_HasEvent(reader):
    c_HasEvent = conf.lib.OTF2_GlobalEvtReader_HasEvent
    c_HasEvent.argtypes = [ ctypes.POINTER(GlobalEvtReader), ctypes.POINTER(ctypes.c_int) ]
    c_HasEvent.restype = ErrorCode
    c_HasEvent.errcheck = HandleErrorCode
    flag = ctypes.c_int()
    c_HasEvent(reader, ctypes.byref(flag))
    return flag.value

def GlobalEvtReader_ReadEvents(reader, recordsToRead):
    c_ReadEvents = conf.lib.OTF2_GlobalEvtReader_ReadEvents
    c_ReadEvents.argtypes = [ ctypes.POINTER(GlobalEvtReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadEvents.restype = ErrorCode
    c_ReadEvents.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadEvents(reader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

__all__ = [
    'GlobalEvtReader',
    'GlobalEvtReader_HasEvent',
    'GlobalEvtReader_ReadEvent',
    'GlobalEvtReader_ReadEvents',
    'GlobalEvtReader_SetCallbacks'
]
