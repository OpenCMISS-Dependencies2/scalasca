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


#ifndef SILAS_SILAS_REPLAY_H
#define SILAS_SILAS_REPLAY_H


#include <pearl/CallbackManager.h>
#include <pearl/LocalTrace.h>

#include "ReenactCallbackData.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    silas_replay.h
 * @brief   Declarations of replay-related library functions.
 *
 * This header file provides declarations of all replay-related library
 * functions provided by SILAS.
 */
/*-------------------------------------------------------------------------*/


namespace silas
{
// --- Function prototypes --------------------------------------------------

/// @name Replay functionality
/// @{

void
SILAS_forward_replay(const pearl::LocalTrace&      trace,
                     const pearl::CallbackManager& cbmanager,
                     CallbackData*                 data,
                     int                           mincbevent);

void
SILAS_backward_replay(const pearl::LocalTrace&      trace,
                      const pearl::CallbackManager& cbmanager,
                      CallbackData*                 data,
                      int                           mincbevent);

/// @}
}    // namespace silas


#endif    // !SILAS_SILAS_REPLAY_H
