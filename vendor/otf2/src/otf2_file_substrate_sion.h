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
 *
 *  @brief      This layer is completely internal to the OTF2 library and should
 *              not be exposed to the end user. Its purpose is to abstract the
 *              file interaction via the specific file substrate.
 */

#ifndef OTF2_FILE_SUBSTRATE_SION_H
#define OTF2_FILE_SUBSTRATE_SION_H

/**
 * @brief Initializes the substrate.
 *
 * Called once for an archive. Substrate can use @a *substrateData, for
 * substrate specific data.
 */
OTF2_ErrorCode
otf2_file_substrate_sion_initialize( OTF2_Archive* archive );


/**
 * @brief Finalizes the substrate.
 *
 * Called once for an archive. Substrate should release @a substrateData, if
 * it was set in @a otf2_file_substrate_initialize.
 */
OTF2_ErrorCode
otf2_file_substrate_sion_finalize( OTF2_Archive* archive );


/**
 * @brief Called when the collective context was set by the user.
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_sion_open( OTF2_Archive* archive,
                               OTF2_FileMode fileMode );


/**
 * @brief Called before closing the archive.
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_sion_close( OTF2_Archive* archive );


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_sion_open_file_type( OTF2_Archive* archive,
                                         OTF2_FileMode fileMode,
                                         OTF2_FileType fileType );


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_sion_close_file_type( OTF2_Archive* archive,
                                          OTF2_FileType fileType );


/**
 * @brief
 *
 *
 */
OTF2_ErrorCode
otf2_file_substrate_sion_open_file( OTF2_Archive*    archive,
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
otf2_file_substrate_sion_close_file( OTF2_File* file );


#endif /* OTF2_FILE_SUBSTRATE_SION_H */
