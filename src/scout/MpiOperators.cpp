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


#include <config.h>

#include "MpiOperators.h"

#include <algorithm>
#include <cstring>

#include "scout_types.h"

using namespace std;
using namespace scout;


// --- Global variables -----------------------------------------------------

/// MPI reduction operator handle for combined minimum/maximum reduction
/// of the TimeVec2 struct
MPI_Op scout::MINMAX_TIMEVEC2;

/// MPI reduction operator handle for most-severe instance reduction
MPI_Op scout::MAX_TOPSEVEREARRAY;

/// MPI reduction operator handle for collective wait-state information
/// reduction
MPI_Op scout::MAXSUM_TOPSEVERECOLL;


// --- Local function prototypes --------------------------------------------

extern "C" void
scout_minmax(TimeVec2*     in,
             TimeVec2*     inout,
             int*          len,
             MPI_Datatype* type);

extern "C" void
top_severe(TopMostSevere* in,
           TopMostSevere* inout,
           int*           len,
           MPI_Datatype*  type);

extern "C" void
top_severe_coll_maxsum(TopMostSevereMaxSum* in,
                       TopMostSevereMaxSum* inout,
                       int*                 len,
                       MPI_Datatype*        type);


// --- Related functions ----------------------------------------------------

/**
 *  @brief Creates all SCOUT-specific MPI reduction operators.
 *
 *  This function creates all SCOUT-specific MPI reduction operators. It can
 *  only be called after MPI has been successfully initialized.
 **/
void
scout::InitOperators()
{
    // Define MPI reduction operators
    MPI_Op_create((MPI_User_function*)scout_minmax, true, &MINMAX_TIMEVEC2);
    MPI_Op_create((MPI_User_function*)top_severe, true, &MAX_TOPSEVEREARRAY);
    MPI_Op_create((MPI_User_function*)top_severe_coll_maxsum, true,
                  &MAXSUM_TOPSEVERECOLL);
}


/**
 *  @brief Releases all SCOUT-specific MPI reduction operators.
 *
 *  This function releases all SCOUT-specific MPI reduction operators created
 *  by InitOperators(). It must be called before MPI is finalized.
 **/
void
scout::FreeOperators()
{
    // Release MPI reduction operators
    MPI_Op_free(&MINMAX_TIMEVEC2);
    MPI_Op_free(&MAX_TOPSEVEREARRAY);
    MPI_Op_free(&MAXSUM_TOPSEVERECOLL);
}


// --- Local functions ------------------------------------------------------

/**
 *  @internal
 *  @brief Performs a combined minimum/maximum reduction.
 *
 *  This function is performs a combined minimum/maximum reduction on an
 *  array of TimeVec2 structs. For each TimeVec2 element of the arrays, the
 *  minimum of the first timestamp and the maximum of the second timestamp
 *  is calculated.
 *
 *  @param  in        First input array
 *  @param  inout     Second input array, also storing the result
 *  @param  len       Pointer to integer storing the number of array elements
 *  @param  datatype  MPI datatype handle
 **/
void
scout_minmax(TimeVec2*     in,
             TimeVec2*     inout,
             int*          len,
             MPI_Datatype* datatype)
{
    for (int i = 0; i < *len; ++i)
    {
        inout[i].value[0] = min(inout[i].value[0], in[i].value[0]);
        inout[i].value[1] = max(inout[i].value[1], in[i].value[1]);
    }
}


/**
 *  @internal
 *  @brief Performs sorting reduction.
 *
 *  This function finds most severe instances among detected,
 *  this array is kept sorted in descending order
 *
 *  @param  in     First input array
 *  @param  inout  Second input array, also storing the result
 *  @param  len    Pointer to integer storing the number of array elements
 *  @param  dtype  MPI datatype handle
 **/
void
top_severe(TopMostSevere* in,
           TopMostSevere* inout,
           int*           len,
           MPI_Datatype*  dtype)
{
    bool flag;
    int  j, k, l;
    int  offset, length;

    for (int i = 0; i < *len; i++)
    {
        // compute offset and length of the array
        // where the reduction should be applied
        offset = i * TOP_SEVERE_INSTANCES;
        length = offset + TOP_SEVERE_INSTANCES;
        for (int m = offset; m < length; m++)
        {
            // flag shows if we found the value
            // with identical cnode
            flag = false;

            // find position of element
            for (j = offset, k = -1; j < length; j++)
            {
                // the value with the same cnode value and
                // greater idletime is necessary to insert at once
                if (  (in[m].cnode == inout[j].cnode)
                   && (in[m].idletime > inout[j].idletime))
                {
                    flag = true;
                    break;
                }

                // the value with the same cnode value and
                // smaller idletime will be skipped
                else if (  (in[m].cnode == inout[j].cnode)
                        && (in[m].idletime < inout[j].idletime))
                {
                    flag = true;
                    j    = -1;
                    break;
                }

                // find the position of smallest element
                // that is is bigger than initial one
                else if (  (in[m].idletime > inout[j].idletime)
                        && (in[m].cnode != inout[j].cnode)
                        && (k < 0))
                {
                    k = j;
                }
            }

            if (flag && (j != -1))
            {
                // insert the element into correct position
                inout[j] = in[m];

                // sort the vector
                TopMostSevere tmp;
                for (int t = offset + 1; t < length; t++)
                {
                    l = t;
                    while (  (l > offset)
                          && (inout[l - 1].idletime < inout[l].idletime))
                    {
                        tmp          = inout[l];
                        inout[l]     = inout[l - 1];
                        inout[l - 1] = tmp;
                        l--;
                    }
                }
            }

            // insert the value and shift the rest values
            else if (!flag && (k >= 0))
            {
                // shift
                std::memmove(&inout[k + 1], &inout[k], (length - k - 1) * sizeof(TopMostSevere));

                // insert
                inout[k] = in[m];
            }
        }
    }
}


/**
 *  @internal
 *  @brief Finds MAX and SUM for idletime
 *
 *  This function finds MAX and SUM for idletime
 *  for detected statistical collective patterns,
 *  it finds MIN time of enter event
 *  and MAX time for exit event
 *
 *  @param  in     First input array
 *  @param  inout  Second input array, also storing the result
 *  @param  len    Pointer to integer storing the number of array elements
 *  @param  dtype  MPI datatype handle
 **/
void
top_severe_coll_maxsum(TopMostSevereMaxSum* in,
                       TopMostSevereMaxSum* inout,
                       int*                 len,
                       MPI_Datatype*        dtype)
{
    // max
    if (inout[0].idletime_max < in[0].idletime_max)
    {
        inout[0].idletime_max = in[0].idletime_max;
        inout[0].rank         = in[0].rank;
    }

    // sum
    inout[0].idletime += in[0].idletime;

    // min
    if (inout[0].entertime > in[0].entertime)
    {
        inout[0].entertime = in[0].entertime;
    }

    // max
    if (inout[0].exittime < in[0].exittime)
    {
        inout[0].exittime = in[0].exittime;
    }
}
