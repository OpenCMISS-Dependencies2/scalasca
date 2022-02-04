/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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


#ifndef EPIK_ELG_RW_H
#define EPIK_ELG_RW_H


#ifdef __cplusplus
    #define EXTERN    extern "C"
#else
    #define EXTERN    extern
#endif


#include <stdio.h>
#include <stdlib.h>

#include "elg_defs.h"


#define ELG_NO_TIME    -1.0

/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Low level event trace reading / (re)writing
 *
 *-----------------------------------------------------------------------------
 */

/*
 *-----------------------------------------------------------------------------
 * Typedefs (to make data types opaque)
 *-----------------------------------------------------------------------------
 */


/* Trace file for reading */
typedef struct ElgIn_struct ElgIn;

/* Trace record */
typedef struct ElgRec_struct ElgRec;


/*
 *-----------------------------------------------------------------------------
 * ElgRec
 *-----------------------------------------------------------------------------
 */

/* Free record */

EXTERN void
ElgRec_free(ElgRec* rec);

/* Reading record header */

EXTERN elg_ui1
ElgRec_get_type(ElgRec* rec);

EXTERN elg_ui1
ElgRec_get_length(ElgRec* rec);

/* Predicate on record type */

EXTERN int
ElgRec_is_event(ElgRec* rec);

EXTERN int
ElgRec_is_attribute(ElgRec* rec);

/* Reading record data -*/

EXTERN elg_ui1
ElgRec_read_ui1(ElgRec* rec);

EXTERN elg_ui4
ElgRec_read_ui4(ElgRec* rec);

EXTERN elg_ui8
ElgRec_read_ui8(ElgRec* rec);

EXTERN elg_d8
ElgRec_read_d8(ElgRec* rec);

EXTERN char*
ElgRec_read_string(ElgRec* rec);

/*
 *-----------------------------------------------------------------------------
 * ElgIn
 *-----------------------------------------------------------------------------
 */

/* Open and close trace file */

EXTERN ElgIn*
ElgIn_open(const char* path_name);

EXTERN int
ElgIn_close(ElgIn* in);

/* EPILOG version */

EXTERN elg_ui4
ElgIn_get_version(ElgIn* in);

/* File position and error management */

EXTERN int
ElgIn_seek(ElgIn* in,
           long   offset);

/* Reading trace records */

EXTERN ElgRec*
ElgIn_read_record(ElgIn* in);


#endif    // !EPIK_ELG_RW_H
