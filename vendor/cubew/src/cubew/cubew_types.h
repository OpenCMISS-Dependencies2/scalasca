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
 * \file cubew_types.h
   \brief Provides needed typedefs for cube writer.
 */
#ifndef TYPES_H
#define TYPES_H

// #include <config.h>
#include <stdint.h>
#include <sys/types.h>
#include <limits.h>
#include <float.h>


#include "cubew_report_layouts_types.h"


#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308
#endif



#ifdef __cplusplus
extern "C" {
#endif

/* CUBE TYPES  */

enum  DataType
{
    CUBE_DATA_TYPE_UNKNOWN,
    CUBE_DATA_TYPE_DOUBLE,
    CUBE_DATA_TYPE_UINT8,
    CUBE_DATA_TYPE_INT8,
    CUBE_DATA_TYPE_UINT16,
    CUBE_DATA_TYPE_INT16,
    CUBE_DATA_TYPE_UINT32,
    CUBE_DATA_TYPE_INT32,
    CUBE_DATA_TYPE_UINT64,
    CUBE_DATA_TYPE_INT64,
    CUBE_DATA_TYPE_TAU_ATOMIC,
    CUBE_DATA_TYPE_COMPLEX,
    CUBE_DATA_TYPE_RATE,
    CUBE_DATA_TYPE_MIN_DOUBLE,
    CUBE_DATA_TYPE_MAX_DOUBLE,
    CUBE_DATA_TYPE_SCALE_FUNC,
    CUBE_DATA_TYPE_HISTOGRAM,
    CUBE_DATA_TYPE_NDOUBLES
};



/**
 * General flat array of pointers on general data (void*) with dynamical memory allocation
 */
typedef struct dyn_array
{
    int    size;
    int    capacity;
    void** data;
} dyn_array;

/**
 * Char mapping. Is used to save string parameters of cnodes.
 */
typedef struct cmap
{
    char* key;
    char* value;
} cmap;

/**
 * Numerical mapping . Is used to save numerical parameters of cnodes.
 */
typedef struct nmap
{
    char*  key;
    double value;
} nmap;

/**
   A dynamic array with numerical parametes for cnodes.
 */
typedef struct nparam_parray
{
    int    size;
    int    capacity;
    nmap** data;
} nparam_parray;

/**
   A dynamic array with string parameters for cnodes.
 */
typedef struct cparam_parray
{
    int    size;
    int    capacity;
    cmap** data;
} cparam_parray;





/**
 * General flat array of pointers on 'char mapping' (cmap*)
 */

typedef struct att_array
{
    int    size;
    int    capacity;
    cmap** data;
} att_array;


/*  METRIC TYPES */
typedef struct marray
{
    int                  size;
    int                  capacity;
    struct cube_metric** data;
} marray;  /**< Dynamical array containing metrics */

/* METRIC VALUE TYPE */

typedef struct cube_value_param_array
{
    int           size;
    int           capacity;
    char*         dtype;
    enum DataType type;
    uint32_t*     data;
} cube_value_param_array;                          /**< A dynamic array of parameters . */



/*   CNODE TYPES */
typedef struct carray
{
    int                 size;
    int                 capacity;
    struct cube_cnode** data;
} carray; /**<  Dynamic array of cnodes. */


/* REGION TYPES */

typedef struct rarray
{
    int                 size;
    int                 capacity;
    struct cube_cnode** data;
} rarray;                          /**< A dynamic array of regions. */
typedef struct rarray cube_rarray; /**< A synonym of the arrays containing only regions. */




typedef struct dyn_array cube_dyn_array;
typedef struct cmap      cube_map;
typedef struct att_array cube_att_array;
typedef struct carray    cube_carray;
typedef struct dyn_array cube_type_param_array;

typedef struct map_thrd  cube_map_thrd;   /**< Declares a synonim for the general maping "map_thrd" as "cube_map_thrd" */

/*  types for the new flexible system tree */


/* forwart declarations*/
typedef struct cube_system_tree_node          cube_system_tree_node;
typedef struct cube_location_group            cube_location_group;
typedef struct cube_location                  cube_location;

typedef struct cube_system_tree_node_stripped cube_system_tree_node_stripped;
typedef struct cube_location_group_stripped   cube_location_group_stripped;
typedef struct cube_location_stripped         cube_location_stripped;


typedef struct cube_meta_data_writer cube_meta_data_writer;
typedef struct cube_metric           cube_metric;
typedef struct cube_region           cube_region;
typedef struct cube_cnode            cube_cnode;
typedef struct cube_cartesian        cube_cartesian;

typedef struct cube_t                cube_t;

/**
   Typedef for old types
 */

typedef cube_system_tree_node cube_node;
typedef cube_system_tree_node cube_machine;
typedef cube_location_group   cube_process;
typedef cube_location         cube_thread;


typedef struct cube_stnarray
{
    int                            size;
    int                            capacity;
    struct cube_system_tree_node** data;
} cube_stnarray;

typedef struct cube_stnarray_stripped
{
    int                                     size;
    int                                     capacity;
    struct cube_system_tree_node_stripped** data;
} cube_stnarray_stripped;

typedef struct cube_lgarray
{
    int                          size;
    int                          capacity;
    struct cube_location_group** data;
} cube_lgarray;

typedef struct cube_lgarray_stripped
{
    int                                   size;
    int                                   capacity;
    struct cube_location_group_stripped** data;
} cube_lgarray_stripped;


typedef struct cube_larray
{
    int                    size;
    int                    capacity;
    struct cube_location** data;
} cube_larray;

typedef struct cube_larray_stripped
{
    int                             size;
    int                             capacity;
    struct cube_location_stripped** data;
} cube_larray_stripped;


typedef cube_larray   cube_tarray;
typedef cube_lgarray  cube_parray;
typedef cube_stnarray cube_narray;



/**
 * Structure collection name and description  of a system tree node.
 */
struct cube_system_tree_node
{
    cube_stnarray*                child;
    cube_lgarray*                 group;
    struct cube_system_tree_node* parent;
    int                           id;
    char*                         name;
    char*                         desc;
    char*                         stn_class;
    cube_att_array*               attr;
};

typedef  struct cube_system_tree_node_plain cube_system_tree_node_plain;
struct cube_system_tree_node_plain
{
    char*           name;
    char*           desc;
    char*           stn_class;
    cube_att_array* attr;
};


typedef void ( * cube_system_tree_processor )( cube_system_tree_node_plain* to_fill );  /*this defines a type pf */


/**
 * Structure collection name, ID and rank of a location group.
 */

typedef enum cube_location_group_type { CUBE_LOCATION_GROUP_TYPE_PROCESS = 0, CUBE_LOCATION_GROUP_TYPE_METRICS = 1 } cube_location_group_type;

struct cube_location_group
{
    cube_larray*                  child;
    struct cube_system_tree_node* parent;
    char*                         name;
    int                           rank;
    int                           id;
    cube_location_group_type      type;
    cube_att_array*               attr;
};

typedef struct cube_location_group_plain cube_location_group_plain;
struct cube_location_group_plain
{
    char*                    name;
    int                      rank;
    cube_location_group_type type;
    cube_att_array*          attr;
};


typedef void ( * cube_location_group_processor )( cube_location_group_plain* to_fill );  /*this defines a type pf */






/**
 * Structure collection name, ID and rank of a location group.
 */
typedef enum cube_location_type { CUBE_LOCATION_TYPE_CPU_THREAD = 0, CUBE_LOCATION_TYPE_GPU = 1, CUBE_LOCATION_TYPE_METRIC = 2 } cube_location_type;

struct cube_location
{
    struct cube_location_group* parent;
    char*                       name;
    int                         rank;
    int                         id;
    cube_location_type          type;
    cube_att_array*             attr;
};


typedef struct cube_location_plain cube_location_plain;
struct cube_location_plain
{
    char*              name;
    int                rank;
    cube_location_type type;
    cube_att_array*    attr;
};

typedef void ( * cube_location_processor )( cube_location_plain* to_fill );  /*this defines a type pf */



typedef struct cube_system_tree_information cube_system_tree_information;
struct cube_system_tree_information
{
    uint32_t number_stn;
    uint32_t number_location_groups;
    uint32_t number_locations;
};




// / section with the type definitions required by the parser-writer

typedef enum cube_system_tree_writer_state { CUBE_SYSTEM_TREE_WRITER_INIT = 0, CUBE_SYSTEM_TREE_WRITER_STN = 1, CUBE_SYSTEM_TREE_WRITER_LG = 2, CUBE_SYSTEM_TREE_WRITER_LOC = 3, CUBE_SYSTEM_TREE_WRITER_UP = 4, CUBE_SYSTEM_TREE_WRITER_END = 5, CUBE_SYSTEM_TREE_WRITER_STOP = 5 } cube_system_tree_writer_state;

typedef void ( * cube_system_tree_writer_init )( cube_system_tree_information*,
                                                 void* );
typedef void ( * cube_system_tree_writer_finish )( void* );
typedef cube_system_tree_writer_state ( * cube_system_tree_writer_step )( cube_system_tree_node_plain* to_fill_stn,
                                                                          cube_location_group_plain*   to_fill_lg,
                                                                          cube_location_plain*         to_fill_loc,
                                                                          void* );



typedef struct cube_system_tree_writer cube_system_tree_writer;
typedef void ( * cube_system_tree_writer_driver )(  cube_system_tree_writer*,
                                                    void* );



struct cube_system_tree_writer
{
    cube_system_tree_information*  system_tree_information;

    cube_system_tree_writer_init   init;
    cube_system_tree_writer_finish finish;
    cube_system_tree_writer_step   step;
    cube_system_tree_writer_driver driver;

    /** track of id's coz _plain strucutred do not contain ids*/
    uint32_t stn_id;
    uint32_t lg_id;
    uint32_t loc_id;

    /** basic state stack */
    uint32_t                       state_index;
    uint32_t                       states_size;
    cube_system_tree_writer_state* states;

    /** pointer to meta data writer*/
    cube_meta_data_writer*       meta_data_writer;

    cube_system_tree_node_plain* stn_plain;
    cube_location_group_plain*   lg_plain;
    cube_location_plain*         loc_plain;


    void* user_ptr;                       /* user defined pointer for callbacks  */
};

















enum CubeFlavours_t { CUBE_MASTER = 0, CUBE_WRITER = 1, CUBE_SLAVE = 2 };

/*  Code is not clear for reader. Why False or FALSE? Why not CUBE_FALSE and CUBE_TRUE. */
enum bool_t { CUBE_FALSE = 0, CUBE_TRUE = 1 };

/* Value compatibility status*/
enum value_compat_t { CUBE_VALUE_SAME = 0, CUBE_VALUE_COMPATIBLE = 1, CUBE_VALUE_INCOMPATIBLE = 2, CUBE_VALUE_PRECISION_LOST = 3, CUBE_VALUE_MEANING_LOST = 4, CUBE_VALUE_RANGE_LOST = 5, CUBE_VALUE_UNKNOWN_RELATION = 127 };

/* Value initialization */
enum value_init_t { CUBE_UNDEFINED_INIT = 0, CUBE_ZERO_INIT = 1, CUBE_VALUE_INIT = 2 };




/*  Set of cannonical definitions of various types supported by cube:
    It is always a packed structure.
 */
/**
 * Cannonical type : uint8 (unsigned char (0-255)).
 */

typedef struct cube_type_uint8
{
    uint8_t value;
} cube_type_uint8;
static const cube_type_uint8 cube_undefined_uint8 = { UINT8_C( 0xff ) };
static const cube_type_uint8 cube_zero_uint8 =     { UINT8_C( 0x00 ) };

/**
 * Cannonical type : int8 (signed char (-127 - 127)).
 */

typedef struct cube_type_int8
{
    int8_t value;
} cube_type_int8;
static const cube_type_int8 cube_undefined_int8 = { ( int8_t )INT8_C( 0xff ) };
static const cube_type_int8 cube_zero_int8 =     { INT8_C( 0x00 ) };
/**
 * Cannonical type : uint16 (unsigned short ).
 */
typedef struct cube_type_uint16
{
    uint16_t value;
} cube_type_uint16;
static const cube_type_uint16 cube_undefined_uint16 = { UINT16_C( 0xffff ) };
static const cube_type_uint16 cube_zero_uint16 =     { UINT64_C( 0x0000 ) };
/**
 * Cannonical type : int16 (signed short ).
 */

typedef struct cube_type_int16
{
    int16_t value;
} cube_type_int16;
static const cube_type_int16 cube_undefined_int16 = { ( int16_t )INT16_C( 0xffff ) };
static const cube_type_int16 cube_zero_int16 =     { INT16_C( 0x0000 ) };

/**
 * Cannonical type : uint32 (unsigned int ).
 */
typedef struct cube_type_uint32
{
    uint32_t value;
} cube_type_uint32;
static const cube_type_uint32 cube_undefined_uint32 = { UINT32_C( 0xffffffff ) };
static const cube_type_uint32 cube_zero_uint32 =     { UINT32_C( 0x00000000 ) };



/**
 * Cannonical type : int32 (signed int ).
 */
typedef struct cube_type_int32
{
    int32_t value;
} cube_type_int32;
static const cube_type_int32 cube_undefined_int32 = { ( int32_t )INT32_C( 0xffffffff ) };
static const cube_type_int32 cube_zero_int32 =     { UINT32_C( 0x00000000 ) };


/**
 * Cannonical type : uint64 (unsigned long int  ).
 */
typedef struct cube_type_uint64
{
    uint64_t value;
} cube_type_uint64;
static const cube_type_uint64 cube_undefined_uint64 = { UINT64_C( 0xffffffffffffffff ) };
static const cube_type_uint64 cube_zero_uint64 =     { UINT64_C( 0x0000000000000000 ) };

/**
 * Cannonical type : int64 (signed long int  ).
 */
typedef struct cube_type_int64
{
    int64_t value;
} cube_type_int64;
static const cube_type_int64 cube_undefined_int64 =  { ( int64_t )INT64_C( 0xffffffffffffffff ) };
static const cube_type_int64 cube_zero_int64 =      { INT64_C( 0x0000000000000000 ) };

/**
 * Cannonical type : double
 */
typedef struct cube_type_double
{
    double value;
} cube_type_double;
static const cube_type_double cube_undefined_double =  { 0. };
static const cube_type_double cube_zero_double =      { 0. };




/**
 * Cannonical type : tau atomic
 */
typedef struct cube_type_tau_atomic cube_type_tau_atomic;
struct cube_type_tau_atomic
{
    uint32_t N;
    double   Min;
    double   Max;
    double   Sum;
    double   Sum2;
}  __attribute__ ( ( __packed__ ) );

static const cube_type_tau_atomic cube_undefined_tau_atomic =  {
    UINT32_C( 0xffffffff ),
    UINT64_C( 0 ),
    UINT64_C( 0 ),
    UINT64_C( 0 ),
    UINT64_C( 0 )
};
static const cube_type_tau_atomic cube_zero_tau_atomic =  {
    UINT32_C( 0x00000000 ),
    DBL_MAX,
    -DBL_MAX,
    UINT64_C( 0x0000000000000000 ),
    UINT64_C( 0x0000000000000000 )
};


/**
 * Cannonical type : double
 */
typedef struct cube_type_scale_func cube_type_scale_func;
struct cube_type_scale_func
{
    double value[ 27 ]; /* there are 27 possible combinations of powers*/
}  __attribute__ ( ( __packed__ ) );
static const cube_type_scale_func cube_undefined_scale_func =  {
    { UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ),
      UINT64_C( 0 ) }
};
static const cube_type_scale_func cube_zero_scale_func =      {
    {
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 ),
        UINT64_C( 0x0000000000000000 )
    }
};



/**
 * Cannonical type : rate
 */
typedef struct cube_type_rate cube_type_rate;
struct cube_type_rate
{
    double numerator;
    double denominator;
}  __attribute__ ( ( __packed__ ) );

static const cube_type_rate cube_undefined_rate =  {
    UINT64_C( 0 ),
    UINT64_C( 0 )
};
static const cube_type_rate cube_zero_rate =  {
    UINT64_C( 0x0000000000000000 ),
    UINT64_C( 0x0000000000000000 )
};



/**
 * Cannonical type : complex
 */
typedef struct cube_type_complex cube_type_complex;
struct cube_type_complex
{
    double re_value;
    double im_value;
}  __attribute__ ( ( __packed__ ) );

static const cube_type_complex cube_undefined_complex =  {
    UINT64_C( 0 ),
    UINT64_C( 0 )
};
static const cube_type_complex cube_zero_complex =  {
    UINT64_C( 0x0000000000000000 ),
    UINT64_C( 0x0000000000000000 )
};







typedef struct marray cube_marray;   /**< Synonim of an array for using it as an array of metrics. */

/**
 * A Structure of a metric.
 */

enum CubeMetricType { CUBE_METRIC_INCLUSIVE = 0, CUBE_METRIC_EXCLUSIVE = 1, CUBE_METRIC_SIMPLE = 2, CUBE_METRIC_POSTDERIVED = 3, CUBE_METRIC_PREDERIVED_INCLUSIVE = 4, CUBE_METRIC_PREDERIVED_EXCLUSIVE = 5 };
enum CubeMetricVizType { CUBE_METRIC_NORMAL = 0, CUBE_METRIC_GHOST = 1 };
enum CubeMetricAggrType { CUBE_METRIC_AGGR_PLUS = 0, CUBE_METRIC_AGGR_MINUS = 1, CUBE_METRIC_AGGR_AGGR = 3 };

enum  IndexFormat { CUBE_INDEX_FORMAT_NONE, CUBE_INDEX_FORMAT_SPARSE, CUBE_INDEX_FORMAT_BITVECTOR, CUBE_INDEX_FORMAT_DENSE };




/* header in metric index */
typedef union
{
    struct
    {
        uint32_t endian;
        uint16_t version;
        uint8_t  metric_format;
    } __attribute__ ( ( __packed__ ) ) named;
    char as_array[ 1 ];
} metric_header;








#ifdef __cplusplus
}
#endif

#endif
