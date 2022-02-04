## -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2014,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

ALL_SERIAL = \
    otf2_reader_example \
    otf2_writer_example

ALL_MPI = \
    otf2_mpi_reader_example \
    otf2_mpi_reader_example_cc \
    otf2_mpi_writer_example

ALL_OPENMP = \
    otf2_openmp_writer_example

ALL_PTHREAD = \
    otf2_pthread_writer_example

all: $(ALL_SERIAL) $(ALL_MPI) $(ALL_OPENMP) $(ALL_PTHREAD)

serial: $(ALL_SERIAL)

mpi: $(ALL_MPI)

openmp: $(ALL_OPENMP)

pthread: $(ALL_PTHREAD)

otf2_reader_example.o: otf2_reader_example.c
	gcc -std=c99 `otf2-config --cflags` \
            -c otf2_reader_example.c \
            -o otf2_reader_example.o

otf2_reader_example: otf2_reader_example.o
	gcc otf2_reader_example.o \
            `otf2-config --ldflags` \
            `otf2-config --libs` \
            -o otf2_reader_example

otf2_writer_example.o: otf2_writer_example.c
	gcc -std=c99 `otf2-config --cflags` \
            -c otf2_writer_example.c \
            -o otf2_writer_example.o

otf2_writer_example: otf2_writer_example.o
	gcc otf2_writer_example.o \
            `otf2-config --ldflags` \
            `otf2-config --libs` \
            -o otf2_writer_example

otf2_mpi_reader_example.o: otf2_mpi_reader_example.c
	mpicc -std=c99 `otf2-config --cflags` \
            -c otf2_mpi_reader_example.c \
            -o otf2_mpi_reader_example.o

otf2_mpi_reader_example: otf2_mpi_reader_example.o
	mpicc otf2_mpi_reader_example.o \
            `otf2-config --ldflags` \
            `otf2-config --libs` \
            -o otf2_mpi_reader_example

otf2_mpi_reader_example_cc.o: otf2_mpi_reader_example.cc
	mpic++ `otf2-config --cflags` \
            -c otf2_mpi_reader_example.cc \
            -o otf2_mpi_reader_example_cc.o

otf2_mpi_reader_example_cc: otf2_mpi_reader_example_cc.o
	mpic++ otf2_mpi_reader_example_cc.o \
            `otf2-config --ldflags` \
            `otf2-config --libs` \
            -o otf2_mpi_reader_example_cc

otf2_mpi_writer_example.o: otf2_mpi_writer_example.c
	mpicc -std=c99 `otf2-config --cflags` \
            -c otf2_mpi_writer_example.c \
            -o otf2_mpi_writer_example.o

otf2_mpi_writer_example: otf2_mpi_writer_example.o
	mpicc otf2_mpi_writer_example.o \
            `otf2-config --ldflags` \
            `otf2-config --libs` \
            -o otf2_mpi_writer_example

otf2_openmp_writer_example.o: otf2_openmp_writer_example.c
	gcc -std=c99 -fopenmp `otf2-config --cflags` \
            -c otf2_openmp_writer_example.c \
            -o otf2_openmp_writer_example.o

otf2_openmp_writer_example: otf2_openmp_writer_example.o
	gcc -fopenmp otf2_openmp_writer_example.o \
            `otf2-config --ldflags` \
            `otf2-config --libs` \
            -o otf2_openmp_writer_example

otf2_pthread_writer_example.o: otf2_pthread_writer_example.c
	gcc -std=c99 -pthread `otf2-config --cflags` \
            -c otf2_pthread_writer_example.c \
            -o otf2_pthread_writer_example.o

otf2_pthread_writer_example: otf2_pthread_writer_example.o
	gcc -pthread otf2_pthread_writer_example.o \
            `otf2-config --ldflags` \
            `otf2-config --libs` \
            -o otf2_pthread_writer_example

clean:
	$(RM) $(ALL) $(ALL_MPI) $(ALL_OPENMP) $(ALL_PTHREAD) *.o
