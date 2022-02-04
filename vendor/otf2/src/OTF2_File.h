/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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

/**
 *  @internal
 *
 *  @file
 */

#ifndef OTF2_FILE_H
#define OTF2_FILE_H


/** @brief Defines size of buffered writes to file system. */
#define OTF2_FILE_BUFFER_SIZE ( 4 * 1024 * 1024 )


/** @brief Defines which part of the buffer is to be read next.
*  Please see OTF2_FilePart_enum for a detailed description. */
typedef uint8_t OTF2_FilePart;

/** @brief Defines which part of the buffer is to be read next. */
enum OTF2_FilePart_enum
{
    /** Read next part of the file. */
    OTF2_FILEPART_NEXT = 0,
    /** Read previous part of the file. */
    OTF2_FILEPART_PREV = 1
};


void
otf2_file_initialize( OTF2_Archive*    archive,
                      OTF2_File*       file,
                      OTF2_FileType    fileType,
                      OTF2_LocationRef location );


OTF2_ErrorCode
otf2_file_finalize( OTF2_File* file );


OTF2_ErrorCode
OTF2_File_Reset( OTF2_File* file );


OTF2_ErrorCode
OTF2_File_CreateDirectory( OTF2_Archive* archive,
                           const char*   mainPath,
                           bool          failIfLastExists );


OTF2_ErrorCode
OTF2_File_Write( OTF2_File*  file,
                 const void* buffer,
                 uint64_t    size );

OTF2_ErrorCode
OTF2_File_Read( OTF2_File* file,
                void*      buffer,
                uint64_t   size );

OTF2_ErrorCode
OTF2_File_GetSizeUnchunked( OTF2_File* file,
                            uint64_t*  size );

OTF2_ErrorCode
OTF2_File_SeekPrevChunk( OTF2_File* file,
                         uint64_t   chunkNumber,
                         uint64_t   chunkSize );

OTF2_ErrorCode
OTF2_File_SeekChunk( OTF2_File* file,
                     uint64_t   chunkNumber,
                     uint64_t   chunkSize );

#endif /* !OTF2_FILE_H */
