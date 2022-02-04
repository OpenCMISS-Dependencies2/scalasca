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
 * \file cubew_system_tree_node_plain.h
 * \brief Declares types and functions to deal with general part of the system tree as whole object.
 */
#ifndef CUBEW_SYSTEM_TREE_NODE_PLAIN_H
#define CUBEW_SYSTEM_TREE_NODE_PLAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cubew_types.h"
/**   struct cube_node; */


cube_system_tree_node_plain*
cube_system_tree_node_plain_create( void );


void
cube_system_tree_node_plain_init( cube_system_tree_node_plain* stn,
                                  char*                        name,
                                  char*                        desc,
                                  char*                        stn_class );


void
cube_system_tree_node_plain_free( cube_system_tree_node_plain* stn );

void
cube_system_tree_node_plain_clear( cube_system_tree_node_plain* stn );


void
cube_system_tree_node_plain_add_attr( cube_system_tree_node_plain* stn,
                                      cmap*                        m );

void
cube_system_tree_node_plain_def_attr( cube_system_tree_node_plain* stn,
                                      char*                        key,
                                      char*                        value );

void
cube_system_tree_node_plain_write( cube_system_tree_node_plain* loc,
                                   uint32_t                     id,
                                   cube_meta_data_writer*       meta_data_writer );

#ifdef __cplusplus
}
#endif

#endif
