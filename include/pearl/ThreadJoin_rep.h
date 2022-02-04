/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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


#ifndef PEARL_THREADJOIN_REP_H
#define PEARL_THREADJOIN_REP_H


#include <pearl/Event_rep.h>
#include <pearl/Paradigm.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    ThreadJoin_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class ThreadJoin_rep.
 *
 *  This header file provides the declaration of the class ThreadJoin_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   ThreadJoin_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for THREAD_JOIN events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR ThreadJoin_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        ThreadJoin_rep(timestamp_t timestamp,
                       Paradigm    paradigm);

        ThreadJoin_rep(const GlobalDefs& defs,
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

        Paradigm
        getParadigm() const;

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
        /// Threading paradigm
        Paradigm mParadigm;
};
}    // namespace pearl


#endif    // !PEARL_THREADJOIN_REP_H
