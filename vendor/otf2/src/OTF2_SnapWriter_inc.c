/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 *  @internal
 *
 *  @file
 *  @source     templates/OTF2_SnapWriter_inc.tmpl.c
 *
 *  @brief      This lowest user-visible layer provides write routines to write
 *              snapshot records for a single location. The inc file holds the
 *              generated part of snapshot writer module.
 */


OTF2_ErrorCode
OTF2_SnapWriter_SnapshotStart( OTF2_SnapWriter*    writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      snapTime,
                               uint64_t            numberOfRecords )
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

    record_data_length += sizeof( uint64_t ) + 1; /* numberOfRecords */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_SNAPSHOT_START );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, numberOfRecords );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_SnapshotEnd( OTF2_SnapWriter*    writerHandle,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      snapTime,
                             uint64_t            contReadPos )
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

    record_data_length += sizeof( uint64_t ) + 1; /* contReadPos */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_SNAPSHOT_END );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, contReadPos );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_MeasurementOnOff( OTF2_SnapWriter*     writerHandle,
                                  OTF2_AttributeList*  attributeList,
                                  OTF2_TimeStamp       snapTime,
                                  OTF2_TimeStamp       origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );       /* origEventTime */
    record_data_length += sizeof( OTF2_MeasurementMode ); /* measurementMode */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_MEASUREMENT_ON_OFF );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, measurementMode );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_Enter( OTF2_SnapWriter*    writerHandle,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      snapTime,
                       OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );     /* origEventTime */
    record_data_length += sizeof( OTF2_RegionRef ) + 1; /* region */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_ENTER );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, region );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_MpiSend( OTF2_SnapWriter*    writerHandle,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      snapTime,
                         OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );   /* origEventTime */
    record_data_length += sizeof( uint32_t ) + 1;     /* receiver */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_MPI_SEND );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, receiver );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, communicator );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, msgTag );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, msgLength );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_MpiIsend( OTF2_SnapWriter*    writerHandle,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      snapTime,
                          OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );   /* origEventTime */
    record_data_length += sizeof( uint32_t ) + 1;     /* receiver */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    record_data_length += sizeof( uint64_t ) + 1;     /* requestID */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_MPI_ISEND );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
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
OTF2_SnapWriter_MpiIsendComplete( OTF2_SnapWriter*    writerHandle,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      snapTime,
                                  OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp ); /* origEventTime */
    record_data_length += sizeof( uint64_t ) + 1;   /* requestID */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_MPI_ISEND_COMPLETE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, requestID );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_MpiRecv( OTF2_SnapWriter*    writerHandle,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      snapTime,
                         OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );   /* origEventTime */
    record_data_length += sizeof( uint32_t ) + 1;     /* sender */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_MPI_RECV );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, sender );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, communicator );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, msgTag );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, msgLength );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_MpiIrecvRequest( OTF2_SnapWriter*    writerHandle,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      snapTime,
                                 OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp ); /* origEventTime */
    record_data_length += sizeof( uint64_t ) + 1;   /* requestID */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_MPI_IRECV_REQUEST );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, requestID );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_MpiIrecv( OTF2_SnapWriter*    writerHandle,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      snapTime,
                          OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );   /* origEventTime */
    record_data_length += sizeof( uint32_t ) + 1;     /* sender */
    record_data_length += sizeof( OTF2_CommRef ) + 1; /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;     /* msgTag */
    record_data_length += sizeof( uint64_t ) + 1;     /* msgLength */
    record_data_length += sizeof( uint64_t ) + 1;     /* requestID */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_MPI_IRECV );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
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
OTF2_SnapWriter_MpiCollectiveBegin( OTF2_SnapWriter*    writerHandle,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      snapTime,
                                    OTF2_TimeStamp      origEventTime )
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

    record_data_length += sizeof( OTF2_TimeStamp ); /* origEventTime */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_MPI_COLLECTIVE_BEGIN );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_MpiCollectiveEnd( OTF2_SnapWriter*    writerHandle,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      snapTime,
                                  OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );    /* origEventTime */
    record_data_length += sizeof( OTF2_CollectiveOp ); /* collectiveOp */
    record_data_length += sizeof( OTF2_CommRef ) + 1;  /* communicator */
    record_data_length += sizeof( uint32_t ) + 1;      /* root */
    record_data_length += sizeof( uint64_t ) + 1;      /* sizeSent */
    record_data_length += sizeof( uint64_t ) + 1;      /* sizeReceived */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_MPI_COLLECTIVE_END );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
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
OTF2_SnapWriter_OmpFork( OTF2_SnapWriter*    writerHandle,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      snapTime,
                         OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp ); /* origEventTime */
    record_data_length += sizeof( uint32_t ) + 1;   /* numberOfRequestedThreads */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_OMP_FORK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, numberOfRequestedThreads );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_OmpAcquireLock( OTF2_SnapWriter*    writerHandle,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      snapTime,
                                OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp ); /* origEventTime */
    record_data_length += sizeof( uint32_t ) + 1;   /* lockID */
    record_data_length += sizeof( uint32_t ) + 1;   /* acquisitionOrder */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_OMP_ACQUIRE_LOCK );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, lockID );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, acquisitionOrder );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_OmpTaskCreate( OTF2_SnapWriter*    writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      snapTime,
                               OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp ); /* origEventTime */
    record_data_length += sizeof( uint64_t ) + 1;   /* taskID */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_OMP_TASK_CREATE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, taskID );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_OmpTaskSwitch( OTF2_SnapWriter*    writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      snapTime,
                               OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp ); /* origEventTime */
    record_data_length += sizeof( uint64_t ) + 1;   /* taskID */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_OMP_TASK_SWITCH );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, taskID );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_Metric( OTF2_SnapWriter*        writerHandle,
                        OTF2_AttributeList*     attributeList,
                        OTF2_TimeStamp          snapTime,
                        OTF2_TimeStamp          origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );                  /* origEventTime */
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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_METRIC );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
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
OTF2_SnapWriter_ParameterString( OTF2_SnapWriter*    writerHandle,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      snapTime,
                                 OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );        /* origEventTime */
    record_data_length += sizeof( OTF2_ParameterRef ) + 1; /* parameter */
    record_data_length += sizeof( OTF2_StringRef ) + 1;    /* string */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_PARAMETER_STRING );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, parameter );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, string );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_ParameterInt( OTF2_SnapWriter*    writerHandle,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      snapTime,
                              OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );        /* origEventTime */
    record_data_length += sizeof( OTF2_ParameterRef ) + 1; /* parameter */
    record_data_length += sizeof( int64_t ) + 1;           /* value */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_PARAMETER_INT );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, parameter );
    OTF2_Buffer_WriteInt64( writerHandle->buffer, value );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}


OTF2_ErrorCode
OTF2_SnapWriter_ParameterUnsignedInt( OTF2_SnapWriter*    writerHandle,
                                      OTF2_AttributeList* attributeList,
                                      OTF2_TimeStamp      snapTime,
                                      OTF2_TimeStamp      origEventTime,
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

    record_data_length += sizeof( OTF2_TimeStamp );        /* origEventTime */
    record_data_length += sizeof( OTF2_ParameterRef ) + 1; /* parameter */
    record_data_length += sizeof( uint64_t ) + 1;          /* value */
    record_length      += record_data_length;

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
                                      snapTime,
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_PARAMETER_UNSIGNED_INT );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64Full( writerHandle->buffer, origEventTime );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, parameter );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, value );

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );


    return ret;
}
