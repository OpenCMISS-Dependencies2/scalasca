import ctypes

from .Config import conf, StrParam, str_decode, free
from .ErrorCodes import ErrorCode, HandleErrorCode, errcheck_not_null
from .GeneralDefinitions import Hint, LocationRef, Compression, FileSubstrate
from .Callbacks import CollectiveCallbacks, LockingCallbacks, CollectiveContext
from .EvtReaderCallbacks import EvtReaderCallbacks
from .GlobalEvtReaderCallbacks import GlobalEvtReaderCallbacks
from .GlobalSnapReaderCallbacks import GlobalSnapReaderCallbacks
from .GlobalDefReaderCallbacks import GlobalDefReaderCallbacks
from .DefReaderCallbacks import DefReaderCallbacks
from .SnapReaderCallbacks import SnapReaderCallbacks
from .MarkerReaderCallbacks import MarkerReaderCallbacks
from .GlobalEvtReader import GlobalEvtReader
from .GlobalDefReader import GlobalDefReader
from .GlobalSnapReader import GlobalSnapReader
from .SnapReader import SnapReader
from .EvtReader import EvtReader
from .DefReader import DefReader
from .MarkerReader import MarkerReader
from .MarkerWriter import MarkerWriter
from .Thumbnail import ThumbReader


class Reader(ctypes.Structure):
    pass

def Reader_Open(anchorFilePath):
    c_Open = conf.lib.OTF2_Reader_Open
    c_Open.argtypes = [ StrParam ]
    c_Open.restype = ctypes.POINTER(Reader)
    c_Open.errcheck = errcheck_not_null
    return c_Open(anchorFilePath)


def Reader_Close(reader):
    c_Close = conf.lib.OTF2_Reader_Close
    c_Close.argtypes = [ ctypes.POINTER(Reader) ]
    c_Close.restype = ErrorCode
    c_Close.errcheck = HandleErrorCode
    c_Close(reader)

def Reader_SetHint(reader, hint, value):
    c_SetHint = conf.lib.OTF2_Reader_SetHint
    c_SetHint.argtypes = [ ctypes.POINTER(Reader), Hint, ctypes.c_void_p ]
    c_SetHint.restype = ErrorCode
    c_SetHint.errcheck = HandleErrorCode
    c_SetHint(reader, hint, ctypes.byref(value))

def Reader_SetCollectiveCallbacks(reader, collectiveCallbacks, collectiveData, globalCommContext, localCommContext):
    c_SetCollectiveCallbacks = conf.lib.OTF2_Reader_SetCollectiveCallbacks
    c_SetCollectiveCallbacks.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(CollectiveCallbacks), ctypes.py_object, ctypes.POINTER(CollectiveContext), ctypes.POINTER(CollectiveContext) ]
    c_SetCollectiveCallbacks.restype = ErrorCode
    c_SetCollectiveCallbacks.errcheck = HandleErrorCode
    c_SetCollectiveCallbacks(reader, collectiveCallbacks, ctypes.py_object(collectiveData), globalCommContext, localCommContext)

def Reader_SetSerialCollectiveCallbacks(reader):
    c_SetSerialCollectiveCallbacks = conf.lib.OTF2_Reader_SetSerialCollectiveCallbacks
    c_SetSerialCollectiveCallbacks.argtypes = [ ctypes.POINTER(Reader) ]
    c_SetSerialCollectiveCallbacks.restype = ErrorCode
    c_SetSerialCollectiveCallbacks.errcheck = HandleErrorCode
    c_SetSerialCollectiveCallbacks(reader)

def Reader_SetLockingCallbacks(reader, lockingCallbacks, lockingData):
    c_SetLockingCallbacks = conf.lib.OTF2_Reader_SetLockingCallbacks
    c_SetLockingCallbacks.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(LockingCallbacks), ctypes.py_object ]
    c_SetLockingCallbacks.restype = ErrorCode
    c_SetLockingCallbacks.errcheck = HandleErrorCode
    c_SetLockingCallbacks(reader, ctypes.byref(lockingCallbacks), ctypes.py_object(lockingData))

def Reader_IsMaster(reader):
    c_IsMaster = conf.lib.OTF2_Reader_IsMaster
    c_IsMaster.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_bool) ]
    c_IsMaster.restype = ErrorCode
    c_IsMaster.errcheck = HandleErrorCode
    result = ctypes.c_bool()
    c_IsMaster(reader, ctypes.byref(result))
    return result.value

def Reader_RegisterEvtCallbacks(reader, evtReader, callbacks, userData):
    c_RegisterEvtCallbacks = conf.lib.OTF2_Reader_RegisterEvtCallbacks
    c_RegisterEvtCallbacks.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(EvtReader), ctypes.POINTER(EvtReaderCallbacks), ctypes.py_object ]
    c_RegisterEvtCallbacks.restype = ErrorCode
    c_RegisterEvtCallbacks.errcheck = HandleErrorCode
    c_RegisterEvtCallbacks(reader, evtReader, callbacks, ctypes.py_object(userData))

def Reader_RegisterGlobalEvtCallbacks(reader, evtReader, callbacks, userData):
    c_RegisterGlobalEvtCallbacks = conf.lib.OTF2_Reader_RegisterGlobalEvtCallbacks
    c_RegisterGlobalEvtCallbacks.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalEvtReader), ctypes.POINTER(GlobalEvtReaderCallbacks), ctypes.py_object ]
    c_RegisterGlobalEvtCallbacks.restype = ErrorCode
    c_RegisterGlobalEvtCallbacks.errcheck = HandleErrorCode
    c_RegisterGlobalEvtCallbacks(reader, evtReader, callbacks, ctypes.py_object(userData))

def Reader_RegisterDefCallbacks(reader, defReader, callbacks, userData):
    c_RegisterDefCallbacks = conf.lib.OTF2_Reader_RegisterDefCallbacks
    c_RegisterDefCallbacks.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(DefReader), ctypes.POINTER(DefReaderCallbacks), ctypes.py_object ]
    c_RegisterDefCallbacks.restype = ErrorCode
    c_RegisterDefCallbacks.errcheck = HandleErrorCode
    c_RegisterDefCallbacks(reader, defReader, callbacks, ctypes.py_object(userData))

def Reader_RegisterGlobalDefCallbacks(reader, defReader, callbacks, userData):
    c_RegisterGlobalDefCallbacks = conf.lib.OTF2_Reader_RegisterGlobalDefCallbacks
    c_RegisterGlobalDefCallbacks.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalDefReader), ctypes.POINTER(GlobalDefReaderCallbacks), ctypes.py_object ]
    c_RegisterGlobalDefCallbacks.restype = ErrorCode
    c_RegisterGlobalDefCallbacks.errcheck = HandleErrorCode
    c_RegisterGlobalDefCallbacks(reader, defReader, callbacks, ctypes.py_object(userData))

def Reader_RegisterSnapCallbacks(reader, snapReader, callbacks, userData):
    c_RegisterSnapCallbacks = conf.lib.OTF2_Reader_RegisterSnapCallbacks
    c_RegisterSnapCallbacks.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(SnapReader), ctypes.POINTER(SnapReaderCallbacks), ctypes.py_object ]
    c_RegisterSnapCallbacks.restype = ErrorCode
    c_RegisterSnapCallbacks.errcheck = HandleErrorCode
    c_RegisterSnapCallbacks(reader, snapReader, callbacks, ctypes.py_object(userData))

def Reader_RegisterGlobalSnapCallbacks(reader, evtReader, callbacks, userData):
    c_RegisterGlobalSnapCallbacks = conf.lib.OTF2_Reader_RegisterGlobalSnapCallbacks
    c_RegisterGlobalSnapCallbacks.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalSnapReader), ctypes.POINTER(GlobalSnapReaderCallbacks), ctypes.py_object ]
    c_RegisterGlobalSnapCallbacks.restype = ErrorCode
    c_RegisterGlobalSnapCallbacks.errcheck = HandleErrorCode
    c_RegisterGlobalSnapCallbacks(reader, evtReader, callbacks, ctypes.py_object(userData))

def Reader_RegisterMarkerCallbacks(reader, markerReader, callbacks, userData):
    c_RegisterMarkerCallbacks = conf.lib.OTF2_Reader_RegisterMarkerCallbacks
    c_RegisterMarkerCallbacks.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(MarkerReader), ctypes.POINTER(MarkerReaderCallbacks), ctypes.py_object ]
    c_RegisterMarkerCallbacks.errcheck = HandleErrorCode
    c_RegisterMarkerCallbacks(reader, markerReader, callbacks, ctypes.py_object(userData))

def Reader_ReadLocalEvents(reader, evtReader, eventsToRead):
    c_ReadLocalEvents = conf.lib.OTF2_Reader_ReadLocalEvents
    c_ReadLocalEvents.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(EvtReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadLocalEvents.restype = ErrorCode
    c_ReadLocalEvents.errcheck = HandleErrorCode
    eventsRead = ctypes.c_uint64()
    c_ReadLocalEvents(reader, evtReader, eventsToRead, ctypes.byref(eventsRead))
    return eventsRead.value

def Reader_ReadAllLocalEvents(reader, evtReader):
    c_ReadAllLocalEvents = conf.lib.OTF2_Reader_ReadAllLocalEvents
    c_ReadAllLocalEvents.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(EvtReader), ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadAllLocalEvents.restype = ErrorCode
    c_ReadAllLocalEvents.errcheck = HandleErrorCode
    eventsRead = ctypes.c_uint64()
    c_ReadAllLocalEvents(reader, evtReader, ctypes.byref(eventsRead))
    return eventsRead.value

def Reader_ReadLocalEventsBackward(reader, evtReader, eventsToRead):
    c_ReadLocalEventsBackward = conf.lib.OTF2_Reader_ReadLocalEventsBackward
    c_ReadLocalEventsBackward.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(EvtReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadLocalEventsBackward.restype = ErrorCode
    c_ReadLocalEventsBackward.errcheck = HandleErrorCode
    eventsRead = ctypes.c_uint64()
    c_ReadLocalEventsBackward(reader, evtReader, eventsToRead, ctypes.byref(eventsRead))
    return eventsRead.value

def Reader_ReadGlobalEvent(reader, evtReader):
    c_ReadGlobalEvent = conf.lib.OTF2_Reader_ReadGlobalEvent
    c_ReadGlobalEvent.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalEvtReader) ]
    c_ReadGlobalEvent.restype = ErrorCode
    c_ReadGlobalEvent.errcheck = HandleErrorCode
    c_ReadGlobalEvent(reader, evtReader)

def Reader_HasGlobalEvent(reader, evtReader):
    c_HasGlobalEvent = conf.lib.OTF2_Reader_HasGlobalEvent
    c_HasGlobalEvent.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalEvtReader), ctypes.POINTER(ctypes.c_int) ]
    c_HasGlobalEvent.restype = ErrorCode
    c_HasGlobalEvent.errcheck = HandleErrorCode
    flag = ctypes.c_int()
    c_HasGlobalEvent(reader, evtReader, ctypes.byref(flag)) #changed from byre to byref
    return flag.value

def Reader_ReadGlobalEvents(reader, evtReader, eventsToRead):
    c_ReadGlobalEvents = conf.lib.OTF2_Reader_ReadGlobalEvents
    c_ReadGlobalEvents.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalEvtReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadGlobalEvents.restype = ErrorCode
    c_ReadGlobalEvents.errcheck = HandleErrorCode
    eventsRead = ctypes.c_uint64()
    c_ReadGlobalEvents(reader, evtReader, eventsToRead, ctypes.byref(eventsRead))
    return eventsRead.value

def Reader_ReadAllGlobalEvents(reader, evtReader):
    c_ReadAllGlobalEvents = conf.lib.OTF2_Reader_ReadAllGlobalEvents
    c_ReadAllGlobalEvents.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalEvtReader), ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadAllGlobalEvents.restype = ErrorCode
    c_ReadAllGlobalEvents.errcheck = HandleErrorCode
    eventsRead = ctypes.c_uint64()
    c_ReadAllGlobalEvents(reader, evtReader, ctypes.byref(eventsRead))
    return eventsRead.value

def Reader_ReadLocalDefinitions(reader, defReader, definitionsToRead):
    c_ReadLocalDefinitions = conf.lib.OTF2_Reader_ReadLocalDefinitions
    c_ReadLocalDefinitions.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(DefReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadLocalDefinitions.restype = ErrorCode
    c_ReadLocalDefinitions.errcheck = HandleErrorCode
    definitionsRead = ctypes.c_uint64()
    c_ReadLocalDefinitions(reader, defReader, definitionsToRead, ctypes.byref(definitionsRead))
    return definitionsRead.value

def Reader_ReadAllLocalDefinitions(reader, defReader):
    c_ReadAllLocalDefinitions = conf.lib.OTF2_Reader_ReadAllLocalDefinitions
    c_ReadAllLocalDefinitions.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(DefReader), ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadAllLocalDefinitions.restype = ErrorCode
    c_ReadAllLocalDefinitions.errcheck = HandleErrorCode
    definitionsRead = ctypes.c_uint64()
    c_ReadAllLocalDefinitions(reader, defReader, ctypes.byref(definitionsRead))
    return definitionsRead.value

def Reader_ReadGlobalDefinitions(reader, defReader, definitionsToRead):
    c_ReadGlobalDefinitions = conf.lib.OTF2_Reader_ReadGlobalDefinitions
    c_ReadGlobalDefinitions.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalDefReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadGlobalDefinitions.restype = ErrorCode
    c_ReadGlobalDefinitions.errcheck = HandleErrorCode
    definitionsRead = ctypes.c_uint64()
    c_ReadGlobalDefinitions(reader, defReader, definitionsToRead, ctypes.byref(definitionsRead))
    return definitionsRead.value

def Reader_ReadAllGlobalDefinitions(reader, defReader):
    c_ReadAllGlobalDefinitions = conf.lib.OTF2_Reader_ReadAllGlobalDefinitions
    c_ReadAllGlobalDefinitions.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalDefReader), ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadAllGlobalDefinitions.restype = ErrorCode
    c_ReadAllGlobalDefinitions.errcheck = HandleErrorCode
    definitionsRead = ctypes.c_uint64()
    c_ReadAllGlobalDefinitions(reader, defReader, ctypes.byref(definitionsRead))
    return definitionsRead.value

def Reader_ReadLocalSnapshots(reader, snapReader, recordsToRead):
    c_ReadLocalSnapshots = conf.lib.OTF2_Reader_ReadLocalSnapshots
    c_ReadLocalSnapshots.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(SnapReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadLocalSnapshots.restype = ErrorCode
    c_ReadLocalSnapshots.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadLocalSnapshots(reader, snapReader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

def Reader_ReadAllLocalSnapshots(reader, snapReader):
    c_ReadAllLocalSnapshots = conf.lib.OTF2_Reader_ReadAllLocalSnapshots
    c_ReadAllLocalSnapshots.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(SnapReader), ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadAllLocalSnapshots.restype = ErrorCode
    c_ReadAllLocalSnapshots.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadAllLocalSnapshots(reader, snapReader, ctypes.byref(recordsRead))
    return recordsRead.value

def Reader_ReadGlobalSnapshots(reader, snapReader, recordsToRead):
    c_ReadGlobalSnapshots = conf.lib.OTF2_Reader_ReadGlobalSnapshots
    c_ReadGlobalSnapshots.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalSnapReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadGlobalSnapshots.restype = ErrorCode
    c_ReadGlobalSnapshots.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadGlobalSnapshots(reader, snapReader, recordsToRead, ctypes.byref(recordsRead))
    return recordsRead.value

def Reader_ReadAllGlobalSnapshots(reader, snapReader):
    c_ReadAllGlobalSnapshots = conf.lib.OTF2_Reader_ReadAllGlobalSnapshots
    c_ReadAllGlobalSnapshots.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalSnapReader), ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadAllGlobalSnapshots.restype = ErrorCode
    c_ReadAllGlobalSnapshots.errcheck = HandleErrorCode
    recordsRead = ctypes.c_uint64()
    c_ReadAllGlobalSnapshots(reader, snapReader, ctypes.byref(recordsRead))
    return recordsRead.value

def Reader_ReadMarkers(reader, markerReader, markersToRead):
    c_ReadMarkers = conf.lib.OTF2_Reader_ReadMarkers
    c_ReadMarkers.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(MarkerReader), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadMarkers.restype = ErrorCode
    c_ReadMarkers.errcheck = HandleErrorCode
    markersRead = ctypes.c_uint8()
    c_ReadMarkers(reader, markerReader, markersToRead, ctypes.byref(markersRead))
    return markersRead.value

def Reader_ReadAllMarkers(reader, markerReader):
    c_ReadAllMarkers = conf.lib.OTF2_Reader_ReadAllMarkers
    c_ReadAllMarkers.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(MarkerReader), ctypes.POINTER(ctypes.c_uint64) ]
    c_ReadAllMarkers.restype = ErrorCode
    c_ReadAllMarkers.errcheck = HandleErrorCode
    markersRead = ctypes.c_uint8()
    c_ReadAllMarkers(reader, markerReader, ctypes.byref(markersRead))
    return markersRead.value

def Reader_GetEvtReader(reader, location):
    c_GetEvtReader = conf.lib.OTF2_Reader_GetEvtReader
    c_GetEvtReader.argtypes = [ ctypes.POINTER(Reader), LocationRef ]
    c_GetEvtReader.restype = ctypes.POINTER(EvtReader)
    return c_GetEvtReader(reader, location)

def Reader_GetGlobalEvtReader(reader):
    c_GetGlobalEvtReader = conf.lib.OTF2_Reader_GetGlobalEvtReader
    c_GetGlobalEvtReader.argtypes = [ ctypes.POINTER(Reader) ]
    c_GetGlobalEvtReader.restype = ctypes.POINTER(GlobalEvtReader)
    return c_GetGlobalEvtReader(reader)

def Reader_GetDefReader(reader, location):
    c_GetDefReader = conf.lib.OTF2_Reader_GetDefReader
    c_GetDefReader.argtypes = [ ctypes.POINTER(Reader), LocationRef ]
    c_GetDefReader.restype = ctypes.POINTER(DefReader)
    return c_GetDefReader(reader, location)

def Reader_GetGlobalDefReader(reader):
    c_GetGlobalDefReader = conf.lib.OTF2_Reader_GetGlobalDefReader
    c_GetGlobalDefReader.argtypes = [ ctypes.POINTER(Reader) ]
    c_GetGlobalDefReader.restype = ctypes.POINTER(GlobalDefReader)
    return c_GetGlobalDefReader(reader)

def Reader_GetSnapReader(reader, location):
    c_GetSnapReader = conf.lib.OTF2_Reader_GetSnapReader
    c_GetSnapReader.argtypes = [ ctypes.POINTER(Reader), LocationRef ]
    c_GetSnapReader.restype = ctypes.POINTER(SnapReader)
    return c_GetSnapReader(reader, location)

def Reader_GetGlobalSnapReader(reader):
    c_GetGlobalSnapReader = conf.lib.OTF2_Reader_GetGlobalSnapReader
    c_GetGlobalSnapReader.argtypes = [ ctypes.POINTER(Reader) ]
    c_GetGlobalSnapReader.restype = ctypes.POINTER(GlobalSnapReader)
    return c_GetGlobalSnapReader(reader)

def Reader_GetThumbReader(reader, number):
    c_GetThumbReader = conf.lib.OTF2_Reader_GetThumbReader
    c_GetThumbReader.argtypes = [ ctypes.POINTER(Reader), ctypes.c_uint32 ]
    c_GetThumbReader.restype = ctypes.POINTER(ThumbReader)
    return c_GetThumbReader(reader, number)

def Reader_GetMarkerReader(reader):
    c_GetMarkerReader = conf.lib.OTF2_Reader_GetMarkerReader
    c_GetMarkerReader.argtypes = [ ctypes.POINTER(Reader) ]
    c_GetMarkerReader.restype = ctypes.POINTER(MarkerReader)
    return c_GetMarkerReader(reader)

def Reader_GetMarkerWriter(reader):
    c_GetMarkerWriter = conf.lib.OTF2_Reader_GetMarkerWriter
    c_GetMarkerWriter.argtypes = [ ctypes.POINTER(Reader) ]
    c_GetMarkerWriter.restype = ctypes.POINTER(MarkerWriter)
    return c_GetMarkerWriter(reader)

def Reader_CloseEvtReader(reader, evtReader):
    c_CloseEvtReader = conf.lib.OTF2_Reader_CloseEvtReader
    c_CloseEvtReader.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(EvtReader) ]
    c_CloseEvtReader.restype = ErrorCode
    c_CloseEvtReader.errcheck = HandleErrorCode
    c_CloseEvtReader(reader, evtReader)

def Reader_CloseGlobalEvtReader(reader, globalEvtReader):
    c_CloseGlobalEvtReader = conf.lib.OTF2_Reader_CloseGlobalEvtReader
    c_CloseGlobalEvtReader.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalEvtReader) ]
    c_CloseGlobalEvtReader.restype = ErrorCode
    c_CloseGlobalEvtReader.errcheck = HandleErrorCode
    c_CloseGlobalEvtReader(reader, globalEvtReader)

def Reader_CloseDefReader(reader, defReader):
    c_CloseDefReader = conf.lib.OTF2_Reader_CloseDefReader
    c_CloseDefReader.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(DefReader) ]
    c_CloseDefReader.restype = ErrorCode
    c_CloseDefReader.errcheck = HandleErrorCode
    c_CloseDefReader(reader, defReader)

def Reader_CloseGlobalDefReader(reader, globalDefReader):
    c_CloseGlobalDefReader = conf.lib.OTF2_Reader_CloseGlobalDefReader
    c_CloseGlobalDefReader.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalDefReader) ]
    c_CloseGlobalDefReader.restype = ErrorCode
    c_CloseGlobalDefReader.errcheck = HandleErrorCode
    c_CloseGlobalDefReader(reader, globalDefReader)

def Reader_CloseSnapReader(reader, snapReader):
    c_CloseSnapReader = conf.lib.OTF2_Reader_CloseSnapReader
    c_CloseSnapReader.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(SnapReader) ]
    c_CloseSnapReader.restype = ErrorCode
    c_CloseSnapReader.errcheck = HandleErrorCode
    c_CloseSnapReader(reader, snapReader)

def Reader_CloseGlobalSnapReader(reader, globalSnapReader):
    c_CloseGlobalSnapReader = conf.lib.OTF2_Reader_CloseGlobalSnapReader
    c_CloseGlobalSnapReader.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(GlobalSnapReader) ]
    c_CloseGlobalSnapReader.restype = ErrorCode
    c_CloseGlobalSnapReader.errcheck = HandleErrorCode
    c_CloseGlobalSnapReader(reader, globalSnapReader)

def Reader_CloseThumbReader(reader, thumbReader):
    c_CloseThumbReader = conf.lib.OTF2_Reader_CloseThumbReader
    c_CloseThumbReader.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ThumbReader) ]
    c_CloseThumbReader.restype = ErrorCode
    c_CloseThumbReader.errcheck = HandleErrorCode
    c_CloseThumbReader(reader, thumbReader)

def Reader_CloseMarkerReader(reader, markerReader):
    c_CloseMarkerReader = conf.lib.OTF2_Reader_CloseMarkerReader
    c_CloseMarkerReader.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(MarkerReader) ]
    c_CloseMarkerReader.restype = ErrorCode
    c_CloseMarkerReader.errcheck = HandleErrorCode
    c_CloseMarkerReader(reader, markerReader)

def Reader_CloseMarkerWriter(reader, markerWriter):
    c_CloseMarkerWriter = conf.lib.OTF2_Reader_CloseMarkerWriter
    c_CloseMarkerWriter.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(MarkerWriter) ]
    c_CloseMarkerWriter.restype = ErrorCode
    c_CloseMarkerWriter.errcheck = HandleErrorCode
    c_CloseMarkerWriter(reader, markerWriter)

def Reader_GetVersion(reader):
    c_GetVersion = conf.lib.OTF2_Reader_GetVersion
    c_GetVersion.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_uint8), ctypes.POINTER(ctypes.c_uint8), ctypes.POINTER(ctypes.c_uint8) ]
    c_GetVersion.restype = ErrorCode
    c_GetVersion.errcheck = HandleErrorCode
    major = ctypes.c_uint8()
    minor = ctypes.c_uint8()
    bugfix  = ctypes.c_uint8()
    c_GetVersion(reader, ctypes.byref(major), ctypes.byref(minor), ctypes.byref(bugfix))
    return (major.value, minor.value, bugfix.value)

def Reader_GetChunkSize(reader):
    c_GetChunkSize = conf.lib.OTF2_Reader_GetChunkSize
    c_GetChunkSize.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_uint64), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetChunkSize.restype = ErrorCode
    c_GetChunkSize.errcheck = HandleErrorCode
    chunkSizeEvents = ctypes.c_uint64()
    chunkSizeDefinitions = ctypes.c_uint64()
    c_GetChunkSize(reader, ctypes.byref(chunkSizeEvents), ctypes.byref(chunkSizeDefinitions))
    return (chunkSizeEvents.value, chunkSizeDefinitions.value)

def Reader_GetFileSubstrate(reader):
    c_GetFileSubstrate = conf.lib.OTF2_Reader_GetFileSubstrate
    c_GetFileSubstrate.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(FileSubstrate) ]
    c_GetFileSubstrate.restype = ErrorCode
    c_GetFileSubstrate.errcheck = HandleErrorCode
    substrate = FileSubstrate()
    c_GetFileSubstrate(reader, ctypes.byref(substrate))
    return substrate

def Reader_GetCompression(reader):
    c_GetCompression = conf.lib.OTF2_Reader_GetCompression
    c_GetCompression.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(Compression) ]
    c_GetCompression.restype = ErrorCode
    c_GetCompression.errcheck = HandleErrorCode
    compression = Compression()
    c_GetCompression(reader, ctypes.byref(compression))
    return compression

def Reader_GetNumberOfLocations(reader):
    c_GetNumberOfLocations = conf.lib.OTF2_Reader_GetNumberOfLocations
    c_GetNumberOfLocations.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetNumberOfLocations.restype = ErrorCode
    c_GetNumberOfLocations.errcheck = HandleErrorCode
    numberOfLocations = ctypes.c_uint64()
    c_GetNumberOfLocations(reader, ctypes.byref(numberOfLocations))
    return numberOfLocations.value

def Reader_GetNumberOfGlobalDefinitions(reader):
    c_GetNumberOfGlobalDefinitions = conf.lib.OTF2_Reader_GetNumberOfGlobalDefinitions
    c_GetNumberOfGlobalDefinitions.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetNumberOfGlobalDefinitions.restype = ErrorCode
    c_GetNumberOfGlobalDefinitions.errcheck = HandleErrorCode
    numberOfDefinitions = ctypes.c_uint64()
    c_GetNumberOfGlobalDefinitions(reader, ctypes.byref(numberOfDefinitions))
    return numberOfDefinitions.value

def Reader_GetMachineName(reader):
    c_GetMachineName = conf.lib.OTF2_Reader_GetMachineName
    c_GetMachineName.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_char_p) ]
    c_GetMachineName.restype = ErrorCode
    c_GetMachineName.errcheck = HandleErrorCode
    machineName = ctypes.c_char_p()

    c_GetMachineName(reader, ctypes.byref(machineName))

    result = str_decode(machineName.value)
    free(machineName)

    return result

def Reader_GetCreator(reader):
    c_GetCreator = conf.lib.OTF2_Reader_GetCreator
    c_GetCreator.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_char_p) ]
    c_GetCreator.restype = ErrorCode
    c_GetCreator.errcheck = HandleErrorCode
    creator = ctypes.c_char_p()

    c_GetCreator(reader, ctypes.byref(creator))

    result = str_decode(creator.value)
    free(creator)

    return result

def Reader_GetDescription(reader):
    c_GetDescription = conf.lib.OTF2_Reader_GetDescription
    c_GetDescription.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_char_p) ]
    c_GetDescription.restype = ErrorCode
    c_GetDescription.errcheck = HandleErrorCode
    description = ctypes.c_char_p()

    c_GetDescription(reader, ctypes.byref(description))

    result = str_decode(description.value)
    free(description)

    return result

def Reader_GetPropertyNames(reader):
    c_GetPropertyNames = conf.lib.OTF2_Reader_GetPropertyNames
    c_GetPropertyNames.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_uint32), ctypes.POINTER(ctypes.POINTER(ctypes.c_char_p)) ]
    c_GetPropertyNames.restype = ErrorCode
    c_GetPropertyNames.errcheck = HandleErrorCode
    numberOfProperties = ctypes.c_uint32()
    names = ctypes.POINTER(ctypes.c_char_p)()

    c_GetPropertyNames(reader, ctypes.byref(numberOfProperties), ctypes.byref(names))

    result = [str_decode(names[i]) for i in range(numberOfProperties.value)]
    free(names)

    return result

def Reader_GetProperty(reader, name):
    c_GetProperty = conf.lib.OTF2_Reader_GetProperty
    c_GetProperty.argtypes = [ ctypes.POINTER(Reader), StrParam, ctypes.POINTER(ctypes.c_char_p) ]
    c_GetProperty.restype = ErrorCode
    c_GetProperty.errcheck = HandleErrorCode
    value = ctypes.c_char_p()
    c_GetProperty(reader, name, ctypes.byref(value))

    result = str_decode(value.value)
    free(value)

    return result

def Reader_GetBoolProperty(reader, name):
    c_GetBoolProperty = conf.lib.OTF2_Reader_GetBoolProperty
    c_GetBoolProperty.argtypes = [ ctypes.POINTER(Reader), StrParam, ctypes.POINTER(ctypes.c_bool) ]
    c_GetBoolProperty.restype = ErrorCode
    c_GetBoolProperty.errcheck = HandleErrorCode
    value = ctypes.c_bool()
    c_GetBoolProperty(reader, name, ctypes.byref(value))
    return value.value

def Reader_GetTraceId(reader):
    c_GetTraceId = conf.lib.OTF2_Reader_GetTraceId
    c_GetTraceId.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetTraceId.restype = ErrorCode
    c_GetTraceId.errcheck = HandleErrorCode
    traceId = ctypes.c_uint64()
    c_GetTraceId(reader, ctypes.byref(traceId))
    return traceId.value

def Reader_GetNumberOfSnapshots(reader):
    c_GetNumberOfSnapshots = conf.lib.OTF2_Reader_GetNumberOfSnapshots
    c_GetNumberOfSnapshots.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_uint32) ]
    c_GetNumberOfSnapshots.restype = ErrorCode
    c_GetNumberOfSnapshots.errcheck = HandleErrorCode
    number = ctypes.c_uint32()
    c_GetNumberOfSnapshots(reader, ctypes.byref(number))
    return number.value

def Reader_GetNumberOfThumbnails(reader):
    c_GetNumberOfThumbnails = conf.lib.OTF2_Reader_GetNumberOfThumbnails
    c_GetNumberOfThumbnails.argtypes = [ ctypes.POINTER(Reader), ctypes.POINTER(ctypes.c_uint32) ]
    c_GetNumberOfThumbnails.restype = ErrorCode
    c_GetNumberOfThumbnails.errcheck = HandleErrorCode
    number = ctypes.c_uint32()
    c_GetNumberOfThumbnails(reader, ctypes.byref(number))
    return number.value

def Reader_OpenEvtFiles(reader):
    c_OpenEvtFiles = conf.lib.OTF2_Reader_OpenEvtFiles
    c_OpenEvtFiles.argtypes = [ ctypes.POINTER(Reader) ]
    c_OpenEvtFiles.restype = ErrorCode
    c_OpenEvtFiles.errcheck = HandleErrorCode
    c_OpenEvtFiles(reader)

def Reader_CloseEvtFiles(reader):
    c_CloseEvtFiles = conf.lib.OTF2_Reader_CloseEvtFiles
    c_CloseEvtFiles.argtypes = [ ctypes.POINTER(Reader) ]
    c_CloseEvtFiles.restype = ErrorCode
    c_CloseEvtFiles.errcheck = HandleErrorCode
    c_CloseEvtFiles(reader)

def Reader_OpenDefFiles(reader):
    c_OpenDefFiles = conf.lib.OTF2_Reader_OpenDefFiles
    c_OpenDefFiles.argtypes = [ ctypes.POINTER(Reader) ]
    c_OpenDefFiles.restype = ErrorCode
    c_OpenDefFiles.errcheck = HandleErrorCode
    c_OpenDefFiles(reader)

def Reader_CloseDefFiles(reader):
    c_CloseDefFiles = conf.lib.OTF2_Reader_CloseDefFiles
    c_CloseDefFiles.argtypes = [ ctypes.POINTER(Reader) ]
    c_CloseDefFiles.restype = ErrorCode
    c_CloseDefFiles.errcheck = HandleErrorCode
    c_CloseDefFiles(reader)

def Reader_OpenSnapFiles(reader):
    c_OpenSnapFiles = conf.lib.OTF2_Reader_OpenSnapFiles
    c_OpenSnapFiles.argtypes = [ ctypes.POINTER(Reader) ]
    c_OpenSnapFiles.restype = ErrorCode
    c_OpenSnapFiles.errcheck = HandleErrorCode
    c_OpenSnapFiles(reader)

def Reader_CloseSnapFiles(reader):
    c_CloseSnapFiles = conf.lib.OTF2_Reader_CloseSnapFiles
    c_CloseSnapFiles.argtypes = [ ctypes.POINTER(Reader) ]
    c_CloseSnapFiles.restype = ErrorCode
    c_CloseSnapFiles.errcheck = HandleErrorCode
    c_CloseSnapFiles(reader)

def Reader_SelectLocation(reader, location):
    c_SelectLocation = conf.lib.OTF2_Reader_SelectLocation
    c_SelectLocation.argtypes = [ ctypes.POINTER(Reader), LocationRef ]
    c_SelectLocation.restype = ErrorCode
    c_SelectLocation.errcheck = HandleErrorCode
    c_SelectLocation(reader, location)


__all__ = [
    'Reader',
    'Reader_Close',
    'Reader_CloseDefFiles',
    'Reader_CloseDefReader',
    'Reader_CloseEvtFiles',
    'Reader_CloseEvtReader',
    'Reader_CloseGlobalDefReader',
    'Reader_CloseGlobalEvtReader',
    'Reader_CloseGlobalSnapReader',
    'Reader_CloseMarkerReader',
    'Reader_CloseMarkerWriter',
    'Reader_CloseSnapFiles',
    'Reader_CloseSnapReader',
    'Reader_CloseThumbReader',
    'Reader_GetBoolProperty',
    'Reader_GetChunkSize',
    'Reader_GetCompression',
    'Reader_GetCreator',
    'Reader_GetDefReader',
    'Reader_GetDescription',
    'Reader_GetEvtReader',
    'Reader_GetFileSubstrate',
    'Reader_GetGlobalDefReader',
    'Reader_GetGlobalEvtReader',
    'Reader_GetGlobalSnapReader',
    'Reader_GetMachineName',
    'Reader_GetMarkerReader',
    'Reader_GetMarkerWriter',
    'Reader_GetNumberOfGlobalDefinitions',
    'Reader_GetNumberOfLocations',
    'Reader_GetNumberOfSnapshots',
    'Reader_GetNumberOfThumbnails',
    'Reader_GetProperty',
    'Reader_GetPropertyNames',
    'Reader_GetSnapReader',
    'Reader_GetThumbReader',
    'Reader_GetTraceId',
    'Reader_GetVersion',
    'Reader_HasGlobalEvent',
    'Reader_IsMaster',
    'Reader_Open',
    'Reader_OpenDefFiles',
    'Reader_OpenEvtFiles',
    'Reader_OpenSnapFiles',
    'Reader_ReadAllGlobalDefinitions',
    'Reader_ReadAllGlobalEvents',
    'Reader_ReadAllGlobalSnapshots',
    'Reader_ReadAllLocalDefinitions',
    'Reader_ReadAllLocalEvents',
    'Reader_ReadAllLocalSnapshots',
    'Reader_ReadAllMarkers',
    'Reader_ReadGlobalDefinitions',
    'Reader_ReadGlobalEvent',
    'Reader_ReadGlobalEvents',
    'Reader_ReadGlobalSnapshots',
    'Reader_ReadLocalDefinitions',
    'Reader_ReadLocalEvents',
    'Reader_ReadLocalEventsBackward',
    'Reader_ReadLocalSnapshots',
    'Reader_ReadMarkers',
    'Reader_RegisterDefCallbacks',
    'Reader_RegisterEvtCallbacks',
    'Reader_RegisterGlobalDefCallbacks',
    'Reader_RegisterGlobalEvtCallbacks',
    'Reader_RegisterGlobalSnapCallbacks',
    'Reader_RegisterMarkerCallbacks',
    'Reader_RegisterSnapCallbacks',
    'Reader_SelectLocation',
    'Reader_SetCollectiveCallbacks',
    'Reader_SetHint',
    'Reader_SetLockingCallbacks',
    'Reader_SetSerialCollectiveCallbacks'
]
