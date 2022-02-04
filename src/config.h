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
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Provides the Scalasca package configuration.
 *
 *  This header file provides the Scalasca package configuration.  It hides
 *  the complexity of including the correct configuration header file needed
 *  for the current build (frontend, backend, backend-mpi) by selecting the
 *  appropriate header generated by the respective <tt>configure</tt> script.
 *  These header files provide a number of <tt>#define</tt> directives, each
 *  one indicating the presence of a particular feature that has been tested
 *  by configure.  Therefore, <tt>config.h</tt> should be included <b>as the
 *  very first header in every source file</b>.
 **/
/*-------------------------------------------------------------------------*/


#ifndef SCALASCA_CONFIG_H
#define SCALASCA_CONFIG_H


#if defined CROSS_BUILD
    #if defined FRONTEND_BUILD
        #include <config-frontend.h>
    #elif defined BACKEND_BUILD_NOMPI
        #include <config-backend.h>
    #elif defined BACKEND_BUILD_MPI
        #include <config-backend-mpi.h>
    #else
        #error "You can not use <config.h> without defining either FRONTEND_BUILD, BACKEND_BUILD_NOMPI or BACKEND_BUILD_MPI."
    #endif
#elif defined NOCROSS_BUILD
    #if defined BACKEND_BUILD_NOMPI
        #include <config-backend.h>
    #elif defined BACKEND_BUILD_MPI
        #include <config-backend-mpi.h>
    #else
        #error "You can not use <config.h> without defining either BACKEND_BUILD_NOMPI or BACKEND_BUILD_MPI."
    #endif
#else
    #error "You can not use <config.h> without defining either CROSS_BUILD or NOCROSS_BUILD."
#endif
#include <config-common.h>
#include <config-custom.h>


#endif    // !SCALASCA_CONFIG_H
