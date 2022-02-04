/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2019                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief   Implementation of functions related to the TimeRank struct.
 *
 *  This header file provides the implementation of functions related to
 *  the TimeRank struct.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "TimeRank.h"

#include <iostream>

#include <pearl/CommSet.h>

using namespace std;
using namespace pearl;
using namespace scout;


// --- Related functions ----------------------------------------------------

ostream&
scout::operator<<(ostream&        stream,
                  const TimeRank& item)
{
    return stream << '(' << item.mTime << ", " << item.mRank << ')';
}


TimeRank
scout::globalize(const TimeRank&     item,
                 const Communicator& comm)
{
    return TimeRank(item.mTime,
                    comm.getCommSet().getGlobalRank(item.mRank));
}
