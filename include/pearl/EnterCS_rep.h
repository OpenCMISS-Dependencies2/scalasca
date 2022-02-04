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


#ifndef PEARL_ENTERCS_REP_H
#define PEARL_ENTERCS_REP_H


#include <pearl/Enter_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    EnterCS_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class EnterCS_rep.
 *
 *  This header file provides the declaration of the class EnterCS_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   EnterCS_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for entering a call site.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR EnterCS_rep
    : public Enter_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        EnterCS_rep(timestamp_t     timestamp,
                    const Callsite& callsite,
                    uint64_t*       metrics);

        EnterCS_rep(const GlobalDefs& defs,
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

        virtual const Callsite&
        getCallsite() const;

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
        /// Associated callsite
        const Callsite& mCallsite;
};
}    // namespace pearl


#endif    // !PEARL_ENTERCS_REP_H
