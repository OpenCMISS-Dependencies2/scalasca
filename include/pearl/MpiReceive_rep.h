/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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


#ifndef PEARL_MPIRECEIVE_REP_H
#define PEARL_MPIRECEIVE_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiReceive_rep.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiReceive_rep.
 *
 *  This header file provides the declaration of the class MpiReceive_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiReceive_rep
 *  @ingroup PEARL_mpi
 *  @brief   %Event representation for MPI_RECV events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiReceive_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        MpiReceive_rep(timestamp_t timestamp,
                       MpiComm*    communicator,
                       uint32_t    source,
                       uint32_t    tag,
                       uint64_t    bytesReceived);

        MpiReceive_rep(const GlobalDefs& defs,
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
        getTag() const;

        virtual uint64_t
        getBytesReceived() const;

        virtual uint32_t
        getSource() const;

        virtual uint64_t
        getRequestId() const;

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
        /// @name Find previous/next communication request entries
        /// @{

        virtual uint32_t
        get_next_reqoffs() const;

        virtual uint32_t
        get_prev_reqoffs() const;

        virtual void
        set_prev_reqoffs(uint32_t offset);

        virtual void
        set_next_reqoffs(uint32_t offset);

        /// @}


    private:
        /// MPI communicator used
        MpiComm* mCommunicator;

        /// Local rank of sender location w.r.t. communicator
        uint32_t mSource;

        /// Message tag
        uint32_t mTag;

        /// Message length
        uint64_t mBytesReceived;
};
}    // namespace pearl


#endif    // !PEARL_MPIRECEIVE_REP_H
