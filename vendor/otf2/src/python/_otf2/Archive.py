import ctypes

from .Config import conf, StrParam, str_decode, free
from .ErrorCodes import ErrorCode, HandleErrorCode, errcheck_not_null
from .GeneralDefinitions import Compression, FileSubstrate, Hint, FileMode, LocationRef, ThumbnailType
from .Callbacks import FlushCallbacks, LockingCallbacks, MemoryCallbacks, CollectiveCallbacks, CollectiveContext
from .DefWriter import DefWriter
from .DefReader import DefReader
from .EvtWriter import EvtWriter
from .EvtReader import EvtReader
from .SnapWriter import SnapWriter
from .SnapReader import SnapReader
from .GlobalDefWriter import GlobalDefWriter
from .GlobalDefReader import GlobalDefReader
from .GlobalEvtReader import GlobalEvtReader
from .GlobalSnapReader import GlobalSnapReader
from .Thumbnail import ThumbWriter, ThumbReader
from .MarkerWriter import MarkerWriter
from .MarkerReader import MarkerReader


class Archive(ctypes.Structure):
    pass

def Archive_Open(archivePath, archiveName, fileMode, chunkSizeEvents, chunkSizeDefs, fileSubstrate, compression):
    c_Open = conf.lib.OTF2_Archive_Open
    c_Open.argtypes = [ StrParam, StrParam, FileMode, ctypes.c_uint64, ctypes.c_uint64, FileSubstrate, Compression ]
    c_Open.restype = ctypes.POINTER(Archive)
    c_Open.errcheck = errcheck_not_null
    return c_Open(archivePath, archiveName, fileMode, chunkSizeEvents, chunkSizeDefs, fileSubstrate, compression)

def Archive_Close(archive):
    c_Close = conf.lib.OTF2_Archive_Close
    c_Close.argtypes = [ ctypes.POINTER(Archive) ]
    c_Close.restype = ErrorCode
    c_Close.errcheck = HandleErrorCode
    c_Close(archive)

def Archive_SwitchFileMode(archive, newFileMode):
    c_SwitchFileMode = conf.lib.OTF2_Archive_SwitchFileMode
    c_SwitchFileMode.argtypes = [ ctypes.POINTER(Archive), FileMode ]
    c_SwitchFileMode.restype = ErrorCode
    c_SwitchFileMode.errcheck = HandleErrorCode
    c_SwitchFileMode(archive, newFileMode)

def Archive_SetDefChunkSize(archive, chunkSizeDefs):
    c_SetDefChunkSize = conf.lib.OTF2_Archive_SetDefChunkSize
    c_SetDefChunkSize.argtypes = [ ctypes.POINTER(Archive), ctypes.c_uint64 ]
    c_SetDefChunkSize.restype = ErrorCode
    c_SetDefChunkSize.errcheck = HandleErrorCode
    c_SetDefChunkSize(archive, chunkSizeDefs)

def Archive_SetMachineName(archive, machineName):
    c_SetMachineName = conf.lib.OTF2_Archive_SetMachineName
    c_SetMachineName.argtypes = [ ctypes.POINTER(Archive), StrParam ]
    c_SetMachineName.restype = ErrorCode
    c_SetMachineName.errcheck = HandleErrorCode
    c_SetMachineName(archive, machineName)

def Archive_SetDescription(archive, description):
    c_SetDescription = conf.lib.OTF2_Archive_SetDescription
    c_SetDescription.argtypes = [ ctypes.POINTER(Archive), StrParam ]
    c_SetDescription.restype = ErrorCode
    c_SetDescription.errcheck = HandleErrorCode
    c_SetDescription(archive, description)

def Archive_SetCreator(archive, creator):
    c_SetCreator = conf.lib.OTF2_Archive_SetCreator
    c_SetCreator.argtypes = [ ctypes.POINTER(Archive), StrParam ]
    c_SetCreator.restype = ErrorCode
    c_SetCreator.errcheck = HandleErrorCode
    c_SetCreator(archive, creator)

def Archive_SetFlushCallbacks(archive, flushCallbacks, flushData):
    c_SetFlushCallbacks = conf.lib.OTF2_Archive_SetFlushCallbacks
    c_SetFlushCallbacks.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(FlushCallbacks), ctypes.py_object ]
    c_SetFlushCallbacks.restype = ErrorCode
    c_SetFlushCallbacks.errcheck = HandleErrorCode
    c_SetFlushCallbacks(archive, ctypes.byref(flushCallbacks), ctypes.py_object(flushData))

def Archive_SetMemoryCallbacks(archive, memoryCallbacks, memoryData):
    c_SetMemoryCallbacks = conf.lib.OTF2_Archive_SetMemoryCallbacks
    c_SetMemoryCallbacks.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(MemoryCallbacks), ctypes.py_object ]
    c_SetMemoryCallbacks.restype = ErrorCode
    c_SetMemoryCallbacks.errcheck = HandleErrorCode
    c_SetMemoryCallbacks(archive, ctypes.byref(memoryCallbacks), ctypes.py_object(memoryData))

def Archive_SetCollectiveCallbacks(archive, collectiveCallbacks, collectiveData, globalCommContext, localCommContext):
    c_SetCollectiveCallbacks = conf.lib.OTF2_Archive_SetCollectiveCallbacks
    c_SetCollectiveCallbacks.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(CollectiveCallbacks), ctypes.py_object, ctypes.POINTER(CollectiveContext), ctypes.POINTER(CollectiveContext) ]
    c_SetCollectiveCallbacks.restype = ErrorCode
    c_SetCollectiveCallbacks.errcheck = HandleErrorCode
    c_SetCollectiveCallbacks(archive, ctypes.byref(collectiveCallbacks), ctypes.py_object(collectiveData), ctypes.byref(globalCommContext), ctypes.byref(localCommContext))

def Archive_SetSerialCollectiveCallbacks(archive):
    c_SetSerialCollectiveCallbacks = conf.lib.OTF2_Archive_SetSerialCollectiveCallbacks
    c_SetSerialCollectiveCallbacks.argtypes = [ ctypes.POINTER(Archive) ]
    c_SetSerialCollectiveCallbacks.restype = ErrorCode
    c_SetSerialCollectiveCallbacks.errcheck = HandleErrorCode
    c_SetSerialCollectiveCallbacks(archive)

def Archive_IsMaster(archive):
    c_IsMaster = conf.lib.OTF2_Archive_IsMaster
    c_IsMaster.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_bool) ]
    c_IsMaster.restype = ErrorCode
    c_IsMaster.errcheck = HandleErrorCode
    result = ctypes.c_bool()
    c_IsMaster(archive, ctypes.byref(result))
    return result.value

def Archive_SetLockingCallbacks(archive, lockingCallbacks, lockingData):
    c_SetLockingCallbacks = conf.lib.OTF2_Archive_SetLockingCallbacks
    c_SetLockingCallbacks.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(LockingCallbacks), ctypes.py_object ]
    c_SetLockingCallbacks.restype = ErrorCode
    c_SetLockingCallbacks.errcheck = HandleErrorCode
    c_SetLockingCallbacks(archive, ctypes.byref(lockingCallbacks), ctypes.py_object(lockingData))

def Archive_SetHint(archive, hint, value):
    c_SetHint = conf.lib.OTF2_Archive_SetHint
    c_SetHint.argtypes = [ ctypes.POINTER(Archive), Hint, ctypes.c_void_p ]
    c_SetHint.restype = ErrorCode
    c_SetHint.errcheck = HandleErrorCode
    c_SetHint(archive, hint, ctypes.byref(value))

def Archive_SetProperty(archive, name, value, overwrite):
    c_SetProperty = conf.lib.OTF2_Archive_SetProperty
    c_SetProperty.argtypes = [ ctypes.POINTER(Archive), StrParam, StrParam, ctypes.c_bool ]
    c_SetProperty.restype = ErrorCode
    c_SetProperty.errcheck = HandleErrorCode
    c_SetProperty(archive, name, value, overwrite)

def Archive_SetBoolProperty(archive, name, value, overwrite):
    c_SetBoolProperty = conf.lib.OTF2_Archive_SetBoolProperty
    c_SetBoolProperty.argtypes = [ ctypes.POINTER(Archive), StrParam, ctypes.c_bool, ctypes.c_bool ]
    c_SetBoolProperty.restype = ErrorCode
    c_SetBoolProperty.errcheck = HandleErrorCode
    c_SetBoolProperty(archive, name, ctypes.c_bool(value), ctypes.c_bool(overwrite))

def Archive_GetPropertyNames(archive):
    c_GetPropertyNames = conf.lib.OTF2_Archive_GetPropertyNames
    c_GetPropertyNames.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_uint32), ctypes.POINTER(ctypes.POINTER(ctypes.c_char_p)) ]
    c_GetPropertyNames.restype = ErrorCode
    c_GetPropertyNames.errcheck = HandleErrorCode
    numberOfProperties = ctypes.c_uint32()
    names = ctypes.POINTER(ctypes.c_char_p)()

    c_GetPropertyNames(archive, ctypes.byref(numberOfProperties), ctypes.byref(names))

    result = [str_decode(names[i]) for i in range(numberOfProperties.value)]
    free(names)

    return result

def Archive_GetProperty(archive, name):
    c_GetProperty = conf.lib.OTF2_Archive_GetProperty
    c_GetProperty.argtypes = [ ctypes.POINTER(Archive), StrParam, ctypes.POINTER(ctypes.c_char_p) ]
    c_GetProperty.restype = ErrorCode
    c_GetProperty.errcheck = HandleErrorCode
    value = ctypes.c_char_p()

    c_GetProperty(archive, name, ctypes.byref(value))

    result = str_decode(value.value)
    free(value)

    return result

def Archive_GetTraceId(archive):
    c_GetTraceId = conf.lib.OTF2_Archive_GetTraceId
    c_GetTraceId.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetTraceId.restype = ErrorCode
    c_GetTraceId.errcheck = HandleErrorCode
    trace_id = ctypes.c_uint64()
    c_GetTraceId(archive, ctypes.byref(trace_id))
    return trace_id.value

def Archive_GetNumberOfLocations(archive):
    c_GetNumberOfLocations = conf.lib.OTF2_Archive_GetNumberOfLocations
    c_GetNumberOfLocations.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetNumberOfLocations.restype = ErrorCode
    c_GetNumberOfLocations.errcheck = HandleErrorCode
    numberOfLocations = ctypes.c_uint64()
    c_GetNumberOfLocations(archive, ctypes.byref(numberOfLocations))
    return numberOfLocations.value

def Archive_GetNumberOfGlobalDefinitions(archive):
    c_GetNumberOfGlobalDefinitions = conf.lib.OTF2_Archive_GetNumberOfGlobalDefinitions
    c_GetNumberOfGlobalDefinitions.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetNumberOfGlobalDefinitions.restype = ErrorCode
    c_GetNumberOfGlobalDefinitions.errcheck = HandleErrorCode
    numberOfDefinitions = ctypes.c_uint64()
    c_GetNumberOfGlobalDefinitions(archive, ctypes.byref(numberOfDefinitions))
    return numberOfDefinitions.value

def Archive_GetMachineName(archive):
    c_GetMachineName = conf.lib.OTF2_Archive_GetMachineName
    c_GetMachineName.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_char_p) ]
    c_GetMachineName.restype = ErrorCode
    c_GetMachineName.errcheck = HandleErrorCode
    machineName = ctypes.c_char_p()

    c_GetMachineName(archive, ctypes.byref(machineName))

    result = str_decode(machineName.value)
    free(machineName)

    return result

def Archive_GetDescription(archive):
    c_GetDescription = conf.lib.OTF2_Archive_GetDescription
    c_GetDescription.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_char_p) ]
    c_GetDescription.restype = ErrorCode
    c_GetDescription.errcheck = HandleErrorCode
    description = ctypes.c_char_p()

    c_GetDescription(archive, ctypes.byref(description))

    result = str_decode(description.value)
    free(description)

    return result

def Archive_GetCreator(archive):
    c_GetCreator = conf.lib.OTF2_Archive_GetCreator
    c_GetCreator.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_char_p) ]
    c_GetCreator.restype = ErrorCode
    c_GetCreator.errcheck = HandleErrorCode
    creator = ctypes.c_char_p()

    c_GetCreator(archive, ctypes.byref(creator))

    result = str_decode(creator.value)
    free(creator)

    return result

def Archive_GetVersion(archive):
    c_GetVersion = conf.lib.OTF2_Archive_GetVersion
    c_GetVersion.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_uint8), ctypes.POINTER(ctypes.c_uint8), ctypes.POINTER(ctypes.c_uint8) ]
    c_GetVersion.restype = ErrorCode
    c_GetVersion.errcheck = HandleErrorCode
    major = ctypes.c_uint8()
    minor = ctypes.c_uint8()
    bugfix = ctypes.c_uint8()
    c_GetVersion(archive, ctypes.byref(major), ctypes.byref(minor), ctypes.byref(bugfix))
    return (major.value, minor.value, bugfix.value)

def Archive_GetChunkSize(archive):
    c_GetChunkSize = conf.lib.OTF2_Archive_GetChunkSize
    c_GetChunkSize.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_uint64), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetChunkSize.restype = ErrorCode
    c_GetChunkSize.errcheck = HandleErrorCode
    chunkSizeEvents = ctypes.c_uint64()
    chunkSizeDefs = ctypes.c_uint64()
    c_GetChunkSize(archive, ctypes.byref(chunkSizeEvents), ctypes.byref(chunkSizeDefs))
    return (chunkSizeEvents.value, chunkSizeDefs.value)

def Archive_GetFileSubstrate(archive):
    c_GetFileSubstrate = conf.lib.OTF2_Archive_GetFileSubstrate
    c_GetFileSubstrate.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(FileSubstrate) ]
    c_GetFileSubstrate.restype = ErrorCode
    c_GetFileSubstrate.errcheck = HandleErrorCode
    substrate = FileSubstrate()
    c_GetFileSubstrate(archive, ctypes.byref(substrate))
    return substrate

def Archive_GetCompression(archive):
    c_GetCompression = conf.lib.OTF2_Archive_GetCompression
    c_GetCompression.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(Compression) ]
    c_GetCompression.restype = ErrorCode
    c_GetCompression.errcheck = HandleErrorCode
    compression = Compression()
    c_GetCompression(archive, ctypes.byref(compression))
    return compression

def Archive_GetEvtWriter(archive, location):
    c_GetEvtWriter = conf.lib.OTF2_Archive_GetEvtWriter
    c_GetEvtWriter.argtypes = [ ctypes.POINTER(Archive), LocationRef ]
    c_GetEvtWriter.restype = ctypes.POINTER(EvtWriter)
    return c_GetEvtWriter(archive, location)

def Archive_GetDefWriter(archive, location):
    c_GetDefWriter = conf.lib.OTF2_Archive_GetDefWriter
    c_GetDefWriter.argtypes = [ ctypes.POINTER(Archive), LocationRef ]
    c_GetDefWriter.restype = ctypes.POINTER(DefWriter)
    return c_GetDefWriter(archive, location)

def Archive_GetGlobalDefWriter(archive):
    c_GetGlobalDefWriter = conf.lib.OTF2_Archive_GetGlobalDefWriter
    c_GetGlobalDefWriter.argtypes = [ ctypes.POINTER(Archive) ]
    c_GetGlobalDefWriter.restype = ctypes.POINTER(GlobalDefWriter)
    return c_GetGlobalDefWriter(archive)

def Archive_GetSnapWriter(archive, location):
    c_GetSnapWriter = conf.lib.OTF2_Archive_GetSnapWriter
    c_GetSnapWriter.argtypes = [ ctypes.POINTER(Archive), LocationRef ]
    c_GetSnapWriter.restype = ctypes.POINTER(SnapWriter)
    return c_GetSnapWriter(archive, location)

def Archive_GetThumbWriter(archive, name, description, type, numberOfSamples, numberOfMetrics, refsToDefs):
    c_GetThumbWriter = conf.lib.OTF2_Archive_GetThumbWriter
    c_GetThumbWriter.argtypes = [ ctypes.POINTER(Archive), StrParam, StrParam, ThumbnailType, ctypes.c_uint32, ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint64) ]
    c_GetThumbWriter.restype = ctypes.POINTER(ThumbWriter)
    return c_GetThumbWriter(archive, ctypes.byref(name), ctypes.byref(description), type, numberOfSamples, numberOfMetrics, ctypes.byref(refsToDefs))

def Archive_GetMarkerWriter(archive):
    c_GetMarkerWriter = conf.lib.OTF2_Archive_GetMarkerWriter
    c_GetMarkerWriter.argtypes = [ ctypes.POINTER(Archive) ]
    c_GetMarkerWriter.restype = ctypes.POINTER(MarkerWriter)
    return c_GetMarkerWriter(archive)

def Archive_GetEvtReader(archive, location):
    c_GetEvtReader = conf.lib.OTF2_Archive_GetEvtReader
    c_GetEvtReader.argtypes = [ ctypes.POINTER(Archive), LocationRef ]
    c_GetEvtReader.restype = ctypes.POINTER(EvtReader)
    return c_GetEvtReader(archive, location)

def Archive_GetGlobalEvtReader(archive):
    c_GetGlobalEvtReader = conf.lib.OTF2_Archive_GetGlobalEvtReader
    c_GetGlobalEvtReader.argtypes = [ ctypes.POINTER(Archive) ]
    c_GetGlobalEvtReader.restype = ctypes.POINTER(GlobalEvtReader)
    return c_GetGlobalEvtReader(archive)

def Archive_GetDefReader(archive, location):
    c_GetDefReader = conf.lib.OTF2_Archive_GetDefReader
    c_GetDefReader.argtypes = [ ctypes.POINTER(Archive), LocationRef ]
    c_GetDefReader.restype = ctypes.POINTER(DefReader)
    return c_GetDefReader(archive, location)

def Archive_GetGlobalDefReader(archive):
    c_GetGlobalDefReader = conf.lib.OTF2_Archive_GetGlobalDefReader
    c_GetGlobalDefReader.argtypes = [ ctypes.POINTER(Archive) ]
    c_GetGlobalDefReader.restype = ctypes.POINTER(GlobalDefReader)
    return c_GetGlobalDefReader(archive)

def Archive_GetSnapReader(archive, location):
    c_GetSnapReader = conf.lib.OTF2_Archive_GetSnapReader
    c_GetSnapReader.argtypes = [ ctypes.POINTER(Archive), LocationRef ]
    c_GetSnapReader.restype = ctypes.POINTER(SnapReader)
    return c_GetSnapReader(archive, location)

def Archive_GetGlobalSnapReader(archive):
    c_GetGlobalSnapReader = conf.lib.OTF2_Archive_GetGlobalSnapReader
    c_GetGlobalSnapReader.argtypes = [ ctypes.POINTER(Archive) ]
    c_GetGlobalSnapReader.restype = ctypes.POINTER(GlobalSnapReader)
    return c_GetGlobalSnapReader(archive)

def Archive_GetThumbReader(archive, number):
    c_GetThumbReader = conf.lib.OTF2_Archive_GetThumbReader
    c_GetThumbReader.argtypes = [ ctypes.POINTER(Archive), ctypes.c_uint32 ]
    c_GetThumbReader.restype = ctypes.POINTER(ThumbReader)
    return c_GetThumbReader(archive, number)

def Archive_GetMarkerReader(archive):
    c_GetMarkerReader = conf.lib.OTF2_Archive_GetMarkerReader
    c_GetMarkerReader.argtypes = [ ctypes.POINTER(Archive) ]
    c_GetMarkerReader.restype = ctypes.POINTER(MarkerReader)
    return c_GetMarkerReader(archive)

def Archive_CloseEvtWriter(archive, writer):
    c_CloseEvtWriter = conf.lib.OTF2_Archive_CloseEvtWriter
    c_CloseEvtWriter.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(EvtWriter) ]
    c_CloseEvtWriter.restype = ErrorCode
    c_CloseEvtWriter.errcheck = HandleErrorCode
    c_CloseEvtWriter(archive, writer)

def Archive_CloseDefWriter(archive, writer):
    c_CloseDefWriter = conf.lib.OTF2_Archive_CloseDefWriter
    c_CloseDefWriter.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(DefWriter) ]
    c_CloseDefWriter.restype = ErrorCode
    c_CloseDefWriter.errcheck = HandleErrorCode
    c_CloseDefWriter(archive, writer)

def Archive_CloseMarkerWriter(archive, writer):
    c_CloseMarkerWriter = conf.lib.OTF2_Archive_CloseMarkerWriter
    c_CloseMarkerWriter.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(MarkerWriter) ]
    c_CloseMarkerWriter.restype = ErrorCode
    c_CloseMarkerWriter.errcheck = HandleErrorCode
    c_CloseMarkerWriter(archive, writer)

def Archive_CloseSnapWriter(archive, writer):
    c_CloseSnapWriter = conf.lib.OTF2_Archive_CloseSnapWriter
    c_CloseSnapWriter.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(SnapWriter) ]
    c_CloseSnapWriter.restype = ErrorCode
    c_CloseSnapWriter.errcheck = HandleErrorCode
    c_CloseSnapWriter(archive, writer)

def Archive_CloseGlobalDefWriter(archive, writer):
    c_CloseGlobalDefWriter = conf.lib.OTF2_Archive_CloseGlobalDefWriter
    c_CloseGlobalDefWriter.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(GlobalDefWriter) ]
    c_CloseGlobalDefWriter.restype = ErrorCode
    c_CloseGlobalDefWriter.errcheck = HandleErrorCode
    c_CloseGlobalDefWriter(archive, writer)

def Archive_CloseEvtReader(archive, reader):
    c_CloseEvtReader = conf.lib.OTF2_Archive_CloseEvtReader
    c_CloseEvtReader.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(EvtReader) ]
    c_CloseEvtReader.restype = ErrorCode
    c_CloseEvtReader.errcheck = HandleErrorCode
    c_CloseEvtReader(archive, reader)

def Archive_CloseThumbReader(archive, reader):
    c_CloseThumbReader = conf.lib.OTF2_Archive_CloseThumbReader
    c_CloseThumbReader.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ThumbReader) ]
    c_CloseThumbReader.restype = ErrorCode
    c_CloseThumbReader.errcheck = HandleErrorCode
    c_CloseThumbReader(archive, reader)

def Archive_CloseGlobalEvtReader(archive, globalEvtReader):
    c_CloseGlobalEvtReader = conf.lib.OTF2_Archive_CloseGlobalEvtReader
    c_CloseGlobalEvtReader.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(GlobalEvtReader) ]
    c_CloseGlobalEvtReader.restype = ErrorCode
    c_CloseGlobalEvtReader.errcheck = HandleErrorCode
    c_CloseGlobalEvtReader(archive, globalEvtReader)

def Archive_CloseDefReader(archive, reader):
    c_CloseDefReader = conf.lib.OTF2_Archive_CloseDefReader
    c_CloseDefReader.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(DefReader) ]
    c_CloseDefReader.restype = ErrorCode
    c_CloseDefReader.errcheck = HandleErrorCode
    c_CloseDefReader(archive, reader)

def Archive_CloseGlobalDefReader(archive, globalDefReader):
    c_CloseGlobalDefReader = conf.lib.OTF2_Archive_CloseGlobalDefReader
    c_CloseGlobalDefReader.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(GlobalDefReader) ]
    c_CloseGlobalDefReader.restype = ErrorCode
    c_CloseGlobalDefReader.errcheck = HandleErrorCode
    c_CloseGlobalDefReader(archive, globalDefReader)

def Archive_CloseSnapReader(archive, reader):
    c_CloseSnapReader = conf.lib.OTF2_Archive_CloseSnapReader
    c_CloseSnapReader.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(SnapReader) ]
    c_CloseSnapReader.restype = ErrorCode
    c_CloseSnapReader.errcheck = HandleErrorCode
    c_CloseSnapReader(archive, reader)

def Archive_CloseGlobalSnapReader(archive, globalSnapReader):
    c_CloseGlobalSnapReader = conf.lib.OTF2_Archive_CloseGlobalSnapReader
    c_CloseGlobalSnapReader.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(GlobalSnapReader) ]
    c_CloseGlobalSnapReader.restype = ErrorCode
    c_CloseGlobalSnapReader.errcheck = HandleErrorCode
    c_CloseGlobalSnapReader(archive, globalSnapReader)

def Archive_CloseMarkerReader(archive, markerReader):
    c_CloseMarkerReader = conf.lib.OTF2_Archive_CloseMarkerReader
    c_CloseMarkerReader.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(MarkerReader) ]
    c_CloseMarkerReader.restype = ErrorCode
    c_CloseMarkerReader.errcheck = HandleErrorCode
    c_CloseMarkerReader(archive, markerReader)

def Archive_GetNumberOfThumbnails(archive):
    c_GetNumberOfThumbnails = conf.lib.OTF2_Archive_GetNumberOfThumbnails
    c_GetNumberOfThumbnails.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_uint32) ]
    c_GetNumberOfThumbnails.restype = ErrorCode
    c_GetNumberOfThumbnails.errcheck = HandleErrorCode
    number = ctypes.c_uint32()
    c_GetNumberOfThumbnails(archive, ctypes.byref(number))
    return number.value

def Archive_SetNumberOfSnapshots(archive, number):
    c_SetNumberOfSnapshots = conf.lib.OTF2_Archive_SetNumberOfSnapshots
    c_SetNumberOfSnapshots.argtypes = [ ctypes.POINTER(Archive), ctypes.c_uint32 ]
    c_SetNumberOfSnapshots.restype = ErrorCode
    c_SetNumberOfSnapshots.errcheck = HandleErrorCode
    c_SetNumberOfSnapshots(archive, number)

def Archive_GetNumberOfSnapshots(archive):
    c_GetNumberOfSnapshots = conf.lib.OTF2_Archive_GetNumberOfSnapshots
    c_GetNumberOfSnapshots.argtypes = [ ctypes.POINTER(Archive), ctypes.POINTER(ctypes.c_uint32) ]
    c_GetNumberOfSnapshots.restype = ErrorCode
    c_GetNumberOfSnapshots.errcheck = HandleErrorCode
    number = ctypes.c_uint32(0)
    c_GetNumberOfSnapshots(archive, ctypes.byref(number))
    return number.value

def Archive_OpenEvtFiles(archive):
    c_OpenEvtFiles = conf.lib.OTF2_Archive_OpenEvtFiles
    c_OpenEvtFiles.argtypes = [ ctypes.POINTER(Archive) ]
    c_OpenEvtFiles.restype = ErrorCode
    c_OpenEvtFiles.errcheck = HandleErrorCode
    c_OpenEvtFiles(archive)

def Archive_CloseEvtFiles(archive):
    c_CloseEvtFiles = conf.lib.OTF2_Archive_CloseEvtFiles
    c_CloseEvtFiles.argtypes = [ ctypes.POINTER(Archive) ]
    c_CloseEvtFiles.restype = ErrorCode
    c_CloseEvtFiles.errcheck = HandleErrorCode
    c_CloseEvtFiles(archive)

def Archive_OpenDefFiles(archive):
    c_OpenDefFiles = conf.lib.OTF2_Archive_OpenDefFiles
    c_OpenDefFiles.argtypes = [ ctypes.POINTER(Archive) ]
    c_OpenDefFiles.restype = ErrorCode
    c_OpenDefFiles.errcheck = HandleErrorCode
    c_OpenDefFiles(archive)

def Archive_CloseDefFiles(archive):
    c_CloseDefFiles = conf.lib.OTF2_Archive_CloseDefFiles
    c_CloseDefFiles.argtypes = [ ctypes.POINTER(Archive) ]
    c_CloseDefFiles.restype = ErrorCode
    c_CloseDefFiles.errcheck = HandleErrorCode
    c_CloseDefFiles(archive)

def Archive_OpenSnapFiles(archive):
    c_OpenSnapFiles = conf.lib.OTF2_Archive_OpenSnapFiles
    c_OpenSnapFiles.argtypes = [ ctypes.POINTER(Archive) ]
    c_OpenSnapFiles.restype = ErrorCode
    c_OpenSnapFiles.errcheck = HandleErrorCode
    c_OpenSnapFiles(archive)

def Archive_CloseSnapFiles(archive):
    c_CloseSnapFiles = conf.lib.OTF2_Archive_CloseSnapFiles
    c_CloseSnapFiles.argtypes = [ ctypes.POINTER(Archive) ]
    c_CloseSnapFiles.restype = ErrorCode
    c_CloseSnapFiles.errcheck = HandleErrorCode
    c_CloseSnapFiles(archive)

def Archive_SelectLocation(archive, location):
    c_SelectLocation = conf.lib.OTF2_Archive_SelectLocation
    c_SelectLocation.argtypes = [ ctypes.POINTER(Archive), LocationRef ]
    c_SelectLocation.restype = ErrorCode
    c_SelectLocation.errcheck = HandleErrorCode
    c_SelectLocation(archive, location)

__all__ = [
    'Archive',
    'Archive_Close',
    'Archive_CloseDefFiles',
    'Archive_CloseDefReader',
    'Archive_CloseDefWriter',
    'Archive_CloseEvtFiles',
    'Archive_CloseEvtReader',
    'Archive_CloseEvtWriter',
    'Archive_CloseGlobalDefReader',
    'Archive_CloseGlobalDefWriter',
    'Archive_CloseGlobalEvtReader',
    'Archive_CloseGlobalSnapReader',
    'Archive_CloseMarkerReader',
    'Archive_CloseMarkerWriter',
    'Archive_CloseSnapFiles',
    'Archive_CloseSnapReader',
    'Archive_CloseSnapWriter',
    'Archive_CloseThumbReader',
    'Archive_GetChunkSize',
    'Archive_GetCompression',
    'Archive_GetCreator',
    'Archive_GetDefReader',
    'Archive_GetDefWriter',
    'Archive_GetDescription',
    'Archive_GetEvtReader',
    'Archive_GetEvtWriter',
    'Archive_GetFileSubstrate',
    'Archive_GetGlobalDefReader',
    'Archive_GetGlobalDefWriter',
    'Archive_GetGlobalEvtReader',
    'Archive_GetGlobalSnapReader',
    'Archive_GetMachineName',
    'Archive_GetMarkerReader',
    'Archive_GetMarkerWriter',
    'Archive_GetNumberOfGlobalDefinitions',
    'Archive_GetNumberOfLocations',
    'Archive_GetNumberOfSnapshots',
    'Archive_GetNumberOfThumbnails',
    'Archive_GetProperty',
    'Archive_GetPropertyNames',
    'Archive_GetSnapReader',
    'Archive_GetSnapWriter',
    'Archive_GetThumbReader',
    'Archive_GetThumbWriter',
    'Archive_GetTraceId',
    'Archive_GetVersion',
    'Archive_IsMaster',
    'Archive_Open',
    'Archive_OpenDefFiles',
    'Archive_OpenEvtFiles',
    'Archive_OpenSnapFiles',
    'Archive_SelectLocation',
    'Archive_SetBoolProperty',
    'Archive_SetCollectiveCallbacks',
    'Archive_SetCreator',
    'Archive_SetDescription',
    'Archive_SetFlushCallbacks',
    'Archive_SetHint',
    'Archive_SetLockingCallbacks',
    'Archive_SetMachineName',
    'Archive_SetMemoryCallbacks',
    'Archive_SetNumberOfSnapshots',
    'Archive_SetProperty',
    'Archive_SetSerialCollectiveCallbacks',
    'Archive_SwitchFileMode',
    'Archive_SetDefChunkSize'
]
