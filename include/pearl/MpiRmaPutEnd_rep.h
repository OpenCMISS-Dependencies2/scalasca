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


#ifndef PEARL_MPIRMAPUTEND_REP_H
#define PEARL_MPIRMAPUTEND_REP_H


#include <pearl/RmaPutEnd_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiRmaPutEnd_rep.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiRmaPutEnd_rep.
 *
 *  This header file provides the declaration of the class MpiRmaPutEnd_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiRmaPutEnd_rep
 *  @ingroup PEARL_mpi
 *  @brief   %Event representation for RMA_PUT_END events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaPutEnd_rep
    : public RmaPutEnd_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        MpiRmaPutEnd_rep(timestamp_t timestamp,
                         uint32_t    rma_id);

        MpiRmaPutEnd_rep(const GlobalDefs& defs,
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


#endif    // !PEARL_MPIRMAPUTEND_REP_H
