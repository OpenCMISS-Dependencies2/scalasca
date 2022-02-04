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


#include <config.h>

#include "SilasConfig.h"

using namespace std;
using namespace silas;


Config::Config()
{
    m_input_trace_name  = "epik_a";
    m_output_trace_name = "epik_sim";
}


Config::~Config()
{
}


bool
Config::is_type(string type)
{
    if (type == "base")
    {
        return true;
    }

    return false;
}


void
Config::set_input_trace(string name)
{
    m_input_trace_name = name;
}


string
Config::get_input_trace()
{
    return m_input_trace_name;
}


void
Config::set_output_trace(string name)
{
    m_output_trace_name = name;
}


string
Config::get_output_trace()
{
    return m_output_trace_name;
}
