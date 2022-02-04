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
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_MPIWINDOW_H
#define PEARL_MPIWINDOW_H


#include <pearl/RmaWindow.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiWindow.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiWindow.
 *
 *  This header file provides the declaration of the class MpiWindow.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class MpiComm;


/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiWindow
 *  @ingroup PEARL_mpi
 *  @brief   Stores information related to an MPI-2 remote memory access
 *           window.
 *
 *  The instances of the class MpiWindow provide information about MPI-2
 *  remote memory access (RMA) windows.
 **/
/*-------------------------------------------------------------------------*/

class MpiWindow
    : public RmaWindow
{
    public:
        /// @name Constructors & destructor
        /// @{

        MpiWindow(IdType   id,
                  MpiComm* communicator);

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_MPIWINDOW_H
