/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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


#ifndef PEARL_PROCESS_H
#define PEARL_PROCESS_H


#include <pearl/LocationGroup.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Process.
 *
 *  This header file provides the declaration of the class Process and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   Process
 *  @ingroup PEARL_base
 *  @brief   Representation of a process definition.
 *
 *  Instances of the Process class provide information about UNIX/MPI process
 *  definitions. As concrete implementation of the LocationGroup interface,
 *  process definitions are characterized by an ID, a name, and a parent node
 *  (of type SystemNode) in the system hierarchy, and provide a list of
 *  associated locations. Furthermore, the Process class has the ability to
 *  store the MPI rank of the process.
 **/
/*-------------------------------------------------------------------------*/

class Process
    : public LocationGroup
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new process location group definition instance and
        /// initializes it with the given values.
        ///
        /// @param  id      Global location group identifier
        /// @param  name    Group name
        /// @param  parent  Parent node in system tree (non-NULL)
        ///
        Process(IdType        id,
                const String& name,
                SystemNode*   parent);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get location group type.
        ///
        /// Returns the location group type, encoding more detailed information.
        /// See the documentation of LocationGroup::Type for details. This
        /// subclass will always return LocationGroup::TYPE_PROCESS.
        ///
        /// @return %Location group type
        ///
        virtual Type
        getType() const;

        /// @brief Get process rank.
        ///
        /// Returns the MPI rank of the process. If this information is not
        /// available, -1 is returned.
        ///
        /// @return MPI rank
        ///
        int
        getRank() const;

        /// @}
        /// @name Set definition data
        /// @{

        /// @brief Set process rank.
        ///
        /// Sets the MPI rank of the process. As MPI ranks do not change over
        /// time, this member function can only be called once to initialize
        /// the definition data.
        ///
        /// @param  rank  MPI process rank (@>= 0)
        ///
        void
        setRank(int rank);

        /// @}


    private:
        // --- Data members --------------------------------

        /// MPI process rank
        int mRank;


        // --- Private methods & friends -------------------

        virtual std::ostream&
        output(std::ostream& stream) const;
};
}    // namespace pearl


#endif    // !PEARL_PROCESS_H
