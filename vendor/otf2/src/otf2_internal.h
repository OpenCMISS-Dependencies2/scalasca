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
 */

#ifndef OTF2_INTERNAL_H
#define OTF2_INTERNAL_H


/** @brief The location. */
typedef struct otf2_archive_location otf2_archive_location;


/** @brief A clock synchronization interval. */
typedef struct otf2_clock_interval otf2_clock_interval;


/** @brief Forward decl for per archive file substrate data.
 */
typedef struct OTF2_FileSubstrateData OTF2_FileSubstrateData;


/** @brief Forward decl for file type substrate data.
 */
typedef struct OTF2_FileSubstrateFileTypeData OTF2_FileSubstrateFileTypeData;


/** @brief Keeps all necessary information about the buffer.
 *  Please see OTF2_Buffer for a detailed description.
 */
typedef struct OTF2_Buffer OTF2_Buffer;


/** @brief Forward decl for the file type.
 */
typedef struct OTF2_File OTF2_File;


/** @brief The currently supported trace format version
 */
#define OTF2_TRACE_FORMAT 2


/** @brief Size of a timestamp record.
 */
#define OTF2_BUFFER_TIMESTAMP_SIZE ( size_t )9


#endif /* OTF2_INTERNAL_H */
