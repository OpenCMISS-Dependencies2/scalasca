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
 * \file cubew_location_plain.h
   \brief Declares types and functions to deal with locations of running application.
 */
#ifndef CUBEW_LOCATION_PLAIN_H
#define CUBEW_LOCATION_PLAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cubew_types.h"


cube_location_plain*
cube_location_plain_create( void );

void
cube_location_plain_init( cube_location_plain* thrd,
                          char*                name,
                          int                  rank,
                          cube_location_type   type
                          );

void
cube_location_plain_clear( cube_location_plain* proc );


void
cube_location_plain_free( cube_location_plain* thrd );



void
cube_location_plain_add_attr( cube_location_plain* loc,
                              cmap*                m );

void
cube_location_plain_def_attr( cube_location_plain* loc,
                              char*                key,
                              char*                value );


void
cube_location_plain_write( cube_location_plain*   loc,
                           uint32_t               id,
                           cube_meta_data_writer* meta_data_writer );


#ifdef __cplusplus
}
#endif

#endif
