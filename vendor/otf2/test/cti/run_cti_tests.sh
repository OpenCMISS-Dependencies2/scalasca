#!/bin/sh

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

set -e

for i in 1 2 3 4 16
do
    test ${V-0} -eq 1 && echo $i
    $VALGRIND ./test-cti -nt $i
done
