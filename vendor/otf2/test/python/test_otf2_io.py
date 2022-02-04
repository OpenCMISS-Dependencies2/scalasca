import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp
import time

import otf2


TIMER_GRANULARITY = 1000000


def t():
    return int(round(time.time() * TIMER_GRANULARITY))


class TestOTF2IO(unittest.TestCase):
    def setUp(self):
        self.old_cwd = os.getcwd()
        self.tmp_dirname = mkdtemp(prefix=os.path.basename(os.path.abspath(__file__))[:-3] + '_tmp', dir=self.old_cwd)

        self.orig_trace = os.path.join(self.tmp_dirname, "orig")
        self.retrace = os.path.join(self.tmp_dirname, "retrc")

        os.mkdir(self.orig_trace)
        os.mkdir(self.retrace)

        os.chdir(self.tmp_dirname)

    def tearDown(self):
        os.chdir(self.old_cwd)
        if 'V' in os.environ and os.environ['V'] != '':
            print(self.tmp_dirname)
        else:
            rmtree(self.tmp_dirname)

    def generate_trace(self, archive_name):
        with otf2.writer.open(archive_name, timer_resolution=TIMER_GRANULARITY) as trace:
            system_tree_node = trace.definitions.system_tree_node("Root", parent=None)
            location_group = trace.definitions.location_group("Process",
                                                              system_tree_parent=system_tree_node)

            writer = trace.event_writer("thread", group=location_group)

            file = trace.definitions.io_regular_file("/home/me/foo.txt", system_tree_node)
            trace.definitions.io_file_property(file, "mount", otf2.Type.STRING, value="/home")
            directory = trace.definitions.io_directory("/etc", system_tree_node)

            posix_paradigm = trace.definitions.io_paradigm("POSIX", "MyFirstPosixParadigm",
                                                           otf2.IoParadigmClass.SERIAL,
                                                           otf2.IoParadigmFlag.NONE,
                                                           {otf2.IoParadigmProperty.VERSION: "1.0"})
            self.assertEqual(posix_paradigm[otf2.IoParadigmProperty.VERSION], "1.0")

            posix_paradigm[otf2.IoParadigmProperty.VERSION] = "2.0"
            self.assertEqual(posix_paradigm[otf2.IoParadigmProperty.VERSION], "2.0")

            posix_paradigm[otf2.IoParadigmProperty.VERSION] =\
                otf2.attribute_value.AttributeValue("3.0", otf2.Type.STRING)
            self.assertEqual(posix_paradigm[otf2.IoParadigmProperty.VERSION], "3.0")

            mpi_io_paradigm = trace.definitions.io_paradigm(
                identification="MPI-IO", name="MyFirstMPI-IOParadigm",
                io_paradigm_class=otf2.IoParadigmClass.PARALLEL,
                io_paradigm_flags=otf2.IoParadigmFlag.NONE,
                properties={otf2.IoParadigmProperty.VERSION:
                            otf2.attribute_value.AttributeValue("0.1", otf2.Type.STRING)})
            self.assertEqual(mpi_io_paradigm[otf2.IoParadigmProperty.VERSION], "0.1")

            stdin = trace.definitions.io_handle('stdin', None, posix_paradigm, otf2.IoHandleFlag.PRE_CREATED)
            trace.definitions.io_pre_created_handle_state(stdin, otf2.IoAccessMode.READ_ONLY)
            stdout = trace.definitions.io_handle('stdout', None, posix_paradigm)
            trace.definitions.io_pre_created_handle_state(stdout, otf2.IoAccessMode.WRITE_ONLY)
            stderr = trace.definitions.io_handle('stderr', None, posix_paradigm)
            trace.definitions.io_pre_created_handle_state(stderr, otf2.IoAccessMode.WRITE_ONLY)

            file_handle = trace.definitions.io_handle('', file, posix_paradigm)
            directory_handle = trace.definitions.io_handle('', directory, mpi_io_paradigm)

            writer.io_create_handle(t(), file_handle, otf2.IoAccessMode.WRITE_ONLY,
                                    otf2.IoCreationFlag.CREATE |
                                    otf2.IoCreationFlag.PATH,
                                    otf2.IoStatusFlag.SYNC |
                                    otf2.IoStatusFlag.CLOSE_ON_EXEC)

    def read_trace(self, archive_name):
        with otf2.reader.open(archive_name) as trace:
            # TODO check something
            pass

    def rewrite_trace(self, old_anchor_path, new_archive_path):
        with otf2.reader.open(old_anchor_path) as trace_reader:
            with otf2.writer.open(new_archive_path,
                                  definitions=trace_reader.definitions) as write_trace:
                for location, event in trace_reader.events:
                    writer = write_trace.event_writer_from_location(location)
                    writer(event)

    def test_rewrite(self):
        self.generate_trace(self.orig_trace)
        self.rewrite_trace(os.path.join(self.orig_trace, "traces.otf2"), self.retrace)
        self.read_trace(os.path.join(self.retrace, "traces.otf2"))


if __name__ == '__main__':
    unittest.main()
