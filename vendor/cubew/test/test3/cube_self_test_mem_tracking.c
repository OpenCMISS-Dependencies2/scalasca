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
 * \file cubew_example.c
   \brief A small example of using "libcubew4.a".
   One creates a cube file "example_derivated.cube", defined structure with two metrics, call tree, machine, cartesian topology
   and fills teh severity matrix (of those two metrics). It defined several derivated metrics.
   At the end the file will be written on the disk.
 */

#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "cubew_cube.h"




#define CNODES_NUMBER 10
#ifndef NWRITERS
#define NWRITERS 1
#endif

#ifndef NTHREADS
#define NTHREADS 64
#endif


#ifndef CNODES
#define CNODES 300
#endif


#ifndef COMPRESSION
#define COMPRESSION CUBE_FALSE
#endif

#define RAW_METRICS 3
#define DERIVED_METRICS 3
#define NMETRICS ( RAW_METRICS + DERIVED_METRICS )


static int current_random = 0; // uswd to create a stable random loooking sequence of numbers (used for creating of a calltree)
static int MAXDEPTH       = 3;


#define NDIMS 2


void
create_cube_structure( cube_t*,
                       cube_metric** metrics,
                       cube_cnode**  cnodes,
                       unsigned      Nthreads );

void
fill_cube_type_double_row( cube_type_double* row,
                           unsigned          size,
                           double            value );
void
fill_cube_type_int64_row( cube_type_int64* row,
                          unsigned         size,
                          int64_t          value );

void
create_calltree( cube_t*       cube,
                 cube_region** regn,
                 cube_cnode**  codes,
                 int*          idx,
                 int*          reg_idx,
                 cube_cnode*   parent,
                 int           level );
int
get_next_random();
void
set_seed_random( int s );
void
calculate_max_depth();



static size_t mymemory_size;
static void*  mymemory;
static void*  head;

void*
own_malloc( size_t size, void* usr_ptr )
{
    if ( ( char* )head - ( char* )mymemory + size  > mymemory_size )
    {
        return NULL;
    }
    void* _ptr = head;
    head = ( char* )head + size;
    return _ptr;
}

bool
my_pointer( void* ptr, void* usr_ptr )
{
    if (
        ( ptr <= head ) &&
        ( mymemory <= ptr )
        )
    {
        return 1;
    }

    return 0;
}



// / Start point of this example.
int
main( int argc, char* argv[] )
{
    mymemory_size = 1024llu * 1024llu * 24llu;

    mymemory = calloc( 1, mymemory_size );
    head     = mymemory;


    cubew_init_allocs( own_malloc, my_pointer, NULL );

    calculate_max_depth();

    // / actual number of writers might be different. For code it is transparent.
    unsigned ArraySize = CNODES;    // / In this pattern, number of cnodes

    unsigned Nthreads = NTHREADS;   // / In this pattern, number of threads


    // / Pattern has to know, which metrics are there to perform parallel writing.
    // / To destroy is only this aray, not metrics inside. Metrics are destroyed by "cube_free"
    cube_metric** metrics_vector = ( cube_metric** )calloc( NMETRICS, sizeof( cube_metric* ) );


    // / Pattern has to know, which cnodes are there to perform parallel writing.
    // / To destroy is only this aray, not cnodes inside. Cnodes are destroyed by "cube_free"
    cube_cnode** cnodes_vector = ( cube_cnode** )calloc( ArraySize, sizeof( cube_cnode* ) );




    char* cubefile = "example_ghost4remap";


    //    FILE* fp = fopen(cubefile, "w");
    cube_t* cube = cube_create( cubefile, CUBE_MASTER, CUBE_TRUE );
    if ( !cube )
    {
        fprintf( stderr, "cube_create failed!\n" );
        exit( 1 );
    }

    create_cube_structure( cube, metrics_vector, cnodes_vector, Nthreads );


    cube_type_double sev_double[ Nthreads ];
    cube_type_int64  sev_int64[ Nthreads ];



    /* generate header */
    cube_def_mirror( cube, "http://icl.cs.utk.edu/software/kojak/" );
    cube_def_mirror( cube, "http://www.fz-juelich.de/jsc/kojak/" );
    cube_def_attr( cube, "description", "an example of the cube with derivated metrics " );
    cube_def_attr( cube, "experiment time", "November 1st, 2004" );

    cube_enable_flat_tree( cube, CUBE_FALSE );




    unsigned mi = 0;
    for ( mi = 0; mi < NMETRICS; mi++ ) // / run over the metrics
    {
        if ( metrics_vector[ mi ] == NULL )
        {
            continue;
        }
        carray* sequence = cube_get_cnodes_for_metric( cube, metrics_vector[ mi ] );

        unsigned ci = 0;
        for ( ci = 0; ci < sequence->size; ci++ )
        {
            if ( metrics_vector[ mi ]->dtype_params->type == CUBE_DATA_TYPE_DOUBLE )
            {
                fill_cube_type_double_row( sev_double, Nthreads, ci + mi );
                cube_write_sev_row_of_cube_type_double( cube, metrics_vector[ mi ], ( cube_cnode* )( sequence->data )[ ci ], sev_double );
            }
            if ( metrics_vector[ mi ]->dtype_params->type == CUBE_DATA_TYPE_INT64 )
            {
                fill_cube_type_int64_row( sev_int64, Nthreads, ci + mi );
                cube_write_sev_row_of_cube_type_int64( cube, metrics_vector[ mi ], ( cube_cnode* )( sequence->data )[ ci ], sev_int64 );
            }
        }
    }

    printf( "Test file %s complete.\n", cubefile );


    free( metrics_vector );               /* content shouldn't be freed, it is done internally by cube */
    free( cnodes_vector );                /* content shouldn't be freed, it is done internally by cube */

    cube_free( cube );

    free( mymemory );

    return 0;
}






void
fill_cube_type_double_row( cube_type_double* row, unsigned size, double value )
{
    unsigned i = 0;
    for ( i = 0; i < size; i++ )
    {
        row[ i ].value = value + i;
    }
}

void
fill_cube_type_int64_row( cube_type_int64* row, unsigned size, int64_t value )
{
    unsigned i = 0;
    for ( i = 0; i < size; i++ )
    {
        row[ i ].value = value + i;
    }
}



void
create_cube_structure( cube_t* cube, cube_metric** metrics, cube_cnode** cnodes, unsigned Nthreads )
{
    /* generate header */
    cube_def_mirror( cube, "http://icl.cs.utk.edu/software/kojak/" );
    cube_def_mirror( cube, "http://www.fz-juelich.de/jsc/kojak/" );
    cube_def_attr( cube, "description", "a simple example" );
    cube_def_attr( cube, "experiment time", "November 1st, 2004" );

    /* define metric tree */
    metrics[ 0 ] = cube_def_met( cube, "Time", "time", "FLOAT", "sec", "",
                                 "@mirror@patterns-2.1.html#execution",
                                 "Total CPU allocation time", NULL, CUBE_METRIC_EXCLUSIVE );


    /* define call tree */
    char* mod = "/ICL/CUBE/example.c";
//      cube_region *regn0, *regn1, *regn2;

    cube_region** regn = ( cube_region** )calloc( 169, sizeof( cube_region* ) );

    char* names[ 169 ];

    names[ 0 ]   = "UNKNOWN";
    names[ 1 ]   = "TRACING";
    names[ 2 ]   = "MPI_Accumulate";
    names[ 3 ]   =  "MPI_Allgather";
    names[ 4 ]   =  "MPI_Allgatherv";
    names[ 5 ]   =  "MPI_Allreduce";
    names[ 6 ]   =  "MPI_Alltoall";
    names[ 7 ]   =   "MPI_Alltoallv";
    names[ 8 ]   =   "MPI_Alltoallw";
    names[ 9 ]   =   "MPI_Barrier";
    names[ 10 ]  =   "MPI_Bcast";
    names[ 11 ]  =   "MPI_Bsend";
    names[ 12 ]  =  "MPI_Bsend_init";
    names[ 13 ]  =   "MPI_Cancel";
    names[ 14 ]  =   "MPI_Cart_create";
    names[ 15 ]  =   "MPI_Comm_create";
    names[ 16 ]  =   "MPI_Comm_dup";
    names[ 17 ]  =   "MPI_Comm_free";
    names[ 18 ]  =   "MPI_Comm_group";
    names[ 19 ]  =    "MPI_Comm_remote_group";
    names[ 20 ]  =    "MPI_Comm_split";
    names[ 21 ]  =    "MPI_Exscan";
    names[ 22 ]  =    "MPI_File_close";
    names[ 23 ]  =    "MPI_File_delete";
    names[ 24 ]  =    "MPI_File_get_amode";
    names[ 25 ]  =    "MPI_File_get_atomicity";
    names[ 26 ]  =    "MPI_File_get_byte_offset";
    names[ 27 ]  =     "MPI_File_get_group";
    names[ 28 ]  =     "MPI_File_get_info";
    names[ 29 ]  =     "MPI_File_get_position";
    names[ 30 ]  =     "MPI_File_get_position_shared";
    names[ 31 ]  =    "MPI_File_get_size";
    names[ 32 ]  =    "MPI_File_get_type_extent";
    names[ 33 ]  =    "MPI_File_get_view";
    names[ 34 ]  =    "MPI_File_iread";
    names[ 35 ]  =   "MPI_File_iread_at";
    names[ 36 ]  =   "MPI_File_iread_shared";
    names[ 37 ]  =   "MPI_File_iwrite";
    names[ 38 ]  =    "MPI_File_iwrite_at";
    names[ 39 ]  =    "MPI_File_iwrite_shared";
    names[ 40 ]  =    "MPI_File_open";
    names[ 41 ]  =    "MPI_File_preallocated";
    names[ 42 ]  =    "MPI_File_read";
    names[ 43 ]  =    "MPI_File_read_all";
    names[ 44 ]  =    "MPI_File_read_all_begin";
    names[ 45 ]  =    "MPI_File_real_all_end";
    names[ 46 ]  =     "MPI_File_read_at";
    names[ 47 ]  =     "MPI_File_read_at_all";
    names[ 48 ]  =     "MPI_File_read_at_all_begin";
    names[ 49 ]  =     "MPI_File_read_at_all_end";
    names[ 50 ]  =    "MPI_File_read_ordered";
    names[ 51 ]  =    "MPI_File_read_ordered_begin";
    names[ 52 ]  =   "MPI_File_read_ordered_end";
    names[ 53 ]  =   "MPI_File_read_shared";
    names[ 54 ]  =   "MPI_File_seek";
    names[ 55 ]  =   "MPI_File_seek_shared";
    names[ 56 ]  =    "MPI_File_set_atomicity";
    names[ 57 ]  =    "MPI_File_set_info";
    names[ 58 ]  =    "MPI_File_set_size";
    names[ 59 ]  =    "MPI_File_set_view";
    names[ 60 ]  =    "MPI_File_sync";
    names[ 61 ]  =    "MPI_file_write";
    names[ 62 ]  =    "MPI_File_write_all";
    names[ 63 ]  =    "MPI_File_write_all_begin";
    names[ 64 ]  =     "MPI_File_write_all_end";
    names[ 65 ]  =     "MPI_File_write_at_all";
    names[ 66 ]  =     "MPI_File_write_at_all_begin";
    names[ 67 ]  =     "MPI_File_write_at_all_end";
    names[ 68 ]  =     "MPI_File_write_ordered";
    names[ 69 ]  =     "MPI_File_write_ordered_begin";
    names[ 70 ]  =    "MPI_File_write_ordered_end";
    names[ 71 ]  =    "MPI_File_write_shared";
    names[ 72 ]  =    "MPI_Finalize";
    names[ 73 ]  =    "MPI_Gather";
    names[ 74 ]  =   "mPI_Gatherv";
    names[ 75 ]  =   "MPI_Get";
    names[ 76 ]  =    "MPI_Graph_create";
    names[ 77 ]  =    "MPI_Group_difference";
    names[ 78 ]  =    "MPI_Group_excl";
    names[ 79 ]  =    "MPI_Group_ free";
    names[ 80 ]  =     "MPI_Group_incl";
    names[ 81 ]  =     "MPI_Group_intersection";
    names[ 82 ]  =     "MPI_Group_range_excl";
    names[ 83 ]  =     "MPI_Group_range_incl";
    names[ 84 ]  =     "MPI_Group_union";
    names[ 85 ]  =     "MPI_Ibsend";
    names[ 86 ]  =     "MPI_Init";
    names[ 87 ]  =     "MPI_Init_thread";
    names[ 88 ]  =     "MPI_Intercomm_create";
    names[ 89 ]  =     "MPI_Intercomm_merge";
    names[ 90 ]  =     "MPI_Irecv";
    names[ 91 ]  =     "MPI_Irsend";
    names[ 92 ]  =     "MPI_Isend";
    names[ 93 ]  =     "MPI_Issend";
    names[ 94 ]  =     "MPI_Probe";
    names[ 95 ]  =     "MPI_Put";
    names[ 96 ]  =     "MPI_Recv";
    names[ 97 ]  =     "MPI_Recv_init";
    names[ 98 ]  =     "MPI_Reduce";
    names[ 99 ]  =     "MPI_Reduce_scatter";
    names[ 100 ] =     "MPI_Request_free";
    names[ 101 ] =     "MPI_Rsend";
    names[ 102 ] =     "MPI_Rsend_init";
    names[ 103 ] =     "MPI_Scan";
    names[ 104 ] =     "MPI_Scatter";
    names[ 105 ] =     "MPI_Scatterv";
    names[ 106 ] =      "MPI_Send";
    names[ 107 ] =      "MPI_Send_init";
    names[ 108 ] =     "MPI_Sendrecv";
    names[ 109 ] =      "MPI_Sendrecv_replace";
    names[ 110 ] =      "MPI_Ssend";
    names[ 111 ] =      "MPI_Ssend_init";
    names[ 112 ] =      "MPI_Start";
    names[ 113 ] =      "MPI_Startall";
    names[ 114 ] =      "MPI_Test";
    names[ 115 ] =      "MPI_Testall";
    names[ 116 ] =      "MPI_Testany";
    names[ 117 ] =      "MPI_Testsome";
    names[ 118 ] =      "MPI_Wait";
    names[ 119 ] =      "MPI_Waitall";
    names[ 120 ] =      "MPI_Waitany";
    names[ 121 ] =      "MPI_Waitsome";
    names[ 122 ] =      "MPI_Win_complete";
    names[ 123 ] =      "MPI_Win_create";
    names[ 124 ] =      "MPI_Win_fence";
    names[ 125 ] =     "MPI_Win_free";
    names[ 126 ] =      "MPI_Win_lock";
    names[ 127 ] =      "MPI_Win_post";
    names[ 128 ] =     "MPI_Win_start";
    names[ 129 ] =     "MPI_Win_test";
    names[ 130 ] =     "MPI_Win_unlock";
    names[ 131 ] =     "MPI_Win_wait";
    names[ 132 ] =     "PARALLEL";
    names[ 133 ] =     "driver";
    names[ 134 ] =     "task_init";
    names[ 135 ] =     "bcast_init";
    names[ 136 ] =     "barriere_sync";
    names[ 137 ] =    "read_input";
    names[ 138 ] =     "bcast_real";
    names[ 139 ] =     "decomp";
    names[ 140 ] =     "inner_auto";
    names[ 141 ] =     "inner";
    names[ 142 ] =     "initialize";
    names[ 143 ] =     "initxs";
    names[ 144 ] =     "initsnc";
    names[ 145 ] =     "octant";
    names[ 146 ] =     "initgeom";
    names[ 147 ] =     "timers";
    names[ 148 ] =     "its=1                    ";
    names[ 149 ] =     "source";
    names[ 150 ] =     "sweep";
    names[ 151 ] =      "rcv_real";
    names[ 152 ] =     "snd_real";
    names[ 153 ] =     "global_init_sum";
    names[ 154 ] =     "flux_err";
    names[ 155 ] =      "global_real_max";
    names[ 156 ] =      "its=2                        ";
    names[ 157 ] =      "its=3                        ";
    names[ 158 ] =      "its=4                        ";
    names[ 159 ] =      "its=5                        ";
    names[ 160 ] =     "its=6                        ";
    names[ 161 ] =     "its=7                        ";
    names[ 162 ] =     "its=8                        ";
    names[ 163 ] =   "its=9                        ";
    names[ 164 ] =    "its=10                        ";
    names[ 165 ] =    "its=11                       ";
    names[ 166 ] =      "its=12                        ";
    names[ 167 ] =     "global_real_sum";
    names[ 168 ] =     "task_end";


    for ( unsigned i = 0; i < 169; i++ )
    {
        char* descr = "";
        if ( i > 1 && i < 132  )
        {
            descr = "MPI";
        }
        if ( i == 1 || i == 132 )
        {
            descr = "EPIK";
        }
        if ( i > 132 )
        {
            descr = "USR";
        }
        regn[ i ] = cube_def_region( cube, names[ i ], names[ i ], "pthread", "p2p", -1, -1, "", descr, mod );
    }

    //     create calltree
    srand( 0 );
    int idx        = 0;
    int region_idx = 0;
    create_calltree( cube, regn, cnodes, &idx, &region_idx,  NULL, 0 );


    /* define location tree */
    cube_machine* mach = cube_def_mach( cube, "MSC", "" );
    cube_node*    node = cube_def_node( cube, "Athena", mach );

    cube_thread** thrds = ( cube_thread** )calloc( Nthreads, sizeof( cube_thread* ) );
    for ( unsigned i = 0; i < Nthreads; i++ )
    {
        cube_process* proc0 = cube_def_proc( cube, "Process 0", i, node );
        thrds[ i ] = cube_def_thrd( cube, "Thread 0", i, proc0 );
    }


    free( regn );




    /* define first topology */
    long            dimv0[ NDIMS ]    = { 5, 5 };
    int             periodv0[ NDIMS ] = { 1, 0 };
    cube_cartesian* cart0             = cube_def_cart( cube, NDIMS, dimv0, periodv0 );
    cube_cart_set_name( cart0, "My Topology 1" );

    long coordv[ NDIMS ] = { 0, 0 };
    cube_def_coords( cube, cart0, thrds[ 1 ], coordv );

    /* define second topology */
    long            dimv1[ NDIMS ]    = { 3, 3 };
    int             periodv1[ NDIMS ] = { 1, 0 };
    cube_cartesian* cart1             = cube_def_cart( cube, NDIMS, dimv1, periodv1 );
    cube_cart_set_name( cart1, "Application topology 3" );

    long coordv0[ NDIMS ] = { 0, 1 };
    long coordv1[ NDIMS ] = { 1, 0 };
    cube_def_coords( cube, cart1, thrds[ 0 ], coordv0 );
    cube_def_coords( cube, cart1, thrds[ 1 ], coordv1 );





    /* define second topology*/
    long            dimv2[ 4 ]    = { 3, 3, 3, 3 };
    int             periodv2[ 4 ] = { 1, 0, 0, 0 };
    cube_cartesian* cart2         = cube_def_cart( cube, 4, dimv2, periodv2 );

    long coordv20[ 4 ] = { 0, 1, 0, 0 };
    long coordv21[ 4 ] = { 1, 0, 0, 0 };
    cube_def_coords( cube, cart2, thrds[ 2 ], coordv20 );
    cube_def_coords( cube, cart2, thrds[ 3 ], coordv21 );
    cube_cart_set_name( cart2, "Second" );


    /* define third topology*/
    long            dimv3[ 14 ]    = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
    int             periodv3[ 14 ] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    cube_cartesian* cart3          = cube_def_cart( cube, 14, dimv3, periodv3 );
    long            coordv32[ 14 ] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 };
    long            coordv33[ 14 ] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    cube_def_coords( cube, cart3, thrds[ 5 ], coordv32 );
    cube_def_coords( cube, cart3, thrds[ 10 ], coordv33 );
    cube_cart_set_name( cart3, "Third" );






    cube_set_statistic_name( cube, "mystat" );

    cube_set_metrics_title( cube, "Metric tree (QMCD App <ver.34>)" );
    cube_set_calltree_title( cube, "Calltree (serial run, #2345)" );
    cube_set_systemtree_title( cube, "System  ( Linux cluster < 43 & 23 >)" );

    free( thrds );
}




int
get_next_random()
{
    current_random = ( current_random + 5 ) % 13;
    return current_random;
}

void
set_seed_random( int s )
{
    current_random = s;
}



void
create_calltree( cube_t* cube, cube_region** regn, cube_cnode** cnodes, int* idx, int* region_idx,  cube_cnode* parent, int level )
{
    if ( ( *idx ) >= CNODES )
    {
        return;
    }
    if ( level >= MAXDEPTH )
    {
        return;
    }
    int _num_children = get_next_random();
    for ( unsigned i = 0; i < _num_children; i++ )
    {
        unsigned rand_reg = *region_idx;
        *region_idx = ( ( *region_idx + 1 ) % 169 );      // cyclic selection of regions
        cube_cnode* cnode = cube_def_cnode( cube, regn[ rand_reg ], parent );
        cnodes[ *idx ] = cnode;
        ( *idx )++;
        create_calltree( cube, regn, cnodes, idx, region_idx, cnode, level + 1 );
        if ( ( *idx ) >= CNODES )
        {
            return;                            // for the case, inside of those function was reached the end of array
        }
    }
}





void
calculate_max_depth()
{
    MAXDEPTH = ( int )( log( CNODES ) / log( 13 ) ) + 2;
}
