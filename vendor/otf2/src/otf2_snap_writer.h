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

#ifndef OTF2_INTERNAL_SNAP_WRITER_H
#define OTF2_INTERNAL_SNAP_WRITER_H


/** @brief Keeps all necessary information about the snap writer. */
struct OTF2_SnapWriter_struct
{
    /** Archive handle */
    OTF2_Archive*    archive;
    /** Pointer to the memory buffer handle. */
    OTF2_Buffer*     buffer;
    /** ID of the location. */
    uint64_t         location_id;
    /** Chain to next snap writer, used in OTF2_Archive */
    OTF2_SnapWriter* next;
};


OTF2_SnapWriter*
otf2_snap_writer_new( OTF2_Archive* archive,
                      uint64_t      locationID );


OTF2_ErrorCode
otf2_snap_writer_delete( OTF2_SnapWriter* writer );


#endif /* OTF2_INTERNAL_SNAP_WRITER_H */
