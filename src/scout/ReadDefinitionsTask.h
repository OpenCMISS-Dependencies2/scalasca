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


#ifndef SCOUT_READDEFINITIONSTASK_H
#define SCOUT_READDEFINITIONSTASK_H


#include <pearl/Task.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file  ReadDefinitionsTask.h
 *  @brief Declaration of the ReadDefinitionsTask class.
 *
 *  This header file provides the declaration of the ReadDefinitionsTask
 *  class which is used to read in the global definition data.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Forward declarations -------------------------------------------------

struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 *  @class ReadDefinitionsTask
 *  @brief Task reading the global definition data.
 *
 *  The ReadDefinitionsTask class is used to read the global definition data
 *  of an experiment.
 **/
/*-------------------------------------------------------------------------*/

class ReadDefinitionsTask
    : public pearl::Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        ReadDefinitionsTask(TaskDataShared& sharedData);

        /// @}
        /// @name Execution control
        /// @{

        virtual bool
        execute();

        /// @}


    private:
        /// Shared task data object
        TaskDataShared& mSharedData;
};
}    // namespace scout


#endif    // !SCOUT_READDEFINITIONSTASK_H
