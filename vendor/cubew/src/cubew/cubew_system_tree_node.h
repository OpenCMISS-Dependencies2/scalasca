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
 * \file cubew_system_tree_node.h
 * \brief Declares types and functions to deal with general part of the system tree as whole object.
 */
#ifndef CUBEW_SYSTEM_TREE_NODE_H
#define CUBEW_SYSTEM_TREE_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cubew_types.h"
/**   struct cube_node; */


cube_system_tree_node*
cube_system_tree_node_create( cube_system_tree_node* stn );

#define cube_machine_create cube_system_tree_node_create
#define cube_node_create cube_system_tree_node_create



void
cube_system_tree_node_init( cube_system_tree_node* stn,
                            const char*            name,
                            const char*            desc,
                            const char*            stn_class,
                            cube_system_tree_node* parent );

#define cube_machine_init( X, Y, Z, W ) cube_system_tree_node_init( X, Y, Z, "machine", W )
#define cube_node_init( X, Y, Z, W ) cube_system_tree_node_init( X, Y, Z, "node", W )

void
cube_system_tree_node_reserve_nodes( cube_system_tree_node* stn,
                                     unsigned               num );

#define cube_machine_reserve_nodes cube_system_tree_node_reserve_nodes


void
cube_system_tree_node_free( cube_system_tree_node* stn );

#define cube_machine_free cube_system_tree_node_free
#define cube_node_free cube_system_tree_node_free



struct cube_system_tree_node*
cube_system_tree_node_get_child( cube_system_tree_node* stn,
                                 int                    i );
struct cube_location_group*
cube_system_tree_node_get_location_group( cube_system_tree_node* stn,
                                          int                    i );

char*
cube_system_tree_node_get_name( cube_system_tree_node* stn );
char*
cube_system_tree_node_get_desc( cube_system_tree_node* stn );
char*
cube_system_tree_node_get_class( cube_system_tree_node* stn );
int
cube_system_tree_node_num_children( cube_system_tree_node* stn );
int
cube_system_tree_node_num_location_group( cube_system_tree_node* stn );
int
cube_system_tree_node_get_level( cube_system_tree_node* stn );
void
cube_system_tree_node_writeXML( cube_system_tree_node* stn,
                                cube_meta_data_writer* writer );

#define cube_machine_writeXML cube_system_tree_node_writeXML
#define cube_node_writeXML cube_system_tree_node_writeXML
void
cube_system_tree_node_set_id( cube_system_tree_node* stn,
                              int                    new_id );

#define cube_machine_set_id cube_system_tree_node_set_id
#define cube_node_set_id cube_system_tree_node_set_id
int
cube_system_tree_node_get_id( cube_system_tree_node* stn );

#define cube_machine_get_id cube_system_tree_node_get_id
#define cube_node_get_id cube_system_tree_node_get_id
int
cube_system_tree_node_equal( cube_system_tree_node* a,
                             cube_system_tree_node* b );
void
cube_system_tree_node_add_child( cube_system_tree_node* parent,
                                 cube_system_tree_node* child );

void
cube_system_tree_node_add_group( cube_system_tree_node* parent,
                                 cube_location_group*   group );


void
cube_system_tree_node_add_attr( cube_system_tree_node* stn,
                                cmap*                  m );
void
cube_system_tree_node_def_attr( cube_system_tree_node* stn,
                                const char*            key,
                                const char*            value );


#ifdef __cplusplus
}
#endif

#endif
