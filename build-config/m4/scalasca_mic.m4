##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 2015                                                     ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


# SCALASCA_DETECT_MIC_BUILD
# -------------------------
# Guesstimates whether a native MIC build already exists in the given install
# location. The decision is based on the existance of the 'scout.ser' binary
# in '${bindir}/mic', which is of course only a very crude heuristic.
#
# List of configure variables set:
#  `scalasca_have_mic_support`:: Set to 'yes' if a native MIC build was
#                                detected, 'no' otherwise.
#
# List of provided config header defines:
#  `HAVE_MIC_SUPPORT`:: Defined if a native MIC build was detected
#
# List of provided automake conditionals:
#  `MIC_SUPPORT`:: Enabled if a native MIC build was detected
#
AC_DEFUN([SCALASCA_DETECT_MIC_BUILD], [
    AC_MSG_CHECKING([for native Intel MIC build])
    scalasca_have_mic_support=no
    AM_COND_IF([PLATFORM_MIC],
        [],
        [adl_RECURSIVE_EVAL([${bindir}], [BINDIR])
         AS_IF([AS_EXECUTABLE_P([${BINDIR}/mic/scout.ser])],
             [scalasca_have_mic_support=yes
              AC_DEFINE([HAVE_MIC_SUPPORT],
                  [1],
                  [Defined to 1 if native MIC build exists])])
         AFS_SUMMARY([MIC target support], [${scalasca_have_mic_support}])])
    AM_CONDITIONAL([MIC_SUPPORT],
        [test "x${scalasca_have_mic_support}" = "xyes"])
    AC_MSG_RESULT([${scalasca_have_mic_support}])
])# SCALASCA_DETECT_MIC_BUILD
