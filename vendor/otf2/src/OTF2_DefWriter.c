/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
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
 *
 *  @brief      This is the lowest user-visible layer which provides write
 *              operations for the event data of a single location (i.e., data
 *              from a single thread). Due file writing is engaged by the
 *              memory buffer module, the related callbacks for buffer-flushing
 *              must be set for the whole archive handle.
 *
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
#include "otf2_id_map.h"

#include "OTF2_File.h"
#include "OTF2_Buffer.h"

#include "otf2_local_definitions.h"

#include "otf2_def_writer.h"

static inline void
otf2_local_def_region_provide_pre_1_1( OTF2_RegionType* regionType,
                                       OTF2_StringRef   name,
                                       OTF2_StringRef   canonicalName,
                                       OTF2_StringRef   description,
                                       OTF2_RegionRole  regionRole,
                                       OTF2_Paradigm    paradigm,
                                       OTF2_RegionFlag  regionFlags,
                                       OTF2_StringRef   sourceFile,
                                       uint32_t         beginLineNumber,
                                       uint32_t         endLineNumber )
{
    *regionType = otf2_attic_def_region_provide_region_type( regionRole,
                                                             paradigm,
                                                             regionFlags );
}


static inline void
otf2_local_def_group_provide_pre_1_2( OTF2_GroupTypePre12* groupTypePre12,
                                      OTF2_StringRef       name,
                                      OTF2_GroupType       groupType,
                                      OTF2_Paradigm        paradigm,
                                      OTF2_GroupFlag       groupFlags,
                                      uint32_t             numberOfMembers,
                                      const uint64_t*      members )
{
    *groupTypePre12
        = otf2_attic_def_group_provide_group_type_pre_1_2( groupType,
                                                           paradigm,
                                                           groupFlags );
}


static inline void
otf2_local_def_system_tree_node_property_provide_pre_2_0( OTF2_StringRef*     stringValue,
                                                          OTF2_StringRef      name,
                                                          OTF2_Type           type,
                                                          OTF2_AttributeValue value )
{
    *stringValue = OTF2_UNDEFINED_STRING;
    if ( OTF2_TYPE_STRING == type )
    {
        *stringValue = value.stringRef;
    }
}

static inline void
otf2_local_def_location_group_property_provide_pre_2_0( OTF2_StringRef*     stringValue,
                                                        OTF2_StringRef      name,
                                                        OTF2_Type           type,
                                                        OTF2_AttributeValue value )
{
    *stringValue = OTF2_UNDEFINED_STRING;
    if ( OTF2_TYPE_STRING == type )
    {
        *stringValue = value.stringRef;
    }
}

static inline void
otf2_local_def_location_property_provide_pre_2_0( OTF2_StringRef*     stringValue,
                                                  OTF2_StringRef      name,
                                                  OTF2_Type           type,
                                                  OTF2_AttributeValue value )
{
    *stringValue = OTF2_UNDEFINED_STRING;
    if ( OTF2_TYPE_STRING == type )
    {
        *stringValue = value.stringRef;
    }
}

/* This _must_ be done for the generated part of the OTF2 local writer */
#include "OTF2_DefWriter_inc.c"
/* This _must_ be done for the generated part of the OTF2 local writer */


/** @brief
 *  Creates a new local definition writer instance.
 *
 *  @param archive      The archive.
 *  @param location     The Location ID is needed to name the trace file.
 *
 *  @threadsafety       Caller must hold the archive lock.
 *
 *  @return Returns a handle to the local definition writer if successful,
 *          NULL otherwise.
 */
OTF2_DefWriter*
otf2_def_writer_new( OTF2_Archive*    archive,
                     OTF2_LocationRef location )
{
    UTILS_ASSERT( archive );
    UTILS_ASSERT( location != OTF2_UNDEFINED_LOCATION );

    OTF2_DefWriter* writer = ( OTF2_DefWriter* )calloc( 1, sizeof( OTF2_DefWriter ) );
    if ( !writer )
    {
        UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                     "Could not allocate memory for writer handle!" );
        return NULL;
    }

    writer->archive     = archive;
    writer->location_id = location;

    uint64_t       chunk_size;
    OTF2_ErrorCode status = otf2_archive_get_def_chunksize( archive,
                                                            &chunk_size );
    if ( status != OTF2_SUCCESS )
    {
        free( writer );
        return NULL;
    }

    writer->buffer = OTF2_Buffer_New( archive,
                                      writer,
                                      chunk_size,
                                      OTF2_BUFFER_WRITE,
                                      OTF2_BUFFER_CHUNKED,
                                      OTF2_FILETYPE_LOCAL_DEFS,
                                      location );

    if ( !writer->buffer )
    {
        UTILS_ERROR( OTF2_ERROR_PROCESSED_WITH_FAULTS,
                     "Creation of buffer handle failed!" );
        free( writer );
        return NULL;
    }

    return writer;
}



/** @brief
 *  Releases the resources associated with the given local definition writer
 *  handle.
 *
 *  @param writerHandle  Writer object which has to be deleted.
 *
 *  @return OTF2_ErrorCode with !=OTF2_SUCCESS if there was an error.
 */

OTF2_ErrorCode
otf2_def_writer_delete( OTF2_DefWriter* writerHandle )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Writer deletion failed!" );
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_BUFFER_END_OF_FILE );

    OTF2_ErrorCode ret = OTF2_Buffer_Delete( writerHandle->buffer );
    if ( ret != OTF2_SUCCESS )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Buffer deletion failed!" );
    }

    free( writerHandle );

    return OTF2_SUCCESS;
}


OTF2_ErrorCode
OTF2_DefWriter_GetLocationID( const OTF2_DefWriter* writerHandle,
                              OTF2_LocationRef*     location )
{
    if ( !writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "Invalid argument for writer!" );
    }

    *location = writerHandle->location_id;

    return OTF2_SUCCESS;
}
