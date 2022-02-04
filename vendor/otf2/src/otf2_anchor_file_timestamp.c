/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/** @internal
 *
 *  @file
 *
 *  @brief      Access the current time for the anchor file trace id.
 */

#include <config.h>

/* For clock_gettime */
#if HAVE( CLOCK_GETTIME )
#  ifdef _POSIX_C_SOURCE
#    if _POSIX_C_SOURCE < 199309L
#      undef _POSIX_C_SOURCE
#      define _POSIX_C_SOURCE 199309L
#    endif
#  else
#    define _POSIX_C_SOURCE 199309L
#  endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#if HAVE( CLOCK_GETTIME )
#  include <time.h>
#elif HAVE( GETTIMEOFDAY )
#  include <sys/time.h>
#endif

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <jenkins_hash.h>

void
otf2_anchor_file_timestamp( uint32_t* hashLower,
                            uint32_t* hashUpper )
{
#if HAVE( CLOCK_GETTIME )
    struct timespec tp;
    clock_gettime( CLOCK_REALTIME, &tp );
    jenkins_hashlittle2( &tp,
                         sizeof( tp ),
                         hashLower,
                         hashUpper );
#elif HAVE( GETTIMEOFDAY )
    struct timeval tv;
    gettimeofday( &tv, NULL );
    jenkins_hashlittle2( &tv,
                         sizeof( tv ),
                         hashLower,
                         hashUpper );
#endif
}
