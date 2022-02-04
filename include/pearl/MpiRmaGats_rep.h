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


#ifndef PEARL_MPIRMAGATS_REP_H
#define PEARL_MPIRMAGATS_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiRmaGats_rep.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiRmaGats_rep.
 *
 *  This header file provides the declaration of the class MpiRmaGats_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiRmaGats_rep
 *  @ingroup PEARL_mpi
 *  @brief   %Event representation for MPI_RMAGATS events representing
 *           group-based synchronization.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaGats_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        MpiRmaGats_rep(timestamp_t timestamp,
                       MpiWindow*  window,
                       MpiGroup*   group,
                       bool        sync);

        MpiRmaGats_rep(const GlobalDefs& defs,
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

        virtual MpiGroup*
        get_group() const;

        virtual bool
        is_sync() const;

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
        /// Memory window
        MpiWindow* m_window;

        /// Access group identifier
        MpiGroup* m_group;

        /// Synchronising flag
        bool m_sync;
};
}    // namespace pearl


#endif    // !PEARL_MPIRMAGATS_REP_H
