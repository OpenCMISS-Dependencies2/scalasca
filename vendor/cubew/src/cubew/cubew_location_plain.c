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
 * \file cubew_location_plain.c
   \brief Defines types and functions to deal with locations of running application.
 */
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include "cubew_location_plain.h"
#include "cubew_location_group_plain.h"
#include "cubew_meta_data_writer.h"
#include "cubew_vector.h"
#include "cubew_services.h"
#include "cubew_memory.h"

#define MEMORY_TRACING_PREFIX "[LOCATION PLAIN]"

/**
 * Allocates memory for the structure "location"
 */
cube_location_plain*
cube_location_plain_create( void )
{
    cube_location_plain* this = NULL;
    ALLOC( this, 1, cube_location_plain, MEMORY_TRACING_PREFIX "Allocate plain location" );
    XALLOC( this->attr, 1, att_array, MEMORY_TRACING_PREFIX "Allocate plain location attribute" );
    this->attr->size     = 0;
    this->attr->capacity = 0;
    return this;
}


/**
 * Fills the location with the information (rank, namem, id).
 *
 */
void
cube_location_plain_init( cube_location_plain* this,
                          char*                name,
                          int                  rank,
                          cube_location_type   type )
{
    this->name = name;
    this->rank = rank;
    this->type = type;
    cube_location_plain_clear( this );
}

void
cube_location_plain_clear( cube_location_plain* this )
{
    if ( this->attr )
    {
        int i = 0;
        for ( i = 0; i < this->attr->size; i++ )
        {
            CUBEW_FREE( ( this->attr->data[ i ] )->key, MEMORY_TRACING_PREFIX "Release key of a plain location attribute" );
            CUBEW_FREE( ( this->attr->data[ i ] )->value, MEMORY_TRACING_PREFIX "Release value of a plain location attribute" );
            CUBEW_FREE( this->attr->data[ i ], MEMORY_TRACING_PREFIX "Release a plain location attribute" );
        }
        CUBEW_FREE( this->attr->data, MEMORY_TRACING_PREFIX "Release vector of plain location attributes" );
        this->attr->size     = 0;
        this->attr->capacity = 0;
    }
}


/**
 * Releases memory of the structure "location"
 */
void
cube_location_plain_free( cube_location_plain* this )
{
    if ( this != NULL )
    {
        cube_location_plain_clear( this );
        CUBEW_FREE( this->attr, MEMORY_TRACING_PREFIX "Release plain location attributes" );
    }
    CUBEW_FREE( this, MEMORY_TRACING_PREFIX "Release a plain location" );
}

void
cube_location_plain_add_attr( cube_location_plain* this,
                              cmap*                m )
{
    ADD_NEXT( this->attr, m, cmap*, MEMORY_TRACING_PREFIX "Add a plain location attrubute" );
}

void
cube_location_plain_def_attr( cube_location_plain* this,
                              char*                key,
                              char*                value )
{
    cmap*
    XALLOC( m,
            1,
            cmap,
            MEMORY_TRACING_PREFIX "Allocate a plain location attribute" );

    m->key   = key;
    m->value = value;
    cube_location_plain_add_attr( this, m );
}

void
cube_location_plain_write( cube_location_plain*   this,
                           uint32_t               id,
                           cube_meta_data_writer* meta_data_writer )
{
    if ( this != NULL )
    {
        char* _name = ( char* )__cube_services_escape_to_xml( this->name );

        cube_location_type __type = this->type;
        char*              _type  =  ( __type == CUBE_LOCATION_TYPE_CPU_THREAD ) ? "thread" :
                                    ( __type == CUBE_LOCATION_TYPE_GPU ) ? "gpu" :
                                    ( __type == CUBE_LOCATION_TYPE_METRIC ) ? "metric" : "not supported";

        __cube_write_meta_data( meta_data_writer,  "<location Id=\"%d\">\n", id );
        __cube_write_meta_data( meta_data_writer,  "<name>%s</name>\n", _name  );
        __cube_write_meta_data( meta_data_writer,  "<rank>%d</rank>\n", this->rank );
        __cube_write_meta_data( meta_data_writer,  "<type>%s</type>\n", _type );
        __cube_services_write_attributes( meta_data_writer,  this->attr );
        CUBEW_FREE( _name, MEMORY_TRACING_PREFIX "Release _name" );
    }
    else
    {
        __cube_write_meta_data( meta_data_writer,  "</location>\n" );
    }
}
