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
 *  @brief   Declaration of the class ThreadTaskComplete_rep.
 *
 *  This header file provides the declaration of the class
 *  ThreadTaskComplete_rep.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_THREADTASKCOMPLETE_REP_H
#define PEARL_THREADTASKCOMPLETE_REP_H


#include <pearl/Event_rep.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Communicator;


/*-------------------------------------------------------------------------*/
/**
 *  @class   ThreadTaskComplete_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for THREAD_TASK_COMPLETE events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR ThreadTaskComplete_rep
    : public pearl::Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        ThreadTaskComplete_rep(timestamp_t   timestamp,
                               Communicator* threadTeam,
                               uint64_t      taskId);

        ThreadTaskComplete_rep(const GlobalDefs& defs,
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


#endif    // !PEARL_THREADTASKCOMPLETE_REP_H
