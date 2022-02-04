/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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
 * \file cubew_file_layout_embedded.h
 * \brief Provides names for avery part of cube report according to the "embedded layout: anchot.cubex + [metic id].index, [metric id].data
 *
 *
 */

#ifndef __CUBEW_FILE_LAYOUT_EMBEDDED_H
#define __CUBEW_FILE_LAYOUT_EMBEDDED_H 0

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "cubew_services.h"
#include "cubew_metric.h"

/**
 *
 */

char*
cube_get_path_to_anchor( const char* cubename );
char*
cube_get_path_to_metric_data( const char*  cubename,
                              cube_metric* met );
char*
cube_get_path_to_metric_index( const char*  cubename,
                               cube_metric* met );
char*
cube_get_path_to_misc_data( const char* cubename,
                            const char* dataname );

#endif
