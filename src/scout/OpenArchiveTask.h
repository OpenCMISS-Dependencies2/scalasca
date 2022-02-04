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


#ifndef SCOUT_OPENARCHIVETASK_H
#define SCOUT_OPENARCHIVETASK_H


#include <string>

#include <pearl/Task.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file  OpenArchiveTask.h
 *  @brief Declaration of the OpenArchiveTask class.
 *
 *  This header file provides the declaration of the OpenArchiveTask
 *  class which is used to open an event trace experiment archive.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Forward declarations -------------------------------------------------

struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 *  @class OpenArchiveTask
 *  @brief Task opening the experiment archive.
 *
 *  The OpenArchiveTask class is used to open an event trace experiment
 *  archive.
 **/
/*-------------------------------------------------------------------------*/

class OpenArchiveTask
    : public pearl::Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        OpenArchiveTask(const std::string& archiveName,
                        TaskDataShared&    sharedData);

        /// @}
        /// @name Execution control
        /// @{

        virtual bool
        execute();

        /// @}


    private:
        /// Archive name
        std::string mArchiveName;

        /// Shared task data object
        TaskDataShared& mSharedData;
};
}    // namespace scout


#endif    // !SCOUT_OPENARCHIVETASK_H
