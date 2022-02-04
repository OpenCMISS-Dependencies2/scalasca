/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/** @internal
 *
 *  @file
 *
 */

#ifndef OTF2_INTERNAL_FILE_SUBSTRATE_SION_H
#define OTF2_INTERNAL_FILE_SUBSTRATE_SION_H


#define OTF2_SION_SUCCESS 0


typedef enum otf2_sion_proxy_type
{
    OTF2_SION_PROXY_TYPE_GLOBAL,
    OTF2_SION_PROXY_TYPE_LOCAL,
    OTF2_SION_PROXY_TYPE_LOCAL_FILE_TYPE
} otf2_sion_proxy_type;


struct otf2_sion_comm_proxy
{
    OTF2_Archive*        archive;
    otf2_sion_proxy_type type;
};


/** @brief Implementation of per archive file substrate data.
 */
struct OTF2_FileSubstrateData
{
    /** SION generic API identifier */
    int api_id;

    /** SION comm group proxy for the user provided global communicator */
    struct otf2_sion_comm_proxy global_comm_proxy;

    /** SION comm group proxy for the user provided local communicator */
    struct otf2_sion_comm_proxy local_comm_proxy;

    OTF2_FileMode               file_mode;

    /** maps locations to sion ranks  */
    OTF2_IdMap* location_to_rank_map;
    /** The rank-map for reading */
    uint32_t    number_of_ranks;
    /** Holds per sion rank the file and the rank in this file. */
    uint32_t*   file_number_per_rank;
    uint32_t*   file_rank_per_rank;

    /** In which file did I write, and what rank I'm in this file. */
    uint32_t file_number;
    uint32_t number_of_files;
    uint32_t file_rank;
    uint32_t number_of_ranks_in_file;

    /* Stores data for the current operation, so that we know
       what to do in the local_comm_create callback */
    OTF2_FileMode fileModeForLocalCommCreate;
    OTF2_FileType fileTypeForLocalCommCreate;
};


/** @brief Implementation of file type substrate data.
 */
struct OTF2_FileSubstrateFileTypeData
{
    /** SION file identifier */
    int sion_id;

    /** The orignal file name */
    char* symbolic_file_path;

    /** The filename returned by an SION open call */
    char*         file_path;

    OTF2_FileMode file_mode;

    /** Was this file open with the mapped call? */
    bool mapped_open;

    /** Use the keyval API for this file. */
    bool keyval;

    /** Duplicate sion_id for each opened OTF2_File. */
    bool dup;

    /** The per-sub-file communicator for this rank in the sion file. */
    struct otf2_sion_comm_proxy proxy;
    OTF2_CollectiveContext*     local_comm;

    /** This locks access to sion_id, as multiple threads want to seek/read/write
     *  to it. In read mode this may be unneeded because of sion_dup.
     */
    OTF2_Lock lock;
};


#define OTF2_SION_FILE_LOCK( archive, file_type ) \
    do \
    { \
        OTF2_ErrorCode err = otf2_lock_lock( archive, ( file_type )->lock ); \
        if ( OTF2_SUCCESS != err ) \
        { \
            UTILS_ERROR( err, "Can't lock SION ID %d.", ( file_type )->sion_id ); \
        } \
    } while ( 0 )


#define OTF2_SION_FILE_UNLOCK( archive, file_type ) \
    do \
    { \
        OTF2_ErrorCode err = otf2_lock_unlock( archive, ( file_type )->lock ); \
        if ( OTF2_SUCCESS != err ) \
        { \
            UTILS_ERROR( err, "Can't unlock SION ID %d.", ( file_type )->sion_id ); \
        } \
    } while ( 0 )


void
otf2_file_substrate_sion_register_callbacks( int api_id );


OTF2_ErrorCode
otf2_sion_read_rank_map( OTF2_Archive* archive,
                         uint32_t*     numberOfFilesOut,
                         uint32_t*     numberOfRanksOut,
                         uint32_t**    fileNumberPerRankOut,
                         uint32_t**    fileRankPerRankOut,
                         OTF2_IdMap**  locationToRankMapOut );


OTF2_ErrorCode
otf2_sion_write_rank_map( OTF2_Archive* archive );


#endif /* OTF2_INTERNAL_FILE_SUBSTRATE_SION_H */
