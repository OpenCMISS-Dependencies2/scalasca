/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2006                                                     **
**  TU Dresden, Zentrum fuer Informationsdienste und Hochleistungsrechnen  **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_LOCALTRACE_H
#define PEARL_LOCALTRACE_H


#include <iterator>
#include <vector>

#include <pearl/Event.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    LocalTrace.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class LocalTrace.
 *
 *  This header file provides the declaration of the class LocalTrace.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Event_rep;
class GlobalDefs;
class Location;


/*-------------------------------------------------------------------------*/
/**
 *  @class   LocalTrace
 *  @ingroup PEARL_base
 *  @brief   Container class for local event traces.
 **/
/*-------------------------------------------------------------------------*/

class LocalTrace
{
    public:
        /// Container type for event representations
        typedef std::vector< Event_rep* > container_type;

        /// Iterator type for event container
        typedef Event iterator;

        /// Reverse iterator type for event container
        #ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
            typedef std::reverse_iterator< iterator > reverse_iterator;
        #else
            // Workaround for Sun Studio compilers using RogueWave STL
            typedef std::reverse_iterator< iterator,
                                           std::bidirectional_iterator_tag,
                                           Event > reverse_iterator;
        #endif

        /// @name Constructors & destructor
        /// @{

        ~LocalTrace();

        /// @}
        /// @name Iterator handling
        /// @{

        iterator
        begin() const;

        iterator
        end() const;

        iterator
        at(uint32_t index) const;

        reverse_iterator
        rbegin() const;

        reverse_iterator
        rend() const;

        /// @}
        /// @name Get trace information
        /// @{

        uint32_t
        size() const;

        uint32_t
        num_events() const;

        uint32_t
        num_omp_parallel() const;

        uint32_t
        num_mpi_regions() const;

        const Location&
        get_location() const;

        const GlobalDefs&
        get_definitions() const;

        /// @}
        /// @name Store new events
        /// @{

        void
        add_event(Event_rep* event);

        /// @}
        /// @name Inserting & removing elements
        /// @{

        iterator
        remove_if(bool (* predicate)(const Event_rep* event));

        iterator
        insert(iterator   pos,
               Event_rep* event);

        iterator
        erase(iterator pos);

        iterator
        erase(iterator begin,
              iterator end);

        iterator
        replace(iterator   pos,
                Event_rep* event);

        iterator
        swap(const iterator& a,
             const iterator& b);

        /// @}


    private:
        /// Associated global definitions
        const GlobalDefs& m_defs;

        /// Local event trace
        container_type m_events;

        /// Number of OpenMP parallel regions
        uint32_t m_omp_parallel;

        /// Number of MPI regions
        uint32_t m_mpi_regions;

        /// Trace location
        Location* m_location;


        /* Private methods */
        LocalTrace(const GlobalDefs& defs,
                   const Location&   location);

        Event_rep*
        operator[](uint32_t index) const;


        /* Declare friends */
        friend class Event;
        friend class TraceArchive;
};
}    // namespace pearl


#endif    // !PEARL_LOCALTRACE_H
