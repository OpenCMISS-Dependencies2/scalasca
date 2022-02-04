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


#ifndef SCOUT_READTRACETASK_H
#define SCOUT_READTRACETASK_H


#include <pearl/CompoundTask.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file  ReadTraceTask.h
 *  @brief Declaration of the ReadTraceTask class.
 *
 *  This header file provides the declaration of the ReadTraceTask class
 *  which is used to read in the trace data per location.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Forward declarations -------------------------------------------------

struct TaskDataPrivate;
struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 *  @class ReadTraceTask
 *  @brief Task used to read the per-location trace data.
 *
 *  The ReadTraceTask class is used to read the entire trace data which is
 *  stored per location into memory.
 **/
/*-------------------------------------------------------------------------*/

class ReadTraceTask
    : public pearl::CompoundTask
{
    public:
        /// @name Constructors & destructor
        /// @{

        ReadTraceTask(const TaskDataShared& sharedData,
                      TaskDataPrivate&      privateData);

        /// @}
};
}    // namespace scout


#endif    // !SCOUT_READTRACETASK_H
