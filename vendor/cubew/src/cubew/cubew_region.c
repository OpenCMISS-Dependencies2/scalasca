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
 * \file cubew_region.c
   \brief Defines types and functions to deal with regions in source code of running application.
 */
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include "cubew_region.h"
#include "cubew_memory.h"
#include "cubew_cnode.h"
#include "cubew_vector.h"
#include "cubew_services.h"
#include "cubew_meta_data_writer.h"
#include "cubew_types.h"

#define MEMORY_TRACING_PREFIX "[REGION]"


/**
 * Creating of callee node to this region.
 *
 */
static
void
cube_region_construct_cnode( cube_region* this )
{
    XALLOC( this->cnode, 1, cube_rarray, MEMORY_TRACING_PREFIX "Allocate vector of caller of a region"  );
    this->cnode->size     = 0;
    this->cnode->capacity = 0;
}


/**
 * Allocationg memory for a region.
 *
 */
cube_region*
cube_region_create( cube_region* this )
{
    if ( this == NULL )
    {
        ALLOC( this, 1, cube_region, MEMORY_TRACING_PREFIX "Allocate region"  );
    }
    if ( this != NULL )
    {
        cube_region_construct_cnode( this );
    }
    this->attr = NULL;
    /* construct attrs array */
    XALLOC( this->attr, 1, att_array, MEMORY_TRACING_PREFIX "Allocate region attributes"  );
    this->attr->size     = 0;
    this->attr->capacity = 0;

    return this;
}

/**
 * Filling a region with the information.
 *
 */
void
cube_region_init( cube_region* this,
                  const char*  name,
                  const char*  mangled_name,
                  const char*  paradigm,
                  const char*  role,
                  int          begln,
                  int          endln,
                  const char*  url,
                  const char*  descr,
                  const char*  mod )
{
    this->name         = cubew_strdup( name );
    this->mangled_name = cubew_strdup( mangled_name );
    this->paradigm     = cubew_strdup( paradigm );
    this->role         = cubew_strdup( role );
    this->begln        = begln;
    this->endln        = endln;
    this->url          = cubew_strdup( url );
    this->descr        = cubew_strdup( descr );
    this->mod          = cubew_strdup( mod );
}


/**
 * Addition of the callee node to this region.
 *
 */
void
cube_region_add_cnode( cube_region* this,
                       cube_cnode*  cnode )
{
    int i    = 0;
    int size = cube_region_num_children( this );
    for ( i = 0; i < size; i++ )
    {
        if ( cube_cnode_equal( cnode, this->cnode->data[ i ] ) == 1 )
        {
            return;
        }
    }
    ADD_NEXT( this->cnode, cnode, cube_cnode*, MEMORY_TRACING_PREFIX "Add caller of the region"  );
}

/**
 * Releases memory for a region.
 *
 */
void
cube_region_free( cube_region* this )
{
    if ( this != NULL )
    {
        CUBEW_FREE( this->name, MEMORY_TRACING_PREFIX "Release name of a region"  );
        CUBEW_FREE( this->mangled_name, MEMORY_TRACING_PREFIX "Release mangled name of a region" );
        CUBEW_FREE( this->paradigm, MEMORY_TRACING_PREFIX "Release paradigm of a region"  );
        CUBEW_FREE( this->role, MEMORY_TRACING_PREFIX "Release role of a region"  );
        CUBEW_FREE( this->url, MEMORY_TRACING_PREFIX "Release url of a region"  );
        CUBEW_FREE( this->descr, MEMORY_TRACING_PREFIX "Release description of a region"  );
        CUBEW_FREE( this->mod, MEMORY_TRACING_PREFIX "Release module of a region"  );

        CUBEW_FREE( this->cnode->data, MEMORY_TRACING_PREFIX "Release vector of caller of a region" );
        CUBEW_FREE( this->cnode, MEMORY_TRACING_PREFIX "Release callers of a region" );

        if ( this->attr )
        {
            int i = 0;
            for ( i = 0; i < this->attr->size; i++ )
            {
                CUBEW_FREE( ( this->attr->data[ i ] )->key, MEMORY_TRACING_PREFIX "Release key of a region attribute"  );
                CUBEW_FREE( ( this->attr->data[ i ] )->value, MEMORY_TRACING_PREFIX "Release value of a region attribute" );
                CUBEW_FREE( this->attr->data[ i ], MEMORY_TRACING_PREFIX "Release region attribute"  );
            }
            CUBEW_FREE( this->attr->data, MEMORY_TRACING_PREFIX "Release vector of region attributes" );
        }
        CUBEW_FREE( this->attr, MEMORY_TRACING_PREFIX "Release region attributes"  );
    }
    CUBEW_FREE( this, MEMORY_TRACING_PREFIX "Release region"  );
}



/**
 * Adds an attribute  "m" into cube "this"
 */
void
cube_region_add_attr( cube_region* this,
                      cmap*        m )
{
    ADD_NEXT( this->attr, m, cmap*, MEMORY_TRACING_PREFIX "Add a region attrubute" );
}

/**
 * Creates and add in to cube "this" the attribute "key" with a vaue "value"
 */
void
cube_region_def_attr( cube_region* this,
                      const char*  key,
                      const char*  value )
{
    char* nkey   = cubew_strdup( key );
    char* nvalue = cubew_strdup( value );
    cmap*
    XALLOC( m,
            1,
            cmap,
            MEMORY_TRACING_PREFIX "Allocate region attribute"   );

    m->key   = nkey;
    m->value = nvalue;
    cube_region_add_attr( this, m );
}


char*
cube_region_get_name( cube_region* this )
{
    return this->name;
}
char*
cube_region_get_mangled_name( cube_region* this )
{
    return this->mangled_name;
}

char*
cube_region_get_paradigm( cube_region* this )
{
    return this->paradigm;
}

char*
cube_region_get_role( cube_region* this )
{
    return this->role;
}

char*
cube_region_get_url( cube_region* this )
{
    return this->url;
}

char*
cube_region_get_descr( cube_region* this )
{
    return this->descr;
}

char*
cube_region_get_mod( cube_region* this )
{
    return this->mod;
}

int
cube_region_get_begn_ln( cube_region* this )
{
    return ( this->begln <= 0 ) ? -1 : this->begln;
}

int
cube_region_get_end_ln( cube_region* this )
{
    return ( this->endln <= 0 ) ? -1 : this->endln;
}

int
cube_region_num_children( cube_region* this )
{
    return this->cnode->size;
}

/**
 * Writes XML output for region in to .cube file.
 *
 * A special algorithm for saving a name of the region implemented.
 */
void
cube_region_writeXML( cube_region*           this,
                      cube_meta_data_writer* meta_data_writer )
{
    char* _mod      = __cube_services_escape_to_xml( cube_region_get_mod( this ) );
    char* _name     = __cube_services_escape_to_xml( cube_region_get_name( this ) );
    char* _m_name   = __cube_services_escape_to_xml( cube_region_get_mangled_name( this ) );
    char* _paradigm = __cube_services_escape_to_xml( cube_region_get_paradigm( this ) );
    char* _role     = __cube_services_escape_to_xml( cube_region_get_role( this ) );
    char* _url      = __cube_services_escape_to_xml( cube_region_get_url( this ) );
    char* _descr    = __cube_services_escape_to_xml( cube_region_get_descr( this ) );

    int _id       =  cube_region_get_id( this );
    int _begin_ln = cube_region_get_begn_ln( this );
    int _end_ln   = cube_region_get_end_ln( this );
    __cube_write_meta_data( meta_data_writer, "<region id=\"%d\" mod=\"%s\" begin=\"%d\" end=\"%d\">\n",
                            _id, _mod ? _mod : cube_region_get_mod( this ),
                            _begin_ln, _end_ln  );


    __cube_write_meta_data( meta_data_writer, "<name>%s</name>\n",  _name ? _name : cube_region_get_name( this ) );
    if ( _name ||  cube_region_get_mangled_name( this ) )
    {
        __cube_write_meta_data( meta_data_writer, "<mangled_name>%s</mangled_name>\n",  _m_name ? _m_name : cube_region_get_mangled_name( this ) );
    }
    __cube_write_meta_data( meta_data_writer, "<paradigm>%s</paradigm>\n",  _paradigm ? _paradigm : cube_region_get_paradigm( this ) );
    __cube_write_meta_data( meta_data_writer, "<role>%s</role>\n",  _role ? _role : cube_region_get_role( this ) );
    __cube_write_meta_data( meta_data_writer, "<url>%s</url>\n",  _url ? _url : cube_region_get_url( this ) );
    __cube_write_meta_data( meta_data_writer, "<descr>%s</descr>\n", _descr ? _descr : cube_region_get_descr( this ) );
    __cube_services_write_attributes( meta_data_writer,  this->attr );
    __cube_write_meta_data( meta_data_writer, "</region>\n" );


    CUBEW_FREE( _mod, MEMORY_TRACING_PREFIX "Release _mod" );
    CUBEW_FREE( _name, MEMORY_TRACING_PREFIX "Release _name" );
    CUBEW_FREE( _m_name, MEMORY_TRACING_PREFIX "Release _m_name" );
    CUBEW_FREE( _paradigm, MEMORY_TRACING_PREFIX "Release _paradigm" );
    CUBEW_FREE( _role, MEMORY_TRACING_PREFIX "Release _role" );
    CUBEW_FREE( _url, MEMORY_TRACING_PREFIX "Release _url" );
    CUBEW_FREE( _descr, MEMORY_TRACING_PREFIX "Release _descr" );
}

/**
 * Compares equaliy of two regions
 */

int
cube_region_equal( cube_region* a,
                   cube_region* b )
{
    const char* _a = cube_region_get_name( a );
    const char* _b = cube_region_get_name( b );
    if ( strcmp( _a, _b ) == 0 )
    {
        const char* _mod_a = cube_region_get_mod( a );
        const char* _mod_b = cube_region_get_mod( b );
        if ( strcmp( _mod_a, _mod_b ) == 0 )
        {
            return 1;
        }
    }
    return 0;
}

void
cube_region_set_id( cube_region* this,
                    int          new_id )
{
    this->id = new_id;
}

int
cube_region_get_id( cube_region* this )
{
    return this->id;
}
