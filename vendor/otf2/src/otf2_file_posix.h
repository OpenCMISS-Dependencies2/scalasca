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

#ifndef OTF2_FILE_POSIX_H
#define OTF2_FILE_POSIX_H


OTF2_ErrorCode
otf2_file_posix_open( OTF2_Archive*    archive,
                      OTF2_FileMode    fileMode,
                      OTF2_FileType    fileType,
                      OTF2_LocationRef location,
                      OTF2_File**      file );


OTF2_ErrorCode
otf2_file_posix_close( OTF2_File* file );


#endif /* OTF2_FILE_POSIX_H */
