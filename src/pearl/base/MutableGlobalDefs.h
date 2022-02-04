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
 *  @brief   Declaration of the class MutableGlobalDefs.
 *
 *  This header file provides the declaration of the class MutableGlobalDefs.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_MUTABLEGLOBALDEFS_H
#define PEARL_MUTABLEGLOBALDEFS_H


#include <vector>

#include <pearl/GlobalDefs.h>
#include <pearl/Paradigm.h>
#include <pearl/pearl_types.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class CommLocationSet;


namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MutableGlobalDefs
 *  @ingroup PEARL_base
 **/
/*-------------------------------------------------------------------------*/

class MutableGlobalDefs
    : public pearl::GlobalDefs
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Default constructor.
        ///
        /// Constructs a new, empty instance.
        ///
        /// @exception_strong
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate storage space
        ///
        MutableGlobalDefs();

        /// @}
        /// @name Insertion functions for global definitions
        /// @{

        /// @brief Add %CallingContext definition.
        ///
        /// Adds the given CallingContext definition object @p item to the
        /// global definitions container, with transfer of ownership from the
        /// caller to the container.
        ///
        /// @param item
        ///     Pointer to the CallingContext definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addCallingContext(CallingContext* item);

        /// @brief Add %Callpath definition.
        ///
        /// Adds the given Callpath definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// @param item
        ///     Pointer to the Callpath definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addCallpath(Callpath* item);

        /// @brief Add %Callsite definition.
        ///
        /// Adds the given Callsite definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// @param item
        ///     Pointer to the Callsite definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addCallsite(Callsite* item);

        /// @brief Add Cartesian topology dimension definition.
        ///
        /// Adds the given Cartesian grid topology dimension definition object
        /// @p item to the global definitions container, with transfer of
        /// ownership from the caller to the container.
        ///
        /// @param item
        ///     Pointer to the Cartesian grid topology dimension definition
        ///     object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addCartDimension(CartDimension* item);

        /// @brief Add Cartesian topology definition.
        ///
        /// Adds the given Cartesian grid topology definition object @p item
        /// to the global definitions container, with transfer of ownership
        /// from the caller to the container.
        ///
        /// @param item
        ///     Pointer to the Cartesian grid topology definition object to
        ///     be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addCartTopology(CartTopology* item);

        /// @brief Add %Communicator definition.
        ///
        /// Adds the given Communicator definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// @param item
        ///     Pointer to the Communicator definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addCommunicator(Communicator* item);

        /// @brief Add %GroupingSet definition.
        ///
        /// Adds the given GroupingSet definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// Note that there can be at most one communication location set
        /// definition (see class CommLocationSet) for each paradigm.  This is
        /// ensured by the validate() member function.
        ///
        /// @param item
        ///     Pointer to the GroupingSet definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addGroupingSet(GroupingSet* item);

        /// @brief Add %Location definition.
        ///
        /// Adds the given Location definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// @param item
        ///     Pointer to the Location definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addLocation(Location* item);

        /// @brief Add %LocationGroup definition.
        ///
        /// Adds the given LocationGroup definition object @p item to the
        /// global definitions container, with transfer of ownership from the
        /// caller to the container.
        ///
        /// @param item
        ///     Pointer to the LocationGroup definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addLocationGroup(LocationGroup* item);

        /// @brief Add %Metric definition.
        ///
        /// Adds the given Metric definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// @param item
        ///     Pointer to the Metric definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addMetric(Metric* item);

        /// @brief Add %Region definition.
        ///
        /// Adds the given Region definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// @param item
        ///     Pointer to the Region definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addRegion(Region* item);

        /// @brief Add %RmaWindow definition.
        ///
        /// Adds the given RmaWindow definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// @param item
        ///     Pointer to the RmaWindow definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addRmaWindow(RmaWindow* item);

        /// @brief Add %SourceLocation definition.
        ///
        /// Adds the given SourceLocation definition object @p item to the
        /// global definitions container, with transfer of ownership from the
        /// caller to the container.
        ///
        /// @param item
        ///     Pointer to the SourceLocation definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addSourceLocation(SourceLocation* item);

        /// @brief Add %String definition.
        ///
        /// Adds the given String definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// @param item
        ///     Pointer to the String definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addString(String* item);

        /// @brief Add %SystemNode definition.
        ///
        /// Adds the given SystemNode definition object @p item to the global
        /// definitions container, with transfer of ownership from the caller
        /// to the container.
        ///
        /// @param item
        ///     Pointer to the SystemNode definition object to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws std::invalid_argument
        ///     on `nullptr` passed as @p item
        /// @throws std::bad_alloc
        ///     on failure to reallocate storage space during expansion
        ///
        void
        addSystemNode(SystemNode* item);

        /// @}
        /// @name Definition amendment functions
        /// @{

        /// @brief Add rank coordinate to Cartesian topology definition.
        ///
        /// Adds the given @p coordinate to the list of Cartesian coordinates
        /// for a particular @p rank in the Cartesian grid topology definition
        /// with the identifier @p cartTopoId.
        ///
        /// @param cartTopoId
        ///     Global Cartesian grid topology identifier
        /// @param rank
        ///     Local rank in associated communicator
        /// @param coordinate
        ///     Cartesian coordinate to be added
        ///
        /// @exception_safety
        ///     If no reallocations happen, the container remains unchanged in
        ///     case of exceptions (strong guarantee).  Otherwise the container
        ///     is guaranteed to end in a valid state (basic guarantee).
        ///
        /// @throws RuntimeError
        ///     on failure to find the Cartesian topology with the given
        ///         @p cartTopoId<br/>
        ///     on communicator &ne; Communicator::UNDEFINED and rank out of
        ///         range<br/>
        ///     on invalid @#dimensions<br/>
        ///     on coordinate value out of range
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        void
        assignCartCoordinate(CartTopology::IdType  cartTopoId,
                             uint32_t              rank,
                             const CartCoordinate& coordinate);

        /// @brief Assign communicator to Cartesian topology definition.
        ///
        /// Assigns the given @p communicator to the Cartesian grid topology
        /// with the global identifier @p cartTopoId.
        ///
        /// @param cartTopoId
        ///     Global Cartesian grid topology identifier
        /// @param communicator
        ///     New associated communicator
        ///
        /// @exception_strong
        ///
        /// @throws RuntimeError
        ///     on failure to find the Cartesian topology with the given
        ///         @p cartTopoId
        ///
        void
        assignCartCommunicator(CartTopology::IdType cartTopoId,
                               const Communicator&  communicator);

        /// @}
        /// @name Query functions for communication location set definitions
        /// @{

        /// @brief Communication location set lookup by paradigm.
        ///
        /// Returns a constant reference to the communication location set
        /// definition object with the given @p paradigm.
        ///
        /// @param paradigm
        ///     %Paradigm of the communication location set definition object
        ///     to be retrieved
        /// @returns
        ///     Constant reference to the corresponding communication location
        ///     set object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a communication location set definition
        ///     object with the given @p paradigm
        ///
        const CommLocationSet&
        getCommLocationSet(Paradigm paradigm) const;

        /// @}
        /// @name Internal methods
        /// @{

        void
        validate();

        void
        setup();

        /// @}
        /// @name Set internal regions
        /// @{

        void
        setPausingRegion(Region* region);

        void
        setFlushingRegion(Region* region);

        void
        setTaskRootRegion(Region* region);

        void
        setThreadRootRegion(Region* region);

        /// @}
        /// @name Call tree handling
        /// @{

        CallTree*
        getCallTree() const;

        void
        setCallTree(CallTree* callTree);

        /// @}
        /// @name Global time offset handling
        /// @{

        timestamp_t
        getGlobalOffset() const;

        void
        setGlobalOffset(timestamp_t offset);

        /// @}


    private:
        // --- Type definitions ----------------------------

        /// Symbolic names for definition types
        enum DefinitionType
        {
            UNDEFINED,    ///< Only used as initial value

            CALLINGCONTEXT,
            CALLPATH,
            CALLSITE,
            CARTDIMENSION,
            CARTTOPOLOGY,
            COMMUNICATOR,
            GROUPINGSET,
            LOCATION,
            LOCATIONGROUP,
            METRIC,
            REGION,
            RMAWINDOW,
            SOURCELOCATION,
            STRING,
            SYSTEMNODE
        };


        /// Container type for communication location set definitions
        typedef std::vector< CommLocationSet* > CommLocationSetContainer;


        // --- Data members --------------------------------

        /// Communication location set definitions
        CommLocationSetContainer mCommLocationSets;

        /// Global time offset subtracted during trace preprocessing
        timestamp_t mGlobalOffset;

        /// Stores the type of the last definition object added
        DefinitionType mLastDefType;


        // --- Private methods -----------------------------

        /// @name Definition type tracking
        /// @{

        void
        trackType(DefinitionType currDefType);

        /// @}
};
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_MUTABLEGLOBALDEFS_H
