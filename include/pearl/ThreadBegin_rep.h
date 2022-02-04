/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2014-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class ThreadBegin_rep.
 *
 *  This header file provides the declaration of the class ThreadBegin_rep.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_THREADBEGIN_REP_H
#define PEARL_THREADBEGIN_REP_H


#include <pearl/Event_rep.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Communicator;


/*-------------------------------------------------------------------------*/
/**
 *  @class   ThreadBegin_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for THREAD_BEGIN events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR ThreadBegin_rep
    : public pearl::Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        ThreadBegin_rep(timestamp_t   timestamp,
                        Communicator* threadContingent,
                        uint64_t      sequenceCount);

        ThreadBegin_rep(const GlobalDefs& defs,
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
        getSequenceCount() const;

        Communicator*
        getThreadContingent() const;

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
        /// Thread creation sequence count
        uint64_t mSequenceCount;

        /// Thread contingent
        Communicator* mThreadContingent;
};
}    // namespace pearl


#endif    // !PEARL_THREADBEGIN_REP_H
