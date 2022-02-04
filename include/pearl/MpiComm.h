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
 *  @brief   Declaration of the class MpiComm.
 *
 *  This header file provides the declaration of the class MpiComm.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_MPICOMM_H
#define PEARL_MPICOMM_H


#include <mpi.h>

#include <pearl/Communicator.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class MpiGroup;


/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiComm
 *  @ingroup PEARL_mpi
 *  @brief   %Communicator definition representation for multi-process
 *           communicators.
 *
 *  The MpiComm class provides a specialized communicator definition
 *  representation for multi-process communicators.  In addition to the
 *  functionality provided by the base class Communicator, the MpiComm
 *  class provides access to an MPI communicator handle resembling the
 *  communication context used in the target application.
 **/
/*-------------------------------------------------------------------------*/

class MpiComm
    : public pearl::Communicator
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new instance with the given global identifier @p id,
        /// communicator @p name, associated multi-process communication set
        /// @p commSet, and a @p parent communicator.
        ///
        /// @param id
        ///     Global communicator identifier
        /// @param name
        ///     Communicator name
        /// @param group
        ///     Associated multi-process communication set (MPI group)
        /// @param parent
        ///     Parent communicator (`nullptr` for root communicators or when
        ///     this information is not available)
        ///
        /// @exception_strong
        ///     However, this call may abort if the underlying MPI calls fail
        ///     (MPI errors are fatal).
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        MpiComm(IdType          id,
                const String&   name,
                const MpiGroup& group,
                const MpiComm*  parent);

        /// @brief Destructor.
        ///
        /// Destroys the communicator instance.
        ///
        /// @exception_nothrow
        ///     However, this call may abort if the underlying MPI calls fail
        ///     (MPI errors are fatal).
        ///
        virtual
        ~MpiComm();

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Get MPI communicator handle.
        ///
        /// Returns the MPI communicator handle associated with this
        /// communicator object.
        ///
        /// @returns
        ///     MPI communicator handle
        ///
        /// @exception_nothrow
        ///
        MPI_Comm
        getHandle() const;

        /// @}
        /// @name Communicator duplication
        /// @{

        /// @brief Create copy.
        ///
        /// Returns a pointer to a copy of the instance (including a copy of
        /// the MPI communicator handle).
        ///
        /// @returns
        ///     Copy of the instance
        ///
        /// @exception_nothrow
        ///     However, this call may abort if the underlying MPI calls fail
        ///     (MPI errors are fatal).
        ///
        MpiComm*
        duplicate() const;

        /// @}


    protected:
        // --- Protected methods ---------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Copy constructor.
        ///
        /// Copies the given instance and duplicates the associated MPI
        /// communicator handle using `MPI_Comm_dup`.  It is 'protected' to
        /// prevent accidental invocation; users need to call
        /// MpiComm::duplicate() explicitly to create a communicator copy.
        ///
        /// @param comm
        ///     Multi-process communicator definition to duplicate
        ///
        /// @exception_nothrow
        ///     However, this call may abort if the underlying MPI calls fail
        ///     (MPI errors are fatal).
        ///
        MpiComm(const MpiComm& comm);

        /// @}


    private:
        // --- Data members --------------------------------

        /// Corresponding MPI communicator handle
        MPI_Comm mHandle;
};
}    // namespace pearl


#endif    // !PEARL_MPICOMM_H
