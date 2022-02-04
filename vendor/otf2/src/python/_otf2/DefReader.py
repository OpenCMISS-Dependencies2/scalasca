import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GeneralDefinitions import LocationRef
from .DefReaderCallbacks import DefReaderCallbacks


# forward declaration
class DefReader(ctypes.Structure):
    pass

def DefReader_GetLocationID(reader):
    c_GetLocationID = conf.lib.OTF2_GetLocationID
    c_GetLocationID.argtypes = [ ctypes.POINTER(DefReader), ctypes.POINTER(LocationRef) ]
    c_GetLocationID.restype = ErrorCode
    c_GetLocationID.errcheck = HandleErrorCode
    location = LocationRef()
    c_GetLocationID(reader, ctypes.byref(location))
    return location.value

def DefReader_SetCallbacks(reader, callbacks, userData):
    c_SetCallbacks = conf.lib.OTF2_DefReader_SetCallbacks
    c_SetCallbacks.argtypes = [ ctypes.POINTER(DefReader), ctypes.POINTER(DefReaderCallbacks), ctypes.py_object ]
    c_SetCallbacks.restype = ErrorCode
    c_SetCallbacks.errcheck = HandleErrorCode
    c_SetCallbacks(reader, callbacks, ctypes.py_object(userData))

def DefReader_ReadDefinitions(reader, recordsToRead):
    c_ReadDefinitions = conf.lib.OTF2_DefReader_ReadDefinitions
    c_ReadDefinitions.argtypes = [ ctypes.POINTER(DefReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadDefinitions.restype = ErrorCode
    c_ReadDefinitions.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadDefinitions(reader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

__all__ = [
    'DefReader',
    'DefReader_GetLocationID',
    'DefReader_ReadDefinitions',
    'DefReader_SetCallbacks'
]
