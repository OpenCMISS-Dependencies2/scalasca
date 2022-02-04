import ctypes

from .GeneralDefinitions import UNDEFINED_UINT32

MarkerRef = ctypes.c_uint8
UNDEFINED_MARKER = UNDEFINED_UINT32

MarkerSeverity = ctypes.c_uint8

SEVERITY_NONE = MarkerSeverity(0)
SEVERITY_LOW = MarkerSeverity(1)
SEVERITY_MEDIUM = MarkerSeverity(2)
SEVERITY_HIGH = MarkerSeverity(3)

MarkerScope = ctypes.c_uint8

MARKER_SCOPE_GLOBAL = MarkerScope(0)
MARKER_SCOPE_LOCATION = MarkerScope(1)
MARKER_SCOPE_LOCATION_GROUP = MarkerScope(2)
MARKER_SCOPE_SYSTEM_TREE_NODE = MarkerScope(3)
MARKER_SCOPE_GROUP = MarkerScope(4)
MARKER_SCOPE_COMM = MarkerScope(5)

__all__ = [
    'MarkerRef',
    'UNDEFINED_MARKER',
    'MarkerSeverity',
    'SEVERITY_NONE',
    'SEVERITY_LOW',
    'SEVERITY_MEDIUM',
    'SEVERITY_HIGH',
    'MarkerScope',
    'MARKER_SCOPE_GLOBAL',
    'MARKER_SCOPE_LOCATION',
    'MARKER_SCOPE_LOCATION_GROUP',
    'MARKER_SCOPE_SYSTEM_TREE_NODE',
    'MARKER_SCOPE_GROUP',
    'MARKER_SCOPE_COMM'
]
