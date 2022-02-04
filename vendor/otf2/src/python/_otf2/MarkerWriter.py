import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GeneralDefinitions import TimeStamp
from .Marker import MarkerRef, MarkerScope, MarkerSeverity


class MarkerWriter(ctypes.Structure):
    pass

def MarkerWriter_WriteDefMarker(writerHandle, self, markerGroup, markerCategory, severity):
    c_WriteDefMarker = conf.lib.OTF2_MarkerWriter_WriteDefMarker
    c_WriteDefMarker.argtypes = [ ctypes.POINTER(MarkerWriter), MarkerRef, StrParam, StrParam, MarkerSeverity ]
    c_WriteDefMarker.restype = ErrorCode
    c_WriteDefMarker.errcheck = HandleErrorCode
    c_WriteDefMarker(writerHandle, self, markerGroup, markerCategory, severity)

def MarkerWriter_WriteMarker(writerHandle, timestamp, duration, marker, scope, scopeRef, text):
    c_WriteMarker = conf.lib.OTF2_MarkerWriter_WriteMarker
    c_WriteMarker.argtypes = [ ctypes.POINTER(MarkerWriter), TimeStamp, TimeStamp, MarkerRef, MarkerScope, ctypes.c_uint64, StrParam ]
    c_WriteMarker.restype = ErrorCode
    c_WriteMarker.errcheck = HandleErrorCode
    c_WriteMarker(writerHandle, timestamp, duration, marker, scope, scopeRef, text)

__all__ = [
    'MarkerWriter',
    'MarkerWriter_WriteDefMarker',
    'MarkerWriter_WriteMarker'
]
