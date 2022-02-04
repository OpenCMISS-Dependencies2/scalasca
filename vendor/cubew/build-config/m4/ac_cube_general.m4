##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2018                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  Copyright (c) 2009-2015                                                ##
##  German Research School for Simulation Sciences GmbH,                   ##
##  Laboratory for Parallel Programming                                    ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


AC_DEFUN([AC_SCOREP_CONFIG_SELECTION_REPORT], [

XEND=$1


AS_IF([ test "x$CROSS_BUILD" = "xyes" ],[

AS_IF([test "x$XEND" = "xfrontend" ], [
AFS_SUMMARY([Frontend zlib compression ], [$MSG_FRONTEND_COMPRESSION] )
AS_IF([test "x$MSG_FRONTEND_COMPRESSION" != "xno" ],[
AFS_SUMMARY([  zlib headers ], [$FRONTEND_LIBZ_HEADERS] )
AFS_SUMMARY([  zlib library ], [$FRONTEND_LIBZ_LDFLAGS $FRONTEND_LIBZ_LIBS] )
AFS_SUMMARY([  Notice], [Front end part of cube (c-writer library) produces compressed cube report.(enabled via environment variable CUBEW_ZLIB_COMPRESSION=true)])
])
])

AS_IF([test "x$XEND" = "xbackend" ], [
AFS_SUMMARY([Backend zlib compression ], [$MSG_BACKEND_COMPRESSION] )
AS_IF([test "x$MSG_BACKEND_COMPRESSION" = "xyes" ],[
AFS_SUMMARY([  zlib headers ], [$BACKEND_LIBZ_HEADERS] )
AFS_SUMMARY([  zlib library ], [$BACKEND_LIBZ_LDFLAGS $BACKEND_LIBZ_LIBS] )
AFS_SUMMARY([  Notice],[Back end part of cube (c-writer library) produces compressed cube report.(enabled via environment variable CUBEW_ZLIB_COMPRESSION=true)])
])
])
]
, [
AFS_SUMMARY([zlib compression ], [$MSG_BACKEND_COMPRESSION] )
AS_IF([test "x$MSG_BACKEND_COMPRESSION" != "xno" ],[
AFS_SUMMARY([  zlib headers ], [$BACKEND_LIBZ_HEADERS] )
AFS_SUMMARY([  zlib library ], [$BACKEND_LIBZ_LDFLAGS $BACKEND_LIBZ_LIBS] )
AFS_SUMMARY([  Notice],[Cube (c-writer library) produces compressed cube report.(enabled via environment variable CUBEW_ZLIB_COMPRESSION=true)])
])
])
AFS_SUMMARY([  zlib compression flags ], [$COMPRESSION] )

AFS_SUMMARY([Advanced memory handling ], [$advmemory] )
AFS_SUMMARY([Internal memory tracking ], [$memory_tracking] )
AFS_SUMMARY([Internal memory tracing ], [$memory_tracing] )




AS_IF([test "x$XEND" = "xbackend" && test "x$CROSS_BUILD" == "xyes" ], [
AFS_SUMMARY([C99 compiler used],[$CC])
AFS_SUMMARY([C++ compiler used],[$CXX])
AFS_SUMMARY([ Compiler flags used],[$CFLAGS])
AFS_SUMMARY([ Compiler cxxflags used],[$CXXFLAGS])
],
[
AFS_SUMMARY([C99 compiler used],[$CC])
AFS_SUMMARY([C++ compiler used],[$CXX])
AFS_SUMMARY([ Compiler flags used],[$CFLAGS])
AFS_SUMMARY([ Compiler cxxflags used],[$CXXFLAGS])
])



])









AC_DEFUN([AC_SCOREP_CUBE_BACKEND_SELECTION], [

#AC_SCOREP_CUBE_INITIALIZE_BACKEND_SELECTION
#AC_SCOREP_CUBE_CWRITER_SELECTION
AC_CUBE_TESTS

AM_CONDITIONAL([CUBEW_COMPRESSION_BACKEND], [test "x$MSG_BACKEND_COMPRESSION" = "xyes"])
AFS_AM_CONDITIONAL([ENABLE_MAKEFILE_RULES_FOR_REGENERATION], [test "x$ENABLE_MAKEFILE_RULES_FOR_REGENERATION" = "xyes"], [false])
])



AC_DEFUN([AC_SCOREP_CUBE_ACCELERATOR_SELECTION], [

#AC_SCOREP_CUBE_INITIALIZE_BACKEND_SELECTION
#AC_SCOREP_CUBE_CWRITER_SELECTION

])



AC_DEFUN([AC_SCOREP_CUBE_FRONTEND_SELECTION], [
AC_SCOREP_CUBE_INITIALIZE_FRONTEND_SELECTION
AC_SCOREP_CUBE_CWRITER_SELECTION

AM_CONDITIONAL([CUBEW_COMPRESSION_FRONTEND], [test "x$MSG_FRONTEND_COMPRESSION" = "xyes"])

])


AC_DEFUN([AC_SCOREP_CUBE_CWRITER_SELECTION], [

#AC_ARG_WITH(cwriter, [AS_HELP_STRING([--with-cwriter | --without-cwriter], [Enables (default) or disables building and installation of the C cube writer.])],[],[])

AS_IF( [ test x$with_cwriter = "xyes" ], [
WITH_C_WRITER="yes"
])

AS_IF( [ test x$with_cwriter = "xno" || test "x$without_cwriter" = "xyes" ], [
WITH_C_WRITER="no"
])
AC_SUBST(WITH_C_WRITER)
AM_CONDITIONAL([WITH_C_WRITER], [test x$WITH_C_WRITER == "xyes"])
])





AC_DEFUN([AC_SCOREP_CUBE_INITIALIZE_BACKEND_SELECTION], [

WITH_C_WRITER="yes"

])


AC_DEFUN([AC_SCOREP_CUBE_INITIALIZE_FRONTEND_SELECTION], [

WITH_C_WRITER="yes"

])




AC_DEFUN([AC_SCOREP_CONSTUCT_LIST_OF_LIB_DIRS], [

sufix=""
   case "x${build_cpu}" in
    "xx86")
        sufix="32"
        ;;
    "xx86_64")
        sufix="64"
        ;;
    "xppc32")
        sufix="32"
        ;;
    "xppc64")
        sufix="64"
        ;;
    "xia64")
        sufix="64"
        ;;
    "xia32")
        sufix="32"
        ;;
    "xpowerpc64")
        sufix="64"
        ;;
    "xpowerpc32")
        sufix="32"
        ;;
    "xmips64")
        sufix="64"
        ;;
    "xmips32")
        sufix="32"
        ;;
   esac
if test "x$sufix" = "x"; then
LIBDIRS_LIST="/lib/ /usr/lib/ "
else
LIBDIRS_LIST="/lib/ /lib$sufix/ /usr/lib/ /usr/lib$sufix/ "
fi


])


