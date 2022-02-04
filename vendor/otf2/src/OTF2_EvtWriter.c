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

/**
 * @internal
 *
 * @file
 *
 * @brief       This layer provides write routines to store event records of a
 *              single location to an OTF2 memory buffer.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_lock.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"

#include "otf2_attic_types.h"

#include "otf2_attribute_value.h"
#include "otf2_attribute_list.h"
#include "otf2_id_map.h"

#include "OTF2_File.h"
#include "OTF2_Buffer.h"

#include "otf2_events_int.h"

#include "otf2_evt_writer.h"

/* This _must_ be done for the generated part of the OTF2 local writer */
#include "OTF2_EvtWriter_inc.c"
/* This _must_ be done for the generated part of the OTF2 local writer */


static OTF2_ErrorCode
otf2_evt_writer_set_location_id( OTF2_EvtWriter*  writer,
                                 OTF2_LocationRef location );


/** @brief Create a new local event writer.
 *
 *  @param archive          The archive.
 *  @param location         The Location ID is needed to name the trace file.
 *
 *  @threadsafety           Caller must hold the archive lock.
 *
 *  @return                 Returns a handle to the event writer if successful,
 *                          NULL otherwise.
 */

OTF2_EvtWriter*
otf2_evt_writer_new( OTF2_Archive*    archive,
                     OTF2_LocationRef location )
{
    UTILS_ASSERT( archive );

    OTF2_EvtWriter* writer = NULL;
    OTF2_ErrorCode  status;

    /* Allocate new writer */
    writer = ( OTF2_EvtWriter* )calloc( 1, sizeof( OTF2_EvtWriter ) );
    if ( !writer )
    {
        goto clean_mem_new;
    }

    uint64_t chunk_size;
    status = otf2_archive_get_event_chunksize( archive,
                                               &chunk_size );
    if ( status != OTF2_SUCCESS )
    {
        goto clean_mem_new;
    }

    /* Allocate the buffer module */
    writer->buffer = OTF2_Buffer_New( archive,
                                      writer,
                                      chunk_size,
                                      OTF2_BUFFER_WRITE,
                                      OTF2_BUFFER_CHUNKED,
                                      OTF2_FILETYPE_EVENTS,
                                      location );

    if ( !writer->buffer )
    {
        goto clean_mem_new;
    }

    /* Initialize the variables */
    writer->archive     = archive;
    writer->location_id = OTF2_UNDEFINED_LOCATION;

    /* Put the location ID into the writer */
    if ( location != OTF2_UNDEFINED_LOCATION )
    {
        status = otf2_evt_writer_set_location_id( writer, location );
        if ( OTF2_SUCCESS != status )
        {
            UTILS_ERROR( status, "Can't set location id." );
            goto clean_mem_new;
        }
    }

    return writer;

clean_mem_new:

    free( writer );

    return NULL;
}



/** @brief
 *  Releases the resources associated with the given local event writer handle.
 *  Does nothing if a NULL pointer is provided.
 *
 *  @param writer   Writer object which has to be deleted.
 *
 *  @return OTF2_ErrorCode with !=OTF2_SUCCESS if there was an error.
 */

OTF2_ErrorCode
otf2_evt_writer_delete( OTF2_EvtWriter* writer )
{
    if ( !writer )
    {
        return OTF2_SUCCESS;
    }

    if ( writer->location_id == OTF2_UNDEFINED_LOCATION )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Invalid location ID in deletion!" );
    }

    OTF2_ErrorCode ret;
    OTF2_Buffer_WriteUint8( writer->buffer, OTF2_BUFFER_END_OF_FILE );

    ret = OTF2_Buffer_Delete( writer->buffer );
    if ( ret != OTF2_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Buffer deletion failed!" );
    }

    free( writer );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EvtWriter_GetLocationID( const OTF2_EvtWriter* writer,
                              OTF2_LocationRef*     location )
{
    if ( !writer )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Writer Object is not valid!" );
    }

    *location = writer->location_id;
    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EvtWriter_SetUserData( OTF2_EvtWriter* writer,
                            void*           userData )
{
    if ( !writer )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writer argument!" );
    }

    writer->user_data = userData;
    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EvtWriter_GetUserData( const OTF2_EvtWriter* writer,
                            void**                userData )
{
    if ( !writer )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writer argument!" );
    }
    if ( !userData )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid userData argument!" );
    }

    *userData = writer->user_data;
    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EvtWriter_SetLocationID( OTF2_EvtWriter*  writer,
                              OTF2_LocationRef location )
{
    /* Some initial argument checks */
    if ( writer == NULL )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Writer Object is not valid!" );
    }

    if ( location == OTF2_UNDEFINED_LOCATION )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid location id given." );
    }

    if ( writer->location_id != OTF2_UNDEFINED_LOCATION )
    {
        if ( writer->location_id != location )
        {
            return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                                "Location ID was already set! %" PRIu64,
                                location );
        }
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status;

    OTF2_ARCHIVE_LOCK( writer->archive );

    status = otf2_evt_writer_set_location_id( writer,
                                              location );

    OTF2_ARCHIVE_UNLOCK( writer->archive );

    return status;
}


OTF2_ErrorCode
OTF2_EvtWriter_GetNumberOfEvents( OTF2_EvtWriter* writer,
                                  uint64_t*       numberOfEvents )
{
    if ( NULL == writer )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid event writer handle!" );
    }

    uint64_t       buffer_first_event;
    uint64_t       buffer_last_event;
    OTF2_ErrorCode status = OTF2_Buffer_GetNumberEvents(
        writer->buffer,
        &buffer_first_event,
        &buffer_last_event );

    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status, "Can't get number of events from buffer." );
    }

    *numberOfEvents = buffer_first_event;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EvtWriter_StoreRewindPoint( OTF2_EvtWriter* writerHandle,
                                 uint32_t        rewindId )
{
    if ( NULL == writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid event writer handle!" );
    }

    return OTF2_Buffer_StoreRewindPoint( writerHandle->buffer, rewindId );
}


OTF2_ErrorCode
OTF2_EvtWriter_ClearRewindPoint( OTF2_EvtWriter* writerHandle,
                                 uint32_t        rewindId )
{
    if ( NULL == writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid event writer handle!" );
    }

    return OTF2_Buffer_ClearRewindPoint( writerHandle->buffer, rewindId );
}


OTF2_ErrorCode
OTF2_EvtWriter_Rewind( OTF2_EvtWriter* writerHandle,
                       uint32_t        rewindId )
{
    if ( NULL == writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid event writer handle!" );
    }

    return OTF2_Buffer_Rewind( writerHandle->buffer, rewindId );
}


/**
 *  @threadsafety  Caller must hold the archive lock.
 */
OTF2_ErrorCode
otf2_evt_writer_set_location_id( OTF2_EvtWriter*  writer,
                                 OTF2_LocationRef location )
{
    UTILS_ASSERT( writer );
    UTILS_ASSERT( writer->buffer );
    UTILS_ASSERT( location != OTF2_UNDEFINED_LOCATION );

    /* Set location ID */
    writer->location_id = location;

    /* Set the location ID in the buffer module */
    OTF2_ErrorCode ret = OTF2_Buffer_SetLocationID( writer->buffer,
                                                    writer->location_id );
    if ( ret != OTF2_SUCCESS )
    {
        return UTILS_ERROR( ret, "Can't reset file path for local trace!" );
    }

    /* Add location ID to locations vector. */
    ret = otf2_archive_add_location( writer->archive,
                                     writer->location_id );
    if ( ret != OTF2_SUCCESS )
    {
        return UTILS_ERROR( ret, "Failed to add new location!" );
    }

    return OTF2_SUCCESS;
}
