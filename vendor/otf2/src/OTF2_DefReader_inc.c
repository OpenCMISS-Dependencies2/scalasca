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
 *  @source     templates/OTF2_DefReader_inc.tmpl.c
 *
 *  @brief      This is the local definition reader, which reads location
 *              dependend definitions, and can also be used to get the mapping
 *              information from the local definition file. Local definitions
 *              are always assigned to a location.
 */


/** @brief Reads a MappingTable record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_mapping_table( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefMappingTable static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.mapping_type );
    ret = OTF2_Buffer_ReadIdMap( reader->buffer, &static_record.id_map );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read idMap attribute of MappingTable record. Can't read IdMap." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        OTF2_IdMap_Free( ( OTF2_IdMap* )static_record.id_map );
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    ret = otf2_archive_location_set_mapping_table( reader->archive,
                                                   reader->location_id,
                                                   static_record.mapping_type,
                                                   static_record.id_map );

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.mapping_table )
    {
        interrupt = reader->reader_callbacks.mapping_table( reader->user_data,
                                                            static_record.mapping_type,
                                                            static_record.id_map );
    }


    if ( OTF2_SUCCESS != ret )
    {
        OTF2_IdMap_Free( ( OTF2_IdMap* )static_record.id_map );
        return UTILS_ERROR( ret, "Can't add MappingTable definition to associated event reader." );
    }

    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a ClockOffset record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_clock_offset( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefClockOffset static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    OTF2_Buffer_ReadUint64Full( reader->buffer, &static_record.time );
    ret = OTF2_Buffer_ReadInt64( reader->buffer, &static_record.offset );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read offset attribute of ClockOffset record. Invalid compression size." );
    }
    OTF2_Buffer_ReadDouble( reader->buffer, &static_record.standard_deviation );

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    ret = otf2_archive_location_add_clock_offset( reader->archive,
                                                  reader->location_id,
                                                  static_record.time,
                                                  static_record.offset,
                                                  static_record.standard_deviation );

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.clock_offset )
    {
        interrupt = reader->reader_callbacks.clock_offset( reader->user_data,
                                                           static_record.time,
                                                           static_record.offset,
                                                           static_record.standard_deviation );
    }


    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Can't add ClockOffset definition to associated event reader." );
    }

    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a String record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_string( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefString static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of String record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadString( reader->buffer, &static_record.string );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read string attribute of String record. Can't read string from buffer." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.string )
    {
        interrupt = reader->reader_callbacks.string( reader->user_data,
                                                     static_record.self,
                                                     static_record.string );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a Attribute record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_attribute( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefAttribute static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    uint8_t* current_pos;
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of Attribute record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of Attribute record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.type );
    OTF2_Type type = static_record.type;

    /*
     * Read record attributes added in version 1.4, if there is
     * space left in the record
     */
    OTF2_Buffer_GetPosition( reader->buffer, &current_pos );
    if ( current_pos < record_end_pos )
    {
        ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.description );
        if ( OTF2_SUCCESS != ret )
        {
            return UTILS_ERROR( ret, "Could not read description attribute of Attribute record. Invalid compression size." );
        }
    }
    else
    {
        otf2_local_def_attribute_convert_pre_1_4( &static_record );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.attribute )
    {
        interrupt = reader->reader_callbacks.attribute( reader->user_data,
                                                        static_record.self,
                                                        static_record.name,
                                                        static_record.description,
                                                        static_record.type );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a SystemTreeNode record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_system_tree_node( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefSystemTreeNode static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of SystemTreeNode record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of SystemTreeNode record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.class_name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read className attribute of SystemTreeNode record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.parent );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read parent attribute of SystemTreeNode record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.system_tree_node )
    {
        interrupt = reader->reader_callbacks.system_tree_node( reader->user_data,
                                                               static_record.self,
                                                               static_record.name,
                                                               static_record.class_name,
                                                               static_record.parent );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a LocationGroup record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_location_group( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefLocationGroup static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of LocationGroup record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of LocationGroup record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.location_group_type );
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.system_tree_parent );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read systemTreeParent attribute of LocationGroup record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.location_group )
    {
        interrupt = reader->reader_callbacks.location_group( reader->user_data,
                                                             static_record.self,
                                                             static_record.name,
                                                             static_record.location_group_type,
                                                             static_record.system_tree_parent );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a Location record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_location( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefLocation static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint64( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of Location record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of Location record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.location_type );
    ret = OTF2_Buffer_ReadUint64( reader->buffer, &static_record.number_of_events );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read numberOfEvents attribute of Location record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.location_group );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read locationGroup attribute of Location record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.location )
    {
        interrupt = reader->reader_callbacks.location( reader->user_data,
                                                       static_record.self,
                                                       static_record.name,
                                                       static_record.location_type,
                                                       static_record.number_of_events,
                                                       static_record.location_group );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a Region record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_region( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefRegion static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    uint8_t* current_pos;
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of Region record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of Region record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.description );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read description attribute of Region record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.region_type );
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.source_file );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read sourceFile attribute of Region record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.begin_line_number );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read beginLineNumber attribute of Region record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.end_line_number );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read endLineNumber attribute of Region record. Invalid compression size." );
    }

    /*
     * Read record attributes added in version 1.1, if there is
     * space left in the record
     */
    OTF2_Buffer_GetPosition( reader->buffer, &current_pos );
    if ( current_pos < record_end_pos )
    {
        ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.canonical_name );
        if ( OTF2_SUCCESS != ret )
        {
            return UTILS_ERROR( ret, "Could not read canonicalName attribute of Region record. Invalid compression size." );
        }
        OTF2_Buffer_ReadUint8( reader->buffer, &static_record.region_role );
        OTF2_Buffer_ReadUint8( reader->buffer, &static_record.paradigm );
        ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.region_flags );
        if ( OTF2_SUCCESS != ret )
        {
            return UTILS_ERROR( ret, "Could not read regionFlags attribute of Region record. Invalid compression size." );
        }
    }
    else
    {
        otf2_local_def_region_convert_pre_1_1( &static_record );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.region )
    {
        interrupt = reader->reader_callbacks.region( reader->user_data,
                                                     static_record.self,
                                                     static_record.name,
                                                     static_record.canonical_name,
                                                     static_record.description,
                                                     static_record.region_role,
                                                     static_record.paradigm,
                                                     static_record.region_flags,
                                                     static_record.source_file,
                                                     static_record.begin_line_number,
                                                     static_record.end_line_number );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a Callsite record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_callsite( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefCallsite static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of Callsite record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.source_file );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read sourceFile attribute of Callsite record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.line_number );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read lineNumber attribute of Callsite record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.entered_region );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read enteredRegion attribute of Callsite record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.left_region );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read leftRegion attribute of Callsite record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.callsite )
    {
        interrupt = reader->reader_callbacks.callsite( reader->user_data,
                                                       static_record.self,
                                                       static_record.source_file,
                                                       static_record.line_number,
                                                       static_record.entered_region,
                                                       static_record.left_region );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a Callpath record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_callpath( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefCallpath static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of Callpath record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.parent );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read parent attribute of Callpath record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.region );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read region attribute of Callpath record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.callpath )
    {
        interrupt = reader->reader_callbacks.callpath( reader->user_data,
                                                       static_record.self,
                                                       static_record.parent,
                                                       static_record.region );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a Group record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_group( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefGroup static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    uint8_t* current_pos;
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of Group record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of Group record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.group_type_pre12 );
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.number_of_members );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read numberOfMembers attribute of Group record. Invalid compression size." );
    }
    static_record.members = malloc( static_record.number_of_members * sizeof( uint64_t ) );
    if ( static_record.number_of_members > 0 && !static_record.members )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Could not allocate memory for members array!" );
    }
    for ( uint32_t number_of_members_i = 0;
          number_of_members_i < static_record.number_of_members;
          number_of_members_i++ )
    {
        ret = OTF2_Buffer_ReadUint64( reader->buffer,
                                      static_record.members + number_of_members_i );
        if ( OTF2_SUCCESS != ret )
        {
            free( static_record.members );
            return UTILS_ERROR( ret, "Could not read members attribute of Group record. Invalid compression size." );
        }
    }

    /*
     * Read record attributes added in version 1.2, if there is
     * space left in the record
     */
    OTF2_Buffer_GetPosition( reader->buffer, &current_pos );
    if ( current_pos < record_end_pos )
    {
        OTF2_Buffer_ReadUint8( reader->buffer, &static_record.group_type );
        OTF2_Buffer_ReadUint8( reader->buffer, &static_record.paradigm );
        ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.group_flags );
        if ( OTF2_SUCCESS != ret )
        {
            return UTILS_ERROR( ret, "Could not read groupFlags attribute of Group record. Invalid compression size." );
        }
    }
    else
    {
        otf2_local_def_group_convert_pre_1_2( &static_record );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        free( static_record.members );
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.group )
    {
        interrupt = reader->reader_callbacks.group( reader->user_data,
                                                    static_record.self,
                                                    static_record.name,
                                                    static_record.group_type,
                                                    static_record.paradigm,
                                                    static_record.group_flags,
                                                    static_record.number_of_members,
                                                    static_record.members );
    }

    free( static_record.members );

    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a MetricMember record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_metric_member( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefMetricMember static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of MetricMember record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of MetricMember record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.description );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read description attribute of MetricMember record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.metric_type );
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.metric_mode );
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.value_type );
    OTF2_Type type = static_record.value_type;
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.base );
    ret = OTF2_Buffer_ReadInt64( reader->buffer, &static_record.exponent );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read exponent attribute of MetricMember record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.unit );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read unit attribute of MetricMember record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.metric_member )
    {
        interrupt = reader->reader_callbacks.metric_member( reader->user_data,
                                                            static_record.self,
                                                            static_record.name,
                                                            static_record.description,
                                                            static_record.metric_type,
                                                            static_record.metric_mode,
                                                            static_record.value_type,
                                                            static_record.base,
                                                            static_record.exponent,
                                                            static_record.unit );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a MetricClass record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_metric_class( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefMetricClass static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    uint8_t* current_pos;
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of MetricClass record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.number_of_metrics );
    static_record.metric_members = malloc( static_record.number_of_metrics * sizeof( OTF2_MetricMemberRef ) );
    if ( static_record.number_of_metrics > 0 && !static_record.metric_members )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Could not allocate memory for metricMembers array!" );
    }
    for ( uint8_t number_of_metrics_i = 0;
          number_of_metrics_i < static_record.number_of_metrics;
          number_of_metrics_i++ )
    {
        ret = OTF2_Buffer_ReadUint32( reader->buffer,
                                      static_record.metric_members + number_of_metrics_i );
        if ( OTF2_SUCCESS != ret )
        {
            free( static_record.metric_members );
            return UTILS_ERROR( ret, "Could not read metricMembers attribute of MetricClass record. Invalid compression size." );
        }
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.metric_occurrence );

    /*
     * Read record attributes added in version 1.2, if there is
     * space left in the record
     */
    OTF2_Buffer_GetPosition( reader->buffer, &current_pos );
    if ( current_pos < record_end_pos )
    {
        OTF2_Buffer_ReadUint8( reader->buffer, &static_record.recorder_kind );
    }
    else
    {
        otf2_local_def_metric_class_convert_pre_1_2( &static_record );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        free( static_record.metric_members );
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.metric_class )
    {
        interrupt = reader->reader_callbacks.metric_class( reader->user_data,
                                                           static_record.self,
                                                           static_record.number_of_metrics,
                                                           static_record.metric_members,
                                                           static_record.metric_occurrence,
                                                           static_record.recorder_kind );
    }

    free( static_record.metric_members );

    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a MetricInstance record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_metric_instance( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefMetricInstance static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of MetricInstance record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.metric_class );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read metricClass attribute of MetricInstance record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint64( reader->buffer, &static_record.recorder );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read recorder attribute of MetricInstance record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.metric_scope );
    ret = OTF2_Buffer_ReadUint64( reader->buffer, &static_record.scope );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read scope attribute of MetricInstance record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.metric_instance )
    {
        interrupt = reader->reader_callbacks.metric_instance( reader->user_data,
                                                              static_record.self,
                                                              static_record.metric_class,
                                                              static_record.recorder,
                                                              static_record.metric_scope,
                                                              static_record.scope );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a Comm record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_comm( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefComm static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of Comm record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of Comm record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.group );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read group attribute of Comm record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.parent );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read parent attribute of Comm record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.comm )
    {
        interrupt = reader->reader_callbacks.comm( reader->user_data,
                                                   static_record.self,
                                                   static_record.name,
                                                   static_record.group,
                                                   static_record.parent );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a Parameter record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_parameter( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefParameter static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of Parameter record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of Parameter record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.parameter_type );

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.parameter )
    {
        interrupt = reader->reader_callbacks.parameter( reader->user_data,
                                                        static_record.self,
                                                        static_record.name,
                                                        static_record.parameter_type );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a RmaWin record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_rma_win( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefRmaWin static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of RmaWin record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of RmaWin record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.comm );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read comm attribute of RmaWin record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.rma_win )
    {
        interrupt = reader->reader_callbacks.rma_win( reader->user_data,
                                                      static_record.self,
                                                      static_record.name,
                                                      static_record.comm );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a MetricClassRecorder record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_metric_class_recorder( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefMetricClassRecorder static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.metric );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read metric attribute of MetricClassRecorder record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint64( reader->buffer, &static_record.recorder );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read recorder attribute of MetricClassRecorder record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.metric_class_recorder )
    {
        interrupt = reader->reader_callbacks.metric_class_recorder( reader->user_data,
                                                                    static_record.metric,
                                                                    static_record.recorder );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a SystemTreeNodeProperty record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_system_tree_node_property( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefSystemTreeNodeProperty static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    uint8_t* current_pos;
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.system_tree_node );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read systemTreeNode attribute of SystemTreeNodeProperty record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of SystemTreeNodeProperty record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.string_value );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read stringValue attribute of SystemTreeNodeProperty record. Invalid compression size." );
    }

    /*
     * Read record attributes added in version 2.0, if there is
     * space left in the record
     */
    OTF2_Buffer_GetPosition( reader->buffer, &current_pos );
    if ( current_pos < record_end_pos )
    {
        OTF2_Buffer_ReadUint8( reader->buffer, &static_record.type );
        OTF2_Type type = static_record.type;
        ret = OTF2_Buffer_ReadAttributeValue( reader->buffer, &static_record.value );
        if ( OTF2_SUCCESS != ret )
        {
            return UTILS_ERROR( ret, "Could not read value attribute of SystemTreeNodeProperty record. Could not read attribute value." );
        }
    }
    else
    {
        otf2_local_def_system_tree_node_property_convert_pre_2_0( &static_record );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.system_tree_node_property )
    {
        interrupt = reader->reader_callbacks.system_tree_node_property( reader->user_data,
                                                                        static_record.system_tree_node,
                                                                        static_record.name,
                                                                        static_record.type,
                                                                        static_record.value );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a SystemTreeNodeDomain record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_system_tree_node_domain( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefSystemTreeNodeDomain static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.system_tree_node );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read systemTreeNode attribute of SystemTreeNodeDomain record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.system_tree_domain );

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.system_tree_node_domain )
    {
        interrupt = reader->reader_callbacks.system_tree_node_domain( reader->user_data,
                                                                      static_record.system_tree_node,
                                                                      static_record.system_tree_domain );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a LocationGroupProperty record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_location_group_property( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefLocationGroupProperty static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    uint8_t* current_pos;
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.location_group );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read locationGroup attribute of LocationGroupProperty record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of LocationGroupProperty record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.string_value );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read stringValue attribute of LocationGroupProperty record. Invalid compression size." );
    }

    /*
     * Read record attributes added in version 2.0, if there is
     * space left in the record
     */
    OTF2_Buffer_GetPosition( reader->buffer, &current_pos );
    if ( current_pos < record_end_pos )
    {
        OTF2_Buffer_ReadUint8( reader->buffer, &static_record.type );
        OTF2_Type type = static_record.type;
        ret = OTF2_Buffer_ReadAttributeValue( reader->buffer, &static_record.value );
        if ( OTF2_SUCCESS != ret )
        {
            return UTILS_ERROR( ret, "Could not read value attribute of LocationGroupProperty record. Could not read attribute value." );
        }
    }
    else
    {
        otf2_local_def_location_group_property_convert_pre_2_0( &static_record );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.location_group_property )
    {
        interrupt = reader->reader_callbacks.location_group_property( reader->user_data,
                                                                      static_record.location_group,
                                                                      static_record.name,
                                                                      static_record.type,
                                                                      static_record.value );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a LocationProperty record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_location_property( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefLocationProperty static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    uint8_t* current_pos;
    ret = OTF2_Buffer_ReadUint64( reader->buffer, &static_record.location );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read location attribute of LocationProperty record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of LocationProperty record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.string_value );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read stringValue attribute of LocationProperty record. Invalid compression size." );
    }

    /*
     * Read record attributes added in version 2.0, if there is
     * space left in the record
     */
    OTF2_Buffer_GetPosition( reader->buffer, &current_pos );
    if ( current_pos < record_end_pos )
    {
        OTF2_Buffer_ReadUint8( reader->buffer, &static_record.type );
        OTF2_Type type = static_record.type;
        ret = OTF2_Buffer_ReadAttributeValue( reader->buffer, &static_record.value );
        if ( OTF2_SUCCESS != ret )
        {
            return UTILS_ERROR( ret, "Could not read value attribute of LocationProperty record. Could not read attribute value." );
        }
    }
    else
    {
        otf2_local_def_location_property_convert_pre_2_0( &static_record );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.location_property )
    {
        interrupt = reader->reader_callbacks.location_property( reader->user_data,
                                                                static_record.location,
                                                                static_record.name,
                                                                static_record.type,
                                                                static_record.value );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a CartDimension record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_cart_dimension( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefCartDimension static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of CartDimension record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of CartDimension record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.size );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read size attribute of CartDimension record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.cart_periodicity );

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.cart_dimension )
    {
        interrupt = reader->reader_callbacks.cart_dimension( reader->user_data,
                                                             static_record.self,
                                                             static_record.name,
                                                             static_record.size,
                                                             static_record.cart_periodicity );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a CartTopology record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_cart_topology( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefCartTopology static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of CartTopology record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of CartTopology record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.communicator );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read communicator attribute of CartTopology record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.number_of_dimensions );
    static_record.cart_dimensions = malloc( static_record.number_of_dimensions * sizeof( OTF2_CartDimensionRef ) );
    if ( static_record.number_of_dimensions > 0 && !static_record.cart_dimensions )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Could not allocate memory for cartDimensions array!" );
    }
    for ( uint8_t number_of_dimensions_i = 0;
          number_of_dimensions_i < static_record.number_of_dimensions;
          number_of_dimensions_i++ )
    {
        ret = OTF2_Buffer_ReadUint32( reader->buffer,
                                      static_record.cart_dimensions + number_of_dimensions_i );
        if ( OTF2_SUCCESS != ret )
        {
            free( static_record.cart_dimensions );
            return UTILS_ERROR( ret, "Could not read cartDimensions attribute of CartTopology record. Invalid compression size." );
        }
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        free( static_record.cart_dimensions );
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.cart_topology )
    {
        interrupt = reader->reader_callbacks.cart_topology( reader->user_data,
                                                            static_record.self,
                                                            static_record.name,
                                                            static_record.communicator,
                                                            static_record.number_of_dimensions,
                                                            static_record.cart_dimensions );
    }

    free( static_record.cart_dimensions );

    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a CartCoordinate record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_cart_coordinate( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefCartCoordinate static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.cart_topology );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read cartTopology attribute of CartCoordinate record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.rank );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read rank attribute of CartCoordinate record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.number_of_dimensions );
    static_record.coordinates = malloc( static_record.number_of_dimensions * sizeof( uint32_t ) );
    if ( static_record.number_of_dimensions > 0 && !static_record.coordinates )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Could not allocate memory for coordinates array!" );
    }
    for ( uint8_t number_of_dimensions_i = 0;
          number_of_dimensions_i < static_record.number_of_dimensions;
          number_of_dimensions_i++ )
    {
        ret = OTF2_Buffer_ReadUint32( reader->buffer,
                                      static_record.coordinates + number_of_dimensions_i );
        if ( OTF2_SUCCESS != ret )
        {
            free( static_record.coordinates );
            return UTILS_ERROR( ret, "Could not read coordinates attribute of CartCoordinate record. Invalid compression size." );
        }
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        free( static_record.coordinates );
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.cart_coordinate )
    {
        interrupt = reader->reader_callbacks.cart_coordinate( reader->user_data,
                                                              static_record.cart_topology,
                                                              static_record.rank,
                                                              static_record.number_of_dimensions,
                                                              static_record.coordinates );
    }

    free( static_record.coordinates );

    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a SourceCodeLocation record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_source_code_location( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefSourceCodeLocation static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of SourceCodeLocation record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.file );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read file attribute of SourceCodeLocation record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.line_number );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read lineNumber attribute of SourceCodeLocation record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.source_code_location )
    {
        interrupt = reader->reader_callbacks.source_code_location( reader->user_data,
                                                                   static_record.self,
                                                                   static_record.file,
                                                                   static_record.line_number );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a CallingContext record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_calling_context( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefCallingContext static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of CallingContext record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.region );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read region attribute of CallingContext record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.source_code_location );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read sourceCodeLocation attribute of CallingContext record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.parent );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read parent attribute of CallingContext record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.calling_context )
    {
        interrupt = reader->reader_callbacks.calling_context( reader->user_data,
                                                              static_record.self,
                                                              static_record.region,
                                                              static_record.source_code_location,
                                                              static_record.parent );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a CallingContextProperty record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_calling_context_property( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefCallingContextProperty static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.calling_context );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read callingContext attribute of CallingContextProperty record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of CallingContextProperty record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.type );
    OTF2_Type type = static_record.type;
    ret = OTF2_Buffer_ReadAttributeValue( reader->buffer, &static_record.value );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read value attribute of CallingContextProperty record. Could not read attribute value." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.calling_context_property )
    {
        interrupt = reader->reader_callbacks.calling_context_property( reader->user_data,
                                                                       static_record.calling_context,
                                                                       static_record.name,
                                                                       static_record.type,
                                                                       static_record.value );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a InterruptGenerator record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_interrupt_generator( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefInterruptGenerator static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of InterruptGenerator record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of InterruptGenerator record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.interrupt_generator_mode );
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.base );
    ret = OTF2_Buffer_ReadInt64( reader->buffer, &static_record.exponent );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read exponent attribute of InterruptGenerator record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint64( reader->buffer, &static_record.period );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read period attribute of InterruptGenerator record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.interrupt_generator )
    {
        interrupt = reader->reader_callbacks.interrupt_generator( reader->user_data,
                                                                  static_record.self,
                                                                  static_record.name,
                                                                  static_record.interrupt_generator_mode,
                                                                  static_record.base,
                                                                  static_record.exponent,
                                                                  static_record.period );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a IoFileProperty record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_io_file_property( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefIoFileProperty static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.io_file );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read ioFile attribute of IoFileProperty record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of IoFileProperty record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.type );
    OTF2_Type type = static_record.type;
    ret = OTF2_Buffer_ReadAttributeValue( reader->buffer, &static_record.value );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read value attribute of IoFileProperty record. Could not read attribute value." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.io_file_property )
    {
        interrupt = reader->reader_callbacks.io_file_property( reader->user_data,
                                                               static_record.io_file,
                                                               static_record.name,
                                                               static_record.type,
                                                               static_record.value );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a IoRegularFile record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_io_regular_file( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefIoRegularFile static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of IoRegularFile record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of IoRegularFile record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.scope );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read scope attribute of IoRegularFile record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.io_regular_file )
    {
        interrupt = reader->reader_callbacks.io_regular_file( reader->user_data,
                                                              static_record.self,
                                                              static_record.name,
                                                              static_record.scope );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a IoDirectory record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_io_directory( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefIoDirectory static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of IoDirectory record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of IoDirectory record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.scope );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read scope attribute of IoDirectory record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.io_directory )
    {
        interrupt = reader->reader_callbacks.io_directory( reader->user_data,
                                                           static_record.self,
                                                           static_record.name,
                                                           static_record.scope );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a IoHandle record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_io_handle( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefIoHandle static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.self );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read self attribute of IoHandle record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.name );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read name attribute of IoHandle record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.file );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read file attribute of IoHandle record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.io_paradigm );
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.io_handle_flags );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read ioHandleFlags attribute of IoHandle record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.comm );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read comm attribute of IoHandle record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.parent );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read parent attribute of IoHandle record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.io_handle )
    {
        interrupt = reader->reader_callbacks.io_handle( reader->user_data,
                                                        static_record.self,
                                                        static_record.name,
                                                        static_record.file,
                                                        static_record.io_paradigm,
                                                        static_record.io_handle_flags,
                                                        static_record.comm,
                                                        static_record.parent );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a IoPreCreatedHandleState record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_io_pre_created_handle_state( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefIoPreCreatedHandleState static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.io_handle );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read ioHandle attribute of IoPreCreatedHandleState record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.mode );
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.status_flags );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read statusFlags attribute of IoPreCreatedHandleState record. Invalid compression size." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.io_pre_created_handle_state )
    {
        interrupt = reader->reader_callbacks.io_pre_created_handle_state( reader->user_data,
                                                                          static_record.io_handle,
                                                                          static_record.mode,
                                                                          static_record.status_flags );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads a CallpathParameter record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_SUCCESS
 */
static inline OTF2_ErrorCode
otf2_def_reader_read_callpath_parameter( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_LocalDefCallpathParameter static_record;

    OTF2_ErrorCode ret;
    uint64_t       record_data_length;
    /* Get record length and test memory availability */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.callpath );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read callpath attribute of CallpathParameter record. Invalid compression size." );
    }
    ret = OTF2_Buffer_ReadUint32( reader->buffer, &static_record.parameter );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read parameter attribute of CallpathParameter record. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &static_record.type );
    OTF2_Type type = static_record.type;
    ret = OTF2_Buffer_ReadAttributeValue( reader->buffer, &static_record.value );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read value attribute of CallpathParameter record. Could not read attribute value." );
    }

    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.callpath_parameter )
    {
        interrupt = reader->reader_callbacks.callpath_parameter( reader->user_data,
                                                                 static_record.callpath,
                                                                 static_record.parameter,
                                                                 static_record.type,
                                                                 static_record.value );
    }


    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}


/** @brief Reads one definition record and triggers the callback.
 *
 *  @param reader   Reference to the reader object
 *
 *  @return OTF2_ErrorCode, which is OTF2_SUCCESS if everything is fine.
 */
static OTF2_ErrorCode
otf2_def_reader_read( OTF2_DefReader* reader )
{
    UTILS_ASSERT( reader );

    /* Read record type */
    uint8_t        record_type;
    OTF2_ErrorCode ret;
    ret = OTF2_Buffer_GuaranteeRead( reader->buffer, 1 );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read record type." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &record_type );

    switch ( record_type )
    {
        case OTF2_BUFFER_END_OF_CHUNK:
            ret = OTF2_Buffer_ReadGetNextChunk( reader->buffer );
            if ( OTF2_SUCCESS != ret )
            {
                return UTILS_ERROR( ret, "Load of next chunk failed!" );
            }
            return otf2_def_reader_read( reader );

        case OTF2_BUFFER_END_OF_FILE:
            return OTF2_ERROR_INDEX_OUT_OF_BOUNDS;

        case OTF2_LOCAL_DEF_MAPPING_TABLE:
            return otf2_def_reader_read_mapping_table( reader );

        case OTF2_LOCAL_DEF_CLOCK_OFFSET:
            return otf2_def_reader_read_clock_offset( reader );

        case OTF2_LOCAL_DEF_STRING:
            return otf2_def_reader_read_string( reader );

        case OTF2_LOCAL_DEF_ATTRIBUTE:
            return otf2_def_reader_read_attribute( reader );

        case OTF2_LOCAL_DEF_SYSTEM_TREE_NODE:
            return otf2_def_reader_read_system_tree_node( reader );

        case OTF2_LOCAL_DEF_LOCATION_GROUP:
            return otf2_def_reader_read_location_group( reader );

        case OTF2_LOCAL_DEF_LOCATION:
            return otf2_def_reader_read_location( reader );

        case OTF2_LOCAL_DEF_REGION:
            return otf2_def_reader_read_region( reader );

        case OTF2_LOCAL_DEF_CALLSITE:
            return otf2_def_reader_read_callsite( reader );

        case OTF2_LOCAL_DEF_CALLPATH:
            return otf2_def_reader_read_callpath( reader );

        case OTF2_LOCAL_DEF_GROUP:
            return otf2_def_reader_read_group( reader );

        case OTF2_LOCAL_DEF_METRIC_MEMBER:
            return otf2_def_reader_read_metric_member( reader );

        case OTF2_LOCAL_DEF_METRIC_CLASS:
            return otf2_def_reader_read_metric_class( reader );

        case OTF2_LOCAL_DEF_METRIC_INSTANCE:
            return otf2_def_reader_read_metric_instance( reader );

        case OTF2_LOCAL_DEF_COMM:
            return otf2_def_reader_read_comm( reader );

        case OTF2_LOCAL_DEF_PARAMETER:
            return otf2_def_reader_read_parameter( reader );

        case OTF2_LOCAL_DEF_RMA_WIN:
            return otf2_def_reader_read_rma_win( reader );

        case OTF2_LOCAL_DEF_METRIC_CLASS_RECORDER:
            return otf2_def_reader_read_metric_class_recorder( reader );

        case OTF2_LOCAL_DEF_SYSTEM_TREE_NODE_PROPERTY:
            return otf2_def_reader_read_system_tree_node_property( reader );

        case OTF2_LOCAL_DEF_SYSTEM_TREE_NODE_DOMAIN:
            return otf2_def_reader_read_system_tree_node_domain( reader );

        case OTF2_LOCAL_DEF_LOCATION_GROUP_PROPERTY:
            return otf2_def_reader_read_location_group_property( reader );

        case OTF2_LOCAL_DEF_LOCATION_PROPERTY:
            return otf2_def_reader_read_location_property( reader );

        case OTF2_LOCAL_DEF_CART_DIMENSION:
            return otf2_def_reader_read_cart_dimension( reader );

        case OTF2_LOCAL_DEF_CART_TOPOLOGY:
            return otf2_def_reader_read_cart_topology( reader );

        case OTF2_LOCAL_DEF_CART_COORDINATE:
            return otf2_def_reader_read_cart_coordinate( reader );

        case OTF2_LOCAL_DEF_SOURCE_CODE_LOCATION:
            return otf2_def_reader_read_source_code_location( reader );

        case OTF2_LOCAL_DEF_CALLING_CONTEXT:
            return otf2_def_reader_read_calling_context( reader );

        case OTF2_LOCAL_DEF_CALLING_CONTEXT_PROPERTY:
            return otf2_def_reader_read_calling_context_property( reader );

        case OTF2_LOCAL_DEF_INTERRUPT_GENERATOR:
            return otf2_def_reader_read_interrupt_generator( reader );

        case OTF2_LOCAL_DEF_IO_FILE_PROPERTY:
            return otf2_def_reader_read_io_file_property( reader );

        case OTF2_LOCAL_DEF_IO_REGULAR_FILE:
            return otf2_def_reader_read_io_regular_file( reader );

        case OTF2_LOCAL_DEF_IO_DIRECTORY:
            return otf2_def_reader_read_io_directory( reader );

        case OTF2_LOCAL_DEF_IO_HANDLE:
            return otf2_def_reader_read_io_handle( reader );

        case OTF2_LOCAL_DEF_IO_PRE_CREATED_HANDLE_STATE:
            return otf2_def_reader_read_io_pre_created_handle_state( reader );

        case OTF2_LOCAL_DEF_CALLPATH_PARAMETER:
            return otf2_def_reader_read_callpath_parameter( reader );

        default:
            return otf2_def_reader_read_unknown( reader );
    }
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMappingTableCallback(
    OTF2_DefReaderCallbacks*            defReaderCallbacks,
    OTF2_DefReaderCallback_MappingTable mappingTableCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->mapping_table = mappingTableCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetClockOffsetCallback(
    OTF2_DefReaderCallbacks*           defReaderCallbacks,
    OTF2_DefReaderCallback_ClockOffset clockOffsetCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->clock_offset = clockOffsetCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetStringCallback(
    OTF2_DefReaderCallbacks*      defReaderCallbacks,
    OTF2_DefReaderCallback_String stringCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->string = stringCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetAttributeCallback(
    OTF2_DefReaderCallbacks*         defReaderCallbacks,
    OTF2_DefReaderCallback_Attribute attributeCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->attribute = attributeCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetSystemTreeNodeCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_SystemTreeNode systemTreeNodeCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->system_tree_node = systemTreeNodeCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetLocationGroupCallback(
    OTF2_DefReaderCallbacks*             defReaderCallbacks,
    OTF2_DefReaderCallback_LocationGroup locationGroupCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->location_group = locationGroupCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetLocationCallback(
    OTF2_DefReaderCallbacks*        defReaderCallbacks,
    OTF2_DefReaderCallback_Location locationCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->location = locationCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetRegionCallback(
    OTF2_DefReaderCallbacks*      defReaderCallbacks,
    OTF2_DefReaderCallback_Region regionCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->region = regionCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallsiteCallback(
    OTF2_DefReaderCallbacks*        defReaderCallbacks,
    OTF2_DefReaderCallback_Callsite callsiteCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->callsite = callsiteCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallpathCallback(
    OTF2_DefReaderCallbacks*        defReaderCallbacks,
    OTF2_DefReaderCallback_Callpath callpathCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->callpath = callpathCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetGroupCallback(
    OTF2_DefReaderCallbacks*     defReaderCallbacks,
    OTF2_DefReaderCallback_Group groupCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->group = groupCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMetricMemberCallback(
    OTF2_DefReaderCallbacks*            defReaderCallbacks,
    OTF2_DefReaderCallback_MetricMember metricMemberCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->metric_member = metricMemberCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMetricClassCallback(
    OTF2_DefReaderCallbacks*           defReaderCallbacks,
    OTF2_DefReaderCallback_MetricClass metricClassCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->metric_class = metricClassCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMetricInstanceCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_MetricInstance metricInstanceCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->metric_instance = metricInstanceCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCommCallback(
    OTF2_DefReaderCallbacks*    defReaderCallbacks,
    OTF2_DefReaderCallback_Comm commCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->comm = commCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetParameterCallback(
    OTF2_DefReaderCallbacks*         defReaderCallbacks,
    OTF2_DefReaderCallback_Parameter parameterCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->parameter = parameterCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetRmaWinCallback(
    OTF2_DefReaderCallbacks*      defReaderCallbacks,
    OTF2_DefReaderCallback_RmaWin rmaWinCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->rma_win = rmaWinCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetMetricClassRecorderCallback(
    OTF2_DefReaderCallbacks*                   defReaderCallbacks,
    OTF2_DefReaderCallback_MetricClassRecorder metricClassRecorderCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->metric_class_recorder = metricClassRecorderCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetSystemTreeNodePropertyCallback(
    OTF2_DefReaderCallbacks*                      defReaderCallbacks,
    OTF2_DefReaderCallback_SystemTreeNodeProperty systemTreeNodePropertyCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->system_tree_node_property = systemTreeNodePropertyCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetSystemTreeNodeDomainCallback(
    OTF2_DefReaderCallbacks*                    defReaderCallbacks,
    OTF2_DefReaderCallback_SystemTreeNodeDomain systemTreeNodeDomainCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->system_tree_node_domain = systemTreeNodeDomainCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetLocationGroupPropertyCallback(
    OTF2_DefReaderCallbacks*                     defReaderCallbacks,
    OTF2_DefReaderCallback_LocationGroupProperty locationGroupPropertyCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->location_group_property = locationGroupPropertyCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetLocationPropertyCallback(
    OTF2_DefReaderCallbacks*                defReaderCallbacks,
    OTF2_DefReaderCallback_LocationProperty locationPropertyCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->location_property = locationPropertyCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCartDimensionCallback(
    OTF2_DefReaderCallbacks*             defReaderCallbacks,
    OTF2_DefReaderCallback_CartDimension cartDimensionCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->cart_dimension = cartDimensionCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCartTopologyCallback(
    OTF2_DefReaderCallbacks*            defReaderCallbacks,
    OTF2_DefReaderCallback_CartTopology cartTopologyCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->cart_topology = cartTopologyCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCartCoordinateCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_CartCoordinate cartCoordinateCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->cart_coordinate = cartCoordinateCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetSourceCodeLocationCallback(
    OTF2_DefReaderCallbacks*                  defReaderCallbacks,
    OTF2_DefReaderCallback_SourceCodeLocation sourceCodeLocationCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->source_code_location = sourceCodeLocationCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallingContextCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_CallingContext callingContextCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->calling_context = callingContextCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallingContextPropertyCallback(
    OTF2_DefReaderCallbacks*                      defReaderCallbacks,
    OTF2_DefReaderCallback_CallingContextProperty callingContextPropertyCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->calling_context_property = callingContextPropertyCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetInterruptGeneratorCallback(
    OTF2_DefReaderCallbacks*                  defReaderCallbacks,
    OTF2_DefReaderCallback_InterruptGenerator interruptGeneratorCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->interrupt_generator = interruptGeneratorCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoFilePropertyCallback(
    OTF2_DefReaderCallbacks*              defReaderCallbacks,
    OTF2_DefReaderCallback_IoFileProperty ioFilePropertyCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->io_file_property = ioFilePropertyCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoRegularFileCallback(
    OTF2_DefReaderCallbacks*             defReaderCallbacks,
    OTF2_DefReaderCallback_IoRegularFile ioRegularFileCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->io_regular_file = ioRegularFileCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoDirectoryCallback(
    OTF2_DefReaderCallbacks*           defReaderCallbacks,
    OTF2_DefReaderCallback_IoDirectory ioDirectoryCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->io_directory = ioDirectoryCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoHandleCallback(
    OTF2_DefReaderCallbacks*        defReaderCallbacks,
    OTF2_DefReaderCallback_IoHandle ioHandleCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->io_handle = ioHandleCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetIoPreCreatedHandleStateCallback(
    OTF2_DefReaderCallbacks*                       defReaderCallbacks,
    OTF2_DefReaderCallback_IoPreCreatedHandleState ioPreCreatedHandleStateCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->io_pre_created_handle_state = ioPreCreatedHandleStateCallback;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetCallpathParameterCallback(
    OTF2_DefReaderCallbacks*                 defReaderCallbacks,
    OTF2_DefReaderCallback_CallpathParameter callpathParameterCallback )
{
    if ( !defReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid defReaderCallbacks argument!" );
    }

    defReaderCallbacks->callpath_parameter = callpathParameterCallback;

    return OTF2_SUCCESS;
}
