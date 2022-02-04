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


#ifndef EPIK_ELG_IMPL_H
#define EPIK_ELG_IMPL_H


/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Basic definitions for elg_gen and elg_rw
 *
 *-----------------------------------------------------------------------------
 */

#define ELG_MIN_BUFSIZE    100000
#define ELG_DEF_BUFSIZE    10000000

#define ELG_HEADER    "EPILOG"

#define ELG_MAJOR_VNR    1
#define ELG_MINOR_VNR    9


typedef elg_ui1* buffer_t;


#endif    // !EPIK_ELG_IMPL_H
