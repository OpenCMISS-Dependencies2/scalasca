/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Region.
 *
 *  This header file provides the declaration of the class Region and
 *  related functions.
 *
 *  @todo Review/clean-up/document predicate functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_REGION_H
#define PEARL_REGION_H


#include <stdint.h>

#include <iosfwd>
#include <string>

#include <pearl/Paradigm.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   Region
 *  @ingroup PEARL_base
 *  @brief   Representation of a region definition.
 *
 *  Instances of the Region class provide information about source-code
 *  regions of the target application. Regions are characterized by their
 *  canonical name (typically the linker decorated, i.e., mangled name),
 *  their display name (i.e., the demangled name if available), a file in
 *  which this region is defined, begin and end line numbers, as well as a
 *  region role and a paradigm. In addition, a free-form textual description
 *  can be provided. The numerical identifiers of the individual regions are
 *  globally defined and continuously enumerated, i.e., the ID is element of
 *  [0,@#regions-1].
 **/
/*-------------------------------------------------------------------------*/

class Region
{
    public:
        // --- Type definitions ----------------------------

        /// %Region definition identifier type.
        typedef uint32_t IdType;

        /// Symbolic names for region classification.
        enum
        {
            // Region classes
            cCLASS_INTERNAL = (0 << 24),
            cCLASS_USER     = (1 << 24),
            cCLASS_MPI      = (2 << 24),
            cCLASS_OMP      = (4 << 24),
            cCLASS_ARMCI    = (8 << 24),
            cCLASS_PTHREAD  = (16 << 24),

            // Categories for class MPI
            cCAT_MPI_SETUP      = (1 << 16),
            cCAT_MPI_COLLECTIVE = (2 << 16),
            cCAT_MPI_P2P        = (4 << 16),
            cCAT_MPI_RMA        = (8 << 16),
            cCAT_MPI_IO         = (16 << 16),

            // Categories for class OMP
            cCAT_OMP_PARALLEL = (1 << 16),
            cCAT_OMP_SYNC     = (2 << 16),

            // Categories for class ARMCI
            cCAT_ARMCI_COMM = (1 << 16),
            cCAT_ARMCI_SYNC = (2 << 16),

            // Categories for class PTHREAD
            cCAT_PTHREAD_MGMT = (1 << 16),
            cCAT_PTHREAD_SYNC = (2 << 16),

            // Call types for category OMP_SYNC
            cTYPE_OMP_BARRIER = (1 << 8),

            // Modes for call types OMP_BARRIER
            cMODE_OMP_IMPLICIT = (1 << 0),
            cMODE_OMP_EXPLICIT = (2 << 0),

            // Call types for category PTHREAD_SYNC
            cTYPE_PTHREAD_MUTEX     = (1 << 8),
            cTYPE_PTHREAD_SPIN      = (2 << 8),
            cTYPE_PTHREAD_CONDITION = (4 << 8),

            // Modes for call types PTHREAD_MUTEX/PTHREAD_SPIN
            cMODE_PTHREAD_LOCK    = (1 << 0),
            cMODE_PTHREAD_TRYLOCK = (2 << 0),
            cMODE_PTHREAD_UNLOCK  = (4 << 0),

            // Modes for call types PTHREAD_CONDITION
            cMODE_PTHREAD_WAIT      = (1 << 0),
            cMODE_PTHREAD_TIMEDWAIT = (2 << 0),

            // Call types for category MPI_SETUP
            cTYPE_MPI_INIT     = (1 << 8),
            cTYPE_MPI_FINALIZE = (2 << 8),

            // Call types for category MPI_COLLECTIVE
            cTYPE_MPI_BARRIER  = (1 << 8),
            cTYPE_MPI_ONE_TO_N = (2 << 8),
            cTYPE_MPI_N_TO_ONE = (4 << 8),
            cTYPE_MPI_N_TO_N   = (8 << 8),
            cTYPE_MPI_PARTIAL  = (16 << 8),

            // Call types for category MPI_P2P
            cTYPE_MPI_SEND     = (1 << 8),
            cTYPE_MPI_RECV     = (2 << 8),
            cTYPE_MPI_SENDRECV = (3 << 8),
            cTYPE_MPI_TEST     = (4 << 8),
            cTYPE_MPI_WAIT     = (8 << 8),

            // Call types for category MPI_RMA
            cTYPE_MPI_RMA_COMM    = (1 << 8),
            cTYPE_MPI_RMA_COLL    = (2 << 8),
            cTYPE_MPI_RMA_GATS    = (4 << 8),
            cTYPE_MPI_RMA_PASSIVE = (8 << 8),

            // Cal types for category ARMCI
            cTYPE_ARMCI_INIT     = (1 << 8),
            cTYPE_ARMCI_FINALIZE = (2 << 8),
            cTYPE_ARMCI_ALLFENCE = (4 << 8),
            cTYPE_ARMCI_BARRIER  = (8 << 8),

            // Transfer modes for call types MPI_SEND/MPI_RECV
            cMODE_MPI_STANDARD    = (1 << 0),
            cMODE_MPI_SYNCHRONOUS = (2 << 0),
            cMODE_MPI_BUFFERED    = (4 << 0),
            cMODE_MPI_READY       = (8 << 0),
            cMODE_MPI_NONBLOCKING = (16 << 0),
            cMODE_MPI_PERSISTENT  = (32 << 0),

            // Testing/waiting modes for call types MPI_TEST/MPI_WAIT/MPI_START
            cMODE_MPI_SINGLE = (1 << 0),
            cMODE_MPI_MANY   = (2 << 0),

            // RMA communication modes
            cMODE_RMA_PUT = (1 << 0),
            cMODE_RMA_GET = (2 << 0),

            cMODE_RMA_START    = (1 << 0),
            cMODE_RMA_COMPLETE = (2 << 0),
            cMODE_RMA_POST     = (4 << 0),
            cMODE_RMA_WAIT     = (8 << 0),
            cMODE_RMA_TEST     = (16 << 0),

            cMODE_RMA_FENCE      = (1 << 0),
            cMODE_RMA_WIN_CREATE = (2 << 0),
            cMODE_RMA_WIN_FREE   = (4 << 0),

            cMODE_RMA_LOCK   = (1 << 0),
            cMODE_RMA_UNLOCK = (2 << 0)
        };


        /// @brief %Region roles.
        ///
        /// Every region has a role field associated to it. The role encodes
        /// more specific information about the region, in particular to which
        /// kind of language construct (e.g., function, loop, OpenMP construct)
        /// the region refers to.
        ///
        enum Role
        {
            ROLE_UNKNOWN,    ///< Unknown role, used only if no other role fits

            ROLE_FUNCTION, ///< Function or subroutine
            ROLE_WRAPPER,  ///< Wrapped API function
            ROLE_LOOP,     ///< Loop construct
            ROLE_CODE,     ///< Arbitrary user-defined code block

            ROLE_PARALLEL,         ///< OpenMP "parallel" construct (structured block)
            ROLE_SECTIONS,         ///< OpenMP "sections" construct
            ROLE_SECTION,          ///< Individual "section" inside an OpenMP "sections" construct
            ROLE_SINGLE,           ///< OpenMP "single" construct
            ROLE_SINGLE_SBLOCK,    ///< OpenMP "single" construct (structured block)
            ROLE_WORKSHARE,        ///< OpenMP "workshare" construct
            ROLE_MASTER,           ///< OpenMP "master" construct
            ROLE_CRITICAL,         ///< OpenMP "critical" construct
            ROLE_CRITICAL_SBLOCK,  ///< OpenMP "critical" construct (structured block)
            ROLE_BARRIER,          ///< Explicit barrier (e.g., MPI or OpenMP)
            ROLE_IMPLICIT_BARRIER, ///< Implicit barrier as part of various OpenMP constructs
            ROLE_ATOMIC,           ///< OpenMP "atomic" construct
            ROLE_FLUSH,            ///< OpenMP "flush" construct
            ROLE_ORDERED,          ///< OpenMP "ordered" construct
            ROLE_ORDERED_SBLOCK,   ///< OpenMP "ordered construct (structured block)
            ROLE_TASK,             ///< OpenMP "task" construct (structured block)
            ROLE_TASK_CREATE,      ///< OpenMP "task" construct (creation)
            ROLE_TASK_WAIT,        ///< OpenMP "taskwait" construct

            ROLE_COLL_ONE2ALL,  ///< Collective 1:N communication operation (e.g., MPI_Bcast)
            ROLE_COLL_ALL2ONE,  ///< Collective N:1 communication operation (e.g., MPI_Gather)
            ROLE_COLL_ALL2ALL,  ///< Collective N:N communication operation (e.g., MPI_Allreduce)
            ROLE_COLL_OTHER,    ///< Collective M:N communication operation (e.g., MPI_Scan)

            ROLE_FILE_IO,       ///< File I/O operation
            ROLE_POINT2POINT,   ///< Point-to-point communication operation
            ROLE_RMA,           ///< Remote memory access communication operation
            ROLE_DATA_TRANSFER, ///< Memory data transfer operation

            ROLE_ARTIFICIAL,    ///< Artificial region (used for measurement-related activities).

            ROLE_THREAD_CREATE,  ///< Thread creation in create-wait paradigm
            ROLE_THREAD_WAIT,    ///< Thread wait in create-wait paradigm

            ROLE_TASK_UNTIED,    ///< OpenMP "untied task" construct (structured block)

            ROLE_ALLOCATE,    ///< Memory allocation operation
            ROLE_DEALLOCATE,  ///< Memory deallocation operation
            ROLE_REALLOCATE,  ///< Memory reallocation operation

            NUMBER_OF_ROLES    ///< Not a valid role, for internal use only
        };


        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined region definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class Region representing undefined region
        /// entries in other definitions.
        static const Region UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new region definition instance and initializes it with
        /// the given values.
        ///
        /// @param  id             Global region identifier
        /// @param  canonicalName  %Region name (typically mangled)
        /// @param  displayName    %Region name (demangled if available,
        ///                        otherwise identical to @a canonicalName)
        /// @param  description    Free-form region description (optional,
        ///                        String::UNDEFINED otherwise)
        /// @param  role           %Region role
        /// @param  paradigm       %Region paradigm
        /// @param  filename       Name of source file in which the region is
        ///                        defined (String::UNDEFINED if unknown)
        /// @param  startLine      Line number where the region starts
        ///                        (zero if unknown)
        /// @param  endLine        Line number where the region ends
        ///                        (zero if unknown)
        ///
        Region(IdType        id,
               const String& canonicalName,
               const String& displayName,
               const String& description,
               Role          role,
               Paradigm      paradigm,
               const String& filename,
               uint32_t      startLine,
               uint32_t      endLine);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the region definition.
        ///
        /// @return Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get canonical region name.
        ///
        /// Returns the string definition storing the canonical region name.
        /// For compiler-instrumented functions, this is typically the mangled
        /// (i.e., linker-decorated) name. For other regions, it can be an
        /// arbitrary character string.
        ///
        /// @return Canonical region name
        ///
        const String&
        getCanonicalName() const;

        /// @brief Get display region name.
        ///
        /// Returns the string definition storing the region's display name.
        /// For compiler-instrumented functions, this corresponds to the
        /// demangled name if available, otherwise it is identical to the
        /// canonical name. For other regions, it can be an arbitrary character
        /// string.
        ///
        /// @return %Region name for display
        ///
        const String&
        getDisplayName() const;

        /// @brief Get region description.
        ///
        /// Returns the string definition storing the free-form region
        /// description associated with the source-code region. If this
        /// information is not available, String::UNDEFINED is returned.
        ///
        /// @return %Region description
        ///
        const String&
        getDescription() const;

        /// @brief Get region role.
        ///
        /// Returns the region role, encoding more detailed information. See
        /// the documentation of Region::Role for details.
        ///
        /// @return %Region role
        ///
        Role
        getRole() const;

        /// @brief Get region paradigm.
        ///
        /// Returns the region paradigm, encoding more detailed information on
        /// the programming model or how the region got generated, respectively.
        ///
        /// @return %Region paradigm
        ///
        Paradigm
        getParadigm() const;

        /// @brief Get filename.
        ///
        /// Returns the string definition storing the name of the file in
        /// which the region is defined. If this information is not available,
        /// String::UNDEFINED is returned.
        ///
        /// @return Filename
        ///
        const String&
        getFilename() const;

        /// @brief Get start line number.
        ///
        /// Returns the line number where the region starts in the associated
        /// source file (zero if this information is not available).
        ///
        /// @return Start line number
        ///
        uint32_t
        getStartLine() const;

        /// @brief Get end line number.
        ///
        /// Returns the line number where the region ends in the associated
        /// source file (zero if this information is not available).
        ///
        /// @return End line number
        ///
        uint32_t
        getEndLine() const;

        /// @brief Get region classification.
        ///
        /// Returns a 32-bit unsigned integer value providing an encoded
        /// classification of the region. This member functions and its
        /// return value is not intended to be used directly. Instead, the
        /// region predicate functions should be called from client code.
        ///
        /// @return %Region classification
        ///
        uint32_t
        getClassification() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// %Region name (mangled)
        const String& mCanonicalName;

        /// %Region name (demangled)
        const String& mDisplayName;

        /// Description of the source-code region
        const String& mDescription;

        /// %Region role
        Role mRole;

        /// %Region paradigm
        Paradigm mParadigm;

        /// Name of file in which the region is defined
        const String& mFilename;

        /// Line number where the region starts
        uint32_t mStartLine;

        /// Line number where the region ends
        uint32_t mEndLine;

        /// %Region classification
        uint32_t mClass;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates Region
///
/// Prints the contents of the given region definition object @a item to an
/// output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    %Region definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream& stream,
           const Region& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates Region
///
/// Compares two region definition objects @a lhs and @a rhs for equality. Two
/// region definitions are considered equal if all of their attributes are
/// identical.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the region definitions are equal, false otherwise.
///
bool
operator==(const Region& lhs,
           const Region& rhs);

/// @brief   Inequality operator.
/// @relates Region
///
/// Compares two region definition objects @a lhs and @a rhs for inequality.
/// Two region definitions are considered unequal if any of their attributes
/// differ.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the region definitions differ, false otherwise.
///
bool
operator!=(const Region& lhs,
           const Region& rhs);

/// @}
/// @name Conversion functions
/// @{

/// @brief Get string representation of a region role.
///
/// Returns a human-readable string representation of the given region
/// @a role.
///
/// @param  role  %Region role
///
/// @return Corresponding string representation
///
const std::string
toString(const Region::Role role);

/// @}
/// @name Region predicates
/// @{

bool
is_omp_api(const pearl::Region& region);

bool
is_omp_parallel(const pearl::Region& region);

bool
is_omp_barrier(const pearl::Region& region);

bool
is_omp_ebarrier(const pearl::Region& region);

bool
is_omp_ibarrier(const pearl::Region& region);

bool
is_pthread_api(const pearl::Region& region);

bool
is_pthread_mutex_lock(const pearl::Region& region);

bool
is_pthread_mutex_trylock(const pearl::Region& region);

bool
is_pthread_spin_lock(const pearl::Region& region);

bool
is_pthread_condition(const pearl::Region& region);

bool
is_mpi_api(const pearl::Region& region);

bool
is_mpi_init(const pearl::Region& region);

bool
is_mpi_finalize(const pearl::Region& region);

bool
is_mpi_collective(const pearl::Region& region);

bool
is_mpi_barrier(const pearl::Region& region);

bool
is_mpi_12n(const pearl::Region& region);

bool
is_mpi_n21(const pearl::Region& region);

bool
is_mpi_n2n(const pearl::Region& region);

bool
is_mpi_scan(const pearl::Region& region);

bool
is_mpi_block_send(const pearl::Region& region);

bool
is_mpi_testx(const pearl::Region& region);

bool
is_mpi_wait_single(const pearl::Region& region);

bool
is_mpi_wait_multi(const pearl::Region& region);

bool
is_mpi_rma_start(const pearl::Region& region);

bool
is_mpi_rma_complete(const pearl::Region& region);

bool
is_mpi_rma_post(const pearl::Region& region);

bool
is_mpi_rma_wait(const pearl::Region& region);

bool
is_mpi_rma_collective(const pearl::Region& region);

bool
is_mpi_rma_fence(const pearl::Region& region);

/// @}


// --- Inline methods & related functions -----------------------------------

#include "Inline/Region-inl.h"
}    // namespace pearl


#endif    // !PEARL_REGION_H
