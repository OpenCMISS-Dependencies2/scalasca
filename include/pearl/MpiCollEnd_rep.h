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


#ifndef PEARL_MPICOLLEND_REP_H
#define PEARL_MPICOLLEND_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiCollEnd_rep.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiCollEnd_rep.
 *
 *  This header file provides the declaration of the class MpiCollEnd_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiCollEnd_rep
 *  @ingroup PEARL_mpi
 *  @brief   %Event representation for end of MPI collective operations.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiCollEnd_rep
    : public Event_rep
{
    public:
        enum coll_type
        {
            BARRIER,
            BCAST,
            GATHER,
            GATHERV,
            SCATTER,
            SCATTERV,
            ALLGATHER,
            ALLGATHERV,
            ALLTOALL,
            ALLTOALLV,
            ALLTOALLW,
            ALLREDUCE,
            REDUCE,
            REDUCE_SCATTER,
            SCAN,
            EXSCAN,
            REDUCE_SCATTER_BLOCK,
            NUMBER_OF_COLL_TYPES
        };


        /// @name Constructors & destructor
        /// @{

        MpiCollEnd_rep(timestamp_t timestamp,
                       coll_type   type,
                       MpiComm*    communicator,
                       uint32_t    root,
                       uint64_t    bytesSent,
                       uint64_t    bytesReceived);

        MpiCollEnd_rep(const GlobalDefs& defs,
                       Buffer&           buffer);

        /// @}
        /// @name Event type information
        /// @{

        virtual event_t
        getType() const;

        virtual bool
        isOfType(event_t type) const;

        /// @}
        /// @name Access event data
        /// @{

        virtual MpiComm*
        getComm() const;

        virtual uint32_t
        getRoot() const;

        virtual uint64_t
        getBytesSent() const;

        virtual uint64_t
        getBytesReceived() const;

        coll_type
        getCollType() const;

        /// @}


    protected:
        /// @name Serialize event data
        /// @{

        virtual void
        pack(Buffer& buffer) const;

        /// @}
        /// @name Generate human-readable output of event data
        /// @{

        virtual std::ostream&
        output(std::ostream& stream) const;

        /// @}


    private:
        /// Type of the MPI collective operation
        coll_type mType;

        /// MPI communicator used in collective operation
        MpiComm* mCommunicator;

        /// Local rank of root location w.r.t. communicator
        uint32_t mRoot;

        /// Number of bytes sent
        uint64_t mBytesSent;

        /// Number of bytes received
        uint64_t mBytesReceived;
};
}    // namespace pearl


#endif    // !PEARL_MPICOLLEND_REP_H
