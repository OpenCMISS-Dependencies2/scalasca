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
 * \file cubew_meta_data_writer.c
   \brief Provides implementation of API for unified meta data writing, if compressed or not.
 */
#include "config.h"

#define META_DATA_WRITER_BUFFER_CHUNK_SIZE 1024*1024*2 /* 2 Mb for buffer  - to avoid unnecessary reallocs*/
#include <stdlib.h>
#include "cubew_meta_data_writer.h"
#include "cubew_memory.h"
#include "UTILS_Error.h"

#define MEMORY_TRACING_PREFIX "[META DATA WRITER]"

cube_meta_data_writer*
create_meta_data_writer( FILE* fh )
{
    cube_meta_data_writer* _writer = ( cube_meta_data_writer* )CUBEW_MALLOC( sizeof( cube_meta_data_writer ), MEMORY_TRACING_PREFIX "Allocate meta data writer" );
    _writer->file = fh;
#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    if ( cubew_compressed == CUBE_TRUE )
    {
        _writer->buffer    = ( char* )CUBEW_MALLOC( META_DATA_WRITER_BUFFER_CHUNK_SIZE, MEMORY_TRACING_PREFIX "Allocate buffer for meta data writer" );
        _writer->bufferlen = META_DATA_WRITER_BUFFER_CHUNK_SIZE;
        _writer->pointer   = _writer->buffer; /* we start to write at the beginning of the buffer */


        _writer->z_buffer = ( Bytef* )CUBEW_MALLOC( META_DATA_WRITER_BUFFER_CHUNK_SIZE, MEMORY_TRACING_PREFIX "Allocate buffer for compressed meta data" );

        _writer->defstream         = ( z_stream* )CUBEW_MALLOC( sizeof( z_stream ), MEMORY_TRACING_PREFIX "Allocate z_stream" );
        _writer->defstream->zalloc = Z_NULL;
        _writer->defstream->zfree  = Z_NULL;
        _writer->defstream->opaque = Z_NULL;

        int ret = deflateInit2(  _writer->defstream,
                                 Z_BEST_SPEED,
                                 Z_DEFLATED,
                                 31, /* 10 + 16 */
                                 8,  /* default level */
                                 Z_DEFAULT_STRATEGY
                                 );


        if ( ret != Z_OK )
        {
            UTILS_FATAL( "ZLib initialization returned error %d\n", ret );
        }
    }
#endif
    return _writer;
}

void
__cube_write_meta_data( cube_meta_data_writer* writer, char* format, ... )
{
    va_list fmtargs;
    va_start( fmtargs, format );
#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    if ( cubew_compressed == CUBE_TRUE )
    {
        int ret = 0;
        int len = 0;
        len = vsnprintf( NULL, 0, format, fmtargs );
        va_end( fmtargs );
        if ( len >= writer->bufferlen - ( writer->pointer - writer->buffer ) )
        {   /*  compress sofar collected buffer and drop it into file */
            /* here we compress the buffer */
            uLongf uncompressed_buffer_size =  writer->pointer - writer->buffer;
            writer->defstream->avail_in = ( uInt )uncompressed_buffer_size;    // size of input
            writer->defstream->next_in  = ( Bytef* )( writer->buffer );        // input char array
            do
            {
                writer->defstream->next_out  = writer->z_buffer;
                writer->defstream->avail_out = ( uInt )( META_DATA_WRITER_BUFFER_CHUNK_SIZE ); // size of output
                ret                          = deflate( writer->defstream, Z_NO_FLUSH );       /* no bad return value */
                UTILS_ASSERT( ret != Z_STREAM_ERROR );                                         /* state not clobbered */
                int have = META_DATA_WRITER_BUFFER_CHUNK_SIZE -  writer->defstream->avail_out;
                if ( fwrite( ( char* )( writer->z_buffer ), 1, ( size_t )have, writer->file ) != have || ferror( writer->file ) )
                {
                    UTILS_FATAL( "Error while zlib comression %d\n", Z_ERRNO );
                }
            }
            while ( writer->defstream->avail_out == 0 );
            UTILS_ASSERT( writer->defstream->avail_in == 0 ); /* all input will be used */
            writer->pointer = writer->buffer;                 /* next time we start to write at the beginning of the buffer */
        }
        /* write */
        va_start( fmtargs, format );
        vsprintf( writer->pointer, format, fmtargs );
        writer->pointer += len;
    }
    else
    {
        vfprintf( writer->file, format, fmtargs );
    }
#else
    vfprintf( writer->file, format, fmtargs );
#endif
    va_end( fmtargs );
}

void
close_meta_data_writer( cube_meta_data_writer* writer )
{
#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    if ( cubew_compressed == CUBE_TRUE )
    {
        int ret = 0;
        if ( ( writer->pointer - writer->buffer )  != 0 )
        {   /* we have to write leftovers of buffer  */
            /*  compress sofar collected buffer and drop it into file */
            /* here we compress the buffer */
            uLongf uncompressed_buffer_size =  writer->pointer - writer->buffer;
            writer->defstream->avail_in = ( uInt )uncompressed_buffer_size;    // size of input
            writer->defstream->next_in  = ( Bytef* )( writer->buffer );        // input char array
            do
            {
                writer->defstream->next_out  = writer->z_buffer;
                writer->defstream->avail_out = ( uInt )( META_DATA_WRITER_BUFFER_CHUNK_SIZE ); // size of output
                ret                          = deflate( writer->defstream, Z_FINISH );         /* no bad return value */
                UTILS_ASSERT( ret != Z_STREAM_ERROR );                                         /* state not clobbered */
                int have = META_DATA_WRITER_BUFFER_CHUNK_SIZE -  writer->defstream->avail_out;
                if ( fwrite( ( char* )( writer->z_buffer ), 1, ( size_t )have, writer->file ) != have || ferror( writer->file ) )
                {
                    UTILS_FATAL( "Error while zlib compression %d\n", Z_ERRNO );
                }
            }
            while ( writer->defstream->avail_out == 0 );
            UTILS_ASSERT( writer->defstream->avail_in == 0 );     /* all input will be used */
            UTILS_ASSERT( ret == Z_STREAM_END );
        }
        deflateEnd( writer->defstream );
        CUBEW_FREE( writer->defstream, MEMORY_TRACING_PREFIX "Release z_stream" );
        CUBEW_FREE( writer->z_buffer, MEMORY_TRACING_PREFIX "Release compressed buffer of a meta data writer"  );
        CUBEW_FREE( writer->buffer, MEMORY_TRACING_PREFIX "Release buffer of a meta data writer"  );
    }
#endif
    CUBEW_FREE( writer, MEMORY_TRACING_PREFIX "Release meta data writer"  );
}
