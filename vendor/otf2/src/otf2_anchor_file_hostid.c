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
 *  @brief      Access the host ID for the anchor file trace id.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#if HAVE( UNISTD_H )
#  include <unistd.h>
#endif

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <jenkins_hash.h>

#define OTF2_MAX_GETHOSTID_RETRIES 10

/* Provide gethostid declaration if we are able to
 * link against gethostid, but found no declaration */
#if !( HAVE( DECL_GETHOSTID ) ) && HAVE( GETHOSTID )

extern long
gethostid( void );

#endif

void
otf2_anchor_file_hostid( uint32_t* hashLower,
                         uint32_t* hashUpper )
{
#if HAVE( GETHOSTID )
    long     host_id        = 0;
    uint32_t hostid_retries = 0;

    /* On some machines gethostid() may fail if
     * called only once, try it multiple times */
    while ( !host_id && ( hostid_retries++ < OTF2_MAX_GETHOSTID_RETRIES ) )
    {
        host_id = gethostid();
    }
    if ( !host_id )
    {
        jenkins_hashlittle2( &host_id,
                             sizeof( host_id ),
                             hashLower,
                             hashUpper );
    }
#endif
}
