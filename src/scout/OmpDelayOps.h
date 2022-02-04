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


#ifndef SCOUT_OMPDELAYOPS_H
#define SCOUT_OMPDELAYOPS_H


#include "scout_types.h"


/**
 *  @file  OmpDelayOps.h
 *  @brief Declaration of OpenMP-specific stand-alone functions for delay analysis.
 **/


// --- Forward declarations

namespace pearl
{
class Event;
}    // namespace pearl


namespace scout
{
class CbData;

void
CalculateOmpBarrierDelay(const TimeRank&     my,
                         const TimeRank&     latest,
                         const pearl::Event& event,
                         CbData*             data);
}    // namespace scout


#endif    // !SCOUT_OMPDELAYOPS_H
