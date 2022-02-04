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
    uint32_t number_of_string_definitions;
    uint8_t  estimate_for_strings;
    uint32_t number_of_attribute_definitions;
    uint8_t  estimate_for_attributes;
    uint64_t number_of_location_definitions;
    uint8_t  estimate_for_locations;
    uint32_t number_of_region_definitions;
    uint8_t  estimate_for_regions;
    uint32_t number_of_group_definitions;
    uint8_t  estimate_for_groups;
    uint32_t number_of_metric_definitions;
    uint8_t  estimate_for_metrics;
    uint32_t number_of_comm_definitions;
    uint8_t  estimate_for_comms;
    uint32_t number_of_parameter_definitions;
    uint8_t  estimate_for_parameters;
    uint32_t number_of_rma_win_definitions;
    uint8_t  estimate_for_rma_wins;
    uint32_t number_of_source_code_location_definitions;
    uint8_t  estimate_for_source_code_locations;
    uint32_t number_of_calling_context_definitions;
    uint8_t  estimate_for_calling_contexts;
    uint32_t number_of_interrupt_generator_definitions;
    uint8_t  estimate_for_interrupt_generators;
    uint32_t number_of_io_file_definitions;
    uint8_t  estimate_for_io_files;
    uint32_t number_of_io_handle_definitions;
    uint8_t  estimate_for_io_handles;
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

    new_estimator->number_of_string_definitions               = 0;
    new_estimator->estimate_for_strings                       = sizeof( OTF2_StringRef ) + 1;
    new_estimator->number_of_attribute_definitions            = 0;
    new_estimator->estimate_for_attributes                    = sizeof( OTF2_AttributeRef ) + 1;
    new_estimator->number_of_location_definitions             = 0;
    new_estimator->estimate_for_locations                     = sizeof( OTF2_LocationRef ) + 1;
    new_estimator->number_of_region_definitions               = 0;
    new_estimator->estimate_for_regions                       = sizeof( OTF2_RegionRef ) + 1;
    new_estimator->number_of_group_definitions                = 0;
    new_estimator->estimate_for_groups                        = sizeof( OTF2_GroupRef ) + 1;
    new_estimator->number_of_metric_definitions               = 0;
    new_estimator->estimate_for_metrics                       = sizeof( OTF2_MetricRef ) + 1;
    new_estimator->number_of_comm_definitions                 = 0;
    new_estimator->estimate_for_comms                         = sizeof( OTF2_CommRef ) + 1;
    new_estimator->number_of_parameter_definitions            = 0;
    new_estimator->estimate_for_parameters                    = sizeof( OTF2_ParameterRef ) + 1;
    new_estimator->number_of_rma_win_definitions              = 0;
    new_estimator->estimate_for_rma_wins                      = sizeof( OTF2_RmaWinRef ) + 1;
    new_estimator->number_of_source_code_location_definitions = 0;
    new_estimator->estimate_for_source_code_locations         = sizeof( OTF2_SourceCodeLocationRef ) + 1;
    new_estimator->number_of_calling_context_definitions      = 0;
    new_estimator->estimate_for_calling_contexts              = sizeof( OTF2_CallingContextRef ) + 1;
    new_estimator->number_of_interrupt_generator_definitions  = 0;
    new_estimator->estimate_for_interrupt_generators          = sizeof( OTF2_InterruptGeneratorRef ) + 1;
    new_estimator->number_of_io_file_definitions              = 0;
    new_estimator->estimate_for_io_files                      = sizeof( OTF2_IoFileRef ) + 1;
    new_estimator->number_of_io_handle_definitions            = 0;
    new_estimator->estimate_for_io_handles                    = sizeof( OTF2_IoHandleRef ) + 1;

    return new_estimator;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_Delete( OTF2_EventSizeEstimator* estimator )
{
    free( estimator );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfStringDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfStringDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfStringDefinitions )
    {
        estimator->estimate_for_strings = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_string_definitions = numberOfStringDefinitions;
    estimator->estimate_for_strings         = otf2_buffer_size_uint32( numberOfStringDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfAttributeDefinitions( OTF2_EventSizeEstimator* estimator,
                                                         uint32_t                 numberOfAttributeDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfAttributeDefinitions )
    {
        estimator->estimate_for_attributes = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_attribute_definitions = numberOfAttributeDefinitions;
    estimator->estimate_for_attributes         = otf2_buffer_size_uint32( numberOfAttributeDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfLocationDefinitions( OTF2_EventSizeEstimator* estimator,
                                                        uint64_t                 numberOfLocationDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfLocationDefinitions )
    {
        estimator->estimate_for_locations = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_location_definitions = numberOfLocationDefinitions;
    /* Location ids do not need to be consecutive, thus we can't use the
       given number to estimate the size for all possible ids. */

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfRegionDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfRegionDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfRegionDefinitions )
    {
        estimator->estimate_for_regions = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_region_definitions = numberOfRegionDefinitions;
    estimator->estimate_for_regions         = otf2_buffer_size_uint32( numberOfRegionDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfGroupDefinitions( OTF2_EventSizeEstimator* estimator,
                                                     uint32_t                 numberOfGroupDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfGroupDefinitions )
    {
        estimator->estimate_for_groups = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_group_definitions = numberOfGroupDefinitions;
    estimator->estimate_for_groups         = otf2_buffer_size_uint32( numberOfGroupDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfMetricDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfMetricDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfMetricDefinitions )
    {
        estimator->estimate_for_metrics = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_metric_definitions = numberOfMetricDefinitions;
    estimator->estimate_for_metrics         = otf2_buffer_size_uint32( numberOfMetricDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfCommDefinitions( OTF2_EventSizeEstimator* estimator,
                                                    uint32_t                 numberOfCommDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfCommDefinitions )
    {
        estimator->estimate_for_comms = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_comm_definitions = numberOfCommDefinitions;
    estimator->estimate_for_comms         = otf2_buffer_size_uint32( numberOfCommDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfParameterDefinitions( OTF2_EventSizeEstimator* estimator,
                                                         uint32_t                 numberOfParameterDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfParameterDefinitions )
    {
        estimator->estimate_for_parameters = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_parameter_definitions = numberOfParameterDefinitions;
    estimator->estimate_for_parameters         = otf2_buffer_size_uint32( numberOfParameterDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfRmaWinDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfRmaWinDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfRmaWinDefinitions )
    {
        estimator->estimate_for_rma_wins = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_rma_win_definitions = numberOfRmaWinDefinitions;
    estimator->estimate_for_rma_wins         = otf2_buffer_size_uint32( numberOfRmaWinDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfSourceCodeLocationDefinitions( OTF2_EventSizeEstimator* estimator,
                                                                  uint32_t                 numberOfSourceCodeLocationDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfSourceCodeLocationDefinitions )
    {
        estimator->estimate_for_source_code_locations = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_source_code_location_definitions = numberOfSourceCodeLocationDefinitions;
    estimator->estimate_for_source_code_locations         = otf2_buffer_size_uint32( numberOfSourceCodeLocationDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfCallingContextDefinitions( OTF2_EventSizeEstimator* estimator,
                                                              uint32_t                 numberOfCallingContextDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfCallingContextDefinitions )
    {
        estimator->estimate_for_calling_contexts = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_calling_context_definitions = numberOfCallingContextDefinitions;
    estimator->estimate_for_calling_contexts         = otf2_buffer_size_uint32( numberOfCallingContextDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfInterruptGeneratorDefinitions( OTF2_EventSizeEstimator* estimator,
                                                                  uint32_t                 numberOfInterruptGeneratorDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfInterruptGeneratorDefinitions )
    {
        estimator->estimate_for_interrupt_generators = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_interrupt_generator_definitions = numberOfInterruptGeneratorDefinitions;
    estimator->estimate_for_interrupt_generators         = otf2_buffer_size_uint32( numberOfInterruptGeneratorDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfIoFileDefinitions( OTF2_EventSizeEstimator* estimator,
                                                      uint32_t                 numberOfIoFileDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfIoFileDefinitions )
    {
        estimator->estimate_for_io_files = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_io_file_definitions = numberOfIoFileDefinitions;
    estimator->estimate_for_io_files         = otf2_buffer_size_uint32( numberOfIoFileDefinitions - 1 );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOfIoHandleDefinitions( OTF2_EventSizeEstimator* estimator,
                                                        uint32_t                 numberOfIoHandleDefinitions )
{
    if ( !estimator )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid estimator arguemnt." );
    }

    /* when zero is given, we need 1 byte for the OTF2_UNDEFINED  */
    if ( 0 == numberOfIoHandleDefinitions )
    {
        estimator->estimate_for_io_handles = 1;
        return OTF2_SUCCESS;
    }

    estimator->number_of_io_handle_definitions = numberOfIoHandleDefinitions;
    estimator->estimate_for_io_handles         = otf2_buffer_size_uint32( numberOfIoHandleDefinitions - 1 );

    return OTF2_SUCCESS;
}


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
            case OTF2_TYPE_UINT8:
                size += sizeof( uint8_t );
                break;

            case OTF2_TYPE_UINT16:
                size += sizeof( uint16_t );
                break;

            case OTF2_TYPE_UINT32:
                size += sizeof( uint32_t ) + 1;
                break;

            case OTF2_TYPE_UINT64:
                size += sizeof( uint64_t ) + 1;
                break;

            case OTF2_TYPE_INT8:
                size += sizeof( int8_t );
                break;

            case OTF2_TYPE_INT16:
                size += sizeof( int16_t );
                break;

            case OTF2_TYPE_INT32:
                size += sizeof( int32_t ) + 1;
                break;

            case OTF2_TYPE_INT64:
                size += sizeof( int64_t ) + 1;
                break;

            case OTF2_TYPE_FLOAT:
                size += sizeof( float );
                break;

            case OTF2_TYPE_DOUBLE:
                size += sizeof( double );
                break;

            case OTF2_TYPE_STRING:
                size += estimator->estimate_for_strings;
                break;

            case OTF2_TYPE_ATTRIBUTE:
                size += estimator->estimate_for_attributes;
                break;

            case OTF2_TYPE_LOCATION:
                size += estimator->estimate_for_locations;
                break;

            case OTF2_TYPE_REGION:
                size += estimator->estimate_for_regions;
                break;

            case OTF2_TYPE_GROUP:
                size += estimator->estimate_for_groups;
                break;

            case OTF2_TYPE_METRIC:
                size += estimator->estimate_for_metrics;
                break;

            case OTF2_TYPE_COMM:
                size += estimator->estimate_for_comms;
                break;

            case OTF2_TYPE_PARAMETER:
                size += estimator->estimate_for_parameters;
                break;

            case OTF2_TYPE_RMA_WIN:
                size += estimator->estimate_for_rma_wins;
                break;

            case OTF2_TYPE_SOURCE_CODE_LOCATION:
                size += estimator->estimate_for_source_code_locations;
                break;

            case OTF2_TYPE_CALLING_CONTEXT:
                size += estimator->estimate_for_calling_contexts;
                break;

            case OTF2_TYPE_INTERRUPT_GENERATOR:
                size += estimator->estimate_for_interrupt_generators;
                break;

            case OTF2_TYPE_IO_FILE:
                size += estimator->estimate_for_io_files;
                break;

            case OTF2_TYPE_IO_HANDLE:
                size += estimator->estimate_for_io_handles;
                break;

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


size_t
OTF2_EventSizeEstimator_GetSizeOfBufferFlushEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_TimeStamp ); /* stopTime */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_TimeStamp ); /* stopTime */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMeasurementOnOffEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_MeasurementMode ); /* measurementMode */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_MeasurementMode ); /* measurementMode */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfEnterEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RegionRef ) + 1; /* region */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_regions; /* region */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfLeaveEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RegionRef ) + 1; /* region */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_regions; /* region */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiSendEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint32_t ) + 1;     /* receiver */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint32_t ) + 1;        /* receiver */
    record_data_length += estimator->estimate_for_comms; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;        /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;        /* msgLength */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiIsendEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint32_t ) + 1;     /* receiver */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    record_data_length += sizeof( uint64_t ) + 1;     /* requestID */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint32_t ) + 1;        /* receiver */
    record_data_length += estimator->estimate_for_comms; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;        /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;        /* msgLength */
    record_data_length += sizeof( uint64_t ) + 1;        /* requestID */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiIsendCompleteEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint64_t ) + 1; /* requestID */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint64_t ) + 1; /* requestID */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiIrecvRequestEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint64_t ) + 1; /* requestID */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint64_t ) + 1; /* requestID */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiRecvEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint32_t ) + 1;     /* sender */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint32_t ) + 1;        /* sender */
    record_data_length += estimator->estimate_for_comms; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;        /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;        /* msgLength */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiIrecvEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint32_t ) + 1;     /* sender */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    record_data_length += sizeof( uint64_t ) + 1;     /* requestID */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint32_t ) + 1;        /* sender */
    record_data_length += estimator->estimate_for_comms; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;        /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;        /* msgLength */
    record_data_length += sizeof( uint64_t ) + 1;        /* requestID */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiRequestTestEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint64_t ) + 1; /* requestID */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint64_t ) + 1; /* requestID */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiRequestCancelledEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint64_t ) + 1; /* requestID */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint64_t ) + 1; /* requestID */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiCollectiveBeginEvent( OTF2_EventSizeEstimator* estimator )
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

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length = 0;

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMpiCollectiveEndEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CollectiveOp ); /* collectiveOp */
    record_data_length += sizeof( OTF2_CommRef ) + 1;  /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;      /* root */
    record_data_length += sizeof( uint64_t ) + 1;      /* sizeSent */
    record_data_length += sizeof( uint64_t ) + 1;      /* sizeReceived */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_CollectiveOp );   /* collectiveOp */
    record_data_length += estimator->estimate_for_comms; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;        /* root */
    record_data_length += sizeof( uint64_t ) + 1;        /* sizeSent */
    record_data_length += sizeof( uint64_t ) + 1;        /* sizeReceived */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfOmpForkEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint32_t ) + 1; /* numberOfRequestedThreads */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint32_t ) + 1; /* numberOfRequestedThreads */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfOmpJoinEvent( OTF2_EventSizeEstimator* estimator )
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

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length = 0;

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfOmpAcquireLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint32_t ) + 1; /* lockID */
    record_data_length += sizeof( uint32_t ) + 1; /* acquisitionOrder */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint32_t ) + 1; /* lockID */
    record_data_length += sizeof( uint32_t ) + 1; /* acquisitionOrder */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfOmpReleaseLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint32_t ) + 1; /* lockID */
    record_data_length += sizeof( uint32_t ) + 1; /* acquisitionOrder */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint32_t ) + 1; /* lockID */
    record_data_length += sizeof( uint32_t ) + 1; /* acquisitionOrder */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfOmpTaskCreateEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint64_t ) + 1; /* taskID */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint64_t ) + 1; /* taskID */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfOmpTaskSwitchEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint64_t ) + 1; /* taskID */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint64_t ) + 1; /* taskID */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfOmpTaskCompleteEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( uint64_t ) + 1; /* taskID */


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( uint64_t ) + 1; /* taskID */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfMetricEvent( OTF2_EventSizeEstimator* estimator,
                                              uint8_t                  numberOfMetrics )
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
    record_data_length += sizeof( OTF2_MetricRef ) + 1;                         /* metric */
    record_data_length += sizeof( uint8_t );                                    /* numberOfMetrics */
    record_data_length += numberOfMetrics * ( sizeof( OTF2_Type ) );            /* typeIDs */
    record_data_length += numberOfMetrics * ( sizeof( OTF2_MetricValue ) + 1 ); /* metricValues */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_metrics;                      /* metric */
    record_data_length += sizeof( uint8_t );                                    /* numberOfMetrics */
    record_data_length += numberOfMetrics * ( sizeof( OTF2_Type ) );            /* typeIDs */
    record_data_length += numberOfMetrics * ( sizeof( OTF2_MetricValue ) + 1 ); /* metricValues */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfParameterStringEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_ParameterRef ) + 1; /* parameter */
    record_data_length += sizeof( OTF2_StringRef ) + 1;    /* string */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_parameters; /* parameter */
    record_data_length += estimator->estimate_for_strings;    /* string */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfParameterIntEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_ParameterRef ) + 1; /* parameter */
    record_data_length += sizeof( int64_t ) + 1;           /* value */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_parameters; /* parameter */
    record_data_length += sizeof( int64_t ) + 1;              /* value */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfParameterUnsignedIntEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_ParameterRef ) + 1; /* parameter */
    record_data_length += sizeof( uint64_t ) + 1;          /* value */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_parameters; /* parameter */
    record_data_length += sizeof( uint64_t ) + 1;             /* value */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaWinCreateEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaWinDestroyEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaCollectiveBeginEvent( OTF2_EventSizeEstimator* estimator )
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

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length = 0;

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaCollectiveEndEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CollectiveOp );     /* collectiveOp */
    record_data_length += sizeof( OTF2_RmaSyncLevel ) + 1; /* syncLevel */
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1;    /* win */
    record_data_length += sizeof( uint32_t ) + 1;          /* root */
    record_data_length += sizeof( uint64_t ) + 1;          /* bytesSent */
    record_data_length += sizeof( uint64_t ) + 1;          /* bytesReceived */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_CollectiveOp );      /* collectiveOp */
    record_data_length += sizeof( OTF2_RmaSyncLevel ) + 1;  /* syncLevel */
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint32_t ) + 1;           /* root */
    record_data_length += sizeof( uint64_t ) + 1;           /* bytesSent */
    record_data_length += sizeof( uint64_t ) + 1;           /* bytesReceived */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaGroupSyncEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaSyncLevel ) + 1; /* syncLevel */
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1;    /* win */
    record_data_length += sizeof( OTF2_GroupRef ) + 1;     /* group */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_RmaSyncLevel ) + 1;  /* syncLevel */
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += estimator->estimate_for_groups;   /* group */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaRequestLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* lockId */
    record_data_length += sizeof( OTF2_LockType );      /* lockType */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint32_t ) + 1;           /* remote */
    record_data_length += sizeof( uint64_t ) + 1;           /* lockId */
    record_data_length += sizeof( OTF2_LockType );          /* lockType */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaAcquireLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* lockId */
    record_data_length += sizeof( OTF2_LockType );      /* lockType */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint32_t ) + 1;           /* remote */
    record_data_length += sizeof( uint64_t ) + 1;           /* lockId */
    record_data_length += sizeof( OTF2_LockType );          /* lockType */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaTryLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* lockId */
    record_data_length += sizeof( OTF2_LockType );      /* lockType */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint32_t ) + 1;           /* remote */
    record_data_length += sizeof( uint64_t ) + 1;           /* lockId */
    record_data_length += sizeof( OTF2_LockType );          /* lockType */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaReleaseLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* lockId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint32_t ) + 1;           /* remote */
    record_data_length += sizeof( uint64_t ) + 1;           /* lockId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaSyncEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( OTF2_RmaSyncType );   /* syncType */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint32_t ) + 1;           /* remote */
    record_data_length += sizeof( OTF2_RmaSyncType );       /* syncType */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaWaitChangeEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaPutEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* bytes */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint32_t ) + 1;           /* remote */
    record_data_length += sizeof( uint64_t ) + 1;           /* bytes */
    record_data_length += sizeof( uint64_t ) + 1;           /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaGetEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* bytes */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint32_t ) + 1;           /* remote */
    record_data_length += sizeof( uint64_t ) + 1;           /* bytes */
    record_data_length += sizeof( uint64_t ) + 1;           /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaAtomicEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( OTF2_RmaAtomicType ); /* type */
    record_data_length += sizeof( uint64_t ) + 1;       /* bytesSent */
    record_data_length += sizeof( uint64_t ) + 1;       /* bytesReceived */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint32_t ) + 1;           /* remote */
    record_data_length += sizeof( OTF2_RmaAtomicType );     /* type */
    record_data_length += sizeof( uint64_t ) + 1;           /* bytesSent */
    record_data_length += sizeof( uint64_t ) + 1;           /* bytesReceived */
    record_data_length += sizeof( uint64_t ) + 1;           /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteBlockingEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint64_t ) + 1;           /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteNonBlockingEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint64_t ) + 1;           /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaOpTestEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint64_t ) + 1;           /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfRmaOpCompleteRemoteEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_rma_wins; /* win */
    record_data_length += sizeof( uint64_t ) + 1;           /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadForkEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    record_data_length += sizeof( uint32_t ) + 1;  /* numberOfRequestedThreads */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    record_data_length += sizeof( uint32_t ) + 1;  /* numberOfRequestedThreads */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadJoinEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_Paradigm ); /* model */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_Paradigm ); /* model */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTeamBeginEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_comms; /* threadTeam */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTeamEndEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_comms; /* threadTeam */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadAcquireLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    record_data_length += sizeof( uint32_t ) + 1;  /* lockID */
    record_data_length += sizeof( uint32_t ) + 1;  /* acquisitionOrder */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    record_data_length += sizeof( uint32_t ) + 1;  /* lockID */
    record_data_length += sizeof( uint32_t ) + 1;  /* acquisitionOrder */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadReleaseLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    record_data_length += sizeof( uint32_t ) + 1;  /* lockID */
    record_data_length += sizeof( uint32_t ) + 1;  /* acquisitionOrder */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    record_data_length += sizeof( uint32_t ) + 1;  /* lockID */
    record_data_length += sizeof( uint32_t ) + 1;  /* acquisitionOrder */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTaskCreateEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */
    record_data_length += sizeof( uint32_t ) + 1;     /* creatingThread */
    record_data_length += sizeof( uint32_t ) + 1;     /* generationNumber */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_comms; /* threadTeam */
    record_data_length += sizeof( uint32_t ) + 1;        /* creatingThread */
    record_data_length += sizeof( uint32_t ) + 1;        /* generationNumber */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTaskSwitchEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */
    record_data_length += sizeof( uint32_t ) + 1;     /* creatingThread */
    record_data_length += sizeof( uint32_t ) + 1;     /* generationNumber */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_comms; /* threadTeam */
    record_data_length += sizeof( uint32_t ) + 1;        /* creatingThread */
    record_data_length += sizeof( uint32_t ) + 1;        /* generationNumber */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadTaskCompleteEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */
    record_data_length += sizeof( uint32_t ) + 1;     /* creatingThread */
    record_data_length += sizeof( uint32_t ) + 1;     /* generationNumber */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_comms; /* threadTeam */
    record_data_length += sizeof( uint32_t ) + 1;        /* creatingThread */
    record_data_length += sizeof( uint32_t ) + 1;        /* generationNumber */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadCreateEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;     /* sequenceCount */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_comms; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;        /* sequenceCount */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadBeginEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;     /* sequenceCount */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_comms; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;        /* sequenceCount */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadWaitEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;     /* sequenceCount */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_comms; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;        /* sequenceCount */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfThreadEndEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;     /* sequenceCount */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_comms; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;        /* sequenceCount */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfCallingContextEnterEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CallingContextRef ) + 1; /* callingContext */
    record_data_length += sizeof( uint32_t ) + 1;               /* unwindDistance */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_calling_contexts; /* callingContext */
    record_data_length += sizeof( uint32_t ) + 1;                   /* unwindDistance */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfCallingContextLeaveEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CallingContextRef ) + 1; /* callingContext */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_calling_contexts; /* callingContext */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfCallingContextSampleEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_CallingContextRef ) + 1;     /* callingContext */
    record_data_length += sizeof( uint32_t ) + 1;                   /* unwindDistance */
    record_data_length += sizeof( OTF2_InterruptGeneratorRef ) + 1; /* interruptGenerator */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_calling_contexts;     /* callingContext */
    record_data_length += sizeof( uint32_t ) + 1;                       /* unwindDistance */
    record_data_length += estimator->estimate_for_interrupt_generators; /* interruptGenerator */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoCreateHandleEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;    /* handle */
    record_data_length += sizeof( OTF2_IoAccessMode );       /* mode */
    record_data_length += sizeof( OTF2_IoCreationFlag ) + 1; /* creationFlags */
    record_data_length += sizeof( OTF2_IoStatusFlag ) + 1;   /* statusFlags */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( OTF2_IoAccessMode );        /* mode */
    record_data_length += sizeof( OTF2_IoCreationFlag ) + 1;  /* creationFlags */
    record_data_length += sizeof( OTF2_IoStatusFlag ) + 1;    /* statusFlags */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoDestroyHandleEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoDuplicateHandleEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;  /* oldHandle */
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;  /* newHandle */
    record_data_length += sizeof( OTF2_IoStatusFlag ) + 1; /* statusFlags */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* oldHandle */
    record_data_length += estimator->estimate_for_io_handles; /* newHandle */
    record_data_length += sizeof( OTF2_IoStatusFlag ) + 1;    /* statusFlags */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoSeekEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( int64_t ) + 1;          /* offsetRequest */
    record_data_length += sizeof( OTF2_IoSeekOption );    /* whence */
    record_data_length += sizeof( uint64_t ) + 1;         /* offsetResult */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( int64_t ) + 1;              /* offsetRequest */
    record_data_length += sizeof( OTF2_IoSeekOption );        /* whence */
    record_data_length += sizeof( uint64_t ) + 1;             /* offsetResult */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoChangeStatusFlagsEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;  /* handle */
    record_data_length += sizeof( OTF2_IoStatusFlag ) + 1; /* statusFlags */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( OTF2_IoStatusFlag ) + 1;    /* statusFlags */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoDeleteFileEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoParadigmRef ); /* ioParadigm */
    record_data_length += sizeof( OTF2_IoFileRef ) + 1; /* file */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( OTF2_IoParadigmRef );     /* ioParadigm */
    record_data_length += estimator->estimate_for_io_files; /* file */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationBeginEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;     /* handle */
    record_data_length += sizeof( OTF2_IoOperationMode );     /* mode */
    record_data_length += sizeof( OTF2_IoOperationFlag ) + 1; /* operationFlags */
    record_data_length += sizeof( uint64_t ) + 1;             /* bytesRequest */
    record_data_length += sizeof( uint64_t ) + 1;             /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( OTF2_IoOperationMode );     /* mode */
    record_data_length += sizeof( OTF2_IoOperationFlag ) + 1; /* operationFlags */
    record_data_length += sizeof( uint64_t ) + 1;             /* bytesRequest */
    record_data_length += sizeof( uint64_t ) + 1;             /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationTestEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;         /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;             /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationIssuedEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;         /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;             /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationCompleteEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;         /* bytesResult */
    record_data_length += sizeof( uint64_t ) + 1;         /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;             /* bytesResult */
    record_data_length += sizeof( uint64_t ) + 1;             /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoOperationCancelledEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;         /* matchingId */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;             /* matchingId */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoAcquireLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( OTF2_LockType );        /* lockType */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( OTF2_LockType );            /* lockType */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoReleaseLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( OTF2_LockType );        /* lockType */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( OTF2_LockType );            /* lockType */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfIoTryLockEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( OTF2_LockType );        /* lockType */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_io_handles; /* handle */
    record_data_length += sizeof( OTF2_LockType );            /* lockType */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfProgramBeginEvent( OTF2_EventSizeEstimator* estimator,
                                                    uint32_t                 numberOfArguments )
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
    record_data_length += sizeof( OTF2_StringRef ) + 1;                         /* programName */
    record_data_length += sizeof( uint32_t ) + 1;                               /* numberOfArguments */
    record_data_length += numberOfArguments * ( sizeof( OTF2_StringRef ) + 1 ); /* programArguments */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += estimator->estimate_for_strings;                      /* programName */
    record_data_length += sizeof( uint32_t ) + 1;                               /* numberOfArguments */
    record_data_length += numberOfArguments * ( sizeof( OTF2_StringRef ) + 1 ); /* programArguments */

    return record_length + record_data_length;
}


size_t
OTF2_EventSizeEstimator_GetSizeOfProgramEndEvent( OTF2_EventSizeEstimator* estimator )
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
    record_data_length += sizeof( int64_t ) + 1; /* exitStatus */

    /* record length byte */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }


    /* Re-do the calculation, but honor known upper bounds for references to
     * definitions.
     */
    record_data_length  = 0;
    record_data_length += sizeof( int64_t ) + 1; /* exitStatus */

    return record_length + record_data_length;
}
