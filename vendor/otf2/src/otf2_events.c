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


/**
 *  @internal
 *
 *  @file
 *
 *  @brief      Holds conversion functions for deprecated event record.
 */


#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"

#include "otf2_attic_types.h"
#include "otf2_attic.h"

#include "otf2_attribute_value.h"
#include "otf2_attribute_list.h"

#include "otf2_events_int.h"


bool
otf2_event_thread_fork_convert_to_omp_fork( OTF2_Archive*          archive,
                                            const OTF2_ThreadFork* threadForkRecord,
                                            OTF2_AttributeList*    attributeList,
                                            OTF2_OmpFork*          ompForkRecord )
{
    if ( threadForkRecord->model != OTF2_PARADIGM_OPENMP )
    {
        return false;
    }

    ompForkRecord->number_of_requested_threads = threadForkRecord->number_of_requested_threads;

    return true;
}


bool
otf2_event_thread_join_convert_to_omp_join( OTF2_Archive*          archive,
                                            const OTF2_ThreadJoin* threadJoinRecord,
                                            OTF2_AttributeList*    attributeList,
                                            OTF2_OmpJoin*          ompJoinRecord )
{
    if ( threadJoinRecord->model != OTF2_PARADIGM_OPENMP )
    {
        return false;
    }

    return true;
}


bool
otf2_event_thread_acquire_lock_convert_to_omp_acquire_lock(
    OTF2_Archive*                 archive,
    const OTF2_ThreadAcquireLock* threadAcquireLockRecord,
    OTF2_AttributeList*           attributeList,
    OTF2_OmpAcquireLock*          ompAcquireLockRecord )
{
    if ( threadAcquireLockRecord->model != OTF2_PARADIGM_OPENMP )
    {
        return false;
    }

    ompAcquireLockRecord->lock_id           = threadAcquireLockRecord->lock_id;
    ompAcquireLockRecord->acquisition_order = threadAcquireLockRecord->acquisition_order;

    return true;
}


bool
otf2_event_thread_release_lock_convert_to_omp_release_lock(
    OTF2_Archive*                 archive,
    const OTF2_ThreadReleaseLock* threadReleaseLockRecord,
    OTF2_AttributeList*           attributeList,
    OTF2_OmpReleaseLock*          ompReleaseLockRecord )
{
    if ( threadReleaseLockRecord->model != OTF2_PARADIGM_OPENMP )
    {
        return false;
    }

    ompReleaseLockRecord->lock_id           = threadReleaseLockRecord->lock_id;
    ompReleaseLockRecord->acquisition_order = threadReleaseLockRecord->acquisition_order;

    return true;
}


bool
otf2_event_calling_context_enter_convert_to_enter( OTF2_Archive*                   archive,
                                                   const OTF2_CallingContextEnter* callingContextEnterRecord,
                                                   OTF2_AttributeList*             attributeList,
                                                   OTF2_Enter*                     enterRecord )
{
    if ( callingContextEnterRecord->calling_context == OTF2_UNDEFINED_CALLING_CONTEXT )
    {
        return false;
    }

    OTF2_ErrorCode ret = otf2_archive_map_calling_context_to_region(
        archive,
        callingContextEnterRecord->calling_context,
        &enterRecord->region );
    if ( OTF2_SUCCESS != ret )
    {
        return false;
    }

    return true;
}


bool
otf2_event_calling_context_leave_convert_to_leave( OTF2_Archive*                   archive,
                                                   const OTF2_CallingContextLeave* callingContextLeaveRecord,
                                                   OTF2_AttributeList*             attributeList,
                                                   OTF2_Leave*                     leaveRecord )
{
    if ( callingContextLeaveRecord->calling_context == OTF2_UNDEFINED_CALLING_CONTEXT )
    {
        return false;
    }

    OTF2_ErrorCode ret = otf2_archive_map_calling_context_to_region(
        archive,
        callingContextLeaveRecord->calling_context,
        &leaveRecord->region );
    if ( OTF2_SUCCESS != ret )
    {
        return false;
    }

    return true;
}
