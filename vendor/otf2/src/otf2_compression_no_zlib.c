/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
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
 *  @brief      Empty compression layer for zlib.
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "OTF2_Compression_Zlib.h"


bool
OTF2_Compression_Zlib_Available( void )
{
    return false;
}


OTF2_ErrorCode
OTF2_Compression_Zlib_Compress( const void*    buffer,
                                const uint64_t size,
                                void**         compBuffer,
                                uint32_t*      compSize )
{
    return UTILS_ERROR( OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED,
                        "Using zlib without library support." );
}


OTF2_ErrorCode
OTF2_Compression_Zlib_Decompress( const void*    compBuffer,
                                  const uint64_t compSize,
                                  void**         buffer,
                                  uint32_t       size )
{
    return UTILS_ERROR( OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED,
                        "Using zlib without library support." );
}
