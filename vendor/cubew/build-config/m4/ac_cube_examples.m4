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


AC_DEFUN([AC_SCOREP_FRONTEND_EXAMPLES],
[

	NOMPI_CUBEW_FRONTEND_EXAMPLES_SRC="cubew_example_complex.c \
					   cubew_example_simple.c \
					   cubew_example_derived.c \
					   cubew_system_tree_memory_footprint_standard.c\
					   cubew_system_tree_memory_footprint_optimized.c"
	NOMPI_CUBEW_FRONTEND_EXAMPLES_EXE="cubew_example_complex.c.exe \
					   cubew_example_simple.c.exe \
					   cubew_example_derived.c.exe \
					    cubew_system_tree_memory_footprint_standard.c.exe\
					    cubew_system_tree_memory_footprint_optimized.c.exe" 
	if test "x$MPICC" != "x"; then
	    MPI_CUBEW_FRONTEND_EXAMPLES_SRC=""
	    MPI_CUBEW_FRONTEND_EXAMPLES_EXE=""
	fi


    NOMPI_FRONTEND_APPS_EXE="$NOMPI_CUBEW_FRONTEND_EXAMPLES_EXE"
    MPI_FRONTEND_APPS_EXE="$MPI_CUBEW_FRONTEND_EXAMPLES_EXE"





    AC_SUBST([NOMPI_CUBEW_FRONTEND_EXAMPLES_SRC])
    AC_SUBST([NOMPI_CUBEW_FRONTEND_EXAMPLES_EXE])
    AC_SUBST([MPI_CUBEW_FRONTEND_EXAMPLES_SRC])
    AC_SUBST([MPI_CUBEW_FRONTEND_EXAMPLES_EXE])

])




AC_DEFUN([AC_SCOREP_BACKEND_EXAMPLES],
[

    MPI_BACKEND_APPS=""
    NOMPI_BACKEND_APPS=""
    
	NOMPI_CUBEW_BACKEND_EXAMPLES_SRC=""
	NOMPI_CUBEW_BACKEND_EXAMPLES_EXE=""
	if test "x$MPICC" != "x"; then
	    MPI_CUBEW_BACKEND_EXAMPLES_SRC="cubew_example_simple.mpi.c \
					    cubew_example_complex.mpi.c"
	    MPI_CUBEW_BACKEND_EXAMPLES_EXE="cubew_example_simple.mpi.c.exe \
					    cubew_example_complex.mpi.c.exe"
	fi
    
    NOMPI_BACKEND_APPS_EXE="$NOMPI_CUBEW_BACKEND_EXAMPLES_EXE"
    MPI_BACKEND_APPS_EXE="$MPI_CUBEW_BACKEND_EXAMPLES_EXE"



    AC_SUBST([NOMPI_CUBEW_BACKEND_EXAMPLES_SRC])
    AC_SUBST([NOMPI_CUBEW_BACKEND_EXAMPLES_EXE])
    AC_SUBST([MPI_CUBEW_BACKEND_EXAMPLES_SRC])
    AC_SUBST([MPI_CUBEW_BACKEND_EXAMPLES_EXE])

])



AC_DEFUN([AC_CUBE_EXAMPLES],
[
    AC_SCOREP_FRONTEND_EXAMPLES
    AC_SCOREP_BACKEND_EXAMPLES


])




