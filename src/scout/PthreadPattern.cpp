/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2019                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "PthreadPattern.h"

#include <pearl/Callpath.h>
#include <pearl/Region.h>

using namespace pearl;
using namespace scout;


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Implementation of the PthreadPattern class.
 *
 *  This file provides the implementation of the PthreadPattern class which
 *  serves as an abstract base class for all Pthread-specific performance
 *  properties calculated by SCOUT.
 **/
/*-------------------------------------------------------------------------*/


// --- Data collation control -----------------------------------------------

bool
PthreadPattern::skip_cnode(const pearl::Callpath& cnode) const
{
    return !is_pthread_api(cnode.getRegion());
}
