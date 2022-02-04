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
 * \file cubew_memory.h
   \brief Provides different headers for different memory allocation
   functions, depending on configure options
 */
#ifndef CUBEW_MEMORY_H
#define CUBEW_MEMORY_H
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef HAVE_ADVANCED_MEMORY /* use ofwn wrappers only if macro is defined*/
#include "cubew_allocs.h"
#define CUBEW_MALLOC( x, y ) cubew_malloc( ( x ), ( y ) )
#define CUBEW_CALLOC( x, y, z ) cubew_calloc( ( x ), ( y ), ( z ) )
#define CUBEW_REALLOC( x, y, z, w ) cubew_realloc( ( x ), ( y ), ( z ), ( w ) )
#define CUBEW_FREE( x, y ) cubew_free( ( x ), ( y )  )

#else /* if no macro is defined -> use standard calls */
#define CUBEW_MALLOC( x, y ) malloc( x )
#define CUBEW_CALLOC( x, y, z ) calloc( ( x ), ( y ) )
#define CUBEW_REALLOC( x, y, z, w ) realloc( ( x ), ( z ) )
#define CUBEW_FREE( x, w ) free( x )

/* define some calls as empty macros */
#define cubew_report_allocs()
int
cubew_initialized();

void
cubew_init_allocs( void* ( *own_alloc )( size_t, void* ),
                   bool ( * is_external_mem )( void*, void* ),
                   void*
                   );

#endif

#ifdef __cplusplus
}
#endif

#endif
