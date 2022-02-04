##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2015                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  Copyright (c) 2009-2013                                                ##
##  German Research School for Simulation Sciences GmbH,                   ##
##  Laboratory for Parallel Programming                                    ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


# SCALASCA_DEBUG_OPTION
# ---------------------
# Scalasca-specific configure checks related to debugging.  Implicitly
# triggers AFS_DEBUG_OPTION to handle the '--enable-debug' option and
# set common variables and config header defines.
#
# List of configure variables set:
#   `ac_cv_header_valgrind_memcheck_h`::
#       'yes' if "valgrind/memcheck.h" is available/usable, 'no' otherwise
#
# List of provided config header defines:
#   `HAVE_VALGRIND_MEMCHECK_H`::
#       Set to '1' if "valgrind/memcheck.h" is available/usable, unset
#       otherwise
#
AC_DEFUN([SCALASCA_DEBUG_OPTION],
[
AC_REQUIRE([AFS_DEBUG_OPTION])
AS_IF([test "x$enable_debug" = "xyes"],
    [AC_CHECK_HEADERS([valgrind/memcheck.h])
     AFS_SUMMARY([Valgrind support], [$ac_cv_header_valgrind_memcheck_h])])
])
