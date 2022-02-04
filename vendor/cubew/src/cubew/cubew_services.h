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
 * \file cubew_services.h
   \brief Provides all kind of service functions. Cannot be linked from libcubew.a.
 */
#ifndef CUBEW_SERVICES_H
#define CUBEW_SERVICES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cubew_types.h"

/*          public part  */
char*
__cube_services_escape_to_xml( char* str );
void
__cube_print_bit_string( char*    bitstring,
                         unsigned size );
void
__cube_print_row( void*    row,
                  unsigned size );

/*     private part, should be not calles "standalone      */
enum DataType
__cube_metric_dtype2data_type( const char* dtype );
int
__cube_metric_size_dtype( cube_value_param_array* _array );
char*
__cube_metric_dtype2string( cube_value_param_array* _array );

cube_value_param_array*
__cube_metric_dtype_param_split( const char* dtype  );

void
__cube_value_param_free( cube_value_param_array* _array );

uint32_t
__cube_metric_bit_position( char*    bit_vector,
                            unsigned vlen,
                            int      cid );
uint32_t
__cube_metric_size_of_index( char*    bit_vector,
                             unsigned vlen );
uint32_t*
__cube_metric_create_index( char*    bit_vector,
                            unsigned vlen );
unsigned
__cube_is_bit_set( char* bit_vector,
                   int   bitposition );
void
__cube_set_bit( char* bit_vector,
                int   bitposition );

int
/* -1 shows empty bitstring */
__cube_metric_last_bit_position( char*    bit_vector,
                                 unsigned vlen
                                 );
size_t
__cube_num_of_set_bits( char*    bit_vector,
                        unsigned vlen
                        );


uint64_t*
__cube_transform_doubles_into_uint64( double*  data_row,
                                      uint32_t size );
int64_t*
__cube_transform_doubles_into_int64( double*  data_row,
                                     uint32_t size );

double*
__cube_transform_uint64_into_doubles( uint64_t* data_row,
                                      uint32_t  size );

double*
__cube_transform_int64_into_doubles( int64_t* data_row,
                                     uint32_t size );


enum value_compat_t
__cube_type_compatibility_check( cube_value_param_array* from,
                                 cube_value_param_array* to );

void*

__cube_transform_row_of_values( cube_value_param_array* from,
                                cube_value_param_array* to,
                                void*                   sevs,
                                uint32_t                size );


void
__cube_value_transform(  cube_value_param_array* from,
                         cube_value_param_array* to,
                         void*                   target_place_for_value,
                         void*                   source,
                         uint32_t                to_sizeof,
                         uint32_t                from_sizeof );

void*

__cube_get_init_value( cube_value_param_array* dtype_params,
                       enum value_init_t       sort_of_value,
                       void*                   value );


void

__cube_initialize_row_of_memory( cube_value_param_array* dtype_params,
                                 enum value_init_t       sort_of_value,
                                 void*,
                                 uint64_t                size,
                                 void*                   value );                                                         /* size -> in bytes */


void*

__cube_get_zero_row( cube_value_param_array* dtype_params,
                     uint64_t                size );   /* size -> in bytes */


void*

__cube_get_undefined_row( cube_value_param_array* dtype_params,
                          uint64_t                size );   /* size -> in bytes */


void*

__cube_get_valued_row( cube_value_param_array* dtype_params,
                       uint64_t                size,
                       void*                   value );                /* size -> in bytes */



void
__cube_services_write_attributes( cube_meta_data_writer* writer,
                                  cube_att_array*        attr
                                  );


char*
cubew_strdup( const char* str );


void
__cube_create_path_for_file( char* path );


/* Our cube wrapper the POSIX fseeko implementation, because it is not available in
   C99 be default.  */
int
cubew_fseeko( FILE*    stream,
              uint64_t offset,
              int      whence );


#ifdef __cplusplus
}
#endif

#endif
