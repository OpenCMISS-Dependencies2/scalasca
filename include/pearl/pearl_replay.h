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


#ifndef PEARL_PEARL_REPLAY_H
#define PEARL_PEARL_REPLAY_H


#include <pearl/LocalTrace.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    pearl_replay.h
 *  @ingroup PEARL_replay
 *  @brief   Declarations of replay-related library functions.
 *
 *  This header file provides declarations of all replay-related library
 *  functions provided by PEARL.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class CallbackData;
class CallbackManager;


// --- Function prototypes --------------------------------------------------

/// @name Replay functionality
/// @{

void
PEARL_forward_replay(const LocalTrace&           trace,
                     const CallbackManager&      cbmanager,
                     CallbackData*               data,
                     const LocalTrace::iterator& begin,
                     const LocalTrace::iterator& end);

void
PEARL_forward_replay(const LocalTrace&      trace,
                     const CallbackManager& cbmanager,
                     CallbackData*          data);


void
PEARL_backward_replay(const LocalTrace&                   trace,
                      const CallbackManager&              cbmanager,
                      CallbackData*                       data,
                      const LocalTrace::reverse_iterator& rbegin,
                      const LocalTrace::reverse_iterator& rend);

void
PEARL_backward_replay(const LocalTrace&      trace,
                      const CallbackManager& cbmanager,
                      CallbackData*          data);

/// @}


// --- Inline functions -----------------------------------------------------

inline void
PEARL_forward_replay(const LocalTrace&      trace,
                     const CallbackManager& cbmanager,
                     CallbackData*          data)
{
    PEARL_forward_replay(trace, cbmanager, data, trace.begin(), trace.end());
}


inline void
PEARL_backward_replay(const LocalTrace&      trace,
                      const CallbackManager& cbmanager,
                      CallbackData*          data)
{
    PEARL_backward_replay(trace, cbmanager, data, trace.rbegin(), trace.rend());
}
}    // namespace pearl


#endif    // !PEARL_PEARL_REPLAY_H
