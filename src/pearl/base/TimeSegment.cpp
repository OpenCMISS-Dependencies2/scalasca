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

#include <pearl/TimeSegment.h>

#include <cassert>

using namespace std;
using namespace pearl;


TimeSegment::TimeSegment(Event enter,
                         Event leave)
    : mCallpath(enter.get_cnode()),
      mStart(enter->getTimestamp()),
      mEnd(leave->getTimestamp())
{
    assert(mCallpath);
    assert(enter->isOfType(GROUP_ENTER));
    assert(leave->isOfType(GROUP_LEAVE));
    assert(enter->getTimestamp() <= leave->getTimestamp());
}


const Callpath&
TimeSegment::getCallpath() const
{
    return *mCallpath;
}


timestamp_t
TimeSegment::getStart() const
{
    return mStart;
}


timestamp_t
TimeSegment::getEnd() const
{
    return mEnd;
}


timestamp_t
TimeSegment::getDuration() const
{
    return (mEnd - mStart);
}


bool
TimeSegment::operator<(const pearl::TimeSegment& rhs) const
{
    return !(*this >= rhs);
}


bool
TimeSegment::operator<=(const pearl::TimeSegment& rhs) const
{
    return (mStart <= rhs.mStart);
}


bool
TimeSegment::operator>(const pearl::TimeSegment& rhs) const
{
    return !(*this <= rhs);
}


bool
TimeSegment::operator>=(const pearl::TimeSegment& rhs) const
{
    return (mStart >= rhs.mStart);
}


bool
TimeSegment::operator==(const pearl::TimeSegment& rhs) const
{
    return (  (mStart == rhs.mStart)
           && (mEnd == rhs.mEnd));
}
