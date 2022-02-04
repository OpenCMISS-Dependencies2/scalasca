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


#ifndef PEARL_REMOTERMAEPOCH_H
#define PEARL_REMOTERMAEPOCH_H


#include <vector>

#include <pearl/RemoteEvent.h>


namespace pearl
{
/// List of local events
typedef std::vector< pearl::RemoteEvent > RemoteRmaEpoch;
}    // namespace pearl


#endif    // !PEARL_REMOTERMAEPOCH_H
