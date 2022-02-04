/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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


#ifndef PEARL_ENTER_REP_H
#define PEARL_ENTER_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    Enter_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Enter_rep.
 *
 *  This header file provides the declaration of the class Enter_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Callpath;


/*-------------------------------------------------------------------------*/
/**
 *  @class   Enter_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for entering a source code region.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR Enter_rep
    : public Event_rep
{
    public:
        /// @name Constructors & destructor
        /// @{

        Enter_rep(timestamp_t   timestamp,
                  const Region& region,
                  uint64_t*     metrics);

        Enter_rep(const GlobalDefs& defs,
                  Buffer&           buffer);

        virtual
        ~Enter_rep();

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

        virtual const Region&
        getRegion() const;

        virtual const Callsite&
        getCallsite() const;

        #ifdef PEARL_ENABLE_METRICS
            virtual uint64_t
            getMetric(int index) const;
        #endif

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
        union
        {
            /// Region being entered (valid before preprocessing)
            Region* mRegion;

            /// Corresponding callpath (valid after preprocessing)
            Callpath* mCallpath;
        };


        #ifdef PEARL_ENABLE_METRICS
            /// Hardware counter metric values
            uint64_t* mMetrics;
        #endif


        /* Private methods */
        const Region&
        getRegionEntered() const;

        Callpath*
        getCallpath() const;

        void
        setCallpath(Callpath* callpath);


        /* Declare friends */
        friend class Event;
        friend class LocalTrace;

        friend void
        PEARL_verify_calltree(GlobalDefs& defs,
                              LocalTrace& trace);

        friend void
        PEARL_preprocess_trace(const GlobalDefs& defs,
                               const LocalTrace& trace);
};
}    // namespace pearl


#endif    // !PEARL_ENTER_REP_H
