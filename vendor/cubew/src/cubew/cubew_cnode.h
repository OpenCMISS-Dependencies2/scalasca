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
 * \file cubew_cnode.h
   \brief Declares a types and functions to deal with calee node in the cube.
 */
#ifndef CUBEW_CNODE_H
#define CUBEW_CNODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cubew_types.h"


/**
 * Synonym for array of cnodes.
 */

struct cube_cnode
{
    struct cube_region* callee;            /**< Calee of this cnode. */
    struct cube_cnode*  parent;            /**< This cnode belongs to cnode "parents". */
    cube_carray*        child;             /**< It has some children. */
    char*               mod;               /**< ?????? */
    int                 line;              /**< Start of the source code lines. */
    int                 id;                /**< id of the cnode. */
    nparam_parray*      num_parameters;    /**<  List of numerical parameters*/
    cparam_parray*      string_parameters; /**<  List of numerical parameters*/
    cube_att_array*     attr;              /**<  List of attributes  */
};


cube_cnode*
cube_cnode_create( cube_cnode* cnode );
void
cube_cnode_init( cube_cnode*         cnode,
                 struct cube_region* callee,
                 const char*         mod,
                 int                 line,
                 cube_cnode*         parent );
void
cube_cnode_add_numeric_parameter( cube_cnode* cnode,
                                  const char* param_name,
                                  double      value );
void
cube_cnode_add_string_parameter( cube_cnode* cnode,
                                 const char* param_name,
                                 const char* value );

void
cube_cnode_free( cube_cnode* cnode );

cube_cnode*
cube_cnode_get_child( cube_cnode* cnode,
                      int         i );
cube_cnode*
cube_cnode_get_parent( cube_cnode* cnode );
int
cube_cnode_get_line( cube_cnode* cnode );
int
cube_cnode_num_children( cube_cnode* cnode );
char*
cube_cnode_get_mod( cube_cnode* cnode );
struct cube_region*
cube_cnode_get_callee( cube_cnode* cnode );
struct cube_region*
cube_cnode_get_caller( cube_cnode* cnode );
void
cube_cnode_writeXML( cube_cnode*            cnode,
                     cube_meta_data_writer* writer );
void
cube_cnode_add_child( cube_cnode* parent,
                      cube_cnode* cnode );
int
cube_cnode_equal( cube_cnode* a,
                  cube_cnode* b );
void
cube_cnode_set_id( cube_cnode* cnode,
                   int         new_id );
int
cube_cnode_get_id( cube_cnode* cnode );
int
cube_cnode_get_level( cube_cnode* cnode );

void
cube_cnode_add_attr( cube_cnode* cnode,
                     cmap*       m );
void
cube_cnode_def_attr( cube_cnode* cnode,
                     const char* key,
                     const char* value );


#ifdef __cplusplus
}
#endif

#endif
