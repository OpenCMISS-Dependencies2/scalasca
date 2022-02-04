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
 * \file cubew_memory.c
   \brief Provides a stub function as a symbol
 */
#include "config.h"
#include "cubew_memory.h"

#ifndef HAVE_ADVANCED_MEMORY

void
cubew_init_allocs( void* ( *own_alloc )( size_t, void* ),
                   bool ( * is_external_mem )( void*, void* ),
                   void* ptr
                   )
{
}


int
cubew_initialized()
{
    return 0;
}


#endif
