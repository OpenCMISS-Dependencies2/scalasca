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

/** @internal
 *
 *  @file
 */

#ifndef OTF2_FILE_SUBSTRATE_POSIX_H
#define OTF2_FILE_SUBSTRATE_POSIX_H

/**
 * @brief Initializes the substrate.
 *
 * Called once for an archive. Substrate can use @a *substrateData, for
 * substrate specific data.
 */
OTF2_ErrorCode
otf2_file_substrate_posix_initialize( OTF2_Archive* archive );


/**
 * @brief Finalizes the substrate.
 *
 * Called once for an archive. Substrate should release @a substrateData, if
 * it was set in @a otf2_file_substrate_initialize.
 */
OTF2_ErrorCode
otf2_file_substrate_posix_finalize( OTF2_Archive* archive );


/**
 * @brief Called when the collective context was set by the user.
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_open( OTF2_Archive* archive,
                                OTF2_FileMode fileMode );


/**
 * @brief Called before closing the archive.
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_close( OTF2_Archive* archive );


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_open_file_type( OTF2_Archive* archive,
                                          OTF2_FileMode fileMode,
                                          OTF2_FileType fileType );


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_close_file_type( OTF2_Archive* archive,
                                           OTF2_FileType fileType );


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_open_file( OTF2_Archive*    archive,
                                     OTF2_FileMode    fileMode,
                                     OTF2_FileType    fileType,
                                     OTF2_LocationRef locationId,
                                     OTF2_File**      file );


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_posix_close_file( OTF2_File* file );


#endif /* OTF2_FILE_SUBSTRATE_POSIX_H */
