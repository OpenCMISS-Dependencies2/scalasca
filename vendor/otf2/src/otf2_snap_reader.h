/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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
 */

#ifndef OTF2_INTERNAL_SNAP_READER_H
#define OTF2_INTERNAL_SNAP_READER_H


/** @brief A local reader is defined by following struct. */
struct OTF2_SnapReader_struct
{
    /** Archive handle */
    OTF2_Archive* archive;

    /** Location ID of the related location */
    uint64_t     location_id;
    /** Buffer handle */
    OTF2_Buffer* buffer;

    /** The last read record from the buffer */
    OTF2_GenericSnap current_snap;

    /** Chain to next snapshot reader, used in OTF2_Archive */
    OTF2_SnapReader* next;

    /** Cached attribute list */
    OTF2_AttributeList attribute_list;

    /** Defines if this reader is stand-alone or operated by a global
     *  snapshot reader. */
    bool operated;

    /** Structure of callback references */
    OTF2_SnapReaderCallbacks reader_callbacks;
    /** Userdata pointer which is passed to the callbacks */
    void*                    user_data;
};


OTF2_SnapReader*
otf2_snap_reader_new( OTF2_Archive*    archive,
                      OTF2_LocationRef location );


OTF2_ErrorCode
otf2_snap_reader_delete( OTF2_SnapReader* readerHandle );


OTF2_ErrorCode
otf2_snap_reader_open_file( OTF2_SnapReader* reader );


void
otf2_snap_reader_operated_by_global_reader( OTF2_SnapReader* reader );


/** @brief Reads one record from the snapshot reader.
 *
 *  @param reader   Reference to the reader object
 *  @param record   Generic handle for returning the record. This ia at
 *                  least needed for the global reader.
 *  @param type     Handle for the timestamp
 */
OTF2_ErrorCode
otf2_snap_reader_read( OTF2_SnapReader* reader );


/** @brief Skips one snapshot record.
 *
 *  @param reader           Reference to the reader object
 */
OTF2_ErrorCode
otf2_snap_reader_skip( OTF2_SnapReader* reader );


#endif /* OTF2_INTERNAL_SNAP_READER_H */
