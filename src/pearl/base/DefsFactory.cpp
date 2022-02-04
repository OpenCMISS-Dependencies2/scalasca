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
 *  @brief   Implementation of the class DefsFactory.
 *
 *  This file provides the implementation of the class DefsFactory.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "DefsFactory.h"

#include <algorithm>
#include <cassert>
#include <cstddef>

#include <pearl/CallingContext.h>
#include <pearl/Callpath.h>
#include <pearl/Callsite.h>
#include <pearl/CommLocationSet.h>
#include <pearl/CommSet.h>
#include <pearl/Communicator.h>
#include <pearl/Error.h>
#include <pearl/Location.h>
#include <pearl/LocationSet.h>
#include <pearl/Metric.h>
#include <pearl/Paradigm.h>
#include <pearl/RmaWindow.h>
#include <pearl/ScopedPtr.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>
#include <pearl/Utils.h>

#include "MutableCartTopology.h"
#include "MutableGlobalDefs.h"
#include "Process.h"
#include "UnknownLocationGroup.h"
#include "elg_defs.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constants ------------------------------------------------------------

// For Metric objects
const string str_integer("INTEGER");
const string str_counter("COUNTER");
const string str_start("START");


// --- Static member variables ----------------------------------------------

const DefsFactory* DefsFactory::m_instance = NULL;


// --- Constructors & destructor --------------------------------------------

DefsFactory::~DefsFactory()
{
}


// --- Generic factory methods ----------------------------------------------

void
DefsFactory::createCallingContext(MutableGlobalDefs& defs,
                                  const uint32_t     id,
                                  const uint32_t     regionId,
                                  const uint32_t     sclId,
                                  const uint32_t     parentId) const
{
    // ID consistency check
    if (  (parentId != CallingContext::NO_ID)
       && (id <= parentId))
    {
        throw FatalError("pearl::detail::DefsFactory::createCallingContext -- "
                         "ID ("
                         + toStdString(id)
                         + ") <= parent ID ("
                         + toStdString(parentId)
                         + ")");
    }

    const CallingContext& parent = defs.getCallingContext(parentId);
    defs.addCallingContext(new CallingContext(id,
                                              defs.getRegion(regionId),
                                              defs.getSourceLocation(sclId),
                                              (parent != CallingContext::UNDEFINED)
                                              ? const_cast< CallingContext* >(&parent)
                                              : 0));
}


void
DefsFactory::createCallpath(MutableGlobalDefs& defs,
                            const uint32_t     id,
                            const uint32_t     regionId,
                            const uint32_t     callsiteId,
                            const uint32_t     parentId) const
{
    // ID consistency check
    if (id != defs.numCallpaths())
    {
        throw FatalError("pearl::detail::DefsFactory::createCallpath -- "
                         "invalid ID: " + toStdString(id));
    }

    defs.addCallpath(new Callpath(id,
                                  defs.getRegion(regionId),
                                  defs.getCallsite(callsiteId),
                                  defs.get_cnode(parentId)));
}


void
DefsFactory::createCallsite(MutableGlobalDefs& defs,
                            const uint32_t     id,
                            const uint32_t     filenameId,
                            const uint32_t     line,
                            const uint32_t     regionId) const
{
    defs.addCallsite(new Callsite(id,
                                  defs.getString(filenameId),
                                  line,
                                  defs.getRegion(regionId)));
}


void
DefsFactory::createCartDimension(MutableGlobalDefs&               defs,
                                 const uint32_t                   id,
                                 const uint32_t                   nameId,
                                 const uint32_t                   size,
                                 const CartDimension::Periodicity periodicity) const
{
    defs.addCartDimension(new CartDimension(id,
                                            defs.getString(nameId),
                                            size,
                                            periodicity));
}


void
DefsFactory::createCartTopology(MutableGlobalDefs&    defs,
                                const uint32_t        id,
                                const uint32_t        nameId,
                                const uint32_t        commId,
                                const uint8_t         numDimensions,
                                const uint32_t* const dimensionIds) const
{
    CartTopology::DimensionContainer dimensions;
    dimensions.reserve(numDimensions);
    for (uint8_t i = 0; i < numDimensions; ++i)
    {
        const CartDimension& dim = defs.getCartDimension(dimensionIds[i]);
        dimensions.push_back(&dim);
    }

    defs.addCartTopology(new MutableCartTopology(id,
                                                 defs.getString(nameId),
                                                 defs.getCommunicator(commId),
                                                 dimensions));
}


void
DefsFactory::createCommunicator(MutableGlobalDefs& defs,
                                const uint32_t     id,
                                const uint32_t     nameId,
                                const uint32_t     commSetId,
                                const uint32_t     parentId) const
{
    // ID consistency check
    if (  (parentId != Communicator::NO_ID)
       && (id <= parentId))
    {
        throw FatalError("pearl::detail::DefsFactory::createCommunicator -- "
                         "ID ("
                         + toStdString(id)
                         + ") <= parent ID ("
                         + toStdString(parentId)
                         + ")");
    }

    const GroupingSet& groupingSet = defs.getGroupingSet(commSetId);
    if (groupingSet.getType() != GroupingSet::COMM_SET)
    {
        throw FatalError("pearl::detail::DefsFactory::createCommunicator -- "
                         "Grouping set "
                         + toStdString(commSetId)
                         + " is not a communication set");
    }

    const CommSet&      commSet = static_cast< const CommSet& >(groupingSet);
    const Communicator& parent  = defs.getCommunicator(parentId);

    if (  (parent != Communicator::UNDEFINED)
       && (parent.getParadigm() != commSet.getParadigm()))
    {
        throw FatalError("pearl::detail::DefsFactory::createCommunicator -- "
                         "Paradigm mismatch: "
                         + toString(commSet.getParadigm())
                         + " (communication set) vs. "
                         + toString(parent.getParadigm())
                         + " (parent)");
    }

    ScopedPtr< Communicator > comm;
    comm.reset(new Communicator(id,
                                defs.getString(nameId),
                                commSet,
                                (parent != Communicator::UNDEFINED)
                                ? const_cast< Communicator* >(&parent)
                                : 0));
    defs.addCommunicator(comm.get());
    comm.release();
}


void
DefsFactory::createGroupingSet(MutableGlobalDefs&             defs,
                               const uint32_t                 id,
                               const uint32_t                 nameId,
                               const GroupingSet::SetType     type,
                               const Paradigm                 paradigm,
                               const GroupingSet::Properties& properties,
                               const uint32_t                 numMembers,
                               const uint64_t* const          members) const
{
    ScopedPtr< GroupingSet > set;
    switch (type)
    {
        case GroupingSet::LOCATION_SET:
            {
                vector< const Location* > locations;
                locations.reserve(numMembers);
                for (uint32_t rank = 0; rank < numMembers; ++rank)
                {
                    locations.push_back(&defs.getLocation(members[rank]));
                }

                set.reset(new LocationSet(id,
                                          defs.getString(nameId),
                                          locations));
            }
            break;

        case GroupingSet::COMM_LOCATION_SET:
            {
                vector< const Location* > locations;
                locations.reserve(numMembers);
                for (uint32_t rank = 0; rank < numMembers; ++rank)
                {
                    const Location& location = defs.getLocation(members[rank]);

                    locations.push_back(&location);

                    if (paradigm == Paradigm::MPI)
                    {
                        Process* process = static_cast< Process* >(location.getParent());
                        process->setRank(rank);
                    }
                }

                set.reset(new CommLocationSet(id,
                                              defs.getString(nameId),
                                              paradigm,
                                              locations));
            }
            break;

        case GroupingSet::COMM_SET:
            {
                vector< uint32_t > ranks;

                // Special case 1: self-like groups
                if (properties.test(GroupingSet::PROPERTY_SELF))
                {
                    // In the serial case, self always refers to rank 0
                    ranks.reserve(1);
                    ranks.push_back(0);
                }

                // Special case 2: world-like groups => empty ranks container;
                // Handle the general case instead
                else if (!properties.test(GroupingSet::PROPERTY_WORLD))
                {
                    ranks.reserve(numMembers);
                    copy(members, members + numMembers, back_inserter(ranks));
                }

                set.reset(new CommSet(id,
                                      defs.getString(nameId),
                                      properties,
                                      defs.getCommLocationSet(paradigm),
                                      ranks));
            }
            break;

        default:
            throw FatalError("pearl::detail::DefsFactory::createGroupingSet -- "
                             "invalid type: " + toStdString(type));
            break;
    }

    defs.addGroupingSet(set.get());
    set.release();
}


void
DefsFactory::createLocation(MutableGlobalDefs&   defs,
                            const uint64_t       id,
                            const uint32_t       nameId,
                            const Location::Type type,
                            const uint64_t       numEvents,
                            const uint32_t       parentId) const
{
    const LocationGroup& lgroup = defs.getLocationGroup(parentId);
    defs.addLocation(new Location(id,
                                  defs.getString(nameId),
                                  type,
                                  numEvents,
                                  const_cast< LocationGroup* >(&lgroup)));
}


void
DefsFactory::createLocationGroup(MutableGlobalDefs&        defs,
                                 const uint32_t            id,
                                 const uint32_t            nameId,
                                 const LocationGroup::Type type,
                                 const uint32_t            parentId) const
{
    const SystemNode& systemNode = defs.getSystemNode(parentId);
    switch (type)
    {
        case LocationGroup::TYPE_UNKNOWN:
            defs.addLocationGroup(new UnknownLocationGroup(id,
                                                           defs.getString(nameId),
                                                           (systemNode != SystemNode::UNDEFINED)
                                                           ? const_cast< SystemNode* >(&systemNode)
                                                           : 0));
            break;

        case LocationGroup::TYPE_PROCESS:
            defs.addLocationGroup(new Process(id,
                                              defs.getString(nameId),
                                              (systemNode != SystemNode::UNDEFINED)
                                              ? const_cast< SystemNode* >(&systemNode)
                                              : 0));
            break;

        default:
            assert(false);
            break;
    }
}


void
DefsFactory::createRegion(MutableGlobalDefs&       defs,
                          const uint32_t           id,
                          const uint32_t           canonicalNameId,
                          const uint32_t           nameId,
                          const uint32_t           descriptionId,
                          const Region::Role       role,
                          const Paradigm           paradigm,
                          const uint32_t           filenameId,
                          const uint32_t           startLine,
                          const uint32_t           endLine,
                          const InternalRegionType type) const
{
    defs.addRegion(new Region(id,
                              defs.getString(canonicalNameId),
                              defs.getString(nameId),
                              defs.getString(descriptionId),
                              role,
                              paradigm,
                              defs.getString(filenameId),
                              startLine,
                              endLine));

    if (type != DefsFactory::USER_REGION)
    {
        const Region& region = defs.getRegion(id);
        switch (type)
        {
            case DefsFactory::PAUSING_REGION:
                defs.setPausingRegion(const_cast< Region* >(&region));
                break;

            case DefsFactory::FLUSHING_REGION:
                defs.setFlushingRegion(const_cast< Region* >(&region));
                break;

            case DefsFactory::TASKROOT_REGION:
                defs.setTaskRootRegion(const_cast< Region* >(&region));
                break;

            case DefsFactory::THREADROOT_REGION:
                defs.setThreadRootRegion(const_cast< Region* >(&region));
                break;

            default:
                break;
        }
    }
}


void
DefsFactory::createSourceLocation(MutableGlobalDefs& defs,
                                  const uint32_t     id,
                                  const uint32_t     filenameId,
                                  const uint32_t     line) const
{
    defs.addSourceLocation(new SourceLocation(id,
                                              defs.getString(filenameId),
                                              line));
}


void
DefsFactory::createString(MutableGlobalDefs& defs,
                          const uint32_t     id,
                          const string&      str) const
{
    defs.addString(new String(id, str));
}


void
DefsFactory::createSystemNode(MutableGlobalDefs& defs,
                              const uint32_t     id,
                              const uint32_t     nameId,
                              const uint32_t     classId,
                              const uint32_t     parentId) const
{
    if (  (parentId != SystemNode::NO_ID)
       && (id <= parentId))
    {
        throw FatalError("pearl::detail::DefsFactory::createSystemNode -- "
                         "ID ("
                         + toStdString(id)
                         + ") <= parent ID ("
                         + toStdString(parentId)
                         + ")");
    }

    const SystemNode& systemNode = defs.getSystemNode(parentId);
    defs.addSystemNode(new SystemNode(id,
                                      defs.getString(nameId),
                                      defs.getString(classId),
                                      (systemNode != SystemNode::UNDEFINED)
                                      ? const_cast< SystemNode* >(&systemNode)
                                      : 0));
}


void
DefsFactory::createMetric(MutableGlobalDefs& defs,
                          ident_t            id,
                          const string&      name,
                          const string&      descr,
                          uint8_t            type,
                          uint8_t            mode,
                          uint8_t            interval) const
{
    // Check parameter values
    if (type != ELG_INTEGER)
    {
        throw FatalError("Unsupported metric data type.");
    }
    if (mode != ELG_COUNTER)
    {
        throw FatalError("Unsupported metric mode.");
    }
    if (interval != ELG_START)
    {
        throw FatalError("Unsupported metric interval semantics.");
    }

    // Create & store new metric object
    defs.addMetric(new Metric(id,
                              name,
                              descr,
                              str_integer,
                              str_counter,
                              str_start));
}


// --- MPI-specific factory methods -----------------------------------------

void
DefsFactory::createMpiWindow(MutableGlobalDefs& defs,
                             ident_t            id,
                             ident_t            comm_id) const
{
    // Create & store new RMA window object
    defs.addRmaWindow(new RmaWindow(id, defs.get_comm(comm_id)));
}


// --- Singleton interface --------------------------------------------------

void
DefsFactory::registerFactory(const DefsFactory* factory)
{
    assert(factory);

    if (m_instance)
    {
        throw FatalError("pearl::detail::DefsFactory::registerFactory(const DefsFactory*) -- "
                         "Factory already registered.");
    }

    m_instance = factory;
}


void
DefsFactory::deregisterFactory()
{
    if (!m_instance)
    {
        throw FatalError("pearl::detail::DefsFactory::deregisterFactory() -- "
                         "No factory registered.");
    }

    delete m_instance;
    m_instance = 0;
}
