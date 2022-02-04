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
 * \file cubew_meta_data_writer.h
   \brief Provides API and data structures for unified meta data writing, if compressed or not.
 */
#ifndef CUBEW_META_DATA_WRITER_H
#define CUBEW_META_DATA_WRITER_H

#include <stdarg.h>
#include "cubew_types.h"

#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
#include <zlib.h>
#endif

extern
enum bool_t cubew_compressed;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A structure collecting information about a region: Start line, end line, description, url, name and so on.
 */
struct cube_meta_data_writer
{
    FILE*     file;
#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    char*     buffer;
    Bytef*    z_buffer;
    int       bufferlen;
    char*     pointer;
    z_stream* defstream;
#endif
};


cube_meta_data_writer*
create_meta_data_writer( FILE* fh );

void
__cube_write_meta_data( cube_meta_data_writer* writer,
                        char*                  format,
                        ... );

void
close_meta_data_writer( cube_meta_data_writer* writer );


#ifdef __cplusplus
}
#endif

#endif
