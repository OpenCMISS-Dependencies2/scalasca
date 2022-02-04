#!/usr/bin/env python

import otf2
from otf2.events import *
import time
from tempfile import mkdtemp
import os
import otf2_writer


def read_trace(archive_name="TestArchive/traces.otf2"):
    with otf2.reader.open(archive_name) as trace:
        print("Read {} definitions".format(len(trace.definitions.strings)))

        for location, event in trace.events:
            print("Encountered event on location {} at {}".format(location, event))


if __name__ == '__main__':
    trace_dir = mkdtemp(prefix="trace_test.", dir=os.getcwd())
    os.rmdir(trace_dir)
    otf2_writer.generate_trace(trace_dir)
    print("Created trace in {}".format(trace_dir))
    read_trace("{}/traces.otf2".format(trace_dir))
    print("Read trace from {}/traces.otf2".format(trace_dir))
