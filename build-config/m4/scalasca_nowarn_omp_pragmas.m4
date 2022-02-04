##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 2014                                                     ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


# SCALASCA_NOWARN_OMP_PRAGMAS
# ---------------------------
# Determines a suitable compiler option (for the currently selected language)
# to suppress warnings about unknown OpenMP pragmas when compiling with OpenMP
# support disabled.
#
# NOTES:
#   - This macro currently only works for C and C++.
#   - The selected compiler option may turn off all pragma warnings, not
#     only OpenMP-specific ones.
#
# List of provided automake substitutions:
#  `NOWARN_OMP_PRAGMAS_langFLAGS`::     Selected compiler option
#                                       (`lang` is either CC or CXX)
#
AC_DEFUN([SCALASCA_NOWARN_OMP_PRAGMAS],
[
AC_MSG_CHECKING([for $[]_AC_CC[] option to suppress OpenMP pragma warnings])

dnl Treat warnings as errors for all compile checks in this macro
_scalasca_save_[]_AC_LANG_ABBREV[]_werror_flag=$ac_[]_AC_LANG_ABBREV[]_werror_flag
ac_[]_AC_LANG_ABBREV[]_werror_flag=yes

dnl Try to compile w/o additional option first
AC_COMPILE_IFELSE([_SCALASCA_LANG_OMP_PRAGMA],
    [_scalasca_nowarn_omp_pragmas='none needed'],
    [_scalasca_nowarn_omp_pragmas='unsupported'
     dnl Compilation failed, so try these flags:
     dnl   Intel >= 14        -diag-disable 3180  (disables OpenMP pragma warnings only)
     dnl   Intel <  14        -diag-disable 161   (disables all pragma warnings)
     dnl   GCC                -Wno-unknown-pragmas
     dnl
     for scalasca_option in "-diag-disable 3180" "-diag-disable 161" \
                            "-Wno-unknown-pragmas"; do
         _scalasca_save_[]_AC_LANG_PREFIX[]FLAGS=$[]_AC_LANG_PREFIX[]FLAGS
         _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $scalasca_option"
         AC_COMPILE_IFELSE([_SCALASCA_LANG_OMP_PRAGMA],
             [_scalasca_nowarn_omp_pragmas=$scalasca_option])
         _AC_LANG_PREFIX[]FLAGS=$_scalasca_save_[]_AC_LANG_PREFIX[]FLAGS
         AS_IF([test "$_scalasca_nowarn_omp_pragmas" != 'unsupported'],
            [break])
     done])
AC_MSG_RESULT([$_scalasca_nowarn_omp_pragmas])

dnl Substitute output variable
AS_CASE([$_scalasca_nowarn_omp_pragmas],
    ["none needed" | unsupported],
        [AS_UNSET([NOWARN_OMP_PRAGMAS_[]_AC_LANG_PREFIX[]FLAGS])],
    [NOWARN_OMP_PRAGMAS_[]_AC_LANG_PREFIX[]FLAGS=$_scalasca_nowarn_omp_pragmas])
AC_SUBST([NOWARN_OMP_PRAGMAS_]_AC_LANG_PREFIX[FLAGS])

dnl Reset environment
ac_[]_AC_LANG_ABBREV[]_werror_flag=_scalasca_save_[]_AC_LANG_ABBREV[]_werror_flag
])


# _SCALASCA_LANG_OMP_PRAGMA
# -------------------------
# Expands to some language dependent source code for testing whether OpenMP
# pragmas issue a compiler warning.
#
AC_DEFUN([_SCALASCA_LANG_OMP_PRAGMA],
[AC_LANG_SOURCE([_AC_LANG_DISPATCH([$0], _AC_LANG, $@)])])


# _SCALASCA_LANG_OMP_PRAGMA(C)
# ----------------------------
# Test source for C.
#
m4_define([_SCALASCA_LANG_OMP_PRAGMA(C)],
[
int main () {
    #pragma omp parallel
    {}
    return 0;
}
])


# _SCALASCA_LANG_OMP_PRAGMA(C++)
# ------------------------------
# Use C test source also for C++.
#
m4_copy([_SCALASCA_LANG_OMP_PRAGMA(C)], [_SCALASCA_LANG_OMP_PRAGMA(C++)])
