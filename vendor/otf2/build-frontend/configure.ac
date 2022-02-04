dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2014, 2018,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2013, 2015, 2018,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.69])
AC_INIT([OTF2], m4_esyscmd([../build-config/common/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [otf2])

AFS_PACKAGE_BUILD_INIT([frontend])
AFS_SUMMARY_INIT

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../build-config/common/generate-library-version.sh ../build-config/VERSION]))
AC_DEFINE_UNQUOTED([LIBRARY_INTERFACE_VERSION], ["$LIBRARY_INTERFACE_VERSION"], [Interface version number])

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4])

AC_CONFIG_HEADERS([../src/config-frontend.h])

AC_DEFUN([AC_SCOREP_FRONTEND], [])

ac_scorep_frontend="yes"
ac_scorep_backend="no"

AC_SCOREP_PRECIOUS_VARS_FOR_BUILD

AC_SCOREP_CHECK_COMPILER_VAR_SET([CC_FOR_BUILD])
AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX_FOR_BUILD])
#AC_SCOREP_CHECK_COMPILER_VAR_SET([F77_FOR_BUILD])
#AC_SCOREP_CHECK_COMPILER_VAR_SET([FC_FOR_BUILD])

AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CPPFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CXXFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([FFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([FCFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([LDFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([LIBS])

AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CC])
AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CXX])
AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([F77])
AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([FC])

AM_CONDITIONAL([BUILD_TESTS], [true])
AM_CONDITIONAL([CROSS_BUILD], [true])

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax nostdinc])
AM_SILENT_RULES([yes])
AM_PROG_AR

m4_include([../build-includes/common.m4])
m4_include([../build-includes/frontend.m4])

AFS_SUMMARY_SECTION_BEGIN([Running tests], [yes])
AFS_SUMMARY([Parallel tests], [${otf2_pthread_barrier_support}])
AFS_SUMMARY_SECTION_END

AFS_SET_SILENT_RULE_PREFIX([FE])
AFS_CONFIG_MAKEFILE

AC_OUTPUT
