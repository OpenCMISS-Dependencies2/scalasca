##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 2015-2016                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


AC_DEFUN([AC_CUBE_TESTS],
[
AC_ARG_WITH( test, [AS_HELP_STRING([--with-test=path], [ Defined path where is the test file generated for the chaining up the test suits])],[ CUBE_TEST_FILE_LOCATION=$withval],[CUBE_TEST_FILE_LOCATION="."])


AS_IF([ test "x$CUBE_TEST_FILE_LOCATION" == "x." || test "x$CUBE_TEST_FILE_LOCATION" == "xyes"], [
CUBE_TEST_FILE_LOCATION="."
])

AS_IF([ ! test -d $CUBE_CUBELIB ],[
CUBE_TEST_FILE_LOCATION="."
])
AC_MSG_NOTICE([Use $CUBE_TEST_FILE_LOCATION to generate an initial test .cubex file ])

AC_SUBST([CUBE_TEST_FILE_LOCATION])

])
