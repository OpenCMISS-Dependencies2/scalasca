/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
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


#ifndef OTF2_ATTRIBUTE_LIST_H
#define OTF2_ATTRIBUTE_LIST_H


/**
 *  @file
 *  @source     template/OTF2_AttributeList.tmpl.h
 *
 *  @brief      This layer enables dynamic appending of arbitrary attributes
 *              to any type of event record.
 *
 *  @addtogroup usage OTF2 usage examples
 *  @{
 *
 *  @defgroup usage_attribute_list How to use the attribute list for writing \
 *                                 additional attributes to event records
 *  @{
 *
 *  First create an attribute list handle.
 *  @code
 *  OTF2_AttributeList attribute_list = OTF2_AttributeList_New();
 *  @endcode
 *
 *  To write your additional attribute to an event record add your attributes to
 *  an empty attribute list right before you call the routine to write the
 *  event.
 *  @code
 *  OTF2_AttributeValue attr_value;
 *  attr_value.uint32 = attribute_value;
 *  OTF2_AttributeList_AddAttribute( attribute_list, attribute_id, OTF2_TYPE_UINT32, attr_value );
 *  ...
 *  @endcode
 *
 *  Then call the routine to write the event and pass the attribute list. The
 *  additional attributes are added to the event record and will be appended
 *  when reading the event later on.
 *  Please note: All attributes in the list will be added to event record. So
 *  make sure that there are only those attributes in the attribute list that
 *  you actually like to write.
 *  Please note: After writing the event record all attributes are removed from
 *  the attribute list. So the attribute list is empty again. If you want to
 *  write identical attributes to multiple events you have to add them each time
 *  new.
 *  @code
 *  OTF2_EvtWriter_WriteEnter( ..., attribute_list, ... );
 *  @endcode
 *
 *  @}
 *
 *  @}
 */


#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>
#include <otf2/OTF2_AttributeValue.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Attribute list handle. */
typedef struct OTF2_AttributeList_struct OTF2_AttributeList;


/** @brief Create a new attribute list handle.
 *
 *  @return                 Returns a handle to the attribute list if
 *                          successful, NULL otherwise.
 */
OTF2_AttributeList*
OTF2_AttributeList_New( void );


/** @brief Delete an attribute list handle.
 *
 *  Deletes an attribute list handle and releases all associated resources.
 *
 *  @param attributeList    Attribute list handle.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_Delete( OTF2_AttributeList* attributeList );


/** @brief Add an attribute to an attribute list.
 *
 *  Adds an attribute to an attribute list. If the attribute already exists,
 *  it fails and returns an error.
 *
 *  @param attributeList    Attribute list handle.
 *  @param attribute        Reference to attribute definition.
 *  @param type             Type of the attribute.
 *  @param attributeValue   Value of the attribute.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_AddAttribute( OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef   attribute,
                                 OTF2_Type           type,
                                 OTF2_AttributeValue attributeValue );


@otf2 for entry in Type.entries if entry.type:
@otf2 with arg_name = entry.type.lower + 'Value'

/** @brief Add an @@entry.name@@ attribute to an attribute list.
 *
 *  Convenient function around @a OTF2_AttributeList_AddAttribute.
 *  @@doxygenparams([('attributeList', 'Attribute list handle.'),
                     ('attribute',     'Reference to @eref{Attribute} definition.'),
                     (arg_name,        'Value of the attribute.')])@@
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_Add@@entry.type.otf2@@( OTF2_AttributeList*  attributeList,
                                           OTF2_AttributeRef    attribute,
                                           @@entry.type@@       @@arg_name@@ );

@otf2 endwith
@otf2 endfor
@otf2 for entry in MappingType.entries:
@otf2 with def = entry.def
@otf2 with arg_name = def.ref_lname + 'Ref'

/** @brief Add an @@Type.prefix@@@@entry.suffix@@ attribute to an attribute list.
 *
 *  Convenience function around @a OTF2_AttributeList_AddAttribute.
 *  @@doxygenparams([('attributeList', 'Attribute list handle.'),
                     ('attribute',     'Reference to @eref{Attribute} definition.'),
                     (arg_name,        'Reference to @eref{%s} definition.' % def.ref_name)])@@
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_Add@@entry.def.ref_name@@Ref( OTF2_AttributeList*    attributeList,
                                                 OTF2_AttributeRef      attribute,
                                                 @@def.self_attr.type@@ @@arg_name@@ );

@otf2 endwith
@otf2 endwith
@otf2 endfor

/** @brief Remove an attribute from an attribute list.
 *
 *  @param attributeList    Attribute list handle.
 *  @param attribute        Reference to Attribute definition.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_RemoveAttribute( OTF2_AttributeList* attributeList,
                                    OTF2_AttributeRef   attribute );


/** @brief Remove all attributes from an attribute list.
 *
 *  @param attributeList    Attribute list handle.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_RemoveAllAttributes( OTF2_AttributeList* attributeList );


/** @brief Test if an attribute is in the attribute list.
 *
 *  @param attributeList    Attribute list handle.
 *  @param attribute        Reference to Attribute definition.
 *
 *  @return                 True if the id is in the list, else false.
 */
bool
OTF2_AttributeList_TestAttributeByID( const OTF2_AttributeList* attributeList,
                                      OTF2_AttributeRef         attribute );


/** @brief Get an attribute from an attribute list by attribute ID.
 *
 *  @param attributeList       Attribute list handle.
 *  @param attribute           Reference to Attribute definition.
 *  @param[out] type           Returned type of the attribute.
 *  @param[out] attributeValue Returned value of the attribute.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_GetAttributeByID( const OTF2_AttributeList* attributeList,
                                     OTF2_AttributeRef         attribute,
                                     OTF2_Type*                type,
                                     OTF2_AttributeValue*      attributeValue );

@otf2 for entry in Type.entries if entry.type:

/** @brief Get an @@entry.name@@ attribute from an attribute list by attribute ID.
 *
 *  Convenient function around @a OTF2_AttributeList_GetAttributeByID.
 *
 *  @param attributeList  Attribute list handle.
 *  @param attribute      Reference to Attribute definition.
 *
 *  @param[out] @@entry.type.lower@@Value  Returned value of the attribute.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_Get@@entry.type.otf2@@( const OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef         attribute,
                                           @@entry.type@@*           @@entry.type.lower@@Value );

@otf2 endfor
@otf2 for entry in MappingType.entries:
@otf2 with def = entry.def

/** @brief Get an @@Type.prefix@@@@entry.suffix@@ attribute from an attribute list by attribute ID.
 *
 *  Convenient function around @a OTF2_AttributeList_GetAttributeByID.
 *
 *  @param attributeList  Attribute list handle.
 *  @param attribute      Reference to attribute definition.
 *
 *  @param[out] @@def.ref_lname@@Ref Returned @@def.ref_lname@@ value.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_Get@@def.ref_name@@Ref( const OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef         attribute,
                                           @@def.self_attr.type@@*   @@def.ref_lname@@Ref );

@otf2 endwith
@otf2 endfor

/** @brief Get an attribute from an attribute list by attribute index.
 *
 *  @param attributeList       Attribute list handle.
 *  @param index               Position of the attribute in the attribute list.
 *  @param[out] attribute      Returned attribute reference.
 *  @param[out] type           Returned type of the attribute.
 *  @param[out] attributeValue Returned value of the attribute.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_GetAttributeByIndex( const OTF2_AttributeList* attributeList,
                                        uint32_t                  index,
                                        OTF2_AttributeRef*        attribute,
                                        OTF2_Type*                type,
                                        OTF2_AttributeValue*      attributeValue );


/** @brief Get first attribute from an attribute list and remove it.
 *
 *  Returns the first entry in the attribute list and removes it from the list.
 *
 *  @param attributeList       Attribute list handle.
 *  @param[out] attribute      Returned attribute reference.
 *  @param[out] type           Returned type of the attribute.
 *  @param[out] attributeValue Returned value of the attribute.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_AttributeList_PopAttribute( OTF2_AttributeList*  attributeList,
                                 OTF2_AttributeRef*   attribute,
                                 OTF2_Type*           type,
                                 OTF2_AttributeValue* attributeValue );


/** @brief Get the number of entries in an attribute list.
 *
 *  @param attributeList    Attribute list handle.
 *
 *  @return                 Returns the number of elements in the list. Returns
 *                          zero if the list does not exist.
 */
uint32_t
OTF2_AttributeList_GetNumberOfElements( const OTF2_AttributeList* attributeList );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_ATTRIBUTE_LIST_H */
