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
**  Copyright (c) 2006                                                     **
**  TU Dresden, Zentrum fuer Informationsdienste und Hochleistungsrechnen  **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_EVENT_H
#define PEARL_EVENT_H


#include <cassert>
#include <iosfwd>
#include <iterator>
#include <string>

#include <pearl/Event_rep.h>
#include <pearl/pearl_types.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    Event.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Event.
 *
 *  This header file provides the declaration of the class Event.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Buffer;
class Callpath;
class LocalTrace;
class Location;

struct EventKeyCompare;


/*-------------------------------------------------------------------------*/
/**
 *  @class   Event
 *  @ingroup PEARL_base
 *  @brief   Generic representation for local events with iterator
 *           functionality.
 **/
/*-------------------------------------------------------------------------*/

class Event
    : public std::iterator< std::bidirectional_iterator_tag, Event_rep >
{
    public:
        /// @name Construcor

        Event();

        /// @name Comparison operators
        /// @{

        bool
        operator==(const Event& rhs) const;

        bool
        operator!=(const Event& rhs) const;

        bool
        operator<(const Event& rhs) const;

        bool
        operator>(const Event& rhs) const;

        /// @}
        /// @name Forward iterator interface
        /// @{

        Event&
        operator++();

        Event
        operator++(int);

        /// @}
        /// @name Backward iterator interface
        /// @{

        Event&
        operator--();

        Event
        operator--(int);

        /// @}
        /// @name Check iterator validity
        /// @{

        bool
        is_valid() const;

        /// @}
        /// @name Check identity
        /// @{

        uint32_t
        get_id() const
        {
            return m_index;
        }

        /// @}
        /// @name Access related events
        /// @{

        Event
        next() const
        {
            return ++Event(*this);
        }

        Event
        prev() const
        {
            return --Event(*this);
        }

        Event
        enterptr() const;

        Event
        leaveptr() const;

        Event
        beginptr() const;

        Event
        endptr() const;

        Event
        request() const;

        Event
        completion() const;

        Event
        next_request_event() const;

        Event
        prev_request_event() const;

        /// @}
        /// @name Access local event information
        /// @{

        Callpath*
        get_cnode() const;

        const Location&
        get_location() const;

        /// @}
        /// @name Access event representation
        /// @{

        Event_rep&
        operator*() const;

        Event_rep*
        operator->() const;

        /// @}


    private:
        /// Associated local event trace
        const LocalTrace* m_trace;

        /// Index of current event
        uint32_t m_index;


        /* Private methods */
        Event(const LocalTrace* trace,
              bool              isEnd = false);

        Event(const LocalTrace* trace,
              uint32_t          index);

        void
        pack(Buffer& buffer) const;

        std::ostream&
        output(std::ostream& stream) const;


        /* Declare friends */
        friend class Buffer;
        friend class LocalTrace;

        friend struct EventKeyCompare;

        friend std::ostream&
        operator<<(std::ostream& stream,
                   const Event&  event);
};


// --- Related functions ----------------------------------------------------

std::ostream&
operator<<(std::ostream& stream,
           const Event&  event);

std::string
event_typestr(event_t type);

uint64_t
getTaskBlockId(const Event& event);


/*-------------------------------------------------------------------------*/
/**
 * @struct  EventKeyCompare
 * @ingroup PEARL_base
 * @brief   Provides a comparison operator that allows Event objects to be
 *          as key in std::map or std::set
 */
/*-------------------------------------------------------------------------*/

struct EventKeyCompare
{
    bool
    operator()(const Event& lhs,
               const Event& rhs) const
    {
        assert(lhs.m_trace == rhs.m_trace);

        return (lhs.m_index < rhs.m_index);
    }
};
}    // namespace pearl


#endif    // !PEARL_EVENT_H
