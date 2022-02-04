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


OTF2_ErrorCode
OTF2_AttributeValue_SetBoolean( OTF2_Boolean         enumValue,
                                OTF2_Type*           type,
                                OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetFileType( OTF2_FileType        enumValue,
                                 OTF2_Type*           type,
                                 OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetFileSubstrate( OTF2_FileSubstrate   enumValue,
                                      OTF2_Type*           type,
                                      OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetMappingType( OTF2_MappingType     enumValue,
                                    OTF2_Type*           type,
                                    OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetType( OTF2_Type            enumValue,
                             OTF2_Type*           type,
                             OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetParadigm( OTF2_Paradigm        enumValue,
                                 OTF2_Type*           type,
                                 OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetParadigmClass( OTF2_ParadigmClass   enumValue,
                                      OTF2_Type*           type,
                                      OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetParadigmProperty( OTF2_ParadigmProperty enumValue,
                                         OTF2_Type*            type,
                                         OTF2_AttributeValue*  value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetThumbnailType( OTF2_ThumbnailType   enumValue,
                                      OTF2_Type*           type,
                                      OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetSystemTreeDomain( OTF2_SystemTreeDomain enumValue,
                                         OTF2_Type*            type,
                                         OTF2_AttributeValue*  value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetLocationGroupType( OTF2_LocationGroupType enumValue,
                                          OTF2_Type*             type,
                                          OTF2_AttributeValue*   value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetLocationType( OTF2_LocationType    enumValue,
                                     OTF2_Type*           type,
                                     OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetRegionRole( OTF2_RegionRole      enumValue,
                                   OTF2_Type*           type,
                                   OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetRegionFlag( OTF2_RegionFlag      enumValue,
                                   OTF2_Type*           type,
                                   OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type         = OTF2_TYPE_UINT32;
    value->uint32 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetGroupType( OTF2_GroupType       enumValue,
                                  OTF2_Type*           type,
                                  OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetGroupFlag( OTF2_GroupFlag       enumValue,
                                  OTF2_Type*           type,
                                  OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type         = OTF2_TYPE_UINT32;
    value->uint32 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetBase( OTF2_Base            enumValue,
                             OTF2_Type*           type,
                             OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetMetricOccurrence( OTF2_MetricOccurrence enumValue,
                                         OTF2_Type*            type,
                                         OTF2_AttributeValue*  value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetMetricType( OTF2_MetricType      enumValue,
                                   OTF2_Type*           type,
                                   OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetMetricValueProperty( OTF2_MetricValueProperty enumValue,
                                            OTF2_Type*               type,
                                            OTF2_AttributeValue*     value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetMetricTiming( OTF2_MetricTiming    enumValue,
                                     OTF2_Type*           type,
                                     OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetMetricMode( OTF2_MetricMode      enumValue,
                                   OTF2_Type*           type,
                                   OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetMetricScope( OTF2_MetricScope     enumValue,
                                    OTF2_Type*           type,
                                    OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetRecorderKind( OTF2_RecorderKind    enumValue,
                                     OTF2_Type*           type,
                                     OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetParameterType( OTF2_ParameterType   enumValue,
                                      OTF2_Type*           type,
                                      OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetCartPeriodicity( OTF2_CartPeriodicity enumValue,
                                        OTF2_Type*           type,
                                        OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetInterruptGeneratorMode( OTF2_InterruptGeneratorMode enumValue,
                                               OTF2_Type*                  type,
                                               OTF2_AttributeValue*        value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetMeasurementMode( OTF2_MeasurementMode enumValue,
                                        OTF2_Type*           type,
                                        OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetCollectiveOp( OTF2_CollectiveOp    enumValue,
                                     OTF2_Type*           type,
                                     OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetRmaSyncType( OTF2_RmaSyncType     enumValue,
                                    OTF2_Type*           type,
                                    OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetRmaSyncLevel( OTF2_RmaSyncLevel    enumValue,
                                     OTF2_Type*           type,
                                     OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type         = OTF2_TYPE_UINT32;
    value->uint32 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetLockType( OTF2_LockType        enumValue,
                                 OTF2_Type*           type,
                                 OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetRmaAtomicType( OTF2_RmaAtomicType   enumValue,
                                      OTF2_Type*           type,
                                      OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoParadigmClass( OTF2_IoParadigmClass enumValue,
                                        OTF2_Type*           type,
                                        OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoParadigmFlag( OTF2_IoParadigmFlag  enumValue,
                                       OTF2_Type*           type,
                                       OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type         = OTF2_TYPE_UINT32;
    value->uint32 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoParadigmProperty( OTF2_IoParadigmProperty enumValue,
                                           OTF2_Type*              type,
                                           OTF2_AttributeValue*    value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoHandleFlag( OTF2_IoHandleFlag    enumValue,
                                     OTF2_Type*           type,
                                     OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type         = OTF2_TYPE_UINT32;
    value->uint32 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoAccessMode( OTF2_IoAccessMode    enumValue,
                                     OTF2_Type*           type,
                                     OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoCreationFlag( OTF2_IoCreationFlag  enumValue,
                                       OTF2_Type*           type,
                                       OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type         = OTF2_TYPE_UINT32;
    value->uint32 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoStatusFlag( OTF2_IoStatusFlag    enumValue,
                                     OTF2_Type*           type,
                                     OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type         = OTF2_TYPE_UINT32;
    value->uint32 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoSeekOption( OTF2_IoSeekOption    enumValue,
                                     OTF2_Type*           type,
                                     OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoOperationMode( OTF2_IoOperationMode enumValue,
                                        OTF2_Type*           type,
                                        OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type        = OTF2_TYPE_UINT8;
    value->uint8 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_SetIoOperationFlag( OTF2_IoOperationFlag enumValue,
                                        OTF2_Type*           type,
                                        OTF2_AttributeValue* value )
{
    if ( !type || !value )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    *type         = OTF2_TYPE_UINT32;
    value->uint32 = enumValue;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetBoolean( OTF2_Type           type,
                                OTF2_AttributeValue value,
                                OTF2_Boolean*       enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_Boolean: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetFileType( OTF2_Type           type,
                                 OTF2_AttributeValue value,
                                 OTF2_FileType*      enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_FileType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetFileSubstrate( OTF2_Type           type,
                                      OTF2_AttributeValue value,
                                      OTF2_FileSubstrate* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_FileSubstrate: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetMappingType( OTF2_Type           type,
                                    OTF2_AttributeValue value,
                                    OTF2_MappingType*   enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_MappingType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetType( OTF2_Type           type,
                             OTF2_AttributeValue value,
                             OTF2_Type*          enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_Type: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetParadigm( OTF2_Type           type,
                                 OTF2_AttributeValue value,
                                 OTF2_Paradigm*      enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_Paradigm: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetParadigmClass( OTF2_Type           type,
                                      OTF2_AttributeValue value,
                                      OTF2_ParadigmClass* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_ParadigmClass: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetParadigmProperty( OTF2_Type              type,
                                         OTF2_AttributeValue    value,
                                         OTF2_ParadigmProperty* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_ParadigmProperty: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetThumbnailType( OTF2_Type           type,
                                      OTF2_AttributeValue value,
                                      OTF2_ThumbnailType* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_ThumbnailType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetSystemTreeDomain( OTF2_Type              type,
                                         OTF2_AttributeValue    value,
                                         OTF2_SystemTreeDomain* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_SystemTreeDomain: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetLocationGroupType( OTF2_Type               type,
                                          OTF2_AttributeValue     value,
                                          OTF2_LocationGroupType* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_LocationGroupType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetLocationType( OTF2_Type           type,
                                     OTF2_AttributeValue value,
                                     OTF2_LocationType*  enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_LocationType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetRegionRole( OTF2_Type           type,
                                   OTF2_AttributeValue value,
                                   OTF2_RegionRole*    enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_RegionRole: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetRegionFlag( OTF2_Type           type,
                                   OTF2_AttributeValue value,
                                   OTF2_RegionFlag*    enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT32 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_RegionFlag: %hhu",
                            type );
    }
    *enumValue = value.uint32;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetGroupType( OTF2_Type           type,
                                  OTF2_AttributeValue value,
                                  OTF2_GroupType*     enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_GroupType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetGroupFlag( OTF2_Type           type,
                                  OTF2_AttributeValue value,
                                  OTF2_GroupFlag*     enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT32 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_GroupFlag: %hhu",
                            type );
    }
    *enumValue = value.uint32;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetBase( OTF2_Type           type,
                             OTF2_AttributeValue value,
                             OTF2_Base*          enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_Base: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetMetricOccurrence( OTF2_Type              type,
                                         OTF2_AttributeValue    value,
                                         OTF2_MetricOccurrence* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_MetricOccurrence: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetMetricType( OTF2_Type           type,
                                   OTF2_AttributeValue value,
                                   OTF2_MetricType*    enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_MetricType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetMetricValueProperty( OTF2_Type                 type,
                                            OTF2_AttributeValue       value,
                                            OTF2_MetricValueProperty* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_MetricValueProperty: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetMetricTiming( OTF2_Type           type,
                                     OTF2_AttributeValue value,
                                     OTF2_MetricTiming*  enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_MetricTiming: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetMetricMode( OTF2_Type           type,
                                   OTF2_AttributeValue value,
                                   OTF2_MetricMode*    enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_MetricMode: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetMetricScope( OTF2_Type           type,
                                    OTF2_AttributeValue value,
                                    OTF2_MetricScope*   enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_MetricScope: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetRecorderKind( OTF2_Type           type,
                                     OTF2_AttributeValue value,
                                     OTF2_RecorderKind*  enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_RecorderKind: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetParameterType( OTF2_Type           type,
                                      OTF2_AttributeValue value,
                                      OTF2_ParameterType* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_ParameterType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetCartPeriodicity( OTF2_Type             type,
                                        OTF2_AttributeValue   value,
                                        OTF2_CartPeriodicity* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_CartPeriodicity: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetInterruptGeneratorMode( OTF2_Type                    type,
                                               OTF2_AttributeValue          value,
                                               OTF2_InterruptGeneratorMode* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_InterruptGeneratorMode: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetMeasurementMode( OTF2_Type             type,
                                        OTF2_AttributeValue   value,
                                        OTF2_MeasurementMode* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_MeasurementMode: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetCollectiveOp( OTF2_Type           type,
                                     OTF2_AttributeValue value,
                                     OTF2_CollectiveOp*  enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_CollectiveOp: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetRmaSyncType( OTF2_Type           type,
                                    OTF2_AttributeValue value,
                                    OTF2_RmaSyncType*   enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_RmaSyncType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetRmaSyncLevel( OTF2_Type           type,
                                     OTF2_AttributeValue value,
                                     OTF2_RmaSyncLevel*  enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT32 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_RmaSyncLevel: %hhu",
                            type );
    }
    *enumValue = value.uint32;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetLockType( OTF2_Type           type,
                                 OTF2_AttributeValue value,
                                 OTF2_LockType*      enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_LockType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetRmaAtomicType( OTF2_Type           type,
                                      OTF2_AttributeValue value,
                                      OTF2_RmaAtomicType* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_RmaAtomicType: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoParadigmClass( OTF2_Type             type,
                                        OTF2_AttributeValue   value,
                                        OTF2_IoParadigmClass* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoParadigmClass: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoParadigmFlag( OTF2_Type            type,
                                       OTF2_AttributeValue  value,
                                       OTF2_IoParadigmFlag* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT32 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoParadigmFlag: %hhu",
                            type );
    }
    *enumValue = value.uint32;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoParadigmProperty( OTF2_Type                type,
                                           OTF2_AttributeValue      value,
                                           OTF2_IoParadigmProperty* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoParadigmProperty: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoHandleFlag( OTF2_Type           type,
                                     OTF2_AttributeValue value,
                                     OTF2_IoHandleFlag*  enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT32 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoHandleFlag: %hhu",
                            type );
    }
    *enumValue = value.uint32;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoAccessMode( OTF2_Type           type,
                                     OTF2_AttributeValue value,
                                     OTF2_IoAccessMode*  enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoAccessMode: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoCreationFlag( OTF2_Type            type,
                                       OTF2_AttributeValue  value,
                                       OTF2_IoCreationFlag* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT32 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoCreationFlag: %hhu",
                            type );
    }
    *enumValue = value.uint32;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoStatusFlag( OTF2_Type           type,
                                     OTF2_AttributeValue value,
                                     OTF2_IoStatusFlag*  enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT32 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoStatusFlag: %hhu",
                            type );
    }
    *enumValue = value.uint32;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoSeekOption( OTF2_Type           type,
                                     OTF2_AttributeValue value,
                                     OTF2_IoSeekOption*  enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoSeekOption: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoOperationMode( OTF2_Type             type,
                                        OTF2_AttributeValue   value,
                                        OTF2_IoOperationMode* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT8 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoOperationMode: %hhu",
                            type );
    }
    *enumValue = value.uint8;

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_AttributeValue_GetIoOperationFlag( OTF2_Type             type,
                                        OTF2_AttributeValue   value,
                                        OTF2_IoOperationFlag* enumValue )
{
    if ( !enumValue )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments." );
    }

    if ( type != OTF2_TYPE_UINT32 )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ATTRIBUTE_TYPE,
                            "Invalid type for enum OTF2_IoOperationFlag: %hhu",
                            type );
    }
    *enumValue = value.uint32;

    return OTF2_SUCCESS;
}


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
        case OTF2_TYPE_UINT8:
            OTF2_Buffer_ReadUint8( buffer, &attributeValue->uint8 );
            break;

        case OTF2_TYPE_UINT16:
            OTF2_Buffer_ReadUint16( buffer, &attributeValue->uint16 );
            break;

        case OTF2_TYPE_UINT32:
            status = OTF2_Buffer_ReadUint32( buffer, &attributeValue->uint32 );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read attribute. "
                                    "Invalid compression size." );
            }
            break;

        case OTF2_TYPE_UINT64:
            status = OTF2_Buffer_ReadUint64( buffer, &attributeValue->uint64 );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read attribute. "
                                    "Invalid compression size." );
            }
            break;

        case OTF2_TYPE_INT8:
            OTF2_Buffer_ReadInt8( buffer, &attributeValue->int8 );
            break;

        case OTF2_TYPE_INT16:
            OTF2_Buffer_ReadInt16( buffer, &attributeValue->int16 );
            break;

        case OTF2_TYPE_INT32:
            status = OTF2_Buffer_ReadInt32( buffer, &attributeValue->int32 );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read attribute. "
                                    "Invalid compression size." );
            }
            break;

        case OTF2_TYPE_INT64:
            status = OTF2_Buffer_ReadInt64( buffer, &attributeValue->int64 );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read attribute. "
                                    "Invalid compression size." );
            }
            break;

        case OTF2_TYPE_FLOAT:
            OTF2_Buffer_ReadFloat( buffer, &attributeValue->float32 );
            break;

        case OTF2_TYPE_DOUBLE:
            OTF2_Buffer_ReadDouble( buffer, &attributeValue->float64 );
            break;

        case OTF2_TYPE_STRING:
            /* OTF2_TYPE_STRING attributes reference a String definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->stringRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read String attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->stringRef = mappingCb( mappingData,
                                                   OTF2_MAPPING_STRING,
                                                   attributeValue->stringRef );
            break;

        case OTF2_TYPE_ATTRIBUTE:
            /* OTF2_TYPE_ATTRIBUTE attributes reference a Attribute definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->attributeRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read Attribute attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->attributeRef = mappingCb( mappingData,
                                                      OTF2_MAPPING_ATTRIBUTE,
                                                      attributeValue->attributeRef );
            break;

        case OTF2_TYPE_LOCATION:
            /* OTF2_TYPE_LOCATION attributes reference a Location definition */
            status = OTF2_Buffer_ReadUint64( buffer,
                                             &attributeValue->locationRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read Location attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->locationRef = mappingCb( mappingData,
                                                     OTF2_MAPPING_LOCATION,
                                                     attributeValue->locationRef );
            break;

        case OTF2_TYPE_REGION:
            /* OTF2_TYPE_REGION attributes reference a Region definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->regionRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read Region attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->regionRef = mappingCb( mappingData,
                                                   OTF2_MAPPING_REGION,
                                                   attributeValue->regionRef );
            break;

        case OTF2_TYPE_GROUP:
            /* OTF2_TYPE_GROUP attributes reference a Group definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->groupRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read Group attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->groupRef = mappingCb( mappingData,
                                                  OTF2_MAPPING_GROUP,
                                                  attributeValue->groupRef );
            break;

        case OTF2_TYPE_METRIC:
            /* OTF2_TYPE_METRIC attributes reference a MetricClass, or a MetricInstance definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->metricRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read MetricClass attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->metricRef = mappingCb( mappingData,
                                                   OTF2_MAPPING_METRIC,
                                                   attributeValue->metricRef );
            break;

        case OTF2_TYPE_COMM:
            /* OTF2_TYPE_COMM attributes reference a Comm definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->commRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read Comm attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->commRef = mappingCb( mappingData,
                                                 OTF2_MAPPING_COMM,
                                                 attributeValue->commRef );
            break;

        case OTF2_TYPE_PARAMETER:
            /* OTF2_TYPE_PARAMETER attributes reference a Parameter definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->parameterRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read Parameter attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->parameterRef = mappingCb( mappingData,
                                                      OTF2_MAPPING_PARAMETER,
                                                      attributeValue->parameterRef );
            break;

        case OTF2_TYPE_RMA_WIN:
            /* OTF2_TYPE_RMA_WIN attributes reference a RmaWin definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->rmaWinRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read RmaWin attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->rmaWinRef = mappingCb( mappingData,
                                                   OTF2_MAPPING_RMA_WIN,
                                                   attributeValue->rmaWinRef );
            break;

        case OTF2_TYPE_SOURCE_CODE_LOCATION:
            /* OTF2_TYPE_SOURCE_CODE_LOCATION attributes reference a SourceCodeLocation definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->sourceCodeLocationRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read SourceCodeLocation attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->sourceCodeLocationRef = mappingCb( mappingData,
                                                               OTF2_MAPPING_SOURCE_CODE_LOCATION,
                                                               attributeValue->sourceCodeLocationRef );
            break;

        case OTF2_TYPE_CALLING_CONTEXT:
            /* OTF2_TYPE_CALLING_CONTEXT attributes reference a CallingContext definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->callingContextRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read CallingContext attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->callingContextRef = mappingCb( mappingData,
                                                           OTF2_MAPPING_CALLING_CONTEXT,
                                                           attributeValue->callingContextRef );
            break;

        case OTF2_TYPE_INTERRUPT_GENERATOR:
            /* OTF2_TYPE_INTERRUPT_GENERATOR attributes reference a InterruptGenerator definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->interruptGeneratorRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read InterruptGenerator attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->interruptGeneratorRef = mappingCb( mappingData,
                                                               OTF2_MAPPING_INTERRUPT_GENERATOR,
                                                               attributeValue->interruptGeneratorRef );
            break;

        case OTF2_TYPE_IO_FILE:
            /* OTF2_TYPE_IO_FILE attributes reference a IoRegularFile, or a IoDirectory definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->ioFileRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read IoRegularFile attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->ioFileRef = mappingCb( mappingData,
                                                   OTF2_MAPPING_IO_FILE,
                                                   attributeValue->ioFileRef );
            break;

        case OTF2_TYPE_IO_HANDLE:
            /* OTF2_TYPE_IO_HANDLE attributes reference a IoHandle definition */
            status = OTF2_Buffer_ReadUint32( buffer,
                                             &attributeValue->ioHandleRef );
            if ( OTF2_SUCCESS != status )
            {
                return UTILS_ERROR( status,
                                    "Could not read IoHandle attribute. "
                                    "Invalid compression size." );
            }
            /* But we need to map them to the global definition */
            attributeValue->ioHandleRef = mappingCb( mappingData,
                                                     OTF2_MAPPING_IO_HANDLE,
                                                     attributeValue->ioHandleRef );
            break;

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
        case OTF2_TYPE_UINT8:
            OTF2_Buffer_WriteUint8( buffer,
                                    attributeValue.uint8 );
            break;

        case OTF2_TYPE_UINT16:
            OTF2_Buffer_WriteUint16( buffer,
                                     attributeValue.uint16 );
            break;

        case OTF2_TYPE_UINT32:
            OTF2_Buffer_WriteUint32( buffer,
                                     attributeValue.uint32 );
            break;

        case OTF2_TYPE_UINT64:
            OTF2_Buffer_WriteUint64( buffer,
                                     attributeValue.uint64 );
            break;

        case OTF2_TYPE_INT8:
            OTF2_Buffer_WriteInt8( buffer,
                                   attributeValue.int8 );
            break;

        case OTF2_TYPE_INT16:
            OTF2_Buffer_WriteInt16( buffer,
                                    attributeValue.int16 );
            break;

        case OTF2_TYPE_INT32:
            OTF2_Buffer_WriteInt32( buffer,
                                    attributeValue.int32 );
            break;

        case OTF2_TYPE_INT64:
            OTF2_Buffer_WriteInt64( buffer,
                                    attributeValue.int64 );
            break;

        case OTF2_TYPE_FLOAT:
            OTF2_Buffer_WriteFloat( buffer,
                                    attributeValue.float32 );
            break;

        case OTF2_TYPE_DOUBLE:
            OTF2_Buffer_WriteDouble( buffer,
                                     attributeValue.float64 );
            break;

        case OTF2_TYPE_STRING:
            /* OTF2_TYPE_STRING attributes reference a String definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.stringRef );
            break;

        case OTF2_TYPE_ATTRIBUTE:
            /* OTF2_TYPE_ATTRIBUTE attributes reference a Attribute definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.attributeRef );
            break;

        case OTF2_TYPE_LOCATION:
            /* OTF2_TYPE_LOCATION attributes reference a Location definition */
            OTF2_Buffer_WriteUint64( buffer, attributeValue.locationRef );
            break;

        case OTF2_TYPE_REGION:
            /* OTF2_TYPE_REGION attributes reference a Region definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.regionRef );
            break;

        case OTF2_TYPE_GROUP:
            /* OTF2_TYPE_GROUP attributes reference a Group definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.groupRef );
            break;

        case OTF2_TYPE_METRIC:
            /* OTF2_TYPE_METRIC attributes reference a MetricClass, or a MetricInstance definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.metricRef );
            break;

        case OTF2_TYPE_COMM:
            /* OTF2_TYPE_COMM attributes reference a Comm definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.commRef );
            break;

        case OTF2_TYPE_PARAMETER:
            /* OTF2_TYPE_PARAMETER attributes reference a Parameter definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.parameterRef );
            break;

        case OTF2_TYPE_RMA_WIN:
            /* OTF2_TYPE_RMA_WIN attributes reference a RmaWin definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.rmaWinRef );
            break;

        case OTF2_TYPE_SOURCE_CODE_LOCATION:
            /* OTF2_TYPE_SOURCE_CODE_LOCATION attributes reference a SourceCodeLocation definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.sourceCodeLocationRef );
            break;

        case OTF2_TYPE_CALLING_CONTEXT:
            /* OTF2_TYPE_CALLING_CONTEXT attributes reference a CallingContext definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.callingContextRef );
            break;

        case OTF2_TYPE_INTERRUPT_GENERATOR:
            /* OTF2_TYPE_INTERRUPT_GENERATOR attributes reference a InterruptGenerator definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.interruptGeneratorRef );
            break;

        case OTF2_TYPE_IO_FILE:
            /* OTF2_TYPE_IO_FILE attributes reference a IoRegularFile, or a IoDirectory definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.ioFileRef );
            break;

        case OTF2_TYPE_IO_HANDLE:
            /* OTF2_TYPE_IO_HANDLE attributes reference a IoHandle definition */
            OTF2_Buffer_WriteUint32( buffer, attributeValue.ioHandleRef );
            break;

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
