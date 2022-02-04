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

char* stn_name  = "Top";
char* stn_descr = "";
char* stn_class = "machine";


void* user_ptr;

void
fill_stn( cube_system_tree_node_plain* stn_plain )
{
    stn_plain->name      = stn_name;
    stn_plain->stn_class = stn_class;
    stn_plain->desc      = stn_descr;
}

char* lg_name = "LG";
void
fill_lg( cube_location_group_plain* lg_plain )
{
    lg_plain->name = lg_name;
    lg_plain->rank = 0;
    lg_plain->type = CUBE_LOCATION_GROUP_TYPE_PROCESS;
}

char* loc_name = "Thread 0<<iterator>>";
void
fill_loc( cube_location_plain* loc_plain )
{
    loc_plain->name = loc_name;
    loc_plain->rank = 0;
    loc_plain->type = CUBE_LOCATION_TYPE_CPU_THREAD;
}



void
system_tree_writer_init( cube_system_tree_information* st_info, void* user_ptr )
{
    st_info->number_locations       = Nlocs;
    st_info->number_location_groups = 1;
    st_info->number_stn             = 1;
}

void
system_tree_writer_finish(  void* user_ptr )
{
}



static cube_system_tree_writer_state state;
static uint64_t                      loc_index;
void*                                user_ptr;

cube_system_tree_writer_state
system_tree_writer_step( cube_system_tree_node_plain* to_fill_stn, cube_location_group_plain* to_fill_lg, cube_location_plain* to_fill_loc, void* user_ptr )
{
    if ( state == CUBE_SYSTEM_TREE_WRITER_INIT )
    {
        state = CUBE_SYSTEM_TREE_WRITER_STN;
        fill_stn( to_fill_stn );
        return state;
    }
    if ( state == CUBE_SYSTEM_TREE_WRITER_STN )
    {
        state = CUBE_SYSTEM_TREE_WRITER_LG;
        fill_lg( to_fill_lg );
        return state;
    }
    if ( state == CUBE_SYSTEM_TREE_WRITER_LG )
    {
        state = CUBE_SYSTEM_TREE_WRITER_LOC;
        loc_index++;
        fill_loc( to_fill_loc );
        return state;
    }
    if ( ( state == CUBE_SYSTEM_TREE_WRITER_LOC ) && ( loc_index < Nlocs ) )
    {
        loc_index++;
        fill_loc( to_fill_loc );
        return state;
    }
    if ( ( state != CUBE_SYSTEM_TREE_WRITER_END ) && ( loc_index >= Nlocs ) )
    {
        state = CUBE_SYSTEM_TREE_WRITER_END;
        return state;
    }
    return CUBE_SYSTEM_TREE_WRITER_STOP;
}





int
main( int argc, char* argv[] )
{
    state     = CUBE_SYSTEM_TREE_WRITER_INIT;
    loc_index = 0;


    char cubefile[ 40 ] = "system_tree_mem_optimized";
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
    cube_system_tree_writer_initialize( cube, system_tree_writer_init, system_tree_writer_step, ( cube_system_tree_writer_driver )0, system_tree_writer_finish, user_ptr );

    /* generate header */
    printf( "Test file %s.cubex is being generated ...\n", cubefile );

    /* root metric Time */
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


    /* Delete cube_t structure and release all memory*/
    cube_free( cube );

    /* finish */
    return 0;
}
