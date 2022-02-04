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
AFS_PACKAGE_BUILD_INIT([MPI backend])

## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4 ../vendor/gtest/m4])

## Define MPI-specific environment variables
AC_ARG_VAR(MPICC, [MPI C compiler command])
AC_ARG_VAR(MPICXX, [MPI C++ compiler command])
AC_ARG_VAR(MPI_CPPFLAGS, [MPI (Objective) C/C++ preprocessor flags, e.g. -I<include dir> if you have headers in a nonstandard directory <include dir>])
AC_ARG_VAR(MPI_CFLAGS, [MPI C compiler flags])
AC_ARG_VAR(MPI_CXXFLAGS, [MPI C++ compiler flags])
AC_ARG_VAR(MPI_LDFLAGS, [MPI linker flags, e.g. -L<lib dir> if you have libraries in a nonstandard directory <lib dir>])
AC_ARG_VAR(MPI_LIBS, [MPI libraries to pass to the linker, e.g. -l<library>])

## Consistency checks for compilers
AC_SCOREP_CHECK_COMPILER_VAR_SET([MPICXX])

## Convert compilers & flags
AC_SCOREP_CONVERT_MPI_COMPILER([CC])
AC_SCOREP_CONVERT_MPI_COMPILER([CXX])
AC_SCOREP_CONVERT_MPI_FLAGS([CPPFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([CFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([CXXFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([LDFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([LIBS])

## Set up Automake
AM_INIT_AUTOMAKE([1.13.4 -Wall foreign parallel-tests color-tests tar-pax])
AM_SILENT_RULES([yes])
AFS_MAINTAINER_MODE

## Initialize configuration summary
AFS_SUMMARY_INIT([../build-backend])

## Set (non-)cross compilation mode
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
      [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
## Prevent execution of configure-generated MPI programs as this may not be
## possible
cross_compiling=yes
## Suppress warning about 'using cross tools not prefixed with host triplet'
ac_tool_warned=yes

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

## Configure internal/external libraries
AS_IF([test -n "${scorep_otf2_bindir}"],
      [scorep_otf2_config_backend_arg=""
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
       AC_SUBST([OTF2_LIBS],     [../vendor/otf2/build-backend/libotf2.la])
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
      [AC_SUBST([CUBE_WRITER_CPPFLAGS], ['-I$(srcdir)/../vendor/cubew/src/cubew  -I../vendor/cubew/src '])
       AC_SUBST([CUBE_WRITER_LIBS],     [])
       AC_SUBST([CUBE_WRITER_LDFLAGS],  [])
       AM_CONDITIONAL([HAVE_EXTERNAL_CUBE_WRITER], [test 1 -eq 0])
      ])


##--- Package-specific configuration ----------------------------------------

## Checks for platform specifics
SCALASCA_DETECT_MIC_BUILD

## Checks for programs
AC_PROG_CC
AC_SCOREP_COMPILER_CHECKS
AC_PROG_CXX
AX_CXX_INTTYPE_MACROS
AC_LANG([C++])
AM_PROG_LEX
AC_PROG_YACC

## Check for MPI support
AC_SCOREP_MPI_WORKING([scalasca_mpi_supported="yes"],
                      [scalasca_mpi_supported="no"])
AM_CONDITIONAL([MPI_SUPPORTED], [test "x${scalasca_mpi_supported}" = "xyes"])
AC_CHECK_DECLS([MPI_Exscan, MPI_Get_address, MPI_Init_thread, MPI_Put, MPI_Type_create_struct, MPI_Type_create_resized], [], [], [[#include <mpi.h>]])
AFS_MPI_DATATYPE_INT8_T
AFS_MPI_DATATYPE_UINT8_T
AFS_MPI_DATATYPE_INT16_T
AFS_MPI_DATATYPE_UINT16_T
AFS_MPI_DATATYPE_INT32_T
AFS_MPI_DATATYPE_UINT32_T
AFS_MPI_DATATYPE_INT64_T
AFS_MPI_DATATYPE_UINT64_T

## Check for OpenMP support
AC_OPENMP
SCALASCA_NOWARN_OMP_PRAGMAS
AM_CONDITIONAL([OPENMP_SUPPORTED],
               [test "x${ac_cv_prog_cxx_openmp}" != "xunsupported" \
                && test "x${enable_openmp}" != "xno"])
AS_IF([test "x${ac_cv_prog_cxx_openmp}" = "xunsupported"],
      [AC_MSG_WARN([no suitable OpenMP compilers found. Scalasca hybrid support disabled.])])

## Handle auxiliary configure options
SCALASCA_DEBUG_OPTION
AC_SCOREP_BACKEND_TEST_RUNS

## Set PEARL library versioning information
AC_SUBST([PEARL_INTERFACE_VERSION],
         m4_esyscmd([../build-config/common/generate-library-version.sh ../build-config/VERSION]))

## Checks for headers/functions
AX_GTEST([../vendor/gtest])

## Checks for types
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([uint32_t])

## Configure common utils code
SCALASCA_COMMON_UTILS


##--- Generate output -------------------------------------------------------

## MPI backend configuration summary
AFS_SUMMARY([MPI support], [$scalasca_mpi_supported])
AFS_SUMMARY([MPI C++ compiler], [$CXX])
AS_IF([test "x${enable_openmp}" = "xno"],
      [AFS_SUMMARY([OpenMP support], [disabled])],
      [test "x${ac_cv_prog_cxx_openmp}" = "xunsupported"],
      [AFS_SUMMARY([OpenMP support], [no, no suitable compilers found])],
      [AFS_SUMMARY([OpenMP support], [yes, using ${ac_cv_prog_cxx_openmp}])])

## Generate config files
AC_CONFIG_HEADERS([../src/config-backend-mpi.h])
AFS_SET_SILENT_RULE_PREFIX([MPI])
AFS_CONFIG_MAKEFILE
AC_CONFIG_FILES([../src/pearl/example/Makefile.mpi])
AM_COND_IF([OPENMP_SUPPORTED],
           [AC_CONFIG_FILES([../src/pearl/example/Makefile.hybrid])])

AC_OUTPUT
