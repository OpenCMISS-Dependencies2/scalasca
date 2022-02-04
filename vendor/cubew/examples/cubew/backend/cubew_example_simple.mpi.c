/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/**
 * \file cubew_example_simple.mpi.c
   \brief A small example of using "libcube4w.a" in a simple MPI pattern.
   Every process runs same code with small differenced due the MPI_Gather and IO.
   Every process creates a cube, but only root creates it as a CUBE_MASTER.
   Other processes create cube as a CUBE_SLAVE, which doesn't perform any IO.

   Then one proceed as usual, creates dimensions, calltree and systems.
   Rank0 performs IO, therefore whole data is sended to rank 0 using MPI_Gather.

   Every call *_write_row writes row to the cube repors directly on disk (only by root).

 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <mpi.h>

#include "cubew_cube.h"

/* used in static array initialization. see below by topology definition*/
#define NDIMS 2

/// Start point of this example.
int
main( int argc, char* argv[] )
{
    int my_rank;
    int root = 0;
    int mpi_cw_size;

    MPI_Init( &argc, &argv );                            /* starts MPI */
    MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );           /* get current process id */
    MPI_Comm_size( MPI_COMM_WORLD, &mpi_cw_size );       /* get number of processes */



    char cubefile[ 20 ] = "simple-mpi-cube";
    /* Create a cube with name cubefile.
        CUBE_MASTER indicates, that this instance will write metadata, indexes and whole data
        CUBE_SLAVE created a cube, but without ability to write any thing to the disk. Used to create symmetrical code witout "if" forrest
        CUBE_FALSE  indicate, that no internal compression of data will be performed.
     */
    cube_t* cube = NULL;
    if ( my_rank == root )
    {
        cube = cube_create( cubefile, CUBE_MASTER, CUBE_FALSE ); /* creates a cube report */
        printf( "Test file %s.cubex is being generated ...\n", cubefile );
    }
    else
    {
        cube = cube_create( cubefile, CUBE_SLAVE, CUBE_FALSE ); /* keeps structure in memory but doesnt write anything on disk */
    }
    if ( !cube )
    {
        fprintf( stderr, "cube_create failed!\n" );
        exit( 1 );
    }

    /* generate header */
    cube_def_mirror( cube, "http://www.fz-juelich.de/jsc/scalasca/" );
    cube_def_attr( cube, "description", "A simple example of Cube report" );
    cube_def_attr( cube, "experiment time", "November 1st, 2004" );


    /* define metric tree */
    cube_metric* met0, * met1, * met2;
    /* root metric Time */
    met0 = cube_def_met( cube, "Time", "time", "FLOAT", "sec", "",
                         "@mirror@patterns-2.1.html#execution",
                         "root node", NULL, CUBE_METRIC_EXCLUSIVE );

    /* A child of previos metric */
    met1 = cube_def_met( cube, "User time", "usertime", "FLOAT", "sec", "",
                         "http://www.cs.utk.edu/usr.html",
                         "2nd level", met0, CUBE_METRIC_INCLUSIVE );
    /* Another root metric */
    met2 = cube_def_met( cube, "Bytes transferred", "bytes", "INTEGER", "sec", "",
                         "http://www.cs.utk.edu/sys.html",
                         "0nd level", NULL, CUBE_METRIC_EXCLUSIVE );

    /* define call tree */
    /* definition of regions in calltree */
    char*        mod = "/ICL/CUBE/example.c";
    cube_region* regn0, * regn1, * regn2;
    regn0 = cube_def_region( cube, "main", 21, 100, "", "1st level", mod );
    regn1 = cube_def_region( cube, "<<init>>foo", 1, 10, "", "2nd level", mod );
    regn2 = cube_def_region( cube, "<<loop>>bar", 11, 20, "", "2nd level", mod );

    /* actual definition of calltree */
    cube_cnode* cnode0, * cnode1, * cnode2;
    cnode0 = cube_def_cnode_cs( cube, regn0, mod, 21, NULL );
    cnode1 = cube_def_cnode_cs( cube, regn1, mod, 60, cnode0 );
    cnode2 = cube_def_cnode_cs( cube, regn2, mod, 80, cnode0 );

    /* specify parameters for callpathes */
    /* first two nimerical and one string parameter for root cnode in callpath */
    cube_cnode_add_numeric_parameter( cnode0, "Phase", 1 );
    cube_cnode_add_numeric_parameter( cnode0, "Phase", 2 );
    cube_cnode_add_string_parameter( cnode0, "Iteration", "Initialization" );
    /* Set string parameter for the last callpath  */
    cube_cnode_add_string_parameter( cnode2, "Etappe", "Finish" );


    /* define location tree */
    cube_machine* mach  = cube_def_mach( cube, "MSC<<juelich>>", "" );
    cube_node*    node  = cube_def_node( cube, "Athena<<juropa>>", mach );
    cube_process* proc0 = cube_def_proc( cube, "Process 0<<master>>", 0, node );
    cube_thread** thrds = ( cube_thread** )calloc( mpi_cw_size, sizeof( cube_thread* ) );
    int           i     = 0;
    for ( i = 0; i < mpi_cw_size; i++ )
    {
        char buffer[ 30 ];
        sprintf( buffer, "Thread %d", i );
        thrds[ i ] = cube_def_thrd( cube, buffer, i, proc0 );
    }


    /* define  first topology */
    /* number of dimensions in topology  - linear topology*/
    /* number of elements in every dimension - size of COMM_WORLD */
    long dimv0[ 1 ] = { mpi_cw_size };
    /* is dimension periodical (torus) or no: 0 - 'no', 1 - 'yes' */
    int periodv0[ 1 ] = { 1 };
    /* create first topology */
    cube_cartesian* cart0 = cube_def_cart( cube, 1, dimv0, periodv0 );
    /* set name of the topology */
    cube_cart_set_name( cart0, "Application Topology  (Linear)" );

    i = 0; /* reuse index from above */
    for ( i = 0; i < mpi_cw_size; i++ )
    {
        /* set coordinate of every thread */
        long coordv[ 1 ] = { i };
        cube_def_coords( cube, cart0, thrds[ i ], coordv );
    }

    /* define  second topology - three times larger, every third is set */
    /* number of dimensions in topology  - linear topology*/
    /* number of elements in every dimension - size of COMM_WORLD */
    long dimv1[ 1 ] = { mpi_cw_size*3 };
    /* is dimension periodical (torus) or no: 0 - 'no', 1 - 'yes' */
    int periodv1[ 1 ] = { 1 };
    /* create second topology */
    cube_cartesian* cart1 = cube_def_cart( cube, 1, dimv1, periodv1 );
    /* set name of the topology */
    cube_cart_set_name( cart1, "Application Topology  (Linear Expanded)" );

    i = 0; /* reuse index from above */
    for ( i = 0; i < mpi_cw_size; i++ )
    {
        /* set coordinate of every thread */
        long coordv[ 1 ] = { i*3 };
        cube_def_coords( cube, cart1, thrds[ i ], coordv );
    }


    /* define  third topology - a minimal square, which contains #nthreads elements */
    /* number of dimensions in topology  - 2d topology*/
    long size_of_dimenstion = ( long )( ceil( sqrt( mpi_cw_size ) ) );
    /* number of elements in every dimension - (ceil(sqrt(size of COMM_WORLD)))^2 */
    long dimv2[ 2 ] = { size_of_dimenstion, size_of_dimenstion };
    /* is dimension periodical (torus) or no: 0 - 'no', 1 - 'yes' */
    int periodv2[ 2 ] = { 1, 1 };
    /* create second topology */
    cube_cartesian* cart2 = cube_def_cart( cube, 2, dimv2, periodv2 );
    /* set name of the topology */
    cube_cart_set_name( cart1, "Application 2D Topology  (Square)" );

    i = 0; /* reuse index from above */
    for ( i = 0; i < mpi_cw_size; i++ )
    {
        /* set coordinate of every thread */
        /* we fill rowwise */
        long coordv[ 2 ] = { i / size_of_dimenstion, i % size_of_dimenstion };
        cube_def_coords( cube, cart2, thrds[ i ], coordv );
    }




    /* create an array with #elements = #threads in cube report. In this case it is the size of MPI_COMM_WORLD */
    double  dsev  = 0.;
    double* drsev = NULL;
    if ( my_rank == root )
    {
        /* root creates also storage for whole data*/
        drsev = ( double* )calloc( mpi_cw_size, sizeof( double ) );
    }



    /* write data for metric met0 */
    /* once started to write one metric, whole data for this metric should be written without interruption for
       writing of another metric. Otherwise created cube will have broken structure */

    /*  cnodes have to be writen in certain sequence, if Compressed cube should be written.

        Every metric, depending if one writes exclusive or inclusive values have to be written in specific order.

        Use call cube_dyn_array * sequence = cube_get_cnodes_for_metric(cube_t * cube, cube_metric* metric);
        to obtain the sequence of cnodes for every metric.
     */


    /* create rank specific data - myrank number + 100 */
    dsev = my_rank + 100.;
    MPI_Gather( &dsev, 1, MPI_DOUBLE, drsev, 1, MPI_DOUBLE, root, MPI_COMM_WORLD );
    /* Only CUBE_MASTER writes data on disk, all others do ignore that call*/
    cube_write_sev_row( cube, met0, cnode0, drsev );

    dsev = my_rank + 300.;
    MPI_Gather( &dsev, 1, MPI_DOUBLE, drsev, 1, MPI_DOUBLE, root, MPI_COMM_WORLD );
    cube_write_sev_row( cube, met0, cnode1, drsev );

    dsev = my_rank - 3.141529;
    MPI_Gather( &dsev, 1, MPI_DOUBLE, drsev, 1, MPI_DOUBLE, root, MPI_COMM_WORLD );
    cube_write_sev_row( cube, met0, cnode2, drsev );

    dsev = ( my_rank * 3.141529 );
    MPI_Gather( &dsev, 1, MPI_DOUBLE, drsev, 1, MPI_DOUBLE, root, MPI_COMM_WORLD );
    cube_write_sev_row( cube, met1, cnode0, drsev );

    /* metric met2 stores integers.
        call cube_write_sev_row(...) doesn't perform type casting in the case, if one writes an arrow of "wrong" types.
        It will assume proper array type and write it as it is. As result - wrong data in cube.

        Use calls
            cube_write_sev_row_of_doubles(...) or
            cube_write_sev_row_of_uint64(...) if you want, that cube performs a proper type casting.

        If you want to write another data that 'double' or 'uint64_t'
        use calls of the family cube_write_sev_row_of_cube_type_<type>(...).
        Inspect Cube C-Writer Reference for the list of the supported data types.
     */
    /* create an array with #elements = #threads in cube report. In this case it is the size of MPI_COMM_WORLD */
    /* last metric stored INTEGER data, which is 64bits wide*/
    uint64_t  isev  = 0.;
    uint64_t* irsev = NULL;
    if ( my_rank == root )
    {
        /* root creates also storage for whole data*/
        irsev = ( uint64_t* )calloc( mpi_cw_size, sizeof( uint64_t ) );
    }
    /* every rank sends its (rank) multiplicity of 5th Mersenne prime number*/
    isev = ( my_rank * 8191L );
    MPI_Gather( &isev, 1, MPI_LONG_LONG, irsev, 1, MPI_LONG_LONG, root, MPI_COMM_WORLD );
    cube_write_sev_row( cube, met2, cnode2, irsev );

    if ( my_rank == root )
    {
        printf( "Test file %s.cubex complete.\n", cubefile );
    }
    /* Delete cube_t structure and release all memory*/
    cube_free( cube );
    free( irsev );
    free( drsev );
    free( thrds );


    /* finish */
    MPI_Finalize();
    return 0;
}
