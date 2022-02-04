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
 *  @brief   Implementation of the class GlobalDefs.
 *
 *  This file provides the implementation of the class GlobalDefs.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/GlobalDefs.h>

#include <pearl/CommSet.h>
#include <pearl/Error.h>
#include <pearl/Utils.h>

#include "CallTree.h"
#include "DefinitionContainer.h"
#include "DefinitionTree.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Helper functions -----------------------------------------------------

namespace
{
/// @todo Fix exception conversion
template< typename DefClassT >
const DefClassT&
getDefinitionById(const DefinitionContainer< DefClassT >& container,
                  const typename DefClassT::IdType        id,
                  const string&                           defName)
{
    DefClassT* item;
    try
    {
        item = container.getItemById(id);
    }
    catch (invalid_argument& ex)
    {
        throw RuntimeError("pearl::GlobalDefs::get"
                           + defName
                           + "("
                           + defName
                           + "::IdType) -- invalid ID: "
                           + toStdString(id));
    }

    return *item;
}


/// @todo Fix exception conversion
template< typename DefClassT >
const DefClassT&
getDefinitionByIndex(const DefinitionContainer< DefClassT >& container,
                     const size_t                            index,
                     const string&                           defName)
{
    DefClassT* item;
    try
    {
        item = container.getItemByIndex(index);
    }
    catch (out_of_range& ex)
    {
        throw RuntimeError("pearl::GlobalDefs::get"
                           + defName
                           + "ByIndex(size_t) -- index out of range: "
                           + toStdString(index));
    }

    return *item;
}
}    // unnamed namespace


// --- Constructors & destructor --------------------------------------------

GlobalDefs::GlobalDefs()
    : mContextTree(new DefinitionTree< CallingContext >()),
      mCallTree(new CallTree()),
      mCallsites(new DefinitionContainer< Callsite >()),
      mCartDimensions(new DefinitionContainer< CartDimension >()),
      mCartTopologies(new DefinitionContainer< CartTopology >()),
      mCommunicators(new DefinitionContainer< Communicator >()),
      mGroupingSets(new DefinitionContainer< GroupingSet >()),
      mLocations(new DefinitionContainer< Location >()),
      mLocationGroups(new DefinitionContainer< LocationGroup >()),
      mMetrics(new DefinitionContainer< Metric >()),
      mRegions(new DefinitionContainer< Region >()),
      mRmaWindows(new DefinitionContainer< RmaWindow >()),
      mSourceLocations(new DefinitionContainer< SourceLocation >()),
      mStrings(new DefinitionContainer< String >()),
      mSystemTree(new DefinitionTree< SystemNode >()),
      mPausingRegion(NULL),
      mFlushingRegion(NULL),
      mTaskRootRegion(NULL),
      mThreadRootRegion(NULL)
{
}


GlobalDefs::~GlobalDefs()
{
}


// --- Query functions for CallingContext definitions -----------------------

size_t
GlobalDefs::numCallingContexts() const
{
    return mContextTree->numItems();
}


/// @todo Fix exception conversion
const CallingContext&
GlobalDefs::getCallingContext(const CallingContext::IdType id) const
{
    if (id == CallingContext::NO_ID)
    {
        return CallingContext::UNDEFINED;
    }

    CallingContext* item;
    try
    {
        item = mContextTree->getItemById(id);
    }
    catch (invalid_argument& ex)
    {
        throw RuntimeError("pearl::GlobalDefs::"
                           "getCallingContext(CallingContext::IdType) -- "
                           "invalid ID: " + toStdString(id));
    }

    return *item;
}


/// @todo Fix exception conversion
const CallingContext&
GlobalDefs::getCallingContextByIndex(const size_t index) const
{
    CallingContext* item;
    try
    {
        item = mContextTree->getItemByIndex(index);
    }
    catch (out_of_range& ex)
    {
        throw RuntimeError("pearl::GlobalDefs::"
                           "getCallingContextByIndex(size_t) -- "
                           "index out of range: " + toStdString(index));
    }

    return *item;
}


// --- Query functions for Callpath definitions -----------------------------

size_t
GlobalDefs::numCallpaths() const
{
    return mCallTree->numCallpaths();
}


/// @todo Check exceptions thrown on error
const Callpath&
GlobalDefs::getCallpath(const Callpath::IdType id) const
{
    if (id == Callpath::NO_ID)
    {
        return Callpath::UNDEFINED;
    }

    const Callpath* const item = mCallTree->getCallpath(id);

    return *item;
}


/// @todo Fix: Assumes compact index
/// @todo Check exceptions thrown on error
const Callpath&
GlobalDefs::getCallpathByIndex(const size_t index) const
{
    const Callpath* const item = mCallTree->getCallpath(index);

    return *item;
}


// --- Query functions for Callsite definitions -----------------------------

size_t
GlobalDefs::numCallsites() const
{
    return mCallsites->numItems();
}


const Callsite&
GlobalDefs::getCallsite(const Callsite::IdType id) const
{
    if (id == Callsite::NO_ID)
    {
        return Callsite::UNDEFINED;
    }

    return getDefinitionById(*mCallsites, id, "Callsite");
}


const Callsite&
GlobalDefs::getCallsiteByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mCallsites, index, "Callsite");
}


// --- Query functions for CartDimension definitions ------------------------

size_t
GlobalDefs::numCartDimensions() const
{
    return mCartDimensions->numItems();
}


const CartDimension&
GlobalDefs::getCartDimension(const CartDimension::IdType id) const
{
    return getDefinitionById(*mCartDimensions, id, "CartDimension");
}


const CartDimension&
GlobalDefs::getCartDimensionByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mCartDimensions, index, "CartDimension");
}


// --- Query functions for CartTopology definitions -------------------------

size_t
GlobalDefs::numCartTopologies() const
{
    return mCartTopologies->numItems();
}


const CartTopology&
GlobalDefs::getCartTopology(const CartTopology::IdType id) const
{
    return getDefinitionById(*mCartTopologies, id, "CartTopology");
}


const CartTopology&
GlobalDefs::getCartTopologyByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mCartTopologies, index, "CartTopology");
}


// --- Query functions for Communicator definitions -------------------------

size_t
GlobalDefs::numCommunicators() const
{
    return mCommunicators->numItems();
}


const Communicator&
GlobalDefs::getCommunicator(const Communicator::IdType id) const
{
    if (id == Communicator::NO_ID)
    {
        return Communicator::UNDEFINED;
    }

    return getDefinitionById(*mCommunicators, id, "Communicator");
}


const Communicator&
GlobalDefs::getCommunicatorByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mCommunicators, index, "Communicator");
}


// --- Query functions for GroupingSet definitions --------------------------

size_t
GlobalDefs::numGroupingSets() const
{
    return mGroupingSets->numItems();
}


const GroupingSet&
GlobalDefs::getGroupingSet(const GroupingSet::IdType id) const
{
    return getDefinitionById(*mGroupingSets, id, "GroupingSet");
}


const GroupingSet&
GlobalDefs::getGroupingSetByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mGroupingSets, index, "GroupingSet");
}


// --- Query functions for Location definitions -----------------------------

size_t
GlobalDefs::numLocations() const
{
    return mLocations->numItems();
}


const Location&
GlobalDefs::getLocation(const Location::IdType id) const
{
    if (id == Location::NO_ID)
    {
        return Location::UNDEFINED;
    }

    return getDefinitionById(*mLocations, id, "Location");
}


const Location&
GlobalDefs::getLocationByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mLocations, index, "Location");
}


// --- Query functions for LocationGroup definitions ------------------------

size_t
GlobalDefs::numLocationGroups() const
{
    return mLocationGroups->numItems();
}


const LocationGroup&
GlobalDefs::getLocationGroup(const LocationGroup::IdType id) const
{
    return getDefinitionById(*mLocationGroups, id, "LocationGroup");
}


const LocationGroup&
GlobalDefs::getLocationGroupByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mLocationGroups, index, "LocationGroup");
}


// --- Query functions for Metric definitions -------------------------------

size_t
GlobalDefs::numMetrics() const
{
    return mMetrics->numItems();
}


const Metric&
GlobalDefs::getMetric(const Metric::IdType id) const
{
    return getDefinitionById(*mMetrics, id, "Metric");
}


const Metric&
GlobalDefs::getMetricByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mMetrics, index, "Metric");
}


// --- Query functions for Region definitions -------------------------------

size_t
GlobalDefs::numRegions() const
{
    return mRegions->numItems();
}


const Region&
GlobalDefs::getRegion(const Region::IdType id) const
{
    if (id == Region::NO_ID)
    {
        return Region::UNDEFINED;
    }

    return getDefinitionById(*mRegions, id, "Region");
}


const Region&
GlobalDefs::getRegionByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mRegions, index, "Region");
}


// --- Query functions for RmaWindow definitions ----------------------------

size_t
GlobalDefs::numRmaWindows() const
{
    return mRmaWindows->numItems();
}


const RmaWindow&
GlobalDefs::getRmaWindow(const RmaWindow::IdType id) const
{
    return getDefinitionById(*mRmaWindows, id, "RmaWindow");
}


const RmaWindow&
GlobalDefs::getRmaWindowByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mRmaWindows, index, "RmaWindow");
}


// --- Query functions for SourceLocation definitions -----------------------

size_t
GlobalDefs::numSourceLocations() const
{
    return mSourceLocations->numItems();
}


const SourceLocation&
GlobalDefs::getSourceLocation(const SourceLocation::IdType id) const
{
    if (id == SourceLocation::NO_ID)
    {
        return SourceLocation::UNDEFINED;
    }

    return getDefinitionById(*mSourceLocations, id, "SourceLocation");
}


const SourceLocation&
GlobalDefs::getSourceLocationByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mSourceLocations, index, "SourceLocation");
}


// --- Query functions for String definitions -------------------------------

size_t
GlobalDefs::numStrings() const
{
    return mStrings->numItems();
}


const String&
GlobalDefs::getString(const String::IdType id) const
{
    switch (id)
    {
        case String::NODE_ID:
            return String::NODE;

        case String::MACHINE_ID:
            return String::MACHINE;

        case String::NO_ID:
            return String::UNDEFINED;

        default:
            break;
    }

    return getDefinitionById(*mStrings, id, "String");
}


const String&
GlobalDefs::getStringByIndex(const size_t index) const
{
    return getDefinitionByIndex(*mStrings, index, "String");
}


// --- Query functions for SystemNode definitions ---------------------------

size_t
GlobalDefs::numSystemNodes() const
{
    return mSystemTree->numItems();
}


/// @todo Fix exception conversion
const SystemNode&
GlobalDefs::getSystemNode(const SystemNode::IdType id) const
{
    if (id == SystemNode::NO_ID)
    {
        return SystemNode::UNDEFINED;
    }

    SystemNode* item;
    try
    {
        item = mSystemTree->getItemById(id);
    }
    catch (invalid_argument& ex)
    {
        throw RuntimeError("pearl::GlobalDefs::"
                           "getSystemNode(SystemNode::IdType) -- "
                           "invalid ID: " + toStdString(id));
    }

    return *item;
}


/// @todo Fix exception conversion
const SystemNode&
GlobalDefs::getSystemNodeByIndex(const size_t index) const
{
    SystemNode* item;
    try
    {
        item = mSystemTree->getItemByIndex(index);
    }
    catch (out_of_range& ex)
    {
        throw RuntimeError("pearl::GlobalDefs::"
                           "getSystemNodeByIndex(size_t) -- "
                           "index out of range: " + toStdString(index));
    }

    return *item;
}


// --- Get stored definitions by ID -----------------------------------------

/// @deprecated
/// @todo Eliminate use of this method
Callpath*
GlobalDefs::get_cnode(const Callpath::IdType id) const
{
    if (id == Callpath::NO_ID)
    {
        return 0;
    }

    Callpath* result = mCallTree->getCallpath(id);
    if (!result)
    {
        throw RuntimeError("GlobalDefs::get_cnode(ident_t) -- ID out of bounds.");
    }

    return result;
}


/// @deprecated
/// @todo Eliminate use of this method
GroupingSet*
GlobalDefs::get_group(const GroupingSet::IdType id) const
{
    if (id == CommSet::NO_ID)
    {
        return 0;
    }

    const GroupingSet& item = getDefinitionById(*mGroupingSets, id,
                                                "GroupingSet");

    return const_cast< GroupingSet* >(&item);
}


/// @deprecated
/// @todo Eliminate use of this method
Communicator*
GlobalDefs::get_comm(const Communicator::IdType id) const
{
    if (id == Communicator::NO_ID)
    {
        return 0;
    }

    const Communicator& item = getDefinitionById(*mCommunicators, id,
                                                 "Communicator");

    return const_cast< Communicator* >(&item);
}


/// @deprecated
/// @todo Eliminate use of this method
RmaWindow*
GlobalDefs::get_window(const RmaWindow::IdType id) const
{
    const RmaWindow& item = getDefinitionById(*mRmaWindows, id, "RmaWindow");

    return const_cast< RmaWindow* >(&item);
}


// --- Get internal regions -------------------------------------------------

const Region&
GlobalDefs::getPausingRegion() const
{
    return *mPausingRegion;
}


const Region&
GlobalDefs::getFlushingRegion() const
{
    return *mFlushingRegion;
}


const Region&
GlobalDefs::getTaskRootRegion() const
{
    return *mTaskRootRegion;
}


const Region&
GlobalDefs::getThreadRootRegion() const
{
    return *mThreadRootRegion;
}
