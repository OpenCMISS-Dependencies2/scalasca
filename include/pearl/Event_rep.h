/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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


#ifndef PEARL_EVENT_REP_H
#define PEARL_EVENT_REP_H


#include <iosfwd>

#include <pearl/SmallObject.h>
#include <pearl/pearl_padding.h>
#include <pearl/pearl_types.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    Event_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Event_rep.
 *
 *  This header file provides the declaration of the class Event_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Buffer;
class Callsite;
class GlobalDefs;
class LocalTrace;
class MpiComm;
class MpiGroup;
class MpiWindow;
class Region;


/*-------------------------------------------------------------------------*/
/**
 *  @class   Event_rep
 *  @ingroup PEARL_base
 *  @brief   Base class for all event representations.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR Event_rep
    : public SmallObject
{
    public:
        /// @name Constructors & destructor
        /// @{

        virtual
        ~Event_rep();

        /// @}
        /// @name Event type information
        /// @{

        virtual event_t
        getType() const = 0;

        virtual bool
        isOfType(event_t type) const = 0;

        /// @}
        /// @name Comparison operators
        /// @{

        bool
        operator==(const timestamp_t& rhs) const;

        bool
        operator!=(const timestamp_t& rhs) const;

        bool
        operator<(const timestamp_t& rhs) const;

        bool
        operator>(const timestamp_t& rhs) const;

        /// @}
        /// @name Access event data
        /// @{

        timestamp_t
        getTimestamp() const;

        virtual const Region&
        getRegion() const;

        virtual const Callsite&
        getCallsite() const;

        virtual MpiComm*
        getComm() const;

        virtual uint32_t
        getRoot() const;

        virtual uint64_t
        getBytesSent() const;

        virtual uint64_t
        getBytesReceived() const;

        virtual uint32_t
        getSource() const;

        virtual uint32_t
        getDestination() const;

        virtual uint32_t
        getTag() const;

        virtual uint64_t
        getRequestId() const;

        virtual uint32_t
        getLockId() const;

        virtual MpiWindow*
        get_window() const;

        virtual uint32_t
        get_remote() const;

        virtual uint32_t
        get_rma_id() const;

        virtual MpiGroup*
        get_group() const;

        virtual bool
        is_sync() const;

        virtual bool
        is_exclusive() const;

        #ifdef PEARL_ENABLE_METRICS
            virtual uint64_t
            getMetric(int index) const;
        #endif

        /// @}
        /// @name Modify event data
        /// @{

        void
        setTimestamp(timestamp_t timestamp);

        virtual void
        setRequestId(uint64_t requestId);

        /// @}


    protected:
        /// @name Constructors & destructor
        /// @{

        Event_rep(pearl::timestamp_t timestamp);

        Event_rep(const GlobalDefs& defs,
                  Buffer&           buffer);

        /// @}
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
        /// @name Find previous/next communication request entries
        /// @{

        virtual uint32_t
        get_next_reqoffs() const;

        virtual uint32_t
        get_prev_reqoffs() const;

        virtual void
        set_prev_reqoffs(uint32_t offset);

        virtual void
        set_next_reqoffs(uint32_t offset);

        /// @}


    private:
        /// Timestamp when the event has occured
        timestamp_t mTimestamp;


        /* Declare friends */
        friend class Buffer;
        friend class Event;
        friend class RemoteEvent;

        friend void
        PEARL_preprocess_trace(const GlobalDefs& defs,
                               const LocalTrace& trace);
};


// --- Related functions ----------------------------------------------------

template< class EvtRepT >
inline EvtRepT&
event_cast(Event_rep& event)
{
    #if defined(HAVE_SCALASCA_DEBUG)
        return dynamic_cast< EvtRepT& >(event);
    #else
        return static_cast< EvtRepT& >(event);
    #endif
}


// --- Inline methods -------------------------------------------------------

#include "Inline/Event_rep-inl.h"
}    // namespace pearl


#endif    // !PEARL_EVENT_REP_H
