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


#ifndef PEARL_EVENT_REP_H
    #error "Do not include this file!  Use <pearl/Event_rep.h> instead."
#endif


// --- Comparison operators (inline) ----------------------------------------

inline bool
Event_rep::operator==(const timestamp_t& rhs) const
{
    return (mTimestamp == rhs);
}


inline bool
Event_rep::operator!=(const timestamp_t& rhs) const
{
    return (mTimestamp != rhs);
}


inline bool
Event_rep::operator<(const timestamp_t& rhs) const
{
    return (mTimestamp < rhs);
}


inline bool
Event_rep::operator>(const timestamp_t& rhs) const
{
    return (mTimestamp > rhs);
}


// --- Access event data (inline) -------------------------------------------

inline timestamp_t
Event_rep::getTimestamp() const
{
    return mTimestamp;
}


// --- Modify event data (inline) -------------------------------------------

inline void
Event_rep::setTimestamp(const timestamp_t timestamp)
{
    mTimestamp = timestamp;
}
