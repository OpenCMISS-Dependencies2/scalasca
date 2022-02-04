##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 2017-2018                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


# AC_CUBE_DETECT_MIC_BUILD
# ---------------------
# Guesstimates whether a native MIC build already exists in the given install
# locations. The decision is based on the existance of a 'cubew-config-mic'
# tool in '${libexecdir}/cubew', which is of course only a very crude heuristic.
#
# List of configure variables set:
#  `cubew_have_mic_support`:: Set to 'yes' if a native MIC build was detected,
#                            'no' otherwise.
#
# List of provided config header defines:
#  `HAVE_MIC_SUPPORT`:: Defined if a native MIC build was detected
#
# List of provided automake conditionals:
#  `MIC_SUPPORT`:: Enabled if a native MIC build was detected
#
AC_DEFUN([AC_CUBEW_DETECT_MIC_BUILD], [
    AC_MSG_CHECKING([for native Intel MIC build])
    cubew_have_mic_support=no
    AM_COND_IF([PLATFORM_MIC],
               [],
               [AS_IF([AS_EXECUTABLE_P([${PKGLIBEXECDIR}/cubew-config-mic])],
                      [cubew_have_mic_support=yes
                       AC_DEFINE([HAVE_MIC_SUPPORT],
                                 [1],
                                 [Defined to 1 if native MIC build exists])
                      ])
               ])
    AM_CONDITIONAL([MIC_SUPPORT], [test x$cubew_have_mic_support = "xyes"])
    AC_MSG_RESULT([$cubew_have_mic_support])
])# AC_CUBEW_DETECT_MIC_BUILD
