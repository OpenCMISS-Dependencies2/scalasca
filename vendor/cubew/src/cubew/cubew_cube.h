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
 * \file cubew_cube.h
 * \brief Header with definition of structure "cube_t", which contains the cube.
 *
 *
 *
 */
#ifndef CUBEW_CUBE_H
#define CUBEW_CUBE_H

#include <stdint.h>
#ifndef CUBE_AS_SVN_EXTERNAL
#  include  "cubew-version.h"
#endif


#include "cubew_types.h"
#include "cubew_memory.h"
#include "cubew_metric.h"
#include "cubew_cnode.h"
#include "cubew_region.h"
#include "cubew_system_tree_node.h"
#include "cubew_location_group.h"
#include "cubew_location.h"
#include "cubew_metric.h"
#include "cubew_cartesian.h"
#include "cubew_system_tree_writer.h"

#ifdef __cplusplus
extern "C" {
#endif



/** CubePL version, writen by this version of c-writer library*/
#define CUBEPL_VERSION "1.1"
#define CUBEPL_VERSION_KEY "CubePL Version"
#define CUBEW_VERSION_KEY "Cube Version"
#ifdef CUBE_AS_SVN_EXTERNAL
#define CUBEW_VERSION "4.3"
#endif

/**
 * Structure defines the CUBE.
 */
struct cube_t
{
    cube_dyn_array*          met_ar;                    /**< Dynamic Array "cube_dyn_array" of metrics.  */
    cube_dyn_array*          rmet_ar;                   /**< Dynamic Array cube_dyn_array of metrics (?????).  */
    cube_dyn_array*          reg_ar;                    /**< Dynamic Array  cube_dyn_array of source code regions.  */
    cube_dyn_array*          cnd_ar;                    /**< Dynamic Array  cube_dyn_array of calees and callers. */
    cube_dyn_array*          rcnd_ar;                   /**< Dynamic Array  cube_dyn_array of calees and callers(????). */
    cube_dyn_array*          root_stn_ar;               /**< Dynamic Array  cube_dyn_array of machines. */
    cube_dyn_array*          stn_ar;                    /**< Dynamic Array  cube_dyn_array of system tree nodes. */
    cube_dyn_array*          lg_ar;                     /**< Dynamic Array  cube_dyn_array of location groups. */
    cube_dyn_array*          locs_ar;                   /**< Dynamic Array  cube_dyn_array of locations / threads. */
    uint64_t                 ntid;                      /**< Explicit number of locations. Needed for an flat system tree writing. */

    cube_system_tree_writer* system_tree_writer;        /**< A structure holding all data required for the iterative system tree writing */

    cube_dyn_array*          cart_ar;                   /**< Dynamic Array  cube_dyn_array of cartesian topologies. */
    cube_dyn_array*          mirr_ar;                   /**< Dynamic Array  cube_dyn_array of mirrors. */
    cube_att_array*          attr;
    int                      sev_flag;
    double***                sev;                       /**< Dynamic Array of dynamic arrays of dynamic arrays cube_dyn_array of severities. */
    int**                    exist;                     /**< Two dimensional array of flags(?) */

    int***                   cn_exist;
    char*                    cubename;         /**< .cube File */
    char*                    metrics_title;    /**< General name for metric dimension. If not, it is "Metrics" */
    char*                    calltree_title;   /**< General name for calltree dimension. If not, it is "Calltree" */
    char*                    systemtree_title; /**< General name for systemtree dimension. If not, it is "Systemtree" */

    /* char* statname      */            /**< Saves the name of the statistic file. If present -> saves it in cube xml file */
    enum bool_t compressed;
/*
    ADVANCED PART
 */
    enum CubeFlavours_t   cube_flavour;        /** Saves if cube is MASTER, WRITER or SLAVE. Needed for parallel writing, if supported.*/
    int                   first_call;          /** Signalizes first call of write_row. Cube creates needed structures. */
    int                   locked_for_writing;  /** Locks cube for writin.g No changes in structure is possible anymore.*/
    uint64_t              size_of_anchor_file; /** Saves the size of the anchor file. Initial value -1 shows, that anchor wasn't saved yet. */
    report_layout_writer* layout;              /**  Saves the pointer on the structure, controling the file layout organization*/
};


cube_t*
cube_create( char*               cube_name,
             enum CubeFlavours_t cube_fv,
             enum bool_t         compression );                                                                         /**< Allocates memory for all structures in cube and initializes it. */

void
cube_sev_init( cube_t* cb );                        /**< Allocates memory for all structures in cube and initializes it. */
void
cube_free( cube_t* cb );                            /**  Writes meta, closes metric files and destroys all allocated memory.*/
void
cube_construct_arrays( cube_t* cb );



void
cube_add_metric( cube_t*      cb,
                 cube_metric* met );
void
cube_add_rmetric( cube_t*      cb,
                  cube_metric* met );
void
cube_add_region( cube_t*      cb,
                 cube_region* reg );
void
cube_add_cnode( cube_t*     cb,
                cube_cnode* cnd );
void
cube_add_rcnode( cube_t*     cb,
                 cube_cnode* cnd );

void
cube_add_system_tree_node( cube_t*                cb,
                           cube_system_tree_node* stn );
void
cube_add_location_group( cube_t*              cb,
                         cube_location_group* group );
void
cube_add_location( cube_t*        cb,
                   cube_location* location );


void
cube_add_cart( cube_t*         cb,
               cube_cartesian* cart );
void
cube_add_mirror( cube_t*     cb,
                 const char* mir );
void
cube_add_attr( cube_t*   cb,
               cube_map* m );

int
cube_reserve_metrics( cube_t*  cb,
                      unsigned num );
int
cube_reserve_regions( cube_t*  cb,
                      unsigned num );
int
cube_reserve_cnodes( cube_t*  cb,
                     unsigned num );

int
cube_reserve_carts( cube_t*  cb,
                    unsigned num );

void
cube_def_attr( cube_t*     cb,
               const char* key,
               const char* value );
void
cube_def_mirror( cube_t*     cb,
                 const char* url );

cube_metric*
cube_def_met( cube_t*             cb,
              const char*         disp_name,
              const char*         uniq_name,
              const char*         dtype,
              const char*         uom,
              const char*         val,
              const char*         url,
              const char*         descr,
              cube_metric*        parent,
              enum CubeMetricType metric_type );
cube_region*
cube_def_region( cube_t*     cb,
                 const char* name,
                 const char* mangled_name,
                 const char* paradigm,
                 const char* role,
                 long        begln,
                 long        endln,
                 const char* url,
                 const char* descr,
                 const char* mod );
cube_cnode*
cube_def_cnode_cs( cube_t*      cb,
                   cube_region* callee,
                   const char*  mod,
                   int          line,
                   cube_cnode*  parent );
cube_cnode*
cube_def_cnode( cube_t*      cb,
                cube_region* callee,
                cube_cnode*  parent );


cube_system_tree_node*
cube_def_system_tree_node( cube_t*                cb,
                           const char*            name,
                           const char*            desc,
                           const char*            type,
                           cube_system_tree_node* parent
                           );

cube_location_group*
cube_def_location_group( cube_t*                  cb,
                         const char*              name,
                         int                      rank,
                         cube_location_group_type type,
                         cube_system_tree_node*   stn );

cube_location*
cube_def_location( cube_t*              cb,
                   const char*          name,
                   int                  rank,
                   cube_location_type   type,
                   cube_location_group* group );

void
cube_system_tree_writer_initialize( cube_t*                        cb,
                                    cube_system_tree_writer_init   init,
                                    cube_system_tree_writer_step   step,
                                    cube_system_tree_writer_driver driver,
                                    cube_system_tree_writer_finish finish,
                                    void*
                                    );

cube_system_tree_information*
cube_get_system_tree_information( cube_t* );


void
cube_set_system_tree_size( cube_t*  cb,
                           uint32_t number_stn,
                           uint32_t number_location_groups,
                           uint32_t number_locations );



cube_machine*
cube_def_mach( cube_t*     cb,
               const char* name,
               const char* desc );
cube_node*
cube_def_node( cube_t*       cb,
               const char*   name,
               cube_machine* mach );
cube_process*
cube_def_proc( cube_t*     cb,
               const char* name,
               int         rank,
               cube_node*  node );
cube_thread*
cube_def_thrd( cube_t*       cb,
               const char*   name,
               int           rank,
               cube_process* proc );
cube_cartesian*
cube_def_cart( cube_t*   cb,
               long int  ndims,
               long int* dim,
               int*      period );
void
cube_def_coords( cube_t*         cb,
                 cube_cartesian* cart,
                 cube_thread*    thrd,
                 long int*       coord );


cube_dyn_array*
cube_get_rmet( cube_t* cb );
cube_dyn_array*
cube_get_met( cube_t* cb );
cube_dyn_array*
cube_get_rcnd( cube_t* cb );
cube_dyn_array*
cube_get_cnd( cube_t* cb );
cube_dyn_array*
cube_get_reg( cube_t* cb );

cube_dyn_array*
cube_get_root_stn( cube_t* cb );
cube_dyn_array*
cube_get_cart( cube_t* cb );
cube_dyn_array*
cube_get_mirr( cube_t* cb );
cube_att_array*
cube_get_attr( cube_t* cb );

/*  Set of methods to access the fill the cube in arbitrary order*/
cube_metric*
cube_get_metric( cube_t*  cube,
                 unsigned metricid );                                       /** Returns metric by its ID. Used to fill up the cube having only metric ids*/
cube_cnode*
cube_get_cnode( cube_t*  cube,
                unsigned cnodeid );                                         /** Returns cnode by its ID. Used to fill up the cube having only cnode ids*/
cube_region*
cube_get_region( cube_t*  cube,
                 unsigned regionid );                                       /** Returns region by its ID. */



void
cube_set_metrics_title( cube_t*     cube,
                        const char* title );                                       /** Saves the title of metrics dimension in cube report */
char*
cube_get_metrics_title( cube_t* cube );                                            /** Returns the name of statistic file in cube report. Do not makes any copy.  */

void
cube_set_calltree_title( cube_t*     cube,
                         const char* statfile );                                        /** Saves the name of statistic file in cube report */
char*
cube_get_calltree_title( cube_t* cube );                                                /** Returns the name of statistic file in cube report. Do not makes any copy.  */

void
cube_set_systemtree_title( cube_t*     cube,
                           const char* statfile );                                        /** Saves the name of statistic file in cube report */
char*
cube_get_systemtree_title( cube_t* cube );                                                /** Returns the name of statistic file in cube report. Do not makes any copy.  */

void
cube_set_statistic_name( cube_t*     cube,
                         const char* statfile );                                         /** Saves the name of statistic file in cube report */
char*
cube_get_statistic_name( cube_t* cube );                                                 /** Returns the name of statistic file in cube report. Do not makes any copy.  */



void
cube_enable_flat_tree( cube_t*           cube,
                       const enum bool_t status );                                             /** Enables or disables a flat tree */
enum bool_t
cube_is_flat_tree_enabled( cube_t* cube );                                                     /** Returns the status of flat tree */


/* Set of methods, which allow to fill the cube on optimal way.
    They delivers recomended sequence of calls.
    From the reason, that cube has separate metrics and writes row-wise, it delivers only cnode array
 */
carray*
cube_get_cnodes_for_metric( cube_t*      cube,
                            cube_metric* metric );                                    /** Returns a dynamic cube array of pointers on cnodes in the order of optimal writing*/

/* */
void
cube_set_known_cnodes_for_metric( cube_t*      cube,
                                  cube_metric* metric,
                                  char*        known_cnodes_as_bitstring );

/*   void            cube_assign_ids(cube_t* cb); */
/*   void            cube_write_all(cube_t* cb); */
void
cube_write_def( cube_t* cb );                                           /**  Writes a metypart of cube*/

void
cube_write_misc_data( cube_t*     cb,
                      const char* dataname,
                      char*       data,
                      uint64_t    len );


void
cube_write_sev_row( cube_t*      cube,
                    cube_metric* met,
                    cube_cnode*  cnd,
                    void*        sevs );                                                              /** Writes a general row of data for given metric and cnode*/
void
cube_write_sev_row_of_doubles( cube_t*      cube,
                               cube_metric* met,
                               cube_cnode*  cnd,
                               double*      sevs );                                                                 /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_uint64( cube_t*      cube,
                              cube_metric* met,
                              cube_cnode*  cnd,
                              uint64_t*    sevs );                                                                  /** Writes a  row of unsigned long long data for given metric and cnode. Performs proper type casting internally*/

void
cube_write_sev_row_of_int64( cube_t*      cube,
                             cube_metric* met,
                             cube_cnode*  cnd,
                             int64_t*     sevs );                                                                  /** Writes a  row of unsigned long long data for given metric and cnode. Performs proper type casting internally*/

/* List of the canonical writing calls. They perform type casting if needed and check metric if it supports that kind of value. Internally they call cube_write_sev_row(...)*/

void
cube_write_sev_row_of_cube_type_uint8( cube_t*          cube,
                                       cube_metric*     met,
                                       cube_cnode*      cnd,
                                       cube_type_uint8* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_int8( cube_t*         cube,
                                      cube_metric*    met,
                                      cube_cnode*     cnd,
                                      cube_type_int8* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_uint16( cube_t*           cube,
                                        cube_metric*      met,
                                        cube_cnode*       cnd,
                                        cube_type_uint16* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_int16( cube_t*          cube,
                                       cube_metric*     met,
                                       cube_cnode*      cnd,
                                       cube_type_int16* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_uint32( cube_t*           cube,
                                        cube_metric*      met,
                                        cube_cnode*       cnd,
                                        cube_type_uint32* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_int32( cube_t*          cube,
                                       cube_metric*     met,
                                       cube_cnode*      cnd,
                                       cube_type_int32* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_uint64( cube_t*           cube,
                                        cube_metric*      met,
                                        cube_cnode*       cnd,
                                        cube_type_uint64* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_int64( cube_t*          cube,
                                       cube_metric*     met,
                                       cube_cnode*      cnd,
                                       cube_type_int64* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_double( cube_t*           cube,
                                        cube_metric*      met,
                                        cube_cnode*       cnd,
                                        cube_type_double* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_tau_atomic( cube_t*               cube,
                                            cube_metric*          met,
                                            cube_cnode*           cnd,
                                            cube_type_tau_atomic* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_complex( cube_t*            cube,
                                         cube_metric*       met,
                                         cube_cnode*        cnd,
                                         cube_type_complex* sevs );                                                                      /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_scale_func( cube_t*               cube,
                                            cube_metric*          met,
                                            cube_cnode*           cnd,
                                            cube_type_scale_func* sevs );                                                                   /** Writes a  row of double data for given metric and cnode. Performs proper type casting internally*/

void
cube_write_sev_row_of_cube_type_rate( cube_t*         cube,
                                      cube_metric*    met,
                                      cube_cnode*     cnd,
                                      cube_type_rate* sevs );                                                                         /** Writes a  row of rate data for given metric and cnode. Performs proper type casting internally*/
void
cube_write_sev_row_of_cube_type_histogram( cube_t*           cube,
                                           cube_metric*      met,
                                           cube_cnode*       cnd,
                                           cube_type_double* sevs );

/*   void            cube_write_sev_row_uint32(cube_t * cube, cube_metric * met,  cube_cnode* cnd, uint32_t* sevs); */
/*   void            cube_write_sev_row_uint64(cube_t * cube, cube_metric * met,  cube_cnode* cnd, uint64_t* sevs); */

/*   void            cube_write_sev_matrix(cube_t* cb); */
/*   void            cube_write_sev_row(cube_t* cb, cube_metric* met, cube_cnode* cnd, double* sevs); */
/*    void            cube_write_sev_threads(cube_t* cb, cube_metric* met, cube_cnode* cnd, double* sevs); */
void
cube_write_finish( cube_t* cb );                                                                    /** Closes and flushes all metric files. */

#ifdef __cplusplus
}
#endif

#endif
