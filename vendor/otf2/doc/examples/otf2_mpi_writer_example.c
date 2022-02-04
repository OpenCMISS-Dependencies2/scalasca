/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <mpi.h>

#include <otf2/otf2.h>

#if MPI_VERSION < 3
#define OTF2_MPI_UINT64_T MPI_UNSIGNED_LONG
#define OTF2_MPI_INT64_T  MPI_LONG
#endif
#include <otf2/OTF2_MPI_Collectives.h>

static OTF2_TimeStamp
get_time( void )
{
    double t = MPI_Wtime() * 1e9;
    return ( uint64_t )t;
}

static OTF2_FlushType
pre_flush( void*            userData,
           OTF2_FileType    fileType,
           OTF2_LocationRef location,
           void*            callerData,
           bool             final )
{
    return OTF2_FLUSH;
}

static OTF2_TimeStamp
post_flush( void*            userData,
            OTF2_FileType    fileType,
            OTF2_LocationRef location )
{
    return get_time();
}

static OTF2_FlushCallbacks flush_callbacks =
{
    .otf2_pre_flush  = pre_flush,
    .otf2_post_flush = post_flush
};

int
main( int    argc,
      char** argv )
{
    MPI_Init( &argc, &argv );
    int size;
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    int rank;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    OTF2_Archive* archive = OTF2_Archive_Open( "ArchivePath",
                                               "ArchiveName",
                                               OTF2_FILEMODE_WRITE,
                                               1024 * 1024 /* event chunk size */,
                                               4 * 1024 * 1024 /* def chunk size */,
                                               OTF2_SUBSTRATE_POSIX,
                                               OTF2_COMPRESSION_NONE );

    OTF2_Archive_SetFlushCallbacks( archive, &flush_callbacks, NULL );

    OTF2_MPI_Archive_SetCollectiveCallbacks( archive,
                                             MPI_COMM_WORLD,
                                             MPI_COMM_NULL );

    OTF2_Archive_OpenEvtFiles( archive );

    OTF2_EvtWriter* evt_writer = OTF2_Archive_GetEvtWriter( archive,
                                                            rank );

    uint64_t epoch_start = get_time();

    OTF2_EvtWriter_Enter( evt_writer,
                          NULL,
                          get_time(),
                          0 /* region */ );

    OTF2_EvtWriter_MpiCollectiveBegin( evt_writer,
                                       NULL,
                                       get_time() );

    MPI_Barrier( MPI_COMM_WORLD );

    OTF2_EvtWriter_MpiCollectiveEnd( evt_writer,
                                     NULL,
                                     get_time(),
                                     OTF2_COLLECTIVE_OP_BARRIER,
                                     0 /* communicator */,
                                     OTF2_UNDEFINED_UINT32 /* root */,
                                     0 /* bytes provided */,
                                     0 /* bytes obtained */ );

    OTF2_EvtWriter_Leave( evt_writer,
                          NULL,
                          get_time(),
                          0 /* region */ );

    uint64_t epoch_end = get_time();

    OTF2_Archive_CloseEvtWriter( archive, evt_writer );

    OTF2_Archive_CloseEvtFiles( archive );

    OTF2_Archive_OpenDefFiles( archive );
    OTF2_DefWriter* def_writer = OTF2_Archive_GetDefWriter( archive,
                                                            rank );
    OTF2_Archive_CloseDefWriter( archive, def_writer );
    OTF2_Archive_CloseDefFiles( archive );

    uint64_t global_epoch_start;
    MPI_Reduce( &epoch_start,
                &global_epoch_start,
                1, OTF2_MPI_UINT64_T, MPI_MIN,
                0, MPI_COMM_WORLD );

    uint64_t global_epoch_end;
    MPI_Reduce( &epoch_end,
                &global_epoch_end,
                1, OTF2_MPI_UINT64_T, MPI_MAX,
                0, MPI_COMM_WORLD );

    if ( 0 == rank )
    {
        OTF2_GlobalDefWriter* global_def_writer = OTF2_Archive_GetGlobalDefWriter( archive );

        OTF2_GlobalDefWriter_WriteClockProperties( global_def_writer,
                                                   1000000000,
                                                   global_epoch_start,
                                                   global_epoch_end - global_epoch_start + 1 );

        OTF2_GlobalDefWriter_WriteString( global_def_writer, 0, "" );
        OTF2_GlobalDefWriter_WriteString( global_def_writer, 1, "Master Thread" );
        OTF2_GlobalDefWriter_WriteString( global_def_writer, 2, "MPI_Barrier" );
        OTF2_GlobalDefWriter_WriteString( global_def_writer, 3, "PMPI_Barrier" );
        OTF2_GlobalDefWriter_WriteString( global_def_writer, 4, "barrier" );
        OTF2_GlobalDefWriter_WriteString( global_def_writer, 5, "MyHost" );
        OTF2_GlobalDefWriter_WriteString( global_def_writer, 6, "node" );
        OTF2_GlobalDefWriter_WriteString( global_def_writer, 7, "MPI" );
        OTF2_GlobalDefWriter_WriteString( global_def_writer, 8, "MPI_COMM_WORLD" );

        OTF2_GlobalDefWriter_WriteRegion( global_def_writer,
                                          0 /* id */,
                                          2 /* region name  */,
                                          3 /* alternative name */,
                                          4 /* description */,
                                          OTF2_REGION_ROLE_BARRIER,
                                          OTF2_PARADIGM_MPI,
                                          OTF2_REGION_FLAG_NONE,
                                          7 /* source file */,
                                          0 /* begin lno */,
                                          0 /* end lno */ );

        OTF2_GlobalDefWriter_WriteSystemTreeNode( global_def_writer,
                                                  0 /* id */,
                                                  5 /* name */,
                                                  6 /* class */,
                                                  OTF2_UNDEFINED_SYSTEM_TREE_NODE /* parent */ );

        for ( int r = 0; r < size; r++ )
        {
            char process_name[ 32 ];
            sprintf( process_name, "MPI Rank %d", r );
            OTF2_GlobalDefWriter_WriteString( global_def_writer,
                                              9 + r,
                                              process_name );

            OTF2_GlobalDefWriter_WriteLocationGroup( global_def_writer,
                                                     r /* id */,
                                                     9 + r /* name */,
                                                     OTF2_LOCATION_GROUP_TYPE_PROCESS,
                                                     0 /* system tree */ );

            OTF2_GlobalDefWriter_WriteLocation( global_def_writer,
                                                r /* id */,
                                                1 /* name */,
                                                OTF2_LOCATION_TYPE_CPU_THREAD,
                                                4 /* # events */,
                                                r /* location group */ );
        }

        uint64_t comm_locations[ size ];
        for ( int r = 0; r < size; r++ )
        {
            comm_locations[ r ] = r;
        }
        OTF2_GlobalDefWriter_WriteGroup( global_def_writer,
                                         0 /* id */,
                                         7 /* name */,
                                         OTF2_GROUP_TYPE_COMM_LOCATIONS,
                                         OTF2_PARADIGM_MPI,
                                         OTF2_GROUP_FLAG_NONE,
                                         size,
                                         comm_locations );

        OTF2_GlobalDefWriter_WriteGroup( global_def_writer,
                                         1 /* id */,
                                         0 /* name */,
                                         OTF2_GROUP_TYPE_COMM_GROUP,
                                         OTF2_PARADIGM_MPI,
                                         OTF2_GROUP_FLAG_NONE,
                                         size,
                                         comm_locations );

        OTF2_GlobalDefWriter_WriteComm( global_def_writer,
                                        0 /* id */,
                                        8 /* name */,
                                        1 /* group */,
                                        OTF2_UNDEFINED_COMM /* parent */ );

        OTF2_Archive_CloseGlobalDefWriter( archive,
                                           global_def_writer );
    }
    MPI_Barrier( MPI_COMM_WORLD );

    OTF2_Archive_Close( archive );

    MPI_Finalize();

    return EXIT_SUCCESS;
}
