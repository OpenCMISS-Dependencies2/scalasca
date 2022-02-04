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
**  Copyright (c) 2006                                                     **
**  TU Dresden, Zentrum fuer Informationsdienste und Hochleistungsrechnen  **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include <pearl/LocalTrace.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <map>
#include <sstream>
#include <stack>

#include <pearl/Callpath.h>
#include <pearl/Enter_rep.h>
#include <pearl/Error.h>
#include <pearl/Region.h>

#include "CallTree.h"
#include "Functors.h"
#include "MutableGlobalDefs.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

LocalTrace::LocalTrace(const GlobalDefs& defs,
                       const Location&   location)
    : m_defs(defs),
      m_omp_parallel(0),
      m_mpi_regions(0),
      m_location(const_cast< Location* >(&location))
{
    // Estimate capacity for the event representation vector.
    //
    // The number of events may be underestimated in the presence of buffer
    // flushes and overestimated in the presence of metric events.  However,
    // even if the number is underestimated and a single reallocation happens,
    // this is still more efficient than resizing the event representation
    // vector multiple times on-the-fly.
    const uint64_t numEvents = m_location->getNumEvents();
    if (numEvents > 0)
    {
        m_events.reserve(numEvents);
    }
}


LocalTrace::~LocalTrace()
{
    /* Delete events */
    for_each(m_events.begin(), m_events.end(), delete_ptr< Event_rep >());
}


// --- Iterator handling ----------------------------------------------------

LocalTrace::iterator
LocalTrace::begin() const
{
    return Event(this);
}


LocalTrace::iterator
LocalTrace::end() const
{
    return Event(this, true);
}


LocalTrace::reverse_iterator
LocalTrace::rbegin() const
{
    return reverse_iterator(end());
}


LocalTrace::reverse_iterator
LocalTrace::rend() const
{
    return reverse_iterator(begin());
}


LocalTrace::iterator
LocalTrace::at(uint32_t index) const
{
    return Event(this, index);
}


// --- Get trace information ------------------------------------------------

uint32_t
LocalTrace::size() const
{
    return m_events.size();
}


uint32_t
LocalTrace::num_events() const
{
    return m_events.size();
}


uint32_t
LocalTrace::num_omp_parallel() const
{
    return m_omp_parallel;
}


uint32_t
LocalTrace::num_mpi_regions() const
{
    return m_mpi_regions;
}


const Location&
LocalTrace::get_location() const
{
    return *m_location;
}


const GlobalDefs&
LocalTrace::get_definitions() const
{
    return m_defs;
}


// --- Store new events -----------------------------------------------------

void
LocalTrace::add_event(Event_rep* event)
{
    // When entering an OpenMP parallel or MPI region, increment counter
    if (event->isOfType(GROUP_ENTER))
    {
        Enter_rep*    enter  = static_cast< Enter_rep* >(event);
        const Region& region = enter->getRegionEntered();

        if (is_omp_parallel(region))
        {
            m_omp_parallel++;
        }

        if (is_mpi_api(region))
        {
            m_mpi_regions++;
        }
    }

    // Store event representation
    m_events.push_back(event);
}


// --- Inserting & removing elements ----------------------------------------

LocalTrace::iterator
LocalTrace::remove_if(bool (* predicate)(const Event_rep* event))
{
    uint32_t index = std::remove_if(m_events.begin(),
                                    m_events.end(),
                                    predicate) - m_events.begin();

    return Event(this, index);
}


LocalTrace::iterator
LocalTrace::insert(LocalTrace::iterator pos,
                   Event_rep*           event)
{
    m_events.insert(m_events.begin() + pos.m_index, event);

    const MutableGlobalDefs& mutableDefs =
        dynamic_cast< const MutableGlobalDefs& >(m_defs);
    mutableDefs.getCallTree()->setModified();

    return pos;
}


LocalTrace::iterator
LocalTrace::erase(LocalTrace::iterator pos)
{
    assert(pos.m_index < size());

    m_events.erase(m_events.begin() + pos.m_index);

    const MutableGlobalDefs& mutableDefs =
        dynamic_cast< const MutableGlobalDefs& >(m_defs);
    mutableDefs.getCallTree()->setModified();

    return pos;
}


LocalTrace::iterator
LocalTrace::erase(LocalTrace::iterator begin,
                  LocalTrace::iterator end)
{
    assert(begin.m_index < size());

    m_events.erase(m_events.begin() + begin.m_index,
                   m_events.begin() + end.m_index);

    const MutableGlobalDefs& mutableDefs =
        dynamic_cast< const MutableGlobalDefs& >(m_defs);
    mutableDefs.getCallTree()->setModified();

    return begin;
}


LocalTrace::iterator
LocalTrace::replace(LocalTrace::iterator pos,
                    Event_rep*           event)
{
    m_events[pos.m_index] = event;

    const MutableGlobalDefs& mutableDefs =
        dynamic_cast< const MutableGlobalDefs& >(m_defs);
    mutableDefs.getCallTree()->setModified();

    return pos;
}


LocalTrace::iterator
LocalTrace::swap(const LocalTrace::iterator& a,
                 const LocalTrace::iterator& b)
{
    std::iter_swap(m_events.begin() + a.m_index, m_events.begin() + b.m_index);

    const MutableGlobalDefs& mutableDefs =
        dynamic_cast< const MutableGlobalDefs& >(m_defs);
    mutableDefs.getCallTree()->setModified();

    return a;
}


// --- Private methods ------------------------------------------------------

Event_rep*
LocalTrace::operator[](uint32_t index) const
{
    return m_events[index];
}
