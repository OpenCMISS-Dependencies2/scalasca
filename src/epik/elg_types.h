/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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


#ifndef EPIK_ELG_TYPES_H
#define EPIK_ELG_TYPES_H


#include <stdint.h>


typedef uint8_t  elg_ui1;   /* unsigned integer of size 1 Byte */
typedef uint32_t elg_ui4;   /* unsigned integer of size 4 Byte */
typedef uint64_t elg_ui8;   /* unsigned integer of size 8 Byte */
typedef double   elg_d8;    /* real of size 8 Byte (ieee 754) */


#if WORDS_BIGENDIAN
    #define ELG_BYTE_ORDER    ELG_BIG_ENDIAN
#else
    #define ELG_BYTE_ORDER    ELG_LITTLE_ENDIAN
#endif


#endif    // !EPIK_ELG_TYPES_H
