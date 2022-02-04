##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2017                                                ##
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


AC_DEFUN([AC_SCALASCA_ZLIB], [
    AS_IF([test "x${ac_scorep_platform}" = xk || \
           test "x${ac_scorep_platform}" = xfx10 || \
           test "x${ac_scorep_platform}" = xfx100],
        [AC_DEFINE([HAVE_ZLIB_H])
         BYPASS_GENERIC_LIB_CHECK_ON_FUJITSU([libz], [-lz])],
        [AC_SCOREP_BACKEND_LIB([libz], [zlib.h], [])])
])


AC_DEFUN([_AC_SCOREP_LIBZ_LIB_CHECK], [
    AC_LANG_PUSH([C++])

    # Check function declarations
    AC_CHECK_DECLS([gzopen, gzclose, gzread], [],
                   [with_libz_lib_checks_successful=no],
                   [
                       AC_INCLUDES_DEFAULT
                       #ifdef HAVE_ZLIB_H
                       # include <zlib.h>
                       #endif
                   ])

    # Link check
    AS_IF([test "x${with_libz_lib_checks_successful}" != "xno"],
          [LIBS="-lz"
           AC_LINK_IFELSE([AC_LANG_PROGRAM([[
                              #ifdef HAVE_ZLIB_H
                              # include <zlib.h>
                              #endif
                          ]],
                          [[
                              int    x  = 42;
                              gzFile fp = gzopen("foo", "r");
                              gzread(fp, &x, sizeof(x));
                              gzclose(fp);
                          ]])],
                          [with_libz_lib_checks_successful=yes
                           with_libz_libs="-lz"],
                          [with_libz_lib_checks_successful=no
                           with_libz_libs=""])
          ])

    AC_LANG_POP([C++])
])
