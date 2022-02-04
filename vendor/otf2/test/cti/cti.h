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

#ifndef CTI_H
#define CTI_H

#define CTI_SUCCESS 0

typedef struct cti_group* CTI_Group;

#define CTI_GROUP_WORLD ( cti_get_group_world() )
#define CTI_GROUP_NULL ( ( struct cti_group* )NULL )

int
CTI_Init( int*    ac,
          char*** av );

int
CTI_Finalize( void );

int
CTI_Group_size( CTI_Group group,
                int*      size );

int
CTI_Group_rank( CTI_Group group,
                int*      rank );

int
CTI_Group_dup( CTI_Group  group,
               CTI_Group* newgroup );

int
CTI_Group_free( CTI_Group* group );

int
CTI_Group_split( CTI_Group  group,
                 int        color,
                 int        key,
                 CTI_Group* newgroup );

int
CTI_Barrier( CTI_Group group );

int
CTI_Bcast( CTI_Group group,
           void*     buf,
           size_t    size,
           int       root );

int
CTI_Gather( CTI_Group   group,
            const void* inbuf,
            void*       outbuf,
            size_t      size,
            int         root );

int
CTI_Gatherv( CTI_Group   group,
             const void* inbuf,
             void*       outbuf,
             size_t      size,
             int         root );

int
CTI_Scatter( CTI_Group   group,
             const void* inbuf,
             void*       outbuf,
             size_t      size,
             int         root );

int
CTI_Scatterv( CTI_Group   group,
              const void* inbuf,
              void*       outbuf,
              size_t      size,
              int         root );

CTI_Group
cti_get_group_world( void );

#endif /* CTI_H */
