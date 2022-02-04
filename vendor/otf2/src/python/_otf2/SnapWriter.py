
''' Generated by OTF2 Template Engine '''

import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GeneralDefinitions import *
from .AttributeList import AttributeList
from .Definitions import *
from .Events import *


class SnapWriter(ctypes.Structure):
    pass

def SnapWriter_GetLocationID(writer, locationID = LocationRef()):
    c_GetLocationID = conf.lib.OTF2_SnapWriter_GetLocationID
    c_GetLocationID.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(LocationRef) ]
    c_GetLocationID.restype = ErrorCode
    c_GetLocationID.errcheck = HandleErrorCode
    c_GetLocationID(writer, ctypes.byref(locationID))
    return locationID.value

def SnapWriter_SnapshotStart(writerHandle, attributeList, snapTime, numberOfRecords):
    c_SnapshotStart = conf.lib.OTF2_SnapWriter_SnapshotStart
    c_SnapshotStart.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, ctypes.c_uint64 ]
    c_SnapshotStart.restype = ErrorCode
    c_SnapshotStart.errcheck = HandleErrorCode
    c_SnapshotStart(writerHandle, attributeList, snapTime, numberOfRecords)

def SnapWriter_SnapshotEnd(writerHandle, attributeList, snapTime, contReadPos):
    c_SnapshotEnd = conf.lib.OTF2_SnapWriter_SnapshotEnd
    c_SnapshotEnd.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, ctypes.c_uint64 ]
    c_SnapshotEnd.restype = ErrorCode
    c_SnapshotEnd.errcheck = HandleErrorCode
    c_SnapshotEnd(writerHandle, attributeList, snapTime, contReadPos)

def SnapWriter_MeasurementOnOff(writerHandle, attributeList, snapTime, origEventTime, measurementMode):
    c_MeasurementOnOff = conf.lib.OTF2_SnapWriter_MeasurementOnOff
    c_MeasurementOnOff.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, MeasurementMode ]
    c_MeasurementOnOff.restype = ErrorCode
    c_MeasurementOnOff.errcheck = HandleErrorCode
    c_MeasurementOnOff(writerHandle, attributeList, snapTime, origEventTime, measurementMode)

def SnapWriter_Enter(writerHandle, attributeList, snapTime, origEventTime, region):
    c_Enter = conf.lib.OTF2_SnapWriter_Enter
    c_Enter.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, RegionRef ]
    c_Enter.restype = ErrorCode
    c_Enter.errcheck = HandleErrorCode
    c_Enter(writerHandle, attributeList, snapTime, origEventTime, region)

def SnapWriter_MpiSend(writerHandle, attributeList, snapTime, origEventTime, receiver, communicator, msgTag, msgLength):
    c_MpiSend = conf.lib.OTF2_SnapWriter_MpiSend
    c_MpiSend.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint32, CommRef, ctypes.c_uint32, ctypes.c_uint64 ]
    c_MpiSend.restype = ErrorCode
    c_MpiSend.errcheck = HandleErrorCode
    c_MpiSend(writerHandle, attributeList, snapTime, origEventTime, receiver, communicator, msgTag, msgLength)

def SnapWriter_MpiIsend(writerHandle, attributeList, snapTime, origEventTime, receiver, communicator, msgTag, msgLength, requestID):
    c_MpiIsend = conf.lib.OTF2_SnapWriter_MpiIsend
    c_MpiIsend.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint32, CommRef, ctypes.c_uint32, ctypes.c_uint64, ctypes.c_uint64 ]
    c_MpiIsend.restype = ErrorCode
    c_MpiIsend.errcheck = HandleErrorCode
    c_MpiIsend(writerHandle, attributeList, snapTime, origEventTime, receiver, communicator, msgTag, msgLength, requestID)

def SnapWriter_MpiIsendComplete(writerHandle, attributeList, snapTime, origEventTime, requestID):
    c_MpiIsendComplete = conf.lib.OTF2_SnapWriter_MpiIsendComplete
    c_MpiIsendComplete.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint64 ]
    c_MpiIsendComplete.restype = ErrorCode
    c_MpiIsendComplete.errcheck = HandleErrorCode
    c_MpiIsendComplete(writerHandle, attributeList, snapTime, origEventTime, requestID)

def SnapWriter_MpiRecv(writerHandle, attributeList, snapTime, origEventTime, sender, communicator, msgTag, msgLength):
    c_MpiRecv = conf.lib.OTF2_SnapWriter_MpiRecv
    c_MpiRecv.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint32, CommRef, ctypes.c_uint32, ctypes.c_uint64 ]
    c_MpiRecv.restype = ErrorCode
    c_MpiRecv.errcheck = HandleErrorCode
    c_MpiRecv(writerHandle, attributeList, snapTime, origEventTime, sender, communicator, msgTag, msgLength)

def SnapWriter_MpiIrecvRequest(writerHandle, attributeList, snapTime, origEventTime, requestID):
    c_MpiIrecvRequest = conf.lib.OTF2_SnapWriter_MpiIrecvRequest
    c_MpiIrecvRequest.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint64 ]
    c_MpiIrecvRequest.restype = ErrorCode
    c_MpiIrecvRequest.errcheck = HandleErrorCode
    c_MpiIrecvRequest(writerHandle, attributeList, snapTime, origEventTime, requestID)

def SnapWriter_MpiIrecv(writerHandle, attributeList, snapTime, origEventTime, sender, communicator, msgTag, msgLength, requestID):
    c_MpiIrecv = conf.lib.OTF2_SnapWriter_MpiIrecv
    c_MpiIrecv.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint32, CommRef, ctypes.c_uint32, ctypes.c_uint64, ctypes.c_uint64 ]
    c_MpiIrecv.restype = ErrorCode
    c_MpiIrecv.errcheck = HandleErrorCode
    c_MpiIrecv(writerHandle, attributeList, snapTime, origEventTime, sender, communicator, msgTag, msgLength, requestID)

def SnapWriter_MpiCollectiveBegin(writerHandle, attributeList, snapTime, origEventTime):
    c_MpiCollectiveBegin = conf.lib.OTF2_SnapWriter_MpiCollectiveBegin
    c_MpiCollectiveBegin.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp ]
    c_MpiCollectiveBegin.restype = ErrorCode
    c_MpiCollectiveBegin.errcheck = HandleErrorCode
    c_MpiCollectiveBegin(writerHandle, attributeList, snapTime, origEventTime)

def SnapWriter_MpiCollectiveEnd(writerHandle, attributeList, snapTime, origEventTime, collectiveOp, communicator, root, sizeSent, sizeReceived):
    c_MpiCollectiveEnd = conf.lib.OTF2_SnapWriter_MpiCollectiveEnd
    c_MpiCollectiveEnd.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, CollectiveOp, CommRef, ctypes.c_uint32, ctypes.c_uint64, ctypes.c_uint64 ]
    c_MpiCollectiveEnd.restype = ErrorCode
    c_MpiCollectiveEnd.errcheck = HandleErrorCode
    c_MpiCollectiveEnd(writerHandle, attributeList, snapTime, origEventTime, collectiveOp, communicator, root, sizeSent, sizeReceived)

def SnapWriter_OmpFork(writerHandle, attributeList, snapTime, origEventTime, numberOfRequestedThreads):
    c_OmpFork = conf.lib.OTF2_SnapWriter_OmpFork
    c_OmpFork.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint32 ]
    c_OmpFork.restype = ErrorCode
    c_OmpFork.errcheck = HandleErrorCode
    c_OmpFork(writerHandle, attributeList, snapTime, origEventTime, numberOfRequestedThreads)

def SnapWriter_OmpAcquireLock(writerHandle, attributeList, snapTime, origEventTime, lockID, acquisitionOrder):
    c_OmpAcquireLock = conf.lib.OTF2_SnapWriter_OmpAcquireLock
    c_OmpAcquireLock.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint32, ctypes.c_uint32 ]
    c_OmpAcquireLock.restype = ErrorCode
    c_OmpAcquireLock.errcheck = HandleErrorCode
    c_OmpAcquireLock(writerHandle, attributeList, snapTime, origEventTime, lockID, acquisitionOrder)

def SnapWriter_OmpTaskCreate(writerHandle, attributeList, snapTime, origEventTime, taskID):
    c_OmpTaskCreate = conf.lib.OTF2_SnapWriter_OmpTaskCreate
    c_OmpTaskCreate.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint64 ]
    c_OmpTaskCreate.restype = ErrorCode
    c_OmpTaskCreate.errcheck = HandleErrorCode
    c_OmpTaskCreate(writerHandle, attributeList, snapTime, origEventTime, taskID)

def SnapWriter_OmpTaskSwitch(writerHandle, attributeList, snapTime, origEventTime, taskID):
    c_OmpTaskSwitch = conf.lib.OTF2_SnapWriter_OmpTaskSwitch
    c_OmpTaskSwitch.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ctypes.c_uint64 ]
    c_OmpTaskSwitch.restype = ErrorCode
    c_OmpTaskSwitch.errcheck = HandleErrorCode
    c_OmpTaskSwitch(writerHandle, attributeList, snapTime, origEventTime, taskID)

def SnapWriter_Metric(writerHandle, attributeList, snapTime, origEventTime, metric, typeIDs, metricValues):
    numberOfMetrics = len(metricValues)
    assert(numberOfMetrics == len(typeIDs))

    TypeArrayType = Type * numberOfMetrics
    type_ids_list = TypeArrayType()
    type_ids_list[:] = typeIDs

    MetricValueArrayType = MetricValue * numberOfMetrics
    metric_values_list = MetricValueArrayType()
    metric_values_list[:] = metricValues

    c_Metric = conf.lib.OTF2_SnapWriter_Metric
    c_Metric.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp , TimeStamp, MetricRef, ctypes.c_uint8, TypeArrayType, MetricValueArrayType ]
    c_Metric.restype = ErrorCode
    c_Metric.errcheck = HandleErrorCode
    c_Metric(writerHandle, attributeList, snapTime , origEventTime, metric, numberOfMetrics, type_ids_list, metric_values_list)

def SnapWriter_ParameterString(writerHandle, attributeList, snapTime, origEventTime, parameter, string):
    c_ParameterString = conf.lib.OTF2_SnapWriter_ParameterString
    c_ParameterString.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ParameterRef, StringRef ]
    c_ParameterString.restype = ErrorCode
    c_ParameterString.errcheck = HandleErrorCode
    c_ParameterString(writerHandle, attributeList, snapTime, origEventTime, parameter, string)

def SnapWriter_ParameterInt(writerHandle, attributeList, snapTime, origEventTime, parameter, value):
    c_ParameterInt = conf.lib.OTF2_SnapWriter_ParameterInt
    c_ParameterInt.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ParameterRef, ctypes.c_int64 ]
    c_ParameterInt.restype = ErrorCode
    c_ParameterInt.errcheck = HandleErrorCode
    c_ParameterInt(writerHandle, attributeList, snapTime, origEventTime, parameter, value)

def SnapWriter_ParameterUnsignedInt(writerHandle, attributeList, snapTime, origEventTime, parameter, value):
    c_ParameterUnsignedInt = conf.lib.OTF2_SnapWriter_ParameterUnsignedInt
    c_ParameterUnsignedInt.argtypes = [ ctypes.POINTER(SnapWriter), ctypes.POINTER(AttributeList), TimeStamp, TimeStamp, ParameterRef, ctypes.c_uint64 ]
    c_ParameterUnsignedInt.restype = ErrorCode
    c_ParameterUnsignedInt.errcheck = HandleErrorCode
    c_ParameterUnsignedInt(writerHandle, attributeList, snapTime, origEventTime, parameter, value)

__all__ = [
    'SnapWriter',
    'SnapWriter_GetLocationID',
    'SnapWriter_SnapshotStart',
    'SnapWriter_SnapshotEnd',
    'SnapWriter_MeasurementOnOff',
    'SnapWriter_Enter',
    'SnapWriter_MpiSend',
    'SnapWriter_MpiIsend',
    'SnapWriter_MpiIsendComplete',
    'SnapWriter_MpiRecv',
    'SnapWriter_MpiIrecvRequest',
    'SnapWriter_MpiIrecv',
    'SnapWriter_MpiCollectiveBegin',
    'SnapWriter_MpiCollectiveEnd',
    'SnapWriter_OmpFork',
    'SnapWriter_OmpAcquireLock',
    'SnapWriter_OmpTaskCreate',
    'SnapWriter_OmpTaskSwitch',
    'SnapWriter_Metric',
    'SnapWriter_ParameterString',
    'SnapWriter_ParameterInt',
    'SnapWriter_ParameterUnsignedInt',
]
