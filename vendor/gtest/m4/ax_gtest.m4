##*************************************************************************##
##  Copyright (c) 2013                                                     ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


# AX_GTEST
# --------
# Configures the Google C++ Testing Framework. Requires the path to the
# top-level GTest source directory relative to the location of the calling
# configure.ac.
#
# List of provided automake substitutions:
#  `GTEST_PATH`::      Top-level GTest source directory
#  `GTEST_CPPFLAGS`::  Preprocessor flags to be used when compiling tests
#  `GTEST_CXXFLAGS`::  C++ compiler flags to be used when compiling tests
#  `GTEST_LIBS`::      Libraries/linker flags to be used when linking tests
#
# List of provided automake conditionals:
#  `HAVE_GTEST_TYPED_TESTS`::  Enabled if all features required by typed tests
#                              are supported by the C++ compiler, disabled
#                              otherwise
#
AC_DEFUN([AX_GTEST], [
    m4_ifblank([$1],
        [m4_fatal([Path to GTest directory relative to configure.ac required])])
    AC_REQUIRE([AC_PROG_CXX])
    AC_REQUIRE([AX_PTHREAD])

    AS_VAR_SET([gtest_cppflags], ["-I$srcdir/$1/include"])

    AS_IF([test "x$ax_pthread_ok" = xyes],
        [AS_VAR_APPEND([gtest_cppflags], [" -DGTEST_HAS_PTHREAD=1"])],
        [AS_VAR_APPEND([gtest_cppflags], [" -DGTEST_HAS_PTHREAD=0"])])

    _AX_GTEST_CXX_TR1_TUPLE
    AS_IF([test "x$ax_gtest_cxx_tr1_tuple" = xno],
        [AS_VAR_APPEND([gtest_cppflags], [" -DGTEST_USE_OWN_TR1_TUPLE=1"])],
        [AS_VAR_APPEND([gtest_cppflags], [" -DGTEST_USE_OWN_TR1_TUPLE=0"])])

    _AX_GTEST_CXX_VARIADIC_TEMPLATES
    AS_IF([test "x$ax_gtest_cxx_variadic_templates" = xyes],
        [AS_VAR_APPEND([gtest_cppflags], [" -DGTEST_HAS_TYPED_TEST=1"])
         AS_VAR_APPEND([gtest_cppflags], [" -DGTEST_HAS_TYPED_TEST_P=1"])])
    AM_CONDITIONAL([HAVE_GTEST_TYPED_TESTS],
        [test "x$ax_gtest_cxx_variadic_templates" = xyes])

    AC_SUBST([GTEST_PATH],     ["$1"])
    AC_SUBST([GTEST_CPPFLAGS], ["$gtest_cppflags"])
    AC_SUBST([GTEST_CXXFLAGS], ["$PTHREAD_CFLAGS"])
    AC_SUBST([GTEST_LIBS],     ["$PTHREAD_LIBS"])
])


# _AX_GTEST_CXX_TR1_TUPLE
# -----------------------
# Checks whether the <tr1/tuple> header file specified in the C++ TR1
# (ISO/IEC TR 19768:2007, C++ Library Extensions) exists.
#
# List of configure variables set:
#  `ax_gtest_cxx_tr1_tuple`::  'yes' if <tr1/tuple> header exists, 'no'
#                              otherwise
#
AC_DEFUN([_AX_GTEST_CXX_TR1_TUPLE], [
    AC_MSG_CHECKING([for tr1/tuple])
    AC_LANG_PUSH([C++])
    AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[@%:@include <tr1/tuple>]],
            [[using std::tr1::tuple;]])],
        AS_VAR_SET([ax_gtest_cxx_tr1_tuple], [yes]),
        AS_VAR_SET([ax_gtest_cxx_tr1_tuple], [no]))
    AC_LANG_POP
    AC_MSG_RESULT([$ax_gtest_cxx_tr1_tuple])
])


# _AX_GTEST_CXX_VARIADIC_TEMPLATES
# --------------------------------
# Checks whether the C++ compiler supports variadic templates.
#
# List of configure variables set:
#  `ax_gtest_cxx_variadic_templates`::  'yes' if variadic templates are
#                                       supported, 'no' otherwise
#
AC_DEFUN([_AX_GTEST_CXX_VARIADIC_TEMPLATES], [
    AC_MSG_CHECKING([whether $CXX supports variadic templates])
    AC_LANG_PUSH([C++])
    AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[template <typename... Args> struct Foo;]], [])],
        AS_VAR_SET([ax_gtest_cxx_variadic_templates], [yes]),
        AS_VAR_SET([ax_gtest_cxx_variadic_templates], [no]))
    AC_LANG_POP
    AC_MSG_RESULT([$ax_gtest_cxx_variadic_templates])
])
