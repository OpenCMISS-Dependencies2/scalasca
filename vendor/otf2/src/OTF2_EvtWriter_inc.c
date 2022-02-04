/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
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
 *  @source     templates/OTF2_EvtWriter_inc.tmpl.c
 *
 *  @brief      This lowest user-visible layer provides write routines to write
 *              event data of a single location. The inc file holds the
 *              generated part of event writer module.
 */


OTF2_ErrorCode
OTF2_EvtWriter_BufferFlush( OTF2_EvtWriter*     writerHandle,
                            OTF2_AttributeList* attributeList,
                            OTF2_TimeStamp      time,
                            OTF2_TimeStamp      stopTime )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_TimeStamp ); /* stopTime */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_BUFFER_FLUSH );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, stopTime );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MeasurementOnOff( OTF2_EvtWriter*      writerHandle,
                                 OTF2_AttributeList*  attributeList,
                                 OTF2_TimeStamp       time,
                                 OTF2_MeasurementMode measurementMode )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_MeasurementMode ); /* measurementMode */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MEASUREMENT_ON_OFF );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, measurementMode );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_Enter( OTF2_EvtWriter*     writerHandle,
                      OTF2_AttributeList* attributeList,
                      OTF2_TimeStamp      time,
                      OTF2_RegionRef      region )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RegionRef ) + 1; /* region */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_ENTER );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, region );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_Leave( OTF2_EvtWriter*     writerHandle,
                      OTF2_AttributeList* attributeList,
                      OTF2_TimeStamp      time,
                      OTF2_RegionRef      region )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RegionRef ) + 1; /* region */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_LEAVE );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, region );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiSend( OTF2_EvtWriter*     writerHandle,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        uint32_t            receiver,
                        OTF2_CommRef        communicator,
                        uint32_t            msgTag,
                        uint64_t            msgLength )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint32_t ) + 1;     /* receiver */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_SEND );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, receiver );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, communicator );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, msgTag );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, msgLength );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiIsend( OTF2_EvtWriter*     writerHandle,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      time,
                         uint32_t            receiver,
                         OTF2_CommRef        communicator,
                         uint32_t            msgTag,
                         uint64_t            msgLength,
                         uint64_t            requestID )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint32_t ) + 1;     /* receiver */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    record_data_length += sizeof( uint64_t ) + 1;     /* requestID */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_ISEND );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, receiver );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, communicator );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, msgTag );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, msgLength );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, requestID );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiIsendComplete( OTF2_EvtWriter*     writerHandle,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 uint64_t            requestID )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint64_t ) + 1; /* requestID */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_ISEND_COMPLETE );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, requestID );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiIrecvRequest( OTF2_EvtWriter*     writerHandle,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                uint64_t            requestID )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint64_t ) + 1; /* requestID */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_IRECV_REQUEST );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, requestID );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiRecv( OTF2_EvtWriter*     writerHandle,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        uint32_t            sender,
                        OTF2_CommRef        communicator,
                        uint32_t            msgTag,
                        uint64_t            msgLength )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint32_t ) + 1;     /* sender */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_RECV );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, sender );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, communicator );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, msgTag );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, msgLength );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiIrecv( OTF2_EvtWriter*     writerHandle,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      time,
                         uint32_t            sender,
                         OTF2_CommRef        communicator,
                         uint32_t            msgTag,
                         uint64_t            msgLength,
                         uint64_t            requestID )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint32_t ) + 1;     /* sender */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    record_data_length += sizeof( uint64_t ) + 1;     /* requestID */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_IRECV );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, sender );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, communicator );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, msgTag );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, msgLength );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, requestID );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiRequestTest( OTF2_EvtWriter*     writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               uint64_t            requestID )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint64_t ) + 1; /* requestID */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_REQUEST_TEST );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, requestID );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiRequestCancelled( OTF2_EvtWriter*     writerHandle,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    uint64_t            requestID )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint64_t ) + 1; /* requestID */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_REQUEST_CANCELLED );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, requestID );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiCollectiveBegin( OTF2_EvtWriter*     writerHandle,
                                   OTF2_AttributeList* attributeList,
                                   OTF2_TimeStamp      time )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * For records without attributes, we need one additional zero byte for the
     * record length.
     */
    record_length += 1;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_COLLECTIVE_BEGIN );

    /* Write the zero byte, which indicates no attributes. */
    OTF2_Buffer_WriteUint8( writerHandle->buffer, 0 );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiCollectiveEnd( OTF2_EvtWriter*     writerHandle,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CollectiveOp   collectiveOp,
                                 OTF2_CommRef        communicator,
                                 uint32_t            root,
                                 uint64_t            sizeSent,
                                 uint64_t            sizeReceived )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CollectiveOp ); /* collectiveOp */
    record_data_length += sizeof( OTF2_CommRef ) + 1;  /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;      /* root */
    record_data_length += sizeof( uint64_t ) + 1;      /* sizeSent */
    record_data_length += sizeof( uint64_t ) + 1;      /* sizeReceived */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_MPI_COLLECTIVE_END );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, collectiveOp );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, communicator );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, root );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, sizeSent );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, sizeReceived );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_OmpFork( OTF2_EvtWriter*     writerHandle,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        uint32_t            numberOfRequestedThreads )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint32_t ) + 1; /* numberOfRequestedThreads */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_OMP_FORK );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, numberOfRequestedThreads );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_OmpJoin( OTF2_EvtWriter*     writerHandle,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * For records without attributes, we need one additional zero byte for the
     * record length.
     */
    record_length += 1;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_OMP_JOIN );

    /* Write the zero byte, which indicates no attributes. */
    OTF2_Buffer_WriteUint8( writerHandle->buffer, 0 );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_OmpAcquireLock( OTF2_EvtWriter*     writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               uint32_t            lockID,
                               uint32_t            acquisitionOrder )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint32_t ) + 1; /* lockID */
    record_data_length += sizeof( uint32_t ) + 1; /* acquisitionOrder */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_OMP_ACQUIRE_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, lockID );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, acquisitionOrder );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_OmpReleaseLock( OTF2_EvtWriter*     writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               uint32_t            lockID,
                               uint32_t            acquisitionOrder )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint32_t ) + 1; /* lockID */
    record_data_length += sizeof( uint32_t ) + 1; /* acquisitionOrder */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_OMP_RELEASE_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, lockID );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, acquisitionOrder );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_OmpTaskCreate( OTF2_EvtWriter*     writerHandle,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              uint64_t            taskID )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint64_t ) + 1; /* taskID */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_OMP_TASK_CREATE );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, taskID );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_OmpTaskSwitch( OTF2_EvtWriter*     writerHandle,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              uint64_t            taskID )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint64_t ) + 1; /* taskID */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_OMP_TASK_SWITCH );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, taskID );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_OmpTaskComplete( OTF2_EvtWriter*     writerHandle,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                uint64_t            taskID )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( uint64_t ) + 1; /* taskID */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_OMP_TASK_COMPLETE );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, taskID );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_Metric( OTF2_EvtWriter*         writerHandle,
                       OTF2_AttributeList*     attributeList,
                       OTF2_TimeStamp          time,
                       OTF2_MetricRef          metric,
                       uint8_t                 numberOfMetrics,
                       const OTF2_Type*        typeIDs,
                       const OTF2_MetricValue* metricValues )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_MetricRef ) + 1;              /* metric */
    record_data_length += sizeof( uint8_t );                         /* numberOfMetrics */
    record_data_length += numberOfMetrics * ( sizeof( OTF2_Type ) ); /* typeIDs */
    if ( numberOfMetrics > 0 && !typeIDs )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid typeIDs array argument." );
    }
    record_data_length += numberOfMetrics * ( sizeof( OTF2_MetricValue ) + 1 ); /* metricValues */
    if ( numberOfMetrics > 0 && !metricValues )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid metricValues array argument." );
    }
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_METRIC );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, metric );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, numberOfMetrics );
    for ( uint8_t numberOfMetrics_i = 0;
          numberOfMetrics_i < numberOfMetrics;
          numberOfMetrics_i++ )
    {
        OTF2_Buffer_WriteUint8( writerHandle->buffer,
                                typeIDs[ numberOfMetrics_i ] );
        OTF2_Buffer_WriteMetricValue( writerHandle->buffer,
                                      metricValues[ numberOfMetrics_i ] );
    }

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ParameterString( OTF2_EvtWriter*     writerHandle,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_ParameterRef   parameter,
                                OTF2_StringRef      string )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_ParameterRef ) + 1; /* parameter */
    record_data_length += sizeof( OTF2_StringRef ) + 1;    /* string */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_PARAMETER_STRING );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, parameter );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, string );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ParameterInt( OTF2_EvtWriter*     writerHandle,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_ParameterRef   parameter,
                             int64_t             value )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_ParameterRef ) + 1; /* parameter */
    record_data_length += sizeof( int64_t ) + 1;           /* value */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_PARAMETER_INT );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, parameter );
    OTF2_Buffer_WriteInt64( writerHandle->buffer, value );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ParameterUnsignedInt( OTF2_EvtWriter*     writerHandle,
                                     OTF2_AttributeList* attributeList,
                                     OTF2_TimeStamp      time,
                                     OTF2_ParameterRef   parameter,
                                     uint64_t            value )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_ParameterRef ) + 1; /* parameter */
    record_data_length += sizeof( uint64_t ) + 1;          /* value */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_PARAMETER_UNSIGNED_INT );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, parameter );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, value );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaWinCreate( OTF2_EvtWriter*     writerHandle,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_RmaWinRef      win )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_WIN_CREATE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaWinDestroy( OTF2_EvtWriter*     writerHandle,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_RmaWinRef      win )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_WIN_DESTROY );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaCollectiveBegin( OTF2_EvtWriter*     writerHandle,
                                   OTF2_AttributeList* attributeList,
                                   OTF2_TimeStamp      time )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * For records without attributes, we need one additional zero byte for the
     * record length.
     */
    record_length += 1;

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_COLLECTIVE_BEGIN );

    /* Write the zero byte, which indicates no attributes. */
    OTF2_Buffer_WriteUint8( writerHandle->buffer, 0 );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaCollectiveEnd( OTF2_EvtWriter*     writerHandle,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CollectiveOp   collectiveOp,
                                 OTF2_RmaSyncLevel   syncLevel,
                                 OTF2_RmaWinRef      win,
                                 uint32_t            root,
                                 uint64_t            bytesSent,
                                 uint64_t            bytesReceived )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CollectiveOp );     /* collectiveOp */
    record_data_length += sizeof( OTF2_RmaSyncLevel ) + 1; /* syncLevel */
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1;    /* win */
    record_data_length += sizeof( uint32_t ) + 1;          /* root */
    record_data_length += sizeof( uint64_t ) + 1;          /* bytesSent */
    record_data_length += sizeof( uint64_t ) + 1;          /* bytesReceived */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_COLLECTIVE_END );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, collectiveOp );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, syncLevel );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, root );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, bytesSent );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, bytesReceived );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaGroupSync( OTF2_EvtWriter*     writerHandle,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_RmaSyncLevel   syncLevel,
                             OTF2_RmaWinRef      win,
                             OTF2_GroupRef       group )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaSyncLevel ) + 1; /* syncLevel */
    record_data_length += sizeof( OTF2_RmaWinRef ) + 1;    /* win */
    record_data_length += sizeof( OTF2_GroupRef ) + 1;     /* group */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_GROUP_SYNC );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, syncLevel );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, group );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaRequestLock( OTF2_EvtWriter*     writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_RmaWinRef      win,
                               uint32_t            remote,
                               uint64_t            lockId,
                               OTF2_LockType       lockType )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* lockId */
    record_data_length += sizeof( OTF2_LockType );      /* lockType */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_REQUEST_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, remote );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, lockId );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, lockType );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaAcquireLock( OTF2_EvtWriter*     writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_RmaWinRef      win,
                               uint32_t            remote,
                               uint64_t            lockId,
                               OTF2_LockType       lockType )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* lockId */
    record_data_length += sizeof( OTF2_LockType );      /* lockType */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_ACQUIRE_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, remote );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, lockId );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, lockType );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaTryLock( OTF2_EvtWriter*     writerHandle,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_RmaWinRef      win,
                           uint32_t            remote,
                           uint64_t            lockId,
                           OTF2_LockType       lockType )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* lockId */
    record_data_length += sizeof( OTF2_LockType );      /* lockType */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_TRY_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, remote );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, lockId );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, lockType );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaReleaseLock( OTF2_EvtWriter*     writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_RmaWinRef      win,
                               uint32_t            remote,
                               uint64_t            lockId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* lockId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_RELEASE_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, remote );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, lockId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaSync( OTF2_EvtWriter*     writerHandle,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        OTF2_RmaWinRef      win,
                        uint32_t            remote,
                        OTF2_RmaSyncType    syncType )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( OTF2_RmaSyncType );   /* syncType */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_SYNC );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, remote );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, syncType );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaWaitChange( OTF2_EvtWriter*     writerHandle,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_RmaWinRef      win )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_WAIT_CHANGE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaPut( OTF2_EvtWriter*     writerHandle,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      time,
                       OTF2_RmaWinRef      win,
                       uint32_t            remote,
                       uint64_t            bytes,
                       uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* bytes */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_PUT );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, remote );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, bytes );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaGet( OTF2_EvtWriter*     writerHandle,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      time,
                       OTF2_RmaWinRef      win,
                       uint32_t            remote,
                       uint64_t            bytes,
                       uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( uint64_t ) + 1;       /* bytes */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_GET );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, remote );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, bytes );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaAtomic( OTF2_EvtWriter*     writerHandle,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_RmaWinRef      win,
                          uint32_t            remote,
                          OTF2_RmaAtomicType  type,
                          uint64_t            bytesSent,
                          uint64_t            bytesReceived,
                          uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint32_t ) + 1;       /* remote */
    record_data_length += sizeof( OTF2_RmaAtomicType ); /* type */
    record_data_length += sizeof( uint64_t ) + 1;       /* bytesSent */
    record_data_length += sizeof( uint64_t ) + 1;       /* bytesReceived */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_ATOMIC );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, remote );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, type );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, bytesSent );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, bytesReceived );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaOpCompleteBlocking( OTF2_EvtWriter*     writerHandle,
                                      OTF2_AttributeList* attributeList,
                                      OTF2_TimeStamp      time,
                                      OTF2_RmaWinRef      win,
                                      uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_OP_COMPLETE_BLOCKING );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaOpCompleteNonBlocking( OTF2_EvtWriter*     writerHandle,
                                         OTF2_AttributeList* attributeList,
                                         OTF2_TimeStamp      time,
                                         OTF2_RmaWinRef      win,
                                         uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_OP_COMPLETE_NON_BLOCKING );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaOpTest( OTF2_EvtWriter*     writerHandle,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_RmaWinRef      win,
                          uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_OP_TEST );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaOpCompleteRemote( OTF2_EvtWriter*     writerHandle,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    OTF2_RmaWinRef      win,
                                    uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_RmaWinRef ) + 1; /* win */
    record_data_length += sizeof( uint64_t ) + 1;       /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_RMA_OP_COMPLETE_REMOTE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, win );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadFork( OTF2_EvtWriter*     writerHandle,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_Paradigm       model,
                           uint32_t            numberOfRequestedThreads )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    record_data_length += sizeof( uint32_t ) + 1;  /* numberOfRequestedThreads */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_FORK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, model );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, numberOfRequestedThreads );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadJoin( OTF2_EvtWriter*     writerHandle,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_Paradigm       model )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_JOIN );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, model );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadTeamBegin( OTF2_EvtWriter*     writerHandle,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_CommRef        threadTeam )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_TEAM_BEGIN );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, threadTeam );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadTeamEnd( OTF2_EvtWriter*     writerHandle,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_CommRef        threadTeam )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_TEAM_END );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, threadTeam );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadAcquireLock( OTF2_EvtWriter*     writerHandle,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_Paradigm       model,
                                  uint32_t            lockID,
                                  uint32_t            acquisitionOrder )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    record_data_length += sizeof( uint32_t ) + 1;  /* lockID */
    record_data_length += sizeof( uint32_t ) + 1;  /* acquisitionOrder */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_ACQUIRE_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, model );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, lockID );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, acquisitionOrder );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadReleaseLock( OTF2_EvtWriter*     writerHandle,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_Paradigm       model,
                                  uint32_t            lockID,
                                  uint32_t            acquisitionOrder )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_Paradigm ); /* model */
    record_data_length += sizeof( uint32_t ) + 1;  /* lockID */
    record_data_length += sizeof( uint32_t ) + 1;  /* acquisitionOrder */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_RELEASE_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, model );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, lockID );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, acquisitionOrder );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadTaskCreate( OTF2_EvtWriter*     writerHandle,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CommRef        threadTeam,
                                 uint32_t            creatingThread,
                                 uint32_t            generationNumber )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */
    record_data_length += sizeof( uint32_t ) + 1;     /* creatingThread */
    record_data_length += sizeof( uint32_t ) + 1;     /* generationNumber */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_TASK_CREATE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, threadTeam );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, creatingThread );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, generationNumber );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadTaskSwitch( OTF2_EvtWriter*     writerHandle,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CommRef        threadTeam,
                                 uint32_t            creatingThread,
                                 uint32_t            generationNumber )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */
    record_data_length += sizeof( uint32_t ) + 1;     /* creatingThread */
    record_data_length += sizeof( uint32_t ) + 1;     /* generationNumber */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_TASK_SWITCH );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, threadTeam );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, creatingThread );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, generationNumber );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadTaskComplete( OTF2_EvtWriter*     writerHandle,
                                   OTF2_AttributeList* attributeList,
                                   OTF2_TimeStamp      time,
                                   OTF2_CommRef        threadTeam,
                                   uint32_t            creatingThread,
                                   uint32_t            generationNumber )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadTeam */
    record_data_length += sizeof( uint32_t ) + 1;     /* creatingThread */
    record_data_length += sizeof( uint32_t ) + 1;     /* generationNumber */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_TASK_COMPLETE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, threadTeam );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, creatingThread );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, generationNumber );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadCreate( OTF2_EvtWriter*     writerHandle,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_CommRef        threadContingent,
                             uint64_t            sequenceCount )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;     /* sequenceCount */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_CREATE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, threadContingent );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, sequenceCount );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadBegin( OTF2_EvtWriter*     writerHandle,
                            OTF2_AttributeList* attributeList,
                            OTF2_TimeStamp      time,
                            OTF2_CommRef        threadContingent,
                            uint64_t            sequenceCount )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;     /* sequenceCount */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_BEGIN );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, threadContingent );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, sequenceCount );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadWait( OTF2_EvtWriter*     writerHandle,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_CommRef        threadContingent,
                           uint64_t            sequenceCount )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;     /* sequenceCount */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_WAIT );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, threadContingent );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, sequenceCount );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadEnd( OTF2_EvtWriter*     writerHandle,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_CommRef        threadContingent,
                          uint64_t            sequenceCount )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CommRef ) + 1; /* threadContingent */
    record_data_length += sizeof( uint64_t ) + 1;     /* sequenceCount */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_THREAD_END );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, threadContingent );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, sequenceCount );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_CallingContextEnter( OTF2_EvtWriter*        writerHandle,
                                    OTF2_AttributeList*    attributeList,
                                    OTF2_TimeStamp         time,
                                    OTF2_CallingContextRef callingContext,
                                    uint32_t               unwindDistance )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CallingContextRef ) + 1; /* callingContext */
    record_data_length += sizeof( uint32_t ) + 1;               /* unwindDistance */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_CALLING_CONTEXT_ENTER );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, callingContext );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, unwindDistance );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_CallingContextLeave( OTF2_EvtWriter*        writerHandle,
                                    OTF2_AttributeList*    attributeList,
                                    OTF2_TimeStamp         time,
                                    OTF2_CallingContextRef callingContext )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CallingContextRef ) + 1; /* callingContext */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_CALLING_CONTEXT_LEAVE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, callingContext );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_CallingContextSample( OTF2_EvtWriter*            writerHandle,
                                     OTF2_AttributeList*        attributeList,
                                     OTF2_TimeStamp             time,
                                     OTF2_CallingContextRef     callingContext,
                                     uint32_t                   unwindDistance,
                                     OTF2_InterruptGeneratorRef interruptGenerator )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_CallingContextRef ) + 1;     /* callingContext */
    record_data_length += sizeof( uint32_t ) + 1;                   /* unwindDistance */
    record_data_length += sizeof( OTF2_InterruptGeneratorRef ) + 1; /* interruptGenerator */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_CALLING_CONTEXT_SAMPLE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, callingContext );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, unwindDistance );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, interruptGenerator );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoCreateHandle( OTF2_EvtWriter*     writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_IoHandleRef    handle,
                               OTF2_IoAccessMode   mode,
                               OTF2_IoCreationFlag creationFlags,
                               OTF2_IoStatusFlag   statusFlags )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;    /* handle */
    record_data_length += sizeof( OTF2_IoAccessMode );       /* mode */
    record_data_length += sizeof( OTF2_IoCreationFlag ) + 1; /* creationFlags */
    record_data_length += sizeof( OTF2_IoStatusFlag ) + 1;   /* statusFlags */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_CREATE_HANDLE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, mode );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, creationFlags );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, statusFlags );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoDestroyHandle( OTF2_EvtWriter*     writerHandle,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_IoHandleRef    handle )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_DESTROY_HANDLE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoDuplicateHandle( OTF2_EvtWriter*     writerHandle,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_IoHandleRef    oldHandle,
                                  OTF2_IoHandleRef    newHandle,
                                  OTF2_IoStatusFlag   statusFlags )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;  /* oldHandle */
    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;  /* newHandle */
    record_data_length += sizeof( OTF2_IoStatusFlag ) + 1; /* statusFlags */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_DUPLICATE_HANDLE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, oldHandle );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, newHandle );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, statusFlags );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoSeek( OTF2_EvtWriter*     writerHandle,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      time,
                       OTF2_IoHandleRef    handle,
                       int64_t             offsetRequest,
                       OTF2_IoSeekOption   whence,
                       uint64_t            offsetResult )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( int64_t ) + 1;          /* offsetRequest */
    record_data_length += sizeof( OTF2_IoSeekOption );    /* whence */
    record_data_length += sizeof( uint64_t ) + 1;         /* offsetResult */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_SEEK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteInt64( writerHandle->buffer, offsetRequest );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, whence );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, offsetResult );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoChangeStatusFlags( OTF2_EvtWriter*     writerHandle,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    OTF2_IoHandleRef    handle,
                                    OTF2_IoStatusFlag   statusFlags )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;  /* handle */
    record_data_length += sizeof( OTF2_IoStatusFlag ) + 1; /* statusFlags */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_CHANGE_STATUS_FLAGS );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, statusFlags );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoDeleteFile( OTF2_EvtWriter*     writerHandle,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_IoParadigmRef  ioParadigm,
                             OTF2_IoFileRef      file )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoParadigmRef ); /* ioParadigm */
    record_data_length += sizeof( OTF2_IoFileRef ) + 1; /* file */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_DELETE_FILE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, ioParadigm );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, file );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoOperationBegin( OTF2_EvtWriter*      writerHandle,
                                 OTF2_AttributeList*  attributeList,
                                 OTF2_TimeStamp       time,
                                 OTF2_IoHandleRef     handle,
                                 OTF2_IoOperationMode mode,
                                 OTF2_IoOperationFlag operationFlags,
                                 uint64_t             bytesRequest,
                                 uint64_t             matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1;     /* handle */
    record_data_length += sizeof( OTF2_IoOperationMode );     /* mode */
    record_data_length += sizeof( OTF2_IoOperationFlag ) + 1; /* operationFlags */
    record_data_length += sizeof( uint64_t ) + 1;             /* bytesRequest */
    record_data_length += sizeof( uint64_t ) + 1;             /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_OPERATION_BEGIN );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, mode );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, operationFlags );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, bytesRequest );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoOperationTest( OTF2_EvtWriter*     writerHandle,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_IoHandleRef    handle,
                                uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;         /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_OPERATION_TEST );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoOperationIssued( OTF2_EvtWriter*     writerHandle,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_IoHandleRef    handle,
                                  uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;         /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_OPERATION_ISSUED );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoOperationComplete( OTF2_EvtWriter*     writerHandle,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    OTF2_IoHandleRef    handle,
                                    uint64_t            bytesResult,
                                    uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;         /* bytesResult */
    record_data_length += sizeof( uint64_t ) + 1;         /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_OPERATION_COMPLETE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, bytesResult );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoOperationCancelled( OTF2_EvtWriter*     writerHandle,
                                     OTF2_AttributeList* attributeList,
                                     OTF2_TimeStamp      time,
                                     OTF2_IoHandleRef    handle,
                                     uint64_t            matchingId )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( uint64_t ) + 1;         /* matchingId */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_OPERATION_CANCELLED );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, matchingId );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoAcquireLock( OTF2_EvtWriter*     writerHandle,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_IoHandleRef    handle,
                              OTF2_LockType       lockType )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( OTF2_LockType );        /* lockType */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_ACQUIRE_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, lockType );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoReleaseLock( OTF2_EvtWriter*     writerHandle,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_IoHandleRef    handle,
                              OTF2_LockType       lockType )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( OTF2_LockType );        /* lockType */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_RELEASE_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, lockType );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoTryLock( OTF2_EvtWriter*     writerHandle,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_IoHandleRef    handle,
                          OTF2_LockType       lockType )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_IoHandleRef ) + 1; /* handle */
    record_data_length += sizeof( OTF2_LockType );        /* lockType */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_IO_TRY_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, handle );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, lockType );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ProgramBegin( OTF2_EvtWriter*       writerHandle,
                             OTF2_AttributeList*   attributeList,
                             OTF2_TimeStamp        time,
                             OTF2_StringRef        programName,
                             uint32_t              numberOfArguments,
                             const OTF2_StringRef* programArguments )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( OTF2_StringRef ) + 1;                         /* programName */
    record_data_length += sizeof( uint32_t ) + 1;                               /* numberOfArguments */
    record_data_length += numberOfArguments * ( sizeof( OTF2_StringRef ) + 1 ); /* programArguments */
    if ( numberOfArguments > 0 && !programArguments )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid programArguments array argument." );
    }
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_PROGRAM_BEGIN );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, programName );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, numberOfArguments );
    for ( uint32_t numberOfArguments_i = 0;
          numberOfArguments_i < numberOfArguments;
          numberOfArguments_i++ )
    {
        OTF2_Buffer_WriteUint32( writerHandle->buffer,
                                 programArguments[ numberOfArguments_i ] );
    }

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}


OTF2_ErrorCode
OTF2_EvtWriter_ProgramEnd( OTF2_EvtWriter*     writerHandle,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           int64_t             exitStatus )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    OTF2_ErrorCode ret;

    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes (excluding the timestamp), and the size of the attribute list.
     */
    uint64_t record_length = 1;

    /*
     * The size of the attribute list, if any.
     */
    uint64_t attribute_list_size = otf2_attribute_list_get_size( attributeList );
    record_length += attribute_list_size;

    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    record_data_length += sizeof( int64_t ) + 1; /* exitStatus */
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional bytes to store real record length. For records that exceed 255 bytes
     * we need a maximum of 9 bytes to encode their length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      time,
                                      record_length );
    if ( OTF2_SUCCESS != ret )
    {
        return ret;
    }

    /* Write only the attribute list, if it is not empty */
    if ( attribute_list_size )
    {
        ret = otf2_attribute_list_write_to_buffer( attributeList,
                                                   writerHandle->buffer );
        if ( OTF2_SUCCESS != ret )
        {
            return ret;
        }
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_EVENT_PROGRAM_END );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteInt64( writerHandle->buffer, exitStatus );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    return ret;
}
