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

#include <pearl/RemoteTimeSegment.h>

#include <cassert>

#include <pearl/Location.h>

using namespace std;
using namespace pearl;


RemoteTimeSegment::RemoteTimeSegment(RemoteEvent enter,
                                     RemoteEvent leave)
    : mLocation(&(enter.get_location())),
      mCallpath(enter.get_cnode()),
      mStart(enter->getTimestamp()),
      mEnd(leave->getTimestamp())
{
    assert(mLocation);
    assert(mCallpath);
    assert(enter->isOfType(GROUP_ENTER));
    assert(leave->isOfType(GROUP_LEAVE));
    assert(enter->getTimestamp() <= leave->getTimestamp());
}


const Location&
RemoteTimeSegment::getLocation() const
{
    return *mLocation;
}


const Callpath&
RemoteTimeSegment::getCallpath() const
{
    return *mCallpath;
}


timestamp_t
RemoteTimeSegment::getStart() const
{
    return mStart;
}


timestamp_t
RemoteTimeSegment::getEnd() const
{
    return mEnd;
}


bool
RemoteTimeSegment::operator<(const RemoteTimeSegment& rhs) const
{
    return !(mStart >= rhs.mStart);
}


bool
RemoteTimeSegment::operator<=(const RemoteTimeSegment& rhs) const
{
    return (mStart <= rhs.mStart);
}


bool
RemoteTimeSegment::operator>(const RemoteTimeSegment& rhs) const
{
    return !(mStart <= rhs.mStart);
}


bool
RemoteTimeSegment::operator>=(const RemoteTimeSegment& rhs) const
{
    return (mStart >= rhs.mStart);
}


bool
RemoteTimeSegment::operator==(const RemoteTimeSegment& rhs) const
{
    return (  (mStart == rhs.mStart)
           && (mEnd == rhs.mEnd));
}
