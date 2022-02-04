/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @internal
 *
 * @file
 *
 * @brief Query file type properties.
 */


#ifndef OTF2_INTERNAL_FILE_TYPES_H
#define OTF2_INTERNAL_FILE_TYPES_H


@otf2 with max_substrate = 0
@otf2  for substrate in FileSubstrate.entries:
@otf2   if substrate.value >= max_substrate:
@otf2    set max_substrate = substrate.value + 1
@otf2   endif
@otf2   if loop.last:
/** @brief Total number of file substrates. */
#define OTF2_NUMBER_OF_SUBSTRATES @@max_substrate@@
@otf2   endif
@otf2  endfor
@otf2 endwith


@otf2 with max_file_type = 0
@otf2  for file_type in FileType.entries:
@otf2   if file_type.value >= max_file_type:
@otf2    set max_file_type = file_type.value + 1
@otf2   endif
@otf2   if loop.last:
/** @brief Total number of file types. */
#define OTF2_NUMBER_OF_FILETYPES @@max_file_type@@
@otf2   endif
@otf2  endfor
@otf2 endwith


/**
 * Does the given file type has an location id associated?
 *
 * @param fileType The to check.
 *
 * @return Needs this type a valid location ID.
 */
static inline bool
otf2_file_type_needs_location_id( OTF2_FileType fileType )
{
    switch ( fileType )
    {
        @otf2 for file_type in FileType.entries if file_type.needs_location_id:
        case @@file_type.name@@:
        @otf2 endfor
            return true;

        @otf2 for file_type in FileType.entries if not file_type.needs_location_id:
        case @@file_type.name@@:
        @otf2 endfor
            return false;

        default:
            UTILS_BUG( "Unhandled OTF2 file type: %d", fileType );
    }

    /* Never reached */
    return false;
}


/**
 * Does the given file type has timestamps in the records?
 *
 * True for event and snapshot files, else false.
 *
 * @param fileType The to check.
 *
 * @return Has this type timestamps in the records.
 */
static inline bool
otf2_file_type_has_timestamps( OTF2_FileType fileType )
{
    switch ( fileType )
    {
        @otf2 for file_type in FileType.entries if file_type.has_timestamps:
        case @@file_type.name@@:
        @otf2 endfor
            return true;

        @otf2 for file_type in FileType.entries if not file_type.has_timestamps:
        case @@file_type.name@@:
        @otf2 endfor
            return false;

        default:
            UTILS_BUG( "Unhandled OTF2 file type: %d", fileType );
    }

    /* Never reached */
    return false;
}


/**
 * Does the given file type ignores the file compression?
 *
 * True for anchor file and thumbnail.
 *
 * @param fileType The to check.
 *
 * @return Ignores this type the file compression layer.
 */
static inline bool
otf2_file_type_ignores_compression( OTF2_FileType fileType )
{
    switch ( fileType )
    {
        @otf2 for file_type in FileType.entries if file_type.ignores_compression:
        case @@file_type.name@@:
        @otf2 endfor
            return true;

        @otf2 for file_type in FileType.entries if not file_type.ignores_compression:
        case @@file_type.name@@:
        @otf2 endfor
            return false;

        default:
            UTILS_BUG( "Unhandled OTF2 file type: %d", fileType );
    }

    /* Never reached */
    return true;
}


#endif /* OTF2_INTERNAL_FILE_TYPES_H */
