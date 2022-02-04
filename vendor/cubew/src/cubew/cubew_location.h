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
 * \file cubew_location.h
   \brief Declares types and functions to deal with locations of running application.
 */
#ifndef CUBEW_LOCATION_H
#define CUBEW_LOCATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cubew_types.h"


cube_location*
cube_location_create( cube_location* thrd );

#define cube_thread_create cube_location_create

void
cube_location_init( cube_location*              thrd,
                    const char*                 name,
                    int                         rank,
                    cube_location_type          type,
                    struct cube_location_group* parent );

#define cube_thread_init( X, Y, Z, W ) cube_location_init( X, Y, Z, "thread", W )


void
cube_location_free( cube_location* thrd );

#define cube_thread_free cube_location_free


struct cube_location_group*
cube_location_get_parent( cube_location* thrd );
char*
cube_location_get_name( cube_location* thrd );
cube_location_type
cube_location_get_type( cube_location* thrd );
int
cube_location_get_rank( cube_location* thrd );

int
cube_location_get_level( cube_location* thrd );
void
cube_location_writeXML( cube_location*         thrd,
                        cube_meta_data_writer* writer );
void
cube_location_set_id( cube_location* thrd,
                      int            new_id );

#define cube_thread_set_id cube_location_set_id

int
cube_location_get_id( cube_location* thrd );

#define cube_thread_get_id cube_location_get_id

void
cube_location_add_child( struct cube_location_group* parent,
                         cube_location*              thrd );
int
cube_location_equal( cube_location* a,
                     cube_location* b );

void
cube_location_add_attr( cube_location* location,
                        cmap*          m );
void
cube_location_def_attr( cube_location* location,
                        const char*    key,
                        const char*    value );


#ifdef __cplusplus
}
#endif

#endif
