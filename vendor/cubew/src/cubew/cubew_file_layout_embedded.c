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
 * \file cubew_file_layout_embedded.c
 * \brief Provides names for avery part of cube report according to the "embedded layout: anchot.cubex + [metic id].index, [metric id].data
 *
 *
 */

#include "config.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "UTILS_Error.h"

#include "cubew_file_layout_embedded.h"

/**
 *
 */

char*
cube_get_path_to_anchor( const char* cubename )
{
    if ( cubename == NULL )
    {
        UTILS_WARNING( "WARNING: cubename in the call \"cube_get_path_to_anchor\" is NULL.\n" );
    }
    char* to_return = cubew_strdup( "anchor.xml" );
    return to_return;
}

char*
cube_get_path_to_metric_data( const char* cubename,  cube_metric* met )
{
    if ( cubename == NULL )
    {
        UTILS_WARNING( "WARNING: cubename in the call \"cube_get_path_to_metric_data\" is NULL.\n" );
    }
    unsigned id = cube_metric_get_id( met );
    char     buf[ 50 ];
    memset( buf, 0, 50 );
    if ( cube_metric_get_viz_type( met ) == CUBE_METRIC_NORMAL )
    {
        sprintf( buf, "%u.data", id );
    }
    else if ( cube_metric_get_viz_type( met ) == CUBE_METRIC_GHOST )
    {
        sprintf( buf, "ghost_%u.data", id );
    }
    char* to_return = cubew_strdup( buf );
    return to_return;
}


char*
cube_get_path_to_metric_index( const char* cubename,   cube_metric* met )
{
    if ( cubename == NULL )
    {
        UTILS_WARNING(  "WARNING: cubename in the call \"cube_get_path_to_metric_index\" is NULL.\n" );
    }
    unsigned id = cube_metric_get_id( met );
    char     buf[ 50 ];
    memset( buf, 0, 50 );
    if ( cube_metric_get_viz_type( met ) == CUBE_METRIC_NORMAL )
    {
        sprintf( buf, "%u.index", id );
    }
    else if ( cube_metric_get_viz_type( met ) == CUBE_METRIC_GHOST )
    {
        sprintf( buf, "ghost_%u.index", id );
    }
    char* to_return = cubew_strdup( buf );
    return to_return;
}

char*
cube_get_path_to_misc_data( const char* cubename,   const char* dataname )
{
    if ( cubename == NULL )
    {
        UTILS_WARNING( "WARNING: cubename in the call \"cube_get_path_to_misc_data\" is NULL.\n" );
    }
    char* to_return = cubew_strdup( dataname );
    return to_return;
}
