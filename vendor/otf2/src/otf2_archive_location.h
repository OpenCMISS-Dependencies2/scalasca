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
 * @internal
 *
 * @file
 *
 * @brief The archive location.
 */


#ifndef OTF2_INTERNAL_ARCHIVE_LOCATION_H
#define OTF2_INTERNAL_ARCHIVE_LOCATION_H


/** @brief A clock synchronization interval. */
struct otf2_clock_interval
{
    otf2_clock_interval* next;
    uint64_t             interval_begin;
    uint64_t             interval_end;
    double               slope;
    int64_t              offset;
};


/** @brief The location. */
struct otf2_archive_location
{
    OTF2_LocationRef location_id;

    /** Array of mapping tables from the local definitions for the
     *  event reader.
     */
    const OTF2_IdMap* mapping_tables[ OTF2_MAPPING_MAX ];

    /** vector of clock intervals from the local definitions for the
     *  event reader.
     */
    otf2_clock_interval*  clock_intervals;
    otf2_clock_interval** tail_clock_intervals;
    otf2_clock_interval*  pending_clock_interval;
};


void
otf2_archive_location_initialize( otf2_archive_location* archiveLocation,
                                  OTF2_LocationRef       location );


void
otf2_archive_location_finalize( otf2_archive_location* archiveLocation );


/** @brief This interface can be used from the local definition reader to set the
 *  mapping tables, which is important for mapped reading.
 *
 *  @param archive  Reader object which reads the events from its buffer.
 *  @param location The location id.
 *  @param iDMap    ID map, which has to be inserted into the reader.
 *  @param mapType  Says which ID has to be mapped.
 *
 *  @return OTF2_ErrorCode with !=OTF2_SUCCESS if there was an error.
 */
OTF2_ErrorCode
otf2_archive_location_set_mapping_table( OTF2_Archive*     archive,
                                         OTF2_LocationRef  location,
                                         OTF2_MappingType  mapType,
                                         const OTF2_IdMap* iDMap );


/** @brief Adds a clock offset to the reader.
 *
 *  @param archive  Reader object which reads the events from its buffer.
 *  @param location The location id.
 *  @param time     Time of the synchronization
 *  @param offset   Offset to the global clock.
 *  @param stdDev   Standard deviation, unused.
 *
 *  @return OTF2_ErrorCode with !=OTF2_SUCCESS if there was an error.
 */
OTF2_ErrorCode
otf2_archive_location_add_clock_offset( OTF2_Archive*    archive,
                                        OTF2_LocationRef location,
                                        uint64_t         time,
                                        int64_t          offset,
                                        double           stdDev );


#endif /* OTF2_INTERNAL_ARCHIVE_LOCATION_H */
