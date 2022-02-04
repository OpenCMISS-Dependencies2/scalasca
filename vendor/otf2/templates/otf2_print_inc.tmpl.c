/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


struct otf2_print_defs
{
    otf2_hash_table* paradigms;
    @otf2 for def in defs|global_defs|referable_defs|first_born_defs:
    otf2_hash_table* @@def.ref_lower@@s;
    @otf2 endfor
};


static void
otf2_print_def_create_hash_tables( struct otf2_print_defs* defs )
{
    defs->paradigms = otf2_hash_table_create_size(
        16,
        otf2_hash_table_hash_uint8,
        otf2_hash_table_compare_uint8 );
    @otf2 for def in defs|global_defs|referable_defs|first_born_defs:
    defs->@@def.ref_lower@@s = otf2_hash_table_create_size(
        1024*1024,
        otf2_hash_table_hash_uint64,
        otf2_hash_table_compare_uint64 );
    @otf2 endfor
}


static void
otf2_print_def_destroy_hash_tables( struct otf2_print_defs* defs )
{
    otf2_hash_table_free_all( defs->paradigms, otf2_hash_table_delete_none, free );
    @otf2 for def in defs|global_defs|referable_defs|first_born_defs:
    otf2_hash_table_free_all( defs->@@def.ref_lower@@s, otf2_hash_table_delete_none, free );
    @otf2 endfor
}


const char*
otf2_print_get_attribute_value( struct otf2_print_defs* defs,
                                OTF2_Type               type,
                                OTF2_AttributeValue     value,
                                bool                    raw )
{
    char* value_buffer = otf2_print_get_buffer( BUFFER_SIZE );

    switch ( type )
    {
        @otf2 for entry in Type.entries if entry.type:
        case @@entry.name@@:
            snprintf( value_buffer, BUFFER_SIZE,
                      "%" PRI@@entry.type.otf2@@, value.@@entry.type.lower@@ );
            break;

        @otf2 endfor
        @otf2 for entry in MappingType.entries:
        @otf2  with def = entry.def
        case @@Type.prefix@@@@entry.suffix@@:
            @otf2 with defsize = 'def'
            @otf2  if def.self_attr.type.c_orig == uint64_t:
            @otf2   set defsize = 'def64'
            @otf2  endif
            value_buffer = ( char* )( raw ? otf2_print_get_@@defsize@@_raw_name : otf2_print_get_@@defsize@@_name )( defs->@@def.ref_lower@@s, value.@@def.ref_lname@@Ref );
            @otf2 endwith
            break;

        @otf2  endwith
        @otf2 endfor
        default:
            snprintf( value_buffer, BUFFER_SIZE,
                      "%" PRIu64, value.uint64 );
            break;
    }

    return value_buffer;
}

@otf2 for def in defs|global_defs:

static OTF2_CallbackCode
print_global_def_@@def.lower@@( void* userData @@def.funcargs()@@ );
@otf2 if def is not global_only_def:
#if 0
{
    struct otf2_print_data* data = userData;
    struct otf2_print_defs* defs = data->defs;

    @otf2 if def is referable_def
    @otf2  with defsize = 'def'
    @otf2   if def.self_attr.type.c_orig == uint64_t:
    @otf2    set defsize = 'def64'
    @otf2   endif
    otf2_print_add_@@defsize@@_name( "@@def.ref_name@@",
                                     defs->@@def.ref_lower@@s,
                                     defs->strings,
                                     self,
                                     OTF2_UNDEFINED_STRING /* name attribute */,
                                     true );
    @otf2  endwith

    @otf2 endif
    /* Print definition if selected. */
    if ( !otf2_GLOBDEFS )
    {
        return OTF2_CALLBACK_SUCCESS;
    }

    printf( "%-*s "
            @otf2 if def is referable_def
            "%12" PRI@@def.self_attr.type.otf2@@
            @otf2 else:
            "%12s"
            @otf2 endif
            "  "
            @otf2 with attrs = def.attributes[1:]
            @otf2  if def is supplement_def:
            @otf2   set attrs = def.attributes
            @otf2  endif
            @otf2  for attr in attrs:
            @otf2   if attr is def_ref_attr or attr is enum_attr:
            "@@attr.name@@: %s, "
            @otf2   else
            "@@attr.name@@: %" PRI@@attr.type.otf2@@ ", "
            @otf2   endif
            @otf2  endfor
            @otf2 endwith
            "%s",
            otf2_DEF_COLUMN_WIDTH, "@@def.upper@@",
            @otf2 if def is referable_def
            self,
            @otf2 else:
            "",
            @otf2 endif
            @otf2 with attrs = def.attributes[1:]
            @otf2  if def is supplement_def:
            @otf2   set attrs = def.attributes
            @otf2  endif
            @otf2  for attr in attrs:
            @otf2   if attr is def_ref_attr:
            otf2_print_get_def_name( defs->@@attr.def_ref.ref_lower@@s, @@attr.name@@ ),
            @otf2   elif attr is enum_attr:
            @otf2    if attr.enum == Paradigm:
            otf2_print_get_paradigm_name( defs->paradigms, @@attr.name@@ ),
            @otf2    else
            otf2_print_get_@@attr.enum.lower@@( @@attr.name@@ ),
            @otf2    endif
            @otf2   else
            @@attr.name@@,
            @otf2   endif
            @otf2  endfor
            @otf2 endwith
            "\n" );

    return OTF2_CALLBACK_SUCCESS;
}
#endif
@otf2 endif

@otf2 endfor

static OTF2_GlobalDefReaderCallbacks*
otf2_print_create_global_def_callbacks( void )
{
    OTF2_GlobalDefReaderCallbacks* def_callbacks = OTF2_GlobalDefReaderCallbacks_New();
    check_pointer( def_callbacks, "Create global definition callback handle." );
    OTF2_GlobalDefReaderCallbacks_SetUnknownCallback( def_callbacks, print_global_def_unknown );
    @otf2 for def in defs|global_defs:
    OTF2_GlobalDefReaderCallbacks_Set@@def.name@@Callback( def_callbacks, print_global_def_@@def.lower@@ );
    @otf2 endfor

    return def_callbacks;
}

@otf2 for event in events:

static OTF2_CallbackCode
print_@@event.lower@@( OTF2_LocationRef    location,
                       OTF2_TimeStamp      time,
                       void*               userData,
                       OTF2_AttributeList* attributes@@event.funcargs()@@ );
#if 0
{
    if ( time < otf2_MINTIME || time > otf2_MAXTIME )
    {
        return OTF2_CALLBACK_SUCCESS;
    }

    struct otf2_print_data* data = userData;
    struct otf2_print_defs* defs = data->defs;

    printf( "%-*s %15" PRIu64 " %20s  "
            @otf2 for attr in event.attributes:
            @otf2  if attr is def_ref_attr or attr is enum_attr:
            "@@attr.name@@: %s, "
            @otf2  else
            "@@attr.name@@: %" PRI@@attr.type.otf2@@ ", "
            @otf2  endif
            @otf2 endfor
            "%s",
            otf2_EVENT_COLUMN_WIDTH, "@@event.upper@@",
            location,
            otf2_print_get_timestamp( data, time ),
            @otf2 for attr in event.attributes:
            @otf2  if attr is def_ref_attr:
            otf2_print_get_def_name( defs->@@attr.def_ref.ref_lower@@s, @@attr.name@@ ),
            @otf2  elif attr is enum_attr:
            @otf2   if attr.enum == Paradigm:
            otf2_print_get_paradigm_name( defs->paradigms, @@attr.name@@ ),
            @otf2   else
            otf2_print_get_@@attr.enum.lower@@( @@attr.name@@ ),
            @otf2   endif
            @otf2  else
            @@attr.name@@,
            @otf2  endif
            @otf2 endfor
            "\n" );

    otf2_print_attribute_list( data, attributes );

    return OTF2_CALLBACK_SUCCESS;
}
#endif

@otf2 endfor

@otf2 for snap in snaps:

@otf2  if snap is derived_snap:
static OTF2_CallbackCode
print_snap_@@snap.lower@@( OTF2_LocationRef    location,
                           OTF2_TimeStamp      snapTime,
                           void*               userData,
                           OTF2_AttributeList* attributes@@snap.funcargs()@@ )
{
    if ( snapTime < otf2_MINTIME || snapTime > otf2_MAXTIME )
    {
        return OTF2_CALLBACK_SUCCESS;
    }

    return print_@@snap.lower@@( location,
                                 @@snap.attributes[0].name@@,
                                 userData,
                                 attributes@@snap.callargs(attributes=snap.attributes[1:])@@ );
}
@otf2  else:
static OTF2_CallbackCode
print_snap_@@snap.lower@@( OTF2_LocationRef    location,
                           OTF2_TimeStamp      snapTime,
                           void*               userData,
                           OTF2_AttributeList* attributes@@snap.funcargs()@@ );
#if 0
{
    if ( snapTime < otf2_MINTIME || snapTime > otf2_MAXTIME )
    {
        return OTF2_CALLBACK_SUCCESS;
    }

    struct otf2_print_data* data = userData;
    struct otf2_print_defs* defs = data->defs;

    printf( "%-*s %15" PRIu64 " %20s  "
            @otf2 for attr in snap.attributes:
            @otf2  if attr is def_ref_attr or attr is enum_attr:
            "@@attr.name@@: %s, "
            @otf2  else
            "@@attr.name@@: %" PRI@@attr.type.otf2@@ ", "
            @otf2  endif
            @otf2 endfor
            "%s",
            otf2_EVENT_COLUMN_WIDTH, "@@snap.upper@@",
            location,
            otf2_print_get_timestamp( data, snapTime ),
            @otf2 for attr in snap.attributes:
            @otf2  if attr is def_ref_attr:
            otf2_print_get_def_name( defs->@@attr.def_ref.ref_lower@@s, @@attr.name@@ ),
            @otf2  elif attr is enum_attr:
            @otf2   if attr.enum == Paradigm:
            otf2_print_get_paradigm_name( defs->paradigms, @@attr.name@@ ),
            @otf2   else
            otf2_print_get_@@attr.enum.lower@@( @@attr.name@@ ),
            @otf2   endif
            @otf2  else
            @@attr.name@@,
            @otf2  endif
            @otf2 endfor
            "\n" );

    otf2_print_attribute_list( data, attributes );

    return OTF2_CALLBACK_SUCCESS;
}
#endif
@otf2  endif

@otf2 endfor

static OTF2_GlobalEvtReaderCallbacks*
otf2_print_create_global_evt_callbacks( void )
{
    OTF2_GlobalEvtReaderCallbacks* evt_callbacks = OTF2_GlobalEvtReaderCallbacks_New();
    check_pointer( evt_callbacks, "Create event reader callbacks." );
    OTF2_GlobalEvtReaderCallbacks_SetUnknownCallback( evt_callbacks, print_unknown );
    @otf2 for event in events:
    OTF2_GlobalEvtReaderCallbacks_Set@@event.name@@Callback( evt_callbacks, print_@@event.lower@@ );
    @otf2 endfor

    return evt_callbacks;
}

static OTF2_GlobalSnapReaderCallbacks*
otf2_print_create_global_snap_callbacks( void )
{
    OTF2_GlobalSnapReaderCallbacks* snap_callbacks = OTF2_GlobalSnapReaderCallbacks_New();
    check_pointer( snap_callbacks, "Create snapshot reader callbacks." );
    OTF2_GlobalSnapReaderCallbacks_SetUnknownCallback( snap_callbacks, print_unknown );
    @otf2 for snap in snaps:
    OTF2_GlobalSnapReaderCallbacks_Set@@snap.name@@Callback( snap_callbacks, print_snap_@@snap.lower@@ );
    @otf2 endfor

    return snap_callbacks;
}
