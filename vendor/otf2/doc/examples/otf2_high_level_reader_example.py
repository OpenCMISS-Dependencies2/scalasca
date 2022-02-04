#!/usr/bin/env python
#
# This file is part of the Score-P software (http://www.score-p.org)
#
# Copyright (c) 2015-2017,
# Technische Universitaet Dresden, Germany
#
# This software may be modified and distributed under the terms of
# a BSD-style license.  See the COPYING file in the package base
# directory for details.
#

import otf2


with otf2.reader.open('TestArchive/traces.otf2') as trace:
    print("Read {} string definitions".format(len(trace.definitions.strings)))

    for location, event in trace.events:
        print("Encountered {} event {} on {}".format(type(event).__name__,
                                                              event, location))
