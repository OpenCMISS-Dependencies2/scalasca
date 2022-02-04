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
 * \file cubew_cnode.c
   \brief Defines types and functions to deal with calee nodes.
 */
#include "config.h"
#include <stdlib.h>
#include <string.h>

#include "cubew_cnode.h"
#include "cubew_memory.h"
#include "cubew_region.h"
#include "cubew_vector.h"
#include "cubew_meta_data_writer.h"
#include "cubew_services.h"
#include "cubew_types.h"
#include "UTILS_Error.h"


#define MEMORY_TRACING_PREFIX "[CNODE]"


/**
 * Allocate memory for a child of some cnoed "this"
 */
static
void
cube_cnode_construct_child( cube_cnode* this )
{
    XALLOC( this->child, 1, carray, MEMORY_TRACING_PREFIX "Allocate cnode children" );
    this->child->size     = 0;
    this->child->capacity = 0;
}



/**
 * Allocates memory for cnodes in the cube.
 */
cube_cnode*
cube_cnode_create( cube_cnode* this )
{
    if ( this == NULL )
    {
        ALLOC( this, 1, cube_cnode, MEMORY_TRACING_PREFIX "Allocate cnode" );
    }
    if ( this != NULL )
    {
        cube_cnode_construct_child( this );
    }
    this->num_parameters    = NULL;
    this->string_parameters = NULL;
    this->attr              = NULL;
    /* construct attrs array */
    XALLOC( this->attr, 1, att_array, MEMORY_TRACING_PREFIX "Allocate cnode attributes" );
    this->attr->size     = 0;
    this->attr->capacity = 0;
    return this;
}

/**
 * Initialize the cnode structure with data and connect parents and children.
 */
void
cube_cnode_init( cube_cnode*         this,
                 struct cube_region* callee,
                 const char*         mod,
                 int                 line,
                 cube_cnode*         parent )
{
    this->parent = parent;
    this->callee = callee;
    this->mod    = cubew_strdup( mod );
    this->line   = line;

//   this->id = id;
    if ( parent != NULL )
    {
        cube_cnode_add_child( this->parent, this );
    }
}


/**
 * Add a new parameter in the parameter list. If there is no parameter yet, memory should be allocated.
 */
void
cube_cnode_add_numeric_parameter( cube_cnode* this,
                                  const char* param_name,
                                  double      value )
{
    if ( this->num_parameters == NULL )
    {
        XALLOC( this->num_parameters, 1, nparam_parray, MEMORY_TRACING_PREFIX "Allocate cnode numeric parameters" );
        this->num_parameters->size     = 0;
        this->num_parameters->capacity = 0;
    }
    nmap* parameter_pair = ( nmap* )CUBEW_MALLOC( sizeof( nmap ), MEMORY_TRACING_PREFIX "Allocate numeric parameter pair"  );
    parameter_pair->key   = cubew_strdup( param_name );
    parameter_pair->value = value;
    ADD_NEXT( this->num_parameters, parameter_pair, nmap*, MEMORY_TRACING_PREFIX "Add numeric parameter" );
}


/**
 * Add a new parameter in the parameter list. If there is no parameter yet, memory should be allocated.
 */
void
cube_cnode_add_string_parameter( cube_cnode* this,
                                 const char* param_name,
                                 const char* value )
{
    if ( this->string_parameters == NULL )
    {
        XALLOC( this->string_parameters, 1, cparam_parray, MEMORY_TRACING_PREFIX "Allocate cnode symbolic parameters" );
        this->string_parameters->size     = 0;
        this->string_parameters->capacity = 0;
    }

    cmap* parameter_pair = ( cmap* )CUBEW_MALLOC( sizeof( cmap ), MEMORY_TRACING_PREFIX "Allocate symbolic parameter pair"   );
    parameter_pair->key   = cubew_strdup( param_name );
    parameter_pair->value = cubew_strdup( value );
    ADD_NEXT( this->string_parameters, parameter_pair, cmap*, MEMORY_TRACING_PREFIX "Add symbolic parameter"  );
}





/**
 * Adds a child cnode "cnode" to the parent "parent"
 */
void
cube_cnode_add_child( cube_cnode* parent,
                      cube_cnode* cnode )
{
    ADD_NEXT( parent->child, cnode, cube_cnode*, MEMORY_TRACING_PREFIX "Add next child" );
}

/**
 * Releases a memory of cnode "this"
 */
void
cube_cnode_free( cube_cnode* this )
{
    if ( this != NULL )
    {
        if ( this->mod != 0 )
        {
            CUBEW_FREE( this->mod, MEMORY_TRACING_PREFIX "Release mod" );
        }
        CUBEW_FREE( this->child->data, MEMORY_TRACING_PREFIX "Release vector of children" );
        CUBEW_FREE( this->child, MEMORY_TRACING_PREFIX "Release child" );
        int i = 0;
        if ( this->num_parameters != NULL )
        {
            for ( i = 0; i < this->num_parameters->size; i++ )
            {
                CUBEW_FREE( this->num_parameters->data[ i ]->key, MEMORY_TRACING_PREFIX "Release key of numeric parameter" );
                CUBEW_FREE( this->num_parameters->data[ i ], MEMORY_TRACING_PREFIX "Release pair of numeric parameters" );
            }
            CUBEW_FREE( this->num_parameters->data, MEMORY_TRACING_PREFIX "Release content of numeric parameters" );
            CUBEW_FREE( this->num_parameters, MEMORY_TRACING_PREFIX "Release numeric parameters"  );
        }
        if ( this->string_parameters != NULL )
        {
            for ( i = 0; i < this->string_parameters->size; i++ )
            {
                CUBEW_FREE( this->string_parameters->data[ i ]->key, MEMORY_TRACING_PREFIX "Release key of symbolic parameter"  );
                CUBEW_FREE( this->string_parameters->data[ i ]->value, MEMORY_TRACING_PREFIX "Release value of symbolic parameter"  );
                CUBEW_FREE( this->string_parameters->data[ i ], MEMORY_TRACING_PREFIX "Release pair of symbolic parameters"  );
            }
            CUBEW_FREE( this->string_parameters->data, MEMORY_TRACING_PREFIX "Release content of symbolic parameters"  );
            CUBEW_FREE( this->string_parameters, MEMORY_TRACING_PREFIX "Release symbolic parameters"  );
        }
        if ( this->attr )
        {
            for ( i = 0; i < this->attr->size; i++ )
            {
                CUBEW_FREE( ( this->attr->data[ i ] )->key, MEMORY_TRACING_PREFIX "Release key of attributes"   );
                CUBEW_FREE( ( this->attr->data[ i ] )->value, MEMORY_TRACING_PREFIX "Release value of attributes"  );
                CUBEW_FREE( this->attr->data[ i ], MEMORY_TRACING_PREFIX "Release pair of attributes" );
            }
            CUBEW_FREE( this->attr->data, MEMORY_TRACING_PREFIX "Release content of attributes"  );
            CUBEW_FREE( this->attr, MEMORY_TRACING_PREFIX "Release attributes" );
        }
        CUBEW_FREE( this, MEMORY_TRACING_PREFIX "Release cnode" );
    }
}



/**
 * Returns a i-th child of the given cnode.
 */
cube_cnode*
cube_cnode_get_child( cube_cnode* this,
                      int         i )
{
    if ( i < 0 || i >= this->child->size )
    {
        UTILS_WARNING( "cube_cnode_get_child: out of range\n" );
    }
    return this->child->data[ i ];
}


/**
 *  Returns a parent of the cnode.
 */
cube_cnode*
cube_cnode_get_parent( cube_cnode* this )
{
    return this->parent;
}


/**
 *  Returns a line of the source code for current cnode
 */
int
cube_cnode_get_line( cube_cnode* this )
{
    return ( this->line <= 0 ) ? -1 : this->line;
}

/**
 *  Returns a modus of the current cnode.
 */
char*
cube_cnode_get_mod( cube_cnode* this )
{
    return this->mod;
}

/**
 *  Returns a calee ot the current cnode.
 */
cube_region*
cube_cnode_get_callee( cube_cnode* this )
{
    return this->callee;
}

/**
 *  Returns a caller of the current cnode.
 */
cube_region*
cube_cnode_get_caller( cube_cnode* this )
{
    if ( cube_cnode_get_parent( this ) == NULL )
    {
        return NULL;
    }
    return cube_cnode_get_callee( cube_cnode_get_parent( this ) );
}

/**
 *  Returns a number of the children of the current cnode.
 */
int
cube_cnode_num_children( cube_cnode* this )
{
    return this->child->size;
}


/**
 *  Sets an id to the cnode.
 */
void
cube_cnode_set_id( cube_cnode* this,
                   int         new_id )
{
    this->id = new_id;
}


/**
 * Returns an id of the cnode.
 */
int
cube_cnode_get_id( cube_cnode* this )
{
    return this->id;
}

/**
 * Returns the level
 */
int
cube_cnode_get_level( cube_cnode* this )
{
    if ( cube_cnode_get_parent( this ) == NULL )
    {
        return 0;
    }
    else
    {
        return cube_cnode_get_level( cube_cnode_get_parent( this ) ) + 1;
    }
}


/**
 * Adds an attribute  "m" into cube "this"
 */
void
cube_cnode_add_attr( cube_cnode* this,
                     cmap*       m )
{
    ADD_NEXT( this->attr, m, cmap*, MEMORY_TRACING_PREFIX "Add attribute" );
}

/**
 * Creates and add in to cube "this" the attribute "key" with a vaue "value"
 */
void
cube_cnode_def_attr( cube_cnode* this,
                     const char* key,
                     const char* value )
{
    char* nkey   = cubew_strdup( key );
    char* nvalue = cubew_strdup( value );
    cmap*
    XALLOC( m,
            1,
            cmap,
            MEMORY_TRACING_PREFIX "Allocate attribure"
            );

    m->key   = nkey;
    m->value = nvalue;
    cube_cnode_add_attr( this, m );
}



/**
 * Writes XML output for cnode in "<program>" part of the .cube file.

 */
void
cube_cnode_writeXML( cube_cnode*            this,
                     cube_meta_data_writer* meta_data_writer )
{
    char* _mod = ( char* )__cube_services_escape_to_xml( cube_cnode_get_mod( this ) );



    int i = 0;
    __cube_write_meta_data( meta_data_writer, "<cnode id=\"%d\" ", cube_cnode_get_id( this ) );
    if ( cube_cnode_get_line( this ) != -1 )
    {
        __cube_write_meta_data( meta_data_writer, "line=\"%d\" ", cube_cnode_get_line( this ) );
    }
    if ( strcmp( cube_cnode_get_mod( this ), "" ) != 0 )
    {
        __cube_write_meta_data( meta_data_writer, "mod=\"%s\" ", _mod ? _mod : cube_cnode_get_mod( this ) );
    }
    __cube_write_meta_data( meta_data_writer, "calleeId=\"%d\">\n", cube_region_get_id( cube_cnode_get_callee( this ) ) );
    /*  write parameters */
    if ( this->num_parameters != NULL )
    {
        for ( i = 0; i < this->num_parameters->size; i++ )
        {
            char* _key = ( char* )__cube_services_escape_to_xml( this->num_parameters->data[ i ]->key );
            __cube_write_meta_data( meta_data_writer, "<parameter partype=\"numeric\" parkey=\"%s\" parvalue=\"%g\" />\n", _key, this->num_parameters->data[ i ]->value );
            CUBEW_FREE( _key, MEMORY_TRACING_PREFIX "Release numeric _key" );
        }
    }
    if ( this->string_parameters != NULL )
    {
        for ( i = 0; i < this->string_parameters->size; i++ )
        {
            char* _key   = ( char* )__cube_services_escape_to_xml( this->string_parameters->data[ i ]->key );
            char* _value = ( char* )__cube_services_escape_to_xml( this->string_parameters->data[ i ]->value );

            __cube_write_meta_data( meta_data_writer, "<parameter partype=\"string\" parkey=\"%s\" parvalue=\"%s\" />\n", _key, _value );
            CUBEW_FREE( _key, MEMORY_TRACING_PREFIX "Release symbolic _key" );
            CUBEW_FREE( _value, MEMORY_TRACING_PREFIX "Release symbolic _value" );
        }
    }

    __cube_services_write_attributes( meta_data_writer,  this->attr );

    for ( i = 0; i < cube_cnode_num_children( this ); i++ )
    {
        cube_cnode* child = cube_cnode_get_child( this, i );
        cube_cnode_writeXML( child, meta_data_writer );
    }
    __cube_write_meta_data( meta_data_writer, "</cnode>\n" );

    CUBEW_FREE( _mod, MEMORY_TRACING_PREFIX "Release mod" );
}

/**
 * Compares on equality two cnodes.
 */
int
cube_cnode_equal( cube_cnode* a,
                  cube_cnode* b )
{
    const char* _a = cube_cnode_get_mod( a );
    const char* _b = cube_cnode_get_mod( b );

    if ( strcmp( _a, _b  ) == 0 )
    {
        cube_region* _reg_a = cube_cnode_get_callee( a );
        cube_region* _reg_b = cube_cnode_get_callee( b );


        if ( cube_region_equal( _reg_a, _reg_b ) == 1 )
        {
            int _line_a = cube_cnode_get_line( a );
            int _line_b = cube_cnode_get_line( b );
            if ( _line_a  == _line_b )
            {
                return 1;
            }
        }
    }
    return 0;
}
