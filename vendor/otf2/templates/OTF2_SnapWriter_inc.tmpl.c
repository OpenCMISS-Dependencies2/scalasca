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

@otf2 for snap in snaps:

OTF2_ErrorCode
OTF2_SnapWriter_@@snap.name@@( OTF2_SnapWriter*    writerHandle,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      snapTime @@snap.funcargs()@@ )
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

    @otf2 if snap.attributes:
    /*
     * Maximum memory usage of pure record data (excluding timestamp, type ID,
     * record length information, and optional attribute list).
     */
    uint64_t record_data_length = 0;

    @otf2  for attr in snap.attributes:
    @otf2   if attr.type.errorcheck_on_argument:
    if ( !@@attr.name@@ )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                             "@@attr.type.errorcheck_on_argument@@" );
    }
    @otf2   endif
    record_data_length += @@attr.size()@@; /* @@attr.name@@ */
    @otf2   if attr is array_attr:
    @otf2    for array_attr in attr.array_attributes:
    record_data_length += @@attr.name@@ * ( @@array_attr.size()@@ ); /* @@array_attr.name@@ */
    if ( @@attr.name@@ > 0 && !@@array_attr.name@@ )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                             "Invalid @@array_attr.name@@ array argument." );
    }
    @otf2    endfor
    @otf2   endif
    @otf2  endfor
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

    @otf2 else:
    /*
     * For records without attributes, we need one additional zero byte for the
     * record length.
     */
    record_length += 1;

    @otf2 endif
    ret = OTF2_Buffer_WriteTimeStamp( writerHandle->buffer,
                                      snapTime,
                                      record_length);
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


    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_SNAP_@@snap.upper@@ );

    @otf2 if snap.attributes:
    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    @otf2  for attr in snap.attributes:
    OTF2_Buffer_Write@@attr.type.otf2@@( writerHandle->buffer, @@attr.name@@ );
    @otf2   if attr is array_attr:
    for ( @@attr.type@@ @@attr.name@@_i = 0;
          @@attr.name@@_i < @@attr.name@@;
          @@attr.name@@_i++ )
    {
        @otf2 for array_attr in attr.array_attributes:
        OTF2_Buffer_Write@@array_attr.type.otf2@@( writerHandle->buffer,
                                                   @@array_attr.name@@[ @@attr.name@@_i ] );
        @otf2 endfor
    }
    @otf2   endif
    @otf2  endfor

    /* Write real record length */
    ret = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );

    @otf2 else:
    /* Write the zero byte, which indicates no attributes. */
    OTF2_Buffer_WriteUint8( writerHandle->buffer, 0 );

    @otf2 endif    
    
    return ret;
}

@otf2 endfor
