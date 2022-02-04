##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2018                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  Copyright (c) 2009-2015                                                ##
##  German Research School for Simulation Sciences GmbH,                   ##
##  Laboratory for Parallel Programming                                    ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


AC_PREREQ([2.69])
AC_INIT([CubeW], m4_esyscmd([../build-config/common/generate-package-version.sh ../build-config/VERSION]), [scalasca@fz-juelich.de], [cubew],[http://www.scalasca.org])

## Initialize configuration summary
AFS_PACKAGE_BUILD_INIT([backend])
AFS_SUMMARY_INIT

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../build-config/common/generate-library-version.sh ../build-config/VERSION]))
AC_DEFINE_UNQUOTED([LIBRARY_INTERFACE_VERSION], ["$LIBRARY_INTERFACE_VERSION"], [Interface version number])
adl_RECURSIVE_EVAL([${libexecdir}/${PACKAGE}], [PKGLIBEXECDIR])
AC_DEFINE_UNQUOTED([PKGLIBEXECDIR],
                   ["${PKGLIBEXECDIR}"],
                   [Directory where ]AC_PACKAGE_NAME[ executables to be run by other programs rather than by users are installed])
adl_RECURSIVE_EVAL([${includedir}], [INCLUDEDIR])
AC_SUBST([INCLUDEDIR])
adl_RECURSIVE_EVAL([${bindir}], [BINDIR])
AC_SUBST([BINDIR])
adl_RECURSIVE_EVAL([${libdir}/${PACKAGE}], [PKGLIBDIR])
AC_SUBST([PKGLIBDIR])
adl_RECURSIVE_EVAL([${libdir}${backend_suffix}/${PACKAGE}], [BACKEND_PKGLIBDIR])
AC_SUBST([BACKEND_PKGLIBDIR])
adl_RECURSIVE_EVAL([${datadir}/${PACKAGE}], [PKGDATADIR])
AC_DEFINE_UNQUOTED([PKGDATADIR],
                   ["${PKGDATADIR}"],
                   [Directory where ]AC_PACKAGE_NAME[ data is stored])
AC_SUBST([PKGDATADIR])
adl_RECURSIVE_EVAL([${docdir}], [DOCDIR])
AC_SUBST([DOCDIR])




## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4])


## Set up Automake
AC_SCOREP_PLATFORM_SETTINGS
AC_CUBEW_DETECT_MIC_BUILD

## Set (non-)cross compilation mode
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
      [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
cross_compiling=${ac_scorep_cross_compiling}
CROSS_BUILD=$cross_compiling
AC_SUBST([CROSS_BUILD])
# On crosscompile machines, know where to look for backend stuff.
#On crosscompile machines, install backend/ /mic stuff into 
# $(bin|lib)dir/(backend|mic). 
backend_suffix=""
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                  [backend_suffix="/mic"],
              [backend_suffix="/backend"])
])
AC_SUBST([backend_suffix])


## Set convenience variables for macros
ac_scorep_backend=yes
AM_COND_IF([CROSS_BUILD],
           [ac_scorep_frontend=no],
           [ac_scorep_frontend=yes])


AC_SCOREP_CONSTUCT_LIST_OF_LIB_DIRS
AC_SCOREP_CUBE_BACKEND_SELECTION

## Platform configuration


##--- Package-specific configuration ----------------------------------------
AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR

## Set up Libtool
LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])
AFS_LINKMODE



AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])
AC_PROG_CXX
AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
## Checks for programs
AC_PROG_CC
AC_SCOREP_PROG_CC_C99
AC_C_BIGENDIAN
AM_PROG_CC_C_O





## trunc and ceil needed by the cube4 writer
AC_LANG_PUSH([C])
AC_SEARCH_LIBS([trunc], [m])
AC_SEARCH_LIBS([ceil], [m])
AC_LANG_POP([C])
AC_CUBE_MEMORY

#AC_LANG([C++])
AC_SCOREP_COMPILER_CHECKS
AC_CUBE_POSIX_FUNCTIONS
AM_CONDITIONAL([CUBEW_COMPRESSION_FRONTEND], [test "x$MSG_FRONTEND_COMPRESSION" = "xyes"])
AM_CONDITIONAL([CUBEW_COMPRESSION_BACKEND], [test "x$MSG_BACKEND_COMPRESSION" = "xyes"])
AC_CUBE_LIBZ_BACKEND_SELECTION


# On crosscompile machines, install backend/mic stuff into
# $(bin|lib)dir/(backend|mic).
backend_suffix=""
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
        [AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                 [backend_suffix="/mic"],
             [backend_suffix="/backend"])
        ])
AC_SUBST([backend_suffix])


AC_SCOREP_CONFIG_SELECTION_REPORT([backend])


#check for examples
AC_CUBE_EXAMPLES


AC_CUBEW_ERROR_CODES
AX_CXX_INTTYPE_MACROS

## Generate config files
AC_CONFIG_HEADERS([../src/cubew-config-backend.h])



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

AC_SUBST([SYS_LIB_DLSEARCH_PATH_SPEC], [${sys_lib_dlsearch_path_spec}])


AC_CONFIG_FILES([../src/tools/cubew_config/cubew_config_data_]AFS_PACKAGE_BUILD_name[.cpp:../src/tools/cubew_config/cubew_config_data_build.cpp.in])

AC_CONFIG_FILES([../examples/Makefile])
AC_CONFIG_FILES([../examples/Makefile.backend])

if test "x${ac_scorep_cross_compiling}" != "xyes"; then
AC_CONFIG_FILES([../examples/Makefile.frontend])
fi



CC="$CUBE_SCOREP $CC"
CXX="$CUBE_SCOREP $CXX"



AC_CONFIG_FILES([Makefile])

if test "x$CROSS_BUILD" != "xyes"; then

AC_CONFIG_FILES([../test/test3/cube_self_test.c
                 ], \
                [
                 chmod +x ../test/test3/cube_self_test.c
                ])
AC_CONFIG_FILES([../test/test3/cube_self_test3_4.c
                 ], \
                [
                 chmod +x ../test/test3/cube_self_test3_4.c
                ])
AC_CONFIG_FILES([../test/test3/run_test_3.sh
                 ], \
                [
                 chmod +x ../test/test3/run_test_3.sh
                ])


fi



AC_OUTPUT
