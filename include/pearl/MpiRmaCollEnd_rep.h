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


#ifndef PEARL_MPIRMACOLLEND_REP_H
#define PEARL_MPIRMACOLLEND_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiRmaCollEnd_rep.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiRmaCollEnd_rep.
 *
 *  This header file provides the declaration of the class MpiRmaCollEnd_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiRmaCollEnd_rep
 *  @ingroup PEARL_mpi
 *  @brief   %Event representation for the end of MPI RMA collective
 *           operations.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaCollEnd_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        MpiRmaCollEnd_rep(timestamp_t timestamp,
                          MpiWindow*  window);

        MpiRmaCollEnd_rep(const GlobalDefs& defs,
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
};
}    // namespace pearl


#endif    // !PEARL_MPIRMACOLLEND_REP_H
