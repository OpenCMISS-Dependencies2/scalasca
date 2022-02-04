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

#ifndef OTF2_CTI_H
#define OTF2_CTI_H


OTF2_ErrorCode
OTF2_CTI_Archive_SetCollectiveCallbacks( OTF2_Archive* archive,
                                         CTI_Group     globalGroup,
                                         CTI_Group     localGroup );


OTF2_ErrorCode
OTF2_CTI_Archive_SetCollectiveCallbacksSplit( OTF2_Archive* archive,
                                              CTI_Group     globalGroup,
                                              uint32_t      numberOfFiles );


OTF2_ErrorCode
OTF2_CTI_Reader_SetCollectiveCallbacks( OTF2_Reader* reader,
                                        CTI_Group    globalGroup,
                                        bool         allowLocalGroup );


#endif /* OTF2_CTI_H */
