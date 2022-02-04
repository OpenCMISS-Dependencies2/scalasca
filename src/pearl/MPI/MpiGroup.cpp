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
 *  @brief   Implementation of the class MpiGroup.
 *
 *  This file provides the implementation of the class MpiGroup.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/MpiGroup.h>

using namespace std;
using namespace pearl;


// --- Constructors & destructor --------------------------------------------

MpiGroup::MpiGroup(const IdType              id,
                   const String&             name,
                   const Properties&         properties,
                   const CommLocationSet&    commLocationSet,
                   const vector< uint32_t >& ranks)
    : CommSet(id, name, properties, commLocationSet, ranks),
      mHandle(MPI_GROUP_NULL)
{
}


MpiGroup::~MpiGroup()
{
    if (  (mHandle != MPI_GROUP_NULL)
       && (mHandle != MPI_GROUP_EMPTY))
    {
        MPI_Group_free(&mHandle);
    }
}


// --- Query functions ------------------------------------------------------

MPI_Group
MpiGroup::getHandle()
{
    // Lazy generation of MPI group handle at first access
    if (mHandle == MPI_GROUP_NULL)
    {
        mHandle = createHandle();
    }

    return mHandle;
}


MPI_Group
MpiGroup::createHandle() const
{
    MPI_Group handle;

    // Special case 1: GROUP_EMPTY
    const uint32_t size = numRanks();
    if (size == 0)
    {
        handle = MPI_GROUP_EMPTY;
    }

    // Special case 2: GROUP_SELF
    else if (getProperties().test(GroupingSet::PROPERTY_SELF))
    {
        MPI_Comm_group(MPI_COMM_SELF, &handle);
    }

    // Special case 3: GROUP_WORLD
    else if (getProperties().test(GroupingSet::PROPERTY_WORLD))
    {
        MPI_Comm_group(MPI_COMM_WORLD, &handle);
    }

    // Regular case: arbitrary group with size > 0
    else
    {
        // Set up ranks array
        vector< int > ranks;
        ranks.reserve(size);
        for (uint32_t rank = 0; rank < size; ++rank)
        {
            ranks.push_back(getGlobalRank(rank));
        }

        // Create group based on world group
        MPI_Group world_group;
        MPI_Comm_group(MPI_COMM_WORLD, &world_group);
        MPI_Group_incl(world_group, size, &ranks[0], &handle);
        MPI_Group_free(&world_group);
    }

    return handle;
}
