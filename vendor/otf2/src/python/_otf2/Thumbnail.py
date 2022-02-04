import ctypes

from .ErrorCodes import ErrorCode, HandleErrorCode
from .Config import conf, free, str_decode
from .GeneralDefinitions import ThumbnailType


class ThumbReader(ctypes.Structure):
    pass

class ThumbWriter(ctypes.Structure):
    pass

def ThumbReader_GetHeader(reader):
    # Note, DO NOT take the parameters by ref here - doesn't work for out string parameter
    name = ctypes.c_char_p()
    description = ctypes.c_char_p(),
    thumbType = ThumbnailType()
    numberOfSamples = ctypes.c_uint32()
    numberOfMetrics = ctypes.c_uint32()
    refsToDefs = ctypes.POINTER(ctypes.c_uint64)()

    c_GetHeader = conf.lib.OTF2_ThumbReader_GetHeader
    c_GetHeader.argtypes = [ ctypes.POINTER(ThumbReader),
                             ctypes.POINTER(ctypes.c_char_p),
                             ctypes.POINTER(ctypes.c_char_p),
                             ctypes.POINTER(ThumbnailType),
                             ctypes.POINTER(ctypes.c_uint32),
                             ctypes.POINTER(ctypes.c_uint32),
                             ctypes.POINTER(ctypes.POINTER(ctypes.c_uint64)) ]
    c_GetHeader.restype = ErrorCode
    c_GetHeader.errcheck = HandleErrorCode
    c_GetHeader(reader, ctypes.byref(name), ctypes.byref(description),
                ctypes.byref(thumbType), ctypes.byref(numberOfSamples),
                ctypes.byref(numberOfMetrics), ctypes.byref(refsToDefs))

    # Unpack the nice values from the pointers
    name_str = str_decode(name.value)
    description_str = str_decode(description.value)
    refs_to_defs_list = refsToDefs[0:numberOfMetrics.value]

    # Unfortunately we must free the memory that OTF2 has allocated for us
    free(name)
    free(description)
    free(refs_to_defs_list)

    return (name_str, description_str, thumbType, numberOfSamples.value,
            numberOfMetrics.value, refs_to_defs_list)

def ThumbReader_ReadSample(reader, numberOfMetrics):
    metric_samples_array_type = ctypes.c_uint64 * numberOfMetrics
    c_ReadSample = conf.lib.OTF2_ThumbReader_ReadSample
    c_ReadSample.argtypes = [ ctypes.POINTER(ThumbReader), ctypes.POINTER(ctypes.c_uint64), ctypes.c_uint32, metric_samples_array_type ]
    c_ReadSample.restype = ErrorCode
    c_ReadSample.errcheck = HandleErrorCode
    baseline = ctypes.c_uint64()
    metric_samples_array = metric_samples_array_type()
    c_ReadSample(reader, ctypes.byref(baseline), numberOfMetrics, metric_samples_array)
    return (baseline.value, metric_samples_array)

def ThumbWriter_WriteSample(writer, baseline, metricSamples):
    number_of_metrics = len(metricSamples)
    metric_samples_array_type = ctypes.c_uint64 * number_of_metrics
    metric_samples_array = metric_samples_array_type()

    c_WriteSample = conf.lib.OTF2_ThumbWriter_WriteSample
    c_WriteSample.argtypes = [ ctypes.POINTER(ThumbWriter), ctypes.c_uint64, ctypes.c_uint32, metric_samples_array_type ]
    c_WriteSample.restype = ErrorCode
    c_WriteSample.errcheck = HandleErrorCode
    c_WriteSample(writer, baseline, number_of_metrics, metric_samples_array)

__all__ = [
    'ThumbReader',
    'ThumbWriter',
    'ThumbReader_GetHeader',
    'ThumbReader_ReadSample',
    'ThumbWriter_WriteSample'
]
