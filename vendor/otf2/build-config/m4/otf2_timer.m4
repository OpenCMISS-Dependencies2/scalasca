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
## Copyright (c) 2009-2014,
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



###############################################################################

AC_DEFUN([OTF2_TIMER_CLOCK_GETTIME_AVAILABLE],[
otf2_timer_clock_gettime_available="no"

otf2_timer_save_LIBS="$LIBS"
AC_SEARCH_LIBS([clock_gettime], [rt],
               [AS_IF([test "x${ac_cv_search_clock_gettime}" != "xnone required"],
                      [otf2_timer_librt="$ac_cv_search_clock_gettime"])])

AC_MSG_CHECKING([for CLOCK_REALTIME])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
#ifdef _POSIX_C_SOURCE
#  if _POSIX_C_SOURCE < 199309L
#    undef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 199309L
#  endif
#else
#  define _POSIX_C_SOURCE 199309L
#endif
#include <time.h>
    ],
    [
    struct timespec tp;
    clock_getres(  CLOCK_REALTIME, &tp );
    clock_gettime( CLOCK_REALTIME, &tp );
    ])],
    [otf2_timer_clock_gettime_available="yes"],
    [])
AC_MSG_RESULT([$otf2_timer_clock_gettime_available])

# perform a final link test
AS_IF([test "x${otf2_timer_clock_gettime_available}" = "xyes"],
      [AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#ifdef _POSIX_C_SOURCE
#  if _POSIX_C_SOURCE < 199309L
#    undef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 199309L
#  endif
#else
#  define _POSIX_C_SOURCE 199309L
#endif
#include <time.h>]], [[
    struct timespec tp;
    clock_getres( CLOCK_REALTIME , &tp );
    clock_gettime( CLOCK_REALTIME, &tp );
]])], [AC_DEFINE([HAVE_CLOCK_GETTIME], [1],
                 [Defined to 1 if the clock_gettime() function is available.])],
      [otf2_timer_clock_gettime_available="no"])])
AC_MSG_CHECKING([for clock_gettime timer])
AC_MSG_RESULT([$otf2_timer_clock_gettime_available])

LIBS="$otf2_timer_save_LIBS"

])

###############################################################################

AC_DEFUN([OTF2_TIMER_GETTIMEOFDAY_AVAILABLE],[
otf2_timer_gettimeofday_available="no"
AH_TEMPLATE([HAVE_GETTIMEOFDAY],
            [Define to 1 if the gettimeofday() function is available.])
AC_MSG_CHECKING([for gettimeofday timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <sys/time.h>
                                ]], [[
struct timeval tp;
gettimeofday( &tp, 0 );
                                ]])], [
otf2_timer_gettimeofday_available="yes"
AC_DEFINE([HAVE_GETTIMEOFDAY])
                                ], [])
AC_MSG_RESULT([$otf2_timer_gettimeofday_available])
])

###############################################################################
