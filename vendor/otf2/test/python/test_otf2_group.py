import os
import os.path
import unittest
from shutil import rmtree
from tempfile import mkdtemp

import otf2


TIMER_GRANULARITY = 1000000


def t():
    return int(round(time.time() * TIMER_GRANULARITY))


class TestOTF2Groups(unittest.TestCase):
    def setUp(self):
        self.old_cwd = os.getcwd()
        self.tmp_dirname = mkdtemp()
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

    num_locations = 8
    paradigm = otf2.Paradigm.MPI
    group_ranks = range(num_locations-1, num_locations//2-1, -1)

    def generate_trace(self, archive_name):
        with otf2.writer.open(archive_name, timer_resolution=TIMER_GRANULARITY) as trace:
            self.definitions = trace.definitions

            system_tree_node = trace.definitions.system_tree_node("Root", parent=None)
            location_group = trace.definitions.location_group("Process",
                                                              system_tree_parent=system_tree_node)
            locations = [trace.definitions.location("Location {}".format(rank),
                                                    group=location_group)
                         for rank in range(self.num_locations)]
            writers = [trace.event_writer_from_location(location) for location in locations]

            mpi_world = trace.definitions.group("MPI_COMM_WORLD",
                                                paradigm=self.paradigm,
                                                group_type=otf2.GroupType.COMM_LOCATIONS,
                                                members=locations)

            group1 = trace.definitions.group("MPI_Group1",
                                             paradigm=self.paradigm,
                                             group_type=otf2.GroupType.COMM_GROUP,
                                             members=self.group_ranks)

            group2 = trace.definitions.group("MPI_Group2",
                                             paradigm=self.paradigm,
                                             group_type=otf2.GroupType.COMM_GROUP,
                                             members=[locations[rank] for rank in
                                                      self.group_ranks])

            self.assert_groups_equal(mpi_world, group1, group2)

    def read_trace(self, archive_name):
        with otf2.reader.open(archive_name) as trace:
            self.assertEqual(3, len(trace.definitions.groups))
            mpi_world = [group for group in trace.definitions.groups if group.group_type ==
                         otf2.GroupType.COMM_LOCATIONS]
            self.assertEqual(1, len(mpi_world))
            mpi_world, = mpi_world
            comm_groups = [group for group in trace.definitions.groups if group.group_type ==
                           otf2.GroupType.COMM_GROUP]
            self.assertEqual(2, len(comm_groups))
            self.assert_groups_equal(mpi_world, *comm_groups)

    def rewrite_trace(self, old_anchor_path, new_archive_path):
        with otf2.reader.open(old_anchor_path) as trace_reader:
            with otf2.writer.open(new_archive_path,
                                  definitions=trace_reader.definitions) as write_trace:
                for location, event in trace_reader.events:
                    writer = write_trace.event_writer_from_location(location)
                    writer(event)

    def assert_groups_equal(self, world, group1, group2):
            for global_rank, local_rank, l1, l2 in \
                    zip(self.group_ranks, range(len(self.group_ranks)), group1.members, group2.members):
                self.assertIs(l1, l2)
                self.assertIs(l1, group1.location(local_rank))
                self.assertIs(l1, group2.location(local_rank))
                self.assertEqual(local_rank, group1.rank(l1))
                self.assertEqual(local_rank, group2.rank(l1))

                self.assertIs(l1, world.location(global_rank))
                self.assertIs(global_rank, world.rank(l1))

    def test_rewrite(self):
        self.generate_trace(self.orig_trace)
        self.rewrite_trace(os.path.join(self.orig_trace, "traces.otf2"), self.retrace)
        self.read_trace(os.path.join(self.retrace, "traces.otf2"))


if __name__ == '__main__':
    unittest.main()
