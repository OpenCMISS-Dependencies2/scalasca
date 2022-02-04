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


#ifndef SCOUT_SCOUT_TYPES_H
#define SCOUT_SCOUT_TYPES_H


#include <map>
#include <stack>

#include <pearl/Callpath.h>
#include <pearl/Event.h>
#include <pearl/Location.h>
#include <pearl/pearl_types.h>

#include "TimeRank.h"
#include "fixed_map.hh"


/*-------------------------------------------------------------------------*/
/**
 * @file  scout_types.h
 * @brief Declaration of SCOUT-specific datatypes.
 *
 * This header file provides the declaration of some SCOUT-specific
 * datatypes.
 */
/*-------------------------------------------------------------------------*/

// --- Forward declarations -------------------------------------------------

namespace pearl
{
class CallbackManager;
}    // namespace pearl


// --- Preprocessor defines -------------------------------------------------

/// Maximum number of most-severe instances to be tracked
#define TOP_SEVERE_INSTANCES    5

// #define MOST_SEVERE_MAX // if one needs to find the maximal time among top severe instances


// --- Type definitions -----------------------------------------------------

namespace scout
{
/// Remote severity map. Stores a location @f$\times@f$ cnode @f$\times@f$
/// severity value matrix.
typedef std::map< pearl::Location::IdType, std::map< pearl::ident_t, double > >
    rem_sev_container_t;

/// Callback manager map. Maps replay phases to callback manager objects.
typedef std::map< std::string, pearl::CallbackManager* >
    CallbackManagerMap;

/// @struct CommInfo
/// @brief  global rank and communicator information for non-communicating events.
///
/// This data structure is used to save communication relevant parameters for events
/// don't have their inherent communciation information. This is needed for
/// patterns like "Wait for Progress" when non-communication events can be synchronization
/// points.
struct CommInfo
{
    int      globalRank;
    uint32_t commId;
};


/// Container for communication information
/// @note The multimap is needed as for patterns like "Wait for progress" a single
/// Enter event can be synchpoint to multiple remote events.
typedef std::multimap< pearl::Event, CommInfo, const pearl::EventKeyCompare >
    CommInfoMap;

/// @struct SynchpointInfo
/// @brief  Synchronization information for MPI and OpenMP calls

struct SynchpointInfo
{
    double waitTime;
    double communicationTime;
    double completionTime;
    double totalTime;
};


/// @struct ContentionInfo
/// @brief  Information needed to reconstruct contention chain in passive target RMA
struct ContentionInfo
{
    pearl::ident_t     eventIndex;
    pearl::timestamp_t waitTime;
    uint32_t           syncRank;
};


typedef std::stack< ContentionInfo >         ContentionInfoStack;
typedef std::map< int, ContentionInfoStack > ContentionInfoStackMap;

/// Type definition of a cnode -> time map
typedef fixed_priority_map< pearl::Callpath::IdType, double, 128 >
    timemap_t;

/// Type definition of a SynchpointInfo -> time map
typedef std::map< pearl::Event, SynchpointInfo, pearl::EventKeyCompare >
    SynchpointInfoMap;

/// Type definition of a Event -> double map
typedef std::map< pearl::Event, double, pearl::EventKeyCompare >
    ScaleMap;

/**
 * @struct TimeMapSum
 * @brief  A combination of a timemap and the sum of its values
 */

struct TimeMapSum
{
    TimeMapSum()
        : mSum(0.0)
    {
    }

    timemap_t mMap; // CNode -> time map
    double    mSum; // Sum of times
};


/**
 * @struct CollectiveInfo
 * @brief  Comprises global information on a collective operation
 **/

struct CollectiveInfo
{
    TimeRank earliest_end;
    TimeRank latest;
    TimeRank root;
    TimeRank my;
};


// Key type for remote severities
typedef std::pair< pearl::Location::IdType, pearl::Callpath::IdType > RemoteSeverityKey;

// Container for remote severities
typedef std::map< RemoteSeverityKey, pearl::timestamp_t > RemoteSeverityMap;


/**
 *  @struct TimeVec2
 *  @brief  Simple datatype storing two timestamp values.
 *
 *  This struct provides a simple datatype to store two timestamp values.
 *  The associated MPI datatype handle is provided by TIMEVEC2.
 **/

struct TimeVec2
{
    /// Timestamp values
    pearl::timestamp_t value[2];
};


struct TopMostSevere
{
    pearl::timestamp_t idletime;
    pearl::timestamp_t entertime;
    pearl::timestamp_t exittime;
    pearl::ident_t     cnode;
    pearl::ident_t     rank;

    TopMostSevere()
        : idletime(0.0),
          entertime(0.0),
          exittime(0.0),
          cnode(0),
          rank(0)
    {
    }

    TopMostSevere(pearl::timestamp_t idletime_,
                  pearl::timestamp_t entertime_,
                  pearl::timestamp_t exittime_,
                  pearl::ident_t     cnode_,
                  pearl::ident_t     rank_)
        : idletime(idletime_),
          entertime(entertime_),
          exittime(exittime_),
          cnode(cnode_),
          rank(rank_)
    {
    }
};


struct TopMostSevereMaxSum
    : TopMostSevere
{
    // idletime will be used for summation of waiting time
    // idletime_max will be used for storage of maximal value of the waiting time
    pearl::timestamp_t idletime_max;

    TopMostSevereMaxSum()
        : idletime_max(0.0)
    {
    }

    TopMostSevereMaxSum(pearl::timestamp_t idletimemax_,
                        pearl::timestamp_t idletimesum_,
                        pearl::timestamp_t entertime_,
                        pearl::timestamp_t exittime_,
                        pearl::ident_t     cnode_,
                        pearl::ident_t     rank_)
        : TopMostSevere(idletimesum_,
                        entertime_,
                        exittime_,
                        cnode_,
                        rank_),
          idletime_max(idletimemax_)
    {
    }
};
}    // namespace scout


#endif    // !SCOUT_SCOUT_TYPES_H
