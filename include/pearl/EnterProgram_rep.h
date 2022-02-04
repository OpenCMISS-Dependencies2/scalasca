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


#ifndef PEARL_ENTERPROGRAM_REP_H
#define PEARL_ENTERPROGRAM_REP_H


#include <vector>

#include <pearl/Enter_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    EnterProgram_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class EnterProgram_rep.
 *
 *  This header file provides the declaration of the class EnterProgram_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   EnterProgram_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for PROGRAM_BEGIN events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR EnterProgram_rep
    : public Enter_rep
{
    public:
        // --- Public member functions ---------------------

        /// @name Constructors & destructor
        /// @{

        EnterProgram_rep(timestamp_t                         timestamp,
                         const Region&                       region,
                         uint64_t*                           metrics,
                         const String&                       name,
                         const std::vector< const String* >& arguments);

        EnterProgram_rep(const GlobalDefs& defs,
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

        const String&
        getName() const;

        uint32_t
        numArguments() const;

        const String&
        getArgument(uint32_t index) const;

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

        /// Name of the program executable
        const String& mName;

        /// List of command-line arguments
        std::vector< const String* > mArguments;
};
}    // namespace pearl


#endif    // !PEARL_ENTERPROGRAM_REP_H
