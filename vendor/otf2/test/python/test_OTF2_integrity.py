#!/usr/bin/env python

import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp

import _otf2


def pre_flush(user_data, file_type, location, caller_data, final):
    del user_data, file_type, location, caller_data, final
    return _otf2.FLUSH


def post_flush(user_data, file_type, location):
    del user_data, file_type, location
    return get_time()


def get_time():
    get_time.counter += 1
    return get_time.counter

get_time.counter = -1
read_locations = []

global_time = 0
global_region = 0


def location_callback(userData, id, name, type, num_events, group):
    read_locations.append(id)


def enter_callback(location, time, userData, attributes, region):
    assert time >= global_time
    assert region == global_region


def leave_callback(location, time, userData, attributes, region):
    global global_time, global_region
    assert time >= global_time
    global_time = time

    assert region == global_region
    global_region = (global_region + 1) % 2

def mpi_send_callback(location, time, userData, attributes, receiver, comm, tag, length):
    global global_time
    assert time >= global_time
    global_time = time

    assert location == receiver
    assert comm == 0
    assert tag == 42
    assert length == 1024

def mpi_recv_callback(location, time, userData, attributes, sender, comm, tag, length):
    global global_time
    assert time >= global_time
    global_time = time

    assert location == sender
    assert comm == 0
    assert tag == 42
    assert length == 1024

class TestOTF2Integrity(unittest.TestCase):

    num_locations = 4
    num_events = 200
    archive_name = "OTF2_Integrity_trace"
    locations = [x * x for x in range(num_locations)]

    def setUp(self):
        self.old_cwd = os.getcwd()
        self.tmp_dirname = mkdtemp(prefix=os.path.basename(os.path.abspath(__file__))[:-3] + '_tmp', dir=self.old_cwd)
        os.chdir(self.tmp_dirname)

    def tearDown(self):
        os.chdir(self.old_cwd)
        if 'V' in os.environ and os.environ['V'] != '':
            print(self.tmp_dirname)
        else:
            rmtree(self.tmp_dirname)

    def write_trace(self):
        archive_path = "%s/%s" % (self.tmp_dirname, self.archive_name)
        archive = _otf2.Archive_Open(archive_path,
                                     "trace_files",
                                     _otf2.FILEMODE_WRITE,
                                     256 * 1024,
                                     _otf2.UNDEFINED_UINT64,
                                     _otf2.SUBSTRATE_POSIX,
                                     _otf2.COMPRESSION_NONE)

        flush_callbacks = _otf2.FlushCallbacks(pre_flush=pre_flush,
                                               post_flush=post_flush)
        _otf2.Archive_SetFlushCallbacks(archive,
                                        flushCallbacks=flush_callbacks,
                                        flushData=None)

        _otf2.Archive_SetSerialCollectiveCallbacks(archive)

        _otf2.Archive_SetDescription(archive, description="OTF2 trace for integrity test.")
        _otf2.Archive_SetCreator(archive, creator="OTF2_Integrity")

        _otf2.Archive_SetDefChunkSize(archive, 4 * 1024 * 1024)

        global_def_writer = _otf2.Archive_GetGlobalDefWriter(archive)

        _otf2.GlobalDefWriter_WriteString(global_def_writer, 0, "Process")

        for location in self.locations:
            _otf2.GlobalDefWriter_WriteLocation(
                global_def_writer, location, 0, _otf2.LOCATION_TYPE_CPU_THREAD, self.num_events * 6, 0)

        _otf2.GlobalDefWriter_WriteString(global_def_writer, 1, "MPI_Send")
        _otf2.GlobalDefWriter_WriteString(global_def_writer, 2, "Send an MPI message")

        _otf2.GlobalDefWriter_WriteString(global_def_writer, 3, "MPI_Recv")
        _otf2.GlobalDefWriter_WriteString(global_def_writer, 4, "Receive an MPI message")

        _otf2.GlobalDefWriter_WriteRegion(
            global_def_writer, 0, 1, 1, 2, _otf2.REGION_ROLE_UNKNOWN, _otf2.PARADIGM_UNKNOWN, _otf2.REGION_FLAG_NONE, 0, 0, 0)
        _otf2.GlobalDefWriter_WriteRegion(
            global_def_writer, 1, 3, 3, 4, _otf2.REGION_ROLE_UNKNOWN, _otf2.PARADIGM_UNKNOWN, _otf2.REGION_FLAG_NONE, 0, 0, 0)

        _otf2.Archive_CloseGlobalDefWriter(archive, global_def_writer)

        _otf2.Archive_OpenDefFiles(archive)

        for location in self.locations:
            def_writer = _otf2.Archive_GetDefWriter(archive, location)
            _otf2.Archive_CloseDefWriter(archive, def_writer)

        _otf2.Archive_CloseDefFiles(archive)

        _otf2.Archive_OpenEvtFiles(archive)

        for _ in range(self.num_events):
            for location in self.locations:
                evt_writer = _otf2.Archive_GetEvtWriter(archive, location)

                ts = get_time()

                _otf2.EvtWriter_Enter(evt_writer, None, ts, 0)
                _otf2.EvtWriter_MpiSend(evt_writer, None, ts, location, 0, 42, 1024)
                _otf2.EvtWriter_Leave(evt_writer, None, get_time(), 0)

                _otf2.EvtWriter_Enter(evt_writer, None, get_time(), 1)
                ts = get_time()
                _otf2.EvtWriter_MpiRecv(evt_writer, None, ts, location, 0, 42, 1024)
                _otf2.EvtWriter_Leave(evt_writer, None, ts, 1)

        for location in self.locations:
            evt_writer = _otf2.Archive_GetEvtWriter(archive, location)
            _otf2.Archive_CloseEvtWriter(archive, evt_writer)

        _otf2.Archive_CloseEvtFiles(archive)

        _otf2.Archive_Close(archive)

    def read_trace(self):
        archive_path = "%s/%s/%s.otf2" % (self.tmp_dirname, self.archive_name, "trace_files")
        reader = _otf2.Reader_Open(archive_path)
        _otf2.Reader_SetSerialCollectiveCallbacks(reader)

        num_locations = _otf2.Reader_GetNumberOfLocations(reader)
        assert num_locations == self.num_locations

        global_def_reader = _otf2.Reader_GetGlobalDefReader(reader)

        global_def_reader_callbacks = _otf2.GlobalDefReaderCallbacks_New()
        _otf2.GlobalDefReaderCallbacks_SetLocationCallback(
            global_def_reader_callbacks, location_callback)

        _otf2.Reader_RegisterGlobalDefCallbacks(reader, global_def_reader,
                                                global_def_reader_callbacks, None)

        _otf2.GlobalDefReaderCallbacks_Delete(global_def_reader_callbacks)

        definitions_read = _otf2.Reader_ReadGlobalDefinitions(
            reader, global_def_reader, _otf2.UNDEFINED_UINT64)

        _otf2.Reader_CloseGlobalDefReader(reader, global_def_reader)

        assert definitions_read == _otf2.Reader_GetNumberOfGlobalDefinitions(reader)

        for location in read_locations:
            _otf2.Reader_SelectLocation(reader, location)

        _otf2.Reader_OpenDefFiles(reader)
        _otf2.Reader_OpenEvtFiles(reader)

        for location in read_locations:
            def_reader = _otf2.Reader_GetDefReader(reader, location)
            definitions_read = _otf2.Reader_ReadAllLocalDefinitions(reader, def_reader)
            _otf2.Reader_CloseDefReader(reader, def_reader)
            evt_reader = _otf2.Reader_GetEvtReader(reader, location)

        _otf2.Reader_CloseDefFiles(reader)

        global_evt_reader = _otf2.Reader_GetGlobalEvtReader(reader)

        event_cbs = _otf2.GlobalEvtReaderCallbacks_New()
        _otf2.GlobalEvtReaderCallbacks_SetEnterCallback(event_cbs, enter_callback)
        _otf2.GlobalEvtReaderCallbacks_SetLeaveCallback(event_cbs, leave_callback)
        _otf2.GlobalEvtReaderCallbacks_SetMpiSendCallback(event_cbs, mpi_send_callback)
        _otf2.GlobalEvtReaderCallbacks_SetMpiRecvCallback(event_cbs, mpi_recv_callback)
        _otf2.Reader_RegisterGlobalEvtCallbacks(reader, global_evt_reader, event_cbs, None)
        _otf2.GlobalEvtReaderCallbacks_Delete(event_cbs)

        events_read = _otf2.Reader_ReadGlobalEvents(
            reader, global_evt_reader, _otf2.UNDEFINED_UINT64)

        assert events_read == 6 * self.num_locations * self.num_events

        _otf2.Reader_CloseGlobalEvtReader(reader, global_evt_reader)
        _otf2.Reader_CloseEvtFiles(reader)
        _otf2.Reader_Close(reader)

    def test_integrity(self):
        self.write_trace()
        self.read_trace()


if __name__ == '__main__':
    unittest.main()
