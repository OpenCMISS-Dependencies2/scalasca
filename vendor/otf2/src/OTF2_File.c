/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
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
 *  @brief      File layer.
 *
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#if HAVE( SYS_STAT_H )
#include <sys/stat.h>
#endif
#if HAVE( IO_H )
#include <io.h>
#endif

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"

#include "OTF2_File.h"
#include "otf2_file_int.h"

#include "OTF2_Compression_Zlib.h"


/** @brief Internal compression header.
 *  The zlib flush point is fixed to 4 bytes: 0x00 0x00 0xff 0xff. In the two
 *  bytes in front of it is a 3 bit header not equal to zero and 0-7 bits equal
 *  to zero to enable byte alignment.
 *  So by putting 2 zero bytes in front of the header we assure that this header
 *  does not clash with zlib's flush point. By adding 2 one bytes at the end we
 *  ensure that the header does not clash with the header content (This pattern
 *  would only emerge if a compressed block would be of size 4GB which is
 *  impossible). This way the header is distinctive to any other data. */
#define OTF2_COMPRESSION_HEADER UINT64_C( 0x00000000ffffffff )

/** @brief Defines size of an internal compression header. */
#define OTF2_COMPRESSION_HEADER_SIZE ( 20 )



/* ___ Prototypes of static functions. ______________________________________ */

static OTF2_ErrorCode
otf2_file_write_compression_header( OTF2_File* file,
                                    uint32_t   compSize,
                                    uint32_t   uncompSize );

static OTF2_ErrorCode
otf2_file_read_compression_header( OTF2_File* file,
                                   uint32_t*  compSize,
                                   uint32_t*  uncompSize );

static inline uint32_t*
otf2_swap32( uint32_t* value );

static OTF2_ErrorCode
otf2_file_write_buffered( OTF2_File*  file,
                          const void* buffer,
                          uint64_t    size );

static OTF2_ErrorCode
otf2_file_create_directory( const char* mainPath );


/* ___ File interaction routines. ___________________________________________ */


/**
 *  Initializes common parts or an OTF2_File object.
 *  Never fails.
 */
void
otf2_file_initialize( OTF2_Archive*    archive,
                      OTF2_File*       file,
                      OTF2_FileType    fileType,
                      OTF2_LocationRef location )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( file );

    /* Initialize common file data. */
    file->archive     = archive;
    file->compression = OTF2_COMPRESSION_NONE;
    if ( !otf2_file_type_ignores_compression( fileType ) )
    {
        file->compression = archive->compression;
    }
    file->file_type   = fileType;
    file->location_id = location;

    file->buffer      = NULL;
    file->buffer_used = 0;

    file->last_block_size = 0;
}


OTF2_ErrorCode
otf2_file_finalize( OTF2_File* file )
{
    /* Write remaining data in buffer to file. */
    if ( file->buffer_used > 0 )
    {
        OTF2_ErrorCode status = file->write( file, file->buffer, file->buffer_used );
        if ( status != OTF2_SUCCESS )
        {
            UTILS_ERROR( status,
                         "Write of buffered data failed!" );
        }

        free( file->buffer );
    }

    return OTF2_SUCCESS;
}


/** @brief Reset a file.
 *
 *  Resets an opened file, i.e. the file pointer is set to the beginning of the
 *  file and the file size is set to zero.
 *
 *  Calls the specific file reset routine defined by the file substrate.
 *
 *  @param file             OTF2 file handle.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
OTF2_File_Reset( OTF2_File* file )
{
    /* Validate arguments. */
    if ( NULL == file )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid file handle!" );
    }

    /* Reset write buffer. */
    free( file->buffer );
    file->buffer_used = 0;

    return file->reset( file );
}


/** @brief Write to a file.
 *
 *  Writes 'size' bytes from the given 'buffer' to the 'file'.
 *
 *  Calls the specific file write routine defined by the file substrate.
 *
 *  @param file             OTF2 file handle.
 *  @param buffer           Pointer to the buffer that should be written.
 *  @param size             Size of the buffer to be written in bytes.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
OTF2_File_Write( OTF2_File*  file,
                 const void* buffer,
                 uint64_t    size )
{
    /* Validate arguments. */
    if ( NULL == file )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid file handle!" );
    }

    if ( NULL == buffer )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid buffer pointer!" );
    }

    if ( 0 == size )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Zero bytes to write!" );
    }

    /* If no compression is selected, write uncompresed data and return. */
    if ( file->compression == OTF2_COMPRESSION_NONE )
    {
        return otf2_file_write_buffered( file, buffer, size );
    }

    if ( OTF2_Compression_Zlib_Available()
         && file->compression == OTF2_COMPRESSION_ZLIB )
    {
        /* Call compression method. */
        void*    comp_buffer = NULL;
        uint32_t comp_size   = 0;

        OTF2_ErrorCode status = OTF2_Compression_Zlib_Compress( buffer, size, &comp_buffer, &comp_size );
        if ( status != OTF2_SUCCESS )
        {
            return status;
        }

        status = otf2_file_write_compression_header( file, comp_size, size );
        if ( status != OTF2_SUCCESS )
        {
            free( comp_buffer );
            return status;
        }
        status = otf2_file_write_buffered( file, comp_buffer, size );
        if ( status != OTF2_SUCCESS )
        {
            free( comp_buffer );
            return status;
        }

        file->last_block_size = comp_size;
        free( comp_buffer );

        return status;
    }

    return UTILS_ERROR( OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED,
                        "Requesting to operate on a compressed file without library support." );
}


/** @brief Read from a file.
 *
 *  Reads 'size' bytes from the given 'file' to the 'buffer'.
 *
 *  Calls the specific file read routine defined by the file substrate.
 *
 *  @param file             OTF2 file handle.
 *  @param buffer           Pointer to the buffer the data should be written to.
 *  @param size             Size of the buffer to be read in bytes.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
OTF2_File_Read( OTF2_File* file,
                void*      buffer,
                uint64_t   size )
{
    /* Validate arguments. */
    if ( NULL == file )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid file handle!" );
    }

    if ( NULL == buffer )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid buffer pointer!" );
    }

    if ( 0 == size )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Zero bytes to read!" );
    }

    /* If no compression is selected, read uncompresed data and return. */
    if ( file->compression == OTF2_COMPRESSION_NONE )
    {
        return file->read( file, buffer, size );
    }

    if ( OTF2_Compression_Zlib_Available()
         && file->compression == OTF2_COMPRESSION_ZLIB )
    {
        /* Call compression method. */
        void*    comp_buffer = NULL;
        uint32_t comp_size   = 0;

        OTF2_ErrorCode status = otf2_file_read_compression_header( file, &comp_size, ( uint32_t* )&size );
        if ( status != OTF2_SUCCESS )
        {
            return status;
        }

        comp_buffer = ( void* )malloc( comp_size );

        status = file->read( file, comp_buffer, comp_size );
        if ( status != OTF2_SUCCESS )
        {
            free( comp_buffer );
            return status;
        }

        status = OTF2_Compression_Zlib_Decompress( comp_buffer, comp_size, &buffer, size );
        free( comp_buffer );
        if ( status != OTF2_SUCCESS )
        {
            return UTILS_ERROR( status,
                                "Can't decompress from buffer." );
        }

        return status;
    }

    return UTILS_ERROR( OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED,
                        "Requesting to operate on a compressed file without library support." );
}


/** @brief Get size of (uncompressed) unchunked file content.
 *
 *  This function is used by the buffer to determine the size that has to be
 *  allocated for a definition buffer. For uncompressed file content this
 *  function simply provides the file size. For compressed data it provides the
 *  size the decompressed data.
 *
 *  @param file             OTF2 file handle.
 *  @param size             Return value for the file content size.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
OTF2_File_GetSizeUnchunked( OTF2_File* file,
                            uint64_t*  size )
{
    if ( !file || !size )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments!" );
    }

    if ( file->compression == OTF2_COMPRESSION_NONE )
    {
        return file->get_file_size( file, size );
    }

    if ( OTF2_Compression_Zlib_Available()
         && file->compression == OTF2_COMPRESSION_ZLIB )
    {
        uint32_t       comp_size   = 0;
        uint32_t       uncomp_size = 0;
        OTF2_ErrorCode status      =
            otf2_file_read_compression_header( file, &comp_size, &uncomp_size );
        if ( status != OTF2_SUCCESS )
        {
            return UTILS_ERROR( status,
                                "Failed to read compression header!" );
        }
        status = file->seek( file, 0 );
        if ( status != OTF2_SUCCESS )
        {
            return UTILS_ERROR( status,
                                "Failed to seek to head of file!" );
        }

        *size = ( uint64_t )uncomp_size;
        return OTF2_SUCCESS;
    }

    return UTILS_ERROR( OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED,
                        "Requesting to operate on a compressed file without library support." );
}


/** @brief Seek to the begin of the previous chunk in a file.
 *
 *  For uncompressed data the file pointer is just set twice the chunk size
 *  back. For compressed data, the compression header is evaluated and the file
 *  pointer is set back the size of the compressed data. This is also done
 *  twice.
 *
 *  @param file             OTF2 file handle.
 *  @param chunkNumber      Number of current chunk.
 *  @param chunkSize        Size of uncompressed chunks.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
OTF2_File_SeekPrevChunk( OTF2_File* file,
                         uint64_t   chunkNumber,
                         uint64_t   chunkSize )
{
    if ( !file || !chunkSize || ( chunkNumber < 2 ) )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid arguments!" );
    }

    if ( file->compression == OTF2_COMPRESSION_NONE )
    {
        /* First chunkNumber is '1', therefore, substract 2 */
        return file->seek( file, ( chunkNumber - 2 ) * chunkSize );
    }

    /* TODO: Seek needs to be replaced by a version that uses OTF2_SEEK_SET
     * instead of OTF2_SEEK_CUR: */
#if 0
    if ( OTF2_Compression_Zlib_Available()
         && file->compression == OTF2_COMPRESSION_ZLIB )
    {
        uint32_t comp_size   = 0;
        uint32_t uncomp_size = 0;

        OTF2_ErrorCode status = otf2_file_read_compression_header( file, &comp_size, &uncomp_size );
        status = file->seek( file, ( -1 ) * file->last_block_size + OTF2_COMPRESSION_HEADER_SIZE, OTF2_SEEK_CUR );
        status = otf2_file_read_compression_header( file, &comp_size, &uncomp_size );
        status = file->seek( file, ( -1 ) * file->last_block_size + OTF2_COMPRESSION_HEADER_SIZE, OTF2_SEEK_CUR );

        return status;
    }
#endif

    return UTILS_ERROR( OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED,
                        "Requesting to operate on a compressed file without library support." );
}


OTF2_ErrorCode
OTF2_File_SeekChunk( OTF2_File* file,
                     uint64_t   chunkNumber,
                     uint64_t   chunkSize )
{
    if ( !file || !chunkSize )
    {
        return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                            "Invalid arguments!" );
    }

    if ( file->compression == OTF2_COMPRESSION_NONE )
    {
        return file->seek( file, ( chunkNumber - 1 ) * chunkSize );
    }

    return UTILS_ERROR( OTF2_ERROR_FILE_INTERACTION,
                        "Operation not supported for compressed trace files!" );
}

/* ___ Implementation of static functions. __________________________________ */


/** @brief Write compression header.
 *
 *  Writes the size of the current and previous compression block.
 *
 *  @param file             OTF2 file handle.
 *  @param compSize         Size of following compressed data block.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
otf2_file_write_compression_header( OTF2_File* file,
                                    uint32_t   compSize,
                                    uint32_t   uncompSize )
{
    /* This is needed to find a header later on in the file.
     * See also definition of OTF2_COMPRESSION_HEADER. */
    uint64_t header_id = OTF2_COMPRESSION_HEADER;

    OTF2_ErrorCode status = otf2_file_write_buffered( file, &header_id, 8 );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status,
                            "Writing of compression header failed!" );
    }
    status = otf2_file_write_buffered( file, otf2_swap32( &compSize ), 4 );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status,
                            "Writing of compression header failed!" );
    }
    status = otf2_file_write_buffered( file, otf2_swap32( &uncompSize ), 4 );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status,
                            "Writing of compression header failed!" );
    }
    status = otf2_file_write_buffered( file, otf2_swap32( &( file->last_block_size ) ), 4 );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status,
                            "Writing of compression header failed!" );
    }

    return OTF2_SUCCESS;
}


/** @brief Read compression header.
 *
 *  Reads the size of the current and previous compression block.
 *
 *  @param file             OTF2 file handle.
 *  @param compSize         Size of following compressed data block.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
otf2_file_read_compression_header( OTF2_File* file,
                                   uint32_t*  compSize,
                                   uint32_t*  uncompSize )
{
    uint64_t header_id = 0;

    OTF2_ErrorCode status = file->read( file, &header_id, 8 );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status,
                            "Reading of compression header failed!" );
    }

    if ( header_id != OTF2_COMPRESSION_HEADER )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_DATA,
                            "This is no valid compression header!" );
    }

    status = file->read( file, compSize, 4 );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status,
                            "Reading of compression header failed!" );
    }
    status = file->read( file, uncompSize, 4 );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status,
                            "Reading of compression header failed!" );
    }
    status = file->read( file, &( file->last_block_size ), 4 );
    if ( status != OTF2_SUCCESS )
    {
        return UTILS_ERROR( status,
                            "Reading of compression header failed!" );
    }

    compSize              = otf2_swap32( compSize );
    uncompSize            = otf2_swap32( uncompSize );
    file->last_block_size = *( otf2_swap32( compSize ) );

    return OTF2_SUCCESS;
}


#ifdef WORDS_BIGENDIAN
/** Swap the bytes in a 4-byte buffer. */
static uint32_t*
otf2_swap32( uint32_t* value )
{
    uint8_t* bytes = ( uint8_t* )value;
    uint8_t  temp  = bytes[ 0 ];
    bytes[ 0 ] = bytes[ 3 ];
    bytes[ 3 ] = temp;
    temp       = bytes[ 1 ];
    bytes[ 1 ] = bytes[ 2 ];
    bytes[ 2 ] = temp;

    return ( uint32_t* )bytes;
}
#else
static uint32_t*
otf2_swap32( uint32_t* value )
{
    return value;
}
#endif /* WORDS_BIGENDIAN */


/** @brief Enables buffered writing with fixed size.
 *
 *  To enable writing to file system with a fixed size the data is buffered
 *  until enough data is collected to write it to the file system.
 *
 *  @param file             OTF2 file handle.
 *  @param buffer           Address of buffer containing the data.
 *  @param size             Size of the content in buffer.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
otf2_file_write_buffered( OTF2_File*  file,
                          const void* buffer,
                          uint64_t    size )
{
    /* If size is big enough just write data to file. */
    if ( size >= OTF2_FILE_BUFFER_SIZE )
    {
        return file->write( file, buffer, size );
    }

    if ( file->buffer == NULL )
    {
        file->buffer = ( void* )malloc( OTF2_FILE_BUFFER_SIZE );
        /* If malloc fails write directly to file. */
        if ( file->buffer == NULL )
        {
            return file->write( file, buffer, size );
        }
    }

    uint32_t buffer_free = OTF2_FILE_BUFFER_SIZE - file->buffer_used;

    if ( size < buffer_free )
    {
        memcpy( ( uint8_t* )file->buffer + file->buffer_used, buffer, size );
        file->buffer_used += size;
    }
    else
    {
        /* Copy first part of content so the buffer is filled and write complete
         * buffer to file system. */
        memcpy( ( uint8_t* )file->buffer + file->buffer_used, buffer, buffer_free );
        OTF2_ErrorCode status = file->write( file, file->buffer, OTF2_FILE_BUFFER_SIZE );
        if ( status != OTF2_SUCCESS )
        {
            free( file->buffer );
            return UTILS_ERROR( status,
                                "Write to file failed!" );
        }

        /* Copy second half of content to buffer. */
        memcpy( file->buffer, ( uint8_t* )buffer + buffer_free, size - buffer_free );
        file->buffer_used = size - buffer_free;
    }

    return OTF2_SUCCESS;
}


/** @brief Create a directory.
 *
 *  @param mainPath         Path to the head of the directory.
 *
 *  @return                 Returns OTF2_SUCCESS if successful, an error code
 *                          if an error occurs.
 */
OTF2_ErrorCode
OTF2_File_CreateDirectory( OTF2_Archive* archive,
                           const char*   mainPath,
                           bool          failIfLastExists )
{
    UTILS_ASSERT( archive );

    if ( archive->substrate == OTF2_SUBSTRATE_NONE )
    {
        return OTF2_SUCCESS;
    }

    OTF2_ErrorCode status = OTF2_SUCCESS;

    /* Validate arguments. */
    if ( NULL == mainPath )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "This is no valid file path!" );
    }

    char* path_copy = UTILS_CStr_dup( mainPath );
    if ( !path_copy )
    {
        return UTILS_ERROR( OTF2_ERROR_MEM_FAULT, "Can't duplicate path" );
    }
    UTILS_IO_SimplifyPath( path_copy );

    /* The +1 skips absolute paths, starting with '/' */
    char* slash = strchr( path_copy + 1, '/' );
    if ( !slash )
    {
        slash = path_copy + strlen( path_copy );
    }
    while ( true )
    {
        bool is_last = *slash == '\0';
        *slash = '\0';

        /* If there is a new file substrate the create directory function has to be
           added in the switch statement below. */
        status = otf2_file_create_directory( path_copy );
        if ( status != OTF2_SUCCESS )
        {
            if ( status != OTF2_ERROR_EEXIST )
            {
                goto out;
            }
            if ( is_last && !failIfLastExists )
            {
                status = OTF2_SUCCESS;
            }
        }

        if ( is_last )
        {
            break;
        }

        *slash = '/';
        slash  = strchr( slash + 1, '/' );
        if ( !slash )
        {
            slash = path_copy + strlen( path_copy );
        }
    }

out:
    free( path_copy );

    return status;
}


OTF2_ErrorCode
otf2_file_create_directory( const char* mainPath )
{
    int status = OTF2_MKDIR( mainPath, 0777 );

    /* ignore already existing directories */
    if ( status != 0 )
    {
        if ( errno == EEXIST )
        {
            return OTF2_ERROR_EEXIST;
        }
        return UTILS_ERROR_POSIX( "%s", mainPath );
    }

    return OTF2_SUCCESS;
}
