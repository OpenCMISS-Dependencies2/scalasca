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

#include "AmNull.h"

#include <pearl/AmRequest.h>
#include <pearl/Buffer.h>


/**
 * @file    AmNull.cpp
 * @ingroup PEARL_base
 * @brief   Implementation of the class AmNull.
 *
 * This file provides the implementation of the class AmNull. It implements a
 * empty AmRequestHandler that can be used to implement a fence operation
 * on the AmRuntime.
 */

using namespace std;
using namespace pearl;


AmNull::AmNull()
{
}


AmNull::~AmNull()
{
}


AmNull&
AmNull::getInstance()
{
    static AmNull mHandler;

    return mHandler;
}


void
AmNull::pack(Buffer& buffer)
{
    buffer.put_id(getInstance().getId());
}


void
AmNull::pack(AmRequest* request)
{
    pack(*request->getBuffer());
}


void
AmNull::execute(Buffer& buffer)
{
}


const string&
AmNull::getName() const
{
    static const string m_name = "pearl::AmNull";

    return m_name;
}
