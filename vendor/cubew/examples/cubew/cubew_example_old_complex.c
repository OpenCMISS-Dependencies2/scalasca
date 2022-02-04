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
 * \file cubew_example_mpi_complex.c
   \brief Example of using "libcube4w.a" in a parallel way.
   One creates a cube file "example.cubex", defined structure of metrics, call tree, machine, cartesian topology
   and fills the cube withdata.
   Call *_write_all_rows writes rows in parallel to the cube repors directly on disk.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <mpi.h>
#include "cubew_cube.h"
#include "cubew_services.h"
#include "cubew_types.h"

/* used in static array initialization. see below by topology definition*/
#define NDIMS1 2
#define NDIMS2 2
#define NDIMS3 4
#define NDIMS4 14

#ifndef MAXNTHREADS
#define MAXNTHREADS 6
#endif

#ifndef NMETRICS
#define NMETRICS 7
#endif

#ifndef CNODES
#define CNODES 100
#endif

static int current_random = 0; // uswd to create a stable random loooking sequence of numbers (used for creating of a calltree)
static int MAXDEPTH       = 6;

void
create_system_tree( cube_t*        cube,
                    cube_process** process,
                    cube_thread**  thread,
                    cube_node*     node,
                    int            size );

void
fill_cube_type_double_row( double*  row,
                           unsigned size,
                           double   value );
void
write_doubles_line_by_line( cube_t*      cube,
                            cube_metric* met,
                            int          nthreads,
                            int          totnthreads,
                            int          ncallpaths,
                            char*        bitmask,
                            int*         counts,
                            int*         displs,
                            carray*      sequence );

void
fill_cube_type_int_row( int64_t* row,
                        unsigned size,
                        int      value );
void
write_int_line_by_line( cube_t*      cube,
                        cube_metric* met,
                        int          nthreads,
                        int          totnthreads,
                        int          ncallpaths,
                        char*        bitmask,
                        int*         counts,
                        int*         displs,
                        carray*      sequence );

void
fill_cube_type_tau_atomic_row( cube_type_tau_atomic* row,
                               unsigned              size,
                               unsigned              value );
void
write_tau_atomic_line_by_line( cube_t*      cube,
                               cube_metric* met,
                               int          nthreads,
                               int          totnthreads,
                               int          ncallpaths,
                               char*        bitmask,
                               int*         counts,
                               int*         displs,
                               carray*      sequence );

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
int
calcaulate_sum2( int start,
                 int finish );
char*
create_bitmask( int   cnodes,
                char* bitmask );

// / Start point of this example.
int
main( int argc, char* argv[] )
{
    int rank;
    int size;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    calculate_max_depth();

    char cubefile[] = "mpi-complex-cube-old";
    /* Create a cube with name cubefile.
        Root uses CUBE_MASTER all others CUBE_SLAVE.
     */

    int nthreads      = ( rank ) % ( MAXNTHREADS )+1;
    int thrdsincycle  = ( MAXNTHREADS )*( MAXNTHREADS + 1 ) / 2;
    int offset        = ( rank ) / ( MAXNTHREADS )*thrdsincycle  + ( nthreads - 1 ) * nthreads / 2;
    int totalnthreads =  ( ( size / ( MAXNTHREADS ) ) * thrdsincycle ) + ( ( size % MAXNTHREADS ) * ( ( size % MAXNTHREADS ) + 1 ) ) / 2;

    int* counts = malloc( size * sizeof( int ) );
    MPI_Gather( &nthreads, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD );
    int* displs = malloc( size * sizeof( int ) );
    MPI_Gather( &offset, 1, MPI_INT, displs, 1, MPI_INT, 0, MPI_COMM_WORLD );

    printf( "size: %d rank: %d, nthrds: %d, offset: %d, totalthreads: %d\n", size, rank, nthreads, offset, totalnthreads );

    cube_t* cube = 0;
    if ( rank == 0 )
    {
        cube = cube_create( cubefile, CUBE_MASTER, CUBE_FALSE );
    }

    if ( !cube )
    {
        fprintf( stderr, "cube_create failed!\n" );
        exit( 1 );
    }
    /* Six metrics */
    cube_metric** met_vector = ( cube_metric** )calloc( NMETRICS, sizeof( cube_metric* ) );
    carray*       sequence   = malloc( CNODES * sizeof( carray ) );

    if ( rank == 0 )
    {
        /* generate header */
        cube_def_mirror( cube, "http://www.fz-juelich.de/jsc/scalasca/" );
        cube_def_attr( cube, "description", "A simple example of Cube report" );
        cube_def_attr( cube, "experiment time", "November 1st, 2004" );

        printf( "Test file %s.cubex is being generated ...\n", cubefile );


        /* root metric Time */
        met_vector[ 0 ] = cube_def_met( cube, "Time", "time", "FLOAT", "sec", "",
                                        "@mirror@patterns-2.1.html#execution",
                                        "root node", NULL, CUBE_METRIC_INCLUSIVE );
        /* A metric of visits */
        met_vector[ 1 ] = cube_def_met( cube, "Visits", "visits", "INTEGER", "occ", "",
                                        "http://www.cs.utk.edu/usr.html",
                                        "Number of visits", NULL,  CUBE_METRIC_EXCLUSIVE );
        /* Another root metric */
        met_vector[ 2 ] = cube_def_met( cube, "P2P bytes sent", "bytes_sent", "INTEGER", "bytes", "",
                                        "http://www.cs.utk.edu/sys.html",
                                        "Number of bytes sent in point-to-point operations", NULL,  CUBE_METRIC_EXCLUSIVE );

        met_vector[ 3 ] = cube_def_met( cube, "P2P bytes received", "bytes_rcvd", "INTEGER", "bytes", "",
                                        "http://www.cs.utk.edu/sys.html",
                                        "Number of bytes received in point-to-point operations", NULL,  CUBE_METRIC_EXCLUSIVE );

        met_vector[ 4 ] = cube_def_met( cube, "Tasks-time", "time in tasks", "MAXDOUBLE", "sec", "",
                                        "http://www.cs.utk.edu/sys.html",
                                        "Time spent in tasks.", NULL,  CUBE_METRIC_EXCLUSIVE );

        met_vector[ 5 ] = cube_def_met( cube, "Counters", "number of counters", "TAU_ATOMIC", "occ", "",
                                        "http://www.cs.utk.edu/sys.html",
                                        "Number of counters", NULL, CUBE_METRIC_EXCLUSIVE );

        met_vector[ 6 ] = cube_def_met( cube, "Memory allocation", "memoryallocation", "TAU_ATOMIC", "tau_statistics", "",
                                        "@mirror@patterns-2.1.html#memoryallocation",
                                        "Total allocated memory", NULL, CUBE_METRIC_EXCLUSIVE );


        /* define call tree */
        /* definition of regions in calltree */
        char* mod = "/ICL/CUBE/example.c";

        cube_region** regn = ( cube_region** )calloc( 169, sizeof( cube_region* ) );


        char* names[ 169 ] = {
            "UNKNOWN",                        "TRACING",                                "MPI_Accumulate",                                "MPI_Allgather",                                         "MPI_Allgatherv",                                               "MPI_Allreduce",                                                     "MPI_A8lltoall",                                                            "MPI_Alltoallv",                                                                     "MPI_Alltoallw",                                                                             "MPI_Barrier",                                                                                       "MPI_Bcast",                                                                                             "MPI_Bsend",
            "MPI_Bsend_init",                 "MPI_Cancel",                             "MPI_Cart_create",                               "MPI_Comm_create",                                       "MPI_Comm_dup",                                                 "MPI_Comm_free",                                                     "MPI_Comm_group",                                                           "MPI_Comm_remote_group",                                                             "MPI_Comm_split",                                                                            "MPI_Exscan",                                                                                        "MPI_File_close",                                                                                        "MPI_File_delete",
            "MPI_File_get_amode",             "MPI_File_get_atomicity",                 "MPI_File_get_byte_offset",                      "MPI_File_get_group",                                    "MPI_File_get_info",                                            "MPI_File_get_position",                                             "MPI_File_get_position_shared",                                             "MPI_File_get_size",
            "MPI_File_get_type_extent",       "MPI_File_get_view",                      "MPI_File_iread",                                "MPI_File_iread_at",                                     "MPI_File_iread_shared",                                        "MPI_File_iwrite",                                                   "MPI_File_iwrite_at",                                                       "MPI_File_iwrite_shared",                                                            "MPI_File_open",                                                                             "MPI_File_preallocated",
            "MPI_File_read",                  "MPI_File_read_all",                      "MPI_File_read_all_begin",                       "MPI_File_real_all_end",                                 "MPI_File_read_at",                                             "MPI_File_read_at_all",                                              "MPI_File_read_at_all_begin",                                               "MPI_File_read_at_all_end",                                                          "MPI_File_read_ordered",
            "MPI_File_read_ordered_begin",    "MPI_File_read_ordered_end",              "MPI_File_read_shared",                          "MPI_File_seek",                                         "MPI_File_seek_shared",                                         "MPI_File_set_atomicity",                                            "MPI_File_set_info",                                                        "MPI_File_set_size",                                                                 "MPI_File_set_view",
            "MPI_File_sync",                  "MPI_file_write",                         "MPI_File_write_all",                            "MPI_File_write_all_begin",                              "MPI_File_write_all_end",                                       "MPI_File_write_at_all",                                             "MPI_File_write_at_all_begin",                                              "MPI_File_write_at_all_end",                                                         "MPI_File_write_ordered",
            "MPI_File_write_ordered_begin",   "MPI_File_write_ordered_end",             "MPI_File_write_shared",                         "MPI_Finalize",                                          "MPI_Gather",                                                   "mPI_Gatherv",                                                       "MPI_Get",                                                                  "MPI_Graph_create",                                                                  "MPI_Group_difference",                                                                      "MPI_Group_excl",                                                                                    "MPI_Group_ free",
            "MPI_Group_incl",                 "MPI_Group_intersection",                 "MPI_Group_range_excl",                          "MPI_Group_range_incl",                                  "MPI_Group_union",                                              "MPI_Ibsend",                                                        "MPI_Init",                                                                 "MPI_Init_thread",                                                                   "MPI_Intercomm_create",                                                                      "MPI_Intercomm_merge",                                                                               "MPI_Irecv",
            "MPI_Irsend",                     "MPI_Isend",                              "MPI_Issend",                                    "MPI_Probe",                                             "MPI_Put",                                                      "MPI_Recv",                                                          "MPI_Recv_init",                                                            "MPI_Reduce",                                                                        "MPI_Reduce_scatter",                                                                        "MPI_Request_free",                                                                                  "MPI_Rsend",                                                                                             "MPI_Rsend_init",                                                                                            "MPI_Scan",                                                                                                     "MPI_Scatter",                                                                                                     "MPI_Scatterv",
            "MPI_Send",                       "MPI_Send_init",                          "MPI_Sendrecv",                                  "MPI_Sendrecv_replace",                                  "MPI_Ssend",                                                    "MPI_Ssend_init",                                                    "MPI_Start",                                                                "MPI_Startall",                                                                      "MPI_Test",                                                                                  "MPI_Testall",                                                                                       "MPI_Testany",                                                                                           "MPI_Testsome",                                                                                              "MPI_Wait",                                                                                                     "MPI_Waitall",
            "MPI_Waitany",                    "MPI_Waitsome",                           "MPI_Win_complete",                              "MPI_Win_create",                                        "MPI_Win_fence",                                                "MPI_Win_free",                                                      "MPI_Win_lock",                                                             "MPI_Win_post",                                                                      "MPI_Win_start",                                                                             "MPI_Win_test",                                                                                      "MPI_Win_unlock",                                                                                        "MPI_Win_wait",                                                                                              "PARALLEL",                                                                                                     "driver",
            "task_init",                      "bcast_init",                             "barriere_sync",                                 "read_input",                                            "bcast_real",                                                   "decomp",                                                            "inner_auto",                                                               "inner",                                                                             "initialize",                                                                                "initxs",                                                                                            "initsnc",                                                                                               "octant",                                                                                                    "initgeom",                                                                                                     "timers",                                                                                                          "its=1                    ",                                                                                                         "source",   "sweep",
            "rcv_real",                       "snd_real",                               "global_init_sum",                               "flux_err",                                              "global_real_max",
            "its=2                        ",
            "its=3                        ",
            "its=4                        ",
            "its=5                        ",
            "its=6                        ",
            "its=7                        ",
            "its=8                        ",
            "its=9                        ",
            "its=10                        ",
            "its=11                       ",
            "its=12                        ", "global_real_sum",                        "task_end"
        };

        unsigned i;
        for ( i = 0; i < 169; i++ )
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
            regn[ i ] = cube_def_region( cube, names[ i ], names[ i ], "mpi", "barrier",  -1, -1, "", descr, mod );
        }


        /* actual definition of calltree */
        srand( 0 );
        cube_cnode** cnodes_vector = ( cube_cnode** )calloc( CNODES, sizeof( cube_cnode* ) );
        int          idx           = 0;
        int          region_idx    = 0;
        create_calltree( cube, regn, cnodes_vector, &idx, &region_idx,  NULL, 0 );
        /* define location tree */
        cube_machine*  mach             = cube_def_mach( cube, "MSC<<juelich>>", "" );
        cube_node*     node             = cube_def_node( cube, "Athena<<juropa>>", mach );
        cube_process** processes_vector = ( cube_process** )calloc( size, sizeof( cube_process* ) );
        cube_thread**  threads_vector   = ( cube_thread** )calloc( size * totalnthreads, sizeof( cube_thread* ) );
        /* Define processes and threads */
        create_system_tree( cube, processes_vector, threads_vector, node, size );
        free( regn );

        cube_set_statistic_name( cube, "mystat" );

        cube_set_metrics_title( cube, "Metric tree (QMCD App <ver.34>)" );
        cube_set_calltree_title( cube, "Calltree (serial run, #2345)" );
        cube_set_systemtree_title( cube, "System  ( Linux cluster < 43 & 23 >)" );


        /* generate header */
        cube_def_mirror( cube, "http://icl.cs.utk.edu/software/kojak/" );
        cube_def_mirror( cube, "http://www.fz-juelich.de/jsc/kojak/" );
        cube_def_attr( cube, "description", "a simple example" );
        cube_def_attr( cube, "experiment time", "November 1st, 2004" );
        cube_enable_flat_tree( cube, CUBE_FALSE );
    }


    MPI_Barrier( MPI_COMM_WORLD );

    int   n_callpaths;
    int   bitmask_size = ( CNODES + 7 ) / 8;
    char* bitmask      = malloc( bitmask_size * sizeof( char ) );

    /* First metric */
    write_doubles_line_by_line( cube, met_vector[ 0 ], nthreads, totalnthreads, CNODES, bitmask, counts, displs, sequence );

    /* Second metric */
    write_int_line_by_line( cube, met_vector[ 1 ], nthreads, totalnthreads, CNODES, bitmask, counts, displs, sequence );
    printf( "2\n" );
    MPI_Barrier( MPI_COMM_WORLD );

    /* Third metric 5% sparse */
    n_callpaths = CNODES * 0.05;
    create_bitmask( n_callpaths, bitmask );
    if ( rank == 0 )
    {
        cube_set_known_cnodes_for_metric( cube, met_vector[ 2 ], bitmask );
    }
    write_int_line_by_line( cube, met_vector[ 2 ], nthreads, totalnthreads, n_callpaths, bitmask, counts, displs, sequence );
    printf( "3\n" );
    MPI_Barrier( MPI_COMM_WORLD );

    /* Fourth metric 5% sparse */
    if ( rank == 0 )
    {
        cube_set_known_cnodes_for_metric( cube, met_vector[ 3 ], bitmask );
    }
    write_int_line_by_line( cube, met_vector[ 3 ], nthreads, totalnthreads, n_callpaths, bitmask, counts, displs, sequence );
    printf( "4\n" );

    /* Fifth metric 20% sparse */
    n_callpaths = CNODES * 0.2;
    create_bitmask( n_callpaths, bitmask );
    if ( rank == 0 )
    {
        cube_set_known_cnodes_for_metric( cube, met_vector[ 4 ], bitmask );
    }
    write_doubles_line_by_line( cube, met_vector[ 4 ], nthreads, totalnthreads, n_callpaths, bitmask, counts, displs, sequence );
    printf( "5\n" );

    /* sixth metric */
    write_tau_atomic_line_by_line( cube, met_vector[ 5 ], nthreads, totalnthreads, CNODES, bitmask, counts, displs, sequence );
    printf( "6\n" );
    /* Seventh metric 2% sparse */
    n_callpaths = CNODES * 0.02;
    create_bitmask( n_callpaths, bitmask );
    if ( rank == 0 )
    {
        cube_set_known_cnodes_for_metric( cube, met_vector[ 6 ], bitmask );
    }
    write_tau_atomic_line_by_line( cube, met_vector[ 6 ], nthreads, totalnthreads, n_callpaths, bitmask, counts, displs, sequence );



    printf( "7\n" );

    free( bitmask );
    free( counts );
    free( displs );
    free( sequence );
    MPI_Barrier( MPI_COMM_WORLD );
    if ( rank == 0 )
    {
        printf( "Test file %s.cubex complete.\n", cubefile );
    }

    /* Delete cube_t structure and release all memory*/
    if ( rank == 0 )
    {
        printf( "before" );
        cube_free( cube );
        printf( "after" );
        free( met_vector );
    }



    MPI_Finalize();
    /* finish */
    return 0;
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
create_system_tree( cube_t* cube, cube_process** process, cube_thread** thread, cube_node* node, int size )
{
    unsigned i, j, nthrds, totthreads;
    nthrds     = 1;
    totthreads = 0;
    for ( i = 0; i < size; i++ )
    {
        char          proc[] = "Process";
        char          thrd[] = "Thread";
        cube_process* proc1  = cube_def_proc( cube, proc, i, node );
        process[ i ] = proc1;
        for ( j = 0; j < nthrds; j++ )
        {
            //printf("id %d, i %d, j %d, threads %d \n", totthreads, i, j, nthrds);
            cube_thread* thread1 = cube_def_thrd( cube, thrd, totthreads, process[ i ] );
            thread[ totthreads ] = thread1;
            totthreads++;
        }

        nthrds = ( nthrds == MAXNTHREADS ) ? 1 : nthrds + 1;
    }
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
    int      _num_children = get_next_random();
    unsigned i;
    for ( i = 0; i < _num_children; i++ )
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

void
fill_cube_type_double_row( double* row, unsigned size, double value )
{
    unsigned i = 0;
    for ( i = 0; i < size; i++ )
    {
        row[ i ] = value + i;
    }
}

void
fill_cube_type_int_row( int64_t* row, unsigned size, int value )
{
    unsigned i = 0;
    for ( i = 0; i < size; i++ )
    {
        row[ i ] = value + i;
    }
}

void
fill_cube_type_tau_atomic_row( cube_type_tau_atomic* row, unsigned size, unsigned value )
{
    unsigned i = 0;
    for ( i = 0; i < size; i++ )
    {
        double A = ( double )( value + i );

        int _a = ( int )floor( A / 2. );
        int a_ = ( int )ceil( A / 2. );



        row[ i ].N    = ( _a + a_ + 1 );
        row[ i ].Min  = -_a;
        row[ i ].Max  = a_;
        row[ i ].Sum  = ( _a != a_ ) ? a_ : 0;
        row[ i ].Sum2 = calcaulate_sum2( _a, a_ );

        if ( i <= 3 )
        {
            printf( "%u %f %f %f %f\n", row[ i ].N, row[ i ].Min, row[ i ].Max, row[ i ].Sum, row[ i ].Sum2 );
            printf( "%d\n", ( char* )&( row[ i + 1 ].Min ) - ( char* )&( row[ i ].Min ) );
        }
    }
}

int
calcaulate_sum2( int start, int finish )
{
    int sum = 0;
    int i   = start;
    for ( i = start; i <= finish; i++ )
    {
        sum += i * i;
    }
    return sum;
}

char*
create_bitmask( int cnodes, char* bitmask )
{
    memset( bitmask, 0x00, ( CNODES + 7 ) / 8 );
    int i = 0;
    for ( i = 0; i < cnodes; i = i + 1 )
    {
        __cube_set_bit( bitmask, i );
    }
    //cube_set_known_cnodes_for_metric( cube, met, bitmask );
    return bitmask;
}

void
write_doubles_line_by_line( cube_t* cube, cube_metric* met, int nthreads, int totnthreads, int ncallpaths, char* bitmask, int* counts, int* displs, carray* sequence )
{
    unsigned* idtocid = malloc( CNODES * sizeof( unsigned ) );
    //carray* sequence = malloc(CNODES * sizeof( carray ) );

    if ( cube->cube_flavour == CUBE_MASTER )
    {
        sequence = cube_get_cnodes_for_metric( cube, met );
        unsigned i = 0;
        for ( i = 0; i < sequence->size; i++ )
        {
            idtocid[ i ] = ( ( cube_cnode* )( sequence->data )[ i ] )->id;
            printf( "%u\n", idtocid[ i ] );
        }
    }
    MPI_Bcast( idtocid, CNODES, MPI_UNSIGNED, 0, MPI_COMM_WORLD );

    double*  local  = malloc( nthreads * sizeof( double ) );
    double*  global = malloc( totnthreads * sizeof( double ) );
    unsigned ci;
    int      w = 0;
    for ( ci = 0; ci < ncallpaths; ci++ )
    {
        if ( CNODES != ncallpaths && !__cube_is_bit_set( bitmask, ci ) )
        {
            continue;
        }

        fill_cube_type_double_row( local, nthreads, idtocid[ ci ] + 1 );
        MPI_Gatherv( local, nthreads, MPI_DOUBLE, global, counts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD );
        if ( cube->cube_flavour == CUBE_MASTER )
        {
            cube_write_sev_row_of_doubles( cube, met, ( cube_cnode* )( sequence->data[ ci ] ), global );
        }

        w++;
    }
    //free( sequence );
    free( idtocid );
    free( local );
    free( global );
}

void
write_int_line_by_line( cube_t* cube, cube_metric* met, int nthreads, int totnthreads, int ncallpaths, char* bitmask, int* counts, int* displs, carray* sequence )
{
    unsigned* idtocid = malloc( CNODES * sizeof( unsigned ) );
    //carray* sequence = malloc(CNODES * sizeof( carray ) );
    if ( cube->cube_flavour == CUBE_MASTER )
    {
        sequence = cube_get_cnodes_for_metric( cube, met );
        unsigned i = 0;
        for ( i = 0; i < sequence->size; i++ )
        {
            idtocid[ i ] = ( ( cube_cnode* )( sequence->data )[ i ] )->id;
            printf( "%u\n", idtocid[ i ] );
        }
    }
    MPI_Bcast( idtocid, CNODES, MPI_UNSIGNED, 0, MPI_COMM_WORLD );

    int64_t* local  = malloc( nthreads * sizeof( int64_t ) );
    int64_t* global = malloc( totnthreads * sizeof( int64_t ) );
    unsigned ci;
    int      w = 0;
    for ( ci = 0; ci < ncallpaths; ci++ )
    {
        if ( CNODES != ncallpaths && !__cube_is_bit_set( bitmask, ci ) )
        {
            continue;
        }

        fill_cube_type_int_row( local, nthreads, idtocid[ ci ] + 1 );
        MPI_Gatherv( local, nthreads, MPI_INT64_T, global, counts, displs, MPI_INT64_T, 0, MPI_COMM_WORLD );
        if ( cube->cube_flavour == CUBE_MASTER )
        {
            cube_write_sev_row_of_int64( cube, met, ( cube_cnode* )( sequence->data[ ci ] ), global );
        }

        w++;
    }
    free( idtocid );
    //free ( sequence );
    free( local );
    free( global );
}

void
write_tau_atomic_line_by_line( cube_t* cube, cube_metric* met,  int nthreads, int totnthreads, int ncallpaths, char* bitmask, int* counts, int* displs, carray* sequence )
{
    unsigned* idtocid = malloc( CNODES * sizeof( unsigned ) );
    //carray* sequence = malloc(CNODES * sizeof( carray ) );
    if ( cube->cube_flavour == CUBE_MASTER )
    {
        sequence = cube_get_cnodes_for_metric( cube, met );
        unsigned i = 0;
        for ( i = 0; i < sequence->size; i++ )
        {
            idtocid[ i ] = ( ( cube_cnode* )( sequence->data )[ i ] )->id;
            printf( "%u\n", idtocid[ i ] );
        }
    }
    MPI_Bcast( idtocid, CNODES, MPI_UNSIGNED, 0, MPI_COMM_WORLD );

    cube_type_tau_atomic* local  = malloc( nthreads * sizeof( cube_type_tau_atomic ) );
    cube_type_tau_atomic* global = malloc( totnthreads * sizeof( cube_type_tau_atomic ) );


    MPI_Datatype tau_atomic;
    MPI_Type_contiguous( sizeof( cube_type_tau_atomic ), MPI_BYTE, &tau_atomic );
    MPI_Type_commit( &tau_atomic );

    unsigned ci;
    int      w = 0;
    for ( ci = 0; ci < ncallpaths; ci++ )
    {
        if ( CNODES != ncallpaths && !__cube_is_bit_set( bitmask, ci ) )
        {
            continue;
        }

        fill_cube_type_tau_atomic_row( local, nthreads, idtocid[ ci ] + 1 );
        MPI_Gatherv( local, nthreads, tau_atomic, global, counts, displs, tau_atomic, 0, MPI_COMM_WORLD );
        if ( cube->cube_flavour == CUBE_MASTER )
        {
            cube_write_sev_row_of_cube_type_tau_atomic( cube, met, ( cube_cnode* )( sequence->data[ ci ] ), global );
        }

        w++;
    }
    free( idtocid );
    //free( sequence );
    free( local );
    free( global );
}
