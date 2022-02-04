/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "DelayOps.h"

#include <pearl/Buffer.h>
#include <pearl/Callpath.h>
#include <pearl/Event.h>


namespace scout
{
/// @brief pack a timemap into a pearl::Buffer

void
PackTimemap(pearl::Buffer*   buf,
            const timemap_t& tmap)
{
    buf->put_uint32(tmap.size());

    for (timemap_t::const_iterator it = tmap.begin(); it != tmap.end(); ++it)
    {
        buf->put_id(it->first);
        buf->put_double(it->second);
    }
}


/// @brief unpack a timemap from a pearl::Buffer

timemap_t
UnpackTimemap(pearl::Buffer* buf)
{
    uint32_t  size = buf->get_uint32();
    timemap_t tmap;

    for (uint32_t i = 0; i < size; ++i)
    {
        pearl::ident_t id   = buf->get_id();
        double         time = buf->get_double();

        tmap[id] += time;
    }

    return tmap;
}


/// @brief Subtract wait states and first-sp communication time from time map

TimeMapSum
PruneDelayTimemap(const timemap_t&      tmap,
                  const timemap_t&      waitmap,
                  const pearl::Event&   from,
                  const SynchpointInfo& spi)
{
    TimeMapSum m = CalculateDiff(tmap, waitmap);

    if (from.is_valid())
    {
        pearl::Callpath::IdType id = from.get_cnode()->getId();

        double mapidtime = m.mMap[id];

        m.mMap[id] = std::max(mapidtime - spi.totalTime + spi.completionTime, 0.0);
        m.mSum    -= std::min(spi.totalTime - spi.completionTime, mapidtime);
    }

    return m;
}


/// @brief create a CNode -> time map with waiting times of synchpoints

TimeMapSum
CalculateWaitMap(const SynchpointInfoMap& s)
{
    TimeMapSum m;

    m.mMap.clear();
    m.mSum = 0.0;

    for (SynchpointInfoMap::const_iterator it = s.begin(); it != s.end(); ++it)
    {
        if (it->second.waitTime > 0)
        {
            m.mMap[it->first.get_cnode()->getId()] += it->second.waitTime;
            m.mSum                                 += it->second.waitTime;
        }
    }

    return m;
}


/// @brief Calulate delay map

TimeMapSum
CalculateDelay(const timemap_t& ltmap,
               const timemap_t& lwmap,
               const timemap_t& rtmap)
{
    TimeMapSum m;

    m.mMap.clear();
    m.mSum = 0.0;

    timemap_t::const_iterator ltmend = ltmap.end();

    for (timemap_t::const_iterator it = ltmap.begin(); it != ltmend; ++it)
    {
        timemap_t::const_iterator fit;

        fit = lwmap.find(it->first);
        double pw = fit == lwmap.end() ? 0.0 : fit->second;
        fit = rtmap.find(it->first);
        double rt = fit == rtmap.end() ? 0.0 : fit->second;

        double d = it->second - pw - rt;

        if (d > 0.0)
        {
            m.mMap[it->first] = d;
            m.mSum           += d;
        }
    }

    return m;
}


TimeMapSum
CalculateDiff(const timemap_t& ltmap,
              const timemap_t& rtmap)
{
    TimeMapSum m;

    m.mMap.clear();
    m.mSum = 0.0;

    timemap_t::const_iterator ltmapend = ltmap.end();
    timemap_t::const_iterator rtmapend = rtmap.end();

    for (timemap_t::const_iterator it = ltmap.begin(); it != ltmapend; ++it)
    {
        timemap_t::const_iterator fit;

        fit = rtmap.find(it->first);
        double rt = fit == rtmapend ? 0.0 : fit->second;
        double d  = it->second - rt;

        if (d > 0.0)
        {
            m.mMap[it->first] = d;
            m.mSum           += d;
        }
    }

    return m;
}


TimeMapSum
CalculateDiff(const TimeMapSum& ltmap,
              const timemap_t&  rtmap)
{
    TimeMapSum m;

    m.mMap.clear();
    m.mSum = 0.0;

    timemap_t::const_iterator ltmapend = ltmap.mMap.end();
    timemap_t::const_iterator rtmapend = rtmap.end();

    for (timemap_t::const_iterator it = ltmap.mMap.begin();
         it != ltmapend;
         ++it)
    {
        timemap_t::const_iterator fit;

        fit = rtmap.find(it->first);
        double rt = fit == rtmapend ? 0.0 : fit->second;
        double d  = it->second - rt;

        if (d > 0.0)
        {
            m.mMap[it->first] = d;
            m.mSum           += d;
        }
    }

    return m;
}


void
AddScaledTimemap(const TimeMapSum& from,
                 double            scale,
                 TimeMapSum&       to)
{
    timemap_t::const_iterator end = from.mMap.end();

    for (timemap_t::const_iterator it = from.mMap.begin(); it != end; ++it)
    {
        to.mMap[it->first] += scale * it->second;
        to.mSum            += scale * it->second;
    }
}
}    // namespace scout
