/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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
 *  @brief   Declaration of the struct TimeRank.
 *
 *  This header file provides the declaration of the struct TimeRank and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef SCOUT_TIMERANK_H
#define SCOUT_TIMERANK_H


#include <iosfwd>
#include <limits>

#include <pearl/Communicator.h>


namespace scout
{
/**
 *  @struct TimeRank
 *  @brief  A combination of a timestamp and rank that conforms to
 *          `MPI_DOUBLE_INT`.
 *
 *  Instances of the TimeRank struct combine information about a timestamp
 *  and a corresponding rank using a data layout that conforms to the MPI
 *  datatype `MPI_DOUBLE_INT`.
 **/

struct TimeRank
{
    // --- Public methods ----------------------------------

    /// @name Constructors & destructor
    /// @{

    /// @brief Default constructor.
    ///
    /// Creates a new the instance with the timestamp set to the lowest finite
    /// representable value and the rank set to `-1`.
    ///
    /// @todo[C++11] Replace `-max()` by `lowest()`.
    ///
    TimeRank()
        : mTime(-std::numeric_limits< double >::max()),
          mRank(-1)
    {
    }

    /// @brief Constructor.
    ///
    /// Creates a new instance with the timestamp set to @p time and the rank
    /// to @p rank.
    ///
    /// @param time
    ///     Initial timestamp
    /// @param rank
    ///     Corresponding MPI rank
    ///
    TimeRank(double time,
             int    rank)
        : mTime(time),
          mRank(rank)
    {
    }

    /// @}


    // --- Data members ------------------------------------

    /// Timestamp
    double mTime;

    /// Corresponding MPI rank
    int mRank;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates TimeRank
///
/// Prints the contents of the given TimeRank struct @p item to an output
/// @p stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     TimeRank struct
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&   stream,
           const TimeRank& item);

/// @}
/// @name Conversion functions
/// @{

/// @brief   Local-to-global rank conversion of TimeRank struct.
/// @relates TimeRank
///
/// Converts @p item using a local rank in the communicator @p comm into a
/// TimeRank struct using the corresponding global rank.
///
/// @param item
///     TimeRank struct
/// @param comm
///     Underlying communicator
/// @returns
///     Converted TimeRank struct using the global rank
///
TimeRank
globalize(const TimeRank&            item,
          const pearl::Communicator& comm);

/// @}
}    // namespace scout


#endif    // !SCOUT_TIMERANK_H
