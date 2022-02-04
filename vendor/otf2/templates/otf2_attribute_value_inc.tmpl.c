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


/**
 *  @internal
 *
 *  @file
 *  @source     templates/otf2_attribute_value_inc.tmpl.c
 *
 *  @brief      Convenience functions to convert from and to OTF2 enum values.
 */

@otf2 for enum in enums:

OTF2_ErrorCode
OTF2_AttributeValue_Set@@enum.name@@( @@enum.type@@        enumValue,
                                      OTF2_Type*           type,
                                      OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    @otf2 with member_name = enum.type.c_orig[:-2]
    @otf2 set  type_name   = member_name|upper
    *type = OTF2_TYPE_@@type_name@@;
    value->@@member_name@@ = enumValue;
    @otf2 endwith

    return OTF2_SUCCESS;
}

@otf2 endfor
@otf2 for enum in enums:

OTF2_ErrorCode
OTF2_AttributeValue_Get@@enum.name@@( OTF2_Type            type,
                                      OTF2_AttributeValue  value,
                                      @@enum.type@@*       enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    @otf2 with member_name = enum.type.c_orig[:-2]
    @otf2 set  type_name = member_name|upper
    if ( type != OTF2_TYPE_@@type_name@@ )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum @@enum.type@@: %hhu",
                            type );
    }
    *enumValue = value.@@member_name@@;
    @otf2 endwith

    return OTF2_SUCCESS;
}

@otf2 endfor

static uint64_t
null_mapping( void*            mappingData,
              OTF2_MappingType mappingType,
              uint64_t         localId )
{
    return localId;
}


OTF2_ErrorCode
otf2_attribute_value_read_from_buffer( OTF2_AttributeValue* attributeValue,
                                       OTF2_Type            type,
                                       OTF2_Buffer*         buffer,
                                       OTF2_MappingCallback mappingCb,
                                       void*                mappingData )
{
    UTILS_ASSERT( buffer );
    UTILS_ASSERT( attributeValue );

    if ( !mappingCb )
    {
        mappingCb = null_mapping;
    }

    OTF2_ErrorCode status;

    /* Cast the container to the right type and read it from the trace */
    switch ( type )
    {
        @otf2 for entry in Type.entries if entry.type:
        case @@entry.name@@:
            @otf2 if entry.type.errorcheck_on_read
            status = OTF2_Buffer_Read@@entry.type.otf2@@( buffer, &attributeValue->@@entry.type.lower@@ );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read attribute. "
                                    "@@entry.type.errorcheck_on_read@@" );
            }
            @otf2 else:
            OTF2_Buffer_Read@@entry.type.otf2@@( buffer, &attributeValue->@@entry.type.lower@@ );
            @otf2 endif
            break;

        @otf2 endfor
        @otf2 for entry in MappingType.entries:
        @otf2 with def = entry.def
        case @@Type.prefix@@@@entry.suffix@@:
            /* @@Type.prefix@@@@entry.suffix@@ attributes reference a @@def.siblings|join(', or a ')@@ definition */
            status = OTF2_Buffer_Read@@def.self_attr.type.otf2@@( buffer,
                                                                  &attributeValue->@@def.ref_lname@@Ref );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read @@def.name@@ attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->@@def.ref_lname@@Ref = mappingCb( mappingData,
                                                              @@entry@@,
                                                              attributeValue->@@def.ref_lname@@Ref );
            break;

        @otf2 endwith
        @otf2 endfor
        default:
            /*
             * There can only be more definition references added in the future,
             * and definition references are guaranteed to be of compressable
             * size, thus we induce that the value is of compressed type
             */
            status = OTF2_Buffer_ReadUint64( buffer, &attributeValue->uint64 );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read attribute of unknown type. "
                                    "Invalid compression size." );
            }
            break;
    }

    return OTF2_SUCCESS;
}


void
otf2_attribute_value_write_to_buffer( OTF2_AttributeValue attributeValue,
                                      OTF2_Type           type,
                                      OTF2_Buffer*        buffer )
{
    UTILS_ASSERT( buffer );

    switch ( type )
    {
        @otf2 for entry in Type.entries if entry.type:
        case @@entry.name@@:
            OTF2_Buffer_Write@@entry.type.otf2@@( buffer,
                                                  attributeValue.@@entry.type.lower@@ );
            break;

        @otf2 endfor
        @otf2 for entry in MappingType.entries:
        @otf2 with def = entry.def
        case @@Type.prefix@@@@entry.suffix@@:
            /* @@Type.prefix@@@@entry.suffix@@ attributes reference a @@def.siblings|join(', or a ')@@ definition */
            OTF2_Buffer_Write@@def.self_attr.type.otf2@@( buffer, attributeValue.@@def.ref_lname@@Ref );
            break;

        @otf2 endwith
        @otf2 endfor
        default:
            /*
             * When getting an unknown type, we write it nevertheless as an
             * uint64, because we assume that future additions will always be
             * definition references and they are always of compressed types.
             */
            OTF2_Buffer_WriteUint64( buffer, attributeValue.uint64 );
            break;
    }
}
