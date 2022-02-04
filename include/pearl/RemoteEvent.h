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


#ifndef PEARL_REMOTEEVENT_H
#define PEARL_REMOTEEVENT_H


#include <iosfwd>

#include <pearl/CountedPtr.h>
#include <pearl/Event_rep.h>
#include <pearl/pearl_types.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    RemoteEvent.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class RemoteEvent.
 *
 *  This header file provides the declaration of the class RemoteEvent.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Buffer;
class Callpath;
class GlobalDefs;
class Location;


/*-------------------------------------------------------------------------*/
/**
 *  @class   RemoteEvent
 *  @ingroup PEARL_base
 *  @brief   Generic representation for remote events without iterator
 *           functionality.
 **/
/*-------------------------------------------------------------------------*/

class RemoteEvent
{
    public:
        /// @name Constructors & destructor
        /// @{

        RemoteEvent(const GlobalDefs& defs,
                    Buffer&           buffer);

        /// @}
        /// @name Access remote event information
        /// @{

        Callpath*
        get_cnode() const;

        const Location&
        get_location() const;

        uint32_t
        get_remote_index() const;

        /// @}
        /// @name Access event representation
        /// @{

        Event_rep&
        operator*() const;

        Event_rep*
        operator->() const;

        /// @}


    private:
        /// Reference-counted event representation
        CountedPtr< Event_rep > m_event;

        /// %Event location
        Location* m_location;

        /// Associated call tree node
        Callpath* m_cnode;

        /// %Event index in remote trace
        uint32_t m_remote_index;


        /* Private methods */
        std::ostream&
        output(std::ostream& stream) const;


        /* Declare friends */
        friend std::ostream&
        operator<<(std::ostream&      stream,
                   const RemoteEvent& event);
};


// --- Related functions ----------------------------------------------------

std::ostream&
operator<<(std::ostream&      stream,
           const RemoteEvent& event);
}    // namespace pearl


#endif    // !PEARL_REMOTEEVENT_H
