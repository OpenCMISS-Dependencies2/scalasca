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

#include "otf2_attribute_value.h"
#include "otf2_attribute_list.h"
#include "otf2_id_map.h"

#include "OTF2_File.h"
#include "OTF2_Buffer.h"

/* ___ Macros _______________________________________________________________ */



static inline otf2_attribute**
otf2_attribute_list_find_id( const OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef         attribute );

void
otf2_attribute_list_init( OTF2_AttributeList* attributeList )
{
    if ( !attributeList )
    {
        return;
    }

    attributeList->capacity = 0;
    attributeList->head     = NULL;
    attributeList->tail     = &attributeList->head;
    attributeList->free     = NULL;
}


OTF2_AttributeList*
OTF2_AttributeList_New( void )
{
    OTF2_AttributeList* new_list = calloc( 1, sizeof( *new_list ) );
    if ( NULL == new_list )
    {
        UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                     "Could not allocate memory for attribute list handle!" );
        return NULL;
    }

    otf2_attribute_list_init( new_list );

    return new_list;
}


void
otf2_attribute_list_clear( OTF2_AttributeList* attributeList )
{
    if ( !attributeList )
    {
        return;
    }

    otf2_attribute* entry = attributeList->head;
    while ( entry )
    {
        otf2_attribute* next = entry->next;
        free( entry );
        entry = next;
    }

    entry = attributeList->free;
    while ( entry )
    {
        otf2_attribute* next = entry->next;
        free( entry );
        entry = next;
    }
}


OTF2_ErrorCode
OTF2_AttributeList_Delete( OTF2_AttributeList* attributeList )
{
    if ( attributeList == NULL )
    {
        return OTF2_SUCCESS;
    }

    otf2_attribute_list_clear( attributeList );

    free( attributeList );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeList_AddAttribute( OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef   attribute,
                                 OTF2_Type           type,
                                 OTF2_AttributeValue attributeValue )
{
    /* Validate arguments. */
    if ( attributeList == NULL )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid attribute list!" );
    }

    if ( otf2_attribute_list_find_id( attributeList, attribute ) )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "The passed attribute ID already exists!" );
    }

    otf2_attribute* new_entry;
    if ( attributeList->free )
    {
        new_entry           = attributeList->free;
        attributeList->free = new_entry->next;
    }
    else
    {
        new_entry = calloc( 1, sizeof( *new_entry ) );
        if ( new_entry == NULL )
        {
            return UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                                "Could not allocate memory for new attribute!" );
        }
    }

    new_entry->type_id      = type;
    new_entry->attribute_id = attribute;
    new_entry->value        = attributeValue;
    new_entry->next         = NULL;

    attributeList->capacity++;
    *attributeList->tail = new_entry;
    attributeList->tail  = &new_entry->next;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeList_RemoveAttribute( OTF2_AttributeList* attributeList,
                                    OTF2_AttributeRef   attribute )
{
    /* Validate arguments. */
    if ( attributeList == NULL )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid attribute list!" );
    }

    /* Check if requested attribute is in the list. */
    otf2_attribute** entry
        = otf2_attribute_list_find_id( attributeList, attribute );
    if ( !entry )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "The passed attribute ID does not exists!" );
    }

    otf2_attribute* remove_entry = *entry;
    *entry = ( *entry )->next;
    if ( remove_entry->next == NULL )
    {
        /* adjust tail, if the removed one was the last */
        attributeList->tail = entry;
    }

    /* keep the removed one in the free list */
    remove_entry->next  = attributeList->free;
    attributeList->free = remove_entry;
    attributeList->capacity--;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeList_RemoveAllAttributes( OTF2_AttributeList* attributeList )
{
    /* Validate arguments. */
    if ( attributeList == NULL )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid attribute list!" );
    }

    return otf2_attribute_list_remove_all_attributes( attributeList );
}


bool
OTF2_AttributeList_TestAttributeByID( const OTF2_AttributeList* attributeList,
                                      OTF2_AttributeRef         attribute )
{
    if ( attributeList == NULL )
    {
        return false;
    }

    return NULL != otf2_attribute_list_find_id( attributeList, attribute );
}


OTF2_ErrorCode
OTF2_AttributeList_GetAttributeByID( const OTF2_AttributeList* attributeList,
                                     OTF2_AttributeRef         attribute,
                                     OTF2_Type*                type,
                                     OTF2_AttributeValue*      attributeValue )
{
    /* Validate arguments. */
    if ( attributeList == NULL || !type || !attributeValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid attribute list!" );
    }

    otf2_attribute** entry = otf2_attribute_list_find_id( attributeList,
                                                          attribute );
    if ( !entry )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "The passed attribute ID does not exist!" );
    }

    *type           = ( *entry )->type_id;
    *attributeValue = ( *entry )->value;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeList_GetAttributeByIndex( const OTF2_AttributeList* attributeList,
                                        uint32_t                  index,
                                        OTF2_AttributeRef*        attribute,
                                        OTF2_Type*                type,
                                        OTF2_AttributeValue*      attributeValue )
{
    /* Validate arguments. */
    if ( attributeList == NULL || !attribute || !type || !attributeValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid attribute list!" );
    }

    if ( index >= attributeList->capacity )
    {
        return UTILS_ERROR( OTF2_ERROR_INDEX_OUT_OF_BOUNDS,
                            "The passed index is out of range!" );
    }

    otf2_attribute* entry = attributeList->head;

    for ( uint32_t i = 0; i < index; i++ )
    {
        entry = entry->next;
    }

    *attribute      = entry->attribute_id;
    *type           = entry->type_id;
    *attributeValue = entry->value;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeList_PopAttribute( OTF2_AttributeList*  attributeList,
                                 OTF2_AttributeRef*   attribute,
                                 OTF2_Type*           type,
                                 OTF2_AttributeValue* attributeValue )
{
    /* Validate arguments. */
    if ( !attributeList || !attribute || !type || !attributeValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid attribute list!" );
    }

    if ( attributeList->capacity == 0 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Attribute list is empty!" );
    }

    otf2_attribute* entry = attributeList->head;

    *attribute      = entry->attribute_id;
    *type           = entry->type_id;
    *attributeValue = entry->value;

    attributeList->head = entry->next;
    if ( attributeList->head == NULL )
    {
        attributeList->tail = &attributeList->head;
    }
    entry->next         = attributeList->free;
    attributeList->free = entry;
    attributeList->capacity--;

    return OTF2_SUCCESS;
}


uint32_t
OTF2_AttributeList_GetNumberOfElements( const OTF2_AttributeList* attributeList )
{
    /* Validate arguments. */
    if ( attributeList == NULL )
    {
        UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                     "This is no valid attribute list!" );
        return 0;
    }

    return attributeList->capacity;
}


/** @brief Test the existence of an attribute ID in the list.
 *
 *  Returns the first entry in the attribute list and removes it from the list.
 *
 *  @param attributeList    Attribute list handle.
 *  @param attribute        Pointer to the returned ID of the attribute.
 *
 *  @return                 Returns a non-zero value if the ID exists in the
 *                          list, zero otherwise.
 */
otf2_attribute**
otf2_attribute_list_find_id( const OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef         attribute )
{
    UTILS_ASSERT( attributeList );

    otf2_attribute** entry = ( otf2_attribute** )&attributeList->head;

    while ( *entry )
    {
        if ( ( *entry )->attribute_id == attribute )
        {
            return entry;
        }
        entry = &( *entry )->next;
    }

    return NULL;
}


OTF2_ErrorCode
otf2_attribute_list_read_from_buffer( OTF2_AttributeList*  attributeList,
                                      OTF2_Buffer*         buffer,
                                      OTF2_MappingCallback mappingCb,
                                      void*                mappingData )
{
    UTILS_ASSERT( attributeList );
    UTILS_ASSERT( buffer );

    OTF2_ErrorCode status;

    /* Get record length and test memory availability */
    uint64_t record_data_length;
    status = OTF2_Buffer_GuaranteeRecord( buffer, &record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status,
                            "Could not read attribute list. "
                            "Not enough memory in buffer." );
    }
    uint8_t* record_start_pos;
    OTF2_Buffer_GetPosition( buffer, &record_start_pos );

    /* Get how many attributes are in the list */
    uint32_t number_of_attributes;
    status = OTF2_Buffer_ReadUint32( buffer, &number_of_attributes );
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status,
                            "Could not read number of attributes in list. "
                            "Invalid compression size." );
    }

    /* Check if number of attributes is consistent with record length */
    if ( record_data_length > number_of_attributes * OTF2_MAXIMUM_ATTRIBUTE_LIST_ENTRY_SIZE + 5 )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Could not read attribute list. "
                            "Record length and number of attributes in list are not consistent." );
    }

    for ( uint32_t i = 0; i < number_of_attributes; i++ )
    {
        uint32_t            attribute_id;
        OTF2_Type           type;
        OTF2_AttributeValue value;

        /* Read attribute id from trace */
        status = OTF2_Buffer_ReadUint32( buffer, &attribute_id );
        if ( OTF2_SUCCESS != status )
        {
            return UTILS_ERROR( status,
                                "Could not read attribute from list. "
                                "Invalid compression size." );
        }
        attribute_id = mappingCb( mappingData, OTF2_MAPPING_ATTRIBUTE, attribute_id );

        /* Read the type from trace */
        OTF2_Buffer_ReadUint8( buffer, &type );

        /* Read the attribute value. */
        status = otf2_attribute_value_read_from_buffer( &value,
                                                        type,
                                                        buffer,
                                                        mappingCb,
                                                        mappingData );
        if ( OTF2_SUCCESS != status )
        {
            return UTILS_ERROR( status,
                                "Could not read attribute value." );
        }

        /* Store the new value in the attribute list. */
        status = OTF2_AttributeList_AddAttribute( attributeList,
                                                  attribute_id,
                                                  type,
                                                  value );
        if ( OTF2_SUCCESS != status )
        {
            return UTILS_ERROR( status,
                                "Could not add attribute to attribute list." );
        }
    }

    /*
     * Always jump to the announced end of the record.
     */
    status = OTF2_Buffer_SetPosition( buffer, record_start_pos + record_data_length );
    if ( OTF2_SUCCESS != status )
    {
        return UTILS_ERROR( status, "Could not read attribute list." );
    }

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
otf2_attribute_list_write_to_buffer( OTF2_AttributeList* attributeList,
                                     OTF2_Buffer*        buffer )
{
    /* Write no attribute list to the buffer, if there is no one. */
    if ( attributeList == NULL || attributeList->capacity == 0 )
    {
        return OTF2_SUCCESS;
    }

    UTILS_ASSERT( buffer );

    /* Write the attribute lists record type id to the trace */
    OTF2_Buffer_WriteUint8( buffer, OTF2_ATTRIBUTE_LIST );

    /* Write initial record length */
    uint32_t data_size_of_attr_list = otf2_attribute_list_get_data_size( attributeList );
    OTF2_Buffer_WriteInitialRecordLength( buffer, data_size_of_attr_list );

    /* Store how many items are in the list to the trace */
    OTF2_Buffer_WriteUint32( buffer, attributeList->capacity );

    for ( otf2_attribute* attr = attributeList->head; attr; attr = attr->next )
    {
        /* Write attribute id to the trace */
        OTF2_Buffer_WriteUint32( buffer, attr->attribute_id );

        /* Write the type to the trace */
        OTF2_Buffer_WriteUint8( buffer, attr->type_id );

        /* Write the value to the trace */
        otf2_attribute_value_write_to_buffer( attr->value,
                                              attr->type_id,
                                              buffer );
    }

    otf2_attribute_list_remove_all_attributes( attributeList );

    /* Write real record length */
    return OTF2_Buffer_WriteFinalRecordLength( buffer, data_size_of_attr_list );
}


/* Include generated convenient helper functions */
#include "otf2_attribute_list_inc.c"
