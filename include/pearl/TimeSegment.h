/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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


#ifndef PEARL_TIMESEGMENT_H
#define PEARL_TIMESEGMENT_H


#include <pearl/Event.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    TimeSegment.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class TimeSegment.
 *
 * This header files provides the declaration of the class
 * TimeSegment.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 * @class   TimeSegment
 * @ingroup PEARL_base
 * @brief   Auxiliary class to ease comparision between time segments.
 */
/*-------------------------------------------------------------------------*/

class TimeSegment
{
    public:
        /// @name Constructors & destructors
        /// @{

        TimeSegment(Event enter,
                    Event leave);

        /// @}
        /// @name Information interface
        /// @{

        const Callpath&
        getCallpath() const;

        timestamp_t
        getStart() const;

        timestamp_t
        getEnd() const;

        timestamp_t
        getDuration() const;

        /// @}
        /// @name Comparison operators
        /// @{

        bool
        operator<(const TimeSegment& rhs) const;

        bool
        operator<=(const TimeSegment& rhs) const;

        bool
        operator>(const TimeSegment& rhs) const;

        bool
        operator>=(const TimeSegment& rhs) const;

        bool
        operator==(const TimeSegment& rhs) const;

        /// @}


    private:
        /// Pointer to callpath object of the timesegment
        Callpath* mCallpath;

        /// Start time of the time segment
        timestamp_t mStart;

        /// End time fo the time segment
        timestamp_t mEnd;
};
}    // namespace pearl


#endif    // !PEARL_TIMESEGMENT_H
