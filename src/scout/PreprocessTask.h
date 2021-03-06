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


#ifndef SCOUT_PREPROCESSTASK_H
#define SCOUT_PREPROCESSTASK_H


#include <pearl/CompoundTask.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file  PreprocessTask.h
 *  @brief Declaration of the PreprocessTask class.
 *
 *  This header file provides the declaration of the PreprocessTask class
 *  which performs all necessary preprocessing steps before the analysis can
 *  take place.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Forward declarations -------------------------------------------------

struct TaskDataPrivate;
struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 *  @class PreprocessTask
 *  @brief Compound task combining individual preprocessing tasks.
 *
 *  This class abstracts away the details of the preprocessing phase by
 *  providing a single, convenient interface to a potentially larger number
 *  of individual preprocessing tasks.
 **/
/*-------------------------------------------------------------------------*/

class PreprocessTask
    : public pearl::CompoundTask
{
    public:
        /// @name Constructors & destructor
        /// @{

        PreprocessTask(const TaskDataShared&  sharedData,
                       const TaskDataPrivate& privateData);

        /// @}
};
}    // namespace scout


#endif    // !SCOUT_PREPROCESSTASK_H
