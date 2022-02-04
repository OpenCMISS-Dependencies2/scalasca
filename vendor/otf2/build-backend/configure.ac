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
dnl Copyright (c) 2009-2015, 2018,
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

AFS_PACKAGE_BUILD_INIT([backend])
AFS_SUMMARY_INIT

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../build-config/common/generate-library-version.sh ../build-config/VERSION]))
AC_DEFINE_UNQUOTED([LIBRARY_INTERFACE_VERSION], ["$LIBRARY_INTERFACE_VERSION"], [Interface version number])

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4])

AC_CONFIG_HEADERS([../src/config-backend.h])
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
    [AC_CONFIG_HEADERS([../src/config-backend-for-frontend.h])])

AC_DEFUN([AC_SCOREP_BACKEND], [])

## require $ac_scorep_cross_compiling to be set to 'yes' on 'no'
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
      [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
cross_compiling=${ac_scorep_cross_compiling}

AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"], [
    # suppress warnings about 'using cross tools not prefixed with host triplet'
    ac_tool_warned=yes])

ac_scorep_backend=yes
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"], [
    ac_scorep_frontend=no
], [
    ac_scorep_frontend=yes
])

AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
#AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])
#AC_SCOREP_CHECK_COMPILER_VAR_SET([F77])
#AC_SCOREP_CHECK_COMPILER_VAR_SET([FC])

AM_CONDITIONAL([BUILD_TESTS], [test "x${ac_scorep_cross_compiling}" = "xno"])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax nostdinc])
AM_SILENT_RULES([yes])
AM_PROG_AR

m4_include([../build-includes/common.m4])
AS_IF([test "x$ac_scorep_cross_compiling" = "xno"],
    [m4_include([../build-includes/frontend.m4])])

AFS_LINKMODE
AC_SCOREP_BACKEND_TEST_RUNS

otf2_running_tests_result=${ac_scorep_enable_backend_test_runs}
AS_IF([test "x$ac_scorep_cross_compiling" = "xyes"],
    [otf2_running_tests_result="no, on cross machine"])

AFS_SUMMARY_SECTION_BEGIN([Running tests], [${otf2_running_tests_result}])
AFS_SUMMARY([Parallel tests], [${otf2_pthread_barrier_support}])
AFS_SUMMARY_SECTION_END

# On crosscompile machines, install backend/mic stuff into
# $(bin|lib)dir/(backend|mic).
backend_suffix=""
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
        [AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                 [backend_suffix="/mic"],
             [backend_suffix="/backend"])
        ])
AC_SUBST([backend_suffix])

AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
    [AFS_SET_SILENT_RULE_PREFIX([BE])])
AFS_CONFIG_MAKEFILE

AC_OUTPUT
