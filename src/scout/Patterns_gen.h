/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2019-2021                                                **
**  RWTH Aachen University, IT Center                                      **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_PATTERNS_GEN_H
#define SCOUT_PATTERNS_GEN_H


namespace scout
{
// --- Constants ------------------------------------------------------------

const long PAT_NONE                                = -1;
const long PAT_TIME                                = 0;
const long PAT_VISITS                              = 1;
const long PAT_EXECUTION                           = 2;
const long PAT_OVERHEAD                            = 3;
const long PAT_COMP                                = 4;
const long PAT_ASYNC                               = 5;
const long PAT_MPI                                 = 6;
const long PAT_MPI_MANAGEMENT                      = 7;
const long PAT_MPI_INIT_EXIT                       = 8;
const long PAT_MPI_INIT_COMPLETION                 = 9;
const long PAT_MPI_FINALIZE_WAIT                   = 10;
const long PAT_MPI_MGMT_COMM                       = 11;
const long PAT_MPI_MGMT_FILE                       = 12;
const long PAT_MPI_MGMT_WIN                        = 13;
const long PAT_MPI_SYNCHRONIZATION                 = 14;
const long PAT_MPI_SYNC_COLLECTIVE                 = 15;
const long PAT_MPI_BARRIER_WAIT                    = 16;
const long PAT_MPI_BARRIER_COMPLETION              = 17;
const long PAT_MPI_COMMUNICATION                   = 18;
const long PAT_MPI_POINT2POINT                     = 19;
const long PAT_MPI_LATESENDER                      = 20;
const long PAT_MPI_LATESENDER_WO                   = 21;
const long PAT_MPI_LSWO_DIFFERENT                  = 22;
const long PAT_MPI_LSWO_SAME                       = 23;
const long PAT_MPI_LATERECEIVER                    = 24;
const long PAT_MPI_COLLECTIVE                      = 25;
const long PAT_MPI_EARLYREDUCE                     = 26;
const long PAT_MPI_EARLYSCAN                       = 27;
const long PAT_MPI_LATEBROADCAST                   = 28;
const long PAT_MPI_WAIT_NXN                        = 29;
const long PAT_MPI_NXN_COMPLETION                  = 30;
const long PAT_MPI_IO                              = 31;
const long PAT_MPI_IO_INDIVIDUAL                   = 32;
const long PAT_MPI_IO_COLLECTIVE                   = 33;
const long PAT_OMP_IDLE_THREADS                    = 34;
const long PAT_OMP_LIMITED_PARALLELISM             = 35;
const long PAT_OMP_TIME                            = 36;
const long PAT_OMP_MANAGEMENT                      = 37;
const long PAT_OMP_FORK                            = 38;
const long PAT_OMP_SYNCHRONIZATION                 = 39;
const long PAT_OMP_BARRIER                         = 40;
const long PAT_OMP_EBARRIER                        = 41;
const long PAT_OMP_EBARRIER_WAIT                   = 42;
const long PAT_OMP_IBARRIER                        = 43;
const long PAT_OMP_IBARRIER_WAIT                   = 44;
const long PAT_OMP_CRITICAL                        = 45;
const long PAT_OMP_LOCK_CONTENTION_CRITICAL        = 46;
const long PAT_OMP_LOCK_API                        = 47;
const long PAT_OMP_LOCK_CONTENTION_API             = 48;
const long PAT_OMP_ORDERED                         = 49;
const long PAT_OMP_TASKWAIT                        = 50;
const long PAT_OMP_FLUSH                           = 51;
const long PAT_PTHREAD_TIME                        = 52;
const long PAT_PTHREAD_MANAGEMENT                  = 53;
const long PAT_PTHREAD_SYNCHRONIZATION             = 54;
const long PAT_PTHREAD_LOCK_API                    = 55;
const long PAT_PTHREAD_LOCK_CONTENTION_MUTEX_LOCK  = 56;
const long PAT_PTHREAD_CONDITIONAL                 = 57;
const long PAT_PTHREAD_LOCK_CONTENTION_CONDITIONAL = 58;
const long PAT_THREAD_LOCK_CONTENTION              = 59;
const long PAT_OPENACC_TIME                        = 60;
const long PAT_OPENACC_SETUP                       = 61;
const long PAT_OPENACC_COMM                        = 62;
const long PAT_OPENACC_SYNC                        = 63;
const long PAT_OPENACC_KERNEL_LAUNCHES             = 64;
const long PAT_OPENCL_KERNEL_EXECUTIONS            = 65;
const long PAT_OPENCL_TIME                         = 66;
const long PAT_OPENCL_SETUP                        = 67;
const long PAT_OPENCL_COMM                         = 68;
const long PAT_OPENCL_SYNC                         = 69;
const long PAT_OPENCL_KERNEL_LAUNCHES              = 70;
const long PAT_CUDA_KERNEL_EXECUTIONS              = 71;
const long PAT_CUDA_TIME                           = 72;
const long PAT_CUDA_SETUP                          = 73;
const long PAT_CUDA_COMM                           = 74;
const long PAT_CUDA_SYNC                           = 75;
const long PAT_CUDA_KERNEL_LAUNCHES                = 76;
const long PAT_SYNCS                               = 77;
const long PAT_SYNCS_P2P                           = 78;
const long PAT_SYNCS_SEND                          = 79;
const long PAT_SYNCS_RECV                          = 80;
const long PAT_SYNCS_COLL                          = 81;
const long PAT_SYNCS_RMA                           = 82;
const long PAT_SYNCS_RMA_ACTIVE                    = 83;
const long PAT_SYNCS_RMA_PASSIVE                   = 84;
const long PAT_COMMS                               = 85;
const long PAT_COMMS_P2P                           = 86;
const long PAT_COMMS_SEND                          = 87;
const long PAT_COMMS_RECV                          = 88;
const long PAT_COMMS_COLL                          = 89;
const long PAT_COMMS_CXCH                          = 90;
const long PAT_COMMS_CSRC                          = 91;
const long PAT_COMMS_CDST                          = 92;
const long PAT_COMMS_RMA                           = 93;
const long PAT_COMMS_RMA_PUTS                      = 94;
const long PAT_COMMS_RMA_GETS                      = 95;
const long PAT_COMMS_RMA_ATOMICS                   = 96;
const long PAT_BYTES                               = 97;
const long PAT_BYTES_P2P                           = 98;
const long PAT_BYTES_SENT                          = 99;
const long PAT_BYTES_RCVD                          = 100;
const long PAT_BYTES_COLL                          = 101;
const long PAT_BYTES_COUT                          = 102;
const long PAT_BYTES_CIN                           = 103;
const long PAT_BYTES_RMA                           = 104;
const long PAT_BYTES_PUT                           = 105;
const long PAT_BYTES_GET                           = 106;
const long PAT_MPI_SLR_COUNT                       = 107;
const long PAT_MPI_SLS_COUNT                       = 108;
const long PAT_MPI_SLSWO_COUNT                     = 109;
const long PAT_MPI_CLR_COUNT                       = 110;
const long PAT_MPI_CLS_COUNT                       = 111;
const long PAT_MPI_CLSWO_COUNT                     = 112;
const long PAT_MPI_FILE_OPS                        = 113;
const long PAT_MPI_FILE_IOPS                       = 114;
const long PAT_MPI_FILE_IROPS                      = 115;
const long PAT_MPI_FILE_IWOPS                      = 116;
const long PAT_MPI_FILE_COPS                       = 117;
const long PAT_MPI_FILE_CROPS                      = 118;
const long PAT_MPI_FILE_CWOPS                      = 119;
const long PAT_MPI_RMA_WAIT_AT_CREATE              = 120;
const long PAT_MPI_RMA_WAIT_AT_FREE                = 121;
const long PAT_MPI_RMA_SYNCHRONIZATION             = 122;
const long PAT_MPI_RMA_SYNC_ACTIVE                 = 123;
const long PAT_MPI_RMA_SYNC_LATE_POST              = 124;
const long PAT_MPI_RMA_EARLY_WAIT                  = 125;
const long PAT_MPI_RMA_LATE_COMPLETE               = 126;
const long PAT_MPI_RMA_WAIT_AT_FENCE               = 127;
const long PAT_MPI_RMA_EARLY_FENCE                 = 128;
const long PAT_MPI_RMA_SYNC_PASSIVE                = 129;
const long PAT_MPI_RMA_SYNC_LOCK_COMPETITION       = 130;
const long PAT_MPI_RMA_SYNC_WAIT_FOR_PROGRESS      = 131;
const long PAT_MPI_RMA_COMMUNICATION               = 132;
const long PAT_MPI_RMA_COMM_LATE_POST              = 133;
const long PAT_MPI_RMA_COMM_LOCK_COMPETITION       = 134;
const long PAT_MPI_RMA_COMM_WAIT_FOR_PROGRESS      = 135;
const long PAT_MPI_RMA_PAIRSYNC_COUNT              = 136;
const long PAT_MPI_RMA_PAIRSYNC_UNNEEDED_COUNT     = 137;
const long PAT_IMBALANCE                           = 138;
const long PAT_IMBALANCE_ABOVE                     = 139;
const long PAT_IMBALANCE_ABOVE_SINGLE              = 140;
const long PAT_IMBALANCE_BELOW                     = 141;
const long PAT_IMBALANCE_BELOW_BYPASS              = 142;
const long PAT_IMBALANCE_BELOW_SINGULARITY         = 143;
const long PAT_STATISTICS                          = 144;
const long PAT_CRITICAL_PATH                       = 145;
const long PAT_CRITICAL_PATH_IMBALANCE             = 146;
const long PAT_PERFORMANCE_IMPACT                  = 147;
const long PAT_PERFORMANCE_IMPACT_CRITICALPATH     = 148;
const long PAT_CRITICAL_PATH_ACTIVITIES            = 149;
const long PAT_CRITICAL_IMBALANCE_IMPACT           = 150;
const long PAT_INTRA_PARTITION_IMBALANCE           = 151;
const long PAT_INTER_PARTITION_IMBALANCE           = 152;
const long PAT_NON_CRITICAL_PATH_ACTIVITIES        = 153;
const long PAT_DELAY                               = 154;
const long PAT_DELAY_MPI                           = 155;
const long PAT_DELAY_P2P                           = 156;
const long PAT_DELAY_LATESENDER_AGGREGATE          = 157;
const long PAT_DELAY_LATESENDER                    = 158;
const long PAT_DELAY_LATESENDER_LONGTERM           = 159;
const long PAT_DELAY_LATERECEIVER_AGGREGATE        = 160;
const long PAT_DELAY_LATERECEIVER                  = 161;
const long PAT_DELAY_LATERECEIVER_LONGTERM         = 162;
const long PAT_DELAY_COLLECTIVE                    = 163;
const long PAT_DELAY_BARRIER_AGGREGATE             = 164;
const long PAT_DELAY_BARRIER                       = 165;
const long PAT_DELAY_BARRIER_LONGTERM              = 166;
const long PAT_DELAY_N2N_AGGREGATE                 = 167;
const long PAT_DELAY_N2N                           = 168;
const long PAT_DELAY_N2N_LONGTERM                  = 169;
const long PAT_DELAY_12N_AGGREGATE                 = 170;
const long PAT_DELAY_12N                           = 171;
const long PAT_DELAY_12N_LONGTERM                  = 172;
const long PAT_DELAY_N21_AGGREGATE                 = 173;
const long PAT_DELAY_N21                           = 174;
const long PAT_DELAY_N21_LONGTERM                  = 175;
const long PAT_DELAY_OMP                           = 176;
const long PAT_DELAY_OMPBARRIER_AGGREGATE          = 177;
const long PAT_DELAY_OMPBARRIER                    = 178;
const long PAT_DELAY_OMPBARRIER_LONGTERM           = 179;
const long PAT_DELAY_OMPIDLE_AGGREGATE             = 180;
const long PAT_DELAY_OMPIDLE                       = 181;
const long PAT_DELAY_OMPIDLE_LONGTERM              = 182;
const long PAT_WAITSTATES_DIRECT_VS_INDIRECT       = 183;
const long PAT_MPI_WAIT_DIRECT                     = 184;
const long PAT_MPI_WAIT_DIRECT_LATESENDER          = 185;
const long PAT_MPI_WAIT_DIRECT_LATERECEIVER        = 186;
const long PAT_MPI_WAIT_INDIRECT                   = 187;
const long PAT_MPI_WAIT_INDIRECT_LATESENDER        = 188;
const long PAT_MPI_WAIT_INDIRECT_LATERECEIVER      = 189;
const long PAT_WAITSTATES_PROPAGATING_VS_TERMINAL  = 190;
const long PAT_MPI_WAIT_PROPAGATING                = 191;
const long PAT_MPI_WAIT_PROPAGATING_LS             = 192;
const long PAT_MPI_WAIT_PROPAGATING_LR             = 193;
const long PAT_MPI_WAIT_TERMINAL                   = 194;
const long PAT_MPI_WAIT_TERMINAL_LS                = 195;
const long PAT_MPI_WAIT_TERMINAL_LR                = 196;


// --- Forward declarations -------------------------------------------------

class AnalyzeTask;


// --- Pattern registration -------------------------------------------------

void
create_patterns(AnalyzeTask* analyzer);
}    // namespace scout


#endif    // !SCOUT_PATTERNS_GEN_H
