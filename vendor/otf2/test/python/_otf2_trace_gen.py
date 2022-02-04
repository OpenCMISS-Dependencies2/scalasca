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

import argparse
import itertools
import os

import _otf2

# ___ Global variables. ____________________________________________________

# @internal
#  @brief Defines the number of locations to generate. */
otf2_LOCATIONS = 4

# @internal
#  @brief Defines the number of events to generate per location. */
otf2_EVENTS = 1

# @internal
#  @brief Defines if to generate MPI-only traces. This default if nothing is
# specified in command line parameters. */
otf2_MPI = True

# @internal
#  @brief Defines if to generate OpenMP-only traces. */
otf2_OPENMP = False

# @internal
#  @brief Defines if to generate hybrid (MPI and OpenMP) traces. */
otf2_HYBRID = False

# @internal
#  @brief Defines if to generate additional attributes to events. */
otf2_ATTRIBUTES = False


def get_time():
    get_time.counter += 1
    return get_time.counter


get_time.counter = -1


def pre_flush(user_data, file_type, location, caller_data, final):
    del user_data, file_type, location, caller_data, final
    return _otf2.FLUSH


def post_flush(user_data, file_type, location):
    del user_data, file_type, location
    return get_time()


def gpot(v):
    """ Calculate the greates power-of-two number which is less/equal to @a v. """
    p = v - 1
    p |= p >> 1
    p |= p >> 2
    p |= p >> 4
    p |= p >> 8
    p |= p >> 16
    p += 1
    if v < p:
        p >>= 1
    return p


# empty string definition */
STRING_EMPTY = 0

# definition IDs for regions */
REGION_MAIN = 0
REGION_OPENMP_PARALLEL = 1
REGION_OPENMP_LOOP = 2
REGION_OPENMP_IMPLICIT_BARRIER = 3
REGION_MPI_SEND = 4
REGION_MPI_RECV = 5
REGION_MPI_BARRIER = 6

# definition IDs for Groups */
GROUP_MPI_LOCATIONS = 0
GROUP_MPI_COMM_WORLD = 1
GROUP_MPI_COMM_SELF = 2
GROUP_ALL_LOCATIONS = 3
GROUP_MPI_COMM_CUBE = 4

# definition IDs for MPI comms */
MPI_COMM_MPI_COMM_WORLD = 0
MPI_COMM_MPI_COMM_SELF = 1
MPI_COMM_MPI_COMM_CUBE = 2

# definition IDs for metrics */
METRIC_A = 0
METRIC_B = 1
METRIC_C = 2
METRIC_D = 3
METRIC_E = 4
NUM_OF_METRICS = 5

# definition IDs for metric classes and instances */
METRIC_CLASS_1 = 0
METRIC_CLASS_2 = 1
METRIC_CLASS_3 = 2
METRIC_CLASS_4 = 3
METRIC_INSTANCE_1 = 4
NUM_OF_CLASSES = 5


def next_value_attr_1():
    next_value_attr_1.value.uint64 += 1
    return next_value_attr_1.value


next_value_attr_1.value = _otf2.AttributeValue()
next_value_attr_1.value.uint64 = 12345


def next_value_attr_2():
    next_value_attr_2.value.int16 += 1
    return next_value_attr_2.value


next_value_attr_2.value = _otf2.AttributeValue()
next_value_attr_2.value.int16 = 23456


# attributes */
class AttributeDef:
    def __init__(self, name, otf2_type, next_value):
        self.name = name
        self.type = otf2_type
        self.next_value = next_value


attribute_defs = [AttributeDef(name="attribute 1",
                               otf2_type=_otf2.TYPE_UINT64,
                               next_value=next_value_attr_1),
                  AttributeDef(name="attribute 2",
                               otf2_type=_otf2.TYPE_INT16,
                               next_value=next_value_attr_2)]


def add_attributes(attributes):
    if otf2_ATTRIBUTES:
        for a, attribute_def in enumerate(attribute_defs):
            _otf2.AttributeList_AddAttribute(attributes, a, attribute_def.type, attribute_def.next_value())


def record_for_all_locations(*_):
    return True


def record_for_location_0(rank, thread):
    return rank == 0 and thread == 0


# Main function
#
#  This tool generates a basic unified OTF2 archive to test the reader
#  components and tools.
#
#  @return                 Returns EXIT_SUCCESS if successful, EXIT_FAILURE
#                          if an error occures.
#
def main():
    archive_name = "TestTrace"

    # Array used to store metric values */
    metric_values = (_otf2.MetricValue * NUM_OF_METRICS)()

    # If you add new metrics please adapt also the array of metric types
    # and the routine to generate random metric values. */

    # Array of metric types */
    metric_types = [_otf2.TYPE_UINT64, _otf2.TYPE_DOUBLE, _otf2.TYPE_INT64,
                    _otf2.TYPE_DOUBLE, _otf2.TYPE_INT64]

    # Initialize accumulated metric */
    metric_values[0].unsigned_int = 0
    metric_values[1].floating_point = 12.5
    metric_values[2].signed_int = 1
    metric_values[3].floating_point = 10.0
    metric_values[4].unsigned_int = 1

    # Distribute metric members to its classes */
    metric_members_of_class_1 = [METRIC_A, METRIC_B]
    metric_members_of_class_2 = [METRIC_C]
    metric_members_of_class_3 = [METRIC_D]
    metric_members_of_class_4 = [METRIC_E]

    # Array of offsets needed to get correct type or values of a metric in corresponding arrays. */
    metric_classes_offsets = [0, 2, 3, 4, 4]

    # The collection of metric classes of instances for which we write metric events */
    class MetricWriteParameter:
        def __init__(self, id_, offset, count, record_for_location):
            self.id = id_
            self.metric_types = metric_types[offset:offset + count]
            self.metric_values = metric_values[offset:offset + count]
            self.record_for_location = record_for_location

    # FIXME missing class 4
    metric_write_parameters = [MetricWriteParameter(METRIC_CLASS_1, metric_classes_offsets[0],
                                                    len(metric_members_of_class_1),
                                                    record_for_all_locations),
                               MetricWriteParameter(METRIC_CLASS_2, metric_classes_offsets[1],
                                                    len(metric_members_of_class_2),
                                                    record_for_all_locations),
                               MetricWriteParameter(METRIC_CLASS_3, metric_classes_offsets[2],
                                                    len(metric_members_of_class_3),
                                                    record_for_all_locations),
                               MetricWriteParameter(METRIC_INSTANCE_1, metric_classes_offsets[3],
                                                    len(metric_members_of_class_4),
                                                    record_for_location_0)]

    def create_new_metric_values():
        metric_values[0].unsigned_int += 1
        metric_values[1].floating_point += 12.5
        metric_values[2].unsigned_int = -metric_values[2].unsigned_int
        metric_values[3].floating_point += 10.0
        metric_values[4].unsigned_int += 1

    def write_metrics(new, metric_time, metric_rank, metric_thread):
        if new:
            create_new_metric_values()

        for metric_write_parameter in metric_write_parameters:
            if metric_write_parameter.record_for_location(metric_rank, metric_thread):
                _otf2.EvtWriter_Metric(evt_writer, None, metric_time,
                                       metric_write_parameter.id,
                                       metric_write_parameter.metric_types,
                                       metric_write_parameter.metric_values)

    # Calculate number of MPI ranks and OpenMP threads. */
    num_ranks = 1
    num_threads = 1

    if otf2_MPI:
        num_ranks = otf2_LOCATIONS
        archive_path = "otf2_trace_gen_trace_mpi_{}_{}{}".format(num_ranks, otf2_EVENTS,
                                                                 "_attr" if otf2_ATTRIBUTES else "")
    elif otf2_OPENMP:
        num_threads = otf2_LOCATIONS
        archive_path = "otf2_trace_gen_trace_omp_{}_{}{}".format(num_threads, otf2_EVENTS,
                                                                 "_attr" if otf2_ATTRIBUTES else "")
    elif otf2_HYBRID:
        if (otf2_LOCATIONS % 4) == 0:
            num_threads = 4
        elif (otf2_LOCATIONS % 3) == 0:
            num_threads = 3
        elif (otf2_LOCATIONS % 2) == 0:
            num_threads = 2
        else:
            # FIXME shadow
            for num_threads in range(5, otf2_LOCATIONS):
                if (otf2_LOCATIONS % num_threads) == 0:
                    break

        num_ranks = otf2_LOCATIONS // num_threads
        archive_path = "otf2_trace_gen_trace_mpi_omp_{}x{}_{}{}".format(num_ranks, num_threads, otf2_EVENTS,
                                                                        "_attr" if otf2_ATTRIBUTES else "")
    else:
        raise RuntimeError("No paradigm set")

    print("\nGenerating an OTF2 test archive")
    print("with {} rank(s) and {} thread(s) each ({} locations total)"
          .format(num_ranks, num_threads, otf2_LOCATIONS))
    print("to {}/{}.otf2\n".format(archive_path, archive_name))

    if os.path.isdir(archive_path):
        for i in itertools.count():
            archive_path_backup = "{}_bk_{}".format(archive_path, i)
            if not os.path.exists(archive_path_backup):
                os.rename(archive_path, archive_path_backup)
                break

    # Create new archive handle. */
    archive = _otf2.Archive_Open(archive_path, archive_name, _otf2.FILEMODE_WRITE,
                                 1024 * 1024, 4 * 1024 * 1024,
                                 _otf2.SUBSTRATE_POSIX, _otf2.COMPRESSION_NONE)

    flush_callbacks = _otf2.FlushCallbacks(pre_flush=pre_flush, post_flush=post_flush)
    _otf2.Archive_SetFlushCallbacks(archive, flush_callbacks, None)

    # Set description, and creator. */
    _otf2.Archive_SetSerialCollectiveCallbacks(archive)
    _otf2.Archive_SetDescription(archive, "Simple OTF2 trace archive to valid reader components")
    _otf2.Archive_SetCreator(archive, "otf2_trace_gen")

    # Generate location IDs. Just to have non-consecutive location IDs. */
    locations = []
    mpi_ranks = []
    master_threads = []
    for rank in range(num_ranks):
        for thread in range(num_threads):
            locations.append((rank << 16) + thread)
        mpi_ranks.append(rank)
        master_threads.append(rank << 16)

    _otf2.Archive_OpenEvtFiles(archive)

    # Create local event writer. */
    # Create an attribute list. */
    attributes = None
    if otf2_ATTRIBUTES:
        attributes = _otf2.AttributeList_New()

    t = get_time()
    for rank in range(num_ranks):
        evt_writer = _otf2.Archive_GetEvtWriter(archive, master_threads[rank])

        # Enter main. */
        write_metrics(True, t, rank, 0)
        add_attributes(attributes)
        _otf2.EvtWriter_Enter(evt_writer, attributes, t, REGION_MAIN)

    for event in range(otf2_EVENTS):
        t = get_time()
        # FIXME - this makes no sense
        if not otf2_OPENMP:
            for rank in range(num_ranks):
                evt_writer = _otf2.Archive_GetEvtWriter(archive, master_threads[rank])

                # Write enter MPI_Send. */
                write_metrics(True, t, rank, 0)
                add_attributes(attributes)
                _otf2.EvtWriter_Enter(evt_writer, attributes, t, REGION_MPI_SEND)
                _otf2.EvtWriter_MpiSend(evt_writer, None, t, (rank + 1) % num_ranks,
                                        rank, 0, 1024)

                t = get_time()

                # Write leave MPI_Send. */
                write_metrics(True, t, rank, 0)
                add_attributes(attributes)
                _otf2.EvtWriter_Leave(evt_writer, attributes, t, REGION_MPI_SEND)

                # Immediately enter the MPI_Recv */
                t = get_time()

                # Write enter MPI_Recv. */
                write_metrics(True, t, rank, 0)
                add_attributes(attributes)
                _otf2.EvtWriter_Enter(evt_writer, attributes, t, REGION_MPI_RECV)

        if not otf2_OPENMP:
            for rank in range(num_ranks):
                evt_writer = _otf2.Archive_GetEvtWriter(archive, master_threads[rank])

                _otf2.EvtWriter_MpiRecv(evt_writer, None, t,
                                        (rank + num_ranks - 1) % num_ranks,
                                        rank, 0, 1024)

                # Write enter MPI_Recv. */
                write_metrics(True, t, rank, 0)
                add_attributes(attributes)
                _otf2.EvtWriter_Leave(evt_writer, attributes, t, REGION_MPI_RECV)

                # MPI Barrier. */
                t = get_time()

                write_metrics(True, t, rank, 0)
                add_attributes(attributes)
                _otf2.EvtWriter_Enter(evt_writer, attributes, t, REGION_MPI_BARRIER)

                _otf2.EvtWriter_MpiCollectiveBegin(evt_writer, None, t)

        # End all started MPI_Barrier */
        t = get_time()
        if not otf2_OPENMP:
            for rank in range(num_ranks):
                evt_writer = _otf2.Archive_GetEvtWriter(archive, master_threads[rank])
                _otf2.EvtWriter_MpiCollectiveEnd(evt_writer, None, t,
                                                 _otf2.COLLECTIVE_OP_BARRIER, rank,
                                                 _otf2.UNDEFINED_UINT32, 0, 0)

                write_metrics(True, t, rank, 0)
                add_attributes(attributes)
                _otf2.EvtWriter_Leave(evt_writer, attributes, t, REGION_MPI_BARRIER)

        # If in OpenMP or Hybrid mode. */
        if not otf2_MPI:
            t_enter = get_time()
            t_leave = get_time()

            for rank in range(num_ranks):
                evt_writer = _otf2.Archive_GetEvtWriter(archive, master_threads[rank])
                # At this point the master creates a team of several parallel threads */
                _otf2.EvtWriter_OmpFork(evt_writer, attributes, t_enter, num_threads)

            for rank in range(num_ranks):
                for thread in range(num_threads):
                    evt_writer = _otf2.Archive_GetEvtWriter(archive,
                                                            locations[num_threads * rank + thread])

                    # Enter OpenMP parallel  loop (outer region). */
                    write_metrics(True, t_enter, rank, thread)
                    add_attributes(attributes)
                    _otf2.EvtWriter_Enter(evt_writer, attributes, t_enter, REGION_OPENMP_PARALLEL)

                    # Enter OpenMP parallel loop  (inner region). */
                    write_metrics(False, t_enter, rank, thread)
                    add_attributes(attributes)
                    _otf2.EvtWriter_Enter(evt_writer, attributes, t_enter, REGION_OPENMP_LOOP)

                    # Enter OpenMP implicit barrier. */
                    write_metrics(False, t_enter, rank, thread)
                    add_attributes(attributes)
                    _otf2.EvtWriter_Enter(evt_writer, attributes, t_enter, REGION_OPENMP_IMPLICIT_BARRIER)

            for rank in range(num_ranks):
                for thread in range(num_threads):
                    evt_writer = _otf2.Archive_GetEvtWriter(archive,
                                                            locations[num_threads * rank + thread])

                    # Leave OpenMP implicit barrier. */
                    write_metrics(True, t_leave, rank, thread)
                    add_attributes(attributes)
                    _otf2.EvtWriter_Leave(evt_writer, attributes, t_leave, REGION_OPENMP_IMPLICIT_BARRIER)

                    # Leave OpenMP parallel loop (inner region). */
                    write_metrics(False, t_leave, rank, thread)
                    add_attributes(attributes)
                    _otf2.EvtWriter_Leave(evt_writer, attributes, t_leave, REGION_OPENMP_LOOP)

                    # Leave OpenMP parallel loop (outer region). */
                    write_metrics(False, t_leave, rank, thread)
                    add_attributes(attributes)
                    _otf2.EvtWriter_Leave(evt_writer, attributes, t_leave, REGION_OPENMP_PARALLEL)

            for rank in range(num_ranks):
                evt_writer = _otf2.Archive_GetEvtWriter(archive, master_threads[rank])
                # All threads have finished execution of statements in parallel region.
                # Now they synchronize and terminates. Only the master continues execution. */
                t_leave = get_time()
                _otf2.EvtWriter_OmpJoin(evt_writer, attributes, t_leave)

        if not otf2_OPENMP:
            for rank in range(num_ranks):
                evt_writer = _otf2.Archive_GetEvtWriter(archive, master_threads[rank])

                t = get_time()

                write_metrics(True, t, rank, 0)
                add_attributes(attributes)
                _otf2.EvtWriter_Enter(evt_writer, attributes, t, REGION_MPI_BARRIER)

                _otf2.EvtWriter_MpiCollectiveBegin(evt_writer, None, t)

        # End all started MPI_Barrier */
        t = get_time()
        if not otf2_OPENMP:
            for rank in range(num_ranks):
                evt_writer = _otf2.Archive_GetEvtWriter(archive, master_threads[rank])

                _otf2.EvtWriter_MpiCollectiveEnd(evt_writer, None, t,
                                                 _otf2.COLLECTIVE_OP_BARRIER,
                                                 rank, _otf2.UNDEFINED_UINT32,
                                                 0, 0)

                write_metrics(True, t, rank, 0)
                add_attributes(attributes)
                _otf2.EvtWriter_Leave(evt_writer, attributes, t, REGION_MPI_BARRIER)

    t = get_time()
    for rank in range(num_ranks):
        evt_writer = _otf2.Archive_GetEvtWriter(archive, master_threads[rank])

        # Leave main. */
        write_metrics(True, t, rank, 0)
        add_attributes(attributes)
        _otf2.EvtWriter_Leave(evt_writer, attributes, t, REGION_MAIN)

    # last timestamp */
    t = get_time()
    number_of_events_per_location = []
    for rank in range(num_ranks):
        for thread in range(num_threads):
            evt_writer = _otf2.Archive_GetEvtWriter(archive,
                                                    locations[num_threads * rank + thread])

            n = _otf2.EvtWriter_GetNumberOfEvents(evt_writer)
            number_of_events_per_location.append(n)

            _otf2.Archive_CloseEvtWriter(archive, evt_writer)

    _otf2.Archive_CloseEvtFiles(archive)

    # Write the global and local definitions. */
    glob_def_writer = _otf2.Archive_GetGlobalDefWriter(archive)

    # Write the timerange of the trace */
    _otf2.GlobalDefWriter_WriteClockProperties(glob_def_writer, 1, 0, t)

    _otf2.GlobalDefWriter_WriteString(glob_def_writer, STRING_EMPTY, "")

    string = STRING_EMPTY + 1

    # Write paradigm definitions */
    if otf2_MPI or otf2_HYBRID:
        _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "MPI")

        _otf2.GlobalDefWriter_WriteParadigm(glob_def_writer,
                                            _otf2.PARADIGM_MPI,
                                            string,
                                            _otf2.PARADIGM_CLASS_PROCESS)

        string += 1

        _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "MPI_COMM_${id}")

        value = _otf2.AttributeValue()
        value.stringRef = string
        string += 1
        _otf2.GlobalDefWriter_WriteParadigmProperty(glob_def_writer,
                                                    _otf2.PARADIGM_MPI,
                                                    _otf2.PARADIGM_PROPERTY_COMM_NAME_TEMPLATE,
                                                    _otf2.TYPE_STRING,
                                                    value)

        _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "MPI_WIN_${id}")

        value.stringRef = string
        string += 1
        _otf2.GlobalDefWriter_WriteParadigmProperty(glob_def_writer,
                                                    _otf2.PARADIGM_MPI,
                                                    _otf2.PARADIGM_PROPERTY_RMA_WIN_NAME_TEMPLATE,
                                                    _otf2.TYPE_STRING,
                                                    value)

        (type_, value) = _otf2.AttributeValue_SetBoolean(_otf2.FALSE)
        _otf2.GlobalDefWriter_WriteParadigmProperty(glob_def_writer,
                                                    _otf2.PARADIGM_OPENMP,
                                                    _otf2.PARADIGM_PROPERTY_RMA_ONLY,
                                                    type_,
                                                    value)

    if otf2_OPENMP or otf2_HYBRID:
        _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "OpenMP")

        _otf2.GlobalDefWriter_WriteParadigm(glob_def_writer,
                                            _otf2.PARADIGM_OPENMP,
                                            string,
                                            _otf2.PARADIGM_CLASS_THREAD_FORK_JOIN)
        string += 1

        _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "OMP Thread Team ${id}")

        value = _otf2.AttributeValue()
        value.stringRef = string
        string += 1
        # FIXME seems like a bad idea to write a hardcoded number 4 here
        _otf2.GlobalDefWriter_WriteParadigmProperty(glob_def_writer,
                                                    _otf2.PARADIGM_OPENMP,
                                                    4, _otf2.TYPE_STRING,
                                                    value)

    # Write system tree definitions. */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "Rack 0")

    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string + 1, "Rack")

    _otf2.GlobalDefWriter_WriteSystemTreeNode(glob_def_writer, 0,
                                              string, string + 1,
                                              _otf2.UNDEFINED_SYSTEM_TREE_NODE)

    string += 2

    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "Blade")

    string_blade = string
    string += 1

    for rank in range(num_ranks // 2):
        name_buffer = "Blade #{}".format(rank)

        _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, name_buffer)

        _otf2.GlobalDefWriter_WriteSystemTreeNode(glob_def_writer, rank + 1,
                                                  string, string_blade, 0)
        string += 1

    # Write location group and location definitions. */
    for rank in range(num_ranks):
        name_buffer = "Process {}".format(rank)
        _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, name_buffer)

        _otf2.GlobalDefWriter_WriteLocationGroup(glob_def_writer, rank,
                                                 string,
                                                 _otf2.LOCATION_GROUP_TYPE_PROCESS,
                                                 (rank // 2) + 1)
        string += 1

        for thread in range(num_threads):
            name_buffer = "Thread {}.{}".format(rank, thread)
            _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, name_buffer)

            _otf2.GlobalDefWriter_WriteLocation(glob_def_writer,
                                                locations[rank * num_threads + thread],
                                                string,
                                                _otf2.LOCATION_TYPE_CPU_THREAD,
                                                number_of_events_per_location[rank * num_threads + thread],
                                                rank)
            string += 1

    # Write region definition for "main". */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string,
                                      "main (mangled name)")
    string += 1
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string,
                                      "main (demangled name)")

    _otf2.GlobalDefWriter_WriteRegion(glob_def_writer, REGION_MAIN,
                                      string, string - 1,
                                      STRING_EMPTY,
                                      _otf2.REGION_ROLE_UNKNOWN,
                                      _otf2.PARADIGM_UNKNOWN,
                                      _otf2.REGION_FLAG_NONE,
                                      STRING_EMPTY, 0, 0)
    string += 1

    # Write region definition for an all-threads function. */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string,
                                      "!$omp parallelfor")
    _otf2.GlobalDefWriter_WriteRegion(glob_def_writer,
                                      REGION_OPENMP_PARALLEL,
                                      string, string, STRING_EMPTY,
                                      _otf2.REGION_ROLE_UNKNOWN,
                                      _otf2.PARADIGM_UNKNOWN,
                                      _otf2.REGION_FLAG_NONE,
                                      STRING_EMPTY, 0, 0)

    _otf2.GlobalDefWriter_WriteRegion(glob_def_writer, REGION_OPENMP_LOOP,
                                      string, string, STRING_EMPTY,
                                      _otf2.REGION_ROLE_UNKNOWN,
                                      _otf2.PARADIGM_UNKNOWN,
                                      _otf2.REGION_FLAG_NONE,
                                      STRING_EMPTY, 0, 0)
    string += 1

    # Write region definition for OpenMP implicit barrier. */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string,
                                      "!$omp implicit barrier")

    _otf2.GlobalDefWriter_WriteRegion(glob_def_writer,
                                      REGION_OPENMP_IMPLICIT_BARRIER,
                                      string, string, STRING_EMPTY,
                                      _otf2.REGION_ROLE_UNKNOWN,
                                      _otf2.PARADIGM_UNKNOWN,
                                      _otf2.REGION_FLAG_NONE,
                                      STRING_EMPTY, 0, 0)
    string += 1

    # Write region definition for "MPI_Send". */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "MPI_Send")

    _otf2.GlobalDefWriter_WriteRegion(glob_def_writer, REGION_MPI_SEND,
                                      string, string, STRING_EMPTY,
                                      _otf2.REGION_ROLE_UNKNOWN,
                                      _otf2.PARADIGM_UNKNOWN,
                                      _otf2.REGION_FLAG_NONE,
                                      STRING_EMPTY, 0, 0)
    string += 1

    # Write region definition for "MPI_Recv". */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "MPI_Recv")

    _otf2.GlobalDefWriter_WriteRegion(glob_def_writer, REGION_MPI_RECV,
                                      string, string, STRING_EMPTY,
                                      _otf2.REGION_ROLE_UNKNOWN,
                                      _otf2.PARADIGM_UNKNOWN,
                                      _otf2.REGION_FLAG_NONE,
                                      STRING_EMPTY, 0, 0)
    string += 1

    # Write region definition for "MPI_Barrier". */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "MPI_Barrier")

    _otf2.GlobalDefWriter_WriteRegion(glob_def_writer, REGION_MPI_BARRIER,
                                      string, string, STRING_EMPTY,
                                      _otf2.REGION_ROLE_UNKNOWN,
                                      _otf2.PARADIGM_UNKNOWN,
                                      _otf2.REGION_FLAG_NONE,
                                      STRING_EMPTY, 0, 0)
    string += 1

    # Define the list of locations which are mpi ranks */
    _otf2.GlobalDefWriter_WriteGroup(glob_def_writer, GROUP_MPI_LOCATIONS, STRING_EMPTY,
                                     _otf2.GROUP_TYPE_COMM_LOCATIONS, _otf2.PARADIGM_MPI,
                                     _otf2.GROUP_FLAG_NONE, master_threads)

    # Define the MPI group for MPI_COMM_WORLD */
    _otf2.GlobalDefWriter_WriteGroup(glob_def_writer, GROUP_MPI_COMM_WORLD, STRING_EMPTY,
                                     _otf2.GROUP_TYPE_COMM_GROUP, _otf2.PARADIGM_MPI,
                                     _otf2.GROUP_FLAG_NONE, mpi_ranks)

    # Define the MPI group for COMM_SELF like communicators */
    _otf2.GlobalDefWriter_WriteGroup(glob_def_writer, GROUP_MPI_COMM_SELF, STRING_EMPTY, _otf2.GROUP_TYPE_COMM_SELF,
                                     _otf2.PARADIGM_MPI, _otf2.GROUP_FLAG_NONE, [])

    # Define a group that includes all locations. It will be used by metric instance. */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "All locations")

    _otf2.GlobalDefWriter_WriteGroup(glob_def_writer, GROUP_ALL_LOCATIONS, string, _otf2.GROUP_TYPE_LOCATIONS,
                                     _otf2.PARADIGM_UNKNOWN, _otf2.GROUP_FLAG_NONE, locations)
    string += 1

    # Define MPI_COMM_WORD */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "My MPI_COMM_WORLD communicator")
    _otf2.GlobalDefWriter_WriteComm(glob_def_writer, MPI_COMM_MPI_COMM_WORLD,
                                    string, GROUP_MPI_COMM_WORLD, _otf2.UNDEFINED_COMM)
    string += 1

    # Define MPI_COMM_SELF */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "My MPI_COMM_SELF communicator")
    _otf2.GlobalDefWriter_WriteComm(glob_def_writer, MPI_COMM_MPI_COMM_SELF,
                                    string, GROUP_MPI_COMM_SELF, _otf2.UNDEFINED_COMM)
    string += 1

    # Write member definition of first metric */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "Metric_A")
    string += 1

    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "FLOPS")
    _otf2.GlobalDefWriter_WriteMetricMember(glob_def_writer, METRIC_A, string - 1, STRING_EMPTY,
                                            _otf2.METRIC_TYPE_PAPI, _otf2.METRIC_ACCUMULATED_START,
                                            metric_types[0], _otf2.BASE_DECIMAL, 0, string)
    string += 1

    # Write member definition of second metric */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "Metric_B")
    string += 1
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "s")

    _otf2.GlobalDefWriter_WriteMetricMember(glob_def_writer, METRIC_B, string - 1,
                                            STRING_EMPTY, _otf2.METRIC_TYPE_OTHER,
                                            _otf2.METRIC_ABSOLUTE_POINT, metric_types[1],
                                            _otf2.BASE_DECIMAL, -6, string)
    string += 1

    # Write member definition of third metric */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "Metric_C")
    string += 1
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "apples")

    _otf2.GlobalDefWriter_WriteMetricMember(glob_def_writer, METRIC_C, string - 1, STRING_EMPTY,
                                            _otf2.METRIC_TYPE_OTHER, _otf2.METRIC_RELATIVE_LAST,
                                            metric_types[2], _otf2.BASE_DECIMAL, 0, string)
    string += 1

    # Write member definition of fourth metric */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "Metric_D")
    string += 1
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "pears")

    _otf2.GlobalDefWriter_WriteMetricMember(glob_def_writer, METRIC_D, string - 1, STRING_EMPTY,
                                            _otf2.METRIC_TYPE_OTHER, _otf2.METRIC_ABSOLUTE_POINT,
                                            metric_types[3], _otf2.BASE_DECIMAL, 0, string)
    string += 1

    # Write member definition of fifth metric */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "Metric_E")
    string += 1
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "cherries")

    _otf2.GlobalDefWriter_WriteMetricMember(glob_def_writer, METRIC_E, string - 1, STRING_EMPTY,
                                            _otf2.METRIC_TYPE_OTHER, _otf2.METRIC_ABSOLUTE_POINT,
                                            metric_types[4], _otf2.BASE_DECIMAL, 0, string)
    string += 1

    # Define metric classes and instances */
    _otf2.GlobalDefWriter_WriteMetricClass(glob_def_writer, METRIC_CLASS_1, metric_members_of_class_1,
                                           _otf2.METRIC_SYNCHRONOUS_STRICT, _otf2.RECORDER_KIND_CPU)
    _otf2.GlobalDefWriter_WriteMetricClass(glob_def_writer, METRIC_CLASS_2, metric_members_of_class_2,
                                           _otf2.METRIC_SYNCHRONOUS_STRICT, _otf2.RECORDER_KIND_CPU)

    _otf2.GlobalDefWriter_WriteMetricClass(glob_def_writer, METRIC_CLASS_3, metric_members_of_class_3,
                                           _otf2.METRIC_SYNCHRONOUS_STRICT, _otf2.RECORDER_KIND_CPU)

    _otf2.GlobalDefWriter_WriteMetricClass(glob_def_writer, METRIC_CLASS_4, metric_members_of_class_4,
                                           _otf2.METRIC_ASYNCHRONOUS, _otf2.RECORDER_KIND_ABSTRACT)

    # We can define an instance with differing recorder and scope.
    # In this example metric values are valid for all locationa
    # but are recorded on first location. */
    _otf2.GlobalDefWriter_WriteMetricInstance(glob_def_writer, METRIC_INSTANCE_1, METRIC_CLASS_4, locations[0],
                                              _otf2.SCOPE_GROUP, GROUP_ALL_LOCATIONS)

    # write attribute defs */
    if otf2_ATTRIBUTES:
        for a, attribute_def in enumerate(attribute_defs):
            _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, attribute_def.name)
            _otf2.GlobalDefWriter_WriteAttribute(glob_def_writer, a, string, STRING_EMPTY, attribute_def.type)
            string += 1

    # create a sub communicator of MPI_COMM_WORLD and embed an hyper cube. */
    num_cube_ranks = gpot(num_ranks)
    cube_ranks = list(range(num_cube_ranks))

    # Define the MPI group for the hyper cube */
    _otf2.GlobalDefWriter_WriteGroup(glob_def_writer, GROUP_MPI_COMM_CUBE, STRING_EMPTY,
                                     _otf2.GROUP_TYPE_COMM_GROUP, _otf2.PARADIGM_MPI,
                                     _otf2.GROUP_FLAG_NONE, cube_ranks)

    # Define MPI_COMM_MPI_COMM_CUBE */
    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "MPI_COMM_CUBE")
    _otf2.GlobalDefWriter_WriteComm(glob_def_writer, MPI_COMM_MPI_COMM_CUBE, string,
                                    GROUP_MPI_COMM_WORLD, MPI_COMM_MPI_COMM_WORLD)

    string += 1

    num_cube_dims = 0
    cube_dim = 1
    while cube_dim < num_cube_ranks:
        name = "Cube {}".format(cube_dim)
        _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, name)
        _otf2.GlobalDefWriter_WriteCartDimension(glob_def_writer, num_cube_dims, string, 2, _otf2.CART_PERIODIC_FALSE)
        string += 1
        num_cube_dims += 1
        cube_dim <<= 1

    cube_dimensions = [_otf2.CartDimensionRef(cube_dim) for cube_dim in range(num_cube_dims)]

    _otf2.GlobalDefWriter_WriteString(glob_def_writer, string, "Hyper Cube")
    _otf2.GlobalDefWriter_WriteCartTopology(glob_def_writer, 0, string, MPI_COMM_MPI_COMM_CUBE, cube_dimensions)
    string += 1

    for rank in range(num_cube_ranks):
        cube_coordinates = [1 if rank & (2 ** cube_dim) else 0 for cube_dim in range(num_cube_dims)]
        _otf2.GlobalDefWriter_WriteCartCoordinate(glob_def_writer, 0, rank, cube_coordinates)

    _otf2.Archive_CloseGlobalDefWriter(archive, glob_def_writer)

    # write local mappings for MPI communicators and metrics */
    # write local mappings for metrics, this is an identity map, just to write
    # something out

    _otf2.Archive_OpenDefFiles(archive)

    metric_map = _otf2.IdMap_Create(_otf2.ID_MAP_DENSE, NUM_OF_CLASSES)
    for c in range(NUM_OF_CLASSES):
        _otf2.IdMap_AddIdPair(metric_map, c, c)

    for rank in range(num_ranks):
        mpi_comm_map = _otf2.IdMap_Create(_otf2.ID_MAP_SPARSE, 2)

        # Each location uses its rank as the communicator id which maps to the global 0 */
        _otf2.IdMap_AddIdPair(mpi_comm_map, rank, MPI_COMM_MPI_COMM_WORLD)
        _otf2.IdMap_AddIdPair(mpi_comm_map, rank + num_ranks, MPI_COMM_MPI_COMM_SELF)

        for thread in range(num_threads):
            # Open a definition writer, so the file is created. */
            def_writer = _otf2.Archive_GetDefWriter(archive, locations[num_threads * rank + thread])

            # Write metric mappings to local definitions. */
            _otf2.DefWriter_WriteMappingTable(def_writer, _otf2.MAPPING_METRIC, metric_map)

            if otf2_MPI or otf2_HYBRID:
                # Write MPI Comm mappings to local definitions. */
                _otf2.DefWriter_WriteMappingTable(def_writer, _otf2.MAPPING_COMM, mpi_comm_map)

            # Write clock offsets to local definitions. */
            _otf2.DefWriter_WriteClockOffset(def_writer, 0, 0, 0.0)
            _otf2.DefWriter_WriteClockOffset(def_writer, t, 0, 0.0)

            _otf2.Archive_CloseDefWriter(archive, def_writer)

        _otf2.IdMap_Free(mpi_comm_map)
    _otf2.IdMap_Free(metric_map)

    _otf2.Archive_CloseDefFiles(archive)

    # Finalize. */
    _otf2.Archive_Close(archive)

    _otf2.AttributeList_Delete(attributes)

    print("Done\n")


# ___ Implementation of static functions ___________________________________ */

# @internal
#  @brief Get command line parameters.
#
#  Parses command line parameters and checks for their existence.
#  Prints help for parameters '-h' or '--help'.
#
#  @param argc             Programs argument counter.
#  @param argv             Programs argument values.
def otf2_get_parameters():
    parser = argparse.ArgumentParser(description="Generate a basic unified OTF2 archive")
    parser.add_argument("-e", "--events", type=int, default=1, help="Approx. number of events to generate per location")
    parser.add_argument("-l", "--locations", type=int, default=4,
                        help="Number of locations to generate (Should be small)")
    parser.add_argument("-V", "--version", action="store_true")
    parser.add_argument("--mpi", action="store_true", help="MPI-only mode (default)")
    parser.add_argument("--openmp", action="store_true", help="OpenMP-only mode")
    parser.add_argument("--hybrid", action="store_true", help="Hybrid mode")
    parser.add_argument("--attributes", action="store_true", help="Add attributes to enter/leave events")
    args = parser.parse_args()

    if args.version:
        print("otf2-trace-gen: version {}".format(_otf2.VERSION))
        exit(0)

    global otf2_MPI, otf2_OPENMP, otf2_HYBRID, otf2_ATTRIBUTES, otf2_LOCATIONS, otf2_EVENTS
    otf2_MPI = False
    otf2_OPENMP = False
    otf2_HYBRID = False
    if args.mpi:
        otf2_MPI = True
    elif args.openmp:
        otf2_OPENMP = True
    elif args.hybrid:
        otf2_HYBRID = True
    else:
        otf2_MPI = True

    otf2_ATTRIBUTES = args.attributes
    otf2_LOCATIONS = args.locations
    otf2_EVENTS = args.events


def otf2_set_parameters(paradigm="mpi", attributes=False, locations=4, events=1):
    global otf2_MPI, otf2_OPENMP, otf2_HYBRID, otf2_ATTRIBUTES, otf2_LOCATIONS, otf2_EVENTS
    otf2_MPI = False
    otf2_OPENMP = False
    otf2_HYBRID = False
    if paradigm == 'mpi':
        otf2_MPI = True
    elif paradigm == 'openmp':
        otf2_OPENMP = True
    elif paradigm == 'hybrid':
        otf2_HYBRID = True
    otf2_ATTRIBUTES = attributes
    otf2_LOCATIONS = locations
    otf2_EVENTS = events


if __name__ == '__main__':
    otf2_get_parameters()
    main()
