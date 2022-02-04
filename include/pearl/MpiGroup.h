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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiGroup.
 *
 *  This header file provides the declaration of the class MpiGroup.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_MPIGROUP_H
#define PEARL_MPIGROUP_H


#include <mpi.h>

#include <pearl/CommSet.h>


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiGroup
 *  @ingroup PEARL_mpi
 *  @brief   Grouping set definition representation for a multi-process
 *           communication set.
 *
 *  The MpiGroup class provides a specialized communication set definition
 *  representation for multi-process communication sets.  In addition to
 *  the functionality of the base class CommSet, the MpiGroup class provides
 *  access to an MPI group handle resembling the communication set used in
 *  the target application.
 **/
/*-------------------------------------------------------------------------*/

class MpiGroup
    : public pearl::CommSet
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new instance with the given global identifier @p id,
        /// communication set @p name, @p properties, the associated
        /// communication location set @p commLocationSet, and a @p ranks
        /// vector providing a lost of global ranks defining the subset.
        ///
        /// @note
        ///     @p ranks should be empty if the GroupingSet::PROPERTY_WORLD
        ///     property flag is set.
        ///
        /// @param id
        ///     Global grouping set identifier
        /// @param name
        ///     Communication set group name
        /// @param properties
        ///     Property flags
        /// @param commLocationSet
        ///     Associated communication location set
        /// @param ranks
        ///     List of ranks (i.e., indices in @p commLocationSet) defining
        ///     the subset
        ///
        /// @exception_strong
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        MpiGroup(IdType                         id,
                 const String&                  name,
                 const Properties&              properties,
                 const CommLocationSet&         commLocationSet,
                 const std::vector< uint32_t >& ranks);

        /// @brief Destructor.
        ///
        /// Destroys the communication set instance.
        ///
        /// @exception_nothrow
        ///     However, this call may abort if the underlying MPI calls fail
        ///     (MPI errors are fatal).
        ///
        virtual
        ~MpiGroup();

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Get MPI group handle.
        ///
        /// Returns the MPI group handle associated with this communication
        /// set object.  The MPI group is lazily created on first access and
        /// then cached until the instance is destructed.
        ///
        /// @returns
        ///     MPI group handle
        ///
        /// @exception_strong
        ///     However, this call may abort if the underlying MPI calls fail
        ///     (MPI errors are fatal).
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        MPI_Group
        getHandle();

        /// @brief Create MPI group handle.
        ///
        /// Returns an MPI group handle corresponding to this communication set
        /// object without caching it.
        ///
        /// @returns
        ///     MPI group handle
        ///
        /// @exception_strong
        ///     However, this call may abort if the underlying MPI calls fail
        ///     (MPI errors are fatal).
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        MPI_Group
        createHandle() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Corresponding MPI group handle
        MPI_Group mHandle;
};
}    // namespace pearl


#endif    // !PEARL_MPIGROUP_H
