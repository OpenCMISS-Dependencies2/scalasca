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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Definition of all static objects.
 *
 *  This file provides the definition of all static objects to ensure a
 *  proper initialization order.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <cstddef>
#include <vector>

#include <pearl/CallingContext.h>
#include <pearl/Callpath.h>
#include <pearl/Callsite.h>
#include <pearl/CommLocationSet.h>
#include <pearl/CommSet.h>
#include <pearl/Communicator.h>
#include <pearl/Location.h>
#include <pearl/Paradigm.h>
#include <pearl/Region.h>
#include <pearl/SourceLocation.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>

using namespace std;
using namespace pearl;


// --- Static class data ----------------------------------------------------

const String::IdType String::NO_ID;
const String         String::UNDEFINED(String::NO_ID,
                                       "");

const String::IdType String::MACHINE_ID;
const String         String::MACHINE(String::MACHINE_ID,
                                     "machine");

const String::IdType String::NODE_ID;
const String         String::NODE(String::NODE_ID,
                                  "node");

const Region::IdType Region::NO_ID;
const Region         Region::UNDEFINED(Region::NO_ID,
                                       String::UNDEFINED,
                                       String::UNDEFINED,
                                       String::UNDEFINED,
                                       Region::ROLE_UNKNOWN,
                                       Paradigm::UNKNOWN,
                                       String::UNDEFINED,
                                       0,
                                       0);

const SourceLocation::IdType SourceLocation::NO_ID;
const SourceLocation         SourceLocation::UNDEFINED(SourceLocation::NO_ID,
                                                       String::UNDEFINED,
                                                       0);

const CallingContext::IdType CallingContext::NO_ID;
const CallingContext         CallingContext::UNDEFINED(CallingContext::NO_ID,
                                                       Region::UNDEFINED,
                                                       SourceLocation::UNDEFINED,
                                                       NULL);

const Callsite::IdType Callsite::NO_ID;
const Callsite         Callsite::UNDEFINED(Callsite::NO_ID,
                                           String::UNDEFINED,
                                           0,
                                           Region::UNDEFINED);

const Callpath::IdType Callpath::NO_ID;
const Callpath         Callpath::UNDEFINED(Callpath::NO_ID,
                                           Region::UNDEFINED,
                                           Callsite::UNDEFINED,
                                           NULL);

const CommLocationSet::IdType CommLocationSet::NO_ID;
const CommLocationSet
CommLocationSet::UNDEFINED(CommLocationSet::NO_ID,
                           String::UNDEFINED,
                           Paradigm::UNKNOWN,
                           vector< const Location* >());

const CommSet::IdType CommSet::NO_ID;
const CommSet
CommSet::UNDEFINED(CommSet::NO_ID,
                   String::UNDEFINED,
                   GroupingSet::PROPERTY_NONE,
                   CommLocationSet::UNDEFINED,
                   vector< uint32_t >());

const Communicator::IdType Communicator::NO_ID;
const Communicator         Communicator::UNDEFINED(Communicator::NO_ID,
                                                   String::UNDEFINED,
                                                   CommSet::UNDEFINED,
                                                   NULL);

const Location::IdType Location::NO_ID;
const Location         Location::UNDEFINED(Location::NO_ID,
                                           String::UNDEFINED,
                                           Location::TYPE_UNKNOWN,
                                           0,
                                           NULL);

const SystemNode::IdType SystemNode::NO_ID;
const SystemNode         SystemNode::UNDEFINED(SystemNode::NO_ID,
                                               String::UNDEFINED,
                                               String::UNDEFINED,
                                               NULL);
