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

#include <pearl/RmaWindow.h>

#include <cassert>
#include <iostream>

#include <pearl/Communicator.h>
#include <pearl/iomanip.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

/**
 *  Creates a new instance and initializes its identifier with the given
 *  value @a id and @a communicator, respectively.
 *
 *  @param id           Window identifier
 *  @param communicator Associated communicator
 **/
RmaWindow::RmaWindow(IdType        id,
                     Communicator* communicator)
    : m_id(id),
      m_communicator(communicator)
{
    assert(communicator);
}


RmaWindow::~RmaWindow()
{
}


// --- Get window information -----------------------------------------------

RmaWindow::IdType
RmaWindow::getId() const
{
    return m_id;
}


/**
 *  Returns the associated communicator object.
 *
 *  @return Communicator
 **/
Communicator*
RmaWindow::get_comm() const
{
    return m_communicator;
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&         stream,
                  const RmaWindow& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << item.getId();
    }

    // Print data
    const Communicator* const comm = item.get_comm();
    stream << "RmaWindow"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "comm: " << (comm ? *comm : Communicator::UNDEFINED)
           << endObject;

    return stream;
}
