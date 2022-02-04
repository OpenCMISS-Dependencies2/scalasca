## -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2012,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2014, 2018-2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2012, 2014-2015,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_SCOREP_GIT_CONTROLLED

AFS_MAINTAINER_MODE

AC_SCOREP_PLATFORM_SETTINGS

## Determine a C compiler to use. If CC is not already set in the environment,
## check for gcc and cc, then for other C compilers. Set output variable CC to
## the name of the compiler found.
##
## This macro may, however, be invoked with an optional first argument which,
## if specified, must be a blank-separated list of C compilers to search
## for. This just gives the user an opportunity to specify an alternative
## search list for the C compiler. For example, if you didn't like the default
## order, then you could invoke AC_PROG_CC like this: AC_PROG_CC([gcc cl cc])
AC_PROG_CC

## If the C compiler is not in C99 mode by default, try to add an option to
## output variable CC to make it so. This macro tries various options that
## select C99 on some system or another. It considers the compiler to be in
## C99 mode if it handles _Bool, // comments, flexible array members, inline,
## signed and unsigned long long int, mixed code and declarations, named
## initialization of structs, restrict, va_copy, varargs macros, variable
## declarations in for loops, and variable length arrays.  After calling this
## macro you can check whether the C compiler has been set to accept C99; if
## not, the shell variable ac_cv_prog_cc_c99 is set to `no'.
AC_SCOREP_PROG_CC_C99([], [AC_MSG_ERROR([No ISO C99 support in C compiler.])])
AFS_SUMMARY([C99 compiler used], [$CC])

AC_SCOREP_COMPILER_CHECKS

## Determine a C++ compiler to use. Check whether the environment variable CXX
## or CCC (in that order) is set; if so, then set output variable CXX to its
## value.
##
## Otherwise, if the macro is invoked without an argument, then search for a
## C++ compiler under the likely names (first g++ and c++ then other
## names). If none of those checks succeed, then as a last resort set CXX to
## g++.
##
## This macro may, however, be invoked with an optional first argument which,
## if specified, must be a blank-separated list of C++ compilers to search
## for.  This just gives the user an opportunity to specify an alternative
## search list for the C++ compiler. For example, if you didn't like the
## default order, then you could invoke AC_PROG_CXX like this:
## AC_PROG_CXX([gcc cl KCC CC cxx cc++ xlC aCC c++ g++])
AC_PROG_CXX

## Enable using per-target flags or subdir-objects with C sources
AM_PROG_CC_C_O

user_enable_shared=${enable_shared-no}
user_enable_static=${enable_static-yes}
enable_shared=yes
LT_PREREQ([2.4.6])
LT_INIT([pic-only])
AC_SUBST([LIBTOOL_DEPS])


## Macro: AC_C_BIGENDIAN ([action-if-true], [action-if-false],
## [action-if-unknown], [action-if-universal])
## The default for action-if-true is to define `WORDS_BIGENDIAN'. The default
## for action-if-false is to do nothing. The default for action-if-unknown is
## to abort configure and tell the installer how to bypass this test. And
## finally, the default for action-if-universal is to ensure that
## `WORDS_BIGENDIAN' is defined if and only if a universal build is detected
## and the current code is big-endian
AC_C_BIGENDIAN

## Search for a library defining function if it's not already available. This
## equates to calling ‘AC_LINK_IFELSE([AC_LANG_CALL([], [function])])’ first
## with no libraries, then for each library listed in search-libs.
##
## Add -llibrary to LIBS for the first library found to contain function, and
## run action-if-found. If the function is not found, run action-if-not-found.
##
## If linking with library results in unresolved symbols that would be
## resolved by linking with additional libraries, give those libraries as the
## other-libraries argument, separated by spaces: e.g., -lXt -lX11. Otherwise,
## this macro fails to detect that function is present, because linking the
## test program always fails with unresolved symbols.
#AC_SEARCH_LIBS([trunc], [m])

#AC_CXX_NAMESPACES
#AC_CXX_HAVE_SSTREAM
#AC_CXX_HAVE_STRSTREAM

OTF2_SIONLIB
AC_SCOREP_COND_HAVE([SIONLIB_SUPPORT],
                    [test "x${otf2_have_sionlib}" = "xyes"],
                    [Defined if SIONlib is available.])

# Set the PKGLIBEXECDIR shell variable, required by OTF2_DETECT_MIC_BUILD
adl_RECURSIVE_EVAL([${libexecdir}/${PACKAGE}], [PKGLIBEXECDIR])
AC_DEFINE_UNQUOTED([PKGLIBEXECDIR],
                   ["${PKGLIBEXECDIR}"],
                   [Directory where ]AC_PACKAGE_NAME[ executables to be run by other programs rather than by users are installed])
OTF2_DETECT_MIC_BUILD

AC_PROG_RANLIB

AFS_DEBUG_OPTION

AC_CUTEST_COLOR_TESTS
AC_SEARCH_LIBS([rint], [m])
AC_SCOREP_POSIX_FUNCTIONS
AC_SCOREP_DEFINE_REVISIONS

AFS_PORTABLE_MKDIR

AC_MSG_CHECKING([for stat])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
]], [[
struct stat stat_buffer;
stat( ".", &stat_buffer );
]])
    ],
    [AC_MSG_RESULT([yes])
     AC_DEFINE([HAVE_STAT], [1], [Define to 1 if the stat() function is available.])],
    [AC_MSG_RESULT([no])])

AM_CONDITIONAL([HAVE_ZLIB], [false])

AC_CONFIG_FILES([
    run_otf2_buffer_test.sh:../test/OTF2_Buffer_test/run_otf2_buffer_test.sh.in
], [
    chmod u+x run_otf2_buffer_test.sh
])
AC_CONFIG_FILES([
    run_otf2_integrity_test.sh:../test/OTF2_Integrity_test/run_otf2_integrity_test.sh.in
], [
    chmod u+x run_otf2_integrity_test.sh
])

OTF2_ENTROPY_SOURCES

OTF2_PTHREAD

AC_SUBST([PACKAGE_ERROR_CODES_INCDIR], [../include])
AC_DEFINE_UNQUOTED(
    [PACKAGE_ERROR_CODES_HEADER],
    [<otf2/OTF2_ErrorCodes.h>],
    [The #include argument used to include this packages error codes header.]
)

AM_CONDITIONAL([USER_BUILD_SHARED_LIBRARIES], [test "x${user_enable_shared}" = "xyes"])
AM_CONDITIONAL([USER_BUILD_STATIC_LIBRARIES], [test "x${user_enable_static}" = "xyes"])

AC_SCOREP_BUILD_MODE
AC_SCOREP_PACKAGE_AND_LIBRARY_VERSION

dnl Predicate to use HARDCODE_LIBDIR_FLAG_* only if linker needs it.
AS_IF([test "x${hardcode_into_libs}" = "xyes"],
      [ac_scorep_hardcode_into_libs=1],
      [ac_scorep_hardcode_into_libs=0])
AC_SUBST([HARDCODE_INTO_LIBS], [${ac_scorep_hardcode_into_libs}])
dnl Flag to hardcode libdir into a binary during linking, C version.
AC_SUBST([HARDCODE_LIBDIR_FLAG_CC], [${hardcode_libdir_flag_spec}])
dnl Flag to hardcode libdir into a binary during linking, C++ version.
AC_SUBST([HARDCODE_LIBDIR_FLAG_CXX], [${hardcode_libdir_flag_spec_CXX}])
dnl The ${wl} part of the HARDCODE_LIBDIR_FLAG_* defines.
AC_SUBST([HARDCODE_LIBDIR_WL], [${wl}])
dnl The ${aix_libpath} part of the HARDCODE_LIBDIR_FLAG_* defines. Only available on AIX.
AC_SUBST([HARDCODE_LIBDIR_AIX_LIBPATH], [${aix_libpath}])

adl_RECURSIVE_EVAL([${includedir}], [INCLUDEDIR])
AC_SUBST([INCLUDEDIR])

AC_SUBST([SYS_LIB_DLSEARCH_PATH_SPEC], [${sys_lib_dlsearch_path_spec}])

AC_CONFIG_FILES([../src/tools/otf2_config/otf2_config_data_]AFS_PACKAGE_BUILD_name[.cpp:../src/tools/otf2_config/otf2_config_data_build.cpp.in])
AC_CONFIG_FILES([../otf2-mini-config-data-]AFS_PACKAGE_BUILD_name[.sh:../otf2-mini-config-data.sh.in])
