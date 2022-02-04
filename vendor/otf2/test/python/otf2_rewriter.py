#!/usr/bin/env python

import otf2
import sys


def rewrite(old_anchor_path, new_archive_path):
    with otf2.reader.open(old_anchor_path) as trace_reader:
        with otf2.writer.open(new_archive_path,
                              definitions=trace_reader.definitions) as write_trace:
            for location, event in trace_reader.events:
                writer = write_trace.event_writer_from_location(location)
                event.time -= trace_reader.clock_properties.global_offset
                writer(event)


if __name__ == '__main__':
    rewrite(sys.argv[1], sys.argv[2])
