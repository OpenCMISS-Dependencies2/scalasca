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


/** @brief Total number of file substrates. */
#define OTF2_NUMBER_OF_SUBSTRATES 4


/** @brief Total number of file types. */
#define OTF2_NUMBER_OF_FILETYPES 8


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
        case OTF2_FILETYPE_LOCAL_DEFS:
        case OTF2_FILETYPE_EVENTS:
        case OTF2_FILETYPE_SNAPSHOTS:
            return true;

        case OTF2_FILETYPE_ANCHOR:
        case OTF2_FILETYPE_GLOBAL_DEFS:
        case OTF2_FILETYPE_THUMBNAIL:
        case OTF2_FILETYPE_MARKER:
        case OTF2_FILETYPE_SIONRANKMAP:
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
        case OTF2_FILETYPE_EVENTS:
        case OTF2_FILETYPE_SNAPSHOTS:
            return true;

        case OTF2_FILETYPE_ANCHOR:
        case OTF2_FILETYPE_GLOBAL_DEFS:
        case OTF2_FILETYPE_LOCAL_DEFS:
        case OTF2_FILETYPE_THUMBNAIL:
        case OTF2_FILETYPE_MARKER:
        case OTF2_FILETYPE_SIONRANKMAP:
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
        case OTF2_FILETYPE_ANCHOR:
        case OTF2_FILETYPE_THUMBNAIL:
        case OTF2_FILETYPE_MARKER:
            return true;

        case OTF2_FILETYPE_GLOBAL_DEFS:
        case OTF2_FILETYPE_LOCAL_DEFS:
        case OTF2_FILETYPE_EVENTS:
        case OTF2_FILETYPE_SNAPSHOTS:
        case OTF2_FILETYPE_SIONRANKMAP:
            return false;

        default:
            UTILS_BUG( "Unhandled OTF2 file type: %d", fileType );
    }

    /* Never reached */
    return true;
}


#endif /* OTF2_INTERNAL_FILE_TYPES_H */
