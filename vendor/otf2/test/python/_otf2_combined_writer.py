#!/usr/bin/env python
#
# This file is part of the Score-P software (http://www.score-p.org)
#
# Copyright (c) 2015-2016,
# Technische Universitaet Dresden, Germany
#
# This software may be modified and distributed under the terms of
# a BSD-style license.  See the COPYING file in the package base
# directory for details.
#

import os
from tempfile import mkdtemp

import _otf2


def time_stamp():
    time_stamp.counter += 1
    return time_stamp.counter


time_stamp.counter = 0


def pre_flush(user_data, file_type, location, caller_data, final):
    return _otf2.FLUSH


def post_flush(user_data, file_type, location):
    return time_stamp()


def write_archive(path, name):
    archive = _otf2.Archive_Open(path, name,
                                 _otf2.FILEMODE_WRITE,
                                 1024 * 1024, 4 * 1024 * 1024,
                                 _otf2.SUBSTRATE_POSIX, _otf2.COMPRESSION_NONE)

    flush_callbacks = _otf2.FlushCallbacks(pre_flush=pre_flush, post_flush=post_flush)
    _otf2.Archive_SetFlushCallbacks(archive, flush_callbacks, None)
    _otf2.Archive_SetSerialCollectiveCallbacks(archive)

    write_events(archive)
    write_definitions(archive)
    write_global_definitions(archive)

    _otf2.Archive_Close(archive)


def write_definitions(archive):
    def_writer = _otf2.Archive_GetDefWriter(archive, 0)


def write_global_definitions(archive):
    global_def_writer = _otf2.Archive_GetGlobalDefWriter(archive)

    _otf2.GlobalDefWriter_WriteClockProperties(global_def_writer, 1, 0, 2)

    # GlobalDefWriter, StringRef, const char*
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 0, "")
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 1, "Master Process")
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 2, "Main Thread")
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 3, "My Function")
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 4, "other function")
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 5, "compute something")
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 6, "myHost")
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 7, "node")
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 8, "Temperature")
    _otf2.GlobalDefWriter_WriteString(global_def_writer, 9, "C")

    # GlobalDefWriter, RegionRef, StringRef, StringRef, StringRef, RegionRole,
    # Paradigm, RegionFlag, StringRef, uint32_t, uint32_t
    _otf2.GlobalDefWriter_WriteRegion(global_def_writer, 0, 3, 4, 5, _otf2.REGION_ROLE_FUNCTION,
                                      _otf2.PARADIGM_USER, _otf2.REGION_FLAG_NONE, 0, 0, 0)
    # GlobalDefWriter, SystemTreeNodeRef, StringRef, StringRef, SystemTreeNodeRef
    _otf2.GlobalDefWriter_WriteSystemTreeNode(global_def_writer, 0, 6, 7, _otf2.UNDEFINED_SYSTEM_TREE_NODE)
    # GlobalDefWriter, LocationGroupRef, StringRef, LocationGroupType, SystemTreeNodeRef
    _otf2.GlobalDefWriter_WriteLocationGroup(global_def_writer, 0, 1, _otf2.LOCATION_GROUP_TYPE_PROCESS, 0)
    # GlobalDefWriter, LocationRef, StringRef, LocationType, uint64_t, LocationGroupRef
    _otf2.GlobalDefWriter_WriteLocation(global_def_writer, 0, 2, _otf2.LOCATION_TYPE_CPU_THREAD, 2, 0)

    write_paradigm(global_def_writer)


def write_events(archive):
    _otf2.Archive_OpenEvtFiles(archive)

    evt_writer = _otf2.Archive_GetEvtWriter(archive, 0)

    _otf2.EvtWriter_Enter(evt_writer, None, time_stamp(), 0)
    _otf2.EvtWriter_Leave(evt_writer, None, time_stamp(), 0)

    _otf2.Archive_CloseEvtWriter(archive, evt_writer)
    _otf2.Archive_CloseEvtFiles(archive)


def write_paradigm(global_def_writer):
    str_id_mpi = write_string(global_def_writer, "MPI")
    _otf2.GlobalDefWriter_WriteParadigm(global_def_writer,
                                        _otf2.PARADIGM_MPI,
                                        str_id_mpi,
                                        _otf2.PARADIGM_CLASS_PROCESS)

    attribute_value = _otf2.AttributeValue()
    attribute_value.stringRef = write_string(global_def_writer, "MPI_COMM_${id}")
    _otf2.GlobalDefWriter_WriteParadigmProperty(global_def_writer,
                                                _otf2.PARADIGM_MPI,
                                                _otf2.PARADIGM_PROPERTY_COMM_NAME_TEMPLATE,
                                                _otf2.TYPE_STRING,
                                                attribute_value)

    attribute_value.stringRef = write_string(global_def_writer, "MPI_WIN_${id}")
    _otf2.GlobalDefWriter_WriteParadigmProperty(global_def_writer,
                                                _otf2.PARADIGM_MPI,
                                                _otf2.PARADIGM_PROPERTY_RMA_WIN_NAME_TEMPLATE,
                                                _otf2.TYPE_STRING,
                                                attribute_value)


#    type_ =_otf2.
#    _otf2.AttributeValue_SetBoolean(_otf2.FALSE, type_, attribute_value)
#    _otf2.GlobalDefWriter_WriteParadigmProperty(global_def_writer,
#                                                _otf2.PARADIGM_OPENMP,
#                                                _otf2.PARADIGM_PROPERTY_RMA_ONLY,
#                                                type_,
#                                                attribute_value)

def write_string(global_def_writer, string):
    _otf2.GlobalDefWriter_WriteString(global_def_writer, write_string.id, string)
    write_string.id += 1
    return write_string.id - 1


write_string.id = 0

if __name__ == '__main__':
    trace_dir = mkdtemp(prefix="trace_test.", dir=os.getcwd())
    os.rmdir(trace_dir)
    write_archive(trace_dir, "traces")
