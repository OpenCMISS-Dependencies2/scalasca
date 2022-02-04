##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 2016-2020                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


# SCALASCA_COMMON_UTILS
# ---------------------
# Configures the common utils code.
#
# List of provided config header defines:
#   `PACKAGE_ERROR_CODES_HEADER`::
#       The file argument used to include the Scalasca error codes header
#
# List of provided automake substitutions:
#   `PACKAGE_ERROR_CODES_INCDIR`::
#       Include directory relative to $builddir where the configure-generated
#       header files can be found
#
AC_DEFUN([SCALASCA_COMMON_UTILS],
[
AC_SCOREP_POSIX_FUNCTIONS
AC_SUBST([PACKAGE_ERROR_CODES_INCDIR], [../src])
AC_DEFINE_UNQUOTED(
    [PACKAGE_ERROR_CODES_HEADER],
    [<ScalascaErrorCodes.h>],
    [The file argument used to include this packages' error codes header.])
])
