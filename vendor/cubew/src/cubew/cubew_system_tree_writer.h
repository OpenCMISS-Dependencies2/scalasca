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
 * \file cubew_system_tree_writer.h
   \brief Provides the data structure traverser and system tree writer
 */
#ifndef CUBEW_SYSTEM_TREE_WRITER_H
#define CUBEW_SYSTEM_TREE_WRITER_H

// #include <config.h>
#include <stdint.h>
#include <sys/types.h>
#include <limits.h>
#include <float.h>

#include "cubew_types.h"
#include "cubew_meta_data_writer.h"
#include "cubew_system_tree_node_plain.h"
#include "cubew_location_group_plain.h"
#include "cubew_location_plain.h"

#ifdef __cplusplus
extern "C" {
#endif




cube_system_tree_writer*
cube_system_tree_writer_create();

void
cube_system_tree_writer_free( cube_system_tree_writer* );

void
cube_system_tree_writer_start( cube_system_tree_writer* );

void
cube_system_tree_writer_write( cube_system_tree_writer*,
                               cube_meta_data_writer* writer );
void
cube_system_tree_writer_driver_write( cube_system_tree_writer*,
                                      cube_meta_data_writer* writer );

void
cube_system_tree_writer_end( cube_system_tree_writer* );



void
cube_system_tree_writer_setup( cube_system_tree_writer*       writer,
                               cube_system_tree_writer_init   init,
                               cube_system_tree_writer_step   step,
                               cube_system_tree_writer_driver driver,
                               cube_system_tree_writer_finish finish,
                               void*                          user_ptr
                               );
void
cube_system_tree_writer_driver_step( void*                         writer,
                                     cube_system_tree_writer_state next_state,
                                     cube_system_tree_node_plain*  stn_plain,
                                     cube_location_group_plain*    lg_plain,
                                     cube_location_plain*          loc_plain );


#ifdef __cplusplus
}
#endif

#endif
