/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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


/** @internal
 *
 *  @brief      Mini chunk pool.
 */


#ifndef OTF2_TEST_CHUNK_POOL_H
#define OTF2_TEST_CHUNK_POOL_H

#ifndef OTF2_TEST_CHUNK_POOL_USER_CODE
#define OTF2_TEST_CHUNK_POOL_USER_CODE
#endif

struct otf2_test_chunk_pool
{
    uint8_t chunks[ OTF2_TEST_CHUNK_POOL_NUMBER_OF_CHUNKS ]
    [ OTF2_TEST_CHUNK_POOL_CHUNK_SIZE ];
    size_t  next_free;
    char*   definitions;
};


static void*
otf2_test_chunk_pool_allocate_chunk( void*         userData,
                                     OTF2_FileType fileType,
                                     uint64_t      locationId,
                                     void**        perBufferData,
                                     uint64_t      chunkSize )
{
    OTF2_TEST_CHUNK_POOL_USER_CODE

    struct otf2_test_chunk_pool* chunk_pool = *perBufferData;
    if ( !chunk_pool )
    {
        *perBufferData = calloc( 1, sizeof( *chunk_pool ) );
        chunk_pool     = *perBufferData;
        check_pointer( *perBufferData, "Allocating chunk pool for buffer." );
    }

    if ( fileType == OTF2_FILETYPE_EVENTS )
    {
        check_condition( chunkSize == OTF2_TEST_CHUNK_POOL_CHUNK_SIZE,
                         "Requested chunk size match: %zu", chunkSize );

        if ( chunk_pool->next_free == OTF2_TEST_CHUNK_POOL_NUMBER_OF_CHUNKS )
        {
            return NULL;
        }

        return chunk_pool->chunks[ chunk_pool->next_free++ ];
    }

    if ( chunk_pool->definitions )
    {
        return NULL;
    }

    chunk_pool->definitions = malloc( chunkSize );
    return chunk_pool->definitions;
}


static void
otf2_test_chunk_pool_free_all_chunks( void*         userData,
                                      OTF2_FileType fileType,
                                      uint64_t      locationId,
                                      void**        perBufferData,
                                      bool          final )
{
    check_pointer( *perBufferData, "Buffer chunk pool." );

    struct otf2_test_chunk_pool* chunk_pool = *perBufferData;

    if ( fileType == OTF2_FILETYPE_EVENTS )
    {
        chunk_pool->next_free = 0;
    }
    else
    {
        free( chunk_pool->definitions );
        chunk_pool->definitions = NULL;
    }

    if ( final )
    {
        free( chunk_pool );
        *perBufferData = NULL;
    }
}


static OTF2_MemoryCallbacks otf2_test_chunk_pool_memory_callbacks =
{
    .otf2_allocate = otf2_test_chunk_pool_allocate_chunk,
    .otf2_free_all = otf2_test_chunk_pool_free_all_chunks
};


#endif /* OTF2_TEST_CHUNK_POOL_H */
