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



AC_DEFUN([AC_SCOREP_CUBE_VARS],
[
AC_REQUIRE([AC_SCOREP_PACKAGE_AND_LIBRARY_VERSION])
    # When working with a svn checkout, write a REVISION file. The REVISION
    # file is updated during each configure call and also at make doxygen-user
    # and make dist.

    # When working with a make-dist-generated tarball, REVISION is already
    # there.


    cubew_version=`${srcdir}/build-config/common/generate-package-version.sh ${srcdir}/build-config/VERSION`
    cubew_revision=${component_revision}
    dnl cube_common_revision=${common_revision}

    CUBEW_BUILD_REVISION=`echo $cubew_revision | sed -e "s,^.*:,,g" | sed -e "s,M$,,g"`
    CUBEW_BUILD_VERSION=`echo $cubew_version`
    CUBEW_BUILD_SHORT_NAME=$PACKAGE_NAME
    CUBEW_BUILD_CODENAME=""

    AC_SUBST([CUBEW_BUILD_VERSION])
    AC_SUBST([CUBEW_BUILD_REVISION])
    AC_SUBST([CUBEW_BUILD_SHORT_NAME])
    AC_SUBST([CUBEW_BUILD_CODENAME])
])



