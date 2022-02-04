/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2018                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_LEAVEPROGRAM_REP_H
#define PEARL_LEAVEPROGRAM_REP_H


#include <pearl/Leave_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    LeaveProgram_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class LeaveProgram_rep.
 *
 *  This header file provides the declaration of the class LeaveProgram_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   LeaveProgram_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for PROGRAM_END events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR LeaveProgram_rep
    : public Leave_rep
{
    public:
        // --- Public member functions ---------------------

        /// @name Constructors & destructor
        /// @{

        LeaveProgram_rep(timestamp_t   timestamp,
                         const Region& region,
                         uint64_t*     metrics,
                         int64_t       exitStatus);

        LeaveProgram_rep(const GlobalDefs& defs,
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

        int64_t
        getExitStatus() const;

        /// @}


    protected:
        // --- Protected member functions ------------------

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
        // --- Data members --------------------------------

        /// Program exit status
        int64_t mExitStatus;
};
}    // namespace pearl


#endif    // !PEARL_LEAVEPROGRAM_REP_H
