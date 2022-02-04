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
AFS_PACKAGE_BUILD_INIT([backend])

## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4 ../vendor/gtest/m4])

## Set up Automake
AM_INIT_AUTOMAKE([1.13.4 -Wall foreign parallel-tests color-tests tar-pax])
AM_SILENT_RULES([yes])
AFS_MAINTAINER_MODE

## Initialize configuration summary
AFS_SUMMARY_INIT

## Set (non-)cross compilation mode
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
      [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
cross_compiling=${ac_scorep_cross_compiling}
## Suppress warning about 'using cross tools not prefixed with host triplet'
## when in cross-compile mode
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
    [ac_tool_warned=yes])

## Set up Libtool
AM_PROG_AR
LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])
AFS_LINKMODE

## Set convenience variables for macros
ac_scorep_backend=yes
AM_COND_IF([CROSS_BUILD],
    [ac_scorep_frontend=no],
    [ac_scorep_frontend=yes])

## On crosscompile machines, install backend/mic stuff into
## $(bin|lib)dir/(backend|mic).
backend_suffix=""
AM_COND_IF([CROSS_BUILD],
    [backend_suffix="/backend"
     AS_IF([test "x${ac_scorep_platform}" = "xmic"],
         [backend_suffix="/mic"])
    ])
AC_SUBST(backend_suffix)

## Platform configuration
AC_SCOREP_PLATFORM_SETTINGS
scalasca_platform_includes=""
AM_COND_IF([PLATFORM_BGQ],
           [scalasca_platform_includes="-I/bgsys/drivers/ppcfloor -I/bgsys/drivers/ppcfloor/spi/include/kernel/cnk"])
AC_SUBST([PLATFORM_INCLUDES], ["${scalasca_platform_includes}"])
AM_COND_IF([CROSS_BUILD],
    [scalasca_build_scan="no"
     AS_CASE([${ac_scorep_platform}],
         [k],     [scalasca_build_scan="yes"],
         [fx10],  [scalasca_build_scan="yes"],
         [fx100], [scalasca_build_scan="yes"],
         [mic],   [scalasca_build_scan="yes"])
    ],
    [scalasca_build_scan=yes])
AM_CONDITIONAL([BUILD_SCAN_TOOL], [test "x${scalasca_build_scan}" = "xyes"])
AM_CONDITIONAL([BUILD_SCALASCA_TOOL], [test "x${scalasca_build_scan}" = "xyes"])

## Configure internal/external libraries
AS_IF([test -n "${scorep_otf2_bindir}"],
      [scorep_otf2_config_backend_arg=
       AM_COND_IF([CROSS_BUILD],
           [scorep_otf2_config_backend_arg="--target=backend"
            AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                [scorep_otf2_config_backend_arg="--target=mic"])
           ])
       AC_SUBST([OTF2_CPPFLAGS], ["`${scorep_otf2_bindir}/otf2-config --cppflags ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LIBS],     ["`${scorep_otf2_bindir}/otf2-config --libs ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LDFLAGS],  ["`${scorep_otf2_bindir}/otf2-config --ldflags ${scorep_otf2_config_backend_arg}`"])
       AM_CONDITIONAL([HAVE_EXTERNAL_OTF2_LIBRARY], [test 1 -eq 1])],
      [AC_SUBST([OTF2_CPPFLAGS], ['-I../vendor/otf2/include -I$(srcdir)/../vendor/otf2/include'])
       AC_SUBST([OTF2_LIBS],     [])
       AC_SUBST([OTF2_LDFLAGS],  [])
       AM_CONDITIONAL([HAVE_EXTERNAL_OTF2_LIBRARY], [test 1 -eq 0])])
AS_IF([test -n "${scorep_cubew_bindir}"],
      [scorep_cubew_config_backend_arg=""
       AM_COND_IF([CROSS_BUILD],
           [scorep_cubew_config_backend_arg="--target=backend"
            AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                [scorep_cubew_config_backend_arg="--target=mic"])
           ])
       AC_SUBST([CUBE_WRITER_CPPFLAGS], ["`${scorep_cubew_bindir}/cubew-config --cflags ${scorep_cubew_config_backend_arg}`"])
       AC_SUBST([CUBE_WRITER_LIBS],     ["`${scorep_cubew_bindir}/cubew-config --libs ${scorep_cubew_config_backend_arg}`"])
       AC_SUBST([CUBE_WRITER_LDFLAGS],  ["`${scorep_cubew_bindir}/cubew-config --ldflags ${scorep_cubew_config_backend_arg}`"])
       AM_CONDITIONAL([HAVE_EXTERNAL_CUBE_WRITER], [test 1 -eq 1])
      ],
      [AC_SUBST([CUBE_WRITER_CPPFLAGS], ['-I$(srcdir)/../vendor/cubew/src/cubew -I../vendor/cubew/src '])
       AC_SUBST([CUBE_WRITER_LIBS],     [])
       AC_SUBST([CUBE_WRITER_LDFLAGS],  [])
       AM_CONDITIONAL([HAVE_EXTERNAL_CUBE_WRITER], [test 1 -eq 0])
      ])


##--- Package-specific configuration ----------------------------------------

## Consistency checks for compilers
AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])

## Checks for platform specifics
SCALASCA_DETECT_MIC_BUILD

## Checks for programs
AC_PROG_CC
AC_SCOREP_COMPILER_CHECKS
AC_SCOREP_PROG_CC_C99
AC_C_BIGENDIAN
AC_PROG_CXX
AX_CXX_INTTYPE_MACROS
AC_LANG([C++])
AM_COND_IF([CROSS_BUILD],
           [], # Handled by frontend configure in cross-compilation mode
           [AM_PROG_LEX
            AC_PROG_YACC])

## Check for OpenMP support
AC_OPENMP
SCALASCA_NOWARN_OMP_PRAGMAS
AM_CONDITIONAL([OPENMP_SUPPORTED],
               [test "x${ac_cv_prog_cxx_openmp}" != "xunsupported" \
                && test "x${enable_openmp}" != "xno"])
AS_IF([test "x${ac_cv_prog_cxx_openmp}" = "xunsupported"],
      [AC_MSG_WARN([no suitable OpenMP compilers found. Scalasca OpenMP support disabled.])])

## Handle auxiliary configure options
SCALASCA_DEBUG_OPTION
AC_SCOREP_BACKEND_TEST_RUNS

## Set PEARL library versioning information
AC_SUBST([PEARL_INTERFACE_VERSION],
         m4_esyscmd([../build-config/common/generate-library-version.sh ../build-config/VERSION]))

## Checks for headers/functions
AC_SCALASCA_GETOPT
AC_SCALASCA_ZLIB
AX_GTEST([../vendor/gtest])

## Configure common utils code
SCALASCA_COMMON_UTILS


##--- Generate output -------------------------------------------------------

## Backend configuration summary
AFS_SUMMARY([C99 compiler], [$CC])
AFS_SUMMARY([C++ compiler], [$CXX])
AS_IF([test "x${enable_openmp}" = "xno"],
      [AFS_SUMMARY([OpenMP support], [disabled])],
      [test "x${ac_cv_prog_cxx_openmp}" = "xunsupported"],
      [AFS_SUMMARY([OpenMP support], [no, no suitable compilers found])],
      [AFS_SUMMARY([OpenMP support], [yes, using ${ac_cv_prog_cxx_openmp}])])

## Generate config files
AC_CONFIG_HEADERS([../src/config-backend.h])
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [AFS_SET_SILENT_RULE_PREFIX([BE])])
AFS_CONFIG_MAKEFILE
AC_CONFIG_FILES([../src/pearl/example/Makefile.serial])
AM_COND_IF([OPENMP_SUPPORTED],
           [AC_CONFIG_FILES([../src/pearl/example/Makefile.openmp])])

AC_OUTPUT
