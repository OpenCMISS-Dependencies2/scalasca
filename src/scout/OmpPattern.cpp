/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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


#include <config.h>

#include "OmpPattern.h"

#include <pearl/Callpath.h>
#include <pearl/Region.h>

using namespace std;
using namespace pearl;
using namespace scout;


// --- Data collation control -----------------------------------------------

/**
 *  @brief Controls collation of data for the given call-tree node.
 *
 *  This method controls the collation of severity data for the given call-tree
 *  node @a cnode. The method returns @em true for all non-OpenMP call paths
 *  since it is guaranteed that no OpenMP-related severity data exists for them.
 *  For all other call paths, it returns @em false.
 *
 *  @param  cnode  Current call-tree node
 *  @return Returns @em true if @p cnode is an OpenMP call path, @em false
 *          otherwise
 **/
bool
OmpPattern::skip_cnode(const pearl::Callpath& cnode) const
{
    return !is_omp_api(cnode.getRegion());
}
