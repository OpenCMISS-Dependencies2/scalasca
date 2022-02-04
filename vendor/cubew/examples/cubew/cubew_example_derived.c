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
    char cubefile[ 20 ] = "simple-cube-derived";
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
    cube_metric* met0,
               * met1,
               * met2,
               * met3,
               * met4,
               * met5,
               * met6,
               * met7
    ;
    /* root metric Time */
    met0 = cube_def_met( cube, "Time", "time", "FLOAT", "sec", "",
                         "@mirror@patterns-2.1.html#execution",
                         "root node", NULL, CUBE_METRIC_EXCLUSIVE );

    /* A child of previos metric */
    met1 = cube_def_met( cube, "Prederived, exclusive", "preexcl", "FLOAT", "sec", "",
                         "http://www.cs.utk.edu/usr.html",
                         "2nd level", NULL, CUBE_METRIC_PREDERIVED_EXCLUSIVE );

    cube_metric_set_expression( met1, "metric::time()" );

    /* A child of previos metric */
    met2 = cube_def_met( cube, "Prederived, inclusive", "preincl", "FLOAT", "sec", "",
                         "http://www.cs.utk.edu/usr.html",
                         "2nd level", NULL, CUBE_METRIC_PREDERIVED_INCLUSIVE );
    cube_metric_set_expression( met2, "metric::time()" );

    /* A child of previos metric */
    met3 = cube_def_met( cube, "Prederived custom, exclusive", "preexclcustom1", "FLOAT", "sec", "",
                         "http://www.cs.utk.edu/usr.html",
                         "2nd level", NULL, CUBE_METRIC_PREDERIVED_EXCLUSIVE );
    cube_metric_set_expression( met3, "metric::time()" );
    cube_metric_set_aggr_expression( met3,
                                     CUBE_METRIC_AGGR_PLUS,
                                     "max(arg1, arg2)" );
    /* A child of previos metric */
    met7 = cube_def_met( cube, "Prederived custom, exclusive, over threads", "preexclcustom2", "FLOAT", "sec", "",
                         "http://www.cs.utk.edu/usr.html",
                         "2nd level", NULL, CUBE_METRIC_PREDERIVED_EXCLUSIVE );
    cube_metric_set_expression( met7, "metric::time()" );
    cube_metric_set_aggr_expression( met7,
                                     CUBE_METRIC_AGGR_AGGR,
                                     "max(arg1, arg2)" );


    /* A child of previos metric */
    met4 = cube_def_met( cube, "Prederived custom, inclusive", "preinclcustom", "FLOAT", "sec", "",
                         "http://www.cs.utk.edu/usr.html",
                         "2nd level", NULL, CUBE_METRIC_PREDERIVED_INCLUSIVE );
    cube_metric_set_expression( met4, "metric::time()" );
    cube_metric_set_aggr_expression( met4,
                                     CUBE_METRIC_AGGR_PLUS,
                                     "arg1*arg2" );
    cube_metric_set_aggr_expression( met4,
                                     CUBE_METRIC_AGGR_MINUS,
                                     "max(arg1, arg2)" );


    /* A child of previos metric */
    met5 = cube_def_met( cube, "Postderived, cacheable", "post", "FLOAT", "sec", "",
                         "http://www.cs.utk.edu/usr.html",
                         "2nd level", NULL, CUBE_METRIC_POSTDERIVED );
    cube_metric_set_expression( met5, "random(100)" );


    /* A child of previos metric */
    met6 = cube_def_met( cube, "Postderived, noncacheable", "postnoncache", "FLOAT", "sec", "",
                         "http://www.cs.utk.edu/usr.html",
                         "2nd level", NULL, CUBE_METRIC_POSTDERIVED );
    cube_metric_set_cacheable( met6, CUBE_FALSE );
    cube_metric_set_expression( met6, "random(10)" );


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
    cube_def_thrd( cube, "Thread 0<<iterator>>", 0, proc0 );
    cube_def_thrd( cube, "Thread 1<<solver>>", 1, proc1 );




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



    printf( "Test file %s.cubex complete.\n", cubefile );

    /* Delete cube_t structure and release all memory*/
    cube_free( cube );

    /* finish */
    return 0;
}
