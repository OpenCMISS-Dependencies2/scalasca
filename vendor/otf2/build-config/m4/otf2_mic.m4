## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2014,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# OTF2_DETECT_MIC_BUILD
# ---------------------
# Guesstimates whether a native MIC build already exists in the given install
# locations. The decision is based on the existance of a 'otf2-config-mic'
# tool in '${libexecdir}/otf2', which is of course only a very crude heuristic.
#
# List of configure variables set:
#  `otf2_have_mic_support`:: Set to 'yes' if a native MIC build was detected,
#                            'no' otherwise.
#
# List of provided config header defines:
#  `HAVE_MIC_SUPPORT`:: Defined if a native MIC build was detected
#
# List of provided automake conditionals:
#  `MIC_SUPPORT`:: Enabled if a native MIC build was detected
#
AC_DEFUN([OTF2_DETECT_MIC_BUILD], [
    AC_MSG_CHECKING([for native Intel MIC build])
    otf2_have_mic_support=no
    AM_COND_IF([PLATFORM_MIC],
               [],
               [AS_IF([AS_EXECUTABLE_P([${PKGLIBEXECDIR}/otf2-config-mic])],
                      [otf2_have_mic_support=yes
                       AC_DEFINE([HAVE_MIC_SUPPORT],
                                 [1],
                                 [Defined to 1 if native MIC build exists])
                      ])
               ])
    AM_CONDITIONAL([MIC_SUPPORT], [test x$otf2_have_mic_support = "xyes"])
    AC_MSG_RESULT([$otf2_have_mic_support])
])# OTF2_DETECT_MIC_BUILD
