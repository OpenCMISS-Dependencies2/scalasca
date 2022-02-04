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


/**
 * @internal
 *
 * @file
 *
 * @brief The archive location.
 */


#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"
#include "otf2_archive_location.h"


void
otf2_archive_location_initialize( otf2_archive_location* archiveLocation,
                                  OTF2_LocationRef       location )
{
    UTILS_ASSERT( archiveLocation );

    /* memory was allcoated with realloc, which does not zero the new memory. */
    memset( archiveLocation, 0, sizeof( *archiveLocation ) );
    archiveLocation->location_id = location;
}


void
otf2_archive_location_finalize( otf2_archive_location* archiveLocation )
{
    for ( uint64_t i = 0; i < OTF2_MAPPING_MAX; i++ )
    {
        OTF2_IdMap_Free( ( OTF2_IdMap* )archiveLocation->mapping_tables[ i ] );
        archiveLocation->mapping_tables[ i ] = NULL;
    }

    while ( archiveLocation->clock_intervals )
    {
        otf2_clock_interval* next = archiveLocation->clock_intervals->next;
        free( archiveLocation->clock_intervals );
        archiveLocation->clock_intervals = next;
    }

    if ( archiveLocation->pending_clock_interval )
    {
        free( archiveLocation->pending_clock_interval );
        archiveLocation->pending_clock_interval = NULL;
    }
}


OTF2_ErrorCode
otf2_archive_location_set_mapping_table( OTF2_Archive*     archive,
                                         OTF2_LocationRef  location,
                                         OTF2_MappingType  mapType,
                                         const OTF2_IdMap* iDMap )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( iDMap );

    if ( mapType >= OTF2_MAPPING_MAX )
    {
        /* We need to ignore unknown mapping types */
        return OTF2_SUCCESS;
    }

    /* these two calls should never fail, the existence was already checked by
     * the calling object (see otf2_def_reader_new) */
    uint32_t archive_location_index;
    otf2_archive_find_location( archive,
                                location,
                                &archive_location_index );

    otf2_archive_location* archive_location;
    otf2_archive_get_location( archive,
                               archive_location_index,
                               &archive_location );

    if ( archive_location->mapping_tables[ mapType ] != NULL )
    {
        return UTILS_ERROR( OTF2_ERROR_DUPLICATE_MAPPING_TABLE,
                            "Mapping %hhu was already set!", mapType );
    }

    archive_location->mapping_tables[ mapType ] = iDMap;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_archive_location_add_clock_offset( OTF2_Archive*    archive,
                                        OTF2_LocationRef location,
                                        uint64_t         time,
                                        int64_t          offset,
                                        double           stdDev )
{
    UTILS_ASSERT( archive );

    /* these two calls should never fail, the existence was already checked by
     * the calling object (see otf2_def_reader_new) */
    uint32_t archive_location_index;
    otf2_archive_find_location( archive,
                                location,
                                &archive_location_index );

    otf2_archive_location* archive_location;
    otf2_archive_get_location( archive,
                               archive_location_index,
                               &archive_location );

    /* if there is a pending timer synchronization interval, complete it */
    if ( archive_location->pending_clock_interval != NULL )
    {
        /* complete the pending timer synchronization interval */
        otf2_clock_interval* new_interval = archive_location->pending_clock_interval;
        archive_location->pending_clock_interval = NULL;

        /* Multiple timer synchronization at the same time are a fault */
        if ( new_interval->interval_begin >= time )
        {
            free( new_interval );
            return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                                "Multiple timer synchronization at the same time" );
        }

        new_interval->interval_end = time;
        /*
         * Note: Convert the difference to double, not the individual values to
         * reduce loss of precision.
         */
        new_interval->slope = ( double )( offset - new_interval->offset )
                              / ( double )( time - new_interval->interval_begin );
        if ( NULL == archive_location->tail_clock_intervals )
        {
            archive_location->tail_clock_intervals = &archive_location->clock_intervals;
        }
        *archive_location->tail_clock_intervals = new_interval;
        archive_location->tail_clock_intervals  = &new_interval->next;
    }

    /* store this time synchronization in a pending interval */
    otf2_clock_interval* new_pending_interval = calloc( 1, sizeof( *new_pending_interval ) );
    if ( !new_pending_interval )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_ALLOC_FAILED,
                            "Can't allocate memory for timer interval" );
    }
    new_pending_interval->next           = NULL;
    new_pending_interval->interval_begin = time;
    new_pending_interval->interval_end   = 0;
    new_pending_interval->offset         = offset;
    new_pending_interval->slope          = 0.0;

    archive_location->pending_clock_interval = new_pending_interval;

    return OTF2_SUCCESS;
}
