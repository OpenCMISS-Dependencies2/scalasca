import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GeneralDefinitions import LocationRef, TimeStamp
from .EvtReaderCallbacks import EvtReaderCallbacks


# forward declaration
class EvtReader(ctypes.Structure):
    pass

def EvtReader_GetLocationID(reader):
    c_GetLocationID = conf.lib.OTF2_EvtReader_GetLocationID
    c_GetLocationID.argtypes = [ ctypes.POINTER(EvtReader), ctypes.POINTER(LocationRef) ]
    c_GetLocationID.restype = ErrorCode
    c_GetLocationID.errcheck = HandleErrorCode
    location = LocationRef()
    c_GetLocationID(reader, ctypes.byref(location))
    return location.value

def EvtReader_GetPos(reader):
    c_GetPos = conf.lib.OTF2_EvtReader_GetPos
    c_GetPos.argtypes = [ ctypes.POINTER(EvtReader), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetPos.restype = ErrorCode
    c_GetPos.errcheck = HandleErrorCode
    position = ctypes.c_uint64()
    c_GetPos(reader, ctypes.byref(position))
    return position.value

def EvtReader_SetCallbacks(reader, callbacks, userData):
    c_SetCallbacks = conf.lib.OTF2_EvtReader_SetCallbacks
    c_SetCallbacks.argtypes = [ ctypes.POINTER(EvtReader), ctypes.POINTER(EvtReaderCallbacks), ctypes.py_object ]
    c_SetCallbacks.restype = ErrorCode
    c_SetCallbacks.errcheck = HandleErrorCode
    c_SetCallbacks(reader, callbacks, ctypes.py_object(userData))

def EvtReader_Seek(reader, position):
    c_Seek = conf.lib.OTF2_EvtReader_Seek
    c_Seek.argtypes = [ ctypes.POINTER(EvtReader), ctypes.c_uint64 ]
    c_Seek.restype = ErrorCode
    c_Seek.errcheck = HandleErrorCode
    c_Seek(reader, position)

def EvtReader_ReadEvents(reader, recordsToRead):
    c_ReadEvents = conf.lib.OTF2_EvtReader_ReadEvents
    c_ReadEvents.argtypes = [ ctypes.POINTER(EvtReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadEvents.restype = ErrorCode
    c_ReadEvents.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadEvents(reader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

def EvtReader_TimeStampRewrite(reader, time):
    c_TimeStampRewrite = conf.lib.OTF2_EvtReader_TimeStampRewrite
    c_TimeStampRewrite.argtypes = [ ctypes.POINTER(EvtReader), TimeStamp ]
    c_TimeStampRewrite.restype = ErrorCode
    c_TimeStampRewrite.errcheck = HandleErrorCode
    c_TimeStampRewrite(reader, time)

def EvtReader_ReadEventsBackward(reader, recordsToRead):
    c_ReadEventsBackward = conf.lib.OTF2_EvtReader_ReadEventsBackward
    c_ReadEventsBackward.argtypes = [ ctypes.POINTER(EvtReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadEventsBackward.restype = ErrorCode
    c_ReadEventsBackward.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadEventsBackward(reader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

def EvtReader_ApplyMappingTables(reader, action):
    c_ApplyMappingTables = conf.lib.OTF2_EvtReader_ApplyMappingTables
    c_ApplyMappingTables.argtypes = [ ctypes.POINTER(EvtReader), ctypes.c_int ]
    c_ApplyMappingTables.restype = ErrorCode
    c_ApplyMappingTables.errcheck = HandleErrorCode
    c_ApplyMappingTables(reader, action)

def EvtReader_ApplyClockOffsets(reader, action):
    c_ApplyClockOffsets = conf.lib.OTF2_EvtReader_ApplyClockOffsets
    c_ApplyClockOffsets.argtypes = [ ctypes.POINTER(EvtReader), ctypes.c_int ]
    c_ApplyClockOffsets.restype = ErrorCode
    c_ApplyClockOffsets.errcheck = HandleErrorCode
    c_ApplyClockOffsets(reader, action)

__all__ = [
    'EvtReader',
    'EvtReader_ApplyClockOffsets',
    'EvtReader_ApplyMappingTables',
    'EvtReader_GetLocationID',
    'EvtReader_GetPos',
    'EvtReader_ReadEvents',
    'EvtReader_ReadEventsBackward',
    'EvtReader_Seek',
    'EvtReader_SetCallbacks',
    'EvtReader_TimeStampRewrite'
]
