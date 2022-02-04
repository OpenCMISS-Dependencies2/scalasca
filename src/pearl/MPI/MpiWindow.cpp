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

#include <pearl/MpiWindow.h>

#include <pearl/MpiComm.h>

using namespace pearl;


// --- Constructors & destructor --------------------------------------------

/**
 *  Creates a new instance and initializes its identifier and associated
 *  communicator with the given values @a id and @a communicator,
 *  respectively.
 *
 *  @param id           Window identifier
 *  @param communicator Associated MPI communicator
 **/
MpiWindow::MpiWindow(IdType   id,
                     MpiComm* communicator)
    : RmaWindow(id, communicator)
{
}
