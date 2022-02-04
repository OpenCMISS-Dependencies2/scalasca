/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/**
 * \file cubew_location.c
   \brief Defines types and functions to deal with locations of running application.
 */
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include "cubew_location.h"
#include "cubew_location_group.h"
#include "cubew_meta_data_writer.h"
#include "cubew_vector.h"
#include "cubew_services.h"
#include "cubew_memory.h"

#define MEMORY_TRACING_PREFIX "[LOCATION]"
/**
 * Allocates memory for the structure "location"
 */
cube_location*
cube_location_create( cube_location* this )
{
    if ( this == NULL )
    {
        ALLOC( this, 1, cube_location, MEMORY_TRACING_PREFIX "Allocate location"  );
    }
    this->attr = NULL;
    /* construct attrs array */
    XALLOC( this->attr, 1, att_array, MEMORY_TRACING_PREFIX "Allocate cnode attribute"  );
    this->attr->size     = 0;
    this->attr->capacity = 0;

    return this;
}


/**
 * Fills the location with the information (rank, namem, id).
 *
 */
void
cube_location_init( cube_location*       this,
                    const char*          name,
                    int                  rank,
                    cube_location_type   type,
                    cube_location_group* parent )
{
    this->name   = cubew_strdup( name );
    this->rank   = rank;
    this->type   = type;
    this->parent = parent;
    if ( parent != NULL )
    {
        cube_location_add_child( this->parent, this );
    }
}


/**
 * Adds a child of this location.
 *
 */
void
cube_location_add_child( cube_location_group* parent,
                         cube_location*       thrd )
{
    ADD_NEXT( parent->child, thrd, cube_location*, MEMORY_TRACING_PREFIX "Add next location to same parent"  );
}

/**
 * Releases memory of the structure "location"
 */
void
cube_location_free( cube_location* this )
{
    if ( this != NULL )
    {
        CUBEW_FREE( this->name, MEMORY_TRACING_PREFIX "Release name of location"  );
        if ( this->attr )
        {
            int i = 0;
            for ( i = 0; i < this->attr->size; i++ )
            {
                CUBEW_FREE( ( this->attr->data[ i ] )->key, MEMORY_TRACING_PREFIX "Release key of a location attribute"  );
                CUBEW_FREE( ( this->attr->data[ i ] )->value, MEMORY_TRACING_PREFIX "Release value of a location attribute"  );
                CUBEW_FREE( this->attr->data[ i ], MEMORY_TRACING_PREFIX "Release location attribute"  );
            }
            CUBEW_FREE( this->attr->data, MEMORY_TRACING_PREFIX "Release vector of location attributes"  );
            CUBEW_FREE( this->attr, MEMORY_TRACING_PREFIX "Release location attributes"  );
        }
    }
    CUBEW_FREE( this, MEMORY_TRACING_PREFIX "Release location"  );
}


/**
 * Adds an attribute  "m" into cube "this"
 */
void
cube_location_add_attr( cube_location* this,
                        cmap*          m )
{
    ADD_NEXT( this->attr, m, cmap*, MEMORY_TRACING_PREFIX "Add a location attrubute"  );
}

/**
 * Creates and add in to cube "this" the attribute "key" with a vaue "value"
 */
void
cube_location_def_attr( cube_location* this,
                        const char*    key,
                        const char*    value )
{
    char* nkey   = cubew_strdup( key );
    char* nvalue = cubew_strdup( value );
    cmap*
    XALLOC( m,
            1,
            cmap,
            MEMORY_TRACING_PREFIX "Allocate location attribute"  );

    m->key   = nkey;
    m->value = nvalue;
    cube_location_add_attr( this, m );
}


cube_location_group*
cube_location_get_parent( cube_location* this )
{
    return this->parent;
}

char*
cube_location_get_name( cube_location* this )
{
    return this->name;
}

cube_location_type
cube_location_get_type( cube_location* this )
{
    return this->type;
}

int
cube_location_get_rank( cube_location* this )
{
    return this->rank;
}

int
cube_location_get_level( cube_location* this )
{
    if ( cube_location_get_parent( this ) == NULL )
    {
        return 0;
    }
    else
    {
        return cube_location_group_get_level( cube_location_get_parent( this ) ) + 1;
    }
}

/**
 * Writes XML output for location in to .cube file.
 * No recursiv. Plain one after another.
 *
 */
void
cube_location_writeXML( cube_location*         this,
                        cube_meta_data_writer* meta_data_writer )
{
    char* _name = ( char* )__cube_services_escape_to_xml( cube_location_get_name( this ) );

    cube_location_type __type = cube_location_get_type( this );
    char*              _type  =  ( __type == CUBE_LOCATION_TYPE_CPU_THREAD ) ? "thread" :
                                ( __type == CUBE_LOCATION_TYPE_GPU ) ? "gpu" :
                                ( __type == CUBE_LOCATION_TYPE_METRIC ) ? "metric" : "not supported";

    __cube_write_meta_data( meta_data_writer,  "<location Id=\"%d\">\n", cube_location_get_id( this ) );
    __cube_write_meta_data( meta_data_writer,  "<name>%s</name>\n", _name ? _name : cube_location_get_name( this ) );
    __cube_write_meta_data( meta_data_writer,  "<rank>%d</rank>\n", cube_location_get_rank( this ) );
    __cube_write_meta_data( meta_data_writer,  "<type>%s</type>\n", _type );
    __cube_services_write_attributes( meta_data_writer,  this->attr );
    __cube_write_meta_data( meta_data_writer,  "</location>\n" );

    CUBEW_FREE( _name, MEMORY_TRACING_PREFIX "Release _name"  );
}

void
cube_location_set_id( cube_location* this,
                      int            new_id )
{
    this->id = new_id;
}

int
cube_location_get_id( cube_location* this )
{
    return this->id;
}

/**
 * Compares equality of two locations.
 */
int
cube_location_equal( cube_location* a,
                     cube_location* b )
{
    int _a = cube_location_get_rank( a );
    int _b = cube_location_get_rank( b );
    if ( _a == _b )
    {
        int _p_a = cube_location_group_get_rank( cube_location_get_parent( a ) );
        int _p_b = cube_location_group_get_rank( cube_location_get_parent( b ) );
        if (  _p_a == _p_b )
        {
            return 1;
        }
    }
    return 0;
}
