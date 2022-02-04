/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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
 *  @brief   Implementation of the class MutableGlobalDefs.
 *
 *  This file provides the implementation of the class MutableGlobalDefs.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "MutableGlobalDefs.h"

#include <cfloat>

#include <UTILS_Error.h>

#include <pearl/CommLocationSet.h>
#include <pearl/Error.h>

#include "CallTree.h"
#include "DefinitionContainer.h"
#include "DefinitionTree.h"
#include "MutableCartTopology.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Helper functions -----------------------------------------------------

namespace
{
/// @brief   Ensures unique IDs for global definitions.
/// @relates pearl::detail::MutableGlobalDefs
///
/// Checks whether the global definition objects stored in @p container have
/// unique global identifiers and throws an exception if not.  This function
/// implicitly also optimizes lookups by identifier.
///
/// @param container
///     Global definition container
/// @param defName
///     Definition name used in the exception message
///
/// @tparam DefContainerT
///     Global definition container type providing the required typedef's and
///     member functions (DefinitionContainer or DefinitionTree)
///
/// @throws RuntimeError
///     on identifying a duplicate identifier
///
template< typename DefContainerT >
void
ensureUniqueIds(DefContainerT& container,
                const string&  defName)
{
    // Special case: empty container
    if (container.isEmpty())
    {
        return;
    }

    // Step 1: ensure that definitions are sorted by ID
    container.optimizeLookups();

    // Step 2: ensure that consecutive definitions have different IDs
    typename DefContainerT::ElementType::IdType prevId =
        container.getItemByIndex(0)->getId();
    const size_t numItems = container.numItems();
    for (size_t i = 1; i < numItems; ++i)
    {
        typename DefContainerT::ElementType::IdType currId =
            container.getItemByIndex(i)->getId();
        if (currId == prevId)
        {
            throw RuntimeError("pearl::detail::MutableGlobalDefs::"
                               "validate() -- "
                               "duplicate " + defName + " ID: "
                               + toStdString(currId));
        }
        prevId = currId;
    }
}
}    // unnamed namespace


// --- Constructors & destructor --------------------------------------------

MutableGlobalDefs::MutableGlobalDefs()
    : mGlobalOffset(DBL_MAX),
      mLastDefType(UNDEFINED)
{
}


// --- Insertion functions for global definitions ---------------------------

void
MutableGlobalDefs::addCallingContext(CallingContext* const item)
{
    trackType(CALLINGCONTEXT);

    mContextTree->addItem(item);
}


/// @todo Check exceptions thrown on error
void
MutableGlobalDefs::addCallpath(Callpath* const item)
{
    trackType(CALLPATH);

    mCallTree->addCallpath(item);
}


void
MutableGlobalDefs::addCallsite(Callsite* const item)
{
    trackType(CALLSITE);

    mCallsites->addItem(item);
}


void
MutableGlobalDefs::addCartDimension(CartDimension* const item)
{
    trackType(CARTDIMENSION);

    mCartDimensions->addItem(item);
}


void
MutableGlobalDefs::addCartTopology(CartTopology* const item)
{
    trackType(CARTTOPOLOGY);

    mCartTopologies->addItem(item);
}


void
MutableGlobalDefs::addCommunicator(Communicator* const item)
{
    trackType(COMMUNICATOR);

    mCommunicators->addItem(item);
}


void
MutableGlobalDefs::addGroupingSet(GroupingSet* const item)
{
    trackType(GROUPINGSET);

    if (item->getType() == GroupingSet::COMM_LOCATION_SET)
    {
        // Track communication location sets separately, using
        // "commit-or-rollback" to fulfill strong/basic exception
        // safety guarantee
        mCommLocationSets.push_back(static_cast< CommLocationSet* >(item));
        try
        {
            mGroupingSets->addItem(item);
        }
        catch (exception& ex)
        {
            mCommLocationSets.pop_back();
            throw;
        }
    }
    else
    {
        mGroupingSets->addItem(item);
    }
}


void
MutableGlobalDefs::addLocation(Location* const item)
{
    trackType(LOCATION);

    mLocations->addItem(item);
}


void
MutableGlobalDefs::addLocationGroup(LocationGroup* const item)
{
    trackType(LOCATIONGROUP);

    mLocationGroups->addItem(item);
}


void
MutableGlobalDefs::addMetric(Metric* const item)
{
    trackType(METRIC);

    mMetrics->addItem(item);
}


void
MutableGlobalDefs::addRegion(Region* const item)
{
    trackType(REGION);

    mRegions->addItem(item);
}


void
MutableGlobalDefs::addRmaWindow(RmaWindow* const item)
{
    trackType(RMAWINDOW);

    mRmaWindows->addItem(item);
}


void
MutableGlobalDefs::addSourceLocation(SourceLocation* const item)
{
    trackType(SOURCELOCATION);

    mSourceLocations->addItem(item);
}


void
MutableGlobalDefs::addString(String* const item)
{
    trackType(STRING);

    mStrings->addItem(item);
}


void
MutableGlobalDefs::addSystemNode(SystemNode* const item)
{
    trackType(SYSTEMNODE);

    mSystemTree->addItem(item);
}


// --- Definition amendment functions ---------------------------------------

void
MutableGlobalDefs::assignCartCoordinate(const CartTopology::IdType cartTopoId,
                                        const uint32_t             rank,
                                        const CartCoordinate&      coordinate)
{
    MutableCartTopology* topology;
    try
    {
        CartTopology* temp = mCartTopologies->getItemById(cartTopoId);

        topology = static_cast< MutableCartTopology* >(temp);
    }
    catch (invalid_argument& ex)
    {
        throw RuntimeError("pearl::detail::MutableGlobalDefs::"
                           "assignCartCoordinate(CartTopology::IdType, "
                           "uint32_t, const CartCoordinate&) -- "
                           "invalid topology ID: " + toStdString(cartTopoId));
    }

    topology->addRankCoordinate(rank, coordinate);
}


void
MutableGlobalDefs::assignCartCommunicator(const CartTopology::IdType cartTopoId,
                                          const Communicator&        communicator)
{
    MutableCartTopology* topology;
    try
    {
        CartTopology* temp = mCartTopologies->getItemById(cartTopoId);

        topology = static_cast< MutableCartTopology* >(temp);
    }
    catch (invalid_argument& ex)
    {
        throw RuntimeError("pearl::detail::MutableGlobalDefs::"
                           "assignCartCommunicator(CartTopology::IdType, "
                           "const Communicator&) -- "
                           "invalid topology ID: " + toStdString(cartTopoId));
    }

    topology->setCommunicator(communicator);
}


// --- Query functions for communication location set definitions -----------

const CommLocationSet&
MutableGlobalDefs::getCommLocationSet(const Paradigm paradigm) const
{
    CommLocationSetContainer::const_iterator it = mCommLocationSets.begin();
    while (  (it != mCommLocationSets.end())
          && ((*it)->getParadigm() != paradigm))
    {
        ++it;
    }

    if (it == mCommLocationSets.end())
    {
        throw RuntimeError("pearl::detail::MutableGlobalDefs::"
                           "getCommLocationSet(Paradigm) -- "
                           "no communication location set defined for: "
                           + toString(paradigm));
    }

    return *(*it);
}


// --- Internal methods -----------------------------------------------------

void
MutableGlobalDefs::validate()
{
    ensureUniqueIds(*mContextTree, "CallingContext");
    ensureUniqueIds(*mCallsites, "Callsite");
    ensureUniqueIds(*mCartDimensions, "CartDimension");
    ensureUniqueIds(*mCartTopologies, "CartTopology");
    ensureUniqueIds(*mCommunicators, "Communicator");
    ensureUniqueIds(*mGroupingSets, "GroupingSet");
    ensureUniqueIds(*mLocations, "Location");
    ensureUniqueIds(*mLocationGroups, "LocationGroup");
    ensureUniqueIds(*mMetrics, "Metric");
    ensureUniqueIds(*mRegions, "Region");
    ensureUniqueIds(*mRmaWindows, "RmaWindow");
    ensureUniqueIds(*mSourceLocations, "SourceLocation");
    ensureUniqueIds(*mStrings, "String");
    ensureUniqueIds(*mSystemTree, "SystemNode");

    // Verify that at most one communication location set exists per paradigm
    vector< uint8_t > counts(Paradigm::NUMBER_OF_PARADIGMS, 0);
    for (CommLocationSetContainer::iterator it = mCommLocationSets.begin();
         it != mCommLocationSets.end();
         ++it)
    {
        const Paradigm paradigm = (*it)->getParadigm();
        if (counts[native_value(paradigm)] > 0)
        {
            throw RuntimeError("pearl::detail::MutableGlobalDefs::"
                               "validate() -- "
                               "duplicate communication location set for: "
                               + toString(paradigm));
        }
        ++counts[native_value(paradigm)];
    }
}


/// @todo Refactor, then remove optimizeLookups() calls already triggered by
///       validate()
void
MutableGlobalDefs::setup()
{
    mContextTree->optimizeLookups();
    mCallsites->optimizeLookups();
    mCartDimensions->optimizeLookups();
    mCartTopologies->optimizeLookups();
    mCommunicators->optimizeLookups();
    mGroupingSets->optimizeLookups();
    mLocations->optimizeLookups();
    mLocationGroups->optimizeLookups();
    mMetrics->optimizeLookups();
    mRegions->optimizeLookups();
    mRmaWindows->optimizeLookups();
    mSourceLocations->optimizeLookups();
    mStrings->optimizeLookups();
    mSystemTree->optimizeLookups();

    // Determine next String identifier
    String::IdType nextStringId = 0;
    if (!mStrings->isEmpty())
    {
        const size_t lastIndex = mStrings->numItems() - 1;

        nextStringId = mStrings->getItemByIndex(lastIndex)->getId() + 1;
    }

    // Determine next Region identifier
    Region::IdType nextRegionId = 0;
    if (!mRegions->isEmpty())
    {
        const size_t lastIndex = mRegions->numItems() - 1;

        nextRegionId = mRegions->getItemByIndex(lastIndex)->getId() + 1;
    }

    // Create special "MEASUREMENT OFF" region if necessary
    if (!mPausingRegion)
    {
        // Create "MEASUREMENT OFF" string
        String* pauseStr = new String(nextStringId++, "MEASUREMENT OFF");
        addString(pauseStr);

        // Create "MEASUREMENT OFF" region
        mPausingRegion = new Region(nextRegionId++,
                                    *pauseStr,
                                    *pauseStr,
                                    String::UNDEFINED,
                                    Region::ROLE_ARTIFICIAL,
                                    Paradigm::USER,
                                    String::UNDEFINED,
                                    PEARL_NO_NUM,
                                    PEARL_NO_NUM);
        addRegion(mPausingRegion);
    }

    // Create special "TRACE BUFFER FLUSH" region if necessary
    if (!mFlushingRegion)
    {
        // Create "TRACE BUFFER FLUSH" string
        String* flushStr = new String(nextStringId++, "TRACE BUFFER FLUSH");
        addString(flushStr);

        // Create "TRACE BUFFER FLUSH" region
        mFlushingRegion = new Region(nextRegionId++,
                                     *flushStr,
                                     *flushStr,
                                     String::UNDEFINED,
                                     Region::ROLE_ARTIFICIAL,
                                     Paradigm::MEASUREMENT_SYSTEM,
                                     String::UNDEFINED,
                                     PEARL_NO_NUM,
                                     PEARL_NO_NUM);
        addRegion(mFlushingRegion);
    }

    // Create special "TASKS" region if necessary
    if (!mTaskRootRegion)
    {
        // Create "TASKS" string
        String* taskRootStr = new String(nextStringId++, "TASKS");
        addString(taskRootStr);

        // Create "TASKS" region
        mTaskRootRegion = new Region(nextRegionId++,
                                     *taskRootStr,
                                     *taskRootStr,
                                     String::UNDEFINED,
                                     Region::ROLE_ARTIFICIAL,
                                     Paradigm::OPENMP,
                                     String::UNDEFINED,
                                     PEARL_NO_NUM,
                                     PEARL_NO_NUM);
        addRegion(mTaskRootRegion);
    }

    // Create special "THREADS" region if necessary
    if (!mThreadRootRegion)
    {
        // Create "THREADS" string
        String* threadRootStr = new String(nextStringId++, "THREADS");
        addString(threadRootStr);

        // Create "THREADS" region
        mThreadRootRegion = new Region(nextRegionId++,
                                       *threadRootStr,
                                       *threadRootStr,
                                       String::UNDEFINED,
                                       Region::ROLE_ARTIFICIAL,
                                       Paradigm::PTHREAD,
                                       *threadRootStr,
                                       PEARL_NO_NUM,
                                       PEARL_NO_NUM);
        addRegion(mThreadRootRegion);
    }

    // Reset call tree modification flag
    mCallTree->setModified(false);
}


// --- Set internal regions -------------------------------------------------

void
MutableGlobalDefs::setPausingRegion(Region* const region)
{
    if (mPausingRegion)
    {
        throw FatalError("Duplicate definition of PAUSING region!");
    }

    mPausingRegion = region;
}


void
MutableGlobalDefs::setFlushingRegion(Region* const region)
{
    if (mFlushingRegion)
    {
        throw FatalError("Duplicate definition of FLUSHING region!");
    }

    mFlushingRegion = region;
}


void
MutableGlobalDefs::setTaskRootRegion(Region* const region)
{
    if (mTaskRootRegion)
    {
        throw FatalError("Duplicate definition of Task Root region!");
    }

    mTaskRootRegion = region;
}


void
MutableGlobalDefs::setThreadRootRegion(Region* const region)
{
    if (mThreadRootRegion)
    {
        throw FatalError("Duplicate definition of THREADS Root region!");
    }

    mThreadRootRegion = region;
}


// --- Call tree handling ---------------------------------------------------

CallTree*
MutableGlobalDefs::getCallTree() const
{
    return mCallTree.get();
}


void
MutableGlobalDefs::setCallTree(CallTree* const callTree)
{
    if (callTree == mCallTree.get())
    {
        return;
    }

    mCallTree.reset(callTree);
}


// --- Global time offset handling ------------------------------------------

timestamp_t
MutableGlobalDefs::getGlobalOffset() const
{
    return mGlobalOffset;
}


void
MutableGlobalDefs::setGlobalOffset(const timestamp_t offset)
{
    mGlobalOffset = offset;
}


// --- Private methods ------------------------------------------------------

/// @brief Track definition type and optimize lookups for previous type.
///
/// Tracks the provided type @p currDefType of the current definition.  If
/// it differs from the type of the definition seen previously, optimizes
/// item lookups by global identifier for objects of the previous type.
///
/// @param currDefType
///     Type of the current definition
///
/// @exception_nothrow
///
void
MutableGlobalDefs::trackType(const DefinitionType currDefType)
{
    if (currDefType != mLastDefType)
    {
        // Optimize lookups for definitions of the previous type
        switch (mLastDefType)
        {
            // This is the initial value, i.e., we get here only for the first
            // definition object added.  In this case, there is nothing to be
            // optimized yet.
            case UNDEFINED:
                break;

            case CALLINGCONTEXT:
                mContextTree->optimizeLookups();
                break;

            // Call trees don't use the DefinitionTree class yet...
            case CALLPATH:
                break;

            case CALLSITE:
                mCallsites->optimizeLookups();
                break;

            case CARTDIMENSION:
                mCartDimensions->optimizeLookups();
                break;

            case CARTTOPOLOGY:
                mCartTopologies->optimizeLookups();
                break;

            case COMMUNICATOR:
                mCommunicators->optimizeLookups();
                break;

            case GROUPINGSET:
                mGroupingSets->optimizeLookups();
                break;

            case LOCATION:
                mLocations->optimizeLookups();
                break;

            case LOCATIONGROUP:
                mLocationGroups->optimizeLookups();
                break;

            case METRIC:
                mMetrics->optimizeLookups();
                break;

            case REGION:
                mRegions->optimizeLookups();
                break;

            case RMAWINDOW:
                mRmaWindows->optimizeLookups();
                break;

            case SOURCELOCATION:
                mSourceLocations->optimizeLookups();
                break;

            case STRING:
                mStrings->optimizeLookups();
                break;

            case SYSTEMNODE:
                mSystemTree->optimizeLookups();
                break;

            default:
                UTILS_BUG("Unknown definition type");
        }

        // Track definition type
        mLastDefType = currDefType;
    }
}
