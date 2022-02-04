/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef EPIK_EPK_ARCHIVE_H
#define EPIK_EPK_ARCHIVE_H


#ifdef __cplusplus
    #define EXTERN    extern "C"
#else
    #define EXTERN    extern
#endif


typedef enum
{
    EPK_TYPE_DIR  = 0,    /* directory */
    EPK_TYPE_ESD  = 1,    /* definitions */
    EPK_TYPE_ETM  = 2,    /* sum profile */
    EPK_TYPE_ELG  = 3,    /* event trace */
    EPK_TYPE_EMP  = 4,    /* id mappings */
    EPK_TYPE_OTF  = 5,    /* OTF trace */
    EPK_TYPE_CUBE = 6,    /* CUBE summary */
    EPK_TYPE_SION = 7,    /* SION traces */
    EPK_TYPE_MAX  = 8
} epk_type_t;


/*
 *-----------------------------------------------------------------------------
 * management of the experiment measurement archive directory (and format)
 *-----------------------------------------------------------------------------
 */

/* get the pretty name of the type of archive file */
EXTERN char*
epk_archive_filetype(const char* filename);

/* determine whether given filename is an epik archive name */
EXTERN char*
epk_archive_is_epik(const char* fprefix);

/* get the experiment measurement archive base directory name */
EXTERN char*
epk_archive_get_name(void);

/* set the experiment measurement archive base directory name */
EXTERN char*
epk_archive_set_name(const char* name);

/* get an archive (sub)directory */
EXTERN char*
epk_archive_directory(const epk_type_t type);

/* get an archive filename of given type from given name */
EXTERN char*
epk_archive_filename(const epk_type_t type,
                     const char*      name);

/* get an archive rank filename of given type from given name and rank */
EXTERN char*
epk_archive_rankname(const epk_type_t type,
                     const char*      name,
                     const unsigned   rank);

/* check whether the experiment archive exists */
EXTERN int
epk_archive_exists(const char* dirname);

/* check whether the experiment archive is locked */
EXTERN int
epk_archive_locked(void);


#endif    // !EPIK_EPK_ARCHIVE_H
