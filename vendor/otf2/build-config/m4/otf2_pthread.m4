## -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013-2014,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

AC_DEFUN([_OTF2_PTHREAD_BARRIER], [

for ftm in "" -D_XOPEN_SOURCE=600 -D_POSIX_C_SOURCE=200112
do
    save_LIBS="$LIBS"
    LIBS="$PTHREAD_LIBS $LIBS"
    save_CFLAGS="$CFLAGS"

    otf2_pthread_xopen_source_save="${otf2_pthread_xopen_source}"
    otf2_pthread_posix_c_source_save="${otf2_pthread_posix_c_source}"
    case $ftm in
    (-D_XOPEN_SOURCE=*)
        value=${ftm##*=}
        if test $value -gt ${otf2_pthread_xopen_source:-0}
        then
            otf2_pthread_xopen_source=$value
        fi
    ;;
    (-D_POSIX_C_SOURCE=*)
        value=${ftm##*=}
        if test $value -gt ${otf2_pthread_posix_c_source:-0}
        then
            otf2_pthread_posix_c_source=$value
        fi
    ;;
    esac

    otf2_pthread_cflags_extra="${otf2_pthread_xopen_source:+ -D_XOPEN_SOURCE=$otf2_pthread_xopen_source}${otf2_pthread_posix_c_source:+ -D_POSIX_C_SOURCE=${otf2_pthread_posix_c_source}L}"
    CFLAGS="$CFLAGS $PTHREAD_CFLAGS$otf2_pthread_cflags_extra"

    save_CC="$CC"
    CC="$PTHREAD_CC"

    AS_UNSET([ac_cv_have_decl_PTHREAD_BARRIER_SERIAL_THREAD])
    AC_CHECK_DECL([PTHREAD_BARRIER_SERIAL_THREAD],
        [otf2_pthread_barrier_serial_thread="yes"],
        [otf2_pthread_xopen_source="$otf2_pthread_xopen_source_save"
         otf2_pthread_posix_c_source="$otf2_pthread_posix_c_source_save"],
        [[#include <pthread.h>]])

    LIBS="$save_LIBS"
    CFLAGS="$save_CFLAGS"
    CC="$save_CC"

    AS_IF([test x"$otf2_pthread_barrier_serial_thread" = xyes], [
        break
    ])
done

AS_IF([test x"$otf2_pthread_barrier_serial_thread" = xyes], [
    :
    $1
], [
    :
    $2
])

AS_UNSET([otf2_pthread_barrier_serial_thread])
AS_UNSET([otf2_pthread_cflags_extra])
AS_UNSET([otf2_pthread_xopen_source_save])
AS_UNSET([otf2_pthread_posix_c_source_save])

]) dnl _OTF2_PTHREAD_BARRIER

AC_DEFUN([_OTF2_PTHREAD_MUTEX_ERRORCHECK], [

for flag in "" -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200809L
do
    save_LIBS="$LIBS"
    LIBS="$PTHREAD_LIBS $LIBS"
    save_CFLAGS="$CFLAGS"

    otf2_pthread_xopen_source_save="${otf2_pthread_xopen_source}"
    otf2_pthread_posix_c_source_save="${otf2_pthread_posix_c_source}"
    case $ftm in
    (-D_XOPEN_SOURCE=*)
        value=${ftm##*=}
        if test $value -gt ${otf2_pthread_xopen_source:-0}
        then
            otf2_pthread_xopen_source=$value
        fi
    ;;
    (-D_POSIX_C_SOURCE=*)
        value=${ftm##*=}
        if test $value -gt ${otf2_pthread_posix_c_source:-0}
        then
            otf2_pthread_posix_c_source=$value
        fi
    ;;
    esac

    otf2_pthread_cflags_extra="${otf2_pthread_xopen_source:+ -D_XOPEN_SOURCE=${otf2_pthread_xopen_source}}${otf2_pthread_posix_c_source:+ -D_POSIX_C_SOURCE=${otf2_pthread_posix_c_source}L}"
    CFLAGS="$CFLAGS $PTHREAD_CFLAGS$otf2_pthread_cflags_extra"

    save_CC="$CC"
    CC="$PTHREAD_CC"

    AS_UNSET([ac_cv_have_decl_PTHREAD_MUTEX_ERRORCHECK])
    AC_CHECK_DECL([PTHREAD_MUTEX_ERRORCHECK],
        [otf2_pthread_mutex_errorcheck="yes"],
        [otf2_pthread_xopen_source="$otf2_pthread_xopen_source_save"
         otf2_pthread_posix_c_source="$otf2_pthread_posix_c_source_save"],
        [[#include <pthread.h>]])

    LIBS="$save_LIBS"
    CFLAGS="$save_CFLAGS"
    CC="$save_CC"

    AS_IF([test x"$otf2_pthread_mutex_errorcheck" = xyes], [
        break
    ])
done

AS_IF([test x"$otf2_pthread_mutex_errorcheck" = xyes], [
    :
    $1
], [
    :
    $2
])

AS_UNSET([otf2_pthread_mutex_errorcheck])
AS_UNSET([otf2_pthread_cflags_extra])
AS_UNSET([otf2_pthread_xopen_source_save])
AS_UNSET([otf2_pthread_posix_c_source_save])

]) dnl _OTF2_PTHREAD_MUTEX_ERRORCHECK

AC_DEFUN([OTF2_PTHREAD], [

otf2_pthread_support=no
otf2_pthread_barrier_support=no
otf2_pthread_mutex_errorcheck_support=no

AX_PTHREAD([otf2_pthread_support=yes])
AS_IF([test x"${otf2_pthread_support}" = xyes], [
    _OTF2_PTHREAD_BARRIER([otf2_pthread_barrier_support=yes])
    _OTF2_PTHREAD_MUTEX_ERRORCHECK([otf2_pthread_mutex_errorcheck_support=yes])
])

AC_SCOREP_COND_HAVE([PTHREAD],
    [test x"${otf2_pthread_support}" = xyes],
    [Define if you have POSIX threads libraries and header files.])

AC_SCOREP_COND_HAVE([PTHREAD_BARRIER],
    [test x"${otf2_pthread_barrier_support}" = xyes],
    [Support for pthread_barrier_t.])

AC_SCOREP_COND_HAVE([PTHREAD_MUTEX_ERRORCHECK],
    [test x"${otf2_pthread_mutex_errorcheck_support}" = xyes],
    [Support for PTHREAD_MUTEX_ERRORCHECK.])

PTHREAD_CFLAGS="${PTHREAD_CFLAGS}${otf2_pthread_xopen_source:+ -D_XOPEN_SOURCE=${otf2_pthread_xopen_source}}${otf2_pthread_posix_c_source:+ -D_POSIX_C_SOURCE=${otf2_pthread_posix_c_source}L}"
AS_UNSET([otf2_pthread_xopen_source])
AS_UNSET([otf2_pthread_posix_c_source])

]) dnl OTF2_PTHREAD
