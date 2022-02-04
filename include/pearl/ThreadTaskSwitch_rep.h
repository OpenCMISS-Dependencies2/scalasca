/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class ThreadTaskSwitch_rep.
 *
 *  This header file provides the declaration of the class
 *  ThreadTaskSwitch_rep.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_THREADTASKSWITCH_REP_H
#define PEARL_THREADTASKSWITCH_REP_H


#include <pearl/Event_rep.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Communicator;


/*-------------------------------------------------------------------------*/
/**
 *  @class   ThreadTaskSwitch_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for THREAD_TASK_SWITCH events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR ThreadTaskSwitch_rep
    : public pearl::Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        ThreadTaskSwitch_rep(timestamp_t   timestamp,
                             Communicator* threadTeam,
                             uint64_t      taskId);

        ThreadTaskSwitch_rep(const GlobalDefs& defs,
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

        uint64_t
        getTaskId() const;

        Communicator*
        getThreadTeam() const;

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
        /// Task identifier
        uint64_t mTaskId;

        /// Thread team
        Communicator* mThreadTeam;
};
}    // namespace pearl


#endif    // !PEARL_THREADTASKSWITCH_REP_H
