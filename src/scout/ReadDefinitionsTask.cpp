/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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

#include "ReadDefinitionsTask.h"

#include <cstddef>

#include <pearl/TraceArchive.h>

#include "TaskData.h"

using namespace std;
using namespace pearl;
using namespace scout;


// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new ReadDefinitionsTask instance.
 *
 *  This constructor creates a new ReadDefinitionsTask instance with the
 *  given parameters.
 *
 *  @param  sharedData  Shared task data object
 **/
ReadDefinitionsTask::ReadDefinitionsTask(TaskDataShared& sharedData)
    : Task(),
      mSharedData(sharedData)
{
}


// --- Execution control ----------------------------------------------------

/**
 *  @brief Executes the task.
 *
 *  Initializes the global definitions object by reading the global definition
 *  data from the experiment archive.
 *
 *  @return Returns @em true if successful, @em false otherwise
 **/
bool
ReadDefinitionsTask::execute()
{
    // Read definitions
    mSharedData.mDefinitions = mSharedData.mArchive->getDefinitions();

    return (mSharedData.mDefinitions != NULL);
}
