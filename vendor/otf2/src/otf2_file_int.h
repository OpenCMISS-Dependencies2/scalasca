/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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

/**
 *  @internal
 *
 *  @file
 */

#ifndef OTF2_INTERNAL_FILE_H
#define OTF2_INTERNAL_FILE_H

/** @brief Keeps all necessary information about the file handle. */
struct OTF2_File
{
    /** Archive handle */
    OTF2_Archive* archive;

    /** Used compression for this file. */
    OTF2_Compression compression;

    /** The type of this file */
    OTF2_FileType    file_type;
    /** The location id of this file, if appropriate */
    OTF2_LocationRef location_id;

    /** Keeps data for buffered writing. */
    void*    buffer;
    /** Keeps size of data already in buffer. */
    uint32_t buffer_used;

    /** Keeps size of last written block. Used in zlib compression layer. */
    uint32_t last_block_size;

    /** @name Function pointer to the actual file operations according to the
     *  file substrate.
     *  @{ */
    OTF2_ErrorCode ( * reset )( OTF2_File* file );
    OTF2_ErrorCode ( * write )( OTF2_File*  file,
                                const void* buffer,
                                uint64_t    size );
    OTF2_ErrorCode ( * read )( OTF2_File* file,
                               void*      buffer,
                               uint64_t   size );
    OTF2_ErrorCode ( * seek )( OTF2_File* file,
                               int64_t    offset );
    OTF2_ErrorCode ( * get_file_size )( OTF2_File* file,
                                        uint64_t*  size );
    /** @} */
};

#endif /* OTF2_INTERNAL_FILE_H */
