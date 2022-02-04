/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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
 *  @brief   Declarations of EPIK callback functions for event records.
 *
 *  This header file provides the declarations of EPIK callback functions
 *  and related data types for handling event records.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_EPIKEVTCALLBACKS_H
#define PEARL_EPIKEVTCALLBACKS_H


#include <stdint.h>

#include <set>
#include <stack>
#include <string>

#include <pearl/Location.h>


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class GlobalDefs;
class LocalTrace;


namespace detail
{
// --- Forward declarations -------------------------------------------------

class EpikArchive;
struct EpikMapCbData;


/*-------------------------------------------------------------------------*/
/**
 *  @struct  EpikEvtCbData
 *  @ingroup PEARL_base
 *  @brief   EPIK event callback data
 *
 *  The EpikEvtCbData struct combines all data elements which are necessary
 *  to process the event records of an EPIK trace archive.
 **/
/*-------------------------------------------------------------------------*/

struct EpikEvtCbData
{
    /// @name Constructors & destructor
    /// @{

    EpikEvtCbData(const EpikArchive& archive,
                  const GlobalDefs&  defs,
                  LocalTrace&        trace,
                  EpikMapCbData&     table,
                  uint32_t           version,
                  Location::IdType   locationId,
                  uint32_t           threadId);

    /// @}


    /// EPIK archive abstraction
    const EpikArchive& mArchive;

    /// %GlobalDefs object providing the global definitions
    const GlobalDefs& mDefinitions;

    /// %LocalTrace object storing the event representations read
    LocalTrace& mTrace;

    /// %EpikMapCbData object used for converting local into global identifiers
    EpikMapCbData& mMappingTable;

    /// EPILOG file format version
    uint32_t mVersion;

    /// %Location identifier for the processed trace file
    Location::IdType mLocation;

    /// %Thread identifier in case of OpenMP, always 0 for MPI
    uint32_t mThreadId;

    /// %Last MPI request ID seen
    uint64_t mRequestId;

    /// Currently active MPI request IDs
    std::set< uint64_t > mActiveRequests;

    /// Current callstack in terms of region IDs
    std::stack< uint32_t > mCallstack;

    /// %Error message of exceptions thrown (and caught) within callbacks
    std::string mErrorMessage;
};
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_EPIKEVTCALLBACKS_H
