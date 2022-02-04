/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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

#include "TimedPhase.h"

#include <inttypes.h>

#define SCALASCA_DEBUG_MODULE_NAME    SCOUT
#include <UTILS_Debug.h>

#include <pearl/Utils.h>

#include "Logging.h"

using namespace std;
using namespace pearl;
using namespace scout;


// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Creates a new timed phase.
 *
 *  Creates a new timed phase which prints the given log @a message when
 *  it is started.
 *
 *  @param  message    Log message to be printed when phase starts
 *  @param  logLevel   Minimum verbosity level at which @a message is printed
 *  @param  showSteps  Set to @b true if step numbers should be shown as
 *                     part of the message
 **/
TimedPhase::TimedPhase(const string& message,
                       const int     logLevel,
                       const bool    showSteps)
    : CompoundTask(),
      m_message(message),
      m_logLevel(logLevel),
      m_showSteps(showSteps)
{
}


// --- TimedPhase wrapping method -------------------------------------------

/**
 *  @brief Creates a timed phase.
 *
 *  This method creates a new timed phase of a single task
 *  by wrapping the given @a task.
 *
 *  @param  task       Task to be wrapped
 *  @param  message    Log message to be printed when phase starts
 *  @param  logLevel   Minimum verbosity level at which @a message is printed
 *  @param  showSteps  Set to @b true if step numbers should be shown as
 *                     part of the message
 *
 *  @return %TimedPhase instance
 **/
TimedPhase*
TimedPhase::make_timed(pearl::Task* const task,
                       const std::string& message,
                       const int          logLevel,
                       const bool         showSteps)
{
    TimedPhase* phase = new TimedPhase(message, logLevel, showSteps);
    phase->add_task(task);

    return phase;
}


// --- Execution control (protected) ----------------------------------------

/**
 *  @brief Task preparation.
 *
 *  The prepare() callback method is called before execution of the subtasks
 *  starts. In case of a timed phase, it will print the initial log message
 *  provided on phase creation and start the timer.
 *
 *  @return Always @em true
 **/
bool
TimedPhase::prepare()
{
    UTILS_DEBUG_ONLY(                                                    \
        m_message = m_message.substr(m_message.find_first_not_of('\n')); \
    )

    LogMsg(m_logLevel, "%s ", m_message.c_str());
    if (m_showSteps)
    {
        CompoundTask* parent = get_parent();
        if (parent)
        {
            uint32_t numSteps    = parent->num_steps();
            uint32_t currentStep = parent->current_step();
            LogMsg(m_logLevel, "(%" PRIu32 "/%" PRIu32 ") ", currentStep, numSteps);
        }
    }
    LogMsg(m_logLevel, "... ");
    m_timer.start();

    UTILS_DEBUG_ONLY(             \
        LogMsg(m_logLevel, "\n"); \
    )
    UTILS_DLOG << "== PHASE BEGIN == " << trim(m_message);

    return true;
}


/**
 *  @brief Task clean up.
 *
 *  The finish() callback method is called after the execution of the subtasks
 *  finishes. In case of a timed phase, it will stop the timer and print the
 *  "done" message, followed by the wall-clock duration of the phase.
 *
 *  @return Always @em true
 **/
bool
TimedPhase::finish()
{
    UTILS_DLOG << "== PHASE END == " << trim(m_message);

    m_timer.stop();
    LogMsg(m_logLevel, "done (%.3fs).\n", m_timer.value());

    return true;
}
