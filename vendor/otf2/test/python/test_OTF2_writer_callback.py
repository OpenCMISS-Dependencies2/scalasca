#!/usr/bin/env python

import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp

import _otf2


class MyCallbacks(object):
    def __init__(self):
        self.calls = 0

    def pre_flush(self, user_data, file_type, location, caller_data, final):
        del file_type, location, caller_data, final

        self.calls += 1

        return _otf2.FLUSH


def post_flush(user_data, file_type, location):
    del file_type, location

    return get_time()


def get_time():
    get_time.counter += 1
    return get_time.counter

get_time.counter = -1
read_locations = []

global_time = 0
global_region = 0


class TestOTF2Callbacks(unittest.TestCase):

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

        callbacks = MyCallbacks()

        flush_callbacks = _otf2.FlushCallbacks(pre_flush=callbacks.pre_flush,
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
            _otf2.GlobalDefWriter_WriteLocation(global_def_writer, location, 0,
                                                _otf2.LOCATION_TYPE_CPU_THREAD,
                                                self.num_events * 6, 0)

        _otf2.GlobalDefWriter_WriteString(global_def_writer, 1, "MPI_Send")
        _otf2.GlobalDefWriter_WriteString(global_def_writer, 2, "Send an MPI message")

        _otf2.GlobalDefWriter_WriteString(global_def_writer, 3, "MPI_Recv")
        _otf2.GlobalDefWriter_WriteString(global_def_writer, 4, "Receive an MPI message")

        _otf2.GlobalDefWriter_WriteRegion(global_def_writer, 0, 1, 1, 2, _otf2.REGION_ROLE_UNKNOWN,
                                          _otf2.PARADIGM_UNKNOWN, _otf2.REGION_FLAG_NONE, 0, 0, 0)
        _otf2.GlobalDefWriter_WriteRegion(global_def_writer, 1, 3, 3, 4, _otf2.REGION_ROLE_UNKNOWN,
                                          _otf2.PARADIGM_UNKNOWN, _otf2.REGION_FLAG_NONE, 0, 0, 0)

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

        assert callbacks.calls == 10

    def test_callbacks(self):
        self.write_trace()


if __name__ == '__main__':
    unittest.main()
