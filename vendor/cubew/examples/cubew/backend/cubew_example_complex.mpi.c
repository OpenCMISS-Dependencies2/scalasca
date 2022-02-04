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
 * \file cubew_example_complex.mpi.c
   \brief A small example of using "libcubew4.a".

   Sterile identical pattern of usage of c-writer how it is used in episode and scout.



 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include <mpi.h>

#include "cubew_cube.h"


#ifndef CNODES
#define CNODES 1000
#endif


#ifndef COMPRESSION
#define COMPRESSION CUBE_FALSE
#endif

#define NMETRICS 40
#define NTAUMETRICS 7



static int current_random = 0; // uswd to create a stable random loooking sequence of numbers (used for creating of a calltree)
static int MAXDEPTH       = 6;


#define NDIMS 2


/* forward declarations */
void
create_cube_structure( cube_t*,
                       cube_metric** metrics,
                       cube_cnode**  cnodes,
                       unsigned      Nthreads );
void
add_tau_events_to_cube_structure( cube_t*       cube,
                                  cube_metric** tau_metrics );

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
calculate_sum2( int start,
                int finish );




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





    calculate_max_depth();

    /// actual number of writers might be different. For code it is transparent.
    unsigned ArraySize = CNODES;     /// In this pattern, number of cnodes

    unsigned Nthreads = mpi_cw_size; /// In this pattern, number of threads


    /// Pattern has to know, which metrics are there to perform parallel writing.
    /// To destroy is only this aray, not metrics inside. Metrics are destroyed by "cube_free"
    cube_metric** metrics_vector = ( cube_metric** )calloc( NMETRICS, sizeof( cube_metric* ) );

    cube_metric** tau_metrics_vector = ( cube_metric** )calloc( NTAUMETRICS, sizeof( cube_metric* ) );


    /// Pattern has to know, which cnodes are there to perform parallel writing.
    /// To destroy is only this aray, not cnodes inside. Cnodes are destroyed by "cube_free"
    cube_cnode** cnodes_vector = ( cube_cnode** )calloc( ArraySize, sizeof( cube_cnode* ) );




    char* cubefile = "complex-mpi-cube";


    //    FILE* fp = fopen(cubefile, "w");
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

    create_cube_structure( cube, metrics_vector, cnodes_vector, Nthreads );

    add_tau_events_to_cube_structure( cube, tau_metrics_vector );




    /* generate header */
    cube_def_mirror( cube, "http://www.fz-juelich.de/jsc/scalasca/" );
    cube_def_attr( cube, "description", "a simple example of a cube, created using scout and episode pattern" );
    cube_def_attr( cube, "experiment time", "November 1st, 2011" );

    cube_enable_flat_tree( cube, CUBE_FALSE );



    double  dsev  = 0.;
    double* drsev = NULL;
    if ( my_rank == root )
    {
        /* root creates also storage for whole data*/
        drsev = ( double* )calloc( Nthreads, sizeof( double ) );
    }
    cube_type_tau_atomic  tausev;
    cube_type_tau_atomic* taursev = NULL;
    if ( my_rank == root )
    {
        /* root creates also storage for whole data*/
        taursev = ( cube_type_tau_atomic* )calloc( Nthreads, __cube_metric_size_dtype( CUBE_DATA_TYPE_TAU_ATOMIC ) );
    }


    /* Create derivated MPI_Datatype which corresponds to Tau Atomic metric: uint32_t + 4x double*/
    MPI_Datatype mpi_tau_datatype;

//     MPI_Type_struct(2, blocklens, offsets, old_types, &mpi_tau_datatype);
    MPI_Type_contiguous( __cube_metric_size_dtype( CUBE_DATA_TYPE_TAU_ATOMIC ), MPI_CHAR,  &mpi_tau_datatype );

    MPI_Type_commit( &mpi_tau_datatype );





    unsigned mi = 0;
    for ( mi = 0; mi < NMETRICS; mi++ )  /// run over the metrics
    {
        cube_dyn_array* sequence = cube_get_cnodes_for_metric( cube, metrics_vector[ mi ] );

        unsigned ci = 0;
        for ( ci = 0; ci < sequence->size; ci++ )
        {
            /* create rank specific data - myrank number + 4th perfect number */
            dsev = my_rank + 8128. + mi + ci;
            MPI_Gather( &dsev, 1, MPI_DOUBLE, drsev, 1, MPI_DOUBLE, root, MPI_COMM_WORLD );
            /* cube_type_double is a packed structure, contains only one single souble -> therefore castin (cube_type_double* ) is permitted */
            cube_write_sev_row_of_cube_type_double( cube, metrics_vector[ mi ], ( cube_cnode* )( sequence->data )[ ci ], ( cube_type_double* )drsev );
        }
    }

    for ( mi = 0; mi < NTAUMETRICS; mi++ )  /// run over the metrics
    {
        cube_dyn_array* sequence = cube_get_cnodes_for_metric( cube, metrics_vector[ mi ] );

        unsigned ci = 0;
        for ( ci = 0; ci < sequence->size; ci++ )
        {
            /* create rand depending numbers: 84. is 2th superperfect number in sequence A019283 (m:2, k:6) */
            /*  is creates statistics for serie of numbers -floor(n/2) to ceil(n/2) */

            double A  = ( double )( ci + mi + 84. + my_rank );
            int    _a = ( int )floor( A / 2. );
            int    a_ = ( int )ceil( A / 2. );

            tausev.N    = ( _a + a_ + 1 );
            tausev.Min  = -_a;
            tausev.Max  = a_;
            tausev.Sum  = ( _a != a_ ) ? a_ : 0;
            tausev.Sum2 = calculate_sum2( _a, a_ );

//          MPI_Gather( &tausev, 36, MPI_CHAR, (char*)taursev, 36, MPI_CHAR , root, MPI_COMM_WORLD);
            MPI_Gather( &tausev, 1, mpi_tau_datatype, taursev, 1, mpi_tau_datatype, root, MPI_COMM_WORLD );
            cube_write_sev_row_of_cube_type_tau_atomic( cube, tau_metrics_vector[ mi ], ( cube_cnode* )( sequence->data )[ ci ], taursev );
        }
    }

    if ( my_rank == root )
    {
        printf( "Test file %s.cubex complete.\n", cubefile );
    }


    /* Delete cube_t structure and release all memory*/
    free( drsev );
    free( taursev );
    free( metrics_vector );               /* content shouldn't be freed, it is done internally by cube */
    free( tau_metrics_vector );           /* content shouldn't be freed, it is done internally by cube */
    free( cnodes_vector );                /* content shouldn't be freed, it is done internally by cube */
    cube_free( cube );
    MPI_Finalize();
    return 0;
}





int
calculate_sum2( int start, int finish )
{
    int sum = 0;
    int i   = start;
    for ( i = start; i <= finish; i++ )
    {
        sum += i * i;
    }
    return sum;
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
    metrics[ 1 ] = cube_def_met( cube, "Visits", "visits", "INTEGER", "occ", "",
                                 "http://www.cs.utk.edu/usr.html",
                                 "Number of visits", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 2 ] = cube_def_met( cube, "Wait at Barrier", "mpi_barrier_wait", "FLOAT", "sec", "",
                                 "http://www.cs.utk.edu/sys.html",
                                 "Waiting time in front of MPI barriers", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 3 ] = cube_def_met( cube, "Barrier Completion", "mpi_barrier_completion", "FLOAT", "sec", "",
                                 "http://www.cs.utk.edu/sys.html",
                                 "Time needed to finish an MPI barrier", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 4 ] = cube_def_met( cube, "Late Sender", "mpi_latesender", "FLOAT", "sec", "",
                                 "http://www.cs.utk.edu/sys.html",
                                 "Time a receiving process is waiting for a message", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 5 ] = cube_def_met( cube, "Messages in Wrong Order", "mpi_latesender_wo", "FLOAT", "sec", "",
                                 "http://www.cs.utk.edu/sys.html",
                                 "Late Sender situation due to messages received in the wrong order", metrics[ 4 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 6 ] = cube_def_met( cube, "Messages from different sources ", "mpi_lswo_defferent", "FLOAT", "sec", "",
                                 "http://www.cs.utk.edu/sys.html",
                                 "Wrong order situation due to messages received from different sources", metrics[ 5 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 7 ] = cube_def_met( cube, "Messages from same sources ", "mpi_lswo_same", "FLOAT", "sec", "",
                                 "http://www.cs.utk.edu/sys.html",
                                 "Wrong order situation due to messages received from same sources", metrics[ 5 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 8 ] = cube_def_met( cube, "Late Receiver", "mpi_latereceiver", "FLOAT", "sec", "",
                                 "http://www.cs.utk.edu/sys.html",
                                 "Time a sending process is waiting for the receiver to become ready", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 9 ] = cube_def_met( cube, "Early Reduce", "mpi_earlyreduce", "FLOAT", "sec", "",
                                 "http://www.cs.utk.edu/sys.html",
                                 "Waiting time due to an early receiver in MPI n-to-1 operation", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 10 ] = cube_def_met( cube, "Early Scan", "mpi_earlyscan", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Waiting time due to an early receiver in MPI scan operation", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 11 ] = cube_def_met( cube, "Late Broadcast", "mpi_latebroadcast", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Waiting time due to an early rreceiver in MPI 1-to-n operation", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 12 ] = cube_def_met( cube, "Wait at N x N", "mpi_wait_nxn", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Time due to inherent synchronization in MPI n-to-n operation", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 13 ] = cube_def_met( cube, "N x N completion", "mpi_nxn_completion", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Time needed to finish a n-to-n collective operation", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 14 ] = cube_def_met( cube, "P2P send synchronizations", "sync_send", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of point-to-point send synchronization operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 15 ] = cube_def_met( cube, "Late Receivers", "mpi_slr_count", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of Late Receivers instances in synchronizations", metrics[ 14 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 16 ] = cube_def_met( cube, "P2P recv synchronizations", "sync_recv", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of point-to-point receive synchronization operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 17 ] = cube_def_met( cube, "Late Senders", "mpi_sls_count", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of Late Sender instances in synchronizations", metrics[ 16 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 18 ] = cube_def_met( cube, "Messages in Wrong Order", "mpi_slswo_count", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of Late Sender instances in synchronizations were messages are receivesd in wrong order", metrics[ 17 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 19 ] = cube_def_met( cube, "Collective synchronizations", "sync_coll", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of collective synchronization oprations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 20 ] = cube_def_met( cube, "P2P send communications", "comms_send", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of point-to-point send communication operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 21 ] = cube_def_met( cube, "Late Receivers", "mpi_clr_count", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of Late Receiver instances in communications", metrics[ 20 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 22 ] = cube_def_met( cube, "P2P recv communications", "comms_recv", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of point-to-point receive communication operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 23 ] = cube_def_met( cube, "Late Sender", "mpi_cls_count", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of Late Sender instances in communications", metrics[ 22 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 24 ] = cube_def_met( cube, "Messages in Wrong Order", "mpi_clswo_count", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of Late Sender instances in communications were messages are received in wrong order", metrics[ 23 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 25 ] = cube_def_met( cube, "Collective exchange communications", "comms_cxch", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of collective communications as source and destination", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 26 ] = cube_def_met( cube, "Collective communications as source", "comms_csrc", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of collective communications as source", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 27 ] = cube_def_met( cube, "Collective communications as destination", "comms_cdst", "INTEGER", "occ", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of collective communications as destination", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 28 ] = cube_def_met( cube, "P2P bytes sent", "bytes_sent", "INTEGER", "bytes", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of bytes sent in point-to-point operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 29 ] = cube_def_met( cube, "P2P bytes received", "bytes_rcvd", "INTEGER", "bytes", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of bytes received in point-to-point operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 30 ] = cube_def_met( cube, "Collective bytes outgoing", "bytes_cout", "INTEGER", "bytes", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of bytes outgoing in collective operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 31 ] = cube_def_met( cube, "Collective bytes incoming", "bytes_cin", "INTEGER", "bytes", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of bytes incoming in collective operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 32 ] = cube_def_met( cube, "RMA bytes received", "bytes_get", "INTEGER", "bytes", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of bytes received in RMA operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 33 ] = cube_def_met( cube, "RMA bytes put", "bytes_put", "INTEGER", "bytes", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Number of bytes sent in RMA operations", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 34 ] = cube_def_met( cube, "Late Post", "mpi_rma_late_post", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Time spent in MPI RMA LAte Post inefficiency pattern", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 35 ] = cube_def_met( cube, "Early Wait", "mpi_rma_early_wait", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Time spent in MPI_Win_Wait waiting for last MPI_Win_complete.", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 36 ] = cube_def_met( cube, "Late Complet", "mpi_rma_late_complete", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Time spent in MPI_Win_Wait waiting between last rma access and last MPI_Win_complete.", metrics[ 35 ],  CUBE_METRIC_EXCLUSIVE );
    metrics[ 37 ] = cube_def_met( cube, "Early Transfer", "mpi_rma_early_transfer", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Time spent in MPI RMA Early Tansfer inefficiency pattern.", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 38 ] = cube_def_met( cube, "Wait at Fence", "mpi_rma_wait_at_fence", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Time spent in MPI_Win_fence, waiting for other processes", NULL,  CUBE_METRIC_EXCLUSIVE );
    metrics[ 39 ] = cube_def_met( cube, "Early Fence", "mpi_rma_early_fence", "FLOAT", "sec", "",
                                  "http://www.cs.utk.edu/sys.html",
                                  "Time spent in MPI_Win_fence while  waiting for pending PMA ops", metrics[ 38 ],  CUBE_METRIC_EXCLUSIVE );





    /* define call tree */
    char* mod = "/ICL/CUBE/example.c";
//      cube_region *regn0, *regn1, *regn2;

    cube_region** regn = ( cube_region** )calloc( 169, sizeof( cube_region* ) );

    char* names[ 169 ] = {
        "UNKNOWN",                        "TRACING",                                "MPI_Accumulate",                                "MPI_Allgather",                                         "MPI_Allgatherv",                                               "MPI_Allreduce",                                                     "MPI_Alltoall",                                                             "MPI_Alltoallv",                                                                     "MPI_Alltoallw",                                                                             "MPI_Barrier",                                                                                       "MPI_Bcast",                                                                                             "MPI_Bsend",
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

    unsigned i = 0;
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
        regn[ i ] = cube_def_region( cube, names[ i ], -1, -1, "", descr, mod );
    }

    //     create calltree
    srand( 0 );
    int idx        = 0;
    int region_idx = 0;
    create_calltree( cube, regn, cnodes, &idx, &region_idx,  NULL, 0 );


    /* define location tree */
    cube_machine* mach  = cube_def_mach( cube, "MSC", "" );
    cube_node*    node  = cube_def_node( cube, "Athena", mach );
    cube_process* proc0 = cube_def_proc( cube, "Process 0", 0, node );
    cube_thread** thrds = ( cube_thread** )calloc( Nthreads, sizeof( cube_thread* ) );

    i = 0;
    for ( i = 0; i < Nthreads; i++ )
    {
        char buffer[ 30 ];
        sprintf( buffer, "Thread %u", i );
        thrds[ i ] = cube_def_thrd( cube, buffer, i, proc0 );
    }


    free( regn );



    /* define  first topology */
    /* number of dimensions in topology  - linear topology*/
    /* number of elements in every dimension - size of COMM_WORLD */
    long dimv0[ 1 ] = { Nthreads };
    /* is dimension periodical (torus) or no: 0 - 'no', 1 - 'yes' */
    int periodv0[ 1 ] = { 1 };
    /* create first topology */
    cube_cartesian* cart0 = cube_def_cart( cube, 1, dimv0, periodv0 );
    /* set name of the topology */
    cube_cart_set_name( cart0, "Application Topology  (Linear)" );

    i = 0; /* reuse index from above */
    for ( i = 0; i < Nthreads; i++ )
    {
        /* set coordinate of every thread */
        long coordv[ 1 ] = { i };
        cube_def_coords( cube, cart0, thrds[ i ], coordv );
    }

    /* define  second topology - three times larger, every third is set */
    /* number of dimensions in topology  - linear topology*/
    /* number of elements in every dimension - size of COMM_WORLD */
    long dimv1[ 1 ] = { Nthreads*3 };
    /* is dimension periodical (torus) or no: 0 - 'no', 1 - 'yes' */
    int periodv1[ 1 ] = { 1 };
    /* create second topology */
    cube_cartesian* cart1 = cube_def_cart( cube, 1, dimv1, periodv1 );
    /* set name of the topology */
    cube_cart_set_name( cart1, "Application Topology  (Linear Expanded)" );

    i = 0; /* reuse index from above */
    for ( i = 0; i < Nthreads; i++ )
    {
        /* set coordinate of every thread */
        long coordv[ 1 ] = { i*3 };
        cube_def_coords( cube, cart1, thrds[ i ], coordv );
    }


    /* define  third topology - a minimal square, which contains #nthreads elements */
    /* number of dimensions in topology  - 2d topology*/
    long size_of_dimenstion = ( long )( ceil( sqrt( Nthreads ) ) );
    /* number of elements in every dimension - (ceil(sqrt(size of COMM_WORLD)))^2 */
    long dimv2[ 2 ] = { size_of_dimenstion, size_of_dimenstion };
    /* is dimension periodical (torus) or no: 0 - 'no', 1 - 'yes' */
    int periodv2[ 2 ] = { 1, 1 };
    /* create second topology */
    cube_cartesian* cart2 = cube_def_cart( cube, 2, dimv2, periodv2 );
    /* set name of the topology */
    cube_cart_set_name( cart1, "Application 2D Topology  (Square)" );

    i = 0; /* reuse index from above */
    for ( i = 0; i < Nthreads; i++ )
    {
        /* set coordinate of every thread */
        /* we fill rowwise */
        long coordv[ 2 ] = { i / size_of_dimenstion, i % size_of_dimenstion };
        cube_def_coords( cube, cart2, thrds[ i ], coordv );
    }


    free( thrds );
}





void
add_tau_events_to_cube_structure( cube_t* cube, cube_metric** tau_metrics )
{
    tau_metrics[ 0 ] = cube_def_met( cube, "Memory allocation", "memoryallocation", "TAU_ATOMIC", "tau_statistics", "",
                                     "@mirror@patterns-2.1.html#memoryallocation",
                                     "Total allocated memory", NULL, CUBE_METRIC_EXCLUSIVE );

    tau_metrics[ 1 ] = cube_def_met( cube, "Memory allocation using operator \"new\"", "newmemoryallocation", "TAU_ATOMIC", "tau_statistics", "",
                                     "@mirror@patterns-2.1.html#newmemoryallocation",
                                     "Allocated memory using \"new\"", NULL, CUBE_METRIC_EXCLUSIVE );

    tau_metrics[ 2 ] = cube_def_met( cube, "Memory allocation using call \"malloc\"", "mallocmemoryallocation", "TAU_ATOMIC", "tau_statistics", "",
                                     "@mirror@patterns-2.1.html#mallocmemoryallocation",
                                     "Allocated memory using \"malloc\"", NULL, CUBE_METRIC_EXCLUSIVE );

    tau_metrics[ 3 ] = cube_def_met( cube, "Memory allocation using call \"calloc\"", "callocmemoryallocation", "TAU_ATOMIC", "tau_statistics", "",
                                     "@mirror@patterns-2.1.html#callocmemoryallocation",
                                     "Allocated memory using \"calloc\"", NULL, CUBE_METRIC_EXCLUSIVE );

    tau_metrics[ 4 ] = cube_def_met( cube, "Memory transfer from/to GPU", "gpumemorytransfer", "TAU_ATOMIC", "tau_statistics", "",
                                     "@mirror@patterns-2.1.html#gpumemorytransfer",
                                     "Memory transferred either to or from GPU", NULL, CUBE_METRIC_EXCLUSIVE );

    tau_metrics[ 5 ] = cube_def_met( cube, "Memory transfer from GPU", "fromgpumemorytransfer", "TAU_ATOMIC", "tau_statistics", "",
                                     "@mirror@patterns-2.1.html#fromgpumemorytransfer",
                                     "Memory transferred either  from GPU", NULL, CUBE_METRIC_EXCLUSIVE );

    tau_metrics[ 6 ] = cube_def_met( cube, "Memory transfer to GPU", "togpumemorytransfer", "TAU_ATOMIC", "tau_statistics", "",
                                     "@mirror@patterns-2.1.html#togpumemorytransfer",
                                     "Memory transferred either to GPU", NULL, CUBE_METRIC_EXCLUSIVE );
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
    int      _num_children = get_next_random();
    unsigned i             = 0;
    for ( i = 0; i < _num_children; i++ )
    {
        unsigned rand_reg = *region_idx;
        *region_idx = ( ( *region_idx + 1 ) % 169 ); // cyclic selection of regions
        cube_cnode* cnode = cube_def_cnode( cube, regn[ rand_reg ], parent );
        cnodes[ *idx ] = cnode;
        ( *idx )++;
        create_calltree( cube, regn, cnodes, idx, region_idx, cnode, level + 1 );
        if ( ( *idx ) >= CNODES )
        {
            return;                    // for the case, inside of those function was reached the end of array
        }
    }
}





void
calculate_max_depth()
{
    MAXDEPTH = ( int )( log( CNODES ) / log( 13 ) ) + 2;
}
