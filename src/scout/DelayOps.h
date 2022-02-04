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


#ifndef SCOUT_DELAYOPS_H
#define SCOUT_DELAYOPS_H


#include "scout_types.h"


/**
 *  @file  DelayOps.h
 *  @brief Declaration of various stand-alone functions for delay analysis.
 **/


// --- Forward declarations

namespace pearl
{
class Buffer;
class Event;
}    // namespace pearl


namespace scout
{
void
PackTimemap(pearl::Buffer*   buf,
            const timemap_t& tmap);

timemap_t
UnpackTimemap(pearl::Buffer* buf);

TimeMapSum
CalculateWaitMap(const SynchpointInfoMap& spmap);

TimeMapSum
PruneDelayTimemap(const timemap_t&      tmap,
                  const timemap_t&      waitmap,
                  const pearl::Event&   from,
                  const SynchpointInfo& spi);

TimeMapSum
CalculateDelay(const timemap_t& ltmap,
               const timemap_t& lwmap,
               const timemap_t& rtmap);

TimeMapSum
CalculateDiff(const timemap_t& ltmap,
              const timemap_t& rtmap);

TimeMapSum
CalculateDiff(const TimeMapSum& ltmap,
              const timemap_t&  rtmap);

void
AddScaledTimemap(const TimeMapSum& from,
                 double            scale,
                 TimeMapSum&       to);
}    // namespace scout


#endif    // !SCOUT_DELAYOPS_H
