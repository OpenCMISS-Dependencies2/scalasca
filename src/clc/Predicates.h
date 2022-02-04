/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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


#ifndef CLC_PREDICATES_H
#define CLC_PREDICATES_H


#include <pearl/Event.h>


// Predicate functions
bool
isOnRoot(const pearl::Event& event,
         uint32_t            globalRank);

bool
isLogicalSend(const pearl::Event& event,
              int                 rank);


#endif    // !CLC_PREDICATES_H
