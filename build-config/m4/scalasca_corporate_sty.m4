##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 2018-2019                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


AC_DEFUN([SCALASCA_CORPORATE_STY], [
AC_REQUIRE([AC_SCOREP_DOXYGEN])
AS_IF([test "x${have_pdflatex}" = xyes],
    [cat << _SCALASCAEOF > conftest.tex
\documentclass{article}
\usepackage{corporate}
\begin{document}
\end{document}
_SCALASCAEOF
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
