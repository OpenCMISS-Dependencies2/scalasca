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


AC_DEFUN([AC_CUBE_MEMORY], [

AC_ARG_WITH(advanced-memory, [AS_HELP_STRING([--with-advanced-memory | --without-advanced-memory ] ,[ Enables advanced memory features such as usage of an external memory allocator.  ])],[advmemory=$with_advanced_memory],[ advmemory="no"])
AC_ARG_WITH(memory-tracking, [AS_HELP_STRING([--with-memory-tracking | --without-memory-tracking ] ,[ Enables internal memory tracking (requires --with-advanced-memory).  ])],[memory_tracking=$with_memory_tracking],[ memory_tracking="no"])
AC_ARG_WITH(memory-tracing, [AS_HELP_STRING([--with-memory-tracing | --without-memory-tracing ] ,[ Enables internal memory tracing (requires --with-advanced-memory), more intensive memory usage report.  ])],[memory_tracing=$with_memory_tracing],[ memory_tracing="no"])
    ##
    ## check function declarations
    ##

    AC_LANG_PUSH(C)
    
        AS_IF([ test "x$memory_tracing" == "xyes" ],[ 
          AC_MSG_NOTICE([Enable internal memory tracing])
          AC_DEFINE(HAVE_MEMORY_TRACING, 1, [Enable internal (detailed) tracing of memory usage])
          advmemory="yes" # enable advanced memory 
          memory_tracking="yes" #enable memory tracking
          ])
        AS_IF([ test "x$memory_tracking" == "xyes" ],[ 
          AC_MSG_NOTICE([Enable internal memory tracking])
          AC_DEFINE(HAVE_MEMORY_TRACKING, 1, [Enable internal tracking of memory usage])
          advmemory="yes" # enable advanced memory 
          ])   
    AS_IF([ test "x$advmemory" == "xyes" ],[ 
          AC_MSG_NOTICE([Enable advanced memory handling])
          AC_DEFINE(HAVE_ADVANCED_MEMORY, 1, [Enable usage of external memory allocator])
          ])
          
    AC_LANG_POP(C)

    ##
    ## result
    ##

    AM_CONDITIONAL([HAVE_ADVANCED_MEMORY], [test "x${advmemory}" = "xyes"])
    AM_CONDITIONAL([HAVE_MEMORY_TRACKING], [test "x${memory_tracking}" = "xyes"])
    AM_CONDITIONAL([HAVE_MEMORY_TRACING], [test "x${memory_tracking}" = "xyes"])
])
