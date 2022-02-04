/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015,
 * Technische Universitaet Dresden, Germany
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
 *  @source     templates/OTF2_EventSizeEstimator.tmpl.c
 *
 *  @brief      Provides a interface to estimate the size of a resulting trace
 *              file.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <limits.h>

#include <otf2/otf2.h>
#include <otf2/OTF2_EventSizeEstimator.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"

#include "otf2_attic_types.h"

#include "otf2_attribute_value.h"
#include "otf2_attribute_list.h"
#include "otf2_id_map.h"

#include "OTF2_File.h"
#include "OTF2_Buffer.h"

struct OTF2_EventSizeEstimator
{
    @otf2 for entry in MappingType.entries:
    @@entry.def.self_attr.type.c_orig@@ number_of_@@entry.def.ref_lower@@_definitions;
    uint8_t                             estimate_for_@@entry.def.ref_lower@@s;
    @otf2 endfor
};

OTF2_EventSizeEstimator*
OTF2_EventSizeEstimator_New( void )
{
    OTF2_EventSizeEstimator* new_estimator = calloc( 1, sizeof( *new_estimator ) );
    if ( !new_estimator )
    {
        UTILS_ERROR( OTF2_ERROR_MEM_ALLOC_FAILED,
                     "Couldn't allocate estimator object." );
        return NULL;
    }

    @otf2 for entry in MappingType.entries:
    new_estimator->number_of_@@entry.def.ref_lower@@_definitions = 0;
    new_estimator->estimate_for_@@entry.def.ref_lower@@s = @@entry.def.self_attr.size()@@;
    @otf2 endfor

    return new_estimator;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_Delete( OTF2_EventSizeEstimator* estimator )
{
    free( estimator );

    return OTF2_SUCCESS;
}

@otf2 for entry in MappingType.entries:

OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOf@@entry.def.ref_name@@Definitions( OTF2_EventSizeEstimator* estimator,
                                                                      @@entry.def.self_attr.type.c_orig@@ numberOf@@entry.def.ref_name@@Definitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOf@@entry.def.ref_name@@Definitions )
    {
        estimator->estimate_for_@@entry.def.ref_lower@@s = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_@@entry.def.ref_lower@@_definitions = numberOf@@entry.def.ref_name@@Definitions;
    @otf2 if entry.def.ref_name == 'Location':
    /* Location ids do not need to be consecutive, thus we can't use the
       given number to estimate the size for all possible ids. */
    @otf2 else
    estimator->estimate_for_@@entry.def.ref_lower@@s = otf2_buffer_size_@@entry.def.self_attr.type.lower@@( numberOf@@entry.def.ref_name@@Definitions - 1 );
    @otf2 endif

    return OTF2_SUCCESS;
}

@otf2 endfor

size_t
OTF2_EventSizeEstimator_GetDefChunkSize( OTF2_EventSizeEstimator* estimator )
{
    if ( !estimator )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid estimator argument." );
        /* You get what you deserve */
        return 0;
    }

    /*
     * We estimate a full chunk, including the chunk header (magic + endianess, see
     * otf2_buffer_write_header) and footer (OTF2_BUFFER_END_OF_CHUNK). Thus
     * we start with these 3 bytes.
     */
    uint64_t record_length = 3;

    /* We currently only estimate the size of a Group definition. Though, we
       need to take the definition with the max estimate. */

    /*
     * Maximum record length is the record id byte, plus the individual sizes
     * of the attributes.
     */
    record_length += 1;

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    record_data_length += sizeof( OTF2_GroupRef ) + 1;
    record_data_length += sizeof( OTF2_StringRef ) + 1;
    record_data_length += sizeof( OTF2_GroupTypePre12 );
    record_data_length += sizeof( uint32_t ) + 1;
    record_data_length += sizeof( OTF2_GroupType );
    record_data_length += sizeof( OTF2_Paradigm );
    record_data_length += sizeof( OTF2_GroupFlag ) + 1;

    /* Start with the locations */
    size_t array_size     = estimator->number_of_location_definitions * estimator->estimate_for_locations;
    size_t max_array_size = array_size;

    array_size = estimator->number_of_region_definitions * estimator->estimate_for_regions;
    if ( array_size > max_array_size )
    {
        max_array_size = array_size;
    }

    array_size = estimator->number_of_metric_definitions * estimator->estimate_for_metrics;
    if ( array_size > max_array_size )
    {
        max_array_size = array_size;
    }

    /* Add the maximum array size to the record data */
    record_data_length += max_array_size;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /* We round up to the next OTF2_CHUNK_SIZE_MIN */
    size_t total_estimate = OTF2_CHUNK_SIZE_MIN * ( ( record_length + OTF2_CHUNK_SIZE_MIN ) / OTF2_CHUNK_SIZE_MIN );

    /*
     * Return 0 if the estimate is larger than the current OTF2_CHUNK_SIZE_MAX.
     * Keep the check inside this function, as this may ease a change of
     * OTF2_CHUNK_SIZE_MAX in the future.
     */
    if ( OTF2_CHUNK_SIZE_MAX < total_estimate )
    {
        return 0;
    }
    return total_estimate;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfTimestamp( OTF2_EventSizeEstimator* estimator )
{
    if ( !estimator )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid estimator argument." );
        /* You get what you deserve */
        return 0;
    }

    return OTF2_BUFFER_TIMESTAMP_SIZE;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfAttributeList( const OTF2_EventSizeEstimator* estimator,
                                                const OTF2_AttributeList*      attributeList )
{
    if ( !estimator )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid estimator argument." );
        /* You get what you deserve */
        return 0;
    }

    if ( NULL == attributeList || 0 == attributeList->capacity )
    {
        return 0;
    }

    /* start with the record id */
    size_t size = 1;

    /* now determine how many bytes we use for the record length */
    size += 1;
    if ( otf2_attribute_list_get_data_size( attributeList ) >= UINT8_MAX )
    {
        size += 8;
    }

    /* we continue with the number of elements in the list */
    size += otf2_buffer_size_uint32( attributeList->capacity );

    /* now iterate over all entries and consider the attribute definition reference
     * estimate and for the values the respective estimates */
    for ( const otf2_attribute* attr = attributeList->head; attr; attr = attr->next )
    {
        /* The attribute reference */
        size += estimator->estimate_for_attributes;

        /* The attribute type */
        size += 1;

        switch ( attr->type_id )
        {
            @otf2 for entry in Type.entries if entry.type:
            case @@entry.name@@:
                size += @@entry.type.size()@@;
                break;

            @otf2 endfor
            @otf2 for entry in MappingType.entries:
            @otf2 with def = entry.def
            case @@Type.prefix@@@@entry.suffix@@:
                size += estimator->estimate_for_@@def.ref_lower@@s;
                break;

            @otf2 endwith
            @otf2 endfor
            default:
                /*
                 * When getting an unknown type, we will write it nevertheless as an
                 * uint64, because we assume that future additions will always be
                 * definition references and they are always of compressed types.
                 */
                size += sizeof( uint64_t ) + 1;
                break;
        }
    }

    return size;
}

@otf2 for event in events:

size_t
OTF2_EventSizeEstimator_GetSizeOf@@event.name@@Event( OTF2_EventSizeEstimator* estimator@@event.funcargs(attributes=event.array_attributes, resolve_array_attributes=False)@@ )
{
    if ( !estimator )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "Invalid estimator argument." );
        /* You get what you deserve */
        return 0;
    }

    /* account the record id */
    size_t record_length = 1;

    /* Calculate the real upper bound, which is used for the record-length-byte
     * calculation
     */
    size_t record_data_length = 0;
    @otf2  for attr in event.attributes:
    record_data_length += @@attr.size()@@; /* @@attr.name@@ */
    @otf2   if attr is array_attr:
    @otf2    for array_attr in attr.array_attributes:
    record_data_length += @@attr.name@@ * ( @@array_attr.size()@@ ); /* @@array_attr.name@@ */
    @otf2    endfor
    @otf2   endif
    @otf2  endfor

    @otf2  if event is not singleton_event:
    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    @otf2  endif

    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length = 0;
    @otf2  for attr in event.attributes:
    @otf2   if attr is def_ref_attr and attr.mapping_type:
    record_data_length += estimator->estimate_for_@@attr.def_ref.ref_lower@@s; /* @@attr.name@@ */
    @otf2   else:
    record_data_length += @@attr.size()@@; /* @@attr.name@@ */
    @otf2    if attr is array_attr:
    @otf2     for array_attr in attr.array_attributes:
    @otf2      if array_attr is def_ref_attr and attr.mapping_type:
    record_data_length += @@attr.name@@ * ( estimator->estimate_for_@@array_attr.def_ref.ref_lower@@s ); /* @@array_attr.name@@ */
    @otf2      else:
    record_data_length += @@attr.name@@ * ( @@array_attr.size()@@ ); /* @@array_attr.name@@ */
    @otf2      endif
    @otf2     endfor
    @otf2    endif
    @otf2   endif
    @otf2  endfor

    return record_length + record_data_length;
}

@otf2 endfor
