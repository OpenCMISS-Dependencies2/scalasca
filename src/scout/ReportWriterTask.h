/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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


#ifndef SCOUT_REPORTWRITERTASK_H
#define SCOUT_REPORTWRITERTASK_H


#include <string>

#include <pearl/Task.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Declaration of the ReportWriter class.
 *
 *  This header file provides the declaration of the ReportWriter class which
 *  writes the final analysis report to disk.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Forward declarations -------------------------------------------------

class AnalyzeTask;

struct ReportData;
struct TaskDataPrivate;
struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 *  @class ReportWriterTask
 *  @brief Task for writing the final analysis report to disk.
 *
 *  This class provides a pearl::Task object which is used to write the final
 *  trace analysis report to disk. This includes the generation of the common
 *  CUBE file header as well as the collation and writing of the distributed
 *  severity data.
 **/
/*-------------------------------------------------------------------------*/

class ReportWriterTask
    : public pearl::Task
{
    public:
        /// @name Constructors & destructor
        /// @{

        ReportWriterTask(const TaskDataShared&  sharedData,
                         const TaskDataPrivate& privateData,
                         const AnalyzeTask*     analyzer,
                         const std::string&     dirname,
                         int                    rank);

        /// @}
        /// @name Executing the task
        /// @{

        virtual bool
        execute();

        /// @}


    private:
        /// @name Report data generation
        /// @{

        void
        embed_specfile(ReportData& data) const;

        void
        gen_sysinfo(ReportData& data) const;

        void
        gen_regions(ReportData& data) const;

        void
        gen_cnodes(ReportData& data) const;

        void
        gen_cartesians(ReportData& data) const;

        /// @}


        /// Shared task data object
        const TaskDataShared& mSharedData;

        /// Private task data object
        const TaskDataPrivate& mPrivateData;

        /// Pointer to analysis task object to query data
        const AnalyzeTask* mAnalyzer;

        /// Experiment archive name
        std::string mDirname;

        /// MPI rank
        int mRank;
};
}    // namespace scout


#endif    // !SCOUT_REPORTWRITERTASK_H
