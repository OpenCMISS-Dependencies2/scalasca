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


#ifndef PEARL_REMOTETIMESEGMENT_H
#define PEARL_REMOTETIMESEGMENT_H


#include <pearl/RemoteEvent.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    RemoteTimeSegment.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class RemoteTimeSegment.
 *
 * This header files provides the declaration of the class
 * RemoteTimeSegment.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 * @class   RemoteTimeSegment
 * @ingroup PEARL_base
 * @brief   Auxiliary class to ease comparision between remote time segments.
 */
/*-------------------------------------------------------------------------*/

class RemoteTimeSegment
{
    public:
        /// @name Constructors & destructors
        /// @{

        RemoteTimeSegment(RemoteEvent enter,
                          RemoteEvent leave);

        /// @}
        /// @name Information interface
        /// @{

        const Location&
        getLocation() const;

        const Callpath&
        getCallpath() const;

        timestamp_t
        getStart() const;

        timestamp_t
        getEnd() const;

        /// @}
        /// @name Comparison operators
        /// @{

        bool
        operator<(const RemoteTimeSegment& rhs) const;

        bool
        operator<=(const RemoteTimeSegment& rhs) const;

        bool
        operator>(const RemoteTimeSegment& rhs) const;

        bool
        operator>=(const RemoteTimeSegment& rhs) const;

        bool
        operator==(const RemoteTimeSegment& rhs) const;

        /// @}


    private:
        /// Pointer to location object for the time segement
        const Location* mLocation;

        /// Pointer to callpath object of the time segment
        const Callpath* mCallpath;

        /// Start time of the time segment
        timestamp_t mStart;

        /// End time fo the time segment
        timestamp_t mEnd;
};
}    // namespace pearl


#endif    // !PEARL_REMOTETIMESEGMENT_H
