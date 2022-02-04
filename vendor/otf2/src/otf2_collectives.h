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


/**
 *  @internal
 *
 *  @file
 *
 *  @brief      Wrappers to call the collectives.
 */


#ifndef OTF2_INTERNAL_COLLECTIVES_H
#define OTF2_INTERNAL_COLLECTIVES_H


OTF2_CallbackCode
otf2_collectives_get_size( OTF2_Archive*           archive,
                           OTF2_CollectiveContext* commContext,
                           uint32_t*               size );


OTF2_CallbackCode
otf2_collectives_get_rank( OTF2_Archive*           archive,
                           OTF2_CollectiveContext* commContext,
                           uint32_t*               rank );


OTF2_CallbackCode
otf2_collectives_create_local_comm( OTF2_Archive*            archive,
                                    OTF2_CollectiveContext** localCommContext,
                                    OTF2_CollectiveContext*  globalCommContext,
                                    uint32_t                 globalRank,
                                    uint32_t                 globalSize,
                                    uint32_t                 localRank,
                                    uint32_t                 localSize,
                                    uint32_t                 fileNumber,
                                    uint32_t                 numberOfFiles );


OTF2_CallbackCode
otf2_collectives_free_local_comm( OTF2_Archive*           archive,
                                  OTF2_CollectiveContext* localCommContext );


OTF2_CallbackCode
otf2_collectives_barrier( OTF2_Archive*           archive,
                          OTF2_CollectiveContext* commContext );


OTF2_CallbackCode
otf2_collectives_bcast( OTF2_Archive*           archive,
                        OTF2_CollectiveContext* commContext,
                        void*                   data,
                        uint32_t                numberElements,
                        OTF2_Type               type,
                        uint32_t                root );


OTF2_CallbackCode
otf2_collectives_gather( OTF2_Archive*           archive,
                         OTF2_CollectiveContext* commContext,
                         const void*             inData,
                         void*                   outData,
                         uint32_t                numberElements,
                         OTF2_Type               type,
                         uint32_t                root );


OTF2_CallbackCode
otf2_collectives_gatherv( OTF2_Archive*           archive,
                          OTF2_CollectiveContext* commContext,
                          const void*             inData,
                          uint32_t                inElements,
                          void*                   outData,
                          const uint32_t*         outElements,
                          OTF2_Type               type,
                          uint32_t                root );


OTF2_CallbackCode
otf2_collectives_scatter( OTF2_Archive*           archive,
                          OTF2_CollectiveContext* commContext,
                          const void*             inData,
                          void*                   outData,
                          uint32_t                numberElements,
                          OTF2_Type               type,
                          uint32_t                root );


OTF2_CallbackCode
otf2_collectives_scatterv( OTF2_Archive*           archive,
                           OTF2_CollectiveContext* commContext,
                           const void*             inData,
                           const uint32_t*         inElements,
                           void*                   outData,
                           uint32_t                outElements,
                           OTF2_Type               type,
                           uint32_t                root );


/** Convenience function to broadcast an OTF2_ErrorCodes value. */
static inline OTF2_CallbackCode
otf2_collectives_bcast_error( OTF2_Archive*           archive,
                              OTF2_CollectiveContext* commContext,
                              OTF2_ErrorCode*         errorCode,
                              uint32_t                root )
{
    int64_t           error_code = *errorCode;
    OTF2_CallbackCode ret        = otf2_collectives_bcast(
        archive,
        commContext,
        &error_code,
        1,
        OTF2_TYPE_INT64,
        root );
    if ( OTF2_CALLBACK_SUCCESS == ret )
    {
        *errorCode = error_code;
    }
    return ret;
}


#endif /* OTF2_INTERNAL_COLLECTIVES_H */
