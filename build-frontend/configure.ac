##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2021                                                ##
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


##--- Setup -----------------------------------------------------------------

## Set up Autoconf
AC_PREREQ([2.69])
AC_INIT([Scalasca],
        m4_esyscmd([../build-config/common/generate-package-version.sh ../build-config/VERSION]),
        [scalasca@fz-juelich.de],,[https://www.scalasca.org])
AFS_PACKAGE_BUILD_INIT([frontend])

## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4])

## Define frontend-specific environment variables
AC_ARG_VAR(CC_FOR_BUILD, [C compiler command for the frontend build])
AC_ARG_VAR(CXX_FOR_BUILD, [C++ compiler command for the frontend build])
AC_ARG_VAR(CPPFLAGS_FOR_BUILD, [(Objective) C/C++ preprocessor flags for the frontend build, e.g. -I<include dir> if you have headers in a nonstandard directory <include dir>])
AC_ARG_VAR(CFLAGS_FOR_BUILD, [C compiler flags for the frontend build])
AC_ARG_VAR(CXXFLAGS_FOR_BUILD, [C++ compiler flags for the frontend build])
AC_ARG_VAR(LDFLAGS_FOR_BUILD, [linker flags for the frontend build, e.g. -L<lib dir> if you have libraries in a nonstandard directory <lib dir>])
AC_ARG_VAR(LIBS_FOR_BUILD, [libraries to pass to the linker for the frontend build, e.g. -l<library>])

## Consistency checks for compilers
AC_SCOREP_CHECK_COMPILER_VAR_SET([CC_FOR_BUILD])
AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX_FOR_BUILD])

## Convert compilers & flags
AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CC])
AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CXX])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CPPFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CXXFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([LDFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([LIBS])

## Set up Automake
AM_INIT_AUTOMAKE([1.13.4 -Wall foreign parallel-tests color-tests tar-pax])
AM_SILENT_RULES([yes])
AFS_MAINTAINER_MODE

## Initialize configuration summary
AFS_SUMMARY_INIT

## Set up Libtool
AM_PROG_AR
LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

## Set convenience variables for macros
ac_scorep_backend=no
ac_scorep_frontend=yes

## Platform configuration
AC_SCOREP_PLATFORM_SETTINGS
scalasca_platform_includes=""
AC_SUBST([PLATFORM_INCLUDES], ["${scalasca_platform_includes}"])
scalasca_build_scan="yes"
AS_CASE([${ac_scorep_platform}],
    [k],     [scalasca_build_scan="no"],
    [fx10],  [scalasca_build_scan="no"],
    [fx100], [scalasca_build_scan="no"])
AM_CONDITIONAL([BUILD_SCAN_TOOL], [test "x${scalasca_build_scan}" = "xyes"])
AM_CONDITIONAL([BUILD_SCALASCA_TOOL], [test 1 -eq 1])


##--- Package-specific configuration ----------------------------------------

## Checks for platform specifics
SCALASCA_DETECT_MIC_BUILD

## Checks for programs
AC_PROG_CC
AC_SCOREP_PROG_CC_C99
AC_C_BIGENDIAN
AC_PROG_CXX
AX_CXX_INTTYPE_MACROS
AC_LANG([C++])
AM_PROG_LEX
AC_PROG_YACC

## Handle auxiliary configure options
SCALASCA_DEBUG_OPTION

## Checks for headers/functions
AC_SCALASCA_GETOPT
AC_SUBST([LIBZ_CPPFLAGS], [])   ## EPILOG reader not used on frontend, i.e.,
AC_SUBST([LIBZ_LDFLAGS],  [])   ## zlib will not be used either
AC_SUBST([LIBZ_LIBS],     [])

## Configure common utils code
SCALASCA_COMMON_UTILS


##--- Generate output -------------------------------------------------------

## Frontend configuration summary
AFS_SUMMARY([C99 compiler], [$CC])
AFS_SUMMARY([C++ compiler], [$CXX])

## Generate config files
AC_CONFIG_HEADERS([../src/config-frontend.h])
AFS_SET_SILENT_RULE_PREFIX([FE])
AFS_CONFIG_MAKEFILE
AC_OUTPUT
