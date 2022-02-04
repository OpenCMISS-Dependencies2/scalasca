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
 * \file cubew_cartesian.c
   \brief Defines a types and functions to deal with cartesian topology.
 */
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include "cubew_cartesian.h"
#include "cubew_system_tree_node.h"
#include "cubew_location_group.h"
#include "cubew_location.h"
#include "cubew_vector.h"
#include "cubew_services.h"
#include "cubew_meta_data_writer.h"
#include "cubew_memory.h"
#include "UTILS_Error.h"

#define MEMORY_TRACING_PREFIX "[CART]"

/* CARTESIAN TYPES*/
/**
 * Maps a thread and its three-dimenasional coordinates.
 */
typedef struct map_thrd
{
    cube_thread* key;
    long int*    value;   /* currently any number of dimensions! */
} map_thrd;



/**
 * Allocates memory for the array with definition of coordnates for every thread.
 */
cube_cartesian*
cube_cart_create( cube_cartesian* this )
{
    if ( this == NULL )
    {
        ALLOC( this, 1, cube_cartesian, MEMORY_TRACING_PREFIX "Allocate topology" );
    }
    this->name = NULL;
    return this;
}

/**
 * Allocates memory for the arrays for dimensions and set the values for them.
 */
int
cube_cart_init( cube_cartesian* this,
                long int        ndims,
                long int*       dim,
                int*            period )
{
    int i = 0, locs = 1;
    this->ndims = ndims;
    ALLOC( this->dim, ndims, long int, MEMORY_TRACING_PREFIX "Allocate dimensions" );
    ALLOC( this->period, ndims, int, MEMORY_TRACING_PREFIX "Allocate period" );
    ALLOC( this->namedims, ndims, char*, MEMORY_TRACING_PREFIX "Allocate dimension names"  );
    if ( ( this->dim == NULL ) || ( this->period == NULL ) )
    {
        return 1;
    }
    for ( i = 0; i < ndims; i++ )
    {
        locs             *= dim[ i ];
        this->dim[ i ]    = dim[ i ];
        this->period[ i ] = period[ i ];
    }
    ALLOC( this->thrd2coord, locs, map_thrd, MEMORY_TRACING_PREFIX "Allocate location->coordinate mapping" );
    for ( i = 0; i < locs; i++ )
    {
        ALLOC( this->thrd2coord[ i ].value, ndims, long int, MEMORY_TRACING_PREFIX "Add coordinate" );
    }
    return this->thrd2coord == NULL;
}

/**
 * Releases allocated memory
 */
void
cube_cart_free( cube_cartesian* this )
{
    if ( this != NULL )
    {
        int i = 0, locs = 1;
        for ( i = 0; i < this->ndims; i++ )
        {
            locs *= this->dim[ i ];
            CUBEW_FREE( this->namedims[ i ], MEMORY_TRACING_PREFIX "Release dimension names" );
        }
        CUBEW_FREE( this->dim, MEMORY_TRACING_PREFIX "Release dimensions" );
        CUBEW_FREE( this->period, MEMORY_TRACING_PREFIX "Release period" );
        for ( i = 0; i < locs; i++ )
        {
            CUBEW_FREE( this->thrd2coord[ i ].value, MEMORY_TRACING_PREFIX "Release coordinate" );
        }
        CUBEW_FREE( this->thrd2coord, MEMORY_TRACING_PREFIX "Release location->coorinate mapping" );
        CUBEW_FREE( this->name, MEMORY_TRACING_PREFIX "Release topology name" );
        CUBEW_FREE( this->namedims, MEMORY_TRACING_PREFIX "Release names of dimensions" );
        CUBEW_FREE( this, MEMORY_TRACING_PREFIX "Release topology" );
    }
}

/**
 * Set for given thread "thrd" in the cartesian topology "this" coordinates "coord"
 */

void
cube_cart_def_coords( cube_cartesian* this,
                      cube_thread*    thrd,
                      long int*       coord )
{
    int i      = 0;
    int pos    = coord[ 0 ];
    int factor = this->dim[ 0 ];
    for ( i = 1; i < this->ndims; i++ )
    {
        pos    += coord[ i ] * factor;
        factor *= this->dim[ i ];
    }
    map_thrd* m = &this->thrd2coord[ pos ];

    m->key = thrd;
    for ( i = 0; i < this->ndims; i++ )
    {
        m->value[ i ] = coord[ i ];
    }
}


/**
   Sets the name of cartesian topology.
 */
void
cube_cart_set_name( cube_cartesian* cart,
                    const char*     name )
{
    CUBEW_FREE( cart->name, MEMORY_TRACING_PREFIX "Release topology name" );
    cart->name = cubew_strdup( name );
}



/**
   Sets the name of the dimensions
 */
void
cube_cart_set_namedims( cube_cartesian* cart,
                        char**          _namedims )
{
    int i = 0;
    for ( i = 0; i < cart->ndims; i++ )
    {
        CUBEW_FREE( cart->namedims[ i ], MEMORY_TRACING_PREFIX "Release name of the dimension" ); /* remove previosli set name, if was done. Prevent possible memory leaks.*/
        cart->namedims[ i ] = cubew_strdup( _namedims[ i ] );
    }
}
/**
   Sets the name of the dimension "dim"
 */
void
cube_cart_set_dim_name( cube_cartesian* cart,
                        const char*     _name,
                        int             dim )
{
    if ( dim < 0 && dim >= cart->ndims )
    {
        UTILS_WARNING( "Index for dimensions %d is out of range [0: %d]. Ignore setting name in topology %s. \n", dim, cart->ndims, cart->name );
        return;                                                                                 /* do not set name if index is out of range*/
    }
    CUBEW_FREE( cart->namedims[ dim ], MEMORY_TRACING_PREFIX "Release name of the dimension" ); /* remove previosli set name, if was done. Prevent possible memory leaks.*/
    cart->namedims[ dim ] = cubew_strdup( _name );
}

/**
 * Writes topology in XML format.
 */
void
cube_cart_writeXML( cube_cartesian*        this,
                    cube_meta_data_writer* meta_data_writer )
{
    int   i    = 0, j = 0, k = 0;
    int   locs = 1;
    char* per;

    char* _name =   ( char* )__cube_services_escape_to_xml( this->name );
    __cube_write_meta_data( meta_data_writer, "<cart " );
    if ( this->name != NULL )
    {
        __cube_write_meta_data( meta_data_writer, "name=\"%s\" ", _name );
    }
    __cube_write_meta_data( meta_data_writer, "ndims=\"%u\">\n", this->ndims );
    CUBEW_FREE( _name, MEMORY_TRACING_PREFIX "Release _name" );
    for ( i = 0; i < this->ndims; i++ )
    {
        locs *= this->dim[ i ];
        if ( this->period[ i ] == 0 )
        {
            per = "false";
        }
        else
        {
            per = "true";
        }
        __cube_write_meta_data( meta_data_writer, "<dim " );
        if ( this->namedims[ i ] != NULL )
        {
            char* dimName = ( char* )__cube_services_escape_to_xml( this->namedims[ i ] );
            __cube_write_meta_data( meta_data_writer, "name=\"%s\" ", dimName );
            CUBEW_FREE( dimName, MEMORY_TRACING_PREFIX "Release dimName" );
        }
        __cube_write_meta_data( meta_data_writer, "size=\"%ld\" periodic=\"%s\"/>\n", this->dim[ i ], per );
    }

    for ( j = 0; j < locs; j++ )
    {
        map_thrd*      m = &this->thrd2coord[ j ];
        cube_location* t = m->key;
        if ( t )
        {
            __cube_write_meta_data( meta_data_writer, "<coord locId=\"%d\">", cube_thread_get_id( t ) );
            for ( k = 0; k < this->ndims; k++ )
            {
                if ( k != 0 )
                {
                    __cube_write_meta_data( meta_data_writer, " " );
                }
                __cube_write_meta_data( meta_data_writer, "%ld", m->value[ k ] );
            }
            __cube_write_meta_data( meta_data_writer, "</coord>\n" );
        }
    }
    __cube_write_meta_data( meta_data_writer, "</cart>\n" );
}
