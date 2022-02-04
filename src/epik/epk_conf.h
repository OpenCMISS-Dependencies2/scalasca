/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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


#ifndef EPIK_EPK_CONF_H
#define EPIK_EPK_CONF_H


/*
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing, Investigating, and Logging)
 *
 *  - Accessing the EPIK configuration and environment variables
 *
 *-----------------------------------------------------------------------------
 */

#ifdef __cplusplus
    #define EXTERN    extern "C"
#else
    #define EXTERN    extern
#endif


EXTERN int
epk_str2int(const char* val);    /* Convert string to int */

EXTERN int
epk_str2bool(const char* str);    /* Convert string to bool */


#endif    // !EPIK_EPK_CONF_H
