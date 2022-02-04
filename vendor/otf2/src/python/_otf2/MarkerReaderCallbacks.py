import ctypes

from .Config import conf, str_decode, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GeneralDefinitions import TimeStamp, CallbackCode
from .Marker import MarkerRef, MarkerScope, MarkerSeverity
from .Callbacks import callback_wrapper


class MarkerReaderCallbacks(ctypes.Structure):
    pass

def MarkerReaderCallbacks_New():
    c_New = conf.lib.OTF2_MarkerReaderCallbacks_New
    c_New.argtypes = []
    c_New.restype = ctypes.POINTER(MarkerReaderCallbacks)
    return c_New()

def MarkerReaderCallbacks_Delete(markerReaderCallbacks):
    c_Delete = conf.lib.OTF2_MarkerReaderCallbacks_Delete
    c_Delete.argtype = [ ctypes.POINTER(MarkerReaderCallbacks) ]
    c_Delete.restype = None
    return c_Delete(markerReaderCallbacks)

def MarkerReaderCallbacks_Clear(markerReaderCallbacks):
    c_Clear = conf.lib.OTF2_MarkerReaderCallbacks_Clear
    c_Clear.argtypes = [ ctypes.POINTER(MarkerReaderCallbacks) ]
    c_Clear.restype = None
    return c_Clear(markerReaderCallbacks)

def _callback_wrapper(type, func, convert_args=None):
    def wrapper(userData, *args):
        if userData:
            py_userData = ctypes.cast(userData, ctypes.py_object).value
        else:
            py_userData = None
        try:
            if convert_args is not None:
                args = convert_args(*args)
            ret = func(py_userData, *args)
            if ret is None:
                ret = CALLBACK_SUCCESS
        except:
            sys.stderr.write("An unhandled python exception has occurred in an "
                             "OTF2_MarkerReaderCallback:\n")
            sys.stderr.write(traceback.format_exc())
            ret = CALLBACK_ERROR
        return ret.value
    return callback_wrapper(func, wrapper, type)

_MarkerReaderCallback_FP_Unknown = ctypes.CFUNCTYPE(CallbackCode, ctypes.c_void_p)

_MarkerReaderCallback_FP_DefMarker = ctypes.CFUNCTYPE(CallbackCode, ctypes.c_void_p, MarkerRef, ctypes.c_char_p, ctypes.c_char_p, MarkerSeverity)

_MarkerReaderCallback_FP_Marker = ctypes.CFUNCTYPE(CallbackCode, ctypes.c_void_p, TimeStamp, TimeStamp, MarkerRef, MarkerScope, ctypes.c_uint64, ctypes.c_char_p)

def MarkerReaderCallbacks_SetUnknownCallback(markerReaderCallbacks, unknownCallback):
    c_SetUnknownCallback = conf.lib.OTF2_MarkerReaderCallbacks_SetUnknownCallback
    c_SetUnknownCallback.argtypes = [ ctypes.POINTER(MarkerReaderCallbacks), _MarkerReaderCallback_FP_Unknown ]
    c_SetUnknownCallback.restype = ErrorCode
    c_SetUnknownCallback.errcheck = HandleErrorCode
    wrapped_callback = _callback_wrapper(_MarkerReaderCallback_FP_Unknown, unknownCallback)
    c_SetUnknownCallback(markerReaderCallbacks, wrapped_callback)

def MarkerReaderCallbacks_SetDefMarkerCallback(markerReaderCallbacks, defMarkerCallback):
    c_SetDefMarkerCallback = conf.lib.OTF2_MarkerReaderCallbacks_SetDefMarkerCallback
    c_SetDefMarkerCallback.argtypes = [ ctypes.POINTER(MarkerReaderCallbacks), _MarkerReaderCallback_FP_DefMarker ]
    c_SetDefMarkerCallback.restype = ErrorCode
    c_SetDefMarkerCallback.errcheck = HandleErrorCode
    def convert_string(ref, group, category, severity):
        return ref, str_decode(group), str_decode(category), severity
    wrapped_callback = _callback_wrapper(_MarkerReaderCallback_FP_DefMarker, defMarkerCallback, convert_string)
    c_SetDefMarkerCallback(markerReaderCallbacks, wrapped_callback)

def MarkerReaderCallbacks_SetMarkerCallback(markerReaderCallbacks, markerCallback):
    c_SetMarkerCallback = conf.lib.OTF2_MarkerReaderCallbacks_SetMarkerCallback
    c_SetMarkerCallback.argtypes = [ ctypes.POINTER(MarkerReaderCallbacks), _MarkerReaderCallback_FP_Marker ]
    c_SetMarkerCallback.restype = ErrorCode
    c_SetMarkerCallback.errcheck = HandleErrorCode
    def convert_string(ref, scope, scope_ref, text):
        return ref, scope, scope_ref, str_decode(text)
    wrapped_callback = _callback_wrapper(_MarkerReaderCallback_FP_Marker, markerCallback, convert_string)
    c_SetMarkerCallback(markerReaderCallbacks, wrapped_callback)

__all__ = [
    'MarkerReaderCallbacks',
    'MarkerReaderCallbacks_New',
    'MarkerReaderCallbacks_Delete',
    'MarkerReaderCallbacks_Clear',
    'MarkerReaderCallbacks_SetUnknownCallback',
    'MarkerReaderCallbacks_SetDefMarkerCallback',
    'MarkerReaderCallbacks_SetMarkerCallback'
]
