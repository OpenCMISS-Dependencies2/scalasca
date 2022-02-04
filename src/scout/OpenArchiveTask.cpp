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

#include "OpenArchiveTask.h"

#include <cstddef>

#include <pearl/TraceArchive.h>

#include "TaskData.h"

using namespace std;
using namespace pearl;
using namespace scout;


// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new OpenArchiveTask instance.
 *
 *  This constructor creates a new OpenArchiveTask instance with the given
 *  parameters.
 *
 *  @param  archiveName  Name of the experiment archive
 *  @param  sharedData   Shared task data object
 **/
OpenArchiveTask::OpenArchiveTask(const string&   archiveName,
                                 TaskDataShared& sharedData)
    : Task(),
      mArchiveName(archiveName),
      mSharedData(sharedData)
{
}


// --- Execution control ----------------------------------------------------

/**
 *  @brief Executes the task.
 *
 *  Opens the experiment archive.
 *
 *  @return Returns @em true if successful, @em false otherwise
 **/
bool
OpenArchiveTask::execute()
{
    // Open experiment archive
    mSharedData.mArchive = TraceArchive::open(mArchiveName);

    return (mSharedData.mArchive != NULL);
}
