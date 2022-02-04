## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2014, 2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2014,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2011,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/otf2_sionlib.m4

# output of sionconfig on jugene:
# sionconfig --be --ser --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -D_SION_BGP
# sionconfig --be --ser --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsionser_32
# sionconfig --be --ser --path
# /usr/local/sionlib/v1.2p2
# sionconfig --be --mpi --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -DSION_MPI -D_SION_BGP
# sionconfig --be --mpi --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsion_32 -lsionser_32
# sionconfig --be --mpi --path
# /usr/local/sionlib/v1.2p2
# sionconfig --fe --ser --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -D_SION_BGP
# sionconfig --fe --ser --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsionserfe_32
# sionconfig --fe --ser --path
# /usr/local/sionlib/v1.2p2
# sionconfig --fe --mpi --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -DSION_MPI -D_SION_BGP
# sionconfig --fe --mpi --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsionfe_32 -lsionserfe_32
# sionconfig --fe --mpi --path
# /usr/local/sionlib/v1.2p2


# OTF2_SIONLIB
AC_DEFUN([OTF2_SIONLIB],
[
dnl <begin> Adaption from AC_SCOREP_HAVE_CONFIG_TOOL
AC_ARG_WITH([sionlib],
    [AS_HELP_STRING([--with-sionlib[[=<sionlib-bindir>]]],
        [Use an already installed sionlib. Provide path to sionconfig. Auto-detected if already in $PATH.])],
    [with_sionlib="${with_sionlib%/}"], dnl yes, no, or <path>
    [with_sionlib="not_given"])

AS_UNSET([otf2_have_sionconfig])
AS_UNSET([otf2_sionlib_config_bin])
AS_UNSET([otf2_sionlib_config_arg])
AS_UNSET([otf2_sionlib_reason])
AS_IF([test "x${with_sionlib}" != "xno"],
    [AS_IF([test "x${with_sionlib}" = "xyes" || test "x${with_sionlib}" = "xnot_given"],
        [AC_CHECK_PROG([otf2_have_sionconfig], [sionconfig], ["yes"], ["no"])
         AS_IF([test "x${otf2_have_sionconfig}" = "xyes"],
             [otf2_sionlib_config_bin="`which sionconfig`"])],
        [# --with-sionlib=<path>
         AC_CHECK_PROG([otf2_have_sionconfig], [sionconfig], ["yes"], ["no"], [${with_sionlib}])
         AS_IF([test "x${otf2_have_sionconfig}" = "xyes"],
             [otf2_sionlib_config_bin="${with_sionlib}/sionconfig"],
             [AS_UNSET([ac_cv_prog_otf2_have_sionconfig])
              AS_UNSET([otf2_have_sionconfig])
              AC_CHECK_PROG([otf2_have_sionconfig], [sionconfig], ["yes"], ["no"], ["${with_sionlib}/bin"])
              AS_IF([test "x${otf2_have_sionconfig}" = "xyes"],
                  [otf2_sionlib_config_bin="${with_sionlib}/bin/sionconfig"])])
        ])
     AS_IF([test "x${otf2_have_sionconfig}" = "xyes"],
         [:
dnl           Version checking via 'sionconfig --interface-version' not available yet.
dnl           otf2_sionlib_config_arg="otf2_sionlib_bindir=`dirname ${otf2_sionlib_config_bin}`"
dnl           # version checking, see http://www.gnu.org/software/libtool/manual/libtool.html#Versioning
dnl           interface_version=`${otf2_sionlib_config_bin} --interface-version 2> /dev/null`
dnl           AS_IF([test $? -eq 0 && test "x${interface_version}" != "x"],
dnl               [# get 'current'
dnl                sion_max_provided_interface_version=`echo ${interface_version} | awk -F ":" '{print $[]1}'`
dnl                # get 'age'
dnl                sion_provided_age=`echo ${interface_version} | awk -F ":" '{print $[]3}'`
dnl               AS_IF([test ${sion_max_provided_interface_version} -eq 0 && test ${sion_provided_age} -eq 0],
dnl                   [# by convention, master is 0:0:0
dnl                    AC_MSG_WARN([external sionlib built from master, version checks disabled, might produce compile and link errors.])
dnl                    AFS_SUMMARY([sionlib support], [yes, using external via ${otf2_sionlib_config_bin} (built from master, version checks disabled, might produce compile and link errors.)])],
dnl                   [# calc 'current - age'
dnl                    AS_VAR_ARITH([sion_min_provided_interface_version], [${sion_max_provided_interface_version} - ${sion_provided_age}])
dnl                    # this is the version check:
dnl                    AS_IF([test ${sion_max_provided_interface_version} -ge $2 && \
dnl                           test $2 -ge ${sion_min_provided_interface_version}],
dnl                        [AFS_SUMMARY([sionlib support], [yes, using external via ${otf2_sionlib_config_bin}])],
dnl                        [AS_IF([test ${sion_provided_age} -eq 0],
dnl                            [AC_MSG_ERROR([provided interface version '${sion_max_provided_interface_version}' of sionlib not sufficient for AC_PACKAGE_NAME, provide '$2' or compatible.])],
dnl                            [AC_MSG_ERROR([provided interface versions [[${sion_min_provided_interface_version},${sion_max_provided_interface_version}]] of sionlib not sufficient for AC_PACKAGE_NAME, provide '$2' or compatible.])
dnl ])])])
dnl               ],
dnl               [AC_MSG_ERROR([required option --interface-version not supported by sionconfig.])])
         ],
         [# otf2_have_sionconfig = no
          AS_IF([test "x${with_sionlib}" = "xnot_given"],
              [:
dnl                There is no internal sionlib yet.
dnl                AFS_SUMMARY([sionlib support], [yes, using internal])
              ],
              [test "x${with_sionlib}" = "xyes"],
              [AC_MSG_ERROR([cannot detect sionconfig although it was requested via --with-sionlib.])],
              [AC_MSG_ERROR([cannot detect sionconfig in ${with_sionlib} and ${with_sionlib}/bin.])
              ])
         ])
    ],
    [# --without-sionlib
     :
     otf2_have_sionconfig="no"
     otf2_sionlib_reason=", explicitly disabled via --without-sionlib"
     dnl There is no internal sionlib yet.
     dnl AS_IF([test ! -d ${srcdir}/vendor/$1],
     dnl     [AC_MSG_ERROR([sionlib is required. Opting out an external sionlib via --without-sionlib is only an option if an internal sionlib is available, which isn't the case here. Please provide an external sionlib.])])
     dnl otf2_have_sionconfig="no"
     dnl AFS_SUMMARY([sionlib support], [yes, using internal])
    ]
)
dnl <end> Adaption from AC_SCOREP_HAVE_CONFIG_TOOL

# macro-internal variables
otf2_sionlib_cppflags=""
otf2_sionlib_ldflags=""
otf2_sionlib_rpathflags=""
otf2_sionlib_libs=""
otf2_have_sionlib="no"
otf2_sionlib_reason=", missing sionconfig"

    AS_IF([test "x${otf2_have_sionconfig}" = "xyes"], [
        AC_LANG_PUSH([C])
        cppflags_save=$CPPFLAGS
        ldflags_save=$LDFLAGS
        libs_save=$LIBS

        otf2_have_sionlib="yes"

        sionconfig_febe_flag=""
        # sionconfig flags are different when built for MIC...
        AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                [AS_IF([test "x${ac_scorep_backend}" = "xyes"],
                         [sionconfig_febe_flag="--mic"],
                     [test "x${ac_scorep_frontend}" = "xyes"],
                         [sionconfig_febe_flag="--be"])
                ],
            [AS_IF([test "x${ac_scorep_backend}" = "xyes"],
                     [sionconfig_febe_flag="--be"],
                 [test "x${ac_scorep_frontend}" = "xyes"],
                     [sionconfig_febe_flag="--fe"])
            ])

        AS_CASE([${build_cpu},${ac_scorep_platform}],
                [i?86,*],
                    [sionconfig_architecture_flags="--32"],
                [x86_64,*|ia64,*|*,bgq],
                    [sionconfig_architecture_flags="--64"],
                [sionconfig_architecture_flags=""])

        sionconfig_paradigm_flag="--gen --threadsafe"

        otf2_sionlib_cppflags=`${otf2_sionlib_config_bin} ${sionconfig_febe_flag} ${sionconfig_paradigm_flag} --cflags`
        CPPFLAGS="$otf2_sionlib_cppflags $CPPFLAGS"
        AC_CHECK_HEADER([sion.h], [], [
            otf2_have_sionlib="no"
            otf2_sionlib_cppflags=""
            otf2_sionlib_reason=", missing sion.h header"])

        AS_IF([test "x${otf2_have_sionlib}" = "xyes"], [
            otf2_sionlib_ldflags=`${otf2_sionlib_config_bin} ${sionconfig_febe_flag} ${sionconfig_paradigm_flag} ${sionconfig_architecture_flags} --libs | \
                                  awk '{for (i=1; i<=NF; i++) {if ([index]($i, "-L") == 1){ldflags = ldflags " " $i}}}END{print ldflags}'`

            otf2_sionlib_rpathflags=`${otf2_sionlib_config_bin} ${sionconfig_febe_flag} ${sionconfig_paradigm_flag} ${sionconfig_architecture_flags} --libs | \
                                     awk '{for (i=1; i<=NF; i++) {if ([index]($i, "-L") == 1){sub(/^-L/, "", $i); rpathflags = rpathflags " -R" $i}}}END{print rpathflags}'`

            otf2_sionlib_libs=`${otf2_sionlib_config_bin} ${sionconfig_febe_flag} ${sionconfig_paradigm_flag} ${sionconfig_architecture_flags} --libs | \
                               awk '{for (i=1; i<=NF; i++) {if ([index]($i, "-l") == 1){libs = libs " " $i}}}END{print libs}'`

            LDFLAGS="$otf2_sionlib_ldflags $LDFLAGS"
            LIBS="$otf2_sionlib_libs $LIBS"

            otf2_sionlib_reason=", using ${otf2_sionlib_config_bin} ${sionconfig_febe_flag} ${sionconfig_paradigm_flag} ${sionconfig_architecture_flags}"

            # SIONlib 2.0 is not supported
            AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <sion.h>
#include <stddef.h>
]],[[
#if SION_MAIN_VERSION >= 2
choke me
#endif
]])],
                           [AC_MSG_RESULT([yes])],
                           [AC_MSG_RESULT([no])
                            otf2_have_sionlib="no"
                            otf2_sionlib_ldflags=""
                            otf2_sionlib_rpathflags=""
                            otf2_sionlib_libs=""
                            otf2_sionlib_reason=", SIONlib 2.0 not supported"])


            # commom libsion checks. for the paradigm specific ones, see below.
            AS_IF([test "x${otf2_have_sionlib}" = "xyes"], [
                AC_MSG_CHECKING([for SIONlib basic API])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <sion.h>
#include <stddef.h>
]],[[
sion_seek(42,42,42,42);
sion_fwrite(NULL,42,42,42);
sion_fread(NULL,42,42,42);
sion_get_current_location(42,NULL,NULL,NULL,NULL);
]])],
                               [AC_MSG_RESULT([yes])],
                               [AC_MSG_RESULT([no])
                                otf2_have_sionlib="no"
                                otf2_sionlib_ldflags=""
                                otf2_sionlib_rpathflags=""
                                otf2_sionlib_libs=""
                                otf2_sionlib_reason=", basic API failed"])])


            # paradigm specific libsion checks
            AS_IF([test "x${otf2_have_sionlib}" = "xyes"], [
                AC_MSG_CHECKING([for SIONlib generic API])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <sion.h>
#include <stddef.h>
]],[[
/* generic sion functions */
sion_generic_create_api("");
sion_generic_register_create_local_commgroup_cb(42,NULL);
sion_generic_register_free_local_commgroup_cb(42,NULL);
sion_generic_register_barrier_cb(42,NULL);
sion_generic_register_bcastr_cb(42,NULL);
sion_generic_register_gatherr_cb(42,NULL);
sion_generic_register_gathervr_cb(42,NULL);
sion_generic_register_scatterr_cb(42,NULL);
sion_generic_register_scattervr_cb(42,NULL);
sion_generic_register_get_multi_filename_cb(42,NULL);
sion_generic_free_api(42);
sion_generic_paropen(42,NULL,NULL,NULL,NULL,NULL,42,42,NULL,NULL,NULL,NULL,NULL,NULL);
sion_generic_paropen_mapped(42,"","",NULL,NULL,42,42,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
sion_generic_parclose(42);
sion_generic_parclose_mapped(42);
]])],
                               [AC_MSG_RESULT([yes])],
                               [AC_MSG_RESULT([no])
                                otf2_have_sionlib="no"
                                otf2_sionlib_ldflags=""
                                otf2_sionlib_rpathflags=""
                                otf2_sionlib_libs=""
                                otf2_sionlib_reason=", generic API failed"])])


            # paradigm specific libsion checks
            AS_IF([test "x${otf2_have_sionlib}" = "xyes"], [
                AC_MSG_CHECKING([for SIONlib key-value API])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <sion.h>
#include <stddef.h>
]],[[
/* key-value functions */
sion_get_keyval_mode(42);
sion_key_full_scan(42);
sion_key_get_stat(42,42,NULL);
sion_seek_key(42,42,42,42);
sion_fread_key(NULL,42,42,42,42);
sion_fwrite_key(NULL,42,42,42,42);
]])],
                               [AC_MSG_RESULT([yes])],
                               [AC_MSG_RESULT([no])
                                otf2_have_sionlib="no"
                                otf2_sionlib_ldflags=""
                                otf2_sionlib_rpathflags=""
                                otf2_sionlib_libs=""
                                otf2_sionlib_reason=", basic key-value failed"])])


            AC_MSG_CHECKING([for SIONlib])
            AC_MSG_RESULT([$otf2_have_sionlib])
        ])

        CPPFLAGS=$cppflags_save
        LDFLAGS=$ldflags_save
        LIBS=$libs_save
        AC_LANG_POP([C])
    ])

#echo "debug: otf2_sionlib_cppflags=$otf2_sionlib_cppflags"
#echo "debug: otf2_sionlib_ldflags=$otf2_sionlib_ldflags"
#echo "debug: otf2_sionlib_libs=$otf2_sionlib_libs"

# The output of this macro
AC_SUBST([OTF2_SIONLIB_CPPFLAGS], [$otf2_sionlib_cppflags])
AC_SUBST([OTF2_SIONLIB_LDFLAGS],  ["$otf2_sionlib_ldflags $otf2_sionlib_rpathflags"])
AC_SUBST([OTF2_SIONLIB_LIBS],     [$otf2_sionlib_libs])
AFS_SUMMARY([SIONlib support], [${otf2_have_sionlib}${otf2_sionlib_reason}])

AS_UNSET([otf2_have_sionconfig])
AS_UNSET([otf2_sionlib_config_bin])
AS_UNSET([otf2_sionlib_config_arg])
AS_UNSET([otf2_sionlib_reason])
AS_UNSET([sionconfig_febe_flag])
AS_UNSET([sionconfig_paradigm_flag])
AS_UNSET([sionconfig_architecture_flags])
])
