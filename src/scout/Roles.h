/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_ROLES_H
#define SCOUT_ROLES_H


#include <stdint.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file  Roles.h
 *  @brief Declaration of constants for event roles.
 *
 *  This header file provides a set of numerical constants which provide
 *  symbolic names for the different roles that can be attributed to events.
 *  These are used when transferring events between processes using the
 *  pearl::EventSet and pearl::RemoteEventSet classes, respectively.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
// --- Role ID constants ----------------------------------------------------

const uint32_t ROLE_SEND       = 0;
const uint32_t ROLE_RECV       = 1;
const uint32_t ROLE_ENTER_SEND = 2;
const uint32_t ROLE_ENTER_RECV = 3;
const uint32_t ROLE_LEAVE_SEND = 4;
const uint32_t ROLE_LEAVE_RECV = 5;

const uint32_t ROLE_SEND_LS       = 6;
const uint32_t ROLE_RECV_LS       = 7;
const uint32_t ROLE_ENTER_SEND_LS = 8;
const uint32_t ROLE_ENTER_RECV_LS = 9;
const uint32_t ROLE_LEAVE_SEND_LS = 10;
const uint32_t ROLE_LEAVE_RECV_LS = 11;

const uint32_t ROLE_SEND_LSWO = 12;
const uint32_t ROLE_RECV_LSWO = 13;

const uint32_t ROLE_SEND_LR       = 14;
const uint32_t ROLE_RECV_LR       = 15;
const uint32_t ROLE_ENTER_SEND_LR = 16;
const uint32_t ROLE_ENTER_RECV_LR = 17;
const uint32_t ROLE_LEAVE_SEND_LR = 18;
const uint32_t ROLE_LEAVE_RECV_LR = 19;

const uint32_t ROLE_ENTER_COLL = 20;
const uint32_t ROLE_BEGIN_COLL = 21;
const uint32_t ROLE_END_COLL   = 22;
const uint32_t ROLE_LEAVE_COLL = 23;

const uint32_t ROLE_ENTER_RMA_START    = 24;
const uint32_t ROLE_LEAVE_RMA_START    = 25;
const uint32_t ROLE_LEAVE_RMA_COMPLETE = 26;

const uint32_t ROLE_LEAVE_SEND_SYNC    = 30;
const uint32_t ROLE_LEAVE_RECV_SYNC    = 31;
const uint32_t ROLE_LEAVE_RECVREQ_SYNC = 32;

const uint32_t BUFFER_SYNC         = 101;
const uint32_t BUFFER_CRITICALPATH = 111;

const uint32_t BUFFER_DELAY            = 121;
const uint32_t BUFFER_RCOST_LS         = 122;
const uint32_t BUFFER_RCOST_LR         = 123;
const uint32_t BUFFER_RCOST_BARRIER    = 124;
const uint32_t BUFFER_RCOST_N2N        = 125;
const uint32_t BUFFER_RCOST_N21        = 126;
const uint32_t BUFFER_RCOST_12N        = 127;
const uint32_t BUFFER_RCOST_OMPBARRIER = 131;
const uint32_t BUFFER_RCOST_OMPIDLE    = 132;

const uint32_t BUFFER_PROPWAIT = 141;

const uint32_t KEY_WAITTIME         = 201;
const uint32_t KEY_RCOST_LS         = 202;
const uint32_t KEY_RCOST_LR         = 203;
const uint32_t KEY_RCOST_BARRIER    = 204;
const uint32_t KEY_RCOST_N2N        = 205;
const uint32_t KEY_RCOST_12N        = 206;
const uint32_t KEY_RCOST_OMPBARRIER = 207;
const uint32_t KEY_RCOST_OMPIDLE    = 208;
const uint32_t KEY_RCOST_N21        = 209;
}    // namespace scout


#endif    // !SCOUT_ROLES_H
