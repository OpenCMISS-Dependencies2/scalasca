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


#ifndef PEARL_MPIRMALOCK_REP_H
#define PEARL_MPIRMALOCK_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiRmaLock_rep.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiRmaLock_rep.
 *
 *  This header file provides the declaration of the class MpiRmaLock_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiRmaLock_rep
 *  @ingroup PEARL_mpi
 *  @brief   Class for MPI RMA lock events
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaLock_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        MpiRmaLock_rep(timestamp_t timestamp,
                       uint32_t    location,
                       MpiWindow*  window,
                       bool        exclusive);

        MpiRmaLock_rep(const GlobalDefs& defs,
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

        virtual MpiWindow*
        get_window() const;

        virtual uint32_t
        get_remote() const;

        virtual bool
        is_exclusive() const;

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
        /// MPI window locked
        MpiWindow* m_window;

        /// Local rank of lock location w.r.t. to communicator associated to
        /// the MPI window
        uint32_t m_location;

        /// Exclusive lock flag
        bool m_exclusive;
};
}    // namespace pearl


#endif    // !PEARL_MPIRMALOCK_REP_H
