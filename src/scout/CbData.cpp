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


/*-------------------------------------------------------------------------*/
/**
 * @file
 * @brief Implementation of the CbData struct member functions.
 *
 * This file provides the implementation of the CbData struct member
 * functions.
 */
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "CbData.h"

#include "Callstack.h"

#if defined(_MPI)
    #include <pearl/LocalData.h>
    #include <pearl/RemoteData.h>
#endif    // _MPI

using namespace std;
using namespace pearl;
using namespace scout;


// --- Constructors & destructor --------------------------------------------

/// @todo[C++11] Initialize pointers with `nullptr` instead of `0`
CbData::CbData()
    : mLocal(0),
      mRemote(0),
      mInvLocal(0),
      mInvRemote(0),
      mDefs(0),
      mTrace(0),
      mSynchpointHandler(0),
      mTmapCacheHandler(0),
      mOmpEventHandler(0),
      mIsSyncpoint(false),
      mIdle(0.0),
      mCompletion(0.0)
{
    mCallstack = new Callstack;
    #if defined(_MPI)
        mLocal     = new LocalData;
        mRemote    = new RemoteData;
        mInvLocal  = new LocalData;
        mInvRemote = new RemoteData;
    #endif    // _MPI
}


CbData::~CbData()
{
    delete mCallstack;
    #if defined(_MPI)
        delete mLocal;
        delete mRemote;
        delete mInvLocal;
        delete mInvRemote;
    #endif    // _MPI
}


// --- Pre- and postprocessing ----------------------------------------------

/// @todo Implicit assumption: POSIX thread routines are instrumented.
void
CbData::postprocess(const Event& event)
{
    // Update callstack
    mCallstack->update(event);

    // Clear event sets
    #if defined(_MPI)
        mLocal->clear();
        mRemote->clear();
        mInvLocal->clear();
        mInvRemote->clear();
    #endif    // _MPI

    // Clear event-specific info
    mIsSyncpoint = false;
    mIdle        = 0.0;
    mCompletion  = 0.0;

    // Clear delay analysis info
    mRwait  = 0;
    mPrevSp = Event();
    mDelayInfo.mMap.clear();
    mDelayInfo.mSum = 0.0;
    mWaitInfo.mMap.clear();
    mWaitInfo.mSum = 0.0;
    mPropSpMap.clear();
    mSumScales.clear();
    mMaxScales.clear();
}
