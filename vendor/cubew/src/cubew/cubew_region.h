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
 * \file cubew_region.h
   \brief Declares types and functions to deal with regions in source code of running application.
 */
#ifndef CUBEW_REGION_H
#define CUBEW_REGION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cubew_types.h"

/**
 * A structure collecting information about a region: Start line, end line, description, url, name and so on.
 */
struct cube_region
{
    char*           name;
    char*           mangled_name;
    int             begln;
    int             endln;
    char*           url;
    char*           descr;
    char*           mod;
    char*           paradigm;
    char*           role;
    int             id;
    cube_rarray*    cnode;

    cube_att_array* attr;
};


cube_region*
cube_region_create( cube_region* reg );
void
cube_region_init( cube_region* reg,
                  const char*  name,
                  const char*  mangled_name,
                  const char*  paradigm,
                  const char*  role,
                  int          begln,
                  int          endln,
                  const char*  url,
                  const char*  descr,
                  const char*  mod );

void
cube_region_free( cube_region* reg );

char*
cube_region_get_name( cube_region* reg );
char*
cube_region_get_mangled_name( cube_region* reg );
char*
cube_region_get_paradigm( cube_region* reg );
char*
cube_region_get_role( cube_region* reg );
char*
cube_region_get_url( cube_region* reg );
char*
cube_region_get_descr( cube_region* reg );
char*
cube_region_get_mod( cube_region* reg );
int
cube_region_get_begn_ln( cube_region* reg );
int
cube_region_get_end_ln( cube_region* reg );
int
cube_region_num_children( cube_region* reg );
void
cube_region_add_cnode( cube_region* reg,
                       cube_cnode*  cnode );
void
cube_region_writeXML( cube_region*           reg,
                      cube_meta_data_writer* writer );
int
cube_region_equal( cube_region* a,
                   cube_region* b );
void
cube_region_set_id( cube_region* reg,
                    int          new_id );
int
cube_region_get_id( cube_region* reg );

void
cube_region_add_attr( cube_region* reg,
                      cmap*        m );
void
cube_region_def_attr( cube_region* reg,
                      const char*  key,
                      const char*  value );


#ifdef __cplusplus
}
#endif

#endif
