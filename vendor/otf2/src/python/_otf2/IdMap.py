import ctypes

from .Config import conf, EnumBase, errcheck_null_to_none
from .ErrorCodes import ErrorCode, HandleErrorCode


class IdMapMode(EnumBase, ctypes.c_uint8):
    pass

ID_MAP_DENSE = IdMapMode(0)
ID_MAP_SPARSE = IdMapMode(1)

class IdMap(ctypes.Structure):
    pass

_IdMap_TraverseCallback_FP = ctypes.CFUNCTYPE(ctypes.c_void_p,
                                              ctypes.c_uint64,
                                              ctypes.c_uint64,
                                              ctypes.c_void_p)

def IdMap_TraverseCallback(func):
    def wrapper(localId, globalId, userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(localId, globalId, py_userData, *args, **kwargs)
    return _IdMap_TraverseCallback_FP(wrapper)

def IdMap_Create(mode, capacity):
    c_Create = conf.lib.OTF2_IdMap_Create
    c_Create.argtypes = [ IdMapMode, ctypes.c_uint64 ]
    c_Create.restype = ctypes.POINTER(IdMap)
    return c_Create(mode, capacity)

def IdMap_CreateFromUint64Array(mappings, optimizeSize):
    length = len(mappings)
    mappings_array_type = ctypes.c_uint64 * length
    mappings_array = mappings_array_type()
    mappings_array[:] = mappings

    c_CreateFromUint64Array = conf.lib.OTF2_IdMap_CreateFromUint64Array
    c_CreateFromUint64Array.argtypes = [ ctypes.c_uint64, mappings_array_type, ctypes.c_bool ]
    c_CreateFromUint64Array.restype = ctypes.POINTER(IdMap)
    c_CreateFromUint64Array.errcheck = errcheck_null_to_none
    return c_CreateFromUint64Array(length, mappings_array, optimizeSize)

def IdMap_CreateFromUint32Array(length, mappings, optimizeSize):
    length = len(mappings)
    mappings_array_type = ctypes.c_uint32 * length
    mappings_array = mappings_array_type()
    mappings_array[:] = mappings

    c_CreateFromUint32Array = conf.lib.OTF2_IdMap_CreateFromUint32Array
    c_CreateFromUint32Array.argtypes = [ ctypes.c_uint64, mappings_array_type, ctypes.c_bool ]
    c_CreateFromUint32Array.restype = ctypes.POINTER(IdMap)
    c_CreateFromUint32Array.errcheck = errcheck_null_to_none
    return c_CreateFromUint32Array(length, mappings_array, optimizeSize)

def IdMap_Free(instance):
    c_Free = conf.lib.OTF2_IdMap_Free
    c_Free.argtypes = [ ctypes.POINTER(IdMap) ]
    c_Free.restype = None
    return c_Free(instance)

def IdMap_GetSize(instance, size = ctypes.c_uint64()):
    c_GetSize = conf.lib.OTF2_IdMap_GetSize
    c_GetSize.argtypes = [ ctypes.POINTER(IdMap), ctypes.POINTER(ctypes.c_uint64) ]
    c_GetSize.restype = ErrorCode
    c_GetSize.errcheck = HandleErrorCode
    c_GetSize(instance, ctypes.byref(size))
    return size.value

def IdMap_GetMode(instance, mode = IdMapMode()):
    c_GetMode = conf.lib.OTF2_IdMap_GetMode
    c_GetMode.argtypes = [ ctypes.POINTER(IdMap), ctypes.POINTER(IdMapMode) ]
    c_GetMode.restype = ErrorCode
    c_GetMode.errcheck = HandleErrorCode
    c_GetMode(instance, ctypes.byref(mode))
    return mode

def IdMap_Clear(instance):
    c_Clear = conf.lib.OTF2_IdMap_Clear
    c_Clear.argtypes = [ ctypes.POINTER(IdMap) ]
    c_Clear.restype = ErrorCode
    c_Clear.errcheck = HandleErrorCode
    c_Clear(instance)

def IdMap_AddIdPair(instance, localId, globalId):
    c_AddIdPair = conf.lib.OTF2_IdMap_AddIdPair
    c_AddIdPair.argtypes = [ ctypes.POINTER(IdMap), ctypes.c_uint64, ctypes.c_uint64 ]
    c_AddIdPair.restype = ErrorCode
    c_AddIdPair.errcheck = HandleErrorCode
    c_AddIdPair(instance, localId, globalId)

def IdMap_GetGlobalId(instance, localId, globalId = ctypes.c_uint64()):
    c_GetGlobalId = conf.lib.OTF2_IdMap_GetGlobalId
    c_GetGlobalId.argtypes = [ ctypes.POINTER(IdMap), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_GetGlobalId.restype = ErrorCode
    c_GetGlobalId.errcheck = HandleErrorCode
    c_GetGlobalId(instance, localId, ctypes.byref(globalId))
    return globalId.value

def IdMap_GetGlobalIdSave(instance, localId, globalId = ctypes.c_uint64()):
    c_GetGlobalIdSave = conf.lib.OTF2_IdMap_GetGlobalIdSave
    c_GetGlobalIdSave.argtypes = [ ctypes.POINTER(IdMap), ctypes.c_uint64, ctypes.POINTER(ctypes.c_uint64) ]
    c_GetGlobalIdSave.restype = ErrorCode
    c_GetGlobalIdSave.errcheck = HandleErrorCode
    c_GetGlobalIdSave(instance, localId, ctypes.byref(globalId))
    return globalId.value

def IdMap_Traverse(instance, callback, userData):
    c_Traverse = conf.lib.OTF2_IdMap_Traverse
    c_Traverse.argtypes = [ ctypes.POINTER(IdMap), _IdMap_TraverseCallback_FP, ctypes.py_object ]
    c_Traverse.restype = ErrorCode
    c_Traverse.errcheck = HandleErrorCode
    c_Traverse(instance, IdMap_TraverseCallback(callback), ctypes.py_object(userData))

__all__ = [
    'IdMap',
    'IdMap_AddIdPair',
    'IdMap_Clear',
    'IdMap_Create',
    'IdMap_CreateFromUint32Array',
    'IdMap_CreateFromUint64Array',
    'IdMap_Free',
    'IdMap_GetGlobalId',
    'IdMap_GetGlobalIdSave',
    'IdMap_GetMode',
    'IdMap_GetSize',
    'IdMap_Traverse',
    'IdMap_TraverseCallback',
    'IdMapMode',
    'ID_MAP_DENSE',
    'ID_MAP_SPARSE'
]
