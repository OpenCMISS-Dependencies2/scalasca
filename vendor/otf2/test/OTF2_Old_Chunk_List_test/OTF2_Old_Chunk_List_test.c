/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief      testing the functionality of the old_chunk_list
 *
 */


#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <otf2/otf2.h>

#if HAVE( PTHREAD )
#include <otf2/OTF2_Pthread_Locks.h>
#endif

#include <otf2-test/check.h>

#include <otf2-test/gettime.h>

#define OTF2_TEST_FLUSH_DEFAULT OTF2_FLUSH
#include <otf2-test/flush.h>

static bool allocate_chunk_called;

#define OTF2_TEST_CHUNK_POOL_CHUNK_SIZE OTF2_CHUNK_SIZE_MIN
#define OTF2_TEST_CHUNK_POOL_NUMBER_OF_CHUNKS 2
#define OTF2_TEST_CHUNK_POOL_USER_CODE allocate_chunk_called = true;
#include <otf2-test/chunk-pool.h>

int
main( int    argc,
      char** argv )
{
    const char* archivePath = "OTF2_OLD_CHUNK_LIST_TEST_PATH";
    const char* archiveName = "OTF2_OLD_CHUNK_LIST_TEST_ARCHIVE";

    bool use_sion = argc > 1;

    OTF2_Archive* archive = OTF2_Archive_Open( archivePath,
                                               archiveName,
                                               OTF2_FILEMODE_WRITE,
                                               OTF2_TEST_CHUNK_POOL_CHUNK_SIZE,
                                               OTF2_TEST_CHUNK_POOL_CHUNK_SIZE,
                                               use_sion
                                               ? OTF2_SUBSTRATE_SION
                                               : OTF2_SUBSTRATE_POSIX,
                                               OTF2_COMPRESSION_NONE );

    OTF2_Archive_SetFlushCallbacks( archive, &otf2_test_flush_callbacks, NULL );
    OTF2_Archive_SetMemoryCallbacks( archive, &otf2_test_chunk_pool_memory_callbacks, NULL );
    OTF2_Archive_SetSerialCollectiveCallbacks( archive );
#if HAVE( PTHREAD )
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
#if HAVE( PTHREAD_MUTEX_ERRORCHECK )
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
#endif
    OTF2_Pthread_Archive_SetLockingCallbacks( archive, &attr );
#endif

    OTF2_Archive_OpenEvtFiles( archive );

    OTF2_EvtWriter* evt_writer = OTF2_Archive_GetEvtWriter( archive, 0 );
    // -> new buffer for local_def_writer
    // -> allocate_chunk is called

    OTF2_EvtWriter_StoreRewindPoint( evt_writer, 0 );

    /* writing events until a new chunk is used */
    allocate_chunk_called = false;
    int i = 0;
    do
    {
        OTF2_EvtWriter_Enter( evt_writer, NULL, otf2_test_gettime(), 1 );
        OTF2_EvtWriter_Leave( evt_writer, NULL, otf2_test_gettime(), 1 );
        i++;     // counting written events
    }
    while ( !allocate_chunk_called );


    OTF2_EvtWriter_Rewind( evt_writer, 0 );
    // (this will not cause enter/leave rewind events! )
    // -> now there should be an old chunk


    /* rewrite the same events as before to force the need of a second chunk again */
    OTF2_EvtWriter_StoreRewindPoint( evt_writer, 0 );
    allocate_chunk_called = false;
    for ( int j = 0; j < i; j++ )
    {
        OTF2_EvtWriter_Enter( evt_writer, NULL, otf2_test_gettime(), 1 );
        OTF2_EvtWriter_Leave( evt_writer, NULL, otf2_test_gettime(), 1 );
    }

    /* evaluation */
    check_condition( !allocate_chunk_called, "No chunk allocated." );

    OTF2_Archive_CloseEvtWriter( archive,
                                 evt_writer );

    OTF2_Archive_CloseEvtFiles( archive );

    OTF2_Archive_Close( archive );
    // -> new buffer for anchor file
    // -> allocate_chunk is called

    return EXIT_SUCCESS;
}
