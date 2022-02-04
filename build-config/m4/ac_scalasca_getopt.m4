##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2013                                                ##
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


AC_DEFUN([AC_SCALASCA_GETOPT],
[
AC_CHECK_HEADERS([getopt.h])
AC_CHECK_DECLS([getopt_long_only], [], [], [
    AC_INCLUDES_DEFAULT
    #ifdef HAVE_GETOPT_H
    # include <getopt.h>
    #endif
])

_AC_SCALASCA_GETOPT_LINK
AS_IF([test "x${with_libiberty_lib_checks_successful}" = "xno"],
      [AC_SCOREP_FRONTEND_LIB([libiberty], [], [])],
      [AM_CONDITIONAL([HAVE_LIBIBERTY], [test 1 -eq 0])
       AFS_SUMMARY([libiberty support], [not required])])
AS_IF([test "x${with_libiberty_lib_checks_successful}" = "xno"],
      [AC_MSG_ERROR([[cannot find getopt_long_only(). Please specify '--with-libiberty=<path>'.]])])
])


AC_DEFUN([_AC_SCOREP_LIBIBERTY_LIB_CHECK], [
LIBS="-liberty"
_AC_SCALASCA_GETOPT_LINK
AS_IF([test "x${with_libiberty_lib_checks_successful}" = "xyes"],
      [with_libiberty_libs="-liberty"],
      [with_libiberty_libs=""])
])


AC_DEFUN([_AC_SCALASCA_GETOPT_LINK], [
AC_LANG_PUSH([C++])
AC_LINK_IFELSE([AC_LANG_PROGRAM(
                [[extern "C" char getopt_long_only();]],
                [[getopt_long_only();]])],
               [with_libiberty_lib_checks_successful=yes],
               [with_libiberty_lib_checks_successful=no])
AC_LANG_POP
])
