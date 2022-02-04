/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2018,
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

#ifndef OTF2_INTERNAL_GLOBAL_EVT_READER_H
#define OTF2_INTERNAL_GLOBAL_EVT_READER_H


/** @brief Keeps all necessary information for the global event reader. */
struct OTF2_GlobalEvtReader_struct
{
    /** Archive handle */
    OTF2_Archive*                 archive;
    /** @brief Number of event readers still in the queue */
    uint64_t                      number_of_evt_readers;
    /** @brief User callbacks. */
    OTF2_GlobalEvtReaderCallbacks reader_callbacks;
    /** @brief Additional user data. */
    void*                         user_data;
    /** @brief Heap keeping all event readers sorted by timestamp. */
    OTF2_EvtReader*               event_queue[];
};

OTF2_GlobalEvtReader*
otf2_global_evt_reader_new( OTF2_Archive* archive );

OTF2_ErrorCode
otf2_global_evt_reader_delete( OTF2_GlobalEvtReader* readerHandle,
                               bool                  locked );


#endif /* OTF2_INTERNAL_GLOBAL_EVT_READER_H */
