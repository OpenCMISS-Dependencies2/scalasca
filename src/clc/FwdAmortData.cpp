/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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


#include <config.h>

#include "FwdAmortData.h"

#include "Synchronizer.h"


FwdAmortData::FwdAmortData(Synchronizer* synchronizer)
    : mSynchronizer(synchronizer),
      mIsAmortized(false)
{
}


void
FwdAmortData::setAmortized()
{
    mIsAmortized = true;
}


void
FwdAmortData::preprocess(const pearl::Event& event)
{
    mIsAmortized = false;
}


void
FwdAmortData::postprocess(const pearl::Event& event)
{
    if (!mIsAmortized)
    {
        mSynchronizer->fa_amortize_intern(event, this);
    }
}
