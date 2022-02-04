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
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_PEARL_PADDING_H
#define PEARL_PEARL_PADDING_H


/*-------------------------------------------------------------------------*/
/**
 *  @file    pearl_padding.h
 *  @ingroup PEARL_base
 *  @brief   Definition of a compiler-/platform-specific preprocessor macro
 *           to avoid padding in structures.
 *
 *  This header file provides the definition of a compiler and platform-
 *  specific preprocessor macro which can be used to turn off padding in
 *  data structures.
 **/
/*-------------------------------------------------------------------------*/


#if defined(__xlC__) && (defined(__bgq__) || defined(__bgp__))
    #define PEARL_NOPAD_ATTR    __attribute__((packed))
#else
    #define PEARL_NOPAD_ATTR
#endif


#endif    // !PEARL_PEARL_PADDING_H
