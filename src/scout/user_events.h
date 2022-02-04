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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Declaration of constants for user-defined events.
 *
 *  This header file provides a set of numerical constants that provide
 *  symbolic names for various user events that can be triggered during
 *  the pattern search replay.
 **/
/*-------------------------------------------------------------------------*/


#ifndef SCOUT_USER_EVENTS_H
#define SCOUT_USER_EVENTS_H


namespace scout
{
// --- Constants ------------------------------------------------------------

const int PREPARE  = 1;
const int FINISHED = 2;

const int PRE_INIT     = 3;
const int INIT         = 4;
const int PRE_FINALIZE = 5;
const int FINALIZE     = 6;

const int PRE_SEND              = 10;
const int POST_SEND             = 11;
const int PRE_RECV              = 12;
const int POST_RECV             = 13;
const int COLL_12N              = 14;
const int COLL_N21              = 15;
const int COLL_N2N              = 16;
const int COLL_SCAN             = 17;
const int SYNC_COLL             = 18;
const int PROFILE               = 19;
const int LATE_SENDER           = 20;
const int LATE_SENDER_WO        = 21;
const int LATE_RECEIVER         = 22;
const int WAIT_NXN              = 23;
const int WAIT_BARRIER          = 24;
const int EARLY_REDUCE          = 25;
const int EARLY_SCAN            = 26;
const int LATE_BCAST            = 27;
const int NXN_COMPL             = 28;
const int BARRIER_COMPL         = 29;
const int LATE_SENDER_CANDIDATE = 30;
const int WAIT_FINALIZE         = 31;
const int INIT_COMPL            = 32;
const int PRE_SYNC_COLL         = 33;
const int PRE_COLL_N2N          = 34;
const int PRE_COLL_12N          = 35;
const int PRE_COLL_N21          = 36;
const int PRE_COLL_SCAN         = 37;

const int GPRE_SEND     = 50;
const int GPOST_SEND    = 51;
const int GPRE_RECV     = 52;
const int GPOST_RECV    = 53;
const int GCOLL_12N     = 54;
const int GCOLL_N21     = 55;
const int GCOLL_N2N     = 56;
const int GCOLL_BARRIER = 57;
const int GLATE_SENDER  = 58;
const int GWAIT_NXN     = 59;

const int NGPRE_SEND     = 60;
const int NGPOST_SEND    = 61;
const int NGPRE_RECV     = 62;
const int NGPOST_RECV    = 63;
const int NGCOLL_12N     = 64;
const int NGCOLL_N21     = 65;
const int NGCOLL_N2N     = 66;
const int NGCOLL_BARRIER = 67;
const int NGLATE_SENDER  = 68;
const int NGWAIT_NXN     = 69;

const int MPI_RMA_POST_WAIT     = 70;
const int MPI_RMA_POST_COMPLETE = 71;
const int MPI_RMA_WIN_FENCE     = 72;
const int MPI_RMA_WIN_CREATE    = 73;
const int MPI_RMA_WIN_FREE      = 74;
const int MPI_RMA_WIN_START     = 75;
const int MPI_RMA_WIN_COMPLETE  = 76;
const int MPI_RMA_WIN_POST      = 77;
const int MPI_RMA_WIN_WAIT      = 78;
const int MPI_RMA_LC_COMPUTE    = 79;

const int OMP_MGMT_FORK      = 90;
const int OMP_MGMT_JOIN      = 91;
const int OMP_EBARRIER       = 92;
const int OMP_IBARRIER       = 93;
const int OMP_EBARRIER_WAIT  = 94;
const int OMP_IBARRIER_WAIT  = 95;
const int OMP_BARRIER        = 96;
const int OMP_MGMT_POST_FORK = 97;

const int CCV_P2P  = 100;
const int CCV_COLL = 101;

const int RMA_SYNC_COLLECTIVE   = 110;
const int RMA_SYNC_GROUP        = 111;
const int RMA_SYNC_DIRECT       = 112;
const int RMA_WFP_SINGLE_DEP    = 113;
const int RMA_WFP_MULTI_DEP     = 114;
const int RMA_SEV_EXCHANGE      = 115;
const int RMA_WAIT_FOR_NOTIFY   = 116;
const int RMA_WAIT_FOR_PROGRESS = 117;

const int RPL_BWS        = 120;
const int RPL_FWS        = 121;
const int RPL_BWC        = 122;
const int RPL_FWC        = 123;
const int RPL_FWP        = 124;
const int RPL_RESUME_FWD = 125;

const int DCOLL_12N     = 140;
const int DCOLL_N21     = 141;
const int DCOLL_N2N     = 142;
const int DCOLL_BARRIER = 143;
const int DCOLL_SCAN    = 144;

const int DELAY_LATE_SENDER   = 150;
const int DELAY_BARRIER       = 151;
const int DELAY_N2N           = 152;
const int DELAY_12N           = 153;
const int DELAY_N21           = 154;
const int DELAY_LATE_RECEIVER = 155;

const int DELAY_OMP_BARRIER = 156;
const int DELAY_OMP_IDLE    = 157;

const int CRITICALPATH_FINISHED = 160;

const int PRE_INV_SENDCMP  = 170;
const int PRE_INV_RECVREQ  = 171;
const int POST_INV_SENDCMP = 172;
const int POST_INV_RECVREQ = 173;

const int MPI_RMA_WAIT_AT_FENCE      = 181;
const int MPI_RMA_EARLY_FENCE        = 182;
const int MPI_RMA_WAIT_AT_CREATE     = 183;
const int MPI_RMA_WAIT_AT_FREE       = 184;
const int MPI_RMA_EARLY_FREE         = 185;
const int MPI_RMA_PWS_COUNT          = 186;
const int MPI_RMA_PWS_UNNEEDED_COUNT = 187;
const int MPI_RMA_LOCK_COMPETITION   = 188;
const int MPI_RMA_WAIT_FOR_PROGRESS  = 189;
const int MPI_RMA_LATE_POST          = 190;
const int MPI_RMA_LATE_COMPLETE      = 191;
const int MPI_RMA_EARLY_WAIT         = 192;
const int MPI_RMA_GATS_SYNC          = 193;
const int RMA_PRE_WAIT               = 195;
const int RMA_POST_CAUSE             = 196;
const int WFP_SYNC                   = 197;

const int DRMA_COLL_CREATE           = 200;
const int DRMA_COLL_FENCE            = 201;
const int DRMA_COLL_FREE             = 202;
const int DRMA_GATS_LATE_POST        = 203;
const int DRMA_GATS_EARLY_WAIT       = 204;
const int DRMA_PASS_LOCK_COMPETITION = 205;

const int DELAY_WAIT_AT_CREATE   = 210;
const int DELAY_WAIT_AT_FENCE    = 211;
const int DELAY_WAIT_AT_FREE     = 212;
const int DELAY_LATE_POST        = 213;
const int DELAY_EARLY_WAIT       = 214;
const int DELAY_LOCK_COMPETITION = 215;

const int PTHREAD_CONTENTION_MUTEX_LOCK                = 220;
const int PTHREAD_CONTENTION_MUTEX_TRYLOCK             = 221;
const int PTHREAD_CONTENTION_CONDITIONAL               = 222;
const int OMP_CONTENTION_CRITICAL                      = 223;
const int OMP_CONTENTION_API                           = 224;
const int THREAD_LOCK_CONTENTION_OMPLOCK_WAIT          = 225;
const int THREAD_LOCK_CONTENTION_OMPCRITICAL_WAIT      = 226;
const int THREAD_LOCK_CONTENTION_PTHREADMUTEX_WAIT     = 227;
const int THREAD_LOCK_CONTENTION_PTHREADCONDITION_WAIT = 228;

const int TASK_CORRECTION = 232;

const int PRE_DELAY_LATE_SENDER   = 240;
const int PRE_DELAY_BARRIER       = 241;
const int PRE_DELAY_N2N           = 242;
const int PRE_DELAY_12N           = 243;
const int PRE_DELAY_N21           = 244;
const int PRE_DELAY_LATE_RECEIVER = 245;

const int PRE_DELAY_OMP_BARRIER = 246;
const int PRE_DELAY_OMP_IDLE    = 247;

const int WAITSTATE_PROP_LS = 250;
const int WAITSTATE_PROP_LR = 251;
}    // namespace scout


#endif    // !SCOUT_USER_EVENTS_H
