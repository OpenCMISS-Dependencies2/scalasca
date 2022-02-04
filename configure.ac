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
        m4_esyscmd([build-config/common/generate-package-version.sh build-config/VERSION]),
        [scalasca@fz-juelich.de],,[https://www.scalasca.org])
AFS_PACKAGE_INIT

## Save user provided arguments for use by sub-configures
SCALASCA_TOPLEVEL_ARGS

## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([build-config/m4 build-config/common/m4])

## Set the default installation prefix
AC_PREFIX_DEFAULT([/opt/scalasca])

## Set up Automake
AM_INIT_AUTOMAKE([1.13.4 -Wall foreign tar-pax])
AM_SILENT_RULES([yes])
AFS_MAINTAINER_MODE

## Initialize configuration summary
AFS_SUMMARY_INIT


##--- Package-specific configuration ----------------------------------------

## Working from git? Get revision info
AC_SCOREP_GIT_CONTROLLED
AC_SCOREP_REVISION

## Platform detection
AC_SCOREP_DETECT_PLATFORMS
AC_SCOREP_PLATFORM_SETTINGS
AFS_CROSSCOMPILING

## Don't detect and load defaults in nested configures
AS_IF([test ! -n "$ac_scorep_platform_data_provided" || \
       test "x${ac_scorep_platform_data_provided}" = "xno"],
      [AFS_CUSTOM_COMPILERS
       AS_IF([test "x${afs_custom_compilers_given}" = "xno"],
             [dnl AFS_COMPILER_BACKEND([CC, CXX, F77, FC])
              AC_SCOREP_WITH_COMPILER_SUITE
              AS_CASE([${afs_custom_compilers_given},${ac_scorep_platform}],
                  [no,mac|no,mingw],
                      [AC_MSG_ERROR([have no suitable defaults to build Scalasca on $afs_scorep_platform_name yet. Please use --with-custom-compilers and provide settings to <AC_PACKAGE_BUGREPORT> on success. Thanks.])],
                  [no,unknown],
                      [AC_MSG_ERROR([cannot detect platform to set suitable defaults. Please use --with-custom-compilers and provide settings and platform to <AC_PACKAGE_BUGREPORT> on success. Thanks.])
                  ])
              AC_SCOREP_WITH_MPI_COMPILER_SUITE
             ])
       # Determine arguments for subdir configures
       args="`$AWK -f $srcdir/build-config/common/process_arguments.awk \
              $ac_scorep_compilers_mpi                                         \
              $ac_scorep_compilers_frontend                                    \
              $ac_scorep_compilers_backend                                     \
              user_provided_configure_args`"
      ],
      [AC_MSG_NOTICE([platform data provided by toplevel configure.])
       AFS_SUMMARY([Platform], [$ac_scorep_platform (provided)])
       # Determine arguments for subdir configures
       args="`$AWK -f $srcdir/build-config/common/process_arguments.awk \
              user_provided_configure_args`"
      ])

## Check for cross-compilation
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])

## Checks for platform specifics
SCALASCA_DETECT_MIC_BUILD
AM_CONDITIONAL([BUILD_MIC_WRAPPER],
    [test "x${ac_scorep_platform}" = "xmic" || \
     test "x${scalasca_have_mic_support}" = "xyes"])

## Check for programs
AC_PROG_LN_S
AC_PROG_SED
AC_SCOREP_DOXYGEN
AC_SCOREP_UNCRUSTIFY
SCALASCA_CORPORATE_STY

## Check for libraries
AC_SCOREP_HAVE_CONFIG_TOOL([otf2], [8], [0])
AS_IF([test -n "${scorep_otf2_config_bin}" && \
       test "x${ac_scorep_platform}" = "xmic"],
      [scorep_otf2_have_mic=no
       for target in `${scorep_otf2_config_bin} --features=targets`
       do
           AS_CASE([${target}],
               [mic], [scorep_otf2_have_mic=yes])
       done
       AS_IF([test "x${scorep_otf2_have_mic}" = "xno"],
             [AC_MSG_ERROR([cannot detect MIC support for external OTF2 installation. Either provide a suitable OTF2 library or reconfigure ]AC_PACKAGE_NAME[ with the '--without-otf2' option to use the included OTF2 copy.])])
      ])
AFS_SUMMARY([epilog support], [yes, using internal])
AC_SCOREP_HAVE_CONFIG_TOOL([cubew], [8], [0])
AS_IF([test -n "${scorep_cubew_config_bin}" && \
       test "x${ac_scorep_platform}" = "xmic"],
      [scorep_cubew_have_mic=no
       for target in `${scorep_cubew_config_bin} --features=targets`
       do
           AS_CASE([${target}],
               [mic], [scorep_cubew_have_mic=yes])
       done
       AS_IF([test "x${scorep_cubew_have_mic}" = "xno"],
             [AC_MSG_ERROR([cannot detect MIC support for external Cube installation. Either provide a suitable Cube library or reconfigure ]AC_PACKAGE_NAME[ with the '--without-cubew' option to use the included Cube C writer copy.])])
      ])

## Handle external package's subdir configures
PROHIBIT_MAKE_DIST=
AS_IF([test "x${scorep_have_otf2_config}" = "xyes"],
          [AS_VAR_APPEND([PROHIBIT_MAKE_DIST], [" --without-otf2"])],
      [test -x ${srcdir}/vendor/otf2/configure],
          [AX_CONFIG_SUBDIR([vendor/otf2], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_platform_data_provided=yes ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/otf2])],
      [AC_MSG_ERROR([No OTF2 component available. Either provide the internal package at 'vendor/otf2' or an external installation with '--with-otf2'.])])
AS_IF([test "x${scorep_have_cubew_config}" = "xyes"],
          [AS_VAR_APPEND([PROHIBIT_MAKE_DIST], [" --without-cubew"])],
      [test -x ${srcdir}/vendor/cubew/configure],
          [AX_CONFIG_SUBDIR([vendor/cubew], [${args} afs_externally_triggered=yes ac_scorep_doxygen_distdir=${ac_scorep_doxygen_distdir}/vendor/cubew])],
      [AC_MSG_ERROR([No CubeW component available. Either provide the internal package at 'vendor/cubew' or an external installation with '--with-cubew'.])])
AC_SUBST([PROHIBIT_MAKE_DIST])

## Handle own package's subdir configures
AX_CONFIG_SUBDIR([build-backend],
                 [${args} ${backend_args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ${scorep_otf2_config_arg} ${scorep_cubew_config_arg}])
AS_IF([test "x${with_mpi}" != "xno"],
      [AX_CONFIG_SUBDIR([build-mpi],
                        [${args} ${backend_args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ${scorep_otf2_config_arg} ${scorep_cubew_config_arg}])])
AM_COND_IF([CROSS_BUILD],
           [AX_CONFIG_SUBDIR([build-frontend],
                             [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling}])])


##--- Generate output -------------------------------------------------------

## Additional configuration summary output
AS_IF([test "x${with_mpi}" = "xno"],
      [AFS_SUMMARY([MPI support], [disabled])])

## Generate config files
AC_CONFIG_FILES([Makefile
                 src/config-custom.h
                 src/config-common.h:build-config/common/config-common.h
                 doc/patterns/remap_info.html
                 share/scout.spec
                 share/scout_pre44.spec])
AM_COND_IF([HAVE_DOXYGEN],
           [AC_CONFIG_FILES([doc/doxygen-common/config.tex
                             doc/doxygen-common/doxygen-common.cfg
                             doc/doxygen-common/doxygen-pearl.cfg
                             doc/doxygen-common/doxygen-scout.cfg
                             doc/doxygen-common/doxygen-silas.cfg
                             doc/doxygen-common/doxygen-utils.cfg
                             doc/doxygen-dev/pearl/doxygen-pearl-dev.cfg
                             doc/doxygen-dev/scout/doxygen-scout-dev.cfg
                             doc/doxygen-dev/silas/doxygen-silas-dev.cfg
                             doc/doxygen-dev/utils/doxygen-utils-dev.cfg
                             doc/doxygen-user/manual/doxygen-manual.cfg
                             doc/doxygen-user/manpages/doxygen-man.cfg
                             doc/doxygen-user/pearl/doxygen-pearl-user.cfg])])

## Generate Scalasca error codes headers
AC_CONFIG_FILES(
    [src/ScalascaErrorCodes.h:common/utils/src/exception/ErrorCodes.tmpl.h],
    [$srcdir/common/utils/src/exception/finalize_error_codes.sh \
        SCALASCA \
        $srcdir/share/scalasca.errors \
        src/ScalascaErrorCodes.h \
        src/scalasca_error_decls.gen.h])

AM_EXTRA_RECURSIVE_TARGETS([beautify check-build])
AC_OUTPUT

## Print configuration summary
AS_IF([test "x$ac_scorep_platform_data_provided" != "xyes"], [
    AFS_SUMMARY_COLLECT
])
