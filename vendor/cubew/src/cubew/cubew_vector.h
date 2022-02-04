/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



/**
 * \file cubew_vector.h
 * \brief Contains macros XALLOC, ALLOC, REALLOC and ADD_NEXT for memory allocation.
 */


#ifndef CUBEW_VECTOR_H
#define CUBEW_VECTOR_H

/** Macros for handling dynamic arrays */

//#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "cubew_memory.h"
#include "UTILS_Error.h"
#ifdef __cplusplus
extern "C" {
#endif

extern int cubew_trace;

/** allocation with optional log message and only warning if unsuccessful */
#define ALLOC( MEMORY, NMEMB, MTYPE, MESSAGE ) MEMORY = ( MTYPE* )CUBEW_CALLOC( NMEMB, sizeof( MTYPE ), MESSAGE ); \
    if ( cubew_trace ) { UTILS_WARNING( "%s: calloc(%lu,%lu) = %p\n", __func__, \
                                        ( unsigned long )NMEMB, ( unsigned long )sizeof( MTYPE ), MEMORY ); } \
    if ( !MEMORY ) { UTILS_WARNING( "%s: calloc(%lu,%lu): %s\n", \
                                    __func__, ( unsigned long )NMEMB, ( unsigned long )sizeof( MTYPE ), \
                                    strerror( errno ) ); }

/** allocation with exit if unsuccessful */
#define XALLOC( MEMORY, NMEMB, MTYPE, MESSAGE ) MEMORY = ( MTYPE* )CUBEW_CALLOC( NMEMB, sizeof( MTYPE ), MESSAGE ); \
    if ( cubew_trace ) { UTILS_WARNING(  "%s: calloc(%lu,%lu) = %p\n", __func__, \
                                         ( unsigned long )NMEMB, ( unsigned long )sizeof( MTYPE ), MEMORY ); } \
    if ( !MEMORY ) { UTILS_WARNING(  "%s: calloc(%lu,%lu): %s\n", \
                                     __func__, ( unsigned long )NMEMB, ( unsigned long )sizeof( MTYPE ), \
                                     strerror( errno ) ); exit( 1 ); }

/** (re)allocation with exit if unsuccessful */
#define REALLOC( MEMORY, CAST, OLDSIZE, MSIZE, MESSAGE ) \
    if ( cubew_trace ) { UTILS_WARNING( "%s: realloc(%p,%lu)", __func__, \
                                        MEMORY, ( unsigned long )MSIZE ); } \
    MEMORY = CAST CUBEW_REALLOC( MEMORY, OLDSIZE, MSIZE, MESSAGE ); \
    if ( cubew_trace ) { UTILS_WARNING( " = %p\n", MEMORY ); } \
    if ( !MEMORY ) { UTILS_WARNING( "%s: realloc(%lu): %s\n", \
                                    __func__, ( unsigned long )MSIZE, strerror( errno ) ); exit( 2 ); }

/** append element to vector */
#define ADD_NEXT( VECTOR, ELEMENT, ETYPE, MESSAGE ) \
    if ( VECTOR->size == VECTOR->capacity ) { \
        size_t old_capacity = VECTOR->capacity; \
        if ( VECTOR->capacity == 0 ) { VECTOR->capacity = 1; } \
        else{ VECTOR->capacity *= 2; } \
        REALLOC( VECTOR->data, ( ETYPE* ), sizeof( ETYPE ) * ( old_capacity ), sizeof( ETYPE ) * ( VECTOR->capacity ), MESSAGE ); \
    } \
    VECTOR->data[ VECTOR->size ] = ELEMENT; \
    VECTOR->size++;

#ifdef __cplusplus
}
#endif

#endif
