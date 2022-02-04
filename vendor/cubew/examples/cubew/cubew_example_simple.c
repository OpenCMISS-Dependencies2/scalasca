/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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
 * \file cubew_example_simple.c
   \brief A small example of using "libcube4w.a".
   One creates a cube file "example.cubex", defined structure of metrics, call tree, machine, cartesian topology
   and fills the cube withdata.
   Every call *_write_row writes row to the cube repors directly on disk.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "cubew_cube.h"

/* used in static array initialization. see below by topology definition*/
#define NDIMS1 2
#define NDIMS2 2
#define NDIMS3 4
#define NDIMS4 14

// / Start point of this example.
int
main( int argc, char* argv[] )
{
    char cubefile[ 12 ] = "simple-cube";
    /* Create a cube with name cubefile.
        CUBE_MASTER indicates, that this instance will write metadata, indexes and whole data
        CUBE_WRITER, CUBE_SLAVE are depricated and should be not used.
        CUBE_FALSE  indicate, that no internal compression of data will be performed.
     */
    cube_t* cube = cube_create( cubefile, CUBE_MASTER, CUBE_FALSE );
    if ( !cube )
    {
        fprintf( stderr, "cube_create failed!\n" );
        exit( 1 );
    }

    /* generate header */
    cube_def_mirror( cube, "http://www.fz-juelich.de/jsc/scalasca/" );
    cube_def_attr( cube, "description", "A simple example of Cube report" );
    cube_def_attr( cube, "experiment time", "November 1st, 2004" );

    printf( "Test file %s.cubex is being generated ...\n", cubefile );

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
    regn0 = cube_def_region( cube, "main", "main", "mpi", "barrier", 21, 100, "", "1st level", mod );
    regn1 = cube_def_region( cube, "<<init>>foo", "<<init>>foo", "mpi", "barrier", 1, 10, "", "2nd level", mod );
    regn2 = cube_def_region( cube, "<<loop>>bar", "<<loop>>bar", "mpi", "barrier", 11, 20, "", "2nd level", mod );

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
    cube_process* proc1 = cube_def_proc( cube, "Process 1<<worker>>", 1, node );
    cube_thread*  thrd0 = cube_def_thrd( cube, "Thread 0<<iterator>>", 0, proc0 );
    cube_thread*  thrd1 = cube_def_thrd( cube, "Thread 1<<solver>>", 1, proc1 );



    /* define first topology */
    /* number of dimensions in topology */
    /* number of elements in every dimension */
    long dimv0[ NDIMS1 ] = { 5, 5 };
    /* is dimension periodical (torus) or no: 0 - 'no', 1 - 'yes' */
    int periodv0[ NDIMS1 ] = { 1, 0 };
    /* create first topology */
    cube_cartesian* cart0 = cube_def_cart( cube, NDIMS1, dimv0, periodv0 );
    /* set name of the topology */
    cube_cart_set_name( cart0, "Application Topology 1" );
    char* dimnames1[ NDIMS1 ] = { "X", "Y" };
    /* set names for every dimension : all names should be given, as null or char* */
    cube_cart_set_namedims( cart0, dimnames1 );

    long coordv[ NDIMS1 ] = { 0, 0 };
    cube_def_coords( cube, cart0, thrd1, coordv );
    /* define second topology */
    long            dimv1[ NDIMS2 ]    = { 3, 3 };
    int             periodv1[ NDIMS2 ] = { 1, 0 };
    cube_cartesian* cart1              = cube_def_cart( cube, NDIMS2, dimv1, periodv1 );
    cube_cart_set_name( cart1, "Application topology 2" );
    /* set names for every dimension separately */
    cube_cart_set_dim_name( cart1, "Dimension 1", 0 );
    cube_cart_set_dim_name( cart1, "Dimension 2", 1 );

    long coordv0[ NDIMS2 ] = { 0, 1 };
    long coordv1[ NDIMS2 ] = { 1, 0 };
    cube_def_coords( cube, cart1, thrd0, coordv0 );
    cube_def_coords( cube, cart1, thrd1, coordv1 );





    /* define second topology*/
    long            dimv2[ NDIMS3 ]    = { 3, 3, 3, 3 };
    int             periodv2[ NDIMS3 ] = { 1, 0, 0, 0 };
    cube_cartesian* cart2              = cube_def_cart( cube, NDIMS3, dimv2, periodv2 );

    long coordv20[ NDIMS3 ] = { 0, 1, 0, 0 };
    long coordv21[ NDIMS3 ] = { 1, 0, 0, 0 };
    cube_def_coords( cube, cart2, thrd0, coordv20 );
    cube_def_coords( cube, cart2, thrd1, coordv21 );
    cube_cart_set_name( cart2, "Application topology 3" );


    /* define third topology*/
    /* cube supports any (limited by range of int) number of dimensions and elements in every dimension */
    long            dimv3[ NDIMS4 ]    = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
    int             periodv3[ NDIMS4 ] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    cube_cartesian* cart3              = cube_def_cart( cube, NDIMS4, dimv3, periodv3 );
    long            coordv32[ NDIMS4 ] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 };
    long            coordv33[ NDIMS4 ] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    cube_def_coords( cube, cart3, thrd0, coordv32 );
    cube_def_coords( cube, cart3, thrd1, coordv33 );
    cube_cart_set_name( cart3, "Huge topology" );


    /* create an array with #elements = #threads in cube report. In this case 2 */
    double sev1[ 2 ];

    /* write data for metric met0 */
    /* once started to write one metric, whole data for this metric should be written without interruption for
       writing of another metric. Otherwise created cube will have broken structure */

    /*  cnodes have to be writen in certain sequence, if Compressed cube should be written.

        Every metric, depending if one writes exclusive or inclusive values have to be written in specific order.

        Use call cube_dyn_array * sequence = cube_get_cnodes_for_metric(cube_t * cube, cube_metric* metric);
        to obtain the sequence of cnodes for every metric.
     */

    sev1[ 0 ] = 123.4;
    sev1[ 1 ] = 567.9;
    cube_write_sev_row( cube, met0, cnode0, sev1 );

    sev1[ 0 ] = 1123.4;
    sev1[ 1 ] = 2567.9;
    cube_write_sev_row( cube, met0, cnode1, sev1 );

    sev1[ 0 ] = -1123.4;
    sev1[ 1 ] = 3567.9;
    cube_write_sev_row( cube, met0, cnode2, sev1 );

    sev1[ 0 ] = -123.4;
    sev1[ 1 ] = -567.9;
    cube_write_sev_row( cube, met1, cnode0, sev1 );

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
    uint64_t sev2[ 2 ];
    sev2[ 0 ] = 23;
    sev2[ 1 ] = 26;
    cube_write_sev_row( cube, met2, cnode2, sev2 );


    printf( "Test file %s.cubex complete.\n", cubefile );

    /* Delete cube_t structure and release all memory*/
    cube_free( cube );

    /* finish */
    return 0;
}
