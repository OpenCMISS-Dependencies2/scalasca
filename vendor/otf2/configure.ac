dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2014, 2019-2020,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2015, 2017-2018,
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
AC_INIT([OTF2], m4_esyscmd([build-config/common/generate-package-version.sh build-config/VERSION]), [support@score-p.org], [otf2])

AFS_PACKAGE_INIT
AFS_SUMMARY_INIT

AC_SCOREP_REVISION

AC_SCOREP_TOPLEVEL_ARGS

## Packages that do manual configuration or use the install program might need
## to tell configure where to find some other shell scripts by calling
## AC_CONFIG_AUX_DIR, though the default places it looks are correct for most
## cases.
## Need to be called early on configure.ac because other macros search tools
## in this directory.
AC_CONFIG_AUX_DIR([build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([build-config/m4 build-config/common/m4])

## Set the default installation prefix to /opt/otf2 instead of /usr/local
AC_PREFIX_DEFAULT([/opt/otf2])

AC_SCOREP_GIT_CONTROLLED

## Call not before AC_CONFIG_MACRO_DIR
AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax nostdinc])
AM_SILENT_RULES([yes])
AFS_MAINTAINER_MODE

AC_SCOREP_DETECT_PLATFORMS
AC_SCOREP_PLATFORM_SETTINGS
AFS_CROSSCOMPILING

# don't detect and load defaults in nested configures (e.g. otf2)
AS_IF([test "x$ac_scorep_platform_data_provided" = "x" ||
       test "${ac_scorep_platform_data_provided}" = "no"],
    [AFS_CUSTOM_COMPILERS
     AS_IF([test "x${afs_custom_compilers_given}" = "xno"],
        [AC_SCOREP_WITH_COMPILER_SUITE])
     # determine arguments for subdir configures
     args="`$AWK -f $srcdir/build-config/common/process_arguments.awk \
            $ac_scorep_compilers_frontend                                    \
            $ac_scorep_compilers_backend                                     \
            user_provided_configure_args`"
     AS_IF([test $? -ne 0], [AC_MSG_ERROR([cannot process provided and/or autodetected arguments. Please contact <AC_PACKAGE_BUGREPORT> and provide the above output. Thanks.])])
    ],
    [AC_MSG_NOTICE([platform data provided by toplevel configure.])
     # determine arguments for subdir configures
     args="`$AWK -f $srcdir/build-config/common/process_arguments.awk \
            user_provided_configure_args`"
     AS_IF([test $? -ne 0], [AC_MSG_ERROR([cannot process provided arguments. Please contact <AC_PACKAGE_BUGREPORT> and provide the above output. Thanks.])])
    ])

AC_SCOREP_UNCRUSTIFY

AC_SCOREP_DOXYGEN
AS_IF([test "x${ac_scorep_git_controlled}" = xyes],
    [AC_CONFIG_FILES([doc/doxygen-user.cfg:doc/doxygen-user/doxygen-user.cfg.in])])

AFS_DEBUG_OPTION

## Autoconf supports changing the names of programs when installing them. In
## order to use these transformations, configure.ac must call the macro
## AC_ARG_PROGRAM.
# implicitly called by ?
#AC_ARG_PROGRAM

# used for EXTRA_DIST in Makefile.am
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])

# handle own package's subdir configures
AX_CONFIG_SUBDIR([build-backend], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling}])

if test "x${ac_scorep_cross_compiling}" = "xyes"; then
    AX_CONFIG_SUBDIR([build-frontend], [${args} ac_scorep_platform=${ac_scorep_platform}])
fi

AC_ARG_VAR([PYTHON],
           [The Python interpreter to be used for the Python bindings. Use PYTHON=: to disable Python support.])

AC_ARG_VAR([PYTHON_FOR_GENERATOR],
           [The Python interpreter used for the generator. Not a build requirement, only needed for developing. Python version 2.7 or above and Jinja2 +2.10 needed. Overwrites PYTHON. Use PYTHON_FOR_GENERATOR=: to disable support.])
AS_IF([test "${ac_env_PYTHON_FOR_GENERATOR_set}" = set],
    [PYTHON=$PYTHON_FOR_GENERATOR])
m4_define_default([_AM_PYTHON_INTERPRETER_LIST],
                  [python3 python3.8 python3.7 python3.6 python3.5 python3.4 python3.3 python3.2 python3.1 python3.0 dnl
                   python dnl
                   python2 python2.7])

otf2_have_python_with_jinja2=no
otf2_python_with_jinja2_reason=", missing python"
AM_PATH_PYTHON([2.7], [
    otf2_python_with_jinja2_reason=", tried $PYTHON but no 'jinja2' module"
    AX_PYTHON_MODULE([jinja2])
    AS_IF([test "x${HAVE_PYMOD_JINJA2}" = "xyes"], [
        AS_CASE(["${HAVE_PYMOD_VERSION_JINJA2}"],
        [[*-otf2]], [
            : all otf2 versions should work
            otf2_have_python_with_jinja2=yes
            otf2_python_with_jinja2_reason=", using $PYTHON with 'jinja2' module in version $HAVE_PYMOD_VERSION_JINJA2"
        ],
        [[2.[0123456789]|2.[0123456789].*]], [
            : unsupported versions
            otf2_python_with_jinja2_reason=", tried $PTHON but version requirement of 'jinja2' module failed: ${HAVE_PYMOD_VERSION_JINJA2} < 2.10"
        ], [
            : supported version +2.10
            otf2_have_python_with_jinja2=yes
            otf2_python_with_jinja2_reason=", using $PYTHON with 'jinja2' module in version $HAVE_PYMOD_VERSION_JINJA2"
        ])
    ])
], [:])
AM_CONDITIONAL([HAVE_PYTHON], [test "x${PYTHON}" != "x:"])
AM_CONDITIONAL([HAVE_PYTHON_FOR_GENERATOR], [test "x${otf2_have_python_with_jinja2}" = "xyes"])
AC_SUBST([PYTHON_FOR_GENERATOR], [$PYTHON])
AM_SUBST_NOTMAKE([PYTHON])dnl we manually set PYTHON to $(PYTHON_FOR_GENERATOR)
AFS_SUMMARY_VERBOSE([Python for generator], [${otf2_have_python_with_jinja2}${otf2_python_with_jinja2_reason}])

# reset PYTHON fo subdir configure
AS_IF([test "${ac_env_PYTHON_set}" = set],
    [PYTHON="${ac_env_PYTHON_value}"; export PYTHON],
    [AS_UNSET([PYTHON])])

AC_ARG_VAR([SPHINX],
           [The spinx-build executable to be used to generate the Python documentation. Use SPHINX=: to disable Sphinx documentation.])

AC_CHECK_PROG([SPHINX], [sphinx-build], [`which sphinx-build`], [:])
AFS_AM_CONDITIONAL([HAVE_PYTHON_SPHINX],
                   [test "x$SPHINX" != "x:"],
                   [false])

AC_CONFIG_FILES([
    Makefile
    src/config-common.h:build-config/common/config-common.h
    share/otf2/__version__.py
    doc/doxygen-user/main.dox
])
AM_COND_IF([HAVE_PYTHON_FOR_GENERATOR], [
    AC_CONFIG_FILES(
        [src/tools/otf2_template/otf2-template:src/tools/otf2_template/otf2_template.py],
        [chmod +x src/tools/otf2_template/otf2-template])
])

# generate the OTF2 error codes header
AC_CONFIG_FILES(
    [include/otf2/OTF2_ErrorCodes.h:common/utils/src/exception/ErrorCodes.tmpl.h],
    [$srcdir/common/utils/src/exception/finalize_error_codes.sh OTF2 $srcdir/share/otf2/otf2.errors include/otf2/OTF2_ErrorCodes.h src/otf2_error_decls.gen.h])

# generate mini-config
adl_RECURSIVE_EVAL([${libexecdir}/${PACKAGE}], [PKGLIBEXECDIR])
AC_SUBST([pkglibexecdir], [${PKGLIBEXECDIR}])
AC_CONFIG_FILES(
    [otf2-mini-config.sh:otf2-mini-config.sh.in],
    [chmod +x otf2-mini-config.sh])

AC_OUTPUT

AFS_SUMMARY_COLLECT([test "x$ac_scorep_platform_data_provided" != "xyes"])
