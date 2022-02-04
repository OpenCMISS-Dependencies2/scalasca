/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/** @internal
 *
 *  @file
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <sion.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#define OTF2_DEBUG_MODULE_NAME SION_RANK_MAP
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"
#include "otf2_archive_location.h"

#include "otf2_attribute_value.h"
#include "otf2_attribute_list.h"
#include "otf2_id_map.h"

#include "OTF2_File.h"
#include "otf2_file_int.h"
#include "otf2_file_sion.h"
#include "otf2_file_substrate.h"
#include "otf2_file_substrate_sion.h"
#include "otf2_file_substrate_sion_int.h"

#include "OTF2_Buffer.h"

#include "otf2_collectives.h"

enum
{
    OTF2_SION_RANK_MAP_HEADER   = 10,
    OTF2_SION_RANK_MAP_RANK     = 11,
    OTF2_SION_RANK_MAP_LOCATION = 12
};

static OTF2_ErrorCode
otf2_sion_load_rank_map( OTF2_Archive* archive,
                         uint32_t*     numberOfFilesOut,
                         uint32_t*     numberOfRanksOut,
                         uint32_t**    fileNumberPerRankOut,
                         uint32_t**    fileRankPerRankOut,
                         uint32_t**    locationsPerRankOut,
                         uint32_t*     totalNumberOfLocationsOut,
                         uint64_t**    locationsOut )
{
    UTILS_DEBUG_ENTRY();

    uint32_t     number_of_files;
    uint32_t     number_of_ranks;
    uint32_t*    file_number_per_rank = NULL;
    uint32_t*    file_rank_per_rank   = NULL;
    uint32_t*    locations_per_rank   = NULL;
    uint32_t     total_number_of_locations;
    uint64_t*    locations  = NULL;
    OTF2_Buffer* srm_buffer = OTF2_Buffer_New( archive,
                                               NULL,
                                               OTF2_CHUNK_SIZE_MIN,
                                               OTF2_BUFFER_READ,
                                               OTF2_BUFFER_CHUNKED,
                                               OTF2_FILETYPE_SIONRANKMAP,
                                               OTF2_UNDEFINED_LOCATION );

    if ( !srm_buffer )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Can't open SION rank-map buffer!" );
    }

    OTF2_ErrorCode status = otf2_buffer_open_file( srm_buffer );
    if ( OTF2_SUCCESS != status )
    {
        UTILS_ERROR( status, "Could not open SION rank-map file." );
        goto out;
    }

    /* record id + number_of_files + number_of_ranks + total_number_of_locations */
    status = OTF2_Buffer_GuaranteeRead( srm_buffer, 1 + 5 + 5 + 5 );
    if ( OTF2_SUCCESS != status )
    {
        UTILS_ERROR( status, "Malformed SION rank-map file." );
        goto out;
    }
    uint8_t record_type;
    OTF2_Buffer_ReadUint8( srm_buffer, &record_type );
    if ( OTF2_SION_RANK_MAP_HEADER != record_type )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_RECORD,
                              "Malformed SION rank-map file: expected header record." );
        goto out;
    }
    status = OTF2_Buffer_ReadUint32( srm_buffer, &number_of_files );
    if ( OTF2_SUCCESS != status )
    {
        UTILS_ERROR( status,
                     "Malformed SION rank-map file: can't read number of files." );
        goto out;
    }
    status = OTF2_Buffer_ReadUint32( srm_buffer, &number_of_ranks );
    if ( OTF2_SUCCESS != status )
    {
        UTILS_ERROR( status,
                     "Malformed SION rank-map file: can't read number of ranks." );
        goto out;
    }
    status = OTF2_Buffer_ReadUint32( srm_buffer, &total_number_of_locations );
    if ( OTF2_SUCCESS != status )
    {
        UTILS_ERROR( status,
                     "Malformed SION rank-map file: can't read total number of locations." );
        goto out;
    }

    UTILS_DEBUG( "#files = %u, #ranks = %u, #locations = %u",
                 number_of_files, number_of_ranks, total_number_of_locations );

    file_number_per_rank = calloc( number_of_ranks, sizeof( *file_number_per_rank ) );
    file_rank_per_rank   = calloc( number_of_ranks, sizeof( *file_rank_per_rank ) );
    locations_per_rank   = calloc( number_of_ranks, sizeof( *locations_per_rank ) );
    locations            = calloc( total_number_of_locations, sizeof( *locations ) );
    if ( !file_number_per_rank || !file_rank_per_rank || !locations_per_rank || !locations )
    {
        status = UTILS_ERROR( OTF2_ERROR_MEM_ALLOC_FAILED,
                              "Can't allocate rank mappings" );
        goto out;
    }

    uint32_t locations_index = 0;
    for ( uint32_t rank = 0; rank < number_of_ranks; )
    {
        status = OTF2_Buffer_GuaranteeRead( srm_buffer, 1 );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status, "Malformed SION rank-map file." );
            goto out;
        }
        OTF2_Buffer_ReadUint8( srm_buffer, &record_type );

        if ( OTF2_BUFFER_END_OF_CHUNK == record_type )
        {
            status = OTF2_Buffer_ReadGetNextChunk( srm_buffer );
            if ( OTF2_SUCCESS != status )
            {
                UTILS_ERROR( status,
                             "Malformed SION rank-map file: load of next chunk failed!" );
            }
            continue;
        }
        else if ( OTF2_SION_RANK_MAP_RANK != record_type )
        {
            status = UTILS_ERROR( OTF2_ERROR_INVALID_RECORD,
                                  "Malformed SION rank-map file: unexpected record!" );
            goto out;
        }

        status = OTF2_Buffer_GuaranteeRead( srm_buffer, 5 + 5 + 5 + 5 );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status, "Malformed SION rank-map file: incomplete record." );
            goto out;
        }
        uint32_t next_rank;
        status = OTF2_Buffer_ReadUint32( srm_buffer, &next_rank );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status,
                         "Malformed SION rank-map file: can't read rank." );
            goto out;
        }
        if ( rank != next_rank )
        {
            status = UTILS_ERROR( OTF2_ERROR_INVALID_DATA,
                                  "Malformed SION rank-map file: unexpected rank: %u", next_rank );
            goto out;
        }
        status = OTF2_Buffer_ReadUint32( srm_buffer, &file_number_per_rank[ rank ] );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status,
                         "Malformed SION rank-map file: can't read file-number of rank." );
            goto out;
        }
        status = OTF2_Buffer_ReadUint32( srm_buffer, &file_rank_per_rank[ rank ] );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status,
                         "Malformed SION rank-map file: can't read file-rank of rank." );
            goto out;
        }
        status = OTF2_Buffer_ReadUint32( srm_buffer, &locations_per_rank[ rank ] );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status,
                         "Malformed SION rank-map file: can't read number of locations in rank." );
            goto out;
        }

        UTILS_DEBUG( "rank %u: file = %u file rank = %u #locations = %u",
                     rank,
                     file_number_per_rank[ rank ],
                     file_rank_per_rank[ rank ],
                     locations_per_rank[ rank ] );

        for ( uint32_t i = 0; i < locations_per_rank[ rank ]; )
        {
            status = OTF2_Buffer_GuaranteeRead( srm_buffer, 1 );
            if ( OTF2_SUCCESS != status )
            {
                UTILS_ERROR( status, "Malformed SION rank-map file." );
                goto out;
            }
            OTF2_Buffer_ReadUint8( srm_buffer, &record_type );

            if ( OTF2_BUFFER_END_OF_CHUNK == record_type )
            {
                status = OTF2_Buffer_ReadGetNextChunk( srm_buffer );
                if ( OTF2_SUCCESS != status )
                {
                    UTILS_ERROR( status,
                                 "Malformed SION rank-map file: load of next chunk failed!" );
                }
                continue;
            }
            else if ( OTF2_SION_RANK_MAP_LOCATION != record_type )
            {
                status = UTILS_ERROR( OTF2_ERROR_INVALID_RECORD,
                                      "Malformed SION rank-map file: unexpected record!" );
                goto out;
            }

            status = OTF2_Buffer_GuaranteeRead( srm_buffer, 9 );
            if ( OTF2_SUCCESS != status )
            {
                UTILS_ERROR( status, "Malformed SION rank-map file: incomplete record." );
                goto out;
            }
            status = OTF2_Buffer_ReadUint64( srm_buffer,
                                             &locations[ locations_index++ ] );
            if ( OTF2_SUCCESS != status )
            {
                UTILS_ERROR( status,
                             "Malformed SION rank-map file: can't read location." );
                goto out;
            }

            UTILS_DEBUG( "locations[ %u ]: %" PRIu64,
                         locations_index - 1, locations[ locations_index - 1 ] );

            i++;
        }

        rank++;
    }

    /* Read end of OTF2_BUFFER_END_OF_FILE */
    status = OTF2_Buffer_GuaranteeRead( srm_buffer, 1 );
    if ( status != OTF2_SUCCESS )
    {
        UTILS_ERROR( status,
                     "Malformed SION rank-map file: unexpected end of file." );
        goto out;
    }
    OTF2_Buffer_ReadUint8( srm_buffer, &record_type );
    if ( OTF2_BUFFER_END_OF_FILE != record_type )
    {
        status = UTILS_ERROR( OTF2_ERROR_INVALID_RECORD,
                              "Malformed SION rank-map file: expecting end of file." );
        goto out;
    }

    status     = OTF2_Buffer_Delete( srm_buffer );
    srm_buffer = NULL;
    if ( OTF2_SUCCESS != status )
    {
        UTILS_ERROR( status, "Could not close buffer." );
        goto out;
    }

    *numberOfFilesOut          = number_of_files;
    *numberOfRanksOut          = number_of_ranks;
    *fileNumberPerRankOut      = file_number_per_rank;
    *fileRankPerRankOut        = file_rank_per_rank;
    *locationsPerRankOut       = locations_per_rank;
    *totalNumberOfLocationsOut = total_number_of_locations;
    *locationsOut              = locations;

    return OTF2_SUCCESS;

out:
    free( file_number_per_rank );
    free( file_rank_per_rank );
    free( locations_per_rank );
    free( locations );
    OTF2_Buffer_Delete( srm_buffer );
    return status;
}


OTF2_ErrorCode
otf2_sion_read_rank_map( OTF2_Archive* archive,
                         uint32_t*     numberOfFilesOut,
                         uint32_t*     numberOfRanksOut,
                         uint32_t**    fileNumberPerRankOut,
                         uint32_t**    fileRankPerRankOut,
                         OTF2_IdMap**  locationToRankMap )
{
    UTILS_DEBUG_ENTRY();

    OTF2_ErrorCode    status;
    OTF2_CallbackCode callback_ret;

    UTILS_ASSERT( archive );
    UTILS_ASSERT( archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] );

    /* Phase one, collect set of locations from each rank */
    uint32_t rank;
    uint32_t size;

    callback_ret = otf2_collectives_get_size( archive,
                                              archive->global_comm_context,
                                              &size );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective get size failed." );
    }
    callback_ret = otf2_collectives_get_rank( archive,
                                              archive->global_comm_context,
                                              &rank );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective get rank failed." );
    }

    uint32_t  number_of_files           = 0;
    uint32_t  number_of_ranks           = 0;
    uint32_t* file_number_per_rank      = NULL;
    uint32_t* file_rank_per_rank        = NULL;
    uint32_t* locations_per_rank        = NULL;
    uint32_t  total_number_of_locations = 0;
    uint64_t* locations                 = NULL;

    if ( OTF2_COLLECTIVES_ROOT == rank )
    {
        status = otf2_sion_load_rank_map( archive,
                                          &number_of_files,
                                          &number_of_ranks,
                                          &file_number_per_rank,
                                          &file_rank_per_rank,
                                          &locations_per_rank,
                                          &total_number_of_locations,
                                          &locations );
    }
    callback_ret = otf2_collectives_bcast_error( archive,
                                                 archive->global_comm_context,
                                                 &status,
                                                 OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status, "Can't read SION rank-map file on root." );
    }

    callback_ret = otf2_collectives_bcast( archive,
                                           archive->global_comm_context,
                                           &number_of_files,
                                           1,
                                           OTF2_TYPE_UINT32,
                                           OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }

    callback_ret = otf2_collectives_bcast( archive,
                                           archive->global_comm_context,
                                           &number_of_ranks,
                                           1,
                                           OTF2_TYPE_UINT32,
                                           OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }

    callback_ret = otf2_collectives_bcast( archive,
                                           archive->global_comm_context,
                                           &total_number_of_locations,
                                           1,
                                           OTF2_TYPE_UINT32,
                                           OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }

    if ( OTF2_COLLECTIVES_ROOT != rank )
    {
        file_number_per_rank = calloc( number_of_ranks, sizeof( *file_number_per_rank ) );
        file_rank_per_rank   = calloc( number_of_ranks, sizeof( *file_rank_per_rank ) );
        locations_per_rank   = calloc( number_of_ranks, sizeof( *locations_per_rank ) );
        locations            = calloc( total_number_of_locations, sizeof( *locations ) );
    }

    callback_ret = otf2_collectives_bcast( archive,
                                           archive->global_comm_context,
                                           file_number_per_rank,
                                           number_of_ranks,
                                           OTF2_TYPE_UINT32,
                                           OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }

    callback_ret = otf2_collectives_bcast( archive,
                                           archive->global_comm_context,
                                           file_rank_per_rank,
                                           number_of_ranks,
                                           OTF2_TYPE_UINT32,
                                           OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }

    callback_ret = otf2_collectives_bcast( archive,
                                           archive->global_comm_context,
                                           locations_per_rank,
                                           number_of_ranks,
                                           OTF2_TYPE_UINT32,
                                           OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }

    callback_ret = otf2_collectives_bcast( archive,
                                           archive->global_comm_context,
                                           locations,
                                           total_number_of_locations,
                                           OTF2_TYPE_UINT64,
                                           OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective bcast failed." );
    }

    /* Convert data into ID map. */
    *locationToRankMap = OTF2_IdMap_Create( OTF2_ID_MAP_SPARSE,
                                            total_number_of_locations );
    if ( !*locationToRankMap )
    {
        free( file_number_per_rank );
        free( file_rank_per_rank );
        free( locations_per_rank );
        free( locations );
        return UTILS_ERROR( OTF2_ERROR_MEM_ALLOC_FAILED,
                            "Can't allocate rank-map." );
    }
    *numberOfFilesOut     = number_of_files;
    *numberOfRanksOut     = number_of_ranks;
    *fileNumberPerRankOut = file_number_per_rank;
    *fileRankPerRankOut   = file_rank_per_rank;

    uint32_t rank_i = 0;
    uint32_t i      = 0;
    while ( rank_i < number_of_ranks && locations_per_rank[ rank_i ] > 0 )
    {
        UTILS_DEBUG( "Adding rank-map entry: %u -> %u", locations[ i ], rank_i );
        otf2_id_map_append_unsorted_id_pair_sparse( *locationToRankMap,
                                                    locations[ i++ ], rank_i );
        if ( --locations_per_rank[ rank_i ] == 0 )
        {
            rank_i++;
        }
    }
    free( locations_per_rank );
    free( locations );

    otf2_id_map_sort_sparse( *locationToRankMap );

    return OTF2_SUCCESS;
}


static OTF2_ErrorCode
otf2_sion_save_rank_map( OTF2_Archive*   archive,
                         uint32_t        numberOfFiles,
                         uint32_t        numberOfRanks,
                         const uint32_t* fileNumberPerRank,
                         const uint32_t* fileRankPerRank,
                         const uint32_t* locationsPerRank,
                         uint32_t        totalNumberOfLocations,
                         const uint64_t* locations )
{
    UTILS_DEBUG_ENTRY();

    OTF2_ErrorCode status;

    UTILS_ASSERT( archive );
    UTILS_ASSERT( archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] );

    UTILS_DEBUG( "#files = %u, #ranks = %u, #locations = %u",
                 numberOfFiles, numberOfRanks, totalNumberOfLocations );

    OTF2_Buffer* srm_buffer = OTF2_Buffer_New( archive,
                                               NULL,
                                               OTF2_CHUNK_SIZE_MIN,
                                               OTF2_BUFFER_WRITE,
                                               OTF2_BUFFER_CHUNKED,
                                               OTF2_FILETYPE_SIONRANKMAP,
                                               OTF2_UNDEFINED_LOCATION );

    if ( !srm_buffer )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Can't open SION rank-map buffer!" );
    }

    /* This should never fail */
    OTF2_Buffer_WriteUint8( srm_buffer, OTF2_SION_RANK_MAP_HEADER );
    OTF2_Buffer_WriteUint32( srm_buffer, numberOfFiles );
    OTF2_Buffer_WriteUint32( srm_buffer, numberOfRanks );
    OTF2_Buffer_WriteUint32( srm_buffer, totalNumberOfLocations );

    uint32_t locations_index = 0;
    for ( uint32_t rank = 0; rank < numberOfRanks; rank++ )
    {
        UTILS_DEBUG( "rank %u: fileNumber = %u, rank in file = %u, #locations = %u",
                     rank,
                     fileNumberPerRank[ rank ],
                     fileRankPerRank[ rank ],
                     locationsPerRank[ rank ] );

        /* 1) Write rank record */
        uint64_t record_length = 1;              /* record ID */
        record_length += sizeof( uint32_t ) + 1; /* rank */
        record_length += sizeof( uint32_t ) + 1; /* file number of this rank */
        record_length += sizeof( uint32_t ) + 1; /* rank of this rank in this file */
        record_length += sizeof( uint32_t ) + 1; /* number of locations in this rank */
        status         = OTF2_Buffer_RecordRequest( srm_buffer, 0, record_length );
        if ( OTF2_SUCCESS != status )
        {
            OTF2_Buffer_Delete( srm_buffer );
            return UTILS_ERROR( status, "Could not request new chunk." );
        }

        OTF2_Buffer_WriteUint8( srm_buffer, OTF2_SION_RANK_MAP_RANK );
        OTF2_Buffer_WriteUint32( srm_buffer, rank );
        OTF2_Buffer_WriteUint32( srm_buffer, fileNumberPerRank[ rank ] );
        OTF2_Buffer_WriteUint32( srm_buffer, fileRankPerRank[ rank ] );
        OTF2_Buffer_WriteUint32( srm_buffer, locationsPerRank[ rank ] );

        /* 2) write all locations for this record. */
        for ( uint32_t i = 0; i < locationsPerRank[ rank ]; i++ )
        {
            UTILS_DEBUG( "locations[ %u ]: %" PRIu64,
                         locations_index, locations[ locations_index ] );

            record_length  = 1;                      /* record ID */
            record_length += sizeof( uint64_t ) + 1; /* location */
            status         = OTF2_Buffer_RecordRequest( srm_buffer, 0, record_length );
            if ( OTF2_SUCCESS != status )
            {
                OTF2_Buffer_Delete( srm_buffer );
                return UTILS_ERROR( status, "Could not request new chunk." );
            }
            OTF2_Buffer_WriteUint8( srm_buffer, OTF2_SION_RANK_MAP_LOCATION );
            OTF2_Buffer_WriteUint64( srm_buffer, locations[ locations_index++ ] );
        }
    }
    OTF2_Buffer_WriteUint8( srm_buffer, OTF2_BUFFER_END_OF_FILE );

    return OTF2_Buffer_Delete( srm_buffer );
}


OTF2_ErrorCode
otf2_sion_write_rank_map( OTF2_Archive* archive )
{
    UTILS_DEBUG_ENTRY();

    OTF2_ErrorCode    status;
    OTF2_CallbackCode callback_ret;

    UTILS_ASSERT( archive );
    UTILS_ASSERT( archive->per_substrate_data[ OTF2_SUBSTRATE_SION ] );

    OTF2_FileSubstrateData* data =
        archive->per_substrate_data[ OTF2_SUBSTRATE_SION ];

    /* Phase one, collect set of locations from each rank */
    uint32_t rank;
    uint32_t size;

    callback_ret = otf2_collectives_get_size( archive,
                                              archive->global_comm_context,
                                              &size );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective get size failed." );
    }
    callback_ret = otf2_collectives_get_rank( archive,
                                              archive->global_comm_context,
                                              &rank );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective get rank failed." );
    }

    /* @todo get this from the archive */
    uint32_t  number_of_locations = archive->locations_number;
    uint64_t* my_locations        = calloc( number_of_locations,
                                            sizeof( *my_locations ) );
    for ( uint32_t i = 0; i < archive->locations_number; i++ )
    {
        my_locations[ i ] = archive->locations[ i ].location_id;
    }

    uint32_t* locations_per_rank   = NULL;
    uint32_t* file_number_per_rank = NULL;
    uint32_t* file_rank_per_rank   = NULL;
    if ( OTF2_COLLECTIVES_ROOT == rank )
    {
        locations_per_rank   = calloc( size, sizeof( *locations_per_rank ) );
        file_number_per_rank = calloc( size, sizeof( *file_number_per_rank ) );
        file_rank_per_rank   = calloc( size, sizeof( *file_rank_per_rank ) );
    }

    callback_ret = otf2_collectives_gather( archive,
                                            archive->global_comm_context,
                                            &number_of_locations,
                                            locations_per_rank,
                                            1,
                                            OTF2_TYPE_UINT32,
                                            OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective gather failed." );
    }

    callback_ret = otf2_collectives_gather( archive,
                                            archive->global_comm_context,
                                            &data->file_number,
                                            file_number_per_rank,
                                            1,
                                            OTF2_TYPE_UINT32,
                                            OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective gather failed." );
    }

    callback_ret = otf2_collectives_gather( archive,
                                            archive->global_comm_context,
                                            &data->file_rank,
                                            file_rank_per_rank,
                                            1,
                                            OTF2_TYPE_UINT32,
                                            OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective gather failed." );
    }

    uint32_t  total_number_of_locations = 0;
    uint64_t* locations                 = NULL;
    if ( OTF2_COLLECTIVES_ROOT == rank )
    {
        for ( uint32_t i = 0; i < size; i++ )
        {
            total_number_of_locations += locations_per_rank[ i ];
        }
        locations = calloc( total_number_of_locations, sizeof( *locations ) );
    }
    callback_ret = otf2_collectives_gatherv( archive,
                                             archive->global_comm_context,
                                             my_locations,
                                             number_of_locations,
                                             locations,
                                             locations_per_rank,
                                             OTF2_TYPE_UINT64,
                                             OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective gatherv failed." );
    }
    free( my_locations );

    if ( OTF2_COLLECTIVES_ROOT == rank )
    {
        status = otf2_sion_save_rank_map( archive,
                                          data->number_of_files,
                                          size,
                                          file_number_per_rank,
                                          file_rank_per_rank,
                                          locations_per_rank,
                                          total_number_of_locations,
                                          locations );
        free( locations );
        free( locations_per_rank );
        free( file_number_per_rank );
        free( file_rank_per_rank );
    }
    callback_ret = otf2_collectives_bcast_error( archive,
                                                 archive->global_comm_context,
                                                 &status,
                                                 OTF2_COLLECTIVES_ROOT );
    if ( callback_ret != OTF2_CALLBACK_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_COLLECTIVE_CALLBACK,
                            "Collective gatherv failed." );
    }
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status, "Can't write SION rank-map on root." );
    }

    return status;
}
