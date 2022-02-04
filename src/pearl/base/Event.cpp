/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class Event.
 *
 *  This file provides the implementation of the class Event.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/Event.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/Callpath.h>
#include <pearl/Enter_rep.h>
#include <pearl/Error.h>
#include <pearl/Event_rep.h>
#include <pearl/GlobalDefs.h>
#include <pearl/Leave_rep.h>
#include <pearl/LocalTrace.h>
#include <pearl/Location.h>
#include <pearl/ThreadTaskComplete_rep.h>
#include <pearl/ThreadTaskSwitch_rep.h>
#include <pearl/pearl.h>

#include "Threading.h"
#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


#define ITEM    (*m_trace)[m_index]


// --- Constructors & destructor --------------------------------------------

Event::Event()
    : m_trace(0),
      m_index(0)
{
}


// --- Comparison operators -------------------------------------------------

bool
Event::operator==(const Event& rhs) const
{
    return (  (m_trace == rhs.m_trace)
           && (m_index == rhs.m_index));
}


bool
Event::operator!=(const Event& rhs) const
{
    return (  (m_trace != rhs.m_trace)
           || (m_index != rhs.m_index));
}


bool
Event::operator<(const Event& rhs) const
{
    // Compare timestamps
    return (ITEM->getTimestamp() < (*rhs.m_trace)[rhs.m_index]->getTimestamp());
}


bool
Event::operator>(const Event& rhs) const
{
    // Compare timestamps
    return (rhs < *this);
}


// --- Forward iterator interface -------------------------------------------

Event&
Event::operator++()
{
    ++m_index;

    return *this;
}


Event
Event::operator++(int)
{
    Event result(*this);
    ++m_index;

    return result;
}


// --- Backward iterator interface ------------------------------------------

Event&
Event::operator--()
{
    --m_index;

    return *this;
}


Event
Event::operator--(int)
{
    Event result(*this);
    --m_index;

    return result;
}


// --- Check iterator validity ----------------------------------------------

bool
Event::is_valid() const
{
    return (m_trace && (m_index < m_trace->size()));
}


// --- Access related events ------------------------------------------------

Event
Event::enterptr() const
{
    // Search for associated ENTER event while keeping track of nested
    // ENTERs/LEAVEs (necessary because of possible buffer flushing)
    int   depth = 0;
    Event result(*this);

    // task switch related
    uint64_t targetTaskId = (static_cast< uint64_t >(PEARL_GetThreadNumber()) << 32);
    uint64_t prevId;
    bool     encounteredTasks = false;
    bool     skipTaskBlock    = false;

    --result;

    while (  !(result->isOfType(GROUP_ENTER) && (depth == 0))
          || skipTaskBlock)
    {
        if ((result->isOfType(GROUP_LEAVE)) && !skipTaskBlock)
        {
            ++depth;
        }
        if ((result->isOfType(GROUP_ENTER)) && !skipTaskBlock)
        {
            --depth;
        }

        // Task switch requirements
        if (result->isOfType(THREAD_TASK_SWITCH))
        {
            if (!encounteredTasks)
            {
                // only called in tasking case
                // (e.g. avoid repeat calls in non task OpenMP cases)
                // with reference to original position
                targetTaskId     = getTaskBlockId(*this);
                encounteredTasks = true;
            }

            // get id from the next "task switch block" in the prev direction
            prevId = getTaskBlockId(result.prev());
            if (prevId != targetTaskId)
            {
                skipTaskBlock = true;
            }
            else
            {
                skipTaskBlock = false;
            }
        }
        if (result->isOfType(THREAD_TASK_COMPLETE))
        {
            if (!encounteredTasks)
            {
                // aka last THREAD_TASK_COMPLETE in your trace
                // only time it is needed, all other cases are handled by the switch
                // targetTaskId is already initialized with 0 no need to do it again
                encounteredTasks = true;

                // as we now we are on the implicit task and encounter the end of an
                // explicit task, we are safe to skip the next task block
                skipTaskBlock = true;
            }
        }
        --result;
    }

    return result;
}


Event
Event::leaveptr() const
{
    // Search for associated LEAVE event while keeping track of nested
    // ENTERs/LEAVEs (necessary because of possible buffer flushing)
    int   depth = 0;
    Event result(*this);

    // task switch related
    uint64_t targetTaskId = 0;
    uint64_t nextId;
    bool     encounteredTasks = false;
    bool     skipTaskBlock    = false;

    ++result;
    while (  !(result->isOfType(GROUP_LEAVE) && (depth == 0))
          || skipTaskBlock)
    {
        if ((result->isOfType(GROUP_ENTER)) && !skipTaskBlock)
        {
            ++depth;
        }
        if ((result->isOfType(GROUP_LEAVE)) && !skipTaskBlock)
        {
            --depth;
        }

        // Task switch requirements
        if (result->isOfType(THREAD_TASK_SWITCH))
        {
            if (!encounteredTasks)
            {
                // only called in tasking case
                // (e.g. avoid repeat calls in non task OpenMP cases)
                // with reference to original position
                targetTaskId     = getTaskBlockId(*this);
                encounteredTasks = true;
            }

            // get id from the next task block, indicated by this switch
            nextId = getTaskBlockId(result);
            if (nextId != targetTaskId)
            {
                skipTaskBlock = true;
            }
            else
            {
                skipTaskBlock = false;
            }
        }
        if (result->isOfType(THREAD_TASK_COMPLETE))
        {
            // after a TASK_COMPLETE implicit fallback on the implicit task
            // in the common case in the middle of task work this doesn't
            // matter much since the next event will be a TASK_SWITCH again
            // but it catches the "last" TASK_COMPLETE
            if (targetTaskId == (static_cast< uint64_t >(PEARL_GetThreadNumber()) << 32))
            {
                skipTaskBlock = false;
            }
            else
            {
                skipTaskBlock = true;
            }
        }
        ++result;
    }

    return result;
}


Event
Event::beginptr() const
{
    // Sanity check
    if (!ITEM->isOfType(GROUP_END))
    {
        throw FatalError("Event::beginptr() -- "
                         "Method only allowed for END-type events!");
    }

    // Search associated BEGIN-type event
    // NOTE: This assumes that no other BEGIN/END pair can be in between
    Event result(*this);
    --result;
    while (!result->isOfType(GROUP_BEGIN))
    {
        --result;
    }

    return result;
}


Event
Event::endptr() const
{
    // Sanity check
    if (!ITEM->isOfType(GROUP_BEGIN))
    {
        throw FatalError("Event::endptr() -- "
                         "Method only allowed for BEGIN-type events!");
    }

    // Search associated END-type event
    // NOTE: This assumes that no other BEGIN/END pair can be in between
    Event result(*this);
    ++result;
    while (!result->isOfType(GROUP_END))
    {
        ++result;
    }

    return result;
}


Event
Event::request() const
{
    // Search for a corresponding request event.
    // For blocking events, reqoffs is 0, hence *this is returned.

    uint32_t i = m_index;

    for (uint32_t offs = (*m_trace)[i]->get_prev_reqoffs(); offs; i -= offs)
    {
        offs = (*m_trace)[i]->get_prev_reqoffs();
    }

    return Event(m_trace, i);
}


Event
Event::completion() const
{
    // Search for a corresponding completion event

    uint32_t i = m_index;

    for (uint32_t offs = (*m_trace)[i]->get_next_reqoffs(); offs; i += offs)
    {
        offs = (*m_trace)[i]->get_next_reqoffs();
    }

    return Event(m_trace, i);
}


Event
Event::next_request_event() const
{
    // Search for next corresponding test or completion event
    return Event(m_trace, m_index + ITEM->get_next_reqoffs());
}


Event
Event::prev_request_event() const
{
    // Search for previous corresponding test or request event
    return Event(m_trace, m_index - ITEM->get_prev_reqoffs());
}


// --- Access local event information ---------------------------------------

Callpath*
Event::get_cnode() const
{
    if (const Enter_rep* event = dynamic_cast< const Enter_rep* >(ITEM))
    {
        return event->getCallpath();
    }

    if (const Leave_rep* event = dynamic_cast< const Leave_rep* >(ITEM))
    {
        return event->getCallpath();
    }

    return enterptr().get_cnode();
}


const Location&
Event::get_location() const
{
    return m_trace->get_location();
}


// --- Access event representation ------------------------------------------

Event_rep&
Event::operator*() const
{
    return *ITEM;
}


Event_rep*
Event::operator->() const
{
    return ITEM;
}


// --- Private methods ------------------------------------------------------

Event::Event(const LocalTrace* trace,
             bool              isEnd)
    : m_trace(trace),
      m_index(isEnd ? trace->size() : 0)
{
}


Event::Event(const LocalTrace* trace,
             uint32_t          index)
    : m_trace(trace),
      m_index(index)
{
}


void
Event::pack(Buffer& buffer) const
{
    // Store type & location
    buffer.put_uint32(static_cast< uint32_t >(ITEM->getType()));
    buffer.put_uint64(m_trace->get_location().getId());
    buffer.put_id(get_cnode()->getId());
    buffer.put_uint32(m_index);

    // Pack event data
    ITEM->pack(buffer);
}


ostream&
Event::output(ostream& stream) const
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << event_typestr(ITEM->getType());
    }

    // Print data
    stream << "Event"
           << beginObject
           << "type: " << event_typestr(ITEM->getType())
           << sep
           << "location: " << m_trace->get_location()
           << sep
           << "index: " << m_index;
    ITEM->output(stream);
    stream << endObject;

    return stream;
}


// --- Related functions ----------------------------------------------------

ostream&
pearl::operator<<(ostream&     stream,
                  const Event& event)
{
    return event.output(stream);
}


string
pearl::event_typestr(event_t type)
{
    static const char* const type_strings[] = {
        "ENTER",
        "ENTER_CS",
        "ENTER_PROGRAM",
        "LEAVE",
        "LEAVE_PROGRAM",
        "MPI_COLLECTIVE_BEGIN",
        "MPI_COLLECTIVE_END",
        "MPI_SEND",
        "MPI_SEND_REQUEST",
        "MPI_SEND_COMPLETE",
        "MPI_RECV",
        "MPI_RECV_REQUEST",
        "MPI_RECV_COMPLETE",
        "MPI_REQUEST_TESTED",
        "MPI_CANCELLED",
        "THREAD_ACQUIRE_LOCK",
        "THREAD_RELEASE_LOCK",
        "THREAD_TASK_CREATE",
        "THREAD_TASK_COMPLETE",
        "THREAD_TASK_SWITCH",
        "THREAD_BEGIN",
        "THREAD_END",
        "THREAD_CREATE",
        "THREAD_WAIT",
        "THREAD_FORK",
        "THREAD_JOIN",
        "THREAD_TEAM_BEGIN",
        "THREAD_TEAM_END",
        "RMA_PUT_START",
        "RMA_PUT_END",
        "RMA_GET_START",
        "RMA_GET_END",
        "MPI_RMA_PUT_START",
        "MPI_RMA_PUT_END",
        "MPI_RMA_GET_START",
        "MPI_RMA_GET_END",
        "MPI_RMA_GATS",
        "MPI_RMA_COLLECTIVE_BEGIN",
        "MPI_RMA_COLLECTIVE_END",
        "MPI_RMA_LOCK",
        "MPI_RMA_UNLOCK",
        "GROUP_ALL",
        "GROUP_ENTER",
        "GROUP_LEAVE",
        "GROUP_SEND",
        "GROUP_RECV",
        "GROUP_BEGIN",
        "GROUP_END",
        "GROUP_NONBLOCK"
    };

    return type_strings[type];
}


uint64_t
pearl::getTaskBlockId(const Event& event)
{
    Event    iter(event);
    uint32_t startId = iter.get_id();

    while (iter.get_id() > 0)
    {
        // default case: going back to the last task switch and
        // return its id
        if (iter->isOfType(THREAD_TASK_SWITCH))
        {
            ThreadTaskSwitch_rep& taskSwitch = event_cast< ThreadTaskSwitch_rep >(*iter);

            return taskSwitch.getTaskId();
        }
        else if (iter->isOfType(THREAD_TASK_COMPLETE))
        {
            if (iter.get_id() == startId)
            {
                // the event *is* the THREAD_TASK_COMPLETE
                // -> just return its task id, no need to go further
                // back in the trace
                ThreadTaskComplete_rep& taskComplete = event_cast< ThreadTaskComplete_rep >(*iter);

                return taskComplete.getTaskId();
            }

            // Special case: thread doesn't explicitly switch back
            // to task 0, therefore a TASK_COMPLETE is the first
            // task event looking back in the trace.
            // Return 0 directly since it's the implicit task.
            return (static_cast< uint64_t >(PEARL_GetThreadNumber()) << 32);
        }
        else if (iter->isOfType(THREAD_TEAM_BEGIN))
        {
            // implicit task id per thread == shifted thread
            return (static_cast< uint64_t >(PEARL_GetThreadNumber()) << 32);
        }

        --iter;
    }

    // OpenMP without tasks case or you meet the start of the trace
    return (static_cast< uint64_t >(PEARL_GetThreadNumber()) << 32);
}
