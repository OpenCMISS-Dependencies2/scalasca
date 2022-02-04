## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2012, 2014,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2012,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


## check fo some usefull entropy sources for the trace id.

AC_DEFUN([OTF2_ENTROPY_SOURCES], [
AC_REQUIRE([AC_SCOREP_POSIX_FUNCTIONS])

dnl be lazy and define some short local macro variables, which expends to
dnl unique shell variables based on the macro name
m4_pushdef([list], [m4_tolower([$0])_list])dnl
m4_pushdef([sep], [m4_tolower([$0])_sep])dnl
list=""
sep=""

AC_LANG_PUSH([C])

OTF2_TIMER_CLOCK_GETTIME_AVAILABLE
OTF2_TIMER_GETTIMEOFDAY_AVAILABLE

otf2_timer_lib=""
AS_IF([test "x${otf2_timer_clock_gettime_available}" = "xyes"],
    [otf2_timer_lib=${otf2_timer_librt}
     AS_VAR_APPEND(list, ["${sep}]clock_gettime["]); sep=" "],
    [test "x${otf2_timer_gettimeofday_available}" = "xyes"],
    [AS_VAR_APPEND(list, ["${sep}]gettimeofday["]); sep=" "])
AC_SUBST([OTF2_TIMER_LIB], ["$otf2_timer_lib"])

AH_TEMPLATE([HAVE_GETPID],
            [Define to 1 if the getpid() function is available.])
AH_TEMPLATE([HAVE_SYS_SYSINFO_H],
            [Define to 1 if you have the <sys/sysinfo.h> header file.])
AH_TEMPLATE([HAVE_SYSINFO],
            [Define to 1 if the sysinfo() function is available.])

AC_MSG_CHECKING([for getpid])
AC_LINK_IFELSE([AC_LANG_PROGRAM([
AC_INCLUDES_DEFAULT
], [
pid_t me = getpid();
])],
    [AC_MSG_RESULT([yes])
     AC_DEFINE([HAVE_GETPID])
     AS_VAR_APPEND(list, ["${sep}]getpid["]); sep=" "],
    [AC_MSG_RESULT([no])])

AC_CHECK_HEADERS([sys/sysinfo.h],
    [AC_MSG_CHECKING([for sysinfo])
     AC_LINK_IFELSE([AC_LANG_PROGRAM([
AC_INCLUDES_DEFAULT
@%:@include <sys/sysinfo.h>
], [
struct sysinfo info;
sysinfo( &info );
])],
        [AC_MSG_RESULT([yes])
         AC_DEFINE([HAVE_SYSINFO])
         AS_VAR_APPEND(list, ["${sep}]sysinfo["]); sep=" "],
        [AC_MSG_RESULT([no])])])

AC_LANG_POP([C])

AS_IF([test "x${has_gethostid_func}" = "xyes"],
      [AS_VAR_APPEND(list, ["${sep}gethostid"]); sep=" "])

AS_IF([test -z "${list}"], [list=none])
AFS_SUMMARY([Entropy sources], [${]list[}])

m4_popdef([list])dnl
m4_popdef([sep])dnl
])
