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


#ifndef SCOUT_TASKDATA_H
#define SCOUT_TASKDATA_H


/*-------------------------------------------------------------------------*/
/**
 *  @file  TaskData.h
 *  @brief Declaration of the TaskDataShared and TaskDataPrivate structs.
 *
 *  This header file provides the declaration of the TaskDataShared and
 *  TastDataPrivate structs used to transfer data between the individual
 *  tasks of the SCOUT trace analyzer.
 **/
/*-------------------------------------------------------------------------*/


// --- Forward declarations -------------------------------------------------

class Synchronizer;


namespace pearl
{
class GlobalDefs;
class LocalIdMaps;
class LocalTrace;
class TraceArchive;
}    // namespace pearl


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 *  @struct TaskDataShared
 *  @brief  SCOUT task data shared between analysis threads.
 *
 *  This structure groups all the data transferred between the individual
 *  tasks of the SCOUT trace analyzer which is shared between all analysis
 *  threads in a multi-threaded environment. In particular, this includes
 *  the trace experiment archive abstraction and the global definitions
 *  object.
 **/
/*-------------------------------------------------------------------------*/

struct TaskDataShared
{
    // --- Public methods ----------------------------------

    /// @name Constructors & destructor
    /// @{

    /// @brief Default constructor.
    ///
    /// Creates a new instance and initializes all pointer attributes with
    /// NULL.
    ///
    TaskDataShared();

    /// @brief Destructor.
    ///
    /// Releases all allocated resources (including the objects referenced by
    /// the stored pointers) and destroys the instance.
    ///
    ~TaskDataShared();

    /// @}


    // --- Data members ------------------------------------

    /// Experiment archive object
    pearl::TraceArchive* mArchive;

    /// Global definitions object
    pearl::GlobalDefs* mDefinitions;
};


/*-------------------------------------------------------------------------*/
/**
 *  @struct TaskDataPrivate
 *  @brief  SCOUT task data private to each analysis thread.
 *
 *  This structure groups all the data transferred between the individual
 *  tasks of the SCOUT trace analyzer which is private to each analysis
 *  thread in a multi-threaded environment. In particular, this includes
 *  the local trace data object.
 **/
/*-------------------------------------------------------------------------*/

struct TaskDataPrivate
{
    // --- Public methods ----------------------------------

    /// @name Constructors & destructor
    /// @{

    /// @brief Default constructor.
    ///
    /// Creates a new instance and initializes all pointer attributes with
    /// NULL.
    ///
    TaskDataPrivate();

    /// @brief Destructor.
    ///
    /// Releases all allocated resources (including the objects referenced by
    /// the stored pointers) and destroys the instance.
    ///
    ~TaskDataPrivate();

    /// @}


    // --- Data members ------------------------------------

    /// Local-to-global definition identifier mapping object
    pearl::LocalIdMaps* mIdMaps;

    /// Local trace data object
    pearl::LocalTrace* mTrace;

    /// Timestamp synchronization object
    Synchronizer* mSynchronizer;
};
}    // namespace scout


#endif    // !SCOUT_TASKDATA_H
