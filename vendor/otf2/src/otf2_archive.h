/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2018,
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
 * @brief This module is an internal representation of the opened
 *        archive. It is mainly used to organize the archive related
 *        meta-data.
 *
 */

#ifndef OTF2_INTERNAL_ARCHIVE_H
#define OTF2_INTERNAL_ARCHIVE_H

/** @brief Datastructure which is used internally to represent
 *         archive properties.
 */
typedef struct otf2_archive_property_struct otf2_archive_property;
struct otf2_archive_property_struct
{
    char*                  name;
    char*                  value;
    otf2_archive_property* next;
    otf2_archive_property* previous;
};

/** @brief Datastructure which is used internally to represent
 *         an archive.
 */
struct OTF2_Archive_struct
{
    /** File mode, which is globally unique (at least for one process). */
    OTF2_FileMode file_mode;
    /** Used to hold the last file mode transition. */
    OTF2_FileMode prev_file_mode;
    /** OTF2 version, which is set automatically. */
    uint8_t       version[ 3 ];
    /** Trace format, which is set automatically. */
    uint8_t       trace_format;

    /** Path to the folder, where the archive is stored. */
    char* archive_path;
    /** Name of the archive. Is partially used to generate filenames. */
    char* archive_name;

    /** Name of the machine, where the trace is recorded (optional) */
    char* machine_name;
    /** Description of the trace (optional) */
    char* description;
    /** Name of the creator (optional) */
    char* creator;

    /** Size of the chunks which are used in an event trace */
    uint64_t           chunk_size_events;
    /** Size of the chunks which are used in a definition trace */
    uint64_t           chunk_size_defs;
    /** Determines how the trace should be written */
    OTF2_FileSubstrate substrate;

    /** Datapointer for substrate specific data. */
    OTF2_FileSubstrateData* per_substrate_data[ OTF2_NUMBER_OF_SUBSTRATES ];

    /** Datapointer for substrate specific data. Each entry is owned by one substrate. */
    OTF2_FileSubstrateFileTypeData* per_filetype_data[ OTF2_NUMBER_OF_FILETYPES ];

    /** Determines if the trace data is compressed. */
    OTF2_Compression compression;

    /** Number of location in this trace */
    uint64_t number_of_locations;
    /** Number of global definition records */
    uint64_t number_of_global_defs;

    /** If the master mode ist turned on, this object holds a global definition writer object. */
    OTF2_GlobalDefWriter* global_def_writer;

    /** Global definition reader object */
    OTF2_GlobalDefReader* global_def_reader;

    /** Global event reader object */
    OTF2_GlobalEvtReader* global_evt_reader;

    /** Global snap reader object */
    OTF2_GlobalSnapReader* global_snap_reader;

    /** A list of already generated local event readers */
    OTF2_EvtReader* local_evt_readers;
    uint32_t        number_of_evt_readers;
    /** A list of already generated local definition readers */
    OTF2_DefReader* local_def_readers;

    /** A list of already generated local event writers */
    OTF2_EvtWriter* local_evt_writers;
    /** A list of already generated local definition writers */
    OTF2_DefWriter* local_def_writers;

    /** Marker reader object. */
    OTF2_MarkerReader* marker_reader;
    /** Marker writer object. */
    OTF2_MarkerWriter* marker_writer;

    /** Callbacks for flushes */
    const OTF2_FlushCallbacks* flush_callbacks;
    void*                      flush_data;

    /** Pointer to an allocate function for internal memory management. */
    const OTF2_MemoryCallbacks* allocator_callbacks;
    /** User data for internal memory management. */
    void*                       allocator_data;

    /** List of additional meta-information about an archive (called properties) */
    otf2_archive_property* properties;
    /** Number of additional meta-information about an archive (called properties) */
    uint32_t               number_of_properties;

    /** Unique identifier of trace file */
    uint64_t trace_id;

    /** A list of already generated local snapshot readers */
    OTF2_SnapReader* local_snap_readers;
    uint32_t         number_of_snap_readers;

    /** A list of already generated local snapshot writers */
    OTF2_SnapWriter* local_snap_writers;

    /** number of snapshots */
    uint32_t number_of_snapshots;

    /** A list of already generated thumb writers */
    OTF2_ThumbWriter* thumb_writers;
    /** A list of already generated thumb readers */
    OTF2_ThumbReader* thumb_readers;

    /** number of thumbnails */
    uint32_t number_of_thumbnails;

    /** Collective callbacks. */
    const OTF2_CollectiveCallbacks* collective_callbacks;
    /** User data for collective callbacks. */
    void*                           collective_data;
    /** Global communication context for collective operations. */
    OTF2_CollectiveContext*         global_comm_context;
    /** Local communication context for collective operations. */
    OTF2_CollectiveContext*         local_comm_context;

    /** Locking callbacks */
    const OTF2_LockingCallbacks* locking_callbacks;
    /** User data for locking callbacks */
    void*                        locking_data;
    /** The lock for the archive */
    OTF2_Lock                    lock;

    /** Number of locations using this archive object. */
    uint32_t locations_number;
    /** Vector keeping location information.
     *  In writing mode this keeps only the location IDs. In reading mode this
     *  is also used to store the mapping tables. */
    otf2_archive_location* locations;

    /** Archive/reader hints */
    bool         hint_global_reader_locked;
    OTF2_Boolean hint_global_reader;

    /** Map of CallingContext to Region. Needed for backward reading without
        new callbacks set. */
    OTF2_RegionRef* calling_context_to_region_map;
    uint32_t        calling_context_to_region_map_size;
    uint32_t        calling_context_to_region_map_capacity;
    OTF2_IdMap*     calling_context_to_region_index_map;
};


OTF2_Archive*
otf2_archive_open( const char*        archivePath,
                   const char*        archiveName,
                   OTF2_FileSubstrate Substrate,
                   OTF2_Compression   compression,
                   uint64_t           chunkSizeEvents,
                   uint64_t           chunkSizeDefs,
                   OTF2_FileMode      fileMode );

OTF2_ErrorCode
otf2_archive_close( OTF2_Archive* archive );

OTF2_ErrorCode
otf2_archive_set_version( OTF2_Archive* archive,
                          uint8_t       major,
                          uint8_t       minor,
                          uint8_t       bugfix );

OTF2_ErrorCode
otf2_archive_set_trace_format( OTF2_Archive* archive,
                               uint8_t       version );

OTF2_ErrorCode
otf2_archive_set_event_chunksize( OTF2_Archive* archive,
                                  uint64_t      chunkSize );

OTF2_ErrorCode
otf2_archive_set_def_chunksize( OTF2_Archive* archive,
                                uint64_t      chunkSize );

OTF2_ErrorCode
otf2_archive_set_machine_name( OTF2_Archive* archive,
                               const char*   machineName );

OTF2_ErrorCode
otf2_archive_set_description( OTF2_Archive* archive,
                              const char*   description );

OTF2_ErrorCode
otf2_archive_set_creator( OTF2_Archive* archive,
                          const char*   creator );

OTF2_ErrorCode
otf2_archive_set_number_of_locations( OTF2_Archive* archive,
                                      uint64_t      locationNumber );

OTF2_ErrorCode
otf2_archive_set_number_of_global_defs( OTF2_Archive* archive,
                                        uint64_t      numberOfGlobalDefs );

OTF2_ErrorCode
otf2_archive_set_file_mode( OTF2_Archive* archive,
                            OTF2_FileMode fileMode );

OTF2_ErrorCode
otf2_archive_set_file_substrate( OTF2_Archive*      archive,
                                 OTF2_FileSubstrate substrate );

OTF2_ErrorCode
otf2_archive_set_compression( OTF2_Archive*    archive,
                              OTF2_Compression compression );

OTF2_ErrorCode
otf2_archive_set_archive_path( OTF2_Archive* archive,
                               const char*   archivePath );

OTF2_ErrorCode
otf2_archive_set_archive_name( OTF2_Archive* archive,
                               const char*   archiveName );

void
otf2_archive_set_flush_callbacks( OTF2_Archive*              archive,
                                  const OTF2_FlushCallbacks* flushCallbacks,
                                  void*                      flushData );

void
otf2_archive_set_memory_callbacks( OTF2_Archive*               archive,
                                   const OTF2_MemoryCallbacks* memoryCallbacks,
                                   void*                       memoryData );

OTF2_ErrorCode
otf2_archive_set_collective_callbacks( OTF2_Archive*                   archive,
                                       const OTF2_CollectiveCallbacks* collectiveCallbacks,
                                       void*                           collectiveData,
                                       OTF2_CollectiveContext*         globalCommContext,
                                       OTF2_CollectiveContext*         localCommContext );

OTF2_ErrorCode
otf2_archive_set_locking_callbacks( OTF2_Archive*                archive,
                                    const OTF2_LockingCallbacks* lockingCallbacks,
                                    void*                        lockingData );

#define OTF2_ARCHIVE_LOCK( archive ) \
    do \
    { \
        OTF2_ErrorCode err = otf2_lock_lock( archive, ( archive )->lock ); \
        if ( OTF2_SUCCESS != err ) \
        { \
            UTILS_ERROR( err, "Can't lock archive." ); \
        } \
    } while ( 0 )

#define OTF2_ARCHIVE_UNLOCK( archive ) \
    do \
    { \
        OTF2_ErrorCode err = otf2_lock_unlock( archive, ( archive )->lock ); \
        if ( OTF2_SUCCESS != err ) \
        { \
            UTILS_ERROR( err, "Can't unlock archive." ); \
        } \
    } while ( 0 )

OTF2_ErrorCode
otf2_archive_set_property( OTF2_Archive* archive,
                           const char*   name,
                           const char*   value,
                           bool          overwrite );

OTF2_ErrorCode
otf2_archive_set_trace_id( OTF2_Archive* archive,
                           uint64_t      id );

OTF2_ErrorCode
otf2_archive_set_number_of_snapshots( OTF2_Archive* archive,
                                      uint32_t      number );

OTF2_ErrorCode
otf2_archive_set_number_of_thumbnails( OTF2_Archive* archive,
                                       uint32_t      numberOfThumbnails );

/*___GET_FUNCTIONS__________*/

OTF2_ErrorCode
otf2_archive_get_version( OTF2_Archive* archive,
                          uint8_t*      major,
                          uint8_t*      minor,
                          uint8_t*      bugfix );

static inline uint32_t
otf2_archive_get_trace_version( OTF2_Archive* archive )
{
    UTILS_ASSERT( archive );

    uint32_t version = archive->version[ 0 ];
    return ( version << 8 ) + archive->version[ 1 ];
}

OTF2_ErrorCode
otf2_archive_get_trace_format( OTF2_Archive* archive,
                               uint8_t*      version );

OTF2_ErrorCode
otf2_archive_get_event_chunksize( OTF2_Archive* archive,
                                  uint64_t*     chunkSize );

OTF2_ErrorCode
otf2_archive_get_def_chunksize( OTF2_Archive* archive,
                                uint64_t*     chunkSize );

OTF2_ErrorCode
otf2_archive_get_machine_name( OTF2_Archive* archive,
                               char**        maschineName );

OTF2_ErrorCode
otf2_archive_get_description( OTF2_Archive* archive,
                              char**        description );

OTF2_ErrorCode
otf2_archive_get_creator( OTF2_Archive* archive,
                          char**        creator );

OTF2_ErrorCode
otf2_archive_get_number_of_locations( OTF2_Archive* archive,
                                      uint64_t*     numberOfLocations );

OTF2_ErrorCode
otf2_archive_get_number_of_global_defs( OTF2_Archive* archive,
                                        uint64_t*     numberOfGlobalDefs );

OTF2_ErrorCode
otf2_archive_get_file_substrate( OTF2_Archive*       archive,
                                 OTF2_FileSubstrate* substrate );

OTF2_ErrorCode
otf2_archive_get_compression( OTF2_Archive*     archive,
                              OTF2_Compression* compression );

OTF2_ErrorCode
otf2_archive_get_file_mode( OTF2_Archive*  archive,
                            OTF2_FileMode* fileMode );

OTF2_ErrorCode
otf2_archive_get_trace_path( OTF2_Archive* archive,
                             char**        localFilePath );

OTF2_ErrorCode
otf2_archive_get_archive_path( OTF2_Archive* archive,
                               char**        archivePath );

OTF2_ErrorCode
otf2_archive_get_property_names( OTF2_Archive* archive,
                                 uint32_t*     numberOfProperties,
                                 char***       name );

OTF2_ErrorCode
otf2_archive_get_property( OTF2_Archive* archive,
                           const char*   name,
                           char**        value );

OTF2_ErrorCode
otf2_archive_get_trace_id( OTF2_Archive* archive,
                           uint64_t*     id );

OTF2_ErrorCode
otf2_archive_get_number_of_snapshots( OTF2_Archive* archive,
                                      uint32_t*     number );

OTF2_ErrorCode
otf2_archive_get_number_of_thumbnails( OTF2_Archive* archive,
                                       uint32_t*     numberOfThumbnails );

/**
 * Generates the full path to the file of specified type.
 *
 * @a locationId needs no be a valid ID, if otf2_file_type_needs_location_id()
 * holds for the file type @a fileType.
 *
 * @return malloc'ed path
 */
char*
otf2_archive_get_file_path( OTF2_Archive*    archive,
                            OTF2_FileType    fileType,
                            OTF2_LocationRef location );

/**
 * Same as @a otf2_archive_get_file_path(), but with the given @a name_prefix
 * for files where otf2_file_type_needs_location_id() holds.
 *
 * @return malloc'ed path
 */
char*
otf2_archive_get_file_path_with_name_prefix( OTF2_Archive* archive,
                                             OTF2_FileType fileType,
                                             const char*   namePrefix );

OTF2_ErrorCode
otf2_archive_get_evt_reader( OTF2_Archive*    archive,
                             OTF2_LocationRef location,
                             OTF2_EvtReader** reader );

OTF2_ErrorCode
otf2_archive_get_def_reader( OTF2_Archive*    archive,
                             OTF2_LocationRef location,
                             OTF2_DefReader** reader );

OTF2_ErrorCode
otf2_archive_get_global_evt_reader( OTF2_Archive*          archive,
                                    OTF2_GlobalEvtReader** reader );

OTF2_ErrorCode
otf2_archive_get_global_def_reader( OTF2_Archive*          archive,
                                    OTF2_GlobalDefReader** reader );

OTF2_ErrorCode
otf2_archive_get_snap_reader( OTF2_Archive*     archive,
                              OTF2_LocationRef  location,
                              OTF2_SnapReader** reader );


OTF2_ErrorCode
otf2_archive_get_global_snap_reader( OTF2_Archive*           archive,
                                     OTF2_GlobalSnapReader** reader );

OTF2_ErrorCode
otf2_archive_get_thumb_reader( OTF2_Archive*      archive,
                               uint32_t           thumbID,
                               OTF2_ThumbReader** reader );

OTF2_ErrorCode
otf2_archive_get_marker_reader( OTF2_Archive*       archive,
                                OTF2_MarkerReader** reader );

OTF2_ErrorCode
otf2_archive_get_evt_writer( OTF2_Archive*    archive,
                             OTF2_LocationRef location,
                             OTF2_EvtWriter** writer );

OTF2_ErrorCode
otf2_archive_get_def_writer( OTF2_Archive*    archive,
                             OTF2_LocationRef location,
                             OTF2_DefWriter** writer );

OTF2_ErrorCode
otf2_archive_get_global_def_writer( OTF2_Archive*          archive,
                                    OTF2_GlobalDefWriter** writer );

OTF2_ErrorCode
otf2_archive_get_snap_writer( OTF2_Archive*     archive,
                              OTF2_LocationRef  location,
                              OTF2_SnapWriter** writer );

OTF2_ErrorCode
otf2_archive_get_thumb_writer( OTF2_Archive*      archive,
                               OTF2_ThumbWriter** writer,
                               const char*        name,
                               const char*        description,
                               OTF2_ThumbnailType type,
                               uint32_t           numberOfSamples,
                               uint32_t           numberOfMetrics,
                               const uint64_t*    refsToDefs );

OTF2_ErrorCode
otf2_archive_get_marker_writer( OTF2_Archive*       archive,
                                OTF2_MarkerWriter** writer );

OTF2_ErrorCode
otf2_archive_close_evt_writer( OTF2_Archive*   archive,
                               OTF2_EvtWriter* writer );

OTF2_ErrorCode
otf2_archive_close_def_writer( OTF2_Archive*   archive,
                               OTF2_DefWriter* writer );

OTF2_ErrorCode
otf2_archive_close_global_def_writer( OTF2_Archive*         archive,
                                      OTF2_GlobalDefWriter* writer );

OTF2_ErrorCode
otf2_archive_close_snap_writer( OTF2_Archive*    archive,
                                OTF2_SnapWriter* writer );

OTF2_ErrorCode
otf2_archive_close_marker_writer( OTF2_Archive*      archive,
                                  OTF2_MarkerWriter* markerWriter );

OTF2_ErrorCode
otf2_archive_close_evt_reader( OTF2_Archive*   archive,
                               OTF2_EvtReader* reader,
                               bool            locked );

OTF2_ErrorCode
otf2_archive_close_thumb_reader( OTF2_Archive*     archive,
                                 OTF2_ThumbReader* reader );

OTF2_ErrorCode
otf2_archive_close_global_evt_reader( OTF2_Archive*         archive,
                                      OTF2_GlobalEvtReader* globalEvtReader );

OTF2_ErrorCode
otf2_archive_close_def_reader( OTF2_Archive*   archive,
                               OTF2_DefReader* reader );

OTF2_ErrorCode
otf2_archive_close_global_def_reader( OTF2_Archive*         archive,
                                      OTF2_GlobalDefReader* globalDefReader );

OTF2_ErrorCode
otf2_archive_close_snap_reader( OTF2_Archive*    archive,
                                OTF2_SnapReader* reader,
                                bool             locked );

OTF2_ErrorCode
otf2_archive_close_global_snap_reader( OTF2_Archive*          archive,
                                       OTF2_GlobalSnapReader* reader );

OTF2_ErrorCode
otf2_archive_close_marker_reader( OTF2_Archive*      archive,
                                  OTF2_MarkerReader* markerReader );

OTF2_ErrorCode
otf2_archive_open_evt_files( OTF2_Archive* archive );

OTF2_ErrorCode
otf2_archive_close_evt_files( OTF2_Archive* archive );

OTF2_ErrorCode
otf2_archive_open_def_files( OTF2_Archive* archive );

OTF2_ErrorCode
otf2_archive_close_def_files( OTF2_Archive* archive );

OTF2_ErrorCode
otf2_archive_open_snap_files( OTF2_Archive* archive );

OTF2_ErrorCode
otf2_archive_close_snap_files( OTF2_Archive* archive );

OTF2_ErrorCode
otf2_archive_create_directory( OTF2_Archive* archive );

bool
otf2_archive_is_master( OTF2_Archive* archive );

/** @brief Register function pointers for OTF2_Reader.
 *
 *  This function is called from OTF2_Reader and delivers the function pointers
 *  needed by OTF2_Reader.
 *  Please note: The function pointers must be provided as void pointer so the
 *  archive does not need to know any declarations provided by OTF2_Reader.
 *
 *  @return                         Returns OTF2_SUCCESS if successful, an error
 *                                  code if an error occurs.
 */
const otf2_reader_archive_impl*
otf2_archive_get_reader_impl( void );


OTF2_ErrorCode
otf2_archive_select_location( OTF2_Archive*    archive,
                              OTF2_LocationRef location );


OTF2_ErrorCode
otf2_archive_add_location( OTF2_Archive*    archive,
                           OTF2_LocationRef location );


OTF2_ErrorCode
otf2_archive_find_location( OTF2_Archive*    archive,
                            OTF2_LocationRef location,
                            uint32_t*        archiveLocationIndex );


OTF2_ErrorCode
otf2_archive_get_location( OTF2_Archive*           archive,
                           uint32_t                archiveLocationIndex,
                           otf2_archive_location** archiveLocation );


OTF2_ErrorCode
otf2_archive_set_hint( OTF2_Archive* archive,
                       OTF2_Hint     hint,
                       void*         value );

OTF2_ErrorCode
otf2_archive_query_hint( OTF2_Archive* archive,
                         OTF2_Hint     hint,
                         void*         value );

OTF2_ErrorCode
otf2_archive_add_calling_context( OTF2_Archive*              archive,
                                  OTF2_CallingContextRef     callingContext,
                                  OTF2_RegionRef             region,
                                  OTF2_SourceCodeLocationRef sourceCodeLocation,
                                  OTF2_CallingContextRef     parent );

OTF2_ErrorCode
otf2_archive_map_calling_context_to_region( OTF2_Archive*          archive,
                                            OTF2_CallingContextRef callingContext,
                                            OTF2_RegionRef*        region );

#endif /* !OTF2_INTERNAL_ARCHIVE_H */
