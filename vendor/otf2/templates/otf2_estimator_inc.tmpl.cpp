/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

static size_t
get_attribute_list( OTF2_EventSizeEstimator* estimator,
                    const string&            input,
                    size_t&                  pos );

static void
handle_get( OTF2_EventSizeEstimator* estimator,
            const string&            input,
            size_t&                  pos )
{
    string event_name = get_token( input, pos );
    string key        = event_name;
    if ( pos != string::npos )
    {
        key += input.substr( pos );
    }
    size_t size = 0;

    if ( event_name == "" )
    {
        die( "missing event specification for get command" );
    }
    else if ( event_name == "DefChunkSize" )
    {
        size = OTF2_EventSizeEstimator_GetDefChunkSize( estimator );
    }
    else if ( event_name == "Timestamp" )
    {
        size = OTF2_EventSizeEstimator_GetSizeOfTimestamp( estimator );
    }
    else if ( event_name == "AttributeList" )
    {
        size = get_attribute_list( estimator, input, pos );
    }
    @otf2 for event in events :
    else if ( event_name == "@@event.name@@" )
    {
        @otf2 for attr in event.attributes if attr is array_attr:
        string @@attr.lower@@_arg = get_token( input, pos );
        if ( @@attr.lower@@_arg == "" )
        {
            die( "missing @@attr.name@@ parameter for @@event.name@@ event" );
        }
        @@attr.type@@ @@attr.lower@@ = strtol( @@attr.lower@@_arg.c_str() , NULL, 0 );
        @otf2 endfor

        size = OTF2_EventSizeEstimator_GetSizeOf@@event.name@@Event( estimator@@event.callargs(attributes=event.array_attributes, case='lower', resolve_array_attributes=False)@@ );
    }
    @otf2 endfor
    else
    {
        die( "unknown event '" << key << "'" );
    }

    cout << key << " " << size << endl;
}

size_t
get_attribute_list( OTF2_EventSizeEstimator* estimator,
                    const string&            input,
                    size_t&                  pos )
{
    OTF2_AttributeList* attribute_list = OTF2_AttributeList_New();
    if ( !attribute_list )
    {
        die( "Out of memory for AttributeList" );
    }

    OTF2_AttributeRef attribute = 0;
    for ( string attribute_type_arg = get_token( input, pos );
          attribute_type_arg != "";
          attribute_type_arg = get_token( input, pos ) )
    {
        OTF2_Type           type;
        OTF2_AttributeValue attribute_value;

        if ( attribute_type_arg == "NONE" )
        {
            continue;
        }
        @otf2 for entry in Type.entries if entry.type:
        else if ( attribute_type_arg == "@@entry.suffix@@" )
        {
            type = @@entry.name@@;
            attribute_value.@@entry.type.lower@@ = 0;
        }
        @otf2 endfor
        @otf2 for entry in MappingType.entries:
        @otf2 with def = entry.def
        else if ( attribute_type_arg == "@@entry.suffix@@" )
        {
            type = @@Type.prefix@@@@entry.suffix@@;
            attribute_value.@@entry.def.ref_lname@@Ref = 0;
        }
        @otf2 endwith
        @otf2 endfor
        else
        {
            die( "unknown attribute type: '" << attribute_type_arg << "'" );
        }

        OTF2_ErrorCode status = OTF2_AttributeList_AddAttribute(
            attribute_list,
            attribute++,
            type,
            attribute_value );
        if ( OTF2_SUCCESS != status )
        {
            die( "can't add attribute of type '" << attribute_type_arg << "'" );
        }
    }

    size_t size = OTF2_EventSizeEstimator_GetSizeOfAttributeList( estimator,
                                                                  attribute_list );

    OTF2_AttributeList_Delete( attribute_list );

    return size;
}

static void
handle_set( OTF2_EventSizeEstimator* estimator,
            const string&            input,
            size_t&                  pos )
{
    string def_name = get_token( input, pos );

    if ( def_name == "" )
    {
        die( "missing definition specification in set command" );
    }

    string value_str = get_token( input, pos );
    if ( value_str == "" )
    {
        die( "missing number of definitions in set command" );
    }

    uint64_t value = strtol( value_str.c_str() , NULL, 0 );
    @otf2 for entry in MappingType.entries :
    if ( def_name == "@@entry.def.ref_name@@" )
    {
        OTF2_EventSizeEstimator_SetNumberOf@@entry.def.ref_name@@Definitions( estimator,
                                                                              value );
        return;
    }
    @otf2 endfor

    die( "unknown definition '" << def_name << "'" );
}

static void
print_event_list( void )
{
    cout
    << "Timestamp" << endl
    << "AttributeList" << endl
@otf2 for event in events :
    << "@@event.name@@"
@otf2 for attr in event.attributes if attr is array_attr:
    << " <@@attr.name@@>"
@otf2 endfor
    << endl
@otf2 endfor
    ;
}

static void
print_definition_list( void )
{
    cout
@otf2 for entry in MappingType.entries :
    << "@@entry.def.ref_name@@" << endl
@otf2 endfor
    ;
}

static void
print_type_list( void )
{
    cout
    @otf2 for entry in Type.entries if entry.type:
    << "@@entry.suffix@@" << endl
    @otf2 endfor
    @otf2 for entry in MappingType.entries:
    << "@@entry.suffix@@" << endl
@otf2 endfor
    ;
}
