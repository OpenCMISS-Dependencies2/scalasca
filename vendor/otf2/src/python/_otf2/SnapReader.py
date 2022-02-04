import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GeneralDefinitions import LocationRef
from .SnapReaderCallbacks import SnapReaderCallbacks


class SnapReader(ctypes.Structure):
    pass

def SnapReader_GetLocationID(reader):
    c_GetLocationID = conf.lib.OTF2_SnapReader_GetLocationID
    c_GetLocationID.argtypes = [ ctypes.POINTER(SnapReader), ctypes.POINTER(LocationRef) ]
    c_GetLocationID.restype = ErrorCode
    c_GetLocationID.errcheck = HandleErrorCode
    location = LocationRef()
    c_GetLocationID(reader, ctypes.byref(location))
    return location.value

def SnapReader_SetCallbacks(reader, callbacks, userData):
    c_SetCallbacks = conf.lib.OTF2_SnapReader_SetCallbacks
    c_SetCallbacks.argtypes = [ ctypes.POINTER(SnapReader), ctypes.POINTER(SnapReaderCallbacks), ctypes.py_object ]
    c_SetCallbacks.restype = ErrorCode
    c_SetCallbacks.errcheck = HandleErrorCode
    c_SetCallbacks(reader, callbacks, ctypes.py_object(userData))

def SnapReader_Seek(reader, req_time):
    c_Seek = conf.lib.OTF2_SnapReader_Seek
    c_Seek.argtypes = [ ctypes.POINTER(SnapReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_int) ]
    c_Seek.restype = ErrorCode
    c_Seek.errcheck = HandleErrorCode
    found = ctypes.c_int()
    c_Seek(reader, req_time, ctypes.byref(found))
    return found.value

def SnapReader_ReadSnapshots(reader, recordsToRead):
    c_ReadSnapshots = conf.lib.OTF2_SnapReader_ReadSnapshots
    c_ReadSnapshots.argtypes = [ ctypes.POINTER(SnapReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadSnapshots.restype = ErrorCode
    c_ReadSnapshots.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadSnapshots(reader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

__all__ = [
    'SnapReader',
    'SnapReader_GetLocationID',
    'SnapReader_ReadSnapshots',
    'SnapReader_Seek',
    'SnapReader_SetCallbacks'
]
