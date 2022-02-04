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


#ifndef SCOUT_MPIDELAYOPS_H
#define SCOUT_MPIDELAYOPS_H


#include "scout_types.h"


/**
 *  @file  MpiDelayOps.h
 *  @brief Declaration of MPI-specific stand-alone functions for delay analysis.
 **/


// --- Forward declarations

namespace pearl
{
class Event;
}    // namespace pearl


namespace scout
{
class CbData;
class SynchpointHandler;
class TmapCacheHandler;

timemap_t
GetMpiDelayTimemap(const pearl::Event&      event,
                   int                      worldrank,
                   const SynchpointHandler* SpH,
                   const TmapCacheHandler*  TmH);

void
CalculateCollectiveDelay(const TimeRank&     my,
                         const TimeRank&     root,
                         const pearl::Event& event,
                         CbData*             data);
}    // namespace scout


#endif    // !SCOUT_MPIDELAYOPS_H
