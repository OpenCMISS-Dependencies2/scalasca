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
 * \file cubew_cartesian.h
   \brief Declares a types and functions to deal with cartesian topology.
 */
#ifndef CUBEW_CARTESIAN_H
#define CUBEW_CARTESIAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cubew_types.h"

/**
 * Defines cartesian topology of the threads.
 */

struct cube_cartesian
{
    char*          name;        /** Saves a name of the topology */
    unsigned       ndims;       /**< Number of dimensions */
    long int*      dim;         /**< Array with dimensions */
    char**         namedims;    /**< in the case one decides to give names for the dimensions of a topology */
    int*           period;      /**< Periond in ID in every dimension */
    cube_map_thrd* thrd2coord;  /**< Thread -> (Coordinates) Mapping */
};

cube_cartesian*
cube_cart_create( cube_cartesian* cart );                                                   /** alocates memory for the topology-structure */
int
cube_cart_init( cube_cartesian* cart,
                long int        ndims,
                long int*       dim,
                int*            period );                                                   /** sets elements of the topology */
void
cube_cart_free( cube_cartesian* cart );                                                     /** releases the alocated memory */
void
cube_cart_def_coords( cube_cartesian* cart,
                      cube_thread*    thrd,
                      long int*       coord );                                              /** creates a mapping "thread->coordinates" */
void
cube_cart_writeXML( cube_cartesian*        cart,
                    cube_meta_data_writer* writer );                                                   /** writes a xml description of the topology in the anchor file */
void
cube_cart_set_name( cube_cartesian* cart,
                    const char*     name );                                                 /** sets the another name than default to the topology. */
void
cube_cart_set_namedims( cube_cartesian* cart,
                        char**          _namedims );                                        /** sets the names for all dimensions. */
void
cube_cart_set_dim_name( cube_cartesian* cart,
                        const char*     _name,
                        int             dim );                                              /** sets the name for dimension "dim". */

#ifdef __cplusplus
}
#endif

#endif
