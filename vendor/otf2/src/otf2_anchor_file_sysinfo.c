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
 *  @brief      Access the current uptime for the anchor file trace id.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#if HAVE( SYS_SYSINFO_H )
#  include <sys/sysinfo.h>
#endif

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <jenkins_hash.h>

void
otf2_anchor_file_sysinfo( uint32_t* hashLower,
                          uint32_t* hashUpper )
{
#if HAVE( SYSINFO )
    struct sysinfo info;
    if ( 0 == sysinfo( &info ) )
    {
        /* add the uptime, or the complete struct? */
        jenkins_hashlittle2( &info.uptime,
                             sizeof( info.uptime ),
                             hashLower,
                             hashUpper );
    }
#endif
}
