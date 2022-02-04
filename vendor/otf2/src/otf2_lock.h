/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
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
 *
 *  @brief      Wrappers to call the locking functions.
 */


#ifndef OTF2_INTERNAL_LOCK_H
#define OTF2_INTERNAL_LOCK_H


OTF2_ErrorCode
otf2_lock_create( OTF2_Archive* archive,
                  OTF2_Lock*    lock );


OTF2_ErrorCode
otf2_lock_destroy( OTF2_Archive* archive,
                   OTF2_Lock     lock );


OTF2_ErrorCode
otf2_lock_lock( OTF2_Archive* archive,
                OTF2_Lock     lock );


OTF2_ErrorCode
otf2_lock_unlock( OTF2_Archive* archive,
                  OTF2_Lock     lock );


#endif /* OTF2_INTERNAL_LOCK_H */
