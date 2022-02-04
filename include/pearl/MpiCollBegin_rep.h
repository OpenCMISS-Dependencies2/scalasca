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


#ifndef PEARL_MPICOLLBEGIN_REP_H
#define PEARL_MPICOLLBEGIN_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiCollBegin_rep.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiCollBegin_rep.
 *
 *  This header file provides the declaration of the class MpiCollBegin_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiCollBegin_rep
 *  @ingroup PEARL_mpi
 *  @brief   %Event representation for the begin of MPI collective
 *           operations.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiCollBegin_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        MpiCollBegin_rep(timestamp_t timestamp);

        MpiCollBegin_rep(const GlobalDefs& defs,
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


#endif    // !PEARL_MPICOLLBEGIN_REP_H
