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
 *  @internal
 *
 *  @file
 *
 *  @brief This is the local definition reader, which reads location
 *         dependend definitions, and can also be used to get the mapping
 *         information from the local definition file. Local definitions are
 *         always assigned to a location.
 *
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
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"
#include "otf2_archive_location.h"

#include "otf2_attic_types.h"
#include "otf2_attic.h"

#include "otf2_attribute_value.h"
#include "otf2_attribute_list.h"
#include "otf2_id_map.h"

#include "OTF2_File.h"
#include "OTF2_Buffer.h"

#include "otf2_local_definitions.h"

#include "otf2_def_reader.h"

/* ___ Prototypes for internal functions. ___________________________________ */



static inline OTF2_ErrorCode
otf2_def_reader_read_unknown( OTF2_DefReader* reader );


static inline void
otf2_local_def_region_convert_pre_1_1( OTF2_LocalDefRegion* record )
{
    /* If there is no additional name available use content of original name field. */
    record->canonical_name = record->name;

    otf2_attic_def_region_convert_region_type( record->region_type,
                                               &record->region_role,
                                               &record->paradigm,
                                               &record->region_flags );
}


static inline void
otf2_local_def_group_convert_pre_1_2( OTF2_LocalDefGroup* record )
{
    otf2_attic_def_group_convert_group_type_pre_1_2( record->group_type_pre12,
                                                     &record->group_type,
                                                     &record->paradigm,
                                                     &record->group_flags );
}


static inline void
otf2_local_def_metric_class_convert_pre_1_2( OTF2_LocalDefMetricClass* record )
{
    record->recorder_kind = OTF2_RECORDER_KIND_UNKNOWN;
}


static inline void
otf2_local_def_attribute_convert_pre_1_4( OTF2_LocalDefAttribute* record )
{
    record->description = OTF2_UNDEFINED_STRING;
}


static inline void
otf2_local_def_system_tree_node_property_convert_pre_2_0( OTF2_LocalDefSystemTreeNodeProperty* record )
{
    record->type            = OTF2_TYPE_STRING;
    record->value.stringRef = record->string_value;
}


static inline void
otf2_local_def_location_group_property_convert_pre_2_0( OTF2_LocalDefLocationGroupProperty* record )
{
    record->type            = OTF2_TYPE_STRING;
    record->value.stringRef = record->string_value;
}


static inline void
otf2_local_def_location_property_convert_pre_2_0( OTF2_LocalDefLocationProperty* record )
{
    record->type            = OTF2_TYPE_STRING;
    record->value.stringRef = record->string_value;
}


/* This _must_ be done for the generated part of the OTF2 local definition reader */
#include "OTF2_DefReader_inc.c"
/* This _must_ be done for the generated part of the OTF2 local definition reader */



/** @brief Creates a new definition reader object. This function is also
 *         meant to be internal, because an external programmer is supposed
 *         to use the related functions of the archive management.
 *
 *  @param archive   Handle to the calling internal archive object. This is
 *                   needed to pass the needed information this constructor
 *                   function.
 *  @param location  The location ID of the related process, for which a
 *                   local definition writer should be opened.
 *
 *  @threadsafety    Caller must hold the archive lock.
 *
 *  @return          Returns a handle to the reader if successful, NULL
 *                   otherwise.
 */

OTF2_DefReader*
otf2_def_reader_new( OTF2_Archive*    archive,
                     OTF2_LocationRef location )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( location != OTF2_UNDEFINED_LOCATION );

    /* Get the archive location index for this location. */
    OTF2_ErrorCode ret = otf2_archive_find_location( archive,
                                                     location,
                                                     NULL );
    if ( OTF2_SUCCESS != ret )
    {
        /* Location was not selected via `SelectLocation`. */
        UTILS_ERROR( OTF2_ERROR_INDEX_OUT_OF_BOUNDS,
                     "Location not selected for reading." );
        return NULL;
    }

    OTF2_DefReader* reader = ( OTF2_DefReader* )calloc( 1, sizeof( OTF2_DefReader ) );
    if ( !reader )
    {
        UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                     "Could not allocate memory for readers handle!" );
        return NULL;
    }

    reader->archive     = archive;
    reader->location_id = location;

    memset( &( reader->reader_callbacks ), '\0', sizeof( OTF2_DefReaderCallbacks ) );
    reader->user_data = NULL;

    uint64_t       chunk_size;
    OTF2_ErrorCode status = otf2_archive_get_def_chunksize( archive,
                                                            &chunk_size );
    if ( status != OTF2_SUCCESS )
    {
        free( reader );
        return NULL;
    }

    /* Generate an internal buffer */
    reader->buffer = OTF2_Buffer_New( archive,
                                      reader,
                                      chunk_size,
                                      OTF2_BUFFER_READ,
                                      OTF2_BUFFER_CHUNKED,
                                      OTF2_FILETYPE_LOCAL_DEFS,
                                      location );

    if ( !reader->buffer )
    {
        UTILS_ERROR( OTF2_ERROR_PROCESSED_WITH_FAULTS,
                     "Creation of buffer handle failed!" );
        free( reader );
        return NULL;
    }

    return reader;
}



/** @brief Deletes a definition reader object. This function is also
 *         meant to be internal, because all reader objectes are deleted
 *         on deconstruction time of the archive object.
 *
 *  @param reader          This given reader object will be deleted.
 *
 *  @return                Returns OTF2_SUCCESS if successful, an error code
 *                         if an error occurs.
 */

OTF2_ErrorCode
otf2_def_reader_delete( OTF2_DefReader* reader )
{
    if ( reader )
    {
        OTF2_ErrorCode ret
            = OTF2_Buffer_Delete( reader->buffer );
        if ( ret != OTF2_SUCCESS )
        {
            return UTILS_ERROR( ret, "Buffer deletion failed!" );
        }

        free( reader );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_def_reader_open_file( OTF2_DefReader* reader )
{
    if ( !reader )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Reader object is not valid!" );
    }

    return otf2_buffer_open_file( reader->buffer );
}


OTF2_ErrorCode
OTF2_DefReader_GetLocationID( const OTF2_DefReader* reader,
                              OTF2_LocationRef*     location )
{
    if ( !reader )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Reader Object is not valid!" );
    }

    *location = reader->location_id;
    return OTF2_SUCCESS;
}


OTF2_DefReaderCallbacks*
OTF2_DefReaderCallbacks_New( void )
{
    return calloc( 1, sizeof( OTF2_DefReaderCallbacks ) );
}


void
OTF2_DefReaderCallbacks_Delete( OTF2_DefReaderCallbacks* defReaderCallbacks )
{
    free( defReaderCallbacks );
}


void
OTF2_DefReaderCallbacks_Clear( OTF2_DefReaderCallbacks* defReaderCallbacks )
{
    if ( defReaderCallbacks != NULL )
    {
        memset( defReaderCallbacks, 0, sizeof( OTF2_DefReaderCallbacks ) );
    }
}


OTF2_ErrorCode
OTF2_DefReader_SetCallbacks( OTF2_DefReader*                reader,
                             const OTF2_DefReaderCallbacks* callbacks,
                             void*                          userData )
{
    /* Check if this is a valid reader object */
    if ( !reader )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "No valid reader object!" );
    }

    if ( NULL == callbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid callback arguments!" );
    }

    /* Set internal callback struct and userdata */
    memcpy( &reader->reader_callbacks,
            callbacks,
            sizeof( reader->reader_callbacks ) );
    reader->user_data = userData;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetUnknownCallback( OTF2_DefReaderCallbacks*       defReaderCallbacks,
                                            OTF2_DefReaderCallback_Unknown unknownCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->unknown = unknownCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReader_ReadDefinitions( OTF2_DefReader* reader,
                                uint64_t        recordsToRead,
                                uint64_t*       recordsRead )
{
    /* Check if this is a valid reader object */
    if ( !reader )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "No valid reader object!" );
    }

    OTF2_ErrorCode ret = OTF2_SUCCESS;
    uint64_t       read;
    for ( read = 0; read < recordsToRead; read++ )
    {
        ret = otf2_def_reader_read( reader );
        if ( ret != OTF2_SUCCESS )
        {
            /* This is the end of file, which is not an error. */
            if ( ret == OTF2_ERROR_INDEX_OUT_OF_BOUNDS )
            {
                ret = OTF2_SUCCESS;
            }
            else if ( ret == OTF2_ERROR_INTERRUPTED_BY_CALLBACK
                      || ret == OTF2_ERROR_DUPLICATE_MAPPING_TABLE )
            {
                /* We still delivered a record, count it */
                read++;
            }
            break;
        }
    }

    *recordsRead = read;
    return ret;
}


/** @brief Reads a definition record of unknown type and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_unknown( OTF2_DefReader* reader )
{
    /* Get record length and test memory availability */
    uint64_t       record_length;
    OTF2_ErrorCode ret;
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read definition record of unknown type." );
    }

    /* Skip record */
    ret = OTF2_Buffer_Skip( reader->buffer, record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not skip definition record of unknown type." );
    }

    /* Trigger callback */
    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.unknown )
    {
        interrupt = reader->reader_callbacks.unknown( reader->user_data );
    }

    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}
