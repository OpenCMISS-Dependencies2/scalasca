/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
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
 *  @source     templates/OTF2_GlobalDefWriter_inc.tmpl.c
 *
 *  @brief      This layer always writes globally defined OTF2 definition
 *              records and is used to write either the global definitions
 *              in addition to local definitions or write all definitions as
 *              globally valid in combination with OTF2_GlobalEventWriter.
 *              Global definitions are stored in one global definition file,
 *              which makes it nearly impossible to write them in a distributed
 *              manner. It is therefore only allowed to get such a writer from
 *              an OTF2_Archive which is the master in the collective context.
 */


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteClockProperties( OTF2_GlobalDefWriter* writerHandle,
                                           uint64_t              timerResolution,
                                           uint64_t              globalOffset,
                                           uint64_t              traceLength )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint64( timerResolution );
    record_data_length += otf2_buffer_size_uint64( globalOffset );
    record_data_length += otf2_buffer_size_uint64( traceLength );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_CLOCK_PROPERTIES );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, timerResolution );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, globalOffset );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, traceLength );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteParadigm( OTF2_GlobalDefWriter* writerHandle,
                                    OTF2_Paradigm         paradigm,
                                    OTF2_StringRef        name,
                                    OTF2_ParadigmClass    paradigmClass )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint8( paradigm );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( paradigmClass );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_PARADIGM );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, paradigm );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, paradigmClass );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteParadigmProperty( OTF2_GlobalDefWriter* writerHandle,
                                            OTF2_Paradigm         paradigm,
                                            OTF2_ParadigmProperty property,
                                            OTF2_Type             type,
                                            OTF2_AttributeValue   value )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint8( paradigm );
    record_data_length += otf2_buffer_size_uint8( property );
    record_data_length += otf2_buffer_size_uint8( type );
    record_data_length += otf2_buffer_size_attribute_value( value );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_PARADIGM_PROPERTY );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, paradigm );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, property );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, type );
    OTF2_Buffer_WriteAttributeValue( writerHandle->buffer, value );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteIoParadigm( OTF2_GlobalDefWriter*          writerHandle,
                                      OTF2_IoParadigmRef             self,
                                      OTF2_StringRef                 identification,
                                      OTF2_StringRef                 name,
                                      OTF2_IoParadigmClass           ioParadigmClass,
                                      OTF2_IoParadigmFlag            ioParadigmFlags,
                                      uint8_t                        numberOfProperties,
                                      const OTF2_IoParadigmProperty* properties,
                                      const OTF2_Type*               types,
                                      const OTF2_AttributeValue*     values )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint8( self );
    record_data_length += otf2_buffer_size_uint32( identification );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( ioParadigmClass );
    record_data_length += otf2_buffer_size_uint32( ioParadigmFlags );
    record_data_length += otf2_buffer_size_uint8( numberOfProperties );
    if ( numberOfProperties > 0 && !properties )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid properties array argument." );
    }
    record_data_length += otf2_buffer_array_size_uint8( numberOfProperties, properties );
    if ( numberOfProperties > 0 && !types )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid types array argument." );
    }
    record_data_length += otf2_buffer_array_size_uint8( numberOfProperties, types );
    if ( numberOfProperties > 0 && !values )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid values array argument." );
    }
    record_data_length += otf2_buffer_array_size_attribute_value( numberOfProperties, values );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_IO_PARADIGM );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint8( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, identification );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, ioParadigmClass );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, ioParadigmFlags );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, numberOfProperties );
    for ( uint8_t numberOfProperties_i = 0;
          numberOfProperties_i < numberOfProperties;
          numberOfProperties_i++ )
    {
        OTF2_Buffer_WriteUint8( writerHandle->buffer,
                                properties[ numberOfProperties_i ] );
        OTF2_Buffer_WriteUint8( writerHandle->buffer,
                                types[ numberOfProperties_i ] );
        OTF2_Type type = types[ numberOfProperties_i ];
        OTF2_Buffer_WriteAttributeValue( writerHandle->buffer,
                                         values[ numberOfProperties_i ] );
    }

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteString( OTF2_GlobalDefWriter* writerHandle,
                                  OTF2_StringRef        self,
                                  const char*           string )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    if ( !string )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid string argument." );
    }
    record_data_length += otf2_buffer_size_string( string );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_STRING );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteString( writerHandle->buffer, string );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteAttribute( OTF2_GlobalDefWriter* writerHandle,
                                     OTF2_AttributeRef     self,
                                     OTF2_StringRef        name,
                                     OTF2_StringRef        description,
                                     OTF2_Type             type )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( type );
    record_data_length += otf2_buffer_size_uint32( description );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_ATTRIBUTE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, type );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, description );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteSystemTreeNode( OTF2_GlobalDefWriter*  writerHandle,
                                          OTF2_SystemTreeNodeRef self,
                                          OTF2_StringRef         name,
                                          OTF2_StringRef         className,
                                          OTF2_SystemTreeNodeRef parent )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( className );
    record_data_length += otf2_buffer_size_uint32( parent );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_SYSTEM_TREE_NODE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, className );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, parent );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteLocationGroup( OTF2_GlobalDefWriter*  writerHandle,
                                         OTF2_LocationGroupRef  self,
                                         OTF2_StringRef         name,
                                         OTF2_LocationGroupType locationGroupType,
                                         OTF2_SystemTreeNodeRef systemTreeParent )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( locationGroupType );
    record_data_length += otf2_buffer_size_uint32( systemTreeParent );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_LOCATION_GROUP );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, locationGroupType );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, systemTreeParent );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteLocation( OTF2_GlobalDefWriter* writerHandle,
                                    OTF2_LocationRef      self,
                                    OTF2_StringRef        name,
                                    OTF2_LocationType     locationType,
                                    uint64_t              numberOfEvents,
                                    OTF2_LocationGroupRef locationGroup )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint64( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( locationType );
    record_data_length += otf2_buffer_size_uint64( numberOfEvents );
    record_data_length += otf2_buffer_size_uint32( locationGroup );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_LOCATION );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, locationType );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, numberOfEvents );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, locationGroup );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    writerHandle->archive->number_of_locations++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteRegion( OTF2_GlobalDefWriter* writerHandle,
                                  OTF2_RegionRef        self,
                                  OTF2_StringRef        name,
                                  OTF2_StringRef        canonicalName,
                                  OTF2_StringRef        description,
                                  OTF2_RegionRole       regionRole,
                                  OTF2_Paradigm         paradigm,
                                  OTF2_RegionFlag       regionFlags,
                                  OTF2_StringRef        sourceFile,
                                  uint32_t              beginLineNumber,
                                  uint32_t              endLineNumber )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /* Generate pre 1.1 record fields */
    OTF2_RegionType regionType;
    otf2_global_def_region_provide_pre_1_1( &regionType,
                                            name,
                                            canonicalName,
                                            description,
                                            regionRole,
                                            paradigm,
                                            regionFlags,
                                            sourceFile,
                                            beginLineNumber,
                                            endLineNumber );

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( description );
    record_data_length += otf2_buffer_size_uint8( regionType );
    record_data_length += otf2_buffer_size_uint32( sourceFile );
    record_data_length += otf2_buffer_size_uint32( beginLineNumber );
    record_data_length += otf2_buffer_size_uint32( endLineNumber );
    record_data_length += otf2_buffer_size_uint32( canonicalName );
    record_data_length += otf2_buffer_size_uint8( regionRole );
    record_data_length += otf2_buffer_size_uint8( paradigm );
    record_data_length += otf2_buffer_size_uint32( regionFlags );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_REGION );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, description );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, regionType );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, sourceFile );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, beginLineNumber );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, endLineNumber );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, canonicalName );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, regionRole );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, paradigm );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, regionFlags );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteCallsite( OTF2_GlobalDefWriter* writerHandle,
                                    OTF2_CallsiteRef      self,
                                    OTF2_StringRef        sourceFile,
                                    uint32_t              lineNumber,
                                    OTF2_RegionRef        enteredRegion,
                                    OTF2_RegionRef        leftRegion )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( sourceFile );
    record_data_length += otf2_buffer_size_uint32( lineNumber );
    record_data_length += otf2_buffer_size_uint32( enteredRegion );
    record_data_length += otf2_buffer_size_uint32( leftRegion );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_CALLSITE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, sourceFile );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, lineNumber );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, enteredRegion );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, leftRegion );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteCallpath( OTF2_GlobalDefWriter* writerHandle,
                                    OTF2_CallpathRef      self,
                                    OTF2_CallpathRef      parent,
                                    OTF2_RegionRef        region )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( parent );
    record_data_length += otf2_buffer_size_uint32( region );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_CALLPATH );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, parent );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, region );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteGroup( OTF2_GlobalDefWriter* writerHandle,
                                 OTF2_GroupRef         self,
                                 OTF2_StringRef        name,
                                 OTF2_GroupType        groupType,
                                 OTF2_Paradigm         paradigm,
                                 OTF2_GroupFlag        groupFlags,
                                 uint32_t              numberOfMembers,
                                 const uint64_t*       members )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /* Generate pre 1.2 record fields */
    OTF2_GroupTypePre12 groupTypePre12;
    otf2_global_def_group_provide_pre_1_2( &groupTypePre12,
                                           name,
                                           groupType,
                                           paradigm,
                                           groupFlags,
                                           numberOfMembers,
                                           members );

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( groupTypePre12 );
    record_data_length += otf2_buffer_size_uint32( numberOfMembers );
    if ( numberOfMembers > 0 && !members )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid members array argument." );
    }
    record_data_length += otf2_buffer_array_size_uint64( numberOfMembers, members );
    record_data_length += otf2_buffer_size_uint8( groupType );
    record_data_length += otf2_buffer_size_uint8( paradigm );
    record_data_length += otf2_buffer_size_uint32( groupFlags );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_GROUP );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, groupTypePre12 );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, numberOfMembers );
    for ( uint32_t numberOfMembers_i = 0;
          numberOfMembers_i < numberOfMembers;
          numberOfMembers_i++ )
    {
        OTF2_Buffer_WriteUint64( writerHandle->buffer,
                                 members[ numberOfMembers_i ] );
    }
    OTF2_Buffer_WriteUint8( writerHandle->buffer, groupType );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, paradigm );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, groupFlags );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteMetricMember( OTF2_GlobalDefWriter* writerHandle,
                                        OTF2_MetricMemberRef  self,
                                        OTF2_StringRef        name,
                                        OTF2_StringRef        description,
                                        OTF2_MetricType       metricType,
                                        OTF2_MetricMode       metricMode,
                                        OTF2_Type             valueType,
                                        OTF2_Base             base,
                                        int64_t               exponent,
                                        OTF2_StringRef        unit )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( description );
    record_data_length += otf2_buffer_size_uint8( metricType );
    record_data_length += otf2_buffer_size_uint8( metricMode );
    record_data_length += otf2_buffer_size_uint8( valueType );
    record_data_length += otf2_buffer_size_uint8( base );
    record_data_length += otf2_buffer_size_int64( exponent );
    record_data_length += otf2_buffer_size_uint32( unit );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_METRIC_MEMBER );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, description );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, metricType );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, metricMode );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, valueType );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, base );
    OTF2_Buffer_WriteInt64( writerHandle->buffer, exponent );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, unit );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteMetricClass( OTF2_GlobalDefWriter*       writerHandle,
                                       OTF2_MetricRef              self,
                                       uint8_t                     numberOfMetrics,
                                       const OTF2_MetricMemberRef* metricMembers,
                                       OTF2_MetricOccurrence       metricOccurrence,
                                       OTF2_RecorderKind           recorderKind )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint8( numberOfMetrics );
    if ( numberOfMetrics > 0 && !metricMembers )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid metricMembers array argument." );
    }
    record_data_length += otf2_buffer_array_size_uint32( numberOfMetrics, metricMembers );
    record_data_length += otf2_buffer_size_uint8( metricOccurrence );
    record_data_length += otf2_buffer_size_uint8( recorderKind );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_METRIC_CLASS );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, numberOfMetrics );
    for ( uint8_t numberOfMetrics_i = 0;
          numberOfMetrics_i < numberOfMetrics;
          numberOfMetrics_i++ )
    {
        OTF2_Buffer_WriteUint32( writerHandle->buffer,
                                 metricMembers[ numberOfMetrics_i ] );
    }
    OTF2_Buffer_WriteUint8( writerHandle->buffer, metricOccurrence );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, recorderKind );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteMetricInstance( OTF2_GlobalDefWriter* writerHandle,
                                          OTF2_MetricRef        self,
                                          OTF2_MetricRef        metricClass,
                                          OTF2_LocationRef      recorder,
                                          OTF2_MetricScope      metricScope,
                                          uint64_t              scope )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( metricClass );
    record_data_length += otf2_buffer_size_uint64( recorder );
    record_data_length += otf2_buffer_size_uint8( metricScope );
    record_data_length += otf2_buffer_size_uint64( scope );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_METRIC_INSTANCE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, metricClass );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, recorder );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, metricScope );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, scope );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteComm( OTF2_GlobalDefWriter* writerHandle,
                                OTF2_CommRef          self,
                                OTF2_StringRef        name,
                                OTF2_GroupRef         group,
                                OTF2_CommRef          parent )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( group );
    record_data_length += otf2_buffer_size_uint32( parent );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_COMM );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, group );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, parent );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteParameter( OTF2_GlobalDefWriter* writerHandle,
                                     OTF2_ParameterRef     self,
                                     OTF2_StringRef        name,
                                     OTF2_ParameterType    parameterType )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( parameterType );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_PARAMETER );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, parameterType );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteRmaWin( OTF2_GlobalDefWriter* writerHandle,
                                  OTF2_RmaWinRef        self,
                                  OTF2_StringRef        name,
                                  OTF2_CommRef          comm )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( comm );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_RMA_WIN );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, comm );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteMetricClassRecorder( OTF2_GlobalDefWriter* writerHandle,
                                               OTF2_MetricRef        metric,
                                               OTF2_LocationRef      recorder )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( metric );
    record_data_length += otf2_buffer_size_uint64( recorder );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_METRIC_CLASS_RECORDER );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, metric );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, recorder );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteSystemTreeNodeProperty( OTF2_GlobalDefWriter*  writerHandle,
                                                  OTF2_SystemTreeNodeRef systemTreeNode,
                                                  OTF2_StringRef         name,
                                                  OTF2_Type              type,
                                                  OTF2_AttributeValue    value )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /* Generate pre 2.0 record fields */
    OTF2_StringRef stringValue;
    otf2_global_def_system_tree_node_property_provide_pre_2_0( &stringValue,
                                                               name,
                                                               type,
                                                               value );

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( systemTreeNode );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( stringValue );
    record_data_length += otf2_buffer_size_uint8( type );
    record_data_length += otf2_buffer_size_attribute_value( value );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_SYSTEM_TREE_NODE_PROPERTY );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, systemTreeNode );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, stringValue );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, type );
    OTF2_Buffer_WriteAttributeValue( writerHandle->buffer, value );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteSystemTreeNodeDomain( OTF2_GlobalDefWriter*  writerHandle,
                                                OTF2_SystemTreeNodeRef systemTreeNode,
                                                OTF2_SystemTreeDomain  systemTreeDomain )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( systemTreeNode );
    record_data_length += otf2_buffer_size_uint8( systemTreeDomain );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_SYSTEM_TREE_NODE_DOMAIN );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, systemTreeNode );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, systemTreeDomain );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteLocationGroupProperty( OTF2_GlobalDefWriter* writerHandle,
                                                 OTF2_LocationGroupRef locationGroup,
                                                 OTF2_StringRef        name,
                                                 OTF2_Type             type,
                                                 OTF2_AttributeValue   value )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /* Generate pre 2.0 record fields */
    OTF2_StringRef stringValue;
    otf2_global_def_location_group_property_provide_pre_2_0( &stringValue,
                                                             name,
                                                             type,
                                                             value );

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( locationGroup );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( stringValue );
    record_data_length += otf2_buffer_size_uint8( type );
    record_data_length += otf2_buffer_size_attribute_value( value );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_LOCATION_GROUP_PROPERTY );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, locationGroup );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, stringValue );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, type );
    OTF2_Buffer_WriteAttributeValue( writerHandle->buffer, value );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteLocationProperty( OTF2_GlobalDefWriter* writerHandle,
                                            OTF2_LocationRef      location,
                                            OTF2_StringRef        name,
                                            OTF2_Type             type,
                                            OTF2_AttributeValue   value )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /* Generate pre 2.0 record fields */
    OTF2_StringRef stringValue;
    otf2_global_def_location_property_provide_pre_2_0( &stringValue,
                                                       name,
                                                       type,
                                                       value );

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint64( location );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( stringValue );
    record_data_length += otf2_buffer_size_uint8( type );
    record_data_length += otf2_buffer_size_attribute_value( value );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_LOCATION_PROPERTY );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint64( writerHandle->buffer, location );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, stringValue );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, type );
    OTF2_Buffer_WriteAttributeValue( writerHandle->buffer, value );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteCartDimension( OTF2_GlobalDefWriter* writerHandle,
                                         OTF2_CartDimensionRef self,
                                         OTF2_StringRef        name,
                                         uint32_t              size,
                                         OTF2_CartPeriodicity  cartPeriodicity )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( size );
    record_data_length += otf2_buffer_size_uint8( cartPeriodicity );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_CART_DIMENSION );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, size );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, cartPeriodicity );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteCartTopology( OTF2_GlobalDefWriter*        writerHandle,
                                        OTF2_CartTopologyRef         self,
                                        OTF2_StringRef               name,
                                        OTF2_CommRef                 communicator,
                                        uint8_t                      numberOfDimensions,
                                        const OTF2_CartDimensionRef* cartDimensions )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( communicator );
    record_data_length += otf2_buffer_size_uint8( numberOfDimensions );
    if ( numberOfDimensions > 0 && !cartDimensions )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid cartDimensions array argument." );
    }
    record_data_length += otf2_buffer_array_size_uint32( numberOfDimensions, cartDimensions );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_CART_TOPOLOGY );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, communicator );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, numberOfDimensions );
    for ( uint8_t numberOfDimensions_i = 0;
          numberOfDimensions_i < numberOfDimensions;
          numberOfDimensions_i++ )
    {
        OTF2_Buffer_WriteUint32( writerHandle->buffer,
                                 cartDimensions[ numberOfDimensions_i ] );
    }

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteCartCoordinate( OTF2_GlobalDefWriter* writerHandle,
                                          OTF2_CartTopologyRef  cartTopology,
                                          uint32_t              rank,
                                          uint8_t               numberOfDimensions,
                                          const uint32_t*       coordinates )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( cartTopology );
    record_data_length += otf2_buffer_size_uint32( rank );
    record_data_length += otf2_buffer_size_uint8( numberOfDimensions );
    if ( numberOfDimensions > 0 && !coordinates )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid coordinates array argument." );
    }
    record_data_length += otf2_buffer_array_size_uint32( numberOfDimensions, coordinates );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_CART_COORDINATE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, cartTopology );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, rank );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, numberOfDimensions );
    for ( uint8_t numberOfDimensions_i = 0;
          numberOfDimensions_i < numberOfDimensions;
          numberOfDimensions_i++ )
    {
        OTF2_Buffer_WriteUint32( writerHandle->buffer,
                                 coordinates[ numberOfDimensions_i ] );
    }

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteSourceCodeLocation( OTF2_GlobalDefWriter*      writerHandle,
                                              OTF2_SourceCodeLocationRef self,
                                              OTF2_StringRef             file,
                                              uint32_t                   lineNumber )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( file );
    record_data_length += otf2_buffer_size_uint32( lineNumber );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_SOURCE_CODE_LOCATION );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, file );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, lineNumber );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteCallingContext( OTF2_GlobalDefWriter*      writerHandle,
                                          OTF2_CallingContextRef     self,
                                          OTF2_RegionRef             region,
                                          OTF2_SourceCodeLocationRef sourceCodeLocation,
                                          OTF2_CallingContextRef     parent )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( region );
    record_data_length += otf2_buffer_size_uint32( sourceCodeLocation );
    record_data_length += otf2_buffer_size_uint32( parent );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_CALLING_CONTEXT );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, region );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, sourceCodeLocation );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, parent );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteCallingContextProperty( OTF2_GlobalDefWriter*  writerHandle,
                                                  OTF2_CallingContextRef callingContext,
                                                  OTF2_StringRef         name,
                                                  OTF2_Type              type,
                                                  OTF2_AttributeValue    value )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( callingContext );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( type );
    record_data_length += otf2_buffer_size_attribute_value( value );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_CALLING_CONTEXT_PROPERTY );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, callingContext );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, type );
    OTF2_Buffer_WriteAttributeValue( writerHandle->buffer, value );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteInterruptGenerator( OTF2_GlobalDefWriter*       writerHandle,
                                              OTF2_InterruptGeneratorRef  self,
                                              OTF2_StringRef              name,
                                              OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                              OTF2_Base                   base,
                                              int64_t                     exponent,
                                              uint64_t                    period )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( interruptGeneratorMode );
    record_data_length += otf2_buffer_size_uint8( base );
    record_data_length += otf2_buffer_size_int64( exponent );
    record_data_length += otf2_buffer_size_uint64( period );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_INTERRUPT_GENERATOR );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, interruptGeneratorMode );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, base );
    OTF2_Buffer_WriteInt64( writerHandle->buffer, exponent );
    OTF2_Buffer_WriteUint64( writerHandle->buffer, period );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteIoFileProperty( OTF2_GlobalDefWriter* writerHandle,
                                          OTF2_IoFileRef        ioFile,
                                          OTF2_StringRef        name,
                                          OTF2_Type             type,
                                          OTF2_AttributeValue   value )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( ioFile );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint8( type );
    record_data_length += otf2_buffer_size_attribute_value( value );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_IO_FILE_PROPERTY );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, ioFile );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, type );
    OTF2_Buffer_WriteAttributeValue( writerHandle->buffer, value );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteIoRegularFile( OTF2_GlobalDefWriter*  writerHandle,
                                         OTF2_IoFileRef         self,
                                         OTF2_StringRef         name,
                                         OTF2_SystemTreeNodeRef scope )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( scope );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_IO_REGULAR_FILE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, scope );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteIoDirectory( OTF2_GlobalDefWriter*  writerHandle,
                                       OTF2_IoFileRef         self,
                                       OTF2_StringRef         name,
                                       OTF2_SystemTreeNodeRef scope )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( scope );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_IO_DIRECTORY );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, scope );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteIoHandle( OTF2_GlobalDefWriter* writerHandle,
                                    OTF2_IoHandleRef      self,
                                    OTF2_StringRef        name,
                                    OTF2_IoFileRef        file,
                                    OTF2_IoParadigmRef    ioParadigm,
                                    OTF2_IoHandleFlag     ioHandleFlags,
                                    OTF2_CommRef          comm,
                                    OTF2_IoHandleRef      parent )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( self );
    record_data_length += otf2_buffer_size_uint32( name );
    record_data_length += otf2_buffer_size_uint32( file );
    record_data_length += otf2_buffer_size_uint8( ioParadigm );
    record_data_length += otf2_buffer_size_uint32( ioHandleFlags );
    record_data_length += otf2_buffer_size_uint32( comm );
    record_data_length += otf2_buffer_size_uint32( parent );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_IO_HANDLE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, self );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, name );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, file );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, ioParadigm );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, ioHandleFlags );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, comm );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, parent );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteIoPreCreatedHandleState( OTF2_GlobalDefWriter* writerHandle,
                                                   OTF2_IoHandleRef      ioHandle,
                                                   OTF2_IoAccessMode     mode,
                                                   OTF2_IoStatusFlag     statusFlags )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( ioHandle );
    record_data_length += otf2_buffer_size_uint8( mode );
    record_data_length += otf2_buffer_size_uint32( statusFlags );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_IO_PRE_CREATED_HANDLE_STATE );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, ioHandle );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, mode );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, statusFlags );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_GlobalDefWriter_WriteCallpathParameter( OTF2_GlobalDefWriter* writerHandle,
                                             OTF2_CallpathRef      callpath,
                                             OTF2_ParameterRef     parameter,
                                             OTF2_Type             type,
                                             OTF2_AttributeValue   value )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid writerHandle argument." );
    }

    /*
     * Maximum memory usage of pure record data (excluding type ID
     * and record length information).
     */
    uint64_t record_data_length = 0;
    /*
     * Maximum record length is the record id byte, plus the individual sizes of the
     * attributes.
     */
    uint64_t record_length = 1;

    record_data_length += otf2_buffer_size_uint32( callpath );
    record_data_length += otf2_buffer_size_uint32( parameter );
    record_data_length += otf2_buffer_size_uint8( type );
    record_data_length += otf2_buffer_size_attribute_value( value );
    /* Finalize the upper bound of the record. */
    record_length += record_data_length;

    /*
     * Additional byte to store real record length. Due to on-line compression
     * real record length can differ from maximum record length.
     */
    record_length += 1;
    if ( record_data_length >= UINT8_MAX )
    {
        record_length += 8;
    }

    OTF2_ErrorCode status =
        OTF2_Buffer_WriteMemoryRequest( writerHandle->buffer, record_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_GLOBAL_DEF_CALLPATH_PARAMETER );

    /* Write initial record length */
    OTF2_Buffer_WriteInitialRecordLength( writerHandle->buffer, record_data_length );

    OTF2_Buffer_WriteUint32( writerHandle->buffer, callpath );
    OTF2_Buffer_WriteUint32( writerHandle->buffer, parameter );
    OTF2_Buffer_WriteUint8( writerHandle->buffer, type );
    OTF2_Buffer_WriteAttributeValue( writerHandle->buffer, value );

    /* Write real record length */
    status = OTF2_Buffer_WriteFinalRecordLength( writerHandle->buffer, record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return status;
    }

    OTF2_ARCHIVE_LOCK( writerHandle->archive );
    writerHandle->archive->number_of_global_defs++;
    OTF2_ARCHIVE_UNLOCK( writerHandle->archive );

    return OTF2_SUCCESS;
}
