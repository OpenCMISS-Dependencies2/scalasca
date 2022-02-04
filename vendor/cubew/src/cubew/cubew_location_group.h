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
 * \file cubew_location_group.h
   \brief Declares types and functions to deal with location group of running application.
 */
#ifndef CUBEW_LOCATION_GROUP_H
#define CUBEW_LOCATION_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cubew_types.h"

cube_location_group*
cube_location_group_create( cube_location_group* proc );

#define cube_process_create cube_location_group_create
void
cube_location_group_init( cube_location_group*          proc,
                          const char*                   name,
                          int                           rank,
                          cube_location_group_type      type,
                          struct cube_system_tree_node* parent );

#define cube_process_init( X, Y, Z, W ) cube_location_group_init( X, Y, Z, CUBE_LOCATION_GROUP_TYPE_PROCESS, W )

void
cube_location_group_free( cube_location_group* proc );

#define cube_process_free cube_location_group_free

cube_location*
cube_location_group_get_child( cube_location_group* proc,
                               int                  i );
struct cube_system_tree_node*
cube_location_group_get_parent( cube_location_group* proc );
int
cube_location_group_get_rank( cube_location_group* proc );
char*
cube_location_group_get_name( cube_location_group* proc );
cube_location_group_type
cube_location_group_get_type( cube_location_group* proc );
int
cube_location_group_num_children( cube_location_group* proc );
int
cube_location_group_get_level( cube_location_group* proc );
void
cube_location_group_writeXML( cube_location_group*   proc,
                              cube_meta_data_writer* writer );
void
cube_location_group_set_id( cube_location_group* proc,
                            int                  new_id );

#define cube_process_set_id cube_location_group_set_id
int
cube_location_group_get_id( cube_location_group* proc );

#define cube_process_get_id cube_location_group_get_id
int
cube_location_group_equal( cube_location_group* a,
                           cube_location_group* b );
void
cube_location_group_add_child( struct cube_system_tree_node* parent,
                               cube_location_group*          proc );


void
cube_location_group_add_attr( cube_location_group* lg,
                              cmap*                m );
void
cube_location_group_def_attr( cube_location_group* lg,
                              const char*          key,
                              const char*          value );



#ifdef __cplusplus
}
#endif

#endif
