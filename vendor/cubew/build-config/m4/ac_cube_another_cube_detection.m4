##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2016                                                ##
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



AC_DEFUN([AC_SCOREP_IS_ANOTHER_CUBE_INSTALLED],
[

function test_cube_location
{
dir=$[]1
if test "x$dir" = "x"; then
command="cube_config.sh"
else
command="$dir/cube_config.sh"
fi
testline=`$command --name  2>&1`
if test "x$testline" == "xCUBE"; then
   return 1
fi
   return 0
}



CUBE_PRESENT=no


test_cube_location
result=$?
if [[ $result -eq 1 ]]; then
   anothercube=`cube_config.sh --cube-dir `
   AC_MSG_WARN(=================================================================================================================)
   AC_MSG_WARN(Another installation of CUBE is detected (in $anothercube). Clash of distributions is possible.)
   AC_MSG_WARN(=================================================================================================================)
   ANOTHER_CUBE_PRESENT=yes
else

    cube_env=`echo "\$CUBE_DIR"`
    test_cube_location $cube_env
    result=$?
    if [[ $result -eq 1 ]]; then
	anothercube=`$cube_env/cube_config.sh --cube-dir `
	AC_MSG_WARN(=================================================================================================================)
	AC_MSG_WARN(Another installation of CUBE is detected (in $anothercube). Clash of distributions is possible.)
	AC_MSG_WARN(=================================================================================================================)
	ANOTHER_CUBE_PRESENT=yes
    fi
fi

test_cube_location $prefix/bin
result=$?
if [[ $result -eq 1 ]]; then
   overwritecube=`$prefix/bin/cube_config.sh --cube-dir `
   AC_MSG_WARN(=================================================================================================================)
   AC_MSG_WARN(Installation of CUBE is detected (in $overwritecube). New installation will overwrite the present one.)
   AC_MSG_WARN(=================================================================================================================)
   ANOTHER_CUBE_OVERWRITE=yes
fi



])



AC_DEFUN([AC_SCOREP_DETECT_MODULES_PATH],
[
if test "x$MODULEPATH" != "x"; then
    INSTALL_MODULE_FILE="yes"
    if test "x$CROSS_BUILD" = "xyes"; then
	DEFAULT_MODULE_VERSION="$PACKAGE_VERSION-frontend"
    else
	DEFAULT_MODULE_VERSION="$PACKAGE_VERSION"
    fi
    AC_SUBST([DEFAULT_MODULE_VERSION])
fi

])
