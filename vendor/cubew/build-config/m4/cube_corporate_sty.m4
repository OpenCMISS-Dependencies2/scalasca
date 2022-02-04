##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 2018                                                     ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2018,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file cube_corporate_sty.m4

AC_DEFUN([CUBE_CORPORATE_STY], [
AC_REQUIRE([AC_SCOREP_DOXYGEN])
AS_IF([test "x${have_pdflatex}" = xyes],
    [cat << _CUBEEOF > conftest.tex
\documentclass{article}
\usepackage{corporate}
\begin{document}
\end{document}
_CUBEEOF
     AC_MSG_CHECKING([for corporate.sty])
     pdflatex -interaction=nonstopmode conftest.tex >/dev/null 2>&1
     AS_IF([test $? -ne 0],
         [AC_MSG_RESULT([no])
          AC_SUBST([HAVE_CORPORATE_STY], [%])],
         [AC_MSG_RESULT([yes])
          AC_SUBST([HAVE_CORPORATE_STY], [])])
          rm -f conftest.*
    ],
    [AC_SUBST([HAVE_CORPORATE_STY], [%])])
AC_CONFIG_FILES([doc/doxygen-common/imgsrc/corporate.tex])
])
