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
 * \file cubew_metric.h
   \brief Declares  types and functions to deal with metrics.
 */
#ifndef CUBEW_METRIC_H
#define CUBEW_METRIC_H

#include "cubew_types.h"

#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
#include <zlib.h>
#endif /* BACKEND_CUBE_COMPRESSED */


#ifdef __cplusplus
extern "C" {
#endif

/** defines a marker at the beginning of the file */
#define CUBE_INDEXFILE_MARKER "CUBEX.INDEX"
#define CUBE_INDEXFILE_MARKER_SIZE 11

/** defines a marker at the beginning of the file */
#define CUBE_DATAFILE_MARKER "CUBEX.DATA"
#define CUBE_DATAFILE_MARKER_SIZE 10

#define CUBE_DATAFILE_COMPRESSED_MARKER        "ZCUBEX.DATA"
#define CUBE_DATAFILE_COMPRESSED_MARKER_SIZE    11




struct cube_metric
{
    char*                   disp_name;
    char*                   uniq_name;
    char*                   dtype;
    char*                   uom;
    char*                   val;
    char*                   url;
    char*                   descr;
    int                     id;
    cube_marray*            child;
    struct cube_metric*     parent;

    cube_att_array*         attr;
    cube_value_param_array* dtype_params;
/*
    advanced part
 */
    uint32_t               ncn;
    uint32_t               nthrd;
    enum IndexFormat       metric_format;
    enum CubeMetricType    metric_type;
    enum CubeMetricVizType metric_viz_type;
    FILE*                  data_file;
    char*                  known_cnodes;          /* bitmask of cnodes */
    char*                  written_cnodes;
    signed long long       start_pos_of_datafile; /**< used to calculate size of datafile */
    enum bool_t            compressed;
    enum bool_t            im_writing;            /**< marks, that last writing call was done for this metric. Needed to write TARed layout of CUBE */
    enum bool_t            im_finished;           /**< marks, that the files with the data were closed */
    enum bool_t            valid_initialization;  /**< marks, if data type and type of metric are compatible */
    report_layout_writer*  layout;                /** Saves the structure controling the organisation of the report file layout */
    uint64_t*              sub_index;
    uint64_t               subi_size;
    uint64_t               icompressed;
    uint64_t               n_compressed;
    uint64_t               incr_compressed;
#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    Bytef*                 compression_buffer;
#endif /* BACKEND_CUBE_COMPRESSED */
    off_t                  last_seek_position;
    cube_dyn_array*        local_cnode_enumeration;
    cube_dyn_array*        optimal_cnodes_sequence;
    carray*                optimal_cnodes_sequence_with_cndptrs;
    char*                  expression;                  /** only derivated metrics do carry here the expression for the calculation. Another metrics ignore this field. */
    char*                  init_expression;             /** only derivated metrics do carry here the initiallization expression for the calculation. Another metrics ignore this field. */
    char*                  aggr_plus_expression;        /** only derivated metrics do carry here the expression for the + calculation. Another metrics ignore this field. */
    char*                  aggr_minus_expression;       /** only derivated metrics do carry here the expression for the - calculation. Another metrics ignore this field. */
    char*                  aggr_aggr_expression;        /** only derivated metrics do carry here the expression for the aggregation over threads only. Another metrics ignore this field. */
    enum bool_t            locationwise;
    enum bool_t            is_cacheable;                /** signalized, that metric cannot be cached (if "no") */
};







cube_metric*
cube_metric_create( cube_metric* metric );
void
cube_metric_init( cube_metric*          metric,
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
                  enum bool_t           compressed );

enum bool_t
cube_metric_valid( cube_metric* metric );  /** after initialization, cube class checks, if initializationwas successfull by calling this method*/





void
cube_metric_free( cube_metric* metric );

void
cube_metric_finish( cube_metric* metric,
                    int          wrte_index );

void
cube_metric_set_expression( cube_metric* metric,
                            char*        expression );

char*
cube_metric_get_expression( cube_metric* metric );

void
cube_metric_set_init_expression( cube_metric* metric,
                                 char*        expression );

char*
cube_metric_get_init_expression( cube_metric* metric );

void
cube_metric_set_aggr_expression( cube_metric*            metric,
                                 enum CubeMetricAggrType type,
                                 char*                   expression );

char*
cube_metric_get_aggr_expression( cube_metric*            metric,
                                 enum CubeMetricAggrType type );

void
cube_metric_set_as_locationwise( cube_metric* metric,
                                 enum bool_t  locationwise );

void
cube_metric_set_viz_type( cube_metric*           metric,
                          enum CubeMetricVizType type );

enum CubeMetricVizType
cube_metric_get_viz_type( cube_metric* metric );

void
cube_metric_set_cacheable( cube_metric* metric,
                           enum bool_t  cacheable );

enum bool_t
cube_metric_is_cacheable( cube_metric* metric );


void
cube_metric_setup_for_writing( cube_metric*    metric,
                               cube_dyn_array* cnds,
                               cube_dyn_array* roots_cnds,
                               uint32_t        n_locations );
carray*
cube_metric_return_enumeration( cube_metric* metric );

void
cube_metric_set_known_cnodes( cube_metric* metric,
                              char*        known_cnodes /*, unsigned size*/ );

const char*
cube_metric_get_own_type( cube_metric* metric );

const char*
cube_metric_get_own_viz_type( cube_metric* metric );

cube_metric*
cube_metric_get_child( cube_metric* metric,
                       int          i );
cube_metric*
cube_metric_get_parent( cube_metric* metric );
char*
cube_metric_get_disp_name( cube_metric* metric );
char*
cube_metric_get_uniq_name( cube_metric* metric );
char*
cube_metric_get_dtype( cube_metric* metric );
char*
cube_metric_get_uom( cube_metric* metric );
char*
cube_metric_get_val( cube_metric* metric );
char*
cube_metric_get_descr( cube_metric* metric );
char*
cube_metric_get_url( cube_metric* metric );
int
cube_metric_num_children( cube_metric* metric );
void
cube_metric_add_child( cube_metric* parent,
                       cube_metric* met );

/**   void  cube_metric_assign_ids(cube_metric* metric, int* id); */
int
cube_metric_get_level( cube_metric* metric );


void
cube_metric_add_attr( cube_metric* metric,
                      cmap*        m );
void
cube_metric_def_attr( cube_metric* metric,
                      const char*  key,
                      const char*  value );

void
cube_metric_writeXML( cube_metric*           metric,
                      cube_meta_data_writer* writer );

void
cube_metric_set_id( cube_metric* metric,
                    int          new_id );
void
cube_metric_set_uniq_name( cube_metric* metric,
                           char*        uniq_name );
int
cube_metric_get_id( cube_metric* metric );

int
cube_metric_equal( cube_metric* a,
                   cube_metric* b );

uint64_t
cube_metric_size_of_data_file( cube_metric* );


void
cube_metric_write_row( cube_metric* metric,
                       cube_cnode*  cnd,
                       void*        data_row );
void
cube_metric_write_row_of_doubles( cube_metric* metric,
                                  cube_cnode*  cnd,
                                  double*      data_row );
void
cube_metric_write_row_of_uint64( cube_metric* metric,
                                 cube_cnode*  cnd,
                                 uint64_t*    data_row );

void
cube_metric_write_row_of_int64( cube_metric* metric,
                                cube_cnode*  cnd,
                                int64_t*     data_row );

/* List of the canonical writing calls. They perform type casting if needed and check metric if it supports that kind of value. Internally they call cube_write_sev_row(...)*/

void
cube_metric_write_row_of_cube_type_uint8( cube_metric*     met,
                                          cube_cnode*      cnd,
                                          cube_type_uint8* sevs );
void
cube_metric_write_row_of_cube_type_int8( cube_metric*    met,
                                         cube_cnode*     cnd,
                                         cube_type_int8* sevs );
void
cube_metric_write_row_of_cube_type_uint16( cube_metric*      met,
                                           cube_cnode*       cnd,
                                           cube_type_uint16* sevs );
void
cube_metric_write_row_of_cube_type_int16( cube_metric*     met,
                                          cube_cnode*      cnd,
                                          cube_type_int16* sevs );
void
cube_metric_write_row_of_cube_type_uint32( cube_metric*      met,
                                           cube_cnode*       cnd,
                                           cube_type_uint32* sevs );
void
cube_metric_write_row_of_cube_type_int32( cube_metric*     met,
                                          cube_cnode*      cnd,
                                          cube_type_int32* sevs );
void
cube_metric_write_row_of_cube_type_uint64( cube_metric*      met,
                                           cube_cnode*       cnd,
                                           cube_type_uint64* sevs );
void
cube_metric_write_row_of_cube_type_int64( cube_metric*     met,
                                          cube_cnode*      cnd,
                                          cube_type_int64* sevs );
void
cube_metric_write_row_of_cube_type_double( cube_metric*      met,
                                           cube_cnode*       cnd,
                                           cube_type_double* sevs );
void
cube_metric_write_row_of_cube_type_complex( cube_metric*       met,
                                            cube_cnode*        cnd,
                                            cube_type_complex* sevs );
void
cube_metric_write_row_of_cube_type_tau_atomic( cube_metric*          met,
                                               cube_cnode*           cnd,
                                               cube_type_tau_atomic* sevs );
void
cube_metric_write_row_of_cube_type_scale_func( cube_metric*          met,
                                               cube_cnode*           cnd,
                                               cube_type_scale_func* sevs );
void
cube_metric_write_row_of_cube_type_rate( cube_metric*    met,
                                         cube_cnode*     cnd,
                                         cube_type_rate* sevs );

void
cube_metric_write_row_of_cube_type_histogram( cube_metric*      met,
                                              cube_cnode*       cnd,
                                              cube_type_double* sevs );


#ifdef __cplusplus
}
#endif

#endif
