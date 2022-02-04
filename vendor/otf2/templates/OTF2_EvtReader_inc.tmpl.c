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
 *  @source     templates/OTF2_EvtReader_inc.tmpl.c
 */


@otf2 for event in events:

static inline OTF2_ErrorCode
otf2_evt_reader_read_@@event.lower@@( OTF2_EvtReader* reader )
{
    UTILS_ASSERT( reader );
    UTILS_ASSERT( reader->archive );

    OTF2_@@event.name@@* record = &reader->current_event.record.@@event.lower@@;

    record->time = otf2_evt_reader_apply_clock_correction( reader, record->time );

    OTF2_ErrorCode ret;
    @otf2 if event is singleton_event:
    /* Event is singleton (just one attribute of compressed data type) */
    ret = OTF2_Buffer_GuaranteeCompressed( reader->buffer );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read @@event.name@@ record. Not enough memory in buffer" );
    }
    @otf2 else:
    uint64_t          record_data_length;
    /* Event contains separate record length information */
    ret = OTF2_Buffer_GuaranteeRecord( reader->buffer, &record_data_length );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read @@event.name@@ record. Not enough memory in buffer" );
    }
    uint8_t* record_end_pos;
    OTF2_Buffer_GetPosition( reader->buffer, &record_end_pos );
    record_end_pos += record_data_length;

    @otf2  if event.versions|length > 1:
    uint8_t* current_pos;
    @otf2  endif
    @otf2 endif

    @otf2 for v in event.versions:
    @otf2  if v > event.since:
    /*
     * Read record attributes added in version @@v.major@@.@@v.minor@@, if there is
     * space left in the record
     */
    OTF2_Buffer_GetPosition( reader->buffer, &current_pos );
    if ( current_pos < record_end_pos )
    {
    @otf2  endif
    @otf2  for attr in event.record_attributes_by_version[v]:
    @otf2   if attr.type.errorcheck_on_read:
    ret = OTF2_Buffer_Read@@attr.type.otf2@@( reader->buffer, &record->@@attr.lower@@ );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read @@attr.name@@ attribute of @@event.name@@ record. @@attr.type.errorcheck_on_read@@" );
    }
    @otf2   else:
    OTF2_Buffer_Read@@attr.type.otf2@@( reader->buffer, &record->@@attr.lower@@ );
    @otf2   endif
    @otf2   if attr.mapping_type:
    record->@@attr.lower@@ = otf2_evt_reader_map( reader, @@attr.mapping_type@@, record->@@attr.lower@@ );
    @otf2   elif attr.apply_clock_offset:
    record->@@attr.lower@@ = otf2_evt_reader_apply_clock_correction( reader, record->@@attr.lower@@ );
    @otf2   elif attr.array_attributes:
    @otf2    for array_attr in attr.array_attributes:
    record->@@array_attr.lower@@ = malloc( record->@@attr.lower@@ * sizeof( @@array_attr.type@@ ) );
    if ( record->@@attr.lower@@ > 0 && !record->@@array_attr.lower@@ )
    {
        @otf2 for array_attr in attr.array_attributes[:loop.index - 1]:
        free( record->@@array_attr.lower@@ );
        @otf2 endfor
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                            "Could not allocate memory for @@array_attr.name@@ array!" );
    }
    @otf2    endfor
    for ( @@attr.type@@ @@attr.lower@@_i = 0;
          @@attr.lower@@_i < record->@@attr.lower@@;
          @@attr.lower@@_i++ )
    {
        @otf2 for array_attr in attr.array_attributes:

        @otf2  if array_attr.type.errorcheck_on_read:
        ret = OTF2_Buffer_Read@@array_attr.type.otf2@@( reader->buffer,
                                                        record->@@array_attr.lower@@ + @@attr.lower@@_i );
        if ( OTF2_SUCCESS != ret )
        {
            @otf2 for array_attr in attr.array_attributes:
            free( record->@@array_attr.lower@@ );
            @otf2 endfor
            return UTILS_ERROR( ret, "Could not read @@array_attr.name@@ attribute of @@event.name@@ record. @@array_attr.type.errorcheck_on_read@@" );
        }
        @otf2  else:
        OTF2_Buffer_Read@@array_attr.type.otf2@@( reader->buffer,
                                                  record->@@array_attr.lower@@ + @@attr.lower@@_i );
        @otf2  endif
        @otf2  if array_attr.mapping_type:
        record->@@array_attr.lower@@[ @@attr.lower@@_i ] = otf2_evt_reader_map( reader, @@array_attr.mapping_type@@, record->@@array_attr.lower@@[ @@attr.lower@@_i ] );
        @otf2  elif array_attr.apply_clock_offset:
        record->@@attr.lower@@ = otf2_evt_reader_apply_clock_correction( reader, record->@@attr.lower@@ );
        @otf2  endif
        @otf2 endfor
    }
    @otf2   endif
    @otf2  endfor
    @otf2  if v > event.since:
    }
    else
    {
        otf2_event_@@event.lower@@_convert_pre_@@v.major@@_@@v.minor@@( record );
    }
    @otf2  endif

    @otf2 endfor
    reader->global_event_position++;
    reader->chunk_local_event_position++;

    @otf2 if event is not singleton_event:
    /*
     * Always jump to the announced end of the record. This way, we skip
     * future additions of attributes to this record.
     */
    ret = OTF2_Buffer_SetPosition( reader->buffer, record_end_pos );
    if ( OTF2_SUCCESS != ret )
    {
        @otf2 for attr in event.all_array_attributes:
        free( record->@@attr.lower@@ );
        @otf2 endfor
        return UTILS_ERROR( ret, "Could not read record of unknown type." );
    }

    @otf2 endif
    /*
     * If a local event reader is operated by a global event reader,
     * don't call our callback, don't clean up the attribute list and
     * don't free any attribute arrays. Otherwise the global event reader
     * will clean up the attribute list after he has triggered his callback.
     */
    if ( reader->operated )
    {
        return OTF2_SUCCESS;
    }

    OTF2_CallbackCode interrupt = OTF2_CALLBACK_SUCCESS;
    if ( reader->reader_callbacks.@@event.lower@@ )
    {
        interrupt = reader->reader_callbacks.@@event.lower@@( reader->location_id,
                                                              record->time,
                                                              reader->global_event_position,
                                                              reader->user_data,
                                                              &reader->attribute_list@@event.callargs(case='lower', argprefix='record->')@@ );
    }
    @otf2 if event.alternative_event:
    else if ( reader->reader_callbacks.@@event.alternative_event.lower@@ )
    {
        OTF2_@@event.alternative_event.name@@ @@event.alternative_event.lower@@_record;
        bool conversion_successful
            = otf2_event_@@event.lower@@_convert_to_@@event.alternative_event.lower@@( reader->archive,
                                                                                       record,
                                                                                       &reader->attribute_list,
                                                                                       &@@event.alternative_event.lower@@_record );
        if ( conversion_successful )
        {
            interrupt = reader->reader_callbacks.@@event.alternative_event.lower@@( reader->location_id,
                                                                                    record->time,
                                                                                    reader->global_event_position,
                                                                                    reader->user_data,
                                                                                    &reader->attribute_list@@event.alternative_event.callargs(case='lower', argprefix=event.alternative_event.lower + '_record.')@@ );
        }
    }
    @otf2 endif

    otf2_attribute_list_remove_all_attributes( &reader->attribute_list );

    @otf2 for attr in event.all_array_attributes:
    free( record->@@attr.lower@@ );
    @otf2 endfor

    return OTF2_CALLBACK_SUCCESS == interrupt
           ? OTF2_SUCCESS
           : OTF2_ERROR_INTERRUPTED_BY_CALLBACK;
}

@otf2 endfor

OTF2_ErrorCode
otf2_evt_reader_read( OTF2_EvtReader* reader )
{
    UTILS_ASSERT( reader );

    OTF2_ErrorCode  status = OTF2_ERROR_INVALID;
    OTF2_GenericEvent* event  = &reader->current_event;

    status = OTF2_Buffer_ReadTimeStamp( reader->buffer, &event->record.time );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status, "Read of timestamp failed!" );
    }

    /* Read record type */
    status = OTF2_Buffer_GuaranteeRead( reader->buffer, 1 );
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status, "Could not read record type." );
    }
    OTF2_Buffer_ReadUint8( reader->buffer, &event->type );

    switch ( event->type )
    {
        case OTF2_BUFFER_END_OF_CHUNK:
            reader->chunk_local_event_position = 0;
            free( reader->position_table );
            reader->position_table = NULL;
            free( reader->timestamp_table );
            reader->timestamp_table = NULL;
            status                  = OTF2_Buffer_ReadGetNextChunk( reader->buffer );
            if ( status != OTF2_SUCCESS )
            {
                return UTILS_ERROR( status, "Load of next chunk failed!" );
            }
            return otf2_evt_reader_read( reader );

        case OTF2_BUFFER_END_OF_FILE:
            return OTF2_ERROR_INDEX_OUT_OF_BOUNDS;

        case OTF2_ATTRIBUTE_LIST:
            status = otf2_attribute_list_read_from_buffer( &reader->attribute_list,
                                                           reader->buffer,
                                                           otf2_evt_reader_map,
                                                           reader );
            if ( status != OTF2_SUCCESS )
            {
                return UTILS_ERROR( status, "Read of additional attributes failed!" );
            }
            return otf2_evt_reader_read( reader );

        @otf2 for event in events:
        case OTF2_EVENT_@@event.upper@@:
            return otf2_evt_reader_read_@@event.lower@@( reader );

        @otf2 endfor
        default:
            return otf2_evt_reader_read_unknown( reader );
    }
}

@otf2 for event in events:

OTF2_ErrorCode
OTF2_EvtReaderCallbacks_Set@@event.name@@Callback(
    OTF2_EvtReaderCallbacks*              evtReaderCallbacks,
    OTF2_EvtReaderCallback_@@event.name@@ @@event.lname@@Callback )
{
    if ( !evtReaderCallbacks )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid evtReaderCallbacks argument!" );
    }

    evtReaderCallbacks->@@event.lower@@ = @@event.lname@@Callback;

    return OTF2_SUCCESS;
}

@otf2 endfor
