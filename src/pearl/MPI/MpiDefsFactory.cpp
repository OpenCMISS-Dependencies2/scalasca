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
 *  @ingroup PEARL_mpi
 *  @brief   Implementation of the class MpiDefsFactory.
 *
 *  This file provides the implementation of the class MpiDefsFactory.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "MpiDefsFactory.h"

#include <algorithm>

#include <mpi.h>

#include <pearl/Error.h>
#include <pearl/MpiComm.h>
#include <pearl/MpiGroup.h>
#include <pearl/MpiWindow.h>
#include <pearl/Paradigm.h>
#include <pearl/ScopedPtr.h>
#include <pearl/Utils.h>

#include "MutableGlobalDefs.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Generic factory methods ----------------------------------------------

void
MpiDefsFactory::createCommunicator(MutableGlobalDefs& defs,
                                   const uint32_t     id,
                                   const uint32_t     nameId,
                                   const uint32_t     commSetId,
                                   const uint32_t     parentId) const
{
    // ID consistency check
    if (  (parentId != Communicator::NO_ID)
       && (id <= parentId))
    {
        throw FatalError("pearl::detail::MpiDefsFactory::createCommunicator -- "
                         "ID ("
                         + toStdString(id)
                         + ") <= parent ID ("
                         + toStdString(parentId)
                         + ")");
    }

    const GroupingSet& groupingSet = defs.getGroupingSet(commSetId);
    if (groupingSet.getType() != GroupingSet::COMM_SET)
    {
        throw FatalError("pearl::detail::MpiDefsFactory::createCommunicator -- "
                         "Grouping set "
                         + toStdString(commSetId)
                         + " is not a communication set");
    }

    const CommSet& commSet  = static_cast< const CommSet& >(groupingSet);
    const Paradigm paradigm = commSet.getParadigm();

    if (paradigm == Paradigm::MPI)
    {
        const MpiGroup& group = static_cast< const MpiGroup& >(commSet);

        const Communicator& parentComm     = defs.getCommunicator(parentId);
        const Paradigm      parentParadigm = parentComm.getParadigm();
        if (  (parentComm != Communicator::UNDEFINED)
           && (parentParadigm != Paradigm::MPI))
        {
            throw FatalError("pearl::detail::MpiDefsFactory::createCommunicator -- "
                             "Paradigm mismatch: "
                             + toString(paradigm)
                             + " (communication set) vs. "
                             + toString(parentParadigm)
                             + " (parent)");
        }
        const MpiComm& parent = static_cast< const MpiComm& >(parentComm);

        ScopedPtr< Communicator > comm;
        comm.reset(new MpiComm(id,
                               defs.getString(nameId),
                               group,
                               (parent != Communicator::UNDEFINED)
                               ? const_cast< MpiComm* >(&parent)
                               : 0));
        defs.addCommunicator(comm.get());
        comm.release();
    }
    else
    {
        DefsFactory::createCommunicator(defs, id, nameId, commSetId, parentId);
    }
}


void
MpiDefsFactory::createGroupingSet(MutableGlobalDefs&             defs,
                                  const uint32_t                 id,
                                  const uint32_t                 nameId,
                                  const GroupingSet::SetType     type,
                                  const Paradigm                 paradigm,
                                  const GroupingSet::Properties& properties,
                                  const uint32_t                 numMembers,
                                  const uint64_t* const          members) const
{
    if (  (type == GroupingSet::COMM_SET)
       && (paradigm == Paradigm::MPI))
    {
        vector< uint32_t > ranks;

        // Special case 1: self-like groups
        if (properties.test(GroupingSet::PROPERTY_SELF))
        {
            int rank;
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);

            ranks.push_back(rank);
        }

        // Special case 2: world-like groups => empty ranks container;
        // Handle the general case instead
        else if (!properties.test(GroupingSet::PROPERTY_WORLD))
        {
            ranks.reserve(numMembers);
            copy(members, members + numMembers, back_inserter(ranks));
        }

        ScopedPtr< MpiGroup > group;
        group.reset(new MpiGroup(id,
                                 defs.getString(nameId),
                                 properties,
                                 defs.getCommLocationSet(paradigm),
                                 ranks));

        // Sanity check
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        if (static_cast< uint32_t >(world_size) < group->numRanks())
        {
            throw FatalError("pearl::detail::MpiDefsFactory::createGroupingSet() -- "
                             "Experiment requires at least "
                             + toStdString(defs.numLocationGroups())
                             + " MPI ranks.");
        }

        defs.addGroupingSet(group.get());
        group.release();
    }
    else
    {
        DefsFactory::createGroupingSet(defs,
                                       id,
                                       nameId,
                                       type,
                                       paradigm,
                                       properties,
                                       numMembers,
                                       members);
    }
}


// --- MPI-specific factory methods -----------------------------------------

void
MpiDefsFactory::createMpiWindow(MutableGlobalDefs& defs,
                                ident_t            id,
                                ident_t            comm_id) const
{
    // Determine communicator
    MpiComm* comm = dynamic_cast< MpiComm* >(defs.get_comm(comm_id));
    if (!comm)
    {
        throw RuntimeError("pearl::detail::MpiDefsFactory::createMpiWindow() -- "
                           "Invalid MPI communicator ID: " + toStdString(comm_id));
    }

    // Create & store new MPI RMA window
    defs.addRmaWindow(new MpiWindow(id, comm));
}
