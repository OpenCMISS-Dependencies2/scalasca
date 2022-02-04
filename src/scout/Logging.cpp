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


#include <config.h>

#include "Logging.h"

#include <cstdarg>
#include <cstdio>

using namespace std;
using namespace scout;


// --- Global variables -----------------------------------------------------

namespace
{
/// Global verbosity level
int logVerbosity = 0;
}    // unnamed namespace


// --- Logging functions ----------------------------------------------------

/**
 *  @brief Selectively prints a log message.
 *
 *  Prints the given log @a message to @c stdout if @a level is less than or
 *  equal to the current verbosity level. Note that in the case of OpenMP,
 *  only messages from the master thread are displayed.
 *
 *  @param  level    Minimal verbosity level at which the message should be
 *                   displayed
 *  @param  message  Log message (format string, printf style)
 *  @param  ...      Variable argument list according to log message
 **/
void
scout::LogMsg(int         level,
              const char* message,
              ...)
{
    // Initialize variable argument list
    va_list ap;
    va_start(ap, message);

    #pragma omp master
    {
        // Display log message on master thread
        if (logVerbosity >= level)
        {
            vprintf(message, ap);
            fflush(stdout);
        }
    }

    va_end(ap);
}


/**
 *  @brief Gets the verbosity level.
 *
 *  Returns the current verbosity level.
 *
 *  @return Verbosity level
 **/
int
scout::LogGetVerbosity()
{
    return logVerbosity;
}


/**
 *  @brief Set the verbosity level.
 *
 *  Sets the verbosity level to the given value @a level.
 *
 *  @param  level  New verbosity level
 **/
void
scout::LogSetVerbosity(int level)
{
    logVerbosity = level;
}
