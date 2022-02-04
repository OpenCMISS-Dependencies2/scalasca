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


#ifndef PEARL_RMAGETSTART_REP_H
#define PEARL_RMAGETSTART_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    RmaGetStart_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class RmaGetStart_rep.
 *
 *  This header file provides the declaration of the class RmaGetStart_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   RmaGetStart_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for RMA_GET_START events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR RmaGetStart_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        RmaGetStart_rep(timestamp_t timestamp,
                        uint32_t    rma_id,
                        uint32_t    remote,
                        uint64_t    bytesReceived);

        RmaGetStart_rep(const GlobalDefs& defs,
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

        virtual uint32_t
        get_remote() const;

        virtual uint64_t
        getBytesReceived() const;

        virtual uint32_t
        get_rma_id() const;

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
        /// Rank of origin location (can be local of global, depending on the
        /// programming model)
        uint32_t m_remote;

        /// Length of data in bytes
        uint64_t mBytesReceived;

        /// RMA ID
        uint32_t m_rma_id;
};
}    // namespace pearl


#endif    // !PEARL_RMAGETSTART_REP_H
