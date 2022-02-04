/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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
 * \file cubew_report_layouts_types.h
 * \brief Includes proper combinatins of file layout and name transformations according to the choosen (via macro) report layout.
 *
 *
 *
 */

#ifndef __CUBEW_REPORT_LAYOUTS_TYPES_H
#define __CUBEW_REPORT_LAYOUTS_TYPES_H 0

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* #include "cubew_cubew.h" */


#ifdef CUBE_LAYOUT_TAR
#ifdef CUBE_LAYOUT_DIRECT
#error Only one layout has to be choosen. Either "tar" via macro CUBE_LAYOUT_TAR or "direct" via macro CUBE_LAYOUT_DIRECT
#endif
#ifndef CUBE_LAYOUT_DIRECT

/* #warning "TAR LAYOUT!" */
struct tar_writer_t;
/*  typedef  struct __tar_writer_t tar_writer_t ; */
typedef  struct tar_writer_t report_layout_writer;


#endif
#endif


#ifdef CUBE_LAYOUT_DIRECT

#ifdef CUBE_LAYOUT_TAR
#error Only one layout has to be choosen. Either "tar" via macro CUBE_LAYOUT_TAR or "direct" via macro CUBE_LAYOUT_DIRECT
#endif

#ifndef CUBE_LAYOUT_TAR

/* #warning "DIRECT HYBRID LAYOUT!" */
struct direct_writer_t;
/*  typedef struct __direct_writer_t direct_writer_t; */
typedef struct direct_writer_t report_layout_writer;

#endif

#endif



#ifndef CUBE_LAYOUT_DIRECT
#ifndef CUBE_LAYOUT_TAR
/* default includes are
   / #warning "DEFUALT LAYOUT! " */

struct tar_writer_t;
/*  typedef struct tar_writer_t __tar_writer_t ; */
typedef struct tar_writer_t report_layout_writer;


#endif
#endif


#endif
