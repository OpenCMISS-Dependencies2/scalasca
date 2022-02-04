/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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
 * \file cubew_services.c
   \brief Provides all kind of service functions. Cannot be linked from libcubew.a.
 */
#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>

#include "cubew_services.h"
#include "cubew_meta_data_writer.h"
#include "cubew_types.h"
#include "cubew_vector.h"
#include "cubew_memory.h"

#include "UTILS_Error.h"

// #include <dir.h>
#include "cubew_compat_platform.h"

#define MEMORY_TRACING_PREFIX "[SERVICES]"

#ifndef PRIu64
#define PRIu64 "llu"
#endif


#ifndef PRIu32
#define PRIu32 "lu"
#endif

enum DataType
__cube_metric_dtype2data_type( const char* dtype )
{
    if ( strcmp( dtype, "FLOAT" ) == 0  ||
         strcmp( dtype, "DOUBLE" ) == 0 )
    {
        return CUBE_DATA_TYPE_DOUBLE;
    }
    if ( strcmp( dtype, "INTEGER" ) == 0 ||
         strcmp( dtype, "INT64" ) == 0 )
    {
        return CUBE_DATA_TYPE_INT64;
    }
    if ( strcmp( dtype, "UINT64" ) == 0 )
    {
        return CUBE_DATA_TYPE_UINT64;
    }
    if ( strcmp( dtype, "INT8" ) == 0 )
    {
        return CUBE_DATA_TYPE_INT8;
    }
    if ( strcmp( dtype, "UINT8" ) == 0 ||
         strcmp( dtype, "CHAR" ) == 0 )
    {
        return CUBE_DATA_TYPE_UINT8;
    }
    if ( strcmp( dtype, "INT16" ) == 0  ||
         strcmp( dtype, "SHORT INT" ) == 0 )
    {
        return CUBE_DATA_TYPE_INT16;
    }
    if ( strcmp( dtype, "UINT16" ) == 0  ||
         strcmp( dtype, "SHORT UNSIGNED INT" ) == 0 )
    {
        return CUBE_DATA_TYPE_UINT16;
    }
    if ( strcmp( dtype, "INT32" ) == 0  ||
         strcmp( dtype, "INT" ) == 0 )
    {
        return CUBE_DATA_TYPE_INT32;
    }
    if ( strcmp( dtype, "UINT32" ) == 0  ||
         strcmp( dtype, "UNSIGNED INT" ) == 0 )
    {
        return CUBE_DATA_TYPE_UINT32;
    }

    if ( strcmp( dtype, "COMPLEX" ) == 0 )
    {
        return CUBE_DATA_TYPE_COMPLEX;
    }
    if ( strcmp( dtype, "TAU_ATOMIC" ) == 0 )
    {
        return CUBE_DATA_TYPE_TAU_ATOMIC;
    }
    if ( strcmp( dtype, "MINDOUBLE" ) == 0 )
    {
        return CUBE_DATA_TYPE_MIN_DOUBLE;
    }
    if ( strcmp( dtype, "MAXDOUBLE" ) == 0 )
    {
        return CUBE_DATA_TYPE_MAX_DOUBLE;
    }
    if ( strcmp( dtype, "RATE" ) == 0 )
    {
        return CUBE_DATA_TYPE_RATE;
    }
    if ( strcmp( dtype, "SCALE_FUNC" ) == 0 )
    {
        return CUBE_DATA_TYPE_SCALE_FUNC;
    }
    if ( strcmp( dtype, "HISTOGRAM" ) == 0 )
    {
        return CUBE_DATA_TYPE_HISTOGRAM;
    }
    return CUBE_DATA_TYPE_UNKNOWN;
}



int
__cube_metric_size_dtype( cube_value_param_array* dtype )
{
    if ( dtype->type == CUBE_DATA_TYPE_INT64 )
    {
        return sizeof( cube_type_int64 );
    }
    if ( dtype->type == CUBE_DATA_TYPE_UINT64 )
    {
        return sizeof( cube_type_uint64 );
    }
    if ( dtype->type == CUBE_DATA_TYPE_DOUBLE )
    {
        return sizeof( cube_type_double );
    }
    if ( dtype->type == CUBE_DATA_TYPE_INT8 )
    {
        return sizeof( cube_type_int8 );
    }
    if ( dtype->type == CUBE_DATA_TYPE_UINT8 )
    {
        return sizeof( cube_type_uint8 );
    }
    if ( dtype->type == CUBE_DATA_TYPE_INT16 )
    {
        return sizeof( cube_type_int16 );
    }
    if ( dtype->type == CUBE_DATA_TYPE_UINT16 )
    {
        return sizeof( cube_type_uint16 );
    }
    if ( dtype->type == CUBE_DATA_TYPE_INT32 )
    {
        return sizeof( cube_type_int32 );
    }
    if ( dtype->type == CUBE_DATA_TYPE_UINT32 )
    {
        return sizeof( cube_type_uint32 );
    }
    if ( dtype->type == CUBE_DATA_TYPE_COMPLEX )
    {
        return
            sizeof( cube_type_double ) +
            sizeof( cube_type_double )
        ;
    }
    if ( dtype->type == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        return sizeof( cube_type_int32 ) +
               sizeof( cube_type_double ) +
               sizeof( cube_type_double ) +
               sizeof( cube_type_double ) +
               sizeof( cube_type_double )
        ;
    }
    if ( dtype->type == CUBE_DATA_TYPE_SCALE_FUNC )
    {
        return 27 * sizeof( cube_type_double )
        ;
    }
    if ( dtype->type == CUBE_DATA_TYPE_MIN_DOUBLE )
    {
        return sizeof( cube_type_double );
    }
    if ( dtype->type == CUBE_DATA_TYPE_MAX_DOUBLE )
    {
        return sizeof( cube_type_double );
    }
    if ( dtype->type == CUBE_DATA_TYPE_RATE )
    {
        return sizeof( cube_type_double ) +
               sizeof( cube_type_double );
    }
    if ( dtype->type == CUBE_DATA_TYPE_HISTOGRAM )
    {
        return dtype->data[ 0 ] * sizeof( cube_type_double ) + 2 * sizeof( cube_type_double );
    }
    if ( dtype->type == CUBE_DATA_TYPE_NDOUBLES )
    {
        return dtype->data[ 0 ] * sizeof( cube_type_double );
    }
    UTILS_WARNING( "[CUBEW Warning]: Selected type of values (%u) in metric is unknown. Return size of it as 0\n", ( unsigned )( dtype->type ) );
    return 0;
}




char*
__cube_metric_dtype2string( cube_value_param_array* dtype )
{
    if ( dtype->type == CUBE_DATA_TYPE_INT64 )
    {
        return "INTEGER";
    }
    if ( dtype->type == CUBE_DATA_TYPE_UINT64 )
    {
        return "UINT64";
    }
    if ( dtype->type == CUBE_DATA_TYPE_DOUBLE )
    {
        return "FLOAT";
    }
    if ( dtype->type == CUBE_DATA_TYPE_INT8 )
    {
        return "INT8";
    }
    if ( dtype->type == CUBE_DATA_TYPE_UINT8 )
    {
        return "UINT8";
    }
    if ( dtype->type == CUBE_DATA_TYPE_INT16 )
    {
        return "INT16";
    }
    if ( dtype->type == CUBE_DATA_TYPE_UINT16 )
    {
        return "UINT16";
    }
    if ( dtype->type == CUBE_DATA_TYPE_INT32 )
    {
        return "INT32";
    }
    if ( dtype->type == CUBE_DATA_TYPE_UINT32 )
    {
        return "UINT32";
    }
    if ( dtype->type == CUBE_DATA_TYPE_COMPLEX )
    {
        return "COMPLEX";
    }
    if ( dtype->type == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        return "TAU_ATOMIC";
    }
    if ( dtype->type == CUBE_DATA_TYPE_MIN_DOUBLE )
    {
        return "MINDOUBLE";
    }
    if ( dtype->type == CUBE_DATA_TYPE_MAX_DOUBLE )
    {
        return "MAXDOUBLE";
    }
    if ( dtype->type == CUBE_DATA_TYPE_RATE )
    {
        return "RATE";
    }
    if ( dtype->type == CUBE_DATA_TYPE_SCALE_FUNC )
    {
        return "SCALE_FUNC";
    }
    if ( dtype->type == CUBE_DATA_TYPE_HISTOGRAM )
    {
        int   len     = snprintf( NULL, 0, "HISTOGRAM(%i)", dtype->data[ 0 ] );
        char* _string = ( char* )CUBEW_CALLOC( len, 1, MEMORY_TRACING_PREFIX "Allocate string for histogram definition"  );
        sprintf( _string, "HISTOGRAM(%i)", dtype->data[ 0 ] );
        return _string;
    }
    if ( dtype->type == CUBE_DATA_TYPE_NDOUBLES )
    {
        int   len     = snprintf( NULL, 0, "NDOUBLES(%i)", dtype->data[ 0 ] );
        char* _string = ( char* )CUBEW_CALLOC( len, 1, MEMORY_TRACING_PREFIX "Allocate string for ndouble definition"  );
        sprintf( _string, "NDOUBLES(%i)", dtype->data[ 0 ] );
        return _string;
    }
    UTILS_WARNING( "[CUBEW Warning]: Selected type of values (%u) in metric is unknown. Return \"UNKNOWN\" name\n", ( unsigned )( dtype->type ) );
    return "UNKNOWN";
}


cube_value_param_array*
__cube_metric_dtype_param_split( const char* dtype  )
{
    cube_value_param_array* _type = ( cube_value_param_array* )CUBEW_CALLOC( sizeof( cube_value_param_array ), 1, MEMORY_TRACING_PREFIX "Allocate array for the data type parameters" );

    char* total_dtype = cubew_strdup( dtype );
    char* token1;
    char* token;
    token1      = strtok( total_dtype, "(,)" );
    _type->type = __cube_metric_dtype2data_type( token1 );
    token       = token1;
    while ( token != NULL )
    {
        token = strtok( NULL, "(,)" );
        if ( token == NULL )
        {
            break;
        }

        uint32_t _value = atoi( token );
        ADD_NEXT( _type, _value, uint32_t, MEMORY_TRACING_PREFIX "Add next parameter to the data type" );
    }
    CUBEW_FREE( total_dtype, MEMORY_TRACING_PREFIX "Release total data type" );
    return _type;
}

void
__cube_value_param_free( cube_value_param_array* _array )
{
    CUBEW_FREE( _array->data, MEMORY_TRACING_PREFIX "Release vector of parameters of a data type" );
    CUBEW_FREE( _array->dtype, MEMORY_TRACING_PREFIX "Release string definition of a data type" );
    CUBEW_FREE( _array, MEMORY_TRACING_PREFIX "Release parameters of a data type" );
}




/* uint32_t assumes, that one can have only 4294967295 cnodes in calltree
 */
uint32_t
__cube_metric_bit_position( char*    bit_vector,
                            unsigned vlen,
                            int      cid )
{
    const char bits[ 256 ] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4,
        4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
        4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4,
        4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4,
        4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5,
        4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4,
        4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };

    if ( vlen * 8 < cid )
    {
        /* position is out of lengts of bit vector */
        UTILS_WARNING( "[CUBEW Fatal Error]: Position %d is out of bit vector with length %u. Resulted Cube file will be invalid.\n", cid, vlen * 8  );
        exit( -1 );
    }
    ;
    unsigned array_pos = cid / 8;
    unsigned bits_pos  = cid % 8;
    uint32_t sum       = 0;

    unsigned i;
    for ( i = 0; i < array_pos; i++ )
    {
        char     itmp = bit_vector[ i ];
        unsigned utmp = ( unsigned )( 0x000000FF & itmp );
        sum += ( uint32_t )bits[ utmp ];
    }
    for ( i = 0; i < bits_pos; i++ )
    {
        sum += ( uint32_t )( ( bit_vector[ array_pos ] >> ( 7 - i ) ) & 0x01 );
    }
    return sum;
}


size_t
__cube_num_of_set_bits( char*    bit_vector,
                        unsigned vlen
                        )
{
    static const int bits[ 256 ] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3,
        4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4,
        4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2,
        3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
        4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3,
        4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3,
        3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5,
        6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5,
        6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };
    size_t           number = 0;
    int              i      = 0;
    for ( i = 0; i < vlen; ++i )
    {
        number += bits[ ( uint8_t )bit_vector[ i ] ];
    }
    return number;
}




uint32_t
__cube_metric_size_of_index( char*    bit_vector,
                             unsigned vlen )
{
    return __cube_metric_bit_position( bit_vector, vlen, vlen * 8 );  /*  position of the last bit... plus one */
}





uint32_t*
__cube_metric_create_index( char*    bit_vector,
                            unsigned vlen )
{
    uint32_t  isize = __cube_metric_size_of_index( bit_vector, vlen );
    uint32_t* index = ( uint32_t* )CUBEW_MALLOC( isize * sizeof( uint32_t ), MEMORY_TRACING_PREFIX "Allocate metric index" );
    memset( index, 0, isize * sizeof( uint32_t ) );
    unsigned i;
    unsigned j;
    for ( i = 0, j = 0; i < vlen * 8; i++ )
    {
        if ( __cube_is_bit_set( bit_vector,  i ) != 0 )
        {
            index[ j ] = i;
            j++;
        }
    }
    return index;
}



unsigned
__cube_is_bit_set( char* bit_vector,
                   int   bitposition )
{
    if ( bit_vector == NULL )
    {
        return 0;
    }
    unsigned      array_pos = bitposition / 8;
    unsigned char bits_pos  = ( unsigned char )( bitposition % 8 );
    unsigned char _mask     = ( ( unsigned char )( ( unsigned char )( 0x80 ) >> bits_pos ) );
    unsigned char _a        = bit_vector[ array_pos ];
    unsigned      test      = ( unsigned )( _a & _mask );
    return test;
}


void
__cube_set_bit( char* bit_vector,
                int   bitposition )
{
    if ( bit_vector == NULL )
    {
        return;
    }
    unsigned      array_pos = bitposition / 8;
    unsigned char bits_pos  = ( unsigned char )( bitposition % 8 );
    unsigned char _mask     = ( ( unsigned char )( ( unsigned char )( 0x80 ) >> bits_pos ) );
    unsigned char _a        = bit_vector[ array_pos ];
    bit_vector[ array_pos ] = ( char )( _a | _mask );
}


int
__cube_metric_last_bit_position( char*    bit_vector,
                                 unsigned vlen
                                 )
{
    static const int bits[ 256 ] = { -1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };
    int              position    = vlen * 8 - 1;
    int              i           = vlen;
    for ( i = vlen - 1; i >= 0; i-- )
    {
        if ( bit_vector[ i ] == 0 )
        {
            position -= 8;
            continue;
        }
        break;
    }
    position -= bits[ 0x0000000FF & bit_vector[ i ] ];
    return position;
}


uint64_t*
__cube_transform_doubles_into_uint64( double*  data_row,
                                      uint32_t size )
{
    uint64_t* target_row = ( uint64_t* )CUBEW_MALLOC( size * sizeof( uint64_t ), MEMORY_TRACING_PREFIX "Allocate target row to convert doubles into uint64_t" );
    unsigned  i          = 0;
    for ( i = 0; i < size; i++ )
    {
        target_row[ i ] = ( uint64_t )data_row[ i ];
    }
    return target_row;
}


int64_t*
__cube_transform_doubles_into_int64( double*  data_row,
                                     uint32_t size )
{
    int64_t* target_row = ( int64_t* )CUBEW_MALLOC( size * sizeof( int64_t ), MEMORY_TRACING_PREFIX "Allocate target row to convert doubles into int64_t" );
    unsigned i          = 0;
    for ( i = 0; i < size; i++ )
    {
        target_row[ i ] = ( int64_t )( data_row[ i ] );
    }
    return target_row;
}



double*
__cube_transform_uint64_into_doubles( uint64_t* data_row,
                                      uint32_t  size )
{
    double*  target_row = ( double* )CUBEW_MALLOC( size * sizeof( double ), MEMORY_TRACING_PREFIX "Allocate target row to convert uint64_t into doubles" );
    unsigned i          = 0;
    for ( i = 0; i < size; i++ )
    {
        target_row[ i ] = ( double )data_row[ i ];
    }
    return target_row;
}



double*
__cube_transform_int64_into_doubles( int64_t* data_row,
                                     uint32_t size )
{
    double*  target_row = ( double* )CUBEW_MALLOC( size * sizeof( double ), MEMORY_TRACING_PREFIX "Allocate target row to convert int64_t into doubles" );
    unsigned i          = 0;
    for ( i = 0; i < size; i++ )
    {
        target_row[ i ] = ( double )data_row[ i ];
    }
    return target_row;
}



void*
__cube_transform_row_of_values( cube_value_param_array* from, cube_value_param_array* to,
                                void*  row,
                                uint32_t size  )

{
    uint32_t to_sizeof   = __cube_metric_size_dtype( to );
    uint32_t from_sizeof = __cube_metric_size_dtype( from );

    char transformed_value[ to_sizeof ];
    memset( transformed_value, 0, to_sizeof ); /*  setting to zero target memory... should be "zero" element of the monoid or group*/

    void* target_row = CUBEW_CALLOC( size,  to_sizeof, MEMORY_TRACING_PREFIX "Allocate target row to provide a raw row" );



    switch ( __cube_type_compatibility_check( from, to ) )
    {
        case CUBE_VALUE_SAME:                   /* no transformation is needed*/
            memcpy( target_row, row, size * to_sizeof );
            return target_row;
            break;
        case CUBE_VALUE_INCOMPATIBLE:                                                                    /*  transformation is inpossible, just return empty row*/
            __cube_initialize_row_of_memory( to, CUBE_ZERO_INIT,  target_row,  size * to_sizeof, NULL ); /* size -> in bytes */
            return target_row;
            break;
        case CUBE_VALUE_COMPATIBLE:
        case CUBE_VALUE_PRECISION_LOST:
        case CUBE_VALUE_RANGE_LOST:
        case CUBE_VALUE_MEANING_LOST: /*  in all those four cases transformation is needed, therefore switch doesn't return, but proceed execution of this routine*/
            break;
        case CUBE_VALUE_UNKNOWN_RELATION:
        default:
            return target_row;
            break;
    }

    uint32_t i = 0;
    for ( i = 0; i < size; i++ )
    {
        __cube_value_transform( from, to, transformed_value, ( uint8_t* )row + i * from_sizeof, to_sizeof, from_sizeof );
        memcpy( ( uint8_t* )target_row + i * to_sizeof, transformed_value, to_sizeof );
//     target_row[ i ] = ( double )data_row[ i ];
    }
    return target_row;
}





void
__cube_value_transform( cube_value_param_array* from, cube_value_param_array* to, void* target_place_for_value, void* source, uint32_t to_sizeof, uint32_t from_sizeof )
{
    memset( target_place_for_value, 0, to_sizeof );
    uint8_t  uint8_tmp  = 0;
    int8_t   int8_tmp   = 0;
    uint16_t uint16_tmp = 0;
    int16_t  int16_tmp  = 0;
    uint32_t uint32_tmp = 0;
    int32_t  int32_tmp  = 0;
    uint64_t uint64_tmp = 0;
    int64_t  int64_tmp  = 0;
    double   double_tmp = 0;


    switch ( from->type )
    {
        case CUBE_DATA_TYPE_UINT8:
            uint8_tmp  = ( uint8_t )( *( ( uint8_t* )source ) );
            int8_tmp   = ( int8_t )( *( ( uint8_t* )source ) );
            uint16_tmp = ( uint16_t )( *( ( uint8_t* )source ) );
            int16_tmp  = ( int16_t )( *( ( uint8_t* )source ) );
            uint32_tmp = ( uint32_t )( *( ( uint8_t* )source ) );
            int32_tmp  = ( int32_t )( *( ( uint8_t* )source ) );
            uint64_tmp = ( uint64_t )( *( ( uint8_t* )source ) );
            int64_tmp  = ( int64_t )( *( ( uint8_t* )source ) );
            double_tmp = ( double )( *( ( uint8_t* )source ) );
            break;
        case CUBE_DATA_TYPE_INT8:
            uint8_tmp  = ( uint8_t )( *( ( int8_t* )source ) );
            int8_tmp   = ( int8_t )( *( ( int8_t* )source ) );
            uint16_tmp = ( uint16_t )( *( ( int8_t* )source ) );
            int16_tmp  = ( int16_t )( *( ( int8_t* )source ) );
            uint32_tmp = ( uint32_t )( *( ( int8_t* )source ) );
            int32_tmp  = ( int32_t )( *( ( int8_t* )source ) );
            uint64_tmp = ( uint64_t )( *( ( int8_t* )source ) );
            int64_tmp  = ( int64_t )( *( ( int8_t* )source ) );
            double_tmp = ( double )( *( ( int8_t* )source ) );
            break;
        case CUBE_DATA_TYPE_UINT16:
            uint8_tmp  = ( uint8_t )( *( ( uint16_t* )source ) );
            int8_tmp   = ( int8_t )( *( ( uint16_t* )source ) );
            uint16_tmp = ( uint16_t )( *( ( uint16_t* )source ) );
            int16_tmp  = ( int16_t )( *( ( uint16_t* )source ) );
            uint32_tmp = ( uint32_t )( *( ( uint16_t* )source ) );
            int32_tmp  = ( int32_t )( *( ( uint16_t* )source ) );
            uint64_tmp = ( uint64_t )( *( ( uint16_t* )source ) );
            int64_tmp  = ( int64_t )( *( ( uint16_t* )source ) );
            double_tmp = ( double )( *( ( uint16_t* )source ) );
            break;
        case CUBE_DATA_TYPE_INT16:
            uint8_tmp  = ( uint8_t )( *( ( int16_t* )source ) );
            int8_tmp   = ( int8_t )( *( ( int16_t* )source ) );
            uint16_tmp = ( uint16_t )( *( ( int16_t* )source ) );
            int16_tmp  = ( int16_t )( *( ( int16_t* )source ) );
            uint32_tmp = ( uint32_t )( *( ( int16_t* )source ) );
            int32_tmp  = ( int32_t )( *( ( int16_t* )source ) );
            uint64_tmp = ( uint64_t )( *( ( int16_t* )source ) );
            int64_tmp  = ( int64_t )( *( ( int16_t* )source ) );
            double_tmp = ( double )( *( ( int16_t* )source ) );
            break;
        case CUBE_DATA_TYPE_UINT32:
            uint8_tmp  = ( uint8_t )( *( ( uint32_t* )source ) );
            int8_tmp   = ( int8_t )( *( ( uint32_t* )source ) );
            uint16_tmp = ( uint16_t )( *( ( uint32_t* )source ) );
            int16_tmp  = ( int16_t )( *( ( uint32_t* )source ) );
            uint32_tmp = ( uint32_t )( *( ( uint32_t* )source ) );
            int32_tmp  = ( int32_t )( *( ( uint32_t* )source ) );
            uint64_tmp = ( uint64_t )( *( ( uint32_t* )source ) );
            int64_tmp  = ( int64_t )( *( ( uint32_t* )source ) );
            double_tmp = ( double )( *( ( uint32_t* )source ) );
            break;
        case CUBE_DATA_TYPE_INT32:
            uint8_tmp  = ( uint8_t )( *( ( int32_t* )source ) );
            int8_tmp   = ( int8_t )( *( ( int32_t* )source ) );
            uint16_tmp = ( uint16_t )( *( ( int32_t* )source ) );
            int16_tmp  = ( int16_t )( *( ( int32_t* )source ) );
            uint32_tmp = ( uint32_t )( *( ( int32_t* )source ) );
            int32_tmp  = ( int32_t )( *( ( int32_t* )source ) );
            uint64_tmp = ( uint64_t )( *( ( int32_t* )source ) );
            int64_tmp  = ( int64_t )( *( ( int32_t* )source ) );
            double_tmp = ( double )( *( ( int32_t* )source ) );
            break;
        case CUBE_DATA_TYPE_UINT64:
            uint8_tmp  = ( uint8_t )( *( ( uint64_t* )source ) );
            int8_tmp   = ( int8_t )( *( ( uint64_t* )source ) );
            uint16_tmp = ( uint16_t )( *( ( uint64_t* )source ) );
            int16_tmp  = ( int16_t )( *( ( uint64_t* )source ) );
            uint32_tmp = ( uint32_t )( *( ( uint64_t* )source ) );
            int32_tmp  = ( int32_t )( *( ( uint64_t* )source ) );
            uint64_tmp = ( uint64_t )( *( ( uint64_t* )source ) );
            int64_tmp  = ( int64_t )( *( ( uint64_t* )source ) );
            double_tmp = ( double )( *( ( uint64_t* )source ) );
            break;
        case CUBE_DATA_TYPE_INT64:
            uint8_tmp  = ( uint8_t )( *( ( int64_t* )source ) );
            int8_tmp   = ( int8_t )( *( ( int64_t* )source ) );
            uint16_tmp = ( uint16_t )( *( ( int64_t* )source ) );
            int16_tmp  = ( int16_t )( *( ( int64_t* )source ) );
            uint32_tmp = ( uint32_t )( *( ( int64_t* )source ) );
            int32_tmp  = ( int32_t )( *( ( int64_t* )source ) );
            uint64_tmp = ( uint64_t )( *( ( int64_t* )source ) );
            int64_tmp  = ( int64_t )( *( ( int64_t* )source ) );
            double_tmp = ( double )( *( ( int64_t* )source ) );
            break;
        case CUBE_DATA_TYPE_DOUBLE:
        case CUBE_DATA_TYPE_MIN_DOUBLE:
        case CUBE_DATA_TYPE_MAX_DOUBLE:
            uint8_tmp  = ( uint8_t )( *( ( double* )source ) );
            int8_tmp   = ( int8_t )( *( ( double* )source ) );
            uint16_tmp = ( uint16_t )( *( ( double* )source ) );
            int16_tmp  = ( int16_t )( *( ( double* )source ) );
            uint32_tmp = ( uint32_t )( *( ( double* )source ) );
            int32_tmp  = ( int32_t )( *( ( double* )source ) );
            uint64_tmp = ( uint64_t )( *( ( double* )source ) );
            int64_tmp  = ( int64_t )( *( ( double* )source ) );
            double_tmp = ( double )( *( ( double* )source ) );
            break;
        case CUBE_DATA_TYPE_UNKNOWN:
        case CUBE_DATA_TYPE_TAU_ATOMIC:
        case CUBE_DATA_TYPE_SCALE_FUNC:
        case CUBE_DATA_TYPE_COMPLEX:
        case CUBE_DATA_TYPE_RATE:
        case CUBE_DATA_TYPE_HISTOGRAM:
        default:
            UTILS_WARNING( "Type casting from UNKNOWN, TAU_ATOMIC, COMPLEX, SCALE_FUNC, RATE or HISTOGRAM type is not supported\n" );
            break;
    }
    switch ( to->type )
    {
        case CUBE_DATA_TYPE_UINT8:
            ( *( ( uint8_t* )target_place_for_value ) ) = uint8_tmp;
            break;
        case CUBE_DATA_TYPE_INT8:
            ( *( ( int8_t* )target_place_for_value ) ) = int8_tmp;
            break;
        case CUBE_DATA_TYPE_UINT16:
            ( *( ( uint16_t* )target_place_for_value ) ) = uint16_tmp;
            break;
        case CUBE_DATA_TYPE_INT16:
            ( *( ( int16_t* )target_place_for_value ) ) = int16_tmp;
            break;
        case CUBE_DATA_TYPE_UINT32:
            ( *( ( uint32_t* )target_place_for_value ) ) = uint32_tmp;
            break;
        case CUBE_DATA_TYPE_INT32:
            ( *( ( int32_t* )target_place_for_value ) ) = int32_tmp;
            break;
        case CUBE_DATA_TYPE_UINT64:
            ( *( ( uint64_t* )target_place_for_value ) ) = uint64_tmp;
            break;
        case CUBE_DATA_TYPE_INT64:
            ( *( ( int64_t* )target_place_for_value ) ) = int64_tmp;
            break;
        case CUBE_DATA_TYPE_DOUBLE:
        case CUBE_DATA_TYPE_MIN_DOUBLE:
        case CUBE_DATA_TYPE_MAX_DOUBLE:
            ( *( ( double* )target_place_for_value ) ) = double_tmp;
            break;
        case CUBE_DATA_TYPE_UNKNOWN:
        case CUBE_DATA_TYPE_TAU_ATOMIC:
        case CUBE_DATA_TYPE_COMPLEX:
        case CUBE_DATA_TYPE_RATE:
        case CUBE_DATA_TYPE_SCALE_FUNC:
        case CUBE_DATA_TYPE_HISTOGRAM:
        default:
            UTILS_WARNING( "Type casting to UNKNOWN, TAU_ATOMIC, COMPLEX, SCALE_FUNC, RATE or HISTOGRAM type is not supported\n" );
            break;
    }
}




/**
 * checks, if the type casting from DataType "from" into data type "to" is safe, precision lost is possible or unpossible, prints a error or warning message, if any.
 */
enum value_compat_t
__cube_type_compatibility_check( cube_value_param_array* dtype_params_from, cube_value_param_array* dtype_params_to )
{
    if ( dtype_params_from->type == CUBE_DATA_TYPE_UINT64 && dtype_params_to->type == CUBE_DATA_TYPE_UINT64 )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_INT64 && dtype_params_to->type == CUBE_DATA_TYPE_INT64 )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_DOUBLE )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_MIN_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_DOUBLE )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_MAX_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_DOUBLE )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_MIN_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_MAX_DOUBLE )
    {
        return CUBE_VALUE_SAME;
    }

    if ( dtype_params_from->type == CUBE_DATA_TYPE_UINT8 && dtype_params_to->type == CUBE_DATA_TYPE_UINT8 )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_INT8 && dtype_params_to->type == CUBE_DATA_TYPE_INT8 )
    {
        return CUBE_VALUE_SAME;
    }

    if ( dtype_params_from->type == CUBE_DATA_TYPE_UINT16 && dtype_params_to->type == CUBE_DATA_TYPE_UINT16 )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_INT16 && dtype_params_to->type == CUBE_DATA_TYPE_INT16 )
    {
        return CUBE_VALUE_SAME;
    }

    if ( dtype_params_from->type == CUBE_DATA_TYPE_UINT32 && dtype_params_to->type == CUBE_DATA_TYPE_UINT32 )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_INT32 && dtype_params_to->type == CUBE_DATA_TYPE_UINT32 )
    {
        return CUBE_VALUE_SAME;
    }

    if ( dtype_params_from->type == CUBE_DATA_TYPE_COMPLEX && dtype_params_to->type == CUBE_DATA_TYPE_COMPLEX )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_TAU_ATOMIC && dtype_params_to->type == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_SCALE_FUNC && dtype_params_to->type == CUBE_DATA_TYPE_SCALE_FUNC )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_RATE && dtype_params_to->type == CUBE_DATA_TYPE_RATE )
    {
        return CUBE_VALUE_SAME;
    }
    if ( dtype_params_from->type == CUBE_DATA_TYPE_HISTOGRAM && dtype_params_to->type == CUBE_DATA_TYPE_HISTOGRAM &&
         ( dtype_params_from->data[ 0 ] ) == ( dtype_params_to->data[ 0 ] )
         )
    {
        return CUBE_VALUE_SAME;
    }


    if (  ( dtype_params_from->type == CUBE_DATA_TYPE_HISTOGRAM && dtype_params_to->type != CUBE_DATA_TYPE_HISTOGRAM )
          ||
          ( dtype_params_from->type != CUBE_DATA_TYPE_HISTOGRAM && dtype_params_to->type == CUBE_DATA_TYPE_HISTOGRAM )
          )
    {
        UTILS_WARNING( "[ERROR] Casting of %s values into %s values is not possible.\n", __cube_metric_dtype2string( dtype_params_from ), __cube_metric_dtype2string( dtype_params_to ) );
        return CUBE_VALUE_INCOMPATIBLE;
    }
    if ( ( dtype_params_from->type == CUBE_DATA_TYPE_RATE && dtype_params_to->type != CUBE_DATA_TYPE_COMPLEX )
         ||
         ( dtype_params_from->type != CUBE_DATA_TYPE_RATE && dtype_params_to->type == CUBE_DATA_TYPE_COMPLEX )
         )
    {
        UTILS_WARNING( "[ERROR] Casting of %s values into %s values is not possible.\n", __cube_metric_dtype2string( dtype_params_from ), __cube_metric_dtype2string( dtype_params_to ) );
        return CUBE_VALUE_INCOMPATIBLE;
    }

    if ( ( dtype_params_from->type == CUBE_DATA_TYPE_COMPLEX && dtype_params_to->type != CUBE_DATA_TYPE_COMPLEX )
         ||
         ( dtype_params_from->type != CUBE_DATA_TYPE_COMPLEX && dtype_params_to->type == CUBE_DATA_TYPE_COMPLEX )
         )
    {
        UTILS_WARNING( "[ERROR] Casting of %s values into %s values is not possible.\n", __cube_metric_dtype2string( dtype_params_from ), __cube_metric_dtype2string( dtype_params_to ) );
        return CUBE_VALUE_INCOMPATIBLE;
    }
    if ( ( dtype_params_from->type == CUBE_DATA_TYPE_TAU_ATOMIC && dtype_params_to->type != CUBE_DATA_TYPE_TAU_ATOMIC )
         ||
         ( dtype_params_from->type != CUBE_DATA_TYPE_TAU_ATOMIC && dtype_params_to->type == CUBE_DATA_TYPE_TAU_ATOMIC ) )
    {
        UTILS_WARNING( "[ERROR] Casting of %s values into %s values is not possible.\n", __cube_metric_dtype2string( dtype_params_from ), __cube_metric_dtype2string( dtype_params_to ) );
        return CUBE_VALUE_INCOMPATIBLE;
    }




    if ( ( dtype_params_from->type == CUBE_DATA_TYPE_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_UINT64 )
         ||
         ( dtype_params_from->type == CUBE_DATA_TYPE_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_INT64 ) )
    {
        return CUBE_VALUE_RANGE_LOST;
    }
    if ( ( dtype_params_to->type == CUBE_DATA_TYPE_DOUBLE && dtype_params_from->type == CUBE_DATA_TYPE_UINT64 )
         ||
         ( dtype_params_to->type == CUBE_DATA_TYPE_DOUBLE && dtype_params_from->type == CUBE_DATA_TYPE_INT64 ) )
    {
        return CUBE_VALUE_RANGE_LOST;
    }

    if ( ( dtype_params_from->type == CUBE_DATA_TYPE_MIN_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_UINT64 )
         ||
         ( dtype_params_from->type == CUBE_DATA_TYPE_MIN_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_INT64 ) )
    {
        return CUBE_VALUE_RANGE_LOST;
    }
    if ( ( dtype_params_to->type == CUBE_DATA_TYPE_MIN_DOUBLE && dtype_params_from->type == CUBE_DATA_TYPE_UINT64 )
         ||
         ( dtype_params_to->type == CUBE_DATA_TYPE_MIN_DOUBLE && dtype_params_from->type == CUBE_DATA_TYPE_INT64 ) )
    {
        return CUBE_VALUE_RANGE_LOST;
    }


    if ( ( dtype_params_from->type == CUBE_DATA_TYPE_MAX_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_UINT64 )
         ||
         ( dtype_params_from->type == CUBE_DATA_TYPE_MAX_DOUBLE && dtype_params_to->type == CUBE_DATA_TYPE_INT64 ) )
    {
        return CUBE_VALUE_RANGE_LOST;
    }
    if ( ( dtype_params_to->type == CUBE_DATA_TYPE_MAX_DOUBLE && dtype_params_from->type == CUBE_DATA_TYPE_UINT64 )
         ||
         ( dtype_params_to->type == CUBE_DATA_TYPE_MAX_DOUBLE && dtype_params_from->type == CUBE_DATA_TYPE_INT64 ) )
    {
        return CUBE_VALUE_RANGE_LOST;
    }



    if ( __cube_metric_size_dtype( dtype_params_from ) < __cube_metric_size_dtype( dtype_params_to ) )
    {
        return CUBE_VALUE_COMPATIBLE;
    }
    if ( __cube_metric_size_dtype( dtype_params_from ) >= __cube_metric_size_dtype( dtype_params_to ) )
    {
        return CUBE_VALUE_PRECISION_LOST;
    }



    UTILS_WARNING( "[ERROR] Relation of %s values with %s values is not unknown.\n", __cube_metric_dtype2string( dtype_params_from ), __cube_metric_dtype2string( dtype_params_to ) );
    return CUBE_VALUE_UNKNOWN_RELATION;
}



/**
   size -> in bytes
 **/
void

__cube_initialize_row_of_memory( cube_value_param_array* dtype_params, enum value_init_t sort_of_value, void* row, uint64_t size, void* value )
{
    void* element = ( void* )__cube_get_init_value( dtype_params, sort_of_value, value );
    if ( element != NULL )
    {
        uint32_t elements_size = __cube_metric_size_dtype( dtype_params );
        uint64_t pos           = 0;
        for (; pos < size; pos += elements_size )
        {
            memcpy( ( uint8_t* )row + pos, element, elements_size );
        }
    }
    else
    {
        UTILS_WARNING( "Initialization of memory with *NULL value is not possible. Leave uninitialized.\n" );
    }
}


void*
__cube_get_init_value( cube_value_param_array* datatype, enum value_init_t sort_of_value, void* value )
{
    if ( datatype->type == CUBE_DATA_TYPE_INT64 )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_int64;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_int64;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_UINT64 )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_uint64;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_uint64;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_DOUBLE )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_double;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_double;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_MIN_DOUBLE )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_double;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_double;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_MAX_DOUBLE )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_double;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_double;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_INT8 )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_int8;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_uint8;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_UINT8 )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_uint8;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_uint8;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_INT16 )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_int16;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_int16;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_UINT16 )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_uint16;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_uint16;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_INT32 )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_int32;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_int32;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_UINT32 )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_uint32;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_uint32;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_COMPLEX )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_complex;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_complex;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_TAU_ATOMIC )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_tau_atomic;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_tau_atomic;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_SCALE_FUNC )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_scale_func;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_scale_func;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_MAX_DOUBLE )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_double;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_double;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_MIN_DOUBLE )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_double;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_double;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_RATE )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_rate;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_rate;
        }
        return value;
    }
    if ( datatype->type == CUBE_DATA_TYPE_HISTOGRAM )
    {
        if ( sort_of_value == CUBE_UNDEFINED_INIT )
        {
            return ( void* )&cube_undefined_double;
        }
        if ( sort_of_value == CUBE_ZERO_INIT )
        {
            return ( void* )&cube_zero_double;
        }
        return value;
    }
    UTILS_WARNING( "[CUBEW Warning]: Selected type of values (%u) in metric is unknown. Return \"UNKNOWN\" name\n", ( unsigned )datatype->type );
    return NULL;
}

/* size -> in elements */
void*
__cube_get_zero_row( cube_value_param_array* dtype_params, uint64_t size )
{
    uint32_t elements_size = __cube_metric_size_dtype( dtype_params );

    uint64_t row_size = elements_size * size;
    void*    row      = CUBEW_MALLOC( row_size, MEMORY_TRACING_PREFIX "Allocate row of zeroes" );
    __cube_initialize_row_of_memory( dtype_params, CUBE_ZERO_INIT, row, row_size, NULL );

    return row;
}

/* size -> in elements */
void*

__cube_get_undefined_row( cube_value_param_array* dtype_params, uint64_t size )
{
    uint32_t elements_size = __cube_metric_size_dtype( dtype_params );
    uint64_t row_size      = elements_size * size;
    void*    row           = CUBEW_MALLOC( row_size, MEMORY_TRACING_PREFIX "Allocate row of undefined"  );
    __cube_initialize_row_of_memory( dtype_params, CUBE_UNDEFINED_INIT, row, row_size, NULL );
    return row;
}


/* size -> in elements */
void*

__cube_get_valued_row( cube_value_param_array* dtype_params, uint64_t size, void* value )
{
    uint32_t elements_size = __cube_metric_size_dtype( dtype_params );
    uint64_t row_size      = elements_size * size;
    void*    row           = CUBEW_MALLOC( row_size, MEMORY_TRACING_PREFIX "Allocate row of defined values" );

    __cube_initialize_row_of_memory( dtype_params, CUBE_VALUE_INIT,  row, row_size, value );
    return row;
}











/**
 * Performs the actual replacing (escaping).
 *
 * Replacement happens in two steps
 * 1. First the list of the positions of parts, which have to be replaced, is created.
 * 2. Then replacement only at saved positions is performed.
 *
 * Note, that in the case  "String -> XML", first ampersands are replaced, then all another symbols.
 * in the case "XML ->String", first all another symbols, and then ampersand.
 * It removes possible infinite loop with ampersands.
 *********************************/
static
char*
__cube_replace_symbols2( char* str )
{
    char*    r    = NULL;
    char*    s    = str;
    int      len  = strlen( s );
    unsigned toks = 0;
    char*    p    = s;
    /* determine number of tokens to convert */
    while ( p <= s + len )
    {
        size_t n = strcspn( p, "<>&\"'" ); /* strcspn(p, "<>&\'\""); */
        p += ( n + 1 );
        toks++;
    }
    if ( toks )
    {
        r = ( char* )CUBEW_CALLOC( len + ( toks * 6 ), 1, MEMORY_TRACING_PREFIX "Allocate string to hold escaped symbols" ); /* allocate sufficiently long string */
    }
    p = s;
    /* replace each token in new string */
    if ( r )
    {
        while ( p <= s + len )
        {
            size_t n = strcspn( p, "<>&\"'" ); /* strcspn(p, "<>&\'\""); */
            strncat( r, p, n );
            p += n;
            switch ( *p )
            {
                case '<':
                    strcat( r, "&lt;" );
                    break;
                case '>':
                    strcat( r, "&gt;" );
                    break;
                case '&':
                    strcat( r, "&amp;" );
                    break;
                case '\'':
                    strcat( r, "&apos;" );
                    break;
                case '\"':
                    strcat( r, "&quot;" );
                    break;
                default:
                    break;
            }
            p++;
        }
    }

    return ( char* )r;
}


/**
 *  Replaces all symbols like "<" or ">" with corresponding HTML equivalents "&lt;", "&gt;"
 *********************************/
char*
__cube_services_escape_to_xml( char* str )
{
    if ( str == NULL )
    {
        return NULL;
    }
    return __cube_replace_symbols2( str );
}





/**
 * Prints the bitstring in binar form
 */
void
__cube_print_bit_string( char*    bitstring,
                         unsigned size )
{
    if ( bitstring != NULL )
    {
        unsigned i = 0;
        for ( i = 0; i < size; i++ )
        {
            int j = 8;
            for ( j = 7; j >= 0; j-- )
            {
                unsigned bit = ( ( bitstring[ i ] >> j ) & 0x01 );
                fprintf( stdout, "%u", bit );
            }
            fprintf( stdout, " " );
        }
    }
    fprintf( stdout, "\n" );
}


/**
 * Prints the bitstring in binar form
 */
void
__cube_print_row( void*    row,
                  unsigned size
                  )
{
    typedef union
    {
        char     ch[ 8 ];
        double   d;
        uint64_t ui;
    } pattern;

    char* end = ( char* )row + size;
    char* ptr = ( char* )row;


    pattern* ptr_pattern = ( pattern* )ptr;
    while ( ptr < end )
    {
        unsigned char u = *ptr;
        printf( "%2x", u );
        if ( ( ptr - ( char* )ptr_pattern ) == 7 )
        {
            printf( ":%20g:%" PRIu64, ( *ptr_pattern ).d, ( *ptr_pattern ).ui );
            ptr_pattern = ( pattern* )( ptr + 1 );
            printf( "   " );
        }
        ptr++;
    }
    fprintf( stdout, "\n" );
    fflush( stdout );
}




/**
 * Provides a missing on some platforms functionality of "strdup".
 *
 */
char*
cubew_strdup( const char* str )
{
    if ( str == NULL )
    {
        return NULL;
    }
    char* new_str = CUBEW_CALLOC( strlen( str ) + 1, 1, MEMORY_TRACING_PREFIX "Allocate string for duplication" );
    strcpy( new_str, str );
    return new_str;
}


void
__cube_create_path_for_file( char* path )
{
//     return;
    unsigned i      = 0;
    char*    mystr1 = path;
    char*    mystr2 = path;
    while ( ( mystr2 = strchr( mystr2, '/' ) ) != NULL && i++ < 1000 )
    {
        unsigned foundlen = mystr2 - mystr1;
        char     buf[ foundlen + 1 ];
        memset( buf, 0, foundlen + 1 );
        memcpy( buf, mystr1, foundlen );
        mkdir( buf, S_IRWXU | S_IROTH | S_IXOTH | S_IRGRP |  S_IXGRP );
        mystr2++;
    }
}


void
__cube_services_write_attributes( cube_meta_data_writer* writer,  cube_att_array* attr )
{
    /* metadata info */
    if ( attr == NULL )
    {
        return;   /* do not print missing attributes */
    }
    int i = 0;
    for ( i = 0; i < attr->size; i++ )
    {
        char* key    = attr->data[ i ]->key;
        char* value  = attr->data[ i ]->value;
        char* _key   = ( char* )__cube_services_escape_to_xml( key );
        char* _value = ( char* )__cube_services_escape_to_xml( value );
        __cube_write_meta_data( writer, "<attr key=\"%s\" value=\"%s\"/>\n", _key ? _key : key,  _value ? _value : value );
        CUBEW_FREE( _key, MEMORY_TRACING_PREFIX "Release _key of an attribute" );
        CUBEW_FREE( _value, MEMORY_TRACING_PREFIX "Release _value of an attribute"  );
    }
}










/* Provide fseeko declarion if we are able to link against fseeko,
   but found no declarion */
#if !( HAVE( DECL_FSEEKO ) ) && HAVE( FSEEKO )
int
fseeko( FILE* stream,
        off_t offset,
        int   whence );

#endif

/* Provide fseeko64 declarion if we are able to link against fseeko64,
   but found no declarion */
#if !( HAVE( DECL_FSEEKO64 ) ) && HAVE( FSEEKO64 )
int
fseeko64( FILE*   stream,
          off64_t offset,
          int     whence );

#endif

/* Provide implmentation of cubew_fseeko. Based on existing fseek variants.
   First try fseeko64, then fseeko, then fseek. */
#if HAVE( FSEEKO64 )
int
cubew_fseeko( FILE*    stream,
              uint64_t offset,
              int      whence )
{
    return fseeko64( stream, offset, whence );
}
#elif HAVE( FSEEKO )
int
cubew_fseeko( FILE*    stream,
              uint64_t offset,
              int      whence )
{
    return fseeko( stream, offset, whence );
}
#else
int
cubew_fseeko( FILE*    stream,
              uint64_t offset,
              int      whence )
{
    return fseek( stream, offset, whence );
}
#endif
