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


#ifndef PEARL_MPIMESSAGE_H
#define PEARL_MPIMESSAGE_H


#include <mpi.h>

#include <pearl/Buffer.h>
#include <pearl/Uncopyable.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiMessage.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiMessage.
 *
 *  This header file provides the declaration of the class MpiMessage.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class MpiComm;


/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiMessage
 *  @ingroup PEARL_mpi
 *  @brief   Abstraction layer for MPI messages.
 **/
/*-------------------------------------------------------------------------*/

class MpiMessage
    : public pearl::Buffer,
      private pearl::Uncopyable
{
    public:
        /// @name Constructors & destructor
        /// @{

        MpiMessage(MPI_Comm comm,
                   int      size = 128);

        MpiMessage(const MpiComm& comm,
                   int            size = 128);

        /// @}
        /// @name Sending & receiving
        /// @{

        void
        send(int dest,
             int tag);

        void
        isend(int dest,
              int tag);

        void
        issend(int dest,
               int tag);

        bool
        test();

        void
        cancel();

        void
        wait();

        void
        recv(int source,
             int tag);

        bool
        probe(int source,
              int tag);

        void
        bcast(int root);

        /// @}
        /// @name Retrieving message information
        /// @{

        MPI_Comm
        get_comm() const;

        MPI_Request
        get_request() const;

        /// @}


    private:
        /// Associated MPI communicator
        MPI_Comm m_comm;

        /// Request object used for non-blocking communication
        MPI_Request m_request;
};
}    // namespace pearl


#endif    // !PEARL_MPIMESSAGE_H
