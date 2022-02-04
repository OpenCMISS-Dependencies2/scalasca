/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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

#include "TaskData.h"

#include <cstddef>

#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/TraceArchive.h>

using namespace std;
using namespace pearl;
using namespace scout;


/*-------------------------------------------------------------------------*/
/**
 *  @file  TaskData.cpp
 *  @brief Implementation of the TaskDataShared and TaskDataPrivate structs.
 *
 *  This file provides the implementation of the TaskDataShared and
 *  TastDataPrivate structs used to transfer data between the individual
 *  tasks of the SCOUT trace analyzer.
 **/
/*-------------------------------------------------------------------------*/


// --- Constructors & destructor (TaskDataShared) ---------------------------

TaskDataShared::TaskDataShared()
    : mArchive(NULL),
      mDefinitions(NULL)
{
}


TaskDataShared::~TaskDataShared()
{
    delete mArchive;
    delete mDefinitions;
}


// --- Constructors & destructor (TaskDataPrivate) --------------------------

TaskDataPrivate::TaskDataPrivate()
    : mIdMaps(NULL),
      mTrace(NULL),
      mSynchronizer(NULL)
{
}


TaskDataPrivate::~TaskDataPrivate()
{
    delete mTrace;
}
