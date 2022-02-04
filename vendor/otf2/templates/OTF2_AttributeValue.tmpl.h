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


#ifndef OTF2_ATTRIBUTE_VALUE_H
#define OTF2_ATTRIBUTE_VALUE_H


/**
 *  @file
 *  @source     template/OTF2_AttributeValue.tmpl.h
 *
 *  @brief      Declares the @eref{OTF2_AttributeValue} and provides convenience
 *              functions to convert from and to OTF2 enum values.
 *
 */


#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>
#include <otf2/OTF2_Definitions.h>
#include <otf2/OTF2_Events.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Value container for an attributes.
 *
 * For definition references (@eref{OTF2_MappingType}) use the same data type as
 * the definition.
 */
typedef union OTF2_AttributeValue_union
{
    @otf2 for entry in Type.entries if entry.type:
    /** @brief Arbitrary value of type @@entry.type@@ */
    @@entry.type@@ @@entry.type.lower@@;
    @otf2 endfor
    @otf2 for entry in MappingType.entries:
    /**
     * @brief References a @eref{@@entry.def.siblings|join('}, or a @eref{')@@} definition
     * and will be mapped to the global definition if a mapping table of
     * type @eref{@@entry.name@@} is available.
     */
    @@entry.def.self_attr.type@@ @@entry.def.ref_lname@@Ref;
    @otf2 endfor
} OTF2_AttributeValue;

@otf2 for enum in enums:

/** @brief Set @eref{OTF2_Type} and @eref{OTF2_AttributeValue} to the appropriate
 *  values for the given enum entry.  No value range checking done.
 *
 *  @param      enumValue The enum value to be converted.
 *  @param[out] type      Matching @eref{OTF2_Type} for the enum
 *                        @eref{@@enum.type@@}.
 *  @param[out] value     Matching @eref{OTF2_AttributeValue} for the
 *                        @a enumValue value.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             if either @a type or @a value are NULL pointer}
 *  @retend
 */
OTF2_ErrorCode
OTF2_AttributeValue_Set@@enum.name@@( @@enum.type@@        enumValue,
                                      OTF2_Type*           type,
                                      OTF2_AttributeValue* value );

@otf2 endfor
@otf2 for enum in enums:

/** @brief Converts a @eref{OTF2_Type} and @eref{OTF2_AttributeValue} pair to
 *  the appropriate value for the enum @eref{@@enum.type@@}.  No value range
 *  checking done.
 *
 *  @param      type      Given type.
 *  @param      value     Given value.
 *  @param[out] enumValue Converted enum value.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             if @a enumValue a NULL pointer}
 *    @retcode{OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
 *             if @a type does not match the base type of the enum @eref{@@enum.type@@}}
 *  @retend
 */
OTF2_ErrorCode
OTF2_AttributeValue_Get@@enum.name@@( OTF2_Type            type,
                                      OTF2_AttributeValue  value,
                                      @@enum.type@@*       enumValue );

@otf2 endfor

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_ATTRIBUTE_VALUE_H */
