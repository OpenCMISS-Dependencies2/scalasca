import ctypes
from functools import wraps

from .GeneralDefinitions import LocationRef, FileType, CallbackCode, Type, FlushType, TimeStamp


_PreFlushCallback_FP = ctypes.CFUNCTYPE(FlushType, ctypes.c_void_p, ctypes.c_uint32,
                                        ctypes.c_uint32, ctypes.c_void_p, ctypes.c_bool)

def callback_wrapper(python_func, wrapper_func, funcptr_type):
    funcptr = funcptr_type(wrapper_func)

    # Attach the wrappers to the original python functions to prevent
    # them from being garbage collected
    try:
        # Works for function / lambdas
        python_func.__wrapper = wrapper_func
        python_func.__wrapper_funcptr = funcptr
    except AttributeError:
        # Probably a bound method, where we cannot set something directly
        setattr(python_func.__self__, '__otf2_wrapper_' + python_func.__name__, wrapper_func)
        setattr(python_func.__self__, '__otf2_wrapper_funcptr_' + python_func.__name__, funcptr)
    return funcptr

def Callbacks_PreFlushCallback(func):
    if func is None:
        return _PreFlushCallback_FP()

    def wrapper(userData, *args, **kwargs):
        if userData:
            py_userData = ctypes.cast(userData, ctypes.py_object).value
        else:
            py_userData = None
        # Why does ctypes want the value (int) here even though FlushType is just a derived int?
        # I think this is a bug.
        return func(py_userData, *args, **kwargs).value

    return callback_wrapper(func, wrapper, _PreFlushCallback_FP)

_PostFlushCallback_FP = ctypes.CFUNCTYPE(TimeStamp, ctypes.c_void_p, ctypes.c_uint32, LocationRef)

def Callbacks_PostFlushCallback(func):
    if func is None:
        return _PostFlushCallback_FP()

    def wrapper(userData, *args, **kwargs):
        if userData:
            py_userData = ctypes.cast(userData, ctypes.py_object).value
        else:
            py_userData = None
        # Note TimeStamp is only an alias, not a derived type, so we do not need .value here
        return func(py_userData, *args, **kwargs)

    return callback_wrapper(func, wrapper, _PostFlushCallback_FP)

class FlushCallbacks(ctypes.Structure):
    def __init__(self, pre_flush, post_flush):
        super(FlushCallbacks, self).__init__(pre_flush=Callbacks_PreFlushCallback(pre_flush),
                                             post_flush=Callbacks_PostFlushCallback(post_flush))

    _fields_ = [
        ("pre_flush", _PreFlushCallback_FP),
        ("post_flush", _PostFlushCallback_FP)
    ]

_MemoryAllocate_FP = ctypes.CFUNCTYPE(ctypes.c_void_p,
                                      FileType,
                                      LocationRef,
                                      ctypes.POINTER(ctypes.c_void_p),
                                      ctypes.c_uint64)

def Callbacks_MemoryAllocate(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _MemoryAllocate_FP(wrapper)

_MemoryFreeAll_FP = ctypes.CFUNCTYPE(ctypes.c_void_p,
                                     FileType,
                                     LocationRef,
                                     ctypes.POINTER(ctypes.c_void_p),
                                     ctypes.c_bool)

def Callbacks_MemoryFreeAll(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _MemoryFreeAll_FP(wrapper)

class MemoryCallbacks(ctypes.Structure):
    _fields_ = [
        ('allocate', _MemoryAllocate_FP),
        ('free_all', _MemoryFreeAll_FP)
    ]

class CollectiveContext(ctypes.Structure):
    pass

# Define Constant
COLLECTIVES_ROOT = ctypes.c_int(0)

_Collectives_GetSize_FP = ctypes.CFUNCTYPE(CallbackCode,
                                           ctypes.c_void_p,
                                           ctypes.POINTER(CollectiveContext),
                                           ctypes.POINTER(ctypes.c_uint32))

def Callbacks_Collectives_GetSize(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Collectives_GetSize_FP(wrapper)

_Collectives_GetRank_FP = ctypes.CFUNCTYPE(CallbackCode,
                                           ctypes.c_void_p,
                                           ctypes.POINTER(CollectiveContext),
                                           ctypes.POINTER(ctypes.c_uint32))

def Callbacks_Collectives_GetRank(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Collectives_GetRank_FP(wrapper)

_Collectives_CreateLocalComm_FP = ctypes.CFUNCTYPE(CallbackCode,
                                                   ctypes.c_void_p,
                                                   ctypes.POINTER(ctypes.POINTER(CollectiveContext)),
                                                   ctypes.POINTER(CollectiveContext),
                                                   ctypes.c_uint32,
                                                   ctypes.c_uint32,
                                                   ctypes.c_uint32,
                                                   ctypes.c_uint32,
                                                   ctypes.c_uint32,
                                                   ctypes.c_uint32)

def Callbacks_Collectives_CreateLocalComm(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Collectives_CreateLocalComm_FP(wrapper)

_Collectives_FreeLocalComm_FP = ctypes.CFUNCTYPE(CallbackCode,
                                                 ctypes.c_void_p,
                                                 ctypes.POINTER(CollectiveContext))

def Callbacks_Collectives_FreeLocalComm(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Collectives_FreeLocalComm_FP(wrapper)

_Collectives_Barrier_FP = ctypes.CFUNCTYPE(CallbackCode,
                                           ctypes.c_void_p,
                                           ctypes.POINTER(CollectiveContext))

def Callbacks_Collectives_Barrier(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Collectives_Barrier_FP(wrapper)

_Collectives_Bcast_FP = ctypes.CFUNCTYPE(CallbackCode,
                                         ctypes.c_void_p,
                                         ctypes.POINTER(CollectiveContext),
                                         ctypes.c_void_p,
                                         ctypes.c_uint32,
                                         Type,
                                         ctypes.c_uint32)

def Callbacks_Collectives_Bcast(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Collectives_Bcast_FP(wrapper)

_Collectives_Gather_FP = ctypes.CFUNCTYPE(CallbackCode,
                                          ctypes.c_void_p,
                                          ctypes.POINTER(CollectiveContext),
                                          ctypes.c_void_p,
                                          ctypes.c_void_p,
                                          ctypes.c_uint32,
                                          Type,
                                          ctypes.c_uint32)

def Callbacks_Collectives_Gather(func):
    def wrapper(userData, commContext, inData, outData,  *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        py_outData = ctypes.cast(outData, ctypes.py_object).value
        return func(py_userData, commContext, inData, py_outData, *args, **kwargs)
    return _Collectives_Gather_FP(wrapper)

_Collectives_Gatherv_FP = ctypes.CFUNCTYPE(CallbackCode,
                                           ctypes.POINTER(CollectiveContext),
                                           ctypes.c_void_p,
                                           ctypes.c_uint32,
                                           ctypes.c_void_p,
                                           ctypes.POINTER(ctypes.c_uint32),
                                           Type,
                                           ctypes.c_uint32)

def Callbacks_Collectives_Gatherv(func):
    def wrapper(userData, commContext, inData, inElements, outData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        py_outData = ctypes.cast(outData, ctypes.py_object).value
        return func(py_userData, commContext, inData, inElements, py_outData, *args, **kwargs)
    return _Collectives_Gatherv_FP(wrapper)

_Collectives_Scatter_FP = ctypes.CFUNCTYPE(CallbackCode,
                                           ctypes.c_void_p,
                                           ctypes.POINTER(CollectiveContext),
                                           ctypes.c_void_p,
                                           ctypes.POINTER(ctypes.c_uint32),
                                           ctypes.c_void_p,
                                           ctypes.c_uint32,
                                           Type,
                                           ctypes.c_uint32)

def Callbacks_Collectives_Scatter(func):
    def wrapper(userData, commContext, inData, outData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        py_outData = ctypes.cast(outData, ctypes.py_object).value
        return func(py_userData, commContext, inData, py_outData, *args, **kwargs)
    return _Collectives_Scatter_FP(wrapper)

_Collectives_Scatterv_FP = ctypes.CFUNCTYPE(CallbackCode,
                                            ctypes.c_void_p,
                                            ctypes.POINTER(CollectiveContext),
                                            ctypes.c_void_p,
                                            ctypes.POINTER(ctypes.c_uint32),
                                            ctypes.c_void_p,
                                            ctypes.c_uint32,
                                            Type,
                                            ctypes.c_uint32)

def Callbacks_Collectives_Scatterv(func):
    def wrapper(userData, commContext, inData, inElements, outData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        py_outData = ctypes.cast(outData, ctypes.py_object).value
        return func(py_userData, commContext, inData, inElements, py_outData, *args, **kwargs)
    return _Collectives_Scatterv_FP(wrapper)

_Collectives_Release_FP = ctypes.CFUNCTYPE(None,
                                           ctypes.c_void_p,
                                           ctypes.POINTER(CollectiveContext),
                                           ctypes.POINTER(CollectiveContext))

def Callbacks_Collectives_Release(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Collectives_Release_FP(wrapper)

class CollectiveCallbacks(ctypes.Structure):
    _fields_ = [
        ("release", _Collectives_Release_FP),
        ("get_size", _Collectives_GetSize_FP),
        ("get_rank", _Collectives_GetRank_FP),
        ("create_local_comm", _Collectives_CreateLocalComm_FP),
        ("free_local_comm", _Collectives_FreeLocalComm_FP),
        ("barrier", _Collectives_Barrier_FP),
        ("bcast", _Collectives_Bcast_FP),
        ("gather", _Collectives_Gather_FP),
        ("gatherv", _Collectives_Gatherv_FP),
        ("scatter", _Collectives_Scatter_FP),
        ("scatterv", _Collectives_Scatterv_FP)
    ]


class LockObject(ctypes.Structure):
    pass

Lock = ctypes.POINTER(LockObject)

_Locking_Create_FP = ctypes.CFUNCTYPE(CallbackCode, ctypes.c_void_p, ctypes.POINTER(Lock))

def Callbacks_Locking_Create(func):
    def wrapper(userData, outData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        py_outData = ctypes.cast(outData, ctypes.py_object).value
        return func(py_userData, py_outData, *args, **kwargs)
    return _Locking_Create_FP(wrapper)

_Locking_Destroy_FP = ctypes.CFUNCTYPE(CallbackCode, ctypes.c_void_p, Lock)

def Callbacks_Locking_Destroy(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Locking_Destroy_FP(wrapper)

_Locking_Lock_FP = ctypes.CFUNCTYPE(CallbackCode, ctypes.c_void_p, Lock)

def Callbacks_Locking_Lock(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Locking_Lock_FP(wrapper)

_Locking_Unlock_FP = ctypes.CFUNCTYPE(CallbackCode, ctypes.c_void_p, Lock)

def Callbacks_Locking_Unlock(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Locking_Unlock_FP(wrapper)

_Locking_Release_FP = ctypes.CFUNCTYPE(None, ctypes.c_void_p)

def Callbacks_Locking_Release(func):
    def wrapper(userData, *args, **kwargs):
        py_userData = ctypes.cast(userData, ctypes.py_object).value
        return func(py_userData, *args, **kwargs)
    return _Locking_Release_FP(wrapper)

class LockingCallbacks(ctypes.Structure):
    _fields_ = [
        ('release', _Locking_Release_FP),
        ('create', _Locking_Create_FP),
        ('destroy', _Locking_Destroy_FP),
        ('lock', _Locking_Lock_FP),
        ('lock', _Locking_Lock_FP),
        ('unlock', _Locking_Unlock_FP)
    ]

__all__ = [
    'Callbacks_PreFlushCallback',
    'Callbacks_PostFlushCallback',
    'FlushCallbacks',
    'Callbacks_MemoryAllocate',
    'Callbacks_MemoryFreeAll',
    'MemoryCallbacks',
    'CollectiveContext',
    'COLLECTIVES_ROOT',
    'Callbacks_Collectives_GetSize',
    'Callbacks_Collectives_GetRank',
    'Callbacks_Collectives_CreateLocalComm',
    'Callbacks_Collectives_FreeLocalComm',
    'Callbacks_Collectives_Barrier',
    'Callbacks_Collectives_Bcast',
    'Callbacks_Collectives_Gather',
    'Callbacks_Collectives_Gatherv',
    'Callbacks_Collectives_Scatter',
    'Callbacks_Collectives_Scatterv',
    'Callbacks_Collectives_Release',
    'CollectiveCallbacks',
    'LockObject',
    'Lock',
    'Callbacks_Locking_Create',
    'Callbacks_Locking_Destroy',
    'Callbacks_Locking_Lock',
    'Callbacks_Locking_Unlock',
    'Callbacks_Locking_Release',
    'LockingCallbacks'
]
