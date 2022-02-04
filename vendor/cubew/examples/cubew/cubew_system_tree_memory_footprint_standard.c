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

#ifndef Nlocs
#define Nlocs ( 16 * 64 * 1024 )
#endif
// / Start point of this example.
int
main( int argc, char* argv[] )
{
    char cubefile[ 40 ] = "system_tree_mem_standard";
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
    printf( "Test file %s.cubex is being generated ...\n", cubefile );

    cube_def_met( cube, "Time", "time", "FLOAT", "sec", "",
                  "@mirror@patterns-2.1.html#execution",
                  "root node", NULL, CUBE_METRIC_EXCLUSIVE );

    /* define call tree */
    /* definition of regions in calltree */
    char*        mod = "/ICL/CUBE/example.c";
    cube_region* regn0;
    regn0 = cube_def_region( cube, "main", "main", "mpi", "barrier", 21, 100, "", "1st level", mod );

    cube_def_cnode_cs( cube, regn0, mod, 21, NULL );
    /* define location tree */
    cube_system_tree_node* top = cube_def_system_tree_node( cube, "Top", "", "machine", NULL );
    cube_location_group*   lg  = cube_def_location_group( cube, "LG", 0, CUBE_LOCATION_GROUP_TYPE_PROCESS, top );
    int                    i   = 0;
    for ( i = 0; i < Nlocs; i++ )
    {
        cube_def_location( cube, "Thread 0<<iterator>>", 0, CUBE_LOCATION_TYPE_CPU_THREAD, lg );
    }
    /* Delete cube_t structure and release all memory*/
    cube_free( cube );

    /* finish */
    return 0;
}
