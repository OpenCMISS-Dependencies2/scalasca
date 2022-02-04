##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 2019                                                     ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


AC_DEFUN([SCALASCA_TOPLEVEL_ARGS],[
AC_REQUIRE([AC_SCOREP_TOPLEVEL_ARGS])
AC_REQUIRE([AC_PROG_GREP])
# If BACKEND_TEST_RUNS_NON_RECURSIVE and --enable-backend-test-runs
# are given, don't pass --enable-backend-test-runs to sub-packages by
# removing --enable-backend-test-runs from user_provided_configure_args.
# Still pass to build-backend/mpi via $backend_args.
AC_ARG_VAR([BACKEND_TEST_RUNS_NON_RECURSIVE],
    [Whether to prevent passing --enable-backend-test-runs to sub-packages. Activation values are '1', 'yes', and 'true'.])
AS_UNSET([backend_args])
AS_IF([test "x${BACKEND_TEST_RUNS_NON_RECURSIVE}" = "x1" ||
       test "x${BACKEND_TEST_RUNS_NON_RECURSIVE}" = "xyes" ||
       test "x${BACKEND_TEST_RUNS_NON_RECURSIVE}" = "xtrue"],
    [GREP_OPTIONS= $GREP '^--enable-backend-test-runs' user_provided_configure_args > /dev/null
     AS_IF([test $? -eq 0],
         [GREP_OPTIONS= $GREP -v '^--enable-backend-test-runs' user_provided_configure_args > user_provided_configure_args_
          mv user_provided_configure_args_ user_provided_configure_args
          backend_args="--enable-backend-test-runs"
         ])
    ])
]) # SCALASCA_TOPLEVEL_ARGS
