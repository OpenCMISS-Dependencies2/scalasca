/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_ANALYZETASK_H
#define SCOUT_ANALYZETASK_H


#include <pearl/CompoundTask.h>

#include "CbData.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file  AnalyzeTask.h
 *  @brief Declaration of the AnalyzeTask class.
 *
 *  This header file provides the declaration of the AnalyzeTask class which
 *  combines individual trace analysis tasks.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Forward declarations -------------------------------------------------

class AnalysisHandler;
class AnalysisReplayTask;
class Pattern;
class StatisticsTask;

struct ReportData;
struct TaskDataPrivate;
struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 *  @class AnalyzeTask
 *  @brief Compound task combining individual trace-analysis tasks.
 *
 *  This class abstracts away the details of the trace-analysis phase by
 *  providing a single, convenient interface to a potentially larger number
 *  of individual trace-analysis tasks.
 **/
/*-------------------------------------------------------------------------*/

class AnalyzeTask
    : public pearl::CompoundTask
{
    public:
        /// @name Constructors & destructor
        /// @{

        AnalyzeTask(const TaskDataShared&  sharedData,
                    const TaskDataPrivate& privateData);

        ~AnalyzeTask();

        /// @}
        /// @name Adding performance patterns
        /// @{

        void
        addPattern(Pattern* pattern);

        /// @}
        /// @name Register statistics task
        /// @{

        void
        register_statistics_cbs(StatisticsTask* sttask);

        /// @}
        /// @name Retrieve report data
        /// @{

        void
        genCubeMetrics(ReportData& data) const;

        void
        genCubeSeverities(ReportData& data,
                          int         rank) const;

        /// @}


    protected:
        /// @name Execution control
        /// @{

        virtual bool
        prepare();

        virtual bool
        finish();

        /// @}


    private:
        /// Container type for pattern objects
        typedef std::vector< Pattern* >         PatternList;
        typedef std::vector< AnalysisHandler* > HandlerList;

        /// Shared task data object
        const TaskDataShared& mSharedData;

        /// Private task data object
        const TaskDataPrivate& mPrivateData;

        /// Callback manager map
        CallbackManagerMap mCallbackManagers;

        /// Callback data object passed to the individual analysis tasks
        CbData mCallbackData;

        /// List of Patterns
        PatternList mPatterns;

        /// List of Handlers
        HandlerList mHandlers;

        /// Pointer to the statistics task object
        StatisticsTask* mStatistics;


        friend class AnalysisReplayTask;
};
}    // namespace scout


#endif    // !SCOUT_ANALYZETASK_H
