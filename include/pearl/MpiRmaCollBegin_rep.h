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


#ifndef PEARL_MPIRMACOLLBEGIN_REP_H
#define PEARL_MPIRMACOLLBEGIN_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiRmaCollBegin_rep.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiRmaCollBegin_rep.
 *
 *  This header file provides the declaration of the class
 *  MpiRmaCollBegin_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiRmaCollBegin_rep
 *  @ingroup PEARL_mpi
 *  @brief   %Event representation for the begin of MPI RMA collective
 *           operations.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaCollBegin_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        MpiRmaCollBegin_rep(timestamp_t timestamp);

        MpiRmaCollBegin_rep(const GlobalDefs& defs,
                            Buffer&           buffer);

        /// @}
        /// @name Event type information
        /// @{

        virtual event_t
        getType() const;

        virtual bool
        isOfType(event_t type) const;

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_MPIRMACOLLBEGIN_REP_H
