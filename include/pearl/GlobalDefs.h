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
 *  @brief   Declaration of the class GlobalDefs.
 *
 *  This header file provides the declaration of the class GlobalDefs.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_GLOBALDEFS_H
#define PEARL_GLOBALDEFS_H


#include <cstddef>

#include <pearl/CallingContext.h>
#include <pearl/Callpath.h>
#include <pearl/Callsite.h>
#include <pearl/CartDimension.h>
#include <pearl/CartTopology.h>
#include <pearl/Communicator.h>
#include <pearl/GroupingSet.h>
#include <pearl/Location.h>
#include <pearl/LocationGroup.h>
#include <pearl/Metric.h>
#include <pearl/Region.h>
#include <pearl/RmaWindow.h>
#include <pearl/ScopedPtr.h>
#include <pearl/SourceLocation.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>
#include <pearl/pearlfwd_detail.h>


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   GlobalDefs
 *  @ingroup PEARL_base
 *  @brief   Stores the global definitions of a tracing experiment.
 **/
/*-------------------------------------------------------------------------*/

class GlobalDefs
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Destructor.
        ///
        /// Destroys the GlobalDefs instance.  This implicitly deletes all
        /// global definition objects stored for the tracing experiment.
        ///
        /// @exception_nothrow
        ///
        virtual
        ~GlobalDefs();

        /// @}
        /// @name Query functions for CallingContext definitions
        /// @{

        /// @brief Get number of %CallingContext definitions.
        ///
        /// Returns the total number of CallingContext definitions for this
        /// tracing experiment.
        ///
        /// @returns
        ///     Number of CallingContext definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numCallingContexts() const;

        /// @brief %CallingContext lookup by identifier.
        ///
        /// Returns a constant reference to the CallingContext definition
        /// object with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the CallingContext definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding CallingContext object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a CallingContext definition object with the
        ///     given @p id
        ///
        const CallingContext&
        getCallingContext(CallingContext::IdType id) const;

        /// @brief %CallingContext lookup by index.
        ///
        /// Returns a constant reference to the CallingContext definition
        /// object stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#calling_contexts-1]
        /// @returns
        ///     Constant reference to the corresponding CallingContext object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const CallingContext&
        getCallingContextByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for Callpath definitions
        /// @{

        /// @brief Get number of %Callpath definitions.
        ///
        /// Returns the total number of Callpath definitions for this tracing
        /// experiment.
        ///
        /// @returns
        ///     Number of Callpath definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numCallpaths() const;

        /// @brief %Callpath lookup by identifier.
        ///
        /// Returns a constant reference to the Callpath definition object with
        /// the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the Callpath definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding Callpath object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a Callpath definition object with the given
        ///     @p id
        ///
        const Callpath&
        getCallpath(Callpath::IdType id) const;

        /// @brief %Callpath lookup by index.
        ///
        /// Returns a constant reference to the Callpath definition object
        /// stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#callpaths-1]
        /// @returns
        ///     Constant reference to the corresponding Callpath object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const Callpath&
        getCallpathByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for Callsite definitions
        /// @{

        /// @brief Get number of %Callsite definitions.
        ///
        /// Returns the total number of Callsite definitions for this tracing
        /// experiment.
        ///
        /// @returns
        ///     Number of Callsite definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numCallsites() const;

        /// @brief %Callsite lookup by identifier.
        ///
        /// Returns a constant reference to the Callsite definition object with
        /// the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the Callsite definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding Callsite object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a Callsite definition object with the given
        ///     @p id
        ///
        const Callsite&
        getCallsite(Callsite::IdType id) const;

        /// @brief %Callsite lookup by index.
        ///
        /// Returns a constant reference to the Callsite definition object
        /// stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#callsites-1]
        /// @returns
        ///     Constant reference to the corresponding Callsite object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const Callsite&
        getCallsiteByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for CartDimension definitions
        /// @{

        /// @brief Get number of %CartDimension definitions.
        ///
        /// Returns the total number of CartDimension definitions for this
        /// tracing experiment.
        ///
        /// @returns
        ///     Number of CartDimension definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numCartDimensions() const;

        /// @brief %CartDimension lookup by identifier.
        ///
        /// Returns a constant reference to the Cartesian grid topology
        /// dimension definition object with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the Cartesian grid topology dimension
        ///     definition object to be retrieved
        /// @returns
        ///     Constant reference to the corresponding CartDimension object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a Cartesian grid topology dimension
        ///     definition object with the given @p id
        ///
        const CartDimension&
        getCartDimension(CartDimension::IdType id) const;

        /// @brief %CartDimension lookup by index.
        ///
        /// Returns a constant reference to the Cartesian grid topology
        /// dimension definition object stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#cart_dimensions-1]
        /// @returns
        ///     Constant reference to the corresponding CartDimension object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const CartDimension&
        getCartDimensionByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for CartTopology definitions
        /// @{

        /// @brief Get number of %CartTopology definitions.
        ///
        /// Returns the total number of %CartTopology definitions for this
        /// tracing experiment.
        ///
        /// @returns
        ///     Number of CartTopology definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numCartTopologies() const;

        /// @brief %CartTopology lookup by identifier.
        ///
        /// Returns a constant reference to the Cartesian grid topology
        /// definition object with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the Cartesian grid topology definition
        ///     object to be retrieved
        /// @returns
        ///     Constant reference to the corresponding CartTopology object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a Cartesian grid topology definition object
        ///     with the given @p id
        ///
        const CartTopology&
        getCartTopology(CartTopology::IdType id) const;

        /// @brief %CartTopology lookup by index.
        ///
        /// Returns a constant reference to the Cartesian grid topology
        /// definition object stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#cart_topologies-1]
        /// @returns
        ///     Constant reference to the corresponding CartTopology object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const CartTopology&
        getCartTopologyByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for Communicator definitions
        /// @{

        /// @brief Get number of %Communicator definitions.
        ///
        /// Returns the total number of Communicator definitions for this
        /// tracing experiment.
        ///
        /// @returns
        ///     Number of Communicator definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numCommunicators() const;

        /// @brief %Communicator lookup by identifier.
        ///
        /// Returns a constant reference to the Communicator definition object
        /// with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the Communicator definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding Communicator object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a Communicator definition object with the
        ///     given @p id
        ///
        const Communicator&
        getCommunicator(Communicator::IdType id) const;

        /// @brief %Communicator lookup by index.
        ///
        /// Returns a constant reference to the Communicator definition object
        /// stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#communicators-1]
        /// @returns
        ///     Constant reference to the corresponding Communicator object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const Communicator&
        getCommunicatorByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for GroupingSet definitions
        /// @{

        /// @brief Get number of %GroupingSet definitions.
        ///
        /// Returns the total number of GroupingSet definitions for this
        /// tracing experiment.
        ///
        /// @returns
        ///     Number of GroupingSet definitons
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numGroupingSets() const;

        /// @brief %GroupingSet lookup by identifier.
        ///
        /// Returns a constant reference to the GroupingSet definition object
        /// with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the GroupingSet definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding GroupingSet object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a GroupingSet definition object with the
        ///     given @p id
        ///
        const GroupingSet&
        getGroupingSet(GroupingSet::IdType id) const;

        /// @brief %GroupingSet lookup by index.
        ///
        /// Returns a constant reference to the GroupingSet definition object
        /// stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#grouping_sets-1]
        /// @returns
        ///     Constant reference to the corresponding GroupingSet object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const GroupingSet&
        getGroupingSetByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for Location definitions
        /// @{

        /// @brief Get number of %Location definitions.
        ///
        /// Returns the total number of Location definitions for this tracing
        /// experiment.
        ///
        /// @returns
        ///     Number of Location definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numLocations() const;

        /// @brief %Location lookup by identifier.
        ///
        /// Returns a constant reference to the Location definition object
        /// with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the Location definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding Location object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a Location definition object with the given
        ///     @p id
        ///
        const Location&
        getLocation(Location::IdType id) const;

        /// @brief %Location lookup by index.
        ///
        /// Returns a constant reference to the Location definition object
        /// stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#locations-1]
        /// @returns
        ///     Constant reference to the corresponding Location object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const Location&
        getLocationByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for LocationGroup definitions
        /// @{

        /// @brief Get number of %LocationGroup definitions.
        ///
        /// Returns the total number of LocationGroup definitions for this
        /// tracing experiment.
        ///
        /// @returns
        ///     Number of LocationGroup definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numLocationGroups() const;

        /// @brief %LocationGroup lookup by identifier.
        ///
        /// Returns a constant reference to the LocationGroup definition object
        /// with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the LocationGroup definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding LocationGroup object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a LocationGroup definition object with the
        ///     given @p id
        ///
        const LocationGroup&
        getLocationGroup(LocationGroup::IdType id) const;

        /// @brief %LocationGroup lookup by index.
        ///
        /// Returns a constant reference to the LocationGroup definition object
        /// stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#location_groups-1]
        /// @returns
        ///     Constant reference to the corresponding LocationGroup object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const LocationGroup&
        getLocationGroupByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for Metric definitions
        /// @{

        /// @brief Get number of Metric definitions.
        ///
        /// Returns the total number of Metric definitions for this tracing
        /// experiment.
        ///
        /// @returns
        ///     Number of Metric definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numMetrics() const;

        /// @brief %Metric lookup by identifier.
        ///
        /// Returns a constant reference to the Metric definition object with
        /// the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the Metric definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding Metric object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a Metric definition object with the given
        ///     @p id
        ///
        const Metric&
        getMetric(Metric::IdType id) const;

        /// @brief %Metric lookup by index.
        ///
        /// Returns a constant reference to the Metric definition object stored
        /// at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#metrics-1]
        /// @returns
        ///     Constant reference to the corresponding Metric object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const Metric&
        getMetricByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for Region definitions
        /// @{

        /// @brief Get number of Region definitions.
        ///
        /// Returns the total number of Region definitions for this tracing
        /// experiment.
        ///
        /// @returns
        ///     Number of Region definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numRegions() const;

        /// @brief %Region lookup by identifier.
        ///
        /// Returns a constant reference to the Region definition object with
        /// the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the Region definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding Region object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a Region definition object with the given
        ///     @p id
        ///
        const Region&
        getRegion(Region::IdType id) const;

        /// @brief %Region lookup by index.
        ///
        /// Returns a constant reference to the Region definition object
        /// stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#regions-1]
        /// @returns
        ///     Constant reference to the corresponding Region object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const Region&
        getRegionByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for RmaWindow definitions
        /// @{

        /// @brief Get number of RmaWindow definitions.
        ///
        /// Returns the total number of RmaWindow definitions for this tracing
        /// experiment.
        ///
        /// @returns
        ///     Number of RmaWindow definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numRmaWindows() const;

        /// @brief %RmaWindow lookup by identifier.
        ///
        /// Returns a constant reference to the RmaWindow definition object
        /// with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the RmaWindow definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding RmaWindow object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a RmaWindow definition object with the given
        ///     @p id
        ///
        const RmaWindow&
        getRmaWindow(RmaWindow::IdType id) const;

        /// @brief %RmaWindow lookup by index.
        ///
        /// Returns a constant reference to the RmaWindow definition object
        /// stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#rma_windows-1]
        /// @returns
        ///     Constant reference to the corresponding RmaWindow object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const RmaWindow&
        getRmaWindowByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for SourceLocation definitions
        /// @{

        /// @brief Get number of %SourceLocation definitions.
        ///
        /// Returns the total number of SourceLocation definitions for this
        /// tracing experiment.
        ///
        /// @returns
        ///     Number of SourceLocation definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numSourceLocations() const;

        /// @brief %SourceLocation lookup by identifier.
        ///
        /// Returns a constant reference to the SourceLocation definition
        /// object with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the SourceLocation definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding SourceLocation object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a SourceLocation definition object with the
        ///     given @p id
        ///
        const SourceLocation&
        getSourceLocation(SourceLocation::IdType id) const;

        /// @brief %SourceLocation lookup by index.
        ///
        /// Returns a constant reference to the SourceLocation definition
        /// object stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#source_locations-1]
        /// @returns
        ///     Constant reference to the corresponding SourceLocation object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const SourceLocation&
        getSourceLocationByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for String definitions
        /// @{

        /// @brief Get number of %String definitions.
        ///
        /// Returns the total number of String definitions for this tracing
        /// experiment.
        ///
        /// @returns
        ///     Number of String definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numStrings() const;

        /// @brief %String lookup by identifier.
        ///
        /// Returns a constant reference to the String definition object with
        /// the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the String definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding String object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a String definition object with the given
        ///     @p id
        ///
        const String&
        getString(String::IdType id) const;

        /// @brief %String lookup by index.
        ///
        /// Returns a constant reference to the String definition object stored
        /// at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#strings-1]
        /// @returns
        ///     Constant reference to the corresponding String object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const String&
        getStringByIndex(std::size_t index) const;

        /// @}
        /// @name Query functions for SystemNode definitions
        /// @{

        /// @brief Get number of %SystemNode definitions.
        ///
        /// Returns the total number of SystemNode definitions for this tracing
        /// experiment.
        ///
        /// @returns
        ///     Number of SystemNode definitions
        ///
        /// @exception_nothrow
        ///
        std::size_t
        numSystemNodes() const;

        /// @brief %SystemNode lookup by identifier.
        ///
        /// Returns a constant reference to the SystemNode definition object
        /// with the given global identifier @p id.
        ///
        /// @param id
        ///     Global identifier of the SystemNode definition object to be
        ///     retrieved
        /// @returns
        ///     Constant reference to the corresponding SystemNode object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     on failure to find a SystemNode definition object with the
        ///     given @p id
        ///
        const SystemNode&
        getSystemNode(SystemNode::IdType id) const;

        /// @brief %SystemNode lookup by index.
        ///
        /// Returns a constant reference to the SystemNode definition object
        /// stored at the given @p index.
        ///
        /// @param index
        ///     Index in the range [0,@#system_nodes-1]
        /// @returns
        ///     Constant reference to the corresponding SystemNode object
        ///
        /// @exception_strong
        ///
        /// @throws pearl::RuntimeError
        ///     if @p index is out of bounds
        ///
        const SystemNode&
        getSystemNodeByIndex(std::size_t index) const;

        /// @}


    protected:
        // --- Type definitions ----------------------------

        /// Container type for CallingContext definitions
        typedef detail::DefinitionTree< CallingContext > CallingContextContainer;

        /// Container type for Callsite definitions
        typedef detail::DefinitionContainer< Callsite > CallsiteContainer;

        /// Container type for CartDimension definitions
        typedef detail::DefinitionContainer< CartDimension > CartDimensionContainer;

        /// Container type for CartTopology definitions
        typedef detail::DefinitionContainer< CartTopology > CartTopologyContainer;

        /// Container type for Communicator definitions
        typedef detail::DefinitionContainer< Communicator > CommunicatorContainer;

        /// Container type for GroupingSet definitions
        typedef detail::DefinitionContainer< GroupingSet > GroupingSetContainer;

        /// Container type for Location definitions
        typedef detail::DefinitionContainer< Location > LocationContainer;

        /// Container type for LocationGroup definitions
        typedef detail::DefinitionContainer< LocationGroup > LocationGroupContainer;

        /// Container type for Metric definitions
        typedef detail::DefinitionContainer< Metric > MetricContainer;

        /// Container type for Region definitions
        typedef detail::DefinitionContainer< Region > RegionContainer;

        /// Container type for RmaWindow definitions
        typedef detail::DefinitionContainer< RmaWindow > RmaWindowContainer;

        /// Container type for SourceLocation definitions
        typedef detail::DefinitionContainer< SourceLocation > SourceLocationContainer;

        /// Container type for String definitions
        typedef detail::DefinitionContainer< String > StringContainer;

        /// Container type for SystemNode definitions
        typedef detail::DefinitionTree< SystemNode > SystemNodeContainer;


        // --- Data members --------------------------------

        /// Global calling context tree &rarr; %CallingContext definitions
        const ScopedPtr< CallingContextContainer > mContextTree;

        /// Global call tree &rarr; %Callpath definitions
        ScopedPtr< detail::CallTree > mCallTree;

        /// %Callsite definitions
        const ScopedPtr< CallsiteContainer > mCallsites;

        /// %CartDimension definitions
        const ScopedPtr< CartDimensionContainer > mCartDimensions;

        /// %CartTopology definitions
        const ScopedPtr< CartTopologyContainer > mCartTopologies;

        /// %Communicator definitions
        const ScopedPtr< CommunicatorContainer > mCommunicators;

        /// %GroupingSet definitions
        const ScopedPtr< GroupingSetContainer > mGroupingSets;

        /// %Location definitions
        const ScopedPtr< LocationContainer > mLocations;

        /// %LocationGroup definitions
        const ScopedPtr< LocationGroupContainer > mLocationGroups;

        /// %Metric definitions
        const ScopedPtr< MetricContainer > mMetrics;

        /// %Region definitions
        const ScopedPtr< RegionContainer > mRegions;

        /// %RmaWindow definitions
        const ScopedPtr< RmaWindowContainer > mRmaWindows;

        /// %SourceLocation definitions
        const ScopedPtr< SourceLocationContainer > mSourceLocations;

        /// %String definitions
        const ScopedPtr< StringContainer > mStrings;

        /// Global system hierarchy &rarr; %SystemNode definitions
        const ScopedPtr< SystemNodeContainer > mSystemTree;


        // --- Protected methods ---------------------------

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
        GlobalDefs();

        /// @}


    public:
        /// @name Get stored definitions by ID
        /// @{

        Callpath*
        get_cnode(Callpath::IdType id) const;

        GroupingSet*
        get_group(GroupingSet::IdType id) const;

        Communicator*
        get_comm(Communicator::IdType id) const;

        RmaWindow*
        get_window(RmaWindow::IdType id) const;

        /// @}
        /// @name Get internal regions
        /// @{

        const Region&
        getPausingRegion() const;

        const Region&
        getFlushingRegion() const;

        const Region&
        getTaskRootRegion() const;

        const Region&
        getThreadRootRegion() const;

        /// @}


    protected:
        /// Pointer to a Region object representing code regions where measurement
        /// was temporarily turned off by the user.
        Region* mPausingRegion;

        /// Pointer to a Region object representing a trace buffer flush triggered
        /// by the measurement system.
        Region* mFlushingRegion;

        /// Pointer to a Region object representing the task root in the
        /// generic tasking model
        Region* mTaskRootRegion;

        /// Pointer to a Region object representing the THREADS root node for
        /// create/wait threading models
        Region* mThreadRootRegion;
};
}    // namespace pearl


#endif    // !PEARL_GLOBALDEFS_H
