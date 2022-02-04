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
 * \file cubew_metric.c
   \brief Defines types and functions to deal with metrics.
 */
#include "config.h"

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include "cubew_memory.h"
#include "cubew_metric.h"
#include "cubew_cnode.h"
#include "cubew_report_layouts.h"
#include "cubew_services.h"
#include "cubew_vector.h"
#include "cubew_meta_data_writer.h"
#include "cubew_types.h"
#include "math.h"

#include "UTILS_Error.h"

#ifndef PRIu64
#define PRIu64 "llu"
#endif


#ifndef PRIu32
#define PRIu32 "lu"
#endif

#define MEMORY_TRACING_PREFIX "[METRIC]"

/******************++*************** local static calls *********************************************************/
/**
 * Creates child of the metric.
 */
static
void
cube_metric_construct_child( cube_metric* this )
{
    XALLOC( this->child, 1, marray, MEMORY_TRACING_PREFIX "Allocate vector of children of a metric" );
    this->child->size     = 0;
    this->child->capacity = 0;
}


#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
static
void
cube_metric_setup_subindex( cube_metric* metric )
{
    if ( metric->compressed == CUBE_TRUE )
    {
        metric->n_compressed = ( metric->known_cnodes == 0 ) ? metric->ncn :
                               __cube_metric_size_of_index( metric->known_cnodes, ( unsigned )ceil( ( double )metric->ncn / 8. ) );

        metric->subi_size =  3 * sizeof( uint64_t ) * metric->n_compressed;        /* maximal size of compressed data. Actual size get during the writing*/

        CUBEW_FREE( metric->sub_index, MEMORY_TRACING_PREFIX "Release previous sub index for the compression" );
        metric->sub_index = ( uint64_t* )CUBEW_CALLOC(  metric->subi_size, 1, MEMORY_TRACING_PREFIX "Allocate new sub index for the compression" );

        CUBEW_FREE( metric->compression_buffer, MEMORY_TRACING_PREFIX "Release previous buffer for the compression" );
        metric->compression_buffer = ( Bytef* )CUBEW_CALLOC( metric->nthrd * __cube_metric_size_dtype( metric->dtype_params ) + 512, 1, MEMORY_TRACING_PREFIX "Allocate new buffer for the compression" );     //  512 bytes bigger than origina.l. For the case of empry source , compressed buffer is bigger
    }
}
#endif

static
uint32_t
cube_metric_get_position_of_row( cube_metric* metric,
                                 unsigned     cid )                           // position in  file without marker. (as if there wouldn't be any markers at the beginning)
{
    uint32_t _position = 0;
    if ( metric->metric_format == CUBE_INDEX_FORMAT_DENSE )
    {
        _position = cid * ( metric->nthrd );
        return _position;
    }
    if ( metric->metric_format == CUBE_INDEX_FORMAT_SPARSE )
    {
        if ( metric->known_cnodes == 0 )
        {
            UTILS_WARNING( "[CUBEW Warning]: Parallel writing of cube with sparse metric is possible only with set bit vector.\n" );
            UTILS_WARNING( "[CUBEW Warning]: Format of metric is changed to DENSE.\n" );
            metric->metric_format = CUBE_INDEX_FORMAT_DENSE;
            return cube_metric_get_position_of_row( metric, cid );
        }
        if ( metric->known_cnodes != 0 )
        {
            /*  cid is the positions of cnode in the optimal enumeraton
             *  known_cnodes saves the sequence of cnodes according to the cnode->id's.
             * To get a proper position, backtransformation from (optimal ) -> to real id needed.
             * One gets it by looking up in the optimal_cnodes_sequence array.
             */
            _position =
                __cube_metric_bit_position( metric->known_cnodes, ( unsigned )ceil( ( double )metric->ncn / 8. ), cid );
            return _position * ( metric->nthrd );
        }
    }
    UTILS_WARNING( "[CUBEW Warning]: Metric binary format is neither DENSE nor SPARSE. Should be error. Return as position %u.\n", _position );
    return _position;
}



static
void*
cube_metric_transform_row_of_doubles( cube_metric* metric,
                                      double*      data_row )
{
    void* target_row = 0;
    ;
    switch ( metric->dtype_params->type )
    {
        case CUBE_DATA_TYPE_INT64:
            target_row = ( void* )__cube_transform_doubles_into_int64( data_row, metric->nthrd );
            break;
        case CUBE_DATA_TYPE_UINT64:
            target_row = ( void* )__cube_transform_doubles_into_uint64( data_row, metric->nthrd );
            break;
        default:
            target_row = ( void* )CUBEW_CALLOC( metric->nthrd * sizeof( double ), 1, MEMORY_TRACING_PREFIX "Allocate row of doubles with no transformation"  );
            memcpy( target_row, data_row, metric->nthrd * sizeof( double ) );
            break;
    }
    return target_row;
}

static
void*
cube_metric_transform_row_of_uint64( cube_metric* metric,
                                     uint64_t*    data_row )
{
    void* target_row = 0;
    ;
    switch ( metric->dtype_params->type )
    {
        case CUBE_DATA_TYPE_DOUBLE:
        case CUBE_DATA_TYPE_MIN_DOUBLE:
        case CUBE_DATA_TYPE_MAX_DOUBLE:
            target_row = ( void* )__cube_transform_uint64_into_doubles( data_row, metric->nthrd );
            break;
        default:
            target_row = ( void* )CUBEW_CALLOC( metric->nthrd * sizeof( uint64_t ), 1, MEMORY_TRACING_PREFIX "Allocate row of uint64_t with no transformation"  );
            memcpy( target_row, data_row, metric->nthrd * sizeof( uint64_t ) );
            break;
    }
    return target_row;
}



static
void*
cube_metric_transform_row_of_int64( cube_metric* metric,
                                    int64_t*     data_row )
{
    void* target_row = 0;
    ;
    switch ( metric->dtype_params->type )
    {
        case CUBE_DATA_TYPE_DOUBLE:
        case CUBE_DATA_TYPE_MIN_DOUBLE:
        case CUBE_DATA_TYPE_MAX_DOUBLE:
            target_row = ( void* )__cube_transform_int64_into_doubles( data_row, metric->nthrd );
            break;
        default:
            target_row = ( void* )CUBEW_CALLOC( metric->nthrd * sizeof( uint64_t ), 1, MEMORY_TRACING_PREFIX "Allocate row of int64_t with no transformation"  );
            memcpy( target_row, data_row, metric->nthrd * sizeof( uint64_t ) );
            break;
    }
    return target_row;
}


/**
 * Creates mapping "cnode->id  ----> metric_cnode_id" for simple metric, which has excl value == inclusive value
 */
static
void
__cube_metric_traverse_cnodes_simple(   unsigned*    map,
                                        unsigned*    sequence,
                                        cube_cnode** cnodes_sequence,
                                        unsigned*    index,
                                        unsigned     size,
                                        cube_cnode*  cnode )
{
    if ( *index >= size )
    {
        UTILS_FATAL( "Traversation index %u became bigger than size %u. Stop traversing (simple)", *index, size );
        return;
    }
    map[ cnode->id ]          = *index;
    sequence[ *index ]        = cnode->id;
    cnodes_sequence[ *index ] = cnode;
    ( *index )++;
    return;
}


/**
 * Creates mapping "cnode->id  ----> metric_cnode_id" for exclusive metric
 */
static
void
__cube_metric_traverse_cnodes_deep_search(      unsigned*    map,
                                                unsigned*    sequence,
                                                cube_cnode** cnodes_sequence,
                                                unsigned*    index,
                                                unsigned     size,
                                                cube_cnode*  cnode )
{
    if ( *index >= size )
    {
        UTILS_FATAL( "Traversation index %u became bigger than size %u. Stop traversing (deep search)", ( unsigned )( *index ), size );
        return;
    }
    map[ cnode->id ]          = *index;
    sequence[ *index ]        = cnode->id;
    cnodes_sequence[ *index ] = cnode;

    ( *index )++;
    if ( cnode->child->size != 0 )
    {
        unsigned i = 0;
        for ( i = 0; i < cnode->child->size; i++ )
        {
            __cube_metric_traverse_cnodes_deep_search( map, sequence, cnodes_sequence,  index, size, cnode->child->data[ i ] );
        }
    }
    return;
}

/**
 * Creates mapping "cnode->id  ----> metric_cnode_id" for inclusive metric
 */
static
void
__cube_metric_traverse_cnodes_wide_search(      unsigned*    map,
                                                unsigned*    sequence,
                                                cube_cnode** cnodes_sequence,
                                                unsigned*    index,
                                                unsigned     size,
                                                cube_cnode*  cnode )
{
    if ( ( *index ) == size )
    {
        return;
    }
    if ( ( *index ) > size )
    {
        UTILS_FATAL( "Traversation index %u became bigger than size %u. Stop traversing (wide search)", *index, size );
        return;
    }
    if ( cnode->parent == NULL )
    {
        map[ cnode->id ]          = *index;
        sequence[ *index ]        = cnode->id;
        cnodes_sequence[ *index ] = cnode;
        ( *index )++;
    }
    unsigned i = 0;
    for ( i = 0; i < cnode->child->size; i++ )
    {
        map[ cnode->child->data[ i ]->id ] = *index;
        sequence[ *index ]                 = cnode->child->data[ i ]->id;
        cnodes_sequence[ *index ]          = ( cnode->child->data[ i ] );

        ( *index )++;
    }
    for ( i = 0; i < cnode->child->size; i++ )
    {
        __cube_metric_traverse_cnodes_wide_search( map, sequence, cnodes_sequence, index, size, cnode->child->data[ i ] );
    }

    return;
}


/**
 *
 * Creates a local enumeration of cnodes for given metric, dependend on its type. Uses different subcalls and different parameters for those subcalls.
 *
 * It removes creates till now local enumeration and redoes according the given arrays
 */
static
void
cube_metric_create_enumeration( cube_metric*    this,
                                cube_dyn_array* roots_cnodes,
                                cube_dyn_array* cnodes )
{
    int          i   = 0;
    unsigned int idx = 0;
    if ( this->local_cnode_enumeration != NULL )
    {
        this->local_cnode_enumeration->size = 0;
        CUBEW_FREE( this->local_cnode_enumeration->data, MEMORY_TRACING_PREFIX "Release vector cnode enumeration"  );
    }
    else
    {
        ALLOC( this->local_cnode_enumeration, 1, cube_dyn_array, MEMORY_TRACING_PREFIX "Allocate local cnode enumeration"  );
    }

    if ( this->optimal_cnodes_sequence != NULL )
    {
        CUBEW_FREE( this->optimal_cnodes_sequence->data, MEMORY_TRACING_PREFIX "Release vector for optimal cnode sequence" );
        this->local_cnode_enumeration->size = 0;
    }
    else
    {
        ALLOC( this->optimal_cnodes_sequence, 1, cube_dyn_array, MEMORY_TRACING_PREFIX "Allocate  optimal cnode sequence" );
    }

    if ( this->optimal_cnodes_sequence_with_cndptrs != NULL )
    {
        CUBEW_FREE( this->optimal_cnodes_sequence_with_cndptrs->data, MEMORY_TRACING_PREFIX "Release vector of pointers for optimal cnode sequence" );
    }
    else
    {
        ALLOC( this->optimal_cnodes_sequence_with_cndptrs, 1, carray, MEMORY_TRACING_PREFIX "Allocate  pointers for optimal cnode sequence" );
    }



    this->local_cnode_enumeration->size              = this->ncn;
    this->optimal_cnodes_sequence->size              = this->ncn;
    this->optimal_cnodes_sequence_with_cndptrs->size = this->ncn;
    ALLOC( this->local_cnode_enumeration->data, this->ncn, void*, MEMORY_TRACING_PREFIX "Allocate vector for local cnode enumeration"  );
    ALLOC( this->optimal_cnodes_sequence->data, this->ncn, void*, MEMORY_TRACING_PREFIX "Allocate vector for optimal cnode sequence" );
    ALLOC( this->optimal_cnodes_sequence_with_cndptrs->data, this->ncn,  cube_cnode*, MEMORY_TRACING_PREFIX "Allocate vector of pointers for optimal cnode sequence" );

    switch ( this->metric_type )
    {
        case CUBE_METRIC_INCLUSIVE:
            for ( i = 0; i < roots_cnodes->size; i++ )
            {
                __cube_metric_traverse_cnodes_wide_search
                (
                    ( unsigned* )this->local_cnode_enumeration->data,
                    ( unsigned* )this->optimal_cnodes_sequence->data,
                    ( cube_cnode** )this->optimal_cnodes_sequence_with_cndptrs->data,
                    &idx,
                    this->local_cnode_enumeration->size,
                    roots_cnodes->data[ i ]
                );
            }
            break;
        case CUBE_METRIC_EXCLUSIVE:
            for ( i = 0; i < roots_cnodes->size; i++ )
            {
                __cube_metric_traverse_cnodes_deep_search(
                    ( unsigned* )this->local_cnode_enumeration->data,
                    ( unsigned* )this->optimal_cnodes_sequence->data,
                    ( cube_cnode** )this->optimal_cnodes_sequence_with_cndptrs->data,
                    &idx,
                    this->local_cnode_enumeration->size,
                    roots_cnodes->data[ i ]
                    );
            }
            break;
        case CUBE_METRIC_SIMPLE:
            for ( i = 0; i < cnodes->size; i++ )
            {
                __cube_metric_traverse_cnodes_simple
                (
                    ( unsigned* )this->local_cnode_enumeration->data,
                    ( unsigned* )this->optimal_cnodes_sequence->data,
                    ( cube_cnode** )this->optimal_cnodes_sequence_with_cndptrs->data,
                    &idx,
                    this->local_cnode_enumeration->size,
                    cnodes->data[ i ]
                );
            }
            break;
        case CUBE_METRIC_POSTDERIVED:          /* for derived metrics nothing to do*/
        case CUBE_METRIC_PREDERIVED_EXCLUSIVE: /* for derived metrics nothing to do*/
        case CUBE_METRIC_PREDERIVED_INCLUSIVE: /* for derived metrics nothing to do*/
        default:
            break;
    }
}




/**
 * Internal functions, which transforms a bitstring (with cnode-ids to btstring of cid's)
 */
char*
__cube_metric_bitstring_transformation( cube_metric* this,
                                        char*        known_cnodes )
{
    if ( known_cnodes == NULL )
    {
        return NULL;                       /*  no transformation is needed in this case*/
    }
    unsigned bitstringsize = ( this->ncn + 7 ) / 8;
    char*    to_return     = ( char* )CUBEW_CALLOC( bitstringsize, 1, MEMORY_TRACING_PREFIX "Allocate bitstring for cnodeid transformation" ); /*  Allocate  same amount of memory for bitstring*/
    memset( to_return, 0, bitstringsize );                                                                                                     /* Set the memory to 0  */
    unsigned i = 0;
    for ( i = 0; i < this->ncn; i++ )
    {
        unsigned id = i; /*  i = id of cnode */       /* ((unsigned *)(this->local_cnode_enumeration->data))[i]; */
        if ( __cube_is_bit_set( known_cnodes, id ) != 0 )
        {
            unsigned newposition = ( ( unsigned* )( this->local_cnode_enumeration->data ) )[ id ];
            __cube_set_bit( to_return, newposition );
        }
    }

    return to_return;
}


enum bool_t
__cube_metric_data_type_compatible( enum DataType datatype, enum CubeMetricType metrictype )
{
    switch ( metrictype )
    {
        case CUBE_METRIC_INCLUSIVE:
            if ( datatype == CUBE_DATA_TYPE_MAX_DOUBLE
                 ||
                 datatype == CUBE_DATA_TYPE_MIN_DOUBLE
                 ||
                 datatype == CUBE_DATA_TYPE_TAU_ATOMIC
                 )
            {
                return CUBE_FALSE;
            }
            break;
        case CUBE_METRIC_POSTDERIVED:
        case CUBE_METRIC_PREDERIVED_EXCLUSIVE:
        case CUBE_METRIC_PREDERIVED_INCLUSIVE:
            if ( datatype != CUBE_DATA_TYPE_DOUBLE )
            {
                return CUBE_FALSE;
            }
            break;
        case CUBE_METRIC_EXCLUSIVE:
        case CUBE_METRIC_SIMPLE:
        default:
            break;
    }
    return CUBE_TRUE;
}


/* *********************** experimental call for future ***+ not used currently***

   /**
   Function calculates the size of the index file for the given metric.
 */
static
uint64_t
cube_metric_size_of_index_file( cube_metric* this )
{
    uint64_t size = 0;
    if ( this->metric_format == CUBE_INDEX_FORMAT_SPARSE )
    {
        size = ( uint64_t )CUBE_INDEXFILE_MARKER_SIZE +                    /*  MARKER OF INDEX */
               ( uint64_t )sizeof( metric_header ) +                       /*  HEADER */
               ( uint64_t )sizeof( uint32_t ) +                            /*  SIZE OF INDEX */
               ( uint64_t )this->ncn * ( uint64_t )( sizeof( uint32_t ) ); /*  INDEX of metric */
    }
    if ( this->metric_format == CUBE_INDEX_FORMAT_DENSE )
    {
        size = ( uint64_t )CUBE_INDEXFILE_MARKER_SIZE +           /*  MARKER OF INDEX */
               ( uint64_t )sizeof( metric_header );               /*  HEADER */
    }
    return size;
}




/**
   Function calculates the size of the data file for the given metric.
 */
uint64_t
cube_metric_size_of_data_file( cube_metric* this )
{
    uint64_t size = 0;
    if ( this->compressed == CUBE_TRUE )
    {
        size += ( uint64_t )CUBE_DATAFILE_COMPRESSED_MARKER_SIZE;
    }
    if ( this->compressed == CUBE_FALSE )
    {
        size += ( uint64_t )CUBE_DATAFILE_MARKER_SIZE;
    }


    uint64_t ncnodes = 0;
    if ( this->known_cnodes != NULL )
    {
        ncnodes = __cube_num_of_set_bits( this->known_cnodes, ( this->ncn + 7 ) / 8 );
    }
    else
    {
        ncnodes = this->ncn;
    }

    size += ncnodes * ( uint64_t )this->nthrd * ( uint64_t )( __cube_metric_size_dtype( this->dtype_params ) );
    return size;
}




static
void*
cube_metric_get_empty_row( cube_metric* met )
{
    return __cube_get_zero_row( met->dtype_params, met->nthrd ); /* size -> in bytes */
}





/******************++************************************************************************/




/**
 * Allocates memory for the metric.
 */
cube_metric*
cube_metric_create( cube_metric* this )
{
    if ( this == NULL )
    {
        ALLOC( this, 1, cube_metric, MEMORY_TRACING_PREFIX "Allocate metric"  );
    }
    if ( this != NULL )
    {
        cube_metric_construct_child( this );
    }
    return this;
}

// extern print_random_bit_vector(unsigned, char*, unsigned , uint32_t*);



/**
 * Fills the metric with data.
 */
void
cube_metric_init( cube_metric*          this,
                  const char*           disp_name,
                  const char*           uniq_name,
                  const char*           dtype,
                  const char*           uom,
                  const char*           val,
                  const char*           url,
                  const char*           descr,
                  cube_metric*          parent,
                  report_layout_writer* layout,
                  enum CubeMetricType   metric_type,
                  enum bool_t           compressed )
{
    this->parent       = parent;
    this->disp_name    = cubew_strdup( disp_name );
    this->uniq_name    = cubew_strdup( uniq_name );
    this->dtype        = cubew_strdup( dtype );
    this->uom          = cubew_strdup( uom );
    this->val          = cubew_strdup( val );
    this->url          = cubew_strdup( url );
    this->descr        = cubew_strdup( descr );
    this->layout       = layout;
    this->data_file    = NULL;
    this->dtype_params = __cube_metric_dtype_param_split( dtype );

    this->ncn             = -1;
    this->nthrd           = -1;
    this->metric_type     = metric_type;
    this->metric_viz_type = CUBE_METRIC_NORMAL;

    if ( this->dtype_params->type != CUBE_DATA_TYPE_DOUBLE && ( this->metric_type == CUBE_METRIC_POSTDERIVED  || this->metric_type == CUBE_METRIC_PREDERIVED_EXCLUSIVE  || this->metric_type == CUBE_METRIC_PREDERIVED_INCLUSIVE )  )
    {
        UTILS_WARNING(  "Derived metrics support only DOUBLE or FLOAT as a datatype. Enforce DOUBLE datatype.\n" );
        __cube_value_param_free( this->dtype_params );
        this->dtype_params = __cube_metric_dtype_param_split( "DOUBLE" );
        CUBEW_FREE( this->dtype, MEMORY_TRACING_PREFIX "Release metric data type"  );
        this->dtype = cubew_strdup( "DOUBLE" );
    }



#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    this->compressed         = compressed;
    this->compression_buffer = 0;
#else
    this->compressed = CUBE_FALSE;
#endif /* CUBE_COMPRESSED */
    this->sub_index                            = 0;
    this->known_cnodes                         = 0;
    this->written_cnodes                       = 0;
    this->n_compressed                         = 0;
    this->icompressed                          = 0;
    this->incr_compressed                      = 0;
    this->last_seek_position                   = -1;
    this->local_cnode_enumeration              = NULL;
    this->optimal_cnodes_sequence              = NULL;
    this->optimal_cnodes_sequence_with_cndptrs = NULL;
    this->im_writing                           = CUBE_FALSE;
    this->im_finished                          = CUBE_FALSE;
    this->expression                           = NULL;
    this->init_expression                      = NULL;
    this->aggr_plus_expression                 = NULL;
    this->aggr_minus_expression                = NULL;
    this->aggr_aggr_expression                 = NULL;
    this->locationwise                         = CUBE_TRUE;
    this->is_cacheable                         = CUBE_TRUE;
    this->attr                                 = NULL;
    /* construct attrs array */
    XALLOC( this->attr, 1, att_array, MEMORY_TRACING_PREFIX "Allocate metric attributes"  );
    this->attr->size           = 0;
    this->attr->capacity       = 0;
    this->valid_initialization = CUBE_TRUE;
    this->metric_format        = ( this->known_cnodes    == NULL ) ? CUBE_INDEX_FORMAT_DENSE : CUBE_INDEX_FORMAT_SPARSE;
    if ( __cube_metric_data_type_compatible( this->dtype_params->type, this->metric_type ) == CUBE_FALSE )
    {
        this->valid_initialization = CUBE_FALSE;
        return;
    }
    if ( parent != NULL )
    {
        cube_metric_add_child( this->parent, this );
    }
}

enum bool_t
cube_metric_valid( cube_metric* this )
{
    return this->valid_initialization;
}



// void cube_metric_setup_for_writing( cube_metric* this, int ncnds, int nthrds)
void
cube_metric_setup_for_writing( cube_metric*    this,
                               cube_dyn_array* cnodes,
                               cube_dyn_array* roots_cnodes,
                               uint32_t        n_locations
                               )
{
//     if ( this->ncn != cnodes->size || this->nthrd != threads->size )
//     {
    this->ncn   = cnodes->size;
    this->nthrd = n_locations;

#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    if ( this->compressed == CUBE_TRUE )
    {
        cube_metric_setup_subindex( this );
    }
#endif  /* HAVE_LIB_Z */
//     }
    cube_metric_create_enumeration( this, roots_cnodes, cnodes );
}

void
cube_metric_set_expression( cube_metric* this, char* expression )
{
    if ( this->metric_type == CUBE_METRIC_POSTDERIVED || this->metric_type == CUBE_METRIC_PREDERIVED_INCLUSIVE || this->metric_type == CUBE_METRIC_PREDERIVED_EXCLUSIVE  )
    {
        this->expression = cubew_strdup( expression );
    }
}

char*
cube_metric_get_expression( cube_metric* this )
{
    return this->expression;
}


void
cube_metric_set_init_expression( cube_metric* this, char* expression )
{
    if ( this->metric_type == CUBE_METRIC_POSTDERIVED || this->metric_type == CUBE_METRIC_PREDERIVED_INCLUSIVE || this->metric_type == CUBE_METRIC_PREDERIVED_EXCLUSIVE  )
    {
        this->init_expression = cubew_strdup( expression );
    }
}

char*
cube_metric_get_init_expression( cube_metric* this )
{
    return this->init_expression;
}


void
cube_metric_set_aggr_expression( cube_metric*            this,
                                 enum CubeMetricAggrType type,
                                 char*                   expression )
{
    if ( !( this->metric_type == CUBE_METRIC_PREDERIVED_INCLUSIVE || this->metric_type == CUBE_METRIC_PREDERIVED_EXCLUSIVE ) )
    {
        UTILS_WARNING( "Only prederived metrics do specify an aggregation. Ignore." );
        return;
    }

    switch ( type )
    {
        case CUBE_METRIC_AGGR_PLUS:
            this->aggr_plus_expression = cubew_strdup( expression );
            break;
        case CUBE_METRIC_AGGR_MINUS:
            if ( this->metric_type == CUBE_METRIC_PREDERIVED_INCLUSIVE  )
            {
                this->aggr_minus_expression = cubew_strdup( expression );
            }
            else
            {
                UTILS_WARNING( "Only prederived inclusive metrics do specify an aggregation for minus operation. Ignore." );
            }
            break;
        case CUBE_METRIC_AGGR_AGGR:
            this->aggr_aggr_expression = cubew_strdup( expression );
            break;
        default:
            UTILS_WARNING( "Unknown type of the aggregation expression is specified. Ignore." );
    }
}

char*
cube_metric_get_aggr_expression( cube_metric*            this,
                                 enum CubeMetricAggrType type )
{
    if ( type == CUBE_METRIC_AGGR_PLUS )
    {
        return this->aggr_plus_expression;
    }
    if ( type == CUBE_METRIC_AGGR_MINUS )
    {
        return this->aggr_minus_expression;
    }
    if ( type == CUBE_METRIC_AGGR_AGGR )
    {
        return this->aggr_aggr_expression;
    }
    return NULL;
}



void
cube_metric_set_as_locationwise( cube_metric* metric,
                                 enum bool_t  _locationwise )
{
    metric->locationwise = _locationwise;
}
void
cube_metric_set_viz_type( cube_metric*           metric,
                          enum CubeMetricVizType viz_type )
{
    metric->metric_viz_type = viz_type;
}

enum CubeMetricVizType
cube_metric_get_viz_type( cube_metric* metric )
{
    return metric->metric_viz_type;
}




/**
 * Creates optimal sequence according to the local_enumeration of cnodes.
 * Optimal sequence one can use to perform optimal filling of the cube.
 * It corresponds to the neigbourhood relation for given metric
 */
carray*
cube_metric_return_enumeration( cube_metric* this )
{
    return this->optimal_cnodes_sequence_with_cndptrs;
}


/**
 * Adds an attribute  "m" into cube "this"
 */
void
cube_metric_add_attr( cube_metric* this,
                      cmap*        m )
{
    ADD_NEXT( this->attr, m, cmap*, MEMORY_TRACING_PREFIX "Add a metric attrubute" );
}

/**
 * Creates and add in to cube "this" the attribute "key" with a vaue "value"
 */
void
cube_metric_def_attr( cube_metric* this,
                      const char*  key,
                      const char*  value )
{
    char* nkey   = cubew_strdup( key );
    char* nvalue = cubew_strdup( value );
    cmap*
    XALLOC( m,
            1,
            cmap,
            MEMORY_TRACING_PREFIX "Allocate metric attribute"  );

    m->key   = nkey;
    m->value = nvalue;
    cube_metric_add_attr( this, m );
}



/**
 * Adds child in  the metric.
 */
void
cube_metric_add_child( cube_metric* parent,
                       cube_metric* met )
{
    ADD_NEXT( parent->child, met, cube_metric*, MEMORY_TRACING_PREFIX "Add metric to same parent"  );
}

/**
 * Releases memory of the metric.
 */
void
cube_metric_free( cube_metric* this )
{
    CUBEW_FREE( this->disp_name, MEMORY_TRACING_PREFIX "Release metric display name"   );
    CUBEW_FREE( this->uniq_name, MEMORY_TRACING_PREFIX "Release metric unique name"   );
    CUBEW_FREE( this->dtype, MEMORY_TRACING_PREFIX "Release metric dtype"  );
    CUBEW_FREE( this->uom, MEMORY_TRACING_PREFIX "Release metric uom" );
    CUBEW_FREE( this->val, MEMORY_TRACING_PREFIX "Release metric value" );
    CUBEW_FREE( this->url, MEMORY_TRACING_PREFIX "Release metric documentation url" );
    CUBEW_FREE( this->descr, MEMORY_TRACING_PREFIX "Release metric description" );
    __cube_value_param_free( this->dtype_params );
    CUBEW_FREE( this->child->data, MEMORY_TRACING_PREFIX "Release vector of metric children" );
    CUBEW_FREE( this->child, MEMORY_TRACING_PREFIX "Release metric children" );
    CUBEW_FREE( this->sub_index, MEMORY_TRACING_PREFIX "Release metric compression subindex" );
#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    if ( this->compressed == CUBE_TRUE )
    {
        CUBEW_FREE( this->compression_buffer, MEMORY_TRACING_PREFIX "Release metric compression buffer" );
    }
#endif /* HAVE_LIB_Z */
    CUBEW_FREE( this->known_cnodes, MEMORY_TRACING_PREFIX "Release metric bitmask of known cnodes" );
    CUBEW_FREE( this->written_cnodes, MEMORY_TRACING_PREFIX "Release metric bitmask of written cnodes" );
    if ( this->attr )
    {
        int i = 0;
        for ( i = 0; i < this->attr->size; i++ )
        {
            CUBEW_FREE( ( this->attr->data[ i ] )->key, MEMORY_TRACING_PREFIX "Release key of a metric attribute" );
            CUBEW_FREE( ( this->attr->data[ i ] )->value, MEMORY_TRACING_PREFIX "Release value of a metric attribute" );
            CUBEW_FREE( this->attr->data[ i ], MEMORY_TRACING_PREFIX "Release metric attribute" );
        }
        CUBEW_FREE( this->attr->data, MEMORY_TRACING_PREFIX "Release vector of metric attributes"  );
    }
    CUBEW_FREE( this->attr, MEMORY_TRACING_PREFIX "Release metric attributes" );

    if ( this->local_cnode_enumeration != 0 )
    {
        CUBEW_FREE( this->local_cnode_enumeration->data, MEMORY_TRACING_PREFIX "Release vector of a metric enumeration" );
    }
    CUBEW_FREE( this->local_cnode_enumeration, MEMORY_TRACING_PREFIX "Release metric enumeration" );
    if ( this->optimal_cnodes_sequence != 0 )
    {
        CUBEW_FREE( this->optimal_cnodes_sequence->data, MEMORY_TRACING_PREFIX "Release vector of a metric cnode sequence" );
    }
    CUBEW_FREE( this->optimal_cnodes_sequence, MEMORY_TRACING_PREFIX "Release metric dtype" );
    if ( this->optimal_cnodes_sequence_with_cndptrs != 0 )
    {
        CUBEW_FREE( this->optimal_cnodes_sequence_with_cndptrs->data, MEMORY_TRACING_PREFIX "Release vector of a metric cnode pointers sequence" );
    }
    CUBEW_FREE( this->optimal_cnodes_sequence_with_cndptrs, MEMORY_TRACING_PREFIX "Release a metric cnode pointers sequence" );
    CUBEW_FREE( this->expression, MEMORY_TRACING_PREFIX "Release a cubepl expression" );
    CUBEW_FREE( this->init_expression, MEMORY_TRACING_PREFIX "Release a cubepl init expression" );
    CUBEW_FREE( this->aggr_plus_expression, MEMORY_TRACING_PREFIX "Release a cubepl aggr plus expression" );
    CUBEW_FREE( this->aggr_minus_expression, MEMORY_TRACING_PREFIX "Release a cubepl aggr minus expression" );
    CUBEW_FREE( this->aggr_aggr_expression, MEMORY_TRACING_PREFIX "Release a cubepl \"aggregation over threads\" expression" );

    CUBEW_FREE( this, MEMORY_TRACING_PREFIX "Release a cube" );
}


/**
 * Closes file and writes index file.
 */
void
cube_metric_finish( cube_metric* this,
                    int          write_index )
{
    if ( this->im_finished == CUBE_TRUE )
    {
        return;
    }
    if ( this->data_file == NULL )
    {
        this->im_finished = CUBE_TRUE;
        return;
    }

    if ( write_index != 0 )
    {
        uint64_t endpos = ftell( this->data_file );
        cubew_fseeko( this->data_file, this->start_pos_of_datafile, 0 );
        if ( this->compressed == CUBE_FALSE )
        {
            fwrite( CUBE_DATAFILE_MARKER, 1, CUBE_DATAFILE_MARKER_SIZE,  this->data_file );
        }
        else
        {
            fwrite( CUBE_DATAFILE_COMPRESSED_MARKER, 1, CUBE_DATAFILE_COMPRESSED_MARKER_SIZE,  this->data_file );
            fwrite( &( this->n_compressed ), 1, sizeof( uint64_t ),
                    this->data_file );
            fwrite( this->sub_index, 1, this->subi_size,  this->data_file );
        }
        cubew_fseeko( this->data_file, endpos, 0 ); /* return to the end of file*/
    }
    if ( write_index != 0 )
    {
        cube_report_metric_data_finish( this->layout, this,  this->data_file );
    }
    else
    {
        cube_report_metric_data_weak_finish( this->layout, this,  this->data_file ); /* called by all others threads, which closes their data files*/
    }

    if ( write_index != 0 )
    {
        uint32_t  size_index     = 0;
        uint32_t* index_intofile = 0;
        if ( this->metric_format == CUBE_INDEX_FORMAT_SPARSE )
        {
            if ( this->known_cnodes != 0 )
            {
                size_index     = __cube_metric_size_of_index( this->known_cnodes, ( unsigned )ceil( ( double )this->ncn / 8. ) );
                index_intofile =  ( uint32_t* )__cube_metric_create_index( this->known_cnodes,  ( unsigned )ceil( ( double )this->ncn / 8. ) );
            }
        }
        else
        {
            /* nothing has to be writen in this case.*/
        }


        FILE*         ifile = cube_report_metric_index_start( this->layout, this );
        metric_header mheader;

        mheader.named.endian        = 1;
        mheader.named.version       = 0;
        mheader.named.metric_format = ( uint8_t )( this->metric_format );

        fwrite( CUBE_INDEXFILE_MARKER, 1, CUBE_INDEXFILE_MARKER_SIZE,  ifile );
        fwrite( &( mheader.named.endian ), 1, sizeof( uint32_t ),  ifile );
        fwrite( &( mheader.named.version ), 1, sizeof( uint16_t ),  ifile );
        fwrite( &( mheader.named.metric_format ), 1, sizeof( uint8_t ),  ifile );


        if ( this->metric_format == CUBE_INDEX_FORMAT_SPARSE )
        {
            fwrite( &( size_index ), 1, sizeof( uint32_t ), ifile );
            fwrite( ( char* )( index_intofile ), 1, size_index * sizeof( uint32_t ), ifile );
            CUBEW_FREE( index_intofile, MEMORY_TRACING_PREFIX "Release index_intofile" );
        }
        cube_report_metric_index_finish( this->layout, this,  ifile );
    }
    this->im_finished = CUBE_TRUE;
}






const char*
cube_metric_get_own_type( cube_metric* this )
{
    if ( this->metric_type == CUBE_METRIC_INCLUSIVE )
    {
        return "INCLUSIVE";
    }
    if ( this->metric_type == CUBE_METRIC_EXCLUSIVE )
    {
        return "EXCLUSIVE";
    }
    if ( this->metric_type == CUBE_METRIC_SIMPLE )
    {
        return "SIMPLE";
    }
    if ( this->metric_type == CUBE_METRIC_POSTDERIVED )
    {
        return "POSTDERIVED";
    }
    if ( this->metric_type == CUBE_METRIC_PREDERIVED_EXCLUSIVE )
    {
        return "PREDERIVED_EXCLUSIVE";
    }
    if ( this->metric_type == CUBE_METRIC_PREDERIVED_INCLUSIVE )
    {
        return "PREDERIVED_INCLUSIVE";
    }

    return "UNKNOWN";
}


const char*
cube_metric_get_own_viz_type( cube_metric* this )
{
    if ( this->metric_viz_type == CUBE_METRIC_NORMAL )
    {
        return "NORMAL";
    }
    if ( this->metric_viz_type == CUBE_METRIC_GHOST )
    {
        return "GHOST";
    }
    return "NORMAL";
}


void
cube_metric_set_cacheable( cube_metric* metric,
                           enum bool_t  cacheable )
{
    metric->is_cacheable = cacheable;
}

enum bool_t
cube_metric_is_cacheable( cube_metric* metric )
{
    return metric->is_cacheable;
}




void
cube_metric_set_known_cnodes( cube_metric* metric,
                              char*        known_cnodes /*, unsigned size*/ )
{
    metric->metric_format = ( known_cnodes    == NULL ) ? CUBE_INDEX_FORMAT_DENSE : CUBE_INDEX_FORMAT_SPARSE;
    if ( known_cnodes == 0 )
    {
        UTILS_WARNING(  "Failed to set a bit vector of known cnodes. Received pointer is zero.\n" );
    }
    CUBEW_FREE( metric->known_cnodes, MEMORY_TRACING_PREFIX "Release previos list of known cnodes" );


    known_cnodes         = __cube_metric_bitstring_transformation( metric, known_cnodes );
    metric->known_cnodes = known_cnodes;
#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    if ( metric->compressed == CUBE_TRUE )
    {
        cube_metric_setup_subindex( metric );
    }
#endif  /* HAVE_LIB_Z */
}






cube_metric*
cube_metric_get_child( cube_metric* this,
                       int          i )
{
    if ( i < 0 || i >= this->child->size )
    {
        UTILS_WARNING( "cube_metric_get_child: out of range\n" );
    }
    return this->child->data[ i ];
}

cube_metric*
cube_metric_get_parent( cube_metric* this )
{
    return this->parent;
}

char*
cube_metric_get_disp_name( cube_metric* this )
{
    return this->disp_name;
}

char*
cube_metric_get_uniq_name( cube_metric* this )
{
    return this->uniq_name;
}

char*
cube_metric_get_dtype( cube_metric* this )
{
    return this->dtype;
}

char*
cube_metric_get_uom( cube_metric* this )
{
    return this->uom;
}

char*
cube_metric_get_val( cube_metric* this )
{
    return this->val;
}

char*
cube_metric_get_descr( cube_metric* this )
{
    return this->descr;
}

char*
cube_metric_get_url( cube_metric* this )
{
    return this->url;
}

/**
 * Writes a XML output belongs to the metric in to .cube file.
 * Does it recursiv with "deep search" algorithm.
 */
void
cube_metric_writeXML( cube_metric*           this,
                      cube_meta_data_writer* meta_data_writer )
{
    char* _disp_name             = ( char* )__cube_services_escape_to_xml( cube_metric_get_disp_name( this ) );
    char* _uniq_name             = ( char* )__cube_services_escape_to_xml( cube_metric_get_uniq_name( this ) );
    char* _dtype                 = ( char* )__cube_services_escape_to_xml( cube_metric_get_dtype( this ) );
    char* _uom                   = ( char* )__cube_services_escape_to_xml( cube_metric_get_uom( this ) );
    char* _val                   = ( char* )__cube_services_escape_to_xml( cube_metric_get_val( this ) );
    char* _url                   = ( char* )__cube_services_escape_to_xml( cube_metric_get_url( this ) );
    char* _descr                 = ( char* )__cube_services_escape_to_xml( cube_metric_get_descr( this ) );
    char* _expression            = ( char* )__cube_services_escape_to_xml( cube_metric_get_expression( this ) );
    char* _init_expression       = ( char* )__cube_services_escape_to_xml( cube_metric_get_init_expression( this ) );
    char* _aggr_plus_expression  = ( char* )__cube_services_escape_to_xml( cube_metric_get_aggr_expression( this, CUBE_METRIC_AGGR_PLUS ) );
    char* _aggr_minus_expression = ( char* )__cube_services_escape_to_xml( cube_metric_get_aggr_expression( this, CUBE_METRIC_AGGR_MINUS ) );
    char* _aggr_aggr_expression  = ( char* )__cube_services_escape_to_xml( cube_metric_get_aggr_expression( this, CUBE_METRIC_AGGR_AGGR ) );
    int   i                      = 0;

    unsigned    _id           = cube_metric_get_id( this );
    const char* _own_type     = cube_metric_get_own_type( this );
    const char* _own_viz_type = cube_metric_get_own_viz_type( this );

    __cube_write_meta_data( meta_data_writer, "<metric id=\"%u\" type=\"%s\"", _id, _own_type );
    if ( this->metric_viz_type != CUBE_METRIC_NORMAL )
    {
        __cube_write_meta_data( meta_data_writer, " viztype=\"%s\"", _own_viz_type );
    }
    if ( this->is_cacheable == CUBE_FALSE )
    {
        __cube_write_meta_data( meta_data_writer, " cacheable=\"false\"" );
    }
    __cube_write_meta_data( meta_data_writer, ">\n" );
    __cube_write_meta_data( meta_data_writer, "<disp_name>%s</disp_name>\n",  _disp_name ? _disp_name : cube_metric_get_disp_name( this ) );
    __cube_write_meta_data( meta_data_writer, "<uniq_name>%s</uniq_name>\n", _uniq_name ? _uniq_name : cube_metric_get_uniq_name( this ) );
    __cube_write_meta_data( meta_data_writer, "<dtype>%s</dtype>\n",  _dtype ? _dtype : cube_metric_get_dtype( this ) );
    __cube_write_meta_data( meta_data_writer, "<uom>%s</uom>\n", _uom ? _uom : cube_metric_get_uom( this ) );
    if ( strcmp( cube_metric_get_val( this ), "" ) != 0 )
    {
        __cube_write_meta_data( meta_data_writer, "<val>%s</val>\n", _val ? _val : cube_metric_get_val( this ) );
    }
    __cube_write_meta_data( meta_data_writer, "<url>%s</url>\n", _url ? _url : cube_metric_get_url( this ) );
    __cube_write_meta_data( meta_data_writer, "<descr>%s</descr>\n", _descr ? _descr : cube_metric_get_descr( this ) );
    if ( this->metric_type == CUBE_METRIC_POSTDERIVED  ||   this->metric_type == CUBE_METRIC_PREDERIVED_INCLUSIVE  ||  this->metric_type == CUBE_METRIC_PREDERIVED_EXCLUSIVE )
    {
        if ( _expression != NULL )
        {
            if ( this->locationwise == CUBE_FALSE )
            {
                __cube_write_meta_data( meta_data_writer, "<cubepl locationwise=false>%s</cubepl>\n", _expression  );
            }
            else
            {
                __cube_write_meta_data( meta_data_writer, "<cubepl>%s</cubepl>\n", _expression  );
            }
        }
        if ( _init_expression != NULL )
        {
            __cube_write_meta_data( meta_data_writer, "<cubeplinit>%s</cubeplinit>\n", _init_expression );
        }
        if ( _aggr_plus_expression != NULL )
        {
            __cube_write_meta_data( meta_data_writer, "<cubeplaggr cubeplaggrtype=plus>%s</cubeplaggr>\n", _aggr_plus_expression );
        }
        if ( _aggr_minus_expression != NULL )
        {
            __cube_write_meta_data( meta_data_writer, "<cubeplaggr cubeplaggrtype=minus>%s</cubeplaggr>\n", _aggr_minus_expression );
        }
        if ( _aggr_aggr_expression != NULL )
        {
            __cube_write_meta_data( meta_data_writer, "<cubeplaggr cubeplaggrtype=aggr>%s</cubeplaggr>\n", _aggr_aggr_expression );
        }
    }

    __cube_services_write_attributes( meta_data_writer,  this->attr );


    for ( i = 0; i < cube_metric_num_children( this ); i++ )
    {
        cube_metric* p = cube_metric_get_child( this, i );
        cube_metric_writeXML( p, meta_data_writer );
    }
    __cube_write_meta_data( meta_data_writer, "</metric>\n" );

    CUBEW_FREE( _disp_name, MEMORY_TRACING_PREFIX "Release _disp_name" );
    CUBEW_FREE( _uniq_name, MEMORY_TRACING_PREFIX "Release _uniq_name" );
    CUBEW_FREE( _dtype, MEMORY_TRACING_PREFIX "Release _dtype" );
    CUBEW_FREE( _uom, MEMORY_TRACING_PREFIX "Release _uom" );
    CUBEW_FREE( _val, MEMORY_TRACING_PREFIX "Release _val" );
    CUBEW_FREE( _url, MEMORY_TRACING_PREFIX "Release _url" );
    CUBEW_FREE( _descr, MEMORY_TRACING_PREFIX "Release _descr" );
    CUBEW_FREE( _expression, MEMORY_TRACING_PREFIX "Release _expression" );
    CUBEW_FREE( _init_expression, MEMORY_TRACING_PREFIX "Release _init_expression" );
    CUBEW_FREE( _aggr_plus_expression, MEMORY_TRACING_PREFIX "Release _aggr_plus_expression" );
    CUBEW_FREE( _aggr_minus_expression, MEMORY_TRACING_PREFIX "Release _aggr_minus_expression" );
    CUBEW_FREE( _aggr_aggr_expression, MEMORY_TRACING_PREFIX "Release _aggr_aggr_expression" );
}

int
cube_metric_num_children( cube_metric* this )
{
    return this->child->size;
}

/**
 * Gives a level of the metric. Root -> 0.
 * Does it recursiv with "deep search" algorithm.
 */
int
cube_metric_get_level( cube_metric* this )
{
    if ( cube_metric_get_parent( this ) == NULL )
    {
        return 0;
    }
    else
    {
        return cube_metric_get_level( cube_metric_get_parent( this ) ) + 1;
    }
}

void
cube_metric_set_id( cube_metric* this,
                    int          new_id )
{
    this->id = new_id;
}

int
cube_metric_get_id( cube_metric* this )
{
    return this->id;
}

/**
 * Compares equality of two metrcs.
 */
int
cube_metric_equal( cube_metric* a,
                   cube_metric* b )
{
    const char* _a = cube_metric_get_uniq_name( a );
    const char* _b = cube_metric_get_uniq_name( b );
    if ( strcmp( _a, _b ) == 0 )
    {
        return 1;
    }
    return 0;
}
/**
 * Just sets a given unique name of the metric.
 */
void
cube_metric_set_uniq_name( cube_metric* this,
                           char*        uniq_name )
{
    this->uniq_name = uniq_name;
}








/*  ====================== Writing routines ===================================*/


void
cube_metric_write_row( cube_metric* metric,
                       cube_cnode*  cnd,
                       void*        data_row )
{
    if ( metric->metric_type == CUBE_METRIC_POSTDERIVED ||  metric->metric_type == CUBE_METRIC_PREDERIVED_INCLUSIVE ||  metric->metric_type == CUBE_METRIC_PREDERIVED_EXCLUSIVE ) // DERIVATED metrics do not store any data.
    {
        return;
    }
    /* signalize that metric gets a row.*/
    metric->im_writing = CUBE_TRUE;
    cube_report_write_metric_row( metric->layout, metric );

    /* metric dependend enumeration */
    unsigned* array = ( unsigned* )( metric->local_cnode_enumeration->data );
    unsigned  cid   = array[ cnd->id ];                                                                              // they are unsigned

    if (  metric->metric_format == CUBE_INDEX_FORMAT_SPARSE && __cube_is_bit_set( metric->known_cnodes, cid ) == 0 ) /* is one tries to write row on cnode with no data - ignore*/
    {
        return;
    }

    /* bookkeeping about written cnodes. Neede to create tared file*/
    if ( metric->written_cnodes == 0 )
    {
        metric->written_cnodes = ( char* )CUBEW_CALLOC( ( unsigned )ceil( ( double )metric->ncn / 8. ), 1, MEMORY_TRACING_PREFIX "Allocate list of written cnodes" );
    }
    __cube_set_bit( metric->written_cnodes,   cid );

    /* end of bookkeeping code */

    uint32_t position  = cube_metric_get_position_of_row( metric, cid );  // in units of value;
    int      unit_size = __cube_metric_size_dtype(  metric->dtype_params );

    off_t    raw_position = unit_size * ( uint64_t )position;
    uint64_t row_size     = unit_size * ( metric->nthrd );


    if ( metric->compressed == CUBE_FALSE )         // if there no compression - then we write directly
    {
        // if there is no need to seek, we avoid seek. Performance reasons.
        if ( metric->last_seek_position != raw_position )
        {
            cubew_fseeko( metric->data_file, metric->start_pos_of_datafile + raw_position + CUBE_DATAFILE_MARKER_SIZE, 0 );
        }

        fwrite( data_row, 1, row_size, metric->data_file );
        metric->last_seek_position = raw_position + row_size;
    }
#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    else
    {
        uint32_t n_row           = position / metric->nthrd;                  // number of row in the data file .
        uLongf   size_compressed = compressBound( row_size );
        compress2( metric->compression_buffer,   &size_compressed, ( Bytef* )( data_row ), row_size, Z_BEST_SPEED );
        uint64_t start_uncompressed = raw_position;
        uint64_t start_compressed   = metric->incr_compressed;
        metric->incr_compressed           += size_compressed;
        metric->sub_index[ 3 * n_row ]     = start_uncompressed;
        metric->sub_index[ 3 * n_row + 1 ] = start_compressed;
        metric->sub_index[ 3 * n_row + 2 ] = size_compressed;
        off_t writing_offset = ( metric->subi_size )  + CUBE_DATAFILE_COMPRESSED_MARKER_SIZE  + ( uint64_t )sizeof( uint64_t ) + start_compressed;
        if ( metric->last_seek_position != writing_offset )
        {
            cubew_fseeko( metric->data_file, metric->start_pos_of_datafile + writing_offset, 0 );
        }
        fwrite( metric->compression_buffer, 1, size_compressed, metric->data_file );
        metric->last_seek_position = writing_offset + size_compressed;
    }
    #endif /* HAVE_LIB_Z */
}



void
cube_metric_write_row_of_doubles( cube_metric* metric,
                                  cube_cnode*  cnd,
                                  double*      data_row )
{
    /* cube_metric_trasnsforms_... always allocate new memory. If no transformation -> then copy*/

    void* target_row = ( void* )cube_metric_transform_row_of_doubles( metric, data_row );

    cube_metric_write_row( metric, cnd, target_row );
    /* release allocated memory*/
    CUBEW_FREE( target_row, MEMORY_TRACING_PREFIX "Release row of doubles" );
}

void
cube_metric_write_row_of_uint64( cube_metric* metric,
                                 cube_cnode*  cnd,
                                 uint64_t*    data_row )
{
    /* cube_metric_trasnsforms_... always allocate new memory. If no transformation -> then copy*/
    void* target_row = ( void* )cube_metric_transform_row_of_uint64( metric, data_row );

    cube_metric_write_row( metric, cnd, target_row );
    /* release allocated memory*/
    CUBEW_FREE( target_row, MEMORY_TRACING_PREFIX "Release row of uint64" );
}


void
cube_metric_write_row_of_int64( cube_metric* metric,
                                cube_cnode*  cnd,
                                int64_t*     data_row )
{
    /* cube_metric_trasnsforms_... always allocate new memory. If no transformation -> then copy*/
    void* target_row = ( void* )cube_metric_transform_row_of_int64( metric, data_row );

    cube_metric_write_row( metric, cnd, target_row );
    /* release allocated memory*/
    CUBEW_FREE( target_row, MEMORY_TRACING_PREFIX "Release row of int64"  );
}





/* List of the canonical writing calls. They perform type casting if needed and check metric if it supports that kind of value. Internally they call cube_write_sev_row(...)*/

void
cube_metric_write_row_of_cube_type_uint8( cube_metric* metric,  cube_cnode* cnd, cube_type_uint8* sevs )
{
    if ( CUBE_DATA_TYPE_UINT8 != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}


void
cube_metric_write_row_of_cube_type_int8( cube_metric* metric,  cube_cnode* cnd, cube_type_int8* sevs )
{
    if ( CUBE_DATA_TYPE_INT8 != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}


void
cube_metric_write_row_of_cube_type_uint16( cube_metric* metric,  cube_cnode* cnd, cube_type_uint16* sevs )
{
    if ( CUBE_DATA_TYPE_UINT16 != metric->dtype_params->type )
    {
        return;
    }

    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_int16( cube_metric* metric,  cube_cnode* cnd, cube_type_int16* sevs )
{
    if ( CUBE_DATA_TYPE_INT16 != metric->dtype_params->type )
    {
        return;
    }

    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_uint32( cube_metric* metric,  cube_cnode* cnd, cube_type_uint32* sevs )
{
    if ( CUBE_DATA_TYPE_UINT32 != metric->dtype_params->type )
    {
        return;
    }

    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_int32( cube_metric* metric,  cube_cnode* cnd, cube_type_int32* sevs )
{
    if ( CUBE_DATA_TYPE_INT32 != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_uint64( cube_metric* metric,  cube_cnode* cnd, cube_type_uint64* sevs )
{
    if ( CUBE_DATA_TYPE_UINT64 != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_int64( cube_metric* metric,  cube_cnode* cnd, cube_type_int64* sevs )
{
    if ( CUBE_DATA_TYPE_INT64 != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_double( cube_metric* metric,  cube_cnode* cnd, cube_type_double* sevs )
{
    if ( CUBE_DATA_TYPE_DOUBLE != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_tau_atomic( cube_metric* metric,  cube_cnode* cnd, cube_type_tau_atomic* sevs )
{
    if ( CUBE_DATA_TYPE_TAU_ATOMIC != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_scale_func( cube_metric* metric,  cube_cnode* cnd, cube_type_scale_func* sevs )
{
    if ( CUBE_DATA_TYPE_SCALE_FUNC != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_complex( cube_metric* metric,  cube_cnode* cnd, cube_type_complex* sevs )
{
    if ( CUBE_DATA_TYPE_COMPLEX != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}

void
cube_metric_write_row_of_cube_type_rate( cube_metric* metric,  cube_cnode* cnd, cube_type_rate* sevs )
{
    if ( CUBE_DATA_TYPE_RATE != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}


void
cube_metric_write_row_of_cube_type_histogram( cube_metric* metric,  cube_cnode* cnd, cube_type_double* sevs )
{
    if ( CUBE_DATA_TYPE_HISTOGRAM != metric->dtype_params->type )
    {
        return;
    }
    cube_metric_write_row( metric, cnd, sevs );
}
