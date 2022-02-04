#!/bin/sh

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2012,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       test/OTF2_AnchorFile_test/run_anchor_file_test.sh

set -e

cleanup()
{
    rm -rf OTF2_AnchorFile_trace
}
trap cleanup EXIT

cleanup
echo "No directories at all:"
$VALGRIND ./OTF2_AnchorFile_test

cleanup
echo "One archive path directory:"
mkdir -p OTF2_AnchorFile_trace
$VALGRIND ./OTF2_AnchorFile_test

cleanup
echo "All archive path directories:"
mkdir -p OTF2_AnchorFile_trace/trace
$VALGRIND ./OTF2_AnchorFile_test

cleanup
echo "Archive trace directory exists (must fail):"
mkdir -p OTF2_AnchorFile_trace/trace/archive
! $VALGRIND ./OTF2_AnchorFile_test
