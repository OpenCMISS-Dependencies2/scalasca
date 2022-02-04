/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 *
 * @file
 *
 */


#ifndef SION_CTI_H
#define SION_CTI_H


struct sion_comm_group
{
    CTI_Group cti_group;
};


static inline size_t
get_sion_type_size( int type )
{
    switch ( type )
    {
        case _SION_CHAR:
            return 1;
        case _SION_INT32:
            return 4;
        case _SION_INT64:
            return 8;
        default:
            return 0;
    }
}


static int
create_local_comm_cb( void** localCommGroupOut,
                      void*  globalCommGroup,
                      int    globalRank,
                      int    globalSize,
                      int    localRank,
                      int    localSize,
                      int    fileNumber,
                      int    numberOfFiles )
{
    struct sion_comm_group* new_comm_group = malloc( sizeof( *new_comm_group ) );
    struct sion_comm_group* global_group   = globalCommGroup;
    CTI_Group_split( global_group->cti_group,
                     fileNumber,
                     localRank,
                     &new_comm_group->cti_group );
    *localCommGroupOut = new_comm_group;

    return 0;
}


static int
free_local_comm_cb( void* localCommGroup )
{
    struct sion_comm_group* comm_group = localCommGroup;
    CTI_Group_free( &comm_group->cti_group );
    free( comm_group );

    return 0;
}


static int
barrier_cb( void* commGroup )
{
    struct sion_comm_group* comm_group = commGroup;
    CTI_Barrier( comm_group->cti_group );
    return 0;
}


static int
bcast_cb( void* data,
          void* commGroup,
          int   type,
          int   nelem,
          int   root )
{
    struct sion_comm_group* comm_group = commGroup;
    size_t                  size       = nelem * get_sion_type_size( type );

    CTI_Bcast( comm_group->cti_group,
               data,
               size,
               root );

    return 0;
}


static int
gather_cb( void* inData,
           void* outData,
           void* commGroup,
           int   type,
           int   nelem,
           int   root )
{
    struct sion_comm_group* comm_group = commGroup;
    size_t                  size       = nelem * get_sion_type_size( type );

    CTI_Gather( comm_group->cti_group,
                inData,
                outData,
                size,
                root );

    return 0;
}


static int
gatherv_cb( void* inData,
            void* outData,
            void* commGroup,
            int   type,
            int*  counts,
            int   nelem,
            int   root )
{
    struct sion_comm_group* comm_group = commGroup;
    size_t                  size       = nelem * get_sion_type_size( type );

    CTI_Gatherv( comm_group->cti_group,
                 inData,
                 outData,
                 size,
                 root );

    return 0;
}


static int
scatter_cb( void* inData,
            void* outData,
            void* commGroup,
            int   type,
            int   nelem,
            int   root )
{
    struct sion_comm_group* comm_group = commGroup;
    size_t                  size       = nelem * get_sion_type_size( type );

    CTI_Scatter( comm_group->cti_group,
                 inData,
                 outData,
                 size,
                 root );

    return 0;
}


static int
scatterv_cb( void* inData,
             void* outData,
             void* commGroup,
             int   type,
             int*  counts,
             int   nelem,
             int   root )
{
    struct sion_comm_group* comm_group = commGroup;
    size_t                  size       = nelem * get_sion_type_size( type );

    CTI_Scatterv( comm_group->cti_group,
                  inData,
                  outData,
                  size,
                  root );

    return 0;
}


static int
sion_cti_get_api( void )
{
    int api_id = sion_generic_create_api( "SION CTI" );
    sion_generic_register_create_local_commgroup_cb( api_id, create_local_comm_cb );
    sion_generic_register_free_local_commgroup_cb( api_id, free_local_comm_cb );
    sion_generic_register_barrier_cb( api_id, barrier_cb );
    sion_generic_register_bcastr_cb( api_id, bcast_cb );
    sion_generic_register_gatherr_cb( api_id, gather_cb );
    sion_generic_register_gathervr_cb( api_id, gatherv_cb );
    sion_generic_register_scatterr_cb( api_id, scatter_cb );
    sion_generic_register_scattervr_cb( api_id, scatterv_cb );

    return api_id;
}

#endif /* SION_CTI_H */
