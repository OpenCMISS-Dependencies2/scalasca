/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_INTERNAL_ATTRIBUTE_VALUE_H
#define OTF2_INTERNAL_ATTRIBUTE_VALUE_H


/**
 *  @internal
 *
 *  @file
 *
 *  @brief      Internal AttributeValue functions.
 */


typedef uint64_t
( * OTF2_MappingCallback )( void*            mappingData,
                            OTF2_MappingType mappingType,
                            uint64_t         localId );


OTF2_ErrorCode
otf2_attribute_value_read_from_buffer( OTF2_AttributeValue* attributeValue,
                                       OTF2_Type            type,
                                       OTF2_Buffer*         buffer,
                                       OTF2_MappingCallback mappingCb,
                                       void*                mappingData );


void
otf2_attribute_value_write_to_buffer( OTF2_AttributeValue attributeValue,
                                      OTF2_Type           type,
                                      OTF2_Buffer*        buffer );


#endif /* OTF2_INTERNAL_ATTRIBUTE_VALUE_H */
