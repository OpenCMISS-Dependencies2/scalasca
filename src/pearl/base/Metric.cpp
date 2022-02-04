/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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

#include <pearl/Metric.h>

#include <pearl/Error.h>

using namespace std;
using namespace pearl;


// --- Constructors & destructor --------------------------------------------

/**
 *  Creates a new instance and initializes the data with the given values.
 *
 *  @param id          %Metric identifier
 *  @param name        %Metric name
 *  @param description Description text of the performance metric
 *  @param type        %Metric type, either "INTEGER" or "FLOAT"
 *  @param mode        %Metric mode, either "COUNTER", "RATE", or "SAMPLE"
 *  @param interval    Measurement interval semantics, either "START", "LAST",
 *                     "NEXT", or "NONE"
 *
 *  @exception FatalError if one of the parameters @a type, @a mode, or @a
 *                        interval contains an invalid value
 **/
Metric::Metric(const IdType  id,
               const string& name,
               const string& description,
               const string& type,
               const string& mode,
               const string& interval)
    : m_id(id),
      m_name(name),
      m_description(description),
      m_type(type),
      m_mode(mode),
      m_interval(interval)
{
    // Check parameter values
    if (  (type != "INTEGER")
       && (type != "FLOAT"))
    {
        throw FatalError("Invalid metric data type.");
    }

    if (  (mode != "COUNTER")
       && (mode != "RATE")
       && (mode != "SAMPLE"))
    {
        throw FatalError("Invalid metric mode.");
    }

    if (  (interval != "START")
       && (interval != "LAST")
       && (interval != "NEXT")
       && (interval != "NONE"))
    {
        throw FatalError("Invalid metric measurement interval.");
    }
}


// --- Get metric information -----------------------------------------------

Metric::IdType
Metric::getId() const
{
    return m_id;
}


string
Metric::get_name() const
{
    return m_name;
}


/**
 *  Returns the description text given for this performance metric.
 *
 *  @return Description
 **/
string
Metric::get_description() const
{
    return m_description;
}


/**
 *  Returns the metric type, which is either "INTEGER" or "FLOAT".
 *
 *  @return %Metric type
 **/
string
Metric::get_type() const
{
    return m_type;
}


/**
 *  Returns the metric mode, which is either "COUNTER", "RATE", or "SAMPLE".
 *
 *  @return %Metric mode
 **/
string
Metric::get_mode() const
{
    return m_mode;
}


/**
 *  Returns the measurement interval semantics, which is either "START", "LAST",
 *  "NEXT", or "NONE".
 *
 *  @return Measurement interval semantics
 **/
string
Metric::get_interval() const
{
    return m_interval;
}
