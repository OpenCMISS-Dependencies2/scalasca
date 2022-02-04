#!/bin/sh

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013-2014,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       test/OTF2_Event_Seek_test/run_otf2_event_seek_test.sh

set -e

cleanup()
{
    rm -rf OTF2_Snapshot_Seek_trace
}
trap cleanup EXIT

cleanup
$VALGRIND ./OTF2_Snapshot_Seek_test
