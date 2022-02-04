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
 * @internal
 *
 * @file
 *
 * @brief
 * This layer always writes globally defined OTF2 definition records and
 * is used to write either the global definitions in addition to local
 * definitions or write all definitions as globally valid in combination
 * with OTF2_GlobalEventWriter. Global definitions are stored in one global
 * definition file, which makes it nearly impossible to write them in a
 * distributed manner. It is therefore only allowed to get such a writer
 * from an OTF2_ArchiveHandler which is the master in the collective context.
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
#include "otf2_lock.h"
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

#include "otf2_global_definitions.h"

#include "otf2_global_def_writer.h"

/** @brief Keeps all necessary information a global definition writer. */
struct OTF2_GlobalDefWriter_struct
{
    /** Archive handle */
    OTF2_Archive* archive;
    /** Reference to a buffer where the global definition are stored. */
    OTF2_Buffer*  buffer;
};


static inline void
otf2_global_def_region_provide_pre_1_1( OTF2_RegionType* regionType,
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
otf2_global_def_group_provide_pre_1_2( OTF2_GroupTypePre12* groupTypePre12,
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
otf2_global_def_system_tree_node_property_provide_pre_2_0( OTF2_StringRef*     stringValue,
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
otf2_global_def_location_group_property_provide_pre_2_0( OTF2_StringRef*     stringValue,
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
otf2_global_def_location_property_provide_pre_2_0( OTF2_StringRef*     stringValue,
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

/* This _must_ be done for the generated part of the OTF2 global writer. */
#include "OTF2_GlobalDefWriter_inc.c"
/* This _must_ be done for the generated part of the OTF2 global writer. */


/** @brief
 *  Creates a new global definition writer instance.
 *
 *  @param archive      The archive.
 *
 *  @threadsafety       Caller must hold the archive lock.
 *
 *  @return Returns a handle to the global definition writer if successful,
 *          NULL otherwise.
 */
OTF2_GlobalDefWriter*
otf2_global_def_writer_new( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    OTF2_GlobalDefWriter* writer = ( OTF2_GlobalDefWriter* )calloc( 1, sizeof( OTF2_GlobalDefWriter ) );
    if ( !writer )
    {
        UTILS_ERROR( OTF2_ERROR_MEM_FAULT,
                     "Could not allocate memory for writer handle!" );
        return NULL;
    }

    writer->archive = archive;

    uint64_t       chunk_size;
    OTF2_ErrorCode ret = otf2_archive_get_def_chunksize( archive,
                                                         &chunk_size );
    if ( ret != OTF2_SUCCESS )
    {
        free( writer );
        return NULL;
    }

    writer->buffer = OTF2_Buffer_New( archive,
                                      writer,
                                      chunk_size,
                                      OTF2_BUFFER_WRITE,
                                      OTF2_BUFFER_CHUNKED,
                                      OTF2_FILETYPE_GLOBAL_DEFS,
                                      OTF2_UNDEFINED_LOCATION );

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
 *  Releases the resources associated with the given global definition writer
 *  handle. Does nothing if a NULL pointer is provided.
 *
 *  @param writerHandle  Writer object which has to be deleted.
 *
 *  @return OTF2_ErrorCode with !=OTF2_SUCCESS if there was an error.
 */
OTF2_ErrorCode
otf2_global_def_writer_delete( OTF2_GlobalDefWriter* writerHandle )
{
    if ( !writerHandle )
    {
        return OTF2_SUCCESS;
    }

    OTF2_Buffer_WriteUint8( writerHandle->buffer, OTF2_BUFFER_END_OF_FILE );

    OTF2_ErrorCode ret = OTF2_Buffer_Delete( writerHandle->buffer );
    if ( ret != OTF2_SUCCESS )
    {
        return UTILS_ERROR( ret, "Buffer deletion failed!" );
    }

    free( writerHandle );

    return OTF2_SUCCESS;
}


/** @brief
 *  Releases the resources associated with the given global definition writer
 *  handle. Does nothing if a NULL pointer is provided.
 *
 *  @param writerHandle  Writer object which has to be deleted.
 *
 *  @return OTF2_ErrorCode with !=OTF2_SUCCESS if there was an error.
 */
OTF2_ErrorCode
OTF2_GlobalDefWriter_GetNumberOfDefinitions( OTF2_GlobalDefWriter* writerHandle,
                                             uint64_t*             numberOfDefinitions )
{
    if ( NULL == writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid global definition writer handle!" );
    }
    UTILS_ASSERT( writerHandle->archive );

    *numberOfDefinitions = writerHandle->archive->number_of_global_defs;

    return OTF2_SUCCESS;
}


/** @brief
 *  Returns the number of global location definitions written so far.
 *
 *  @param writerHandle      Writer object which has to be deleted.
 *  @param numberOfLocations Pointer where to store the number of locations.
 *
 *  @return OTF2_ErrorCode with !=OTF2_SUCCESS if there was an error.
 */
OTF2_ErrorCode
OTF2_GlobalDefWriter_GetNumberOfLocations( OTF2_GlobalDefWriter* writerHandle,
                                           uint64_t*             numberOfLocations )
{
    if ( NULL == writerHandle )
    {
        return UTILS_ERROR( OTF2_ERROR_INVALID_ARGUMENT,
                            "Invalid global definition writer handle!" );
    }
    UTILS_ASSERT( writerHandle->archive );

    *numberOfLocations = writerHandle->archive->number_of_locations;

    return OTF2_SUCCESS;
}
